/* ************************************************************************ */
/* Georgiev Lab (c) 2015                                                    */
/* ************************************************************************ */
/* Department of Cybernetics                                                */
/* Faculty of Applied Sciences                                              */
/* University of West Bohemia in Pilsen                                     */
/* ************************************************************************ */
/* Author: Jiří Fatka <fatkaj@ntis.zcu.cz>                                  */
/* ************************************************************************ */

// Declaration
#include "simulator/Simulation.hpp"

// C++
#include <algorithm>
#include <chrono>
#include <fstream>

// Simulator
#include "core/Real.hpp"
#include "core/Log.hpp"
#include "core/Tuple.hpp"
#include "core/Exception.hpp"
#include "core/OutStream.hpp"
#include "simulator/Simulator.hpp"
#include "simulator/Plugin.hpp"
#include "simulator/PluginApi.hpp"
#include "simulator/PluginManager.hpp"
#include "simulator/Obstacle.hpp"

/* ************************************************************************ */

namespace simulator {

/* ************************************************************************ */

namespace {

/* ************************************************************************ */

Tuple<String, String> splitModulePath(const String& path) noexcept
{
    auto pos = path.find(':');

    // Using wrapper
    if (pos != String::npos)
    {
        // Create wrapper version
        return std::make_tuple(path.substr(0, pos), path.substr(pos + 1));
    }
    else
    {
        // Find dot separator
        auto pos = path.find('.');

        if (pos == String::npos)
            return std::make_tuple(path, String{});
        else
            return std::make_tuple(path.substr(0, pos), path.substr(pos + 1));
    }
}

/* ************************************************************************ */

/**
 * @brief Write CSV line into output stream.
 *
 * @param os
 * @param container
 */
template<typename Container>
void writeCsvLine(OutStream& os, const Container& container)
{
    using std::begin;
    using std::end;

    for (auto it = begin(container); it != end(container); ++it)
    {
        if (it != begin(container))
            os << ';';

        os << *it;
    }

    os << "\n";
}

/* ************************************************************************ */

}

/* ************************************************************************ */

/**
 * @brief Read object type from stream.
 *
 * @param is   Input stream.
 * @param type
 *
 * @return is.
 */
InStream& operator>>(InStream& is, Object::Type& type)
{
    String value;
    is >> value;

    if (value == "static")
        type = Object::Type::Static;
    else if (value == "pinned")
        type = Object::Type::Pinned;
    else
        type = Object::Type::Dynamic; // Default

    return is;
}

/* ************************************************************************ */

Simulation::Simulation() noexcept
#if ENABLE_PHYSICS
    : m_world{b2Vec2{0.0f, 0.0f}}
#endif
{
    // Nothing to do
}

/* ************************************************************************ */

Simulation::~Simulation()
{
    // Store data tables
    storeDataTables();

    // Call finalize simulations for all plugins
    for (auto it = m_plugins.rbegin(); it != m_plugins.rend(); ++it)
    {
        assert(it->second);
        it->second->finalizeSimulation(*this);
    }
}

/* ************************************************************************ */

Module* Simulation::useModule(const String& path, String storePath)
{
    if (storePath.empty())
        storePath = path;

    // Module exists, return the existing one
    if (hasModule(storePath))
        return getModule(storePath);

    Log::debug("Loading library: ", path);

    // Split path into parts
    String library, name;
    std::tie(library, name) = splitModulePath(path);

    // Get API
    PluginApi* api = requirePlugin(library);

    // Load only library
    if (name.empty())
        Log::debug("Create module '", library, "'");
    else
        Log::debug("Create module '", library, ".", name, "'");

    // Create module with given name
    auto module = api->createModule(*this, name);

    // Register module
    if (module)
    {
        if (storePath != path)
        {
            Log::info("Using module '", path, "' as '", storePath, "'");
        }
        else
        {
            Log::info("Using module '", path, "'");
        }
        return addModule(storePath, std::move(module));
    }

    Log::warning("Unable to create module '", path, "' (unsupported by library?)");

    return nullptr;
}

/* ************************************************************************ */

ViewPtr<Object> Simulation::buildObject(const String& path, Object::Type type)
{
    // Split path into parts
    String library, name;
    std::tie(library, name) = splitModulePath(path);

    if (name.empty())
        throw InvalidArgumentException("Missing object type name");

    // Get API
    PluginApi* api = requirePlugin(library);

    Log::debug("Create object '", library, ".", name, "'");

    // Create object with given name
    auto object = api->createObject(*this, name, type);

    // Register module
    if (object)
        return addObject(std::move(object));

    Log::warning("Unable to create object: ", path, " (unsupported by library?)");

    return nullptr;
}

/* ************************************************************************ */

Program Simulation::buildProgram(const String& path)
{
    // Split path into parts
    String library, type;
    std::tie(library, type) = splitModulePath(path);

    if (type.empty())
        throw InvalidArgumentException("Missing program type");

    // Get API
    PluginApi* api = requirePlugin(library);

    Log::debug("Create program '", library, ".", type, "'");

    // Create object with given name
    return api->createProgram(*this, type);
}

/* ************************************************************************ */

void Simulation::reset()
{
    m_iteration = 0;
    m_totalTime = Zero;
    m_initialized = false;
}

/* ************************************************************************ */

bool Simulation::update(units::Duration dt)
{
    // Initialize simulation
    if (!isInitialized())
        initialize();

    // Increase step number
    m_iteration++;
    m_totalTime += dt;

    // Clear all stored forces
    for (auto& obj : getObjects())
        obj->setForce(Zero);

    // Update modules
    {
        auto _ = measure_time("sim.modules", TimeMeasurementIterationOutput(this));

        for (auto& module : getModules())
            module.second->update(dt, *this);
    }

    {
        auto _ = measure_time("sim.objects", TimeMeasurementIterationOutput(this));

        // Update simulations objects
        //for (auto& obj : getObjects())
        for (size_t i = 0; i < m_objects.size(); ++i)
        {
            auto& obj = m_objects[i];
            assert(obj);
            obj->update(dt);
        }
    }

    // Remove objects that are outside world.
    {
        auto _ = measure_time("sim.delete", TimeMeasurementIterationOutput(this));

        // Delete queued objects
        for (auto obj : m_objectsToDelete)
        {
            m_objects.erase(std::remove_if(m_objects.begin(), m_objects.end(), [obj](const ObjectContainer::value_type& rhs) {
                return obj.get() == rhs.get();
            }), m_objects.end());
        }

        const auto hh = getWorldSize() * 0.5f;

        // Kill objects that are outside world
        m_objects.erase(std::remove_if(m_objects.begin(), m_objects.end(), [&hh](const ObjectContainer::value_type& obj) {
            // Ignore static objects
            if (obj->getType() == Object::Type::Static)
                return false;

            // Get object position
            const auto& pos = obj->getPosition();

            // TODO: optimize
            return !(
                ((pos.getX() >= -hh.getX()) && (pos.getX() <= hh.getX())) &&
                ((pos.getY() >= -hh.getY()) && (pos.getY() <= hh.getY()))
            );
        }), m_objects.end());
    }

#ifdef ENABLE_PHYSICS
    {
        auto _ = measure_time("sim.physics", TimeMeasurementIterationOutput(this));

        m_world.Step(dt.value(), 5, 5);
    }
#endif

    return (hasUnlimitedIterations() || getIteration() <= getIterations());
}

/* ************************************************************************ */

void Simulation::initialize()
{
    assert(!isInitialized());

    for (auto& init : m_initializers)
        init(*this);

    m_initialized = true;
}

/* ************************************************************************ */

void Simulation::configure(const Configuration& config)
{
    // Resize world
    {
        auto size = config.get<SizeVector>("world-size");

        if (size.getWidth() == Zero || size.getHeight() == Zero)
            throw simulator::ConfigException("Width or height is zero!");

        setWorldSize(size);
    }

    // Time step
    {
        String dtStr = config.get("dt");

        // Real-time time step
        if (dtStr.empty() || dtStr == "auto")
        {
            setTimeStep(Zero);
        }
        else
        {
            // Parse time step
            setTimeStep(config.get<units::Time>("dt"));
        }
    }

    // Number of iterations
    setIterations(config.get("iterations", getIterations()));

    // Background color
    setBackgroundColor(config.get("background", getBackgroundColor()));

    // Parse parameters
    for (auto&& parameterConfig : config.getConfigurations("parameter"))
    {
        setParameter(parameterConfig.get("name"), units::parse(parameterConfig.get("value")));
    }

    // Parse init
    for (auto&& initConfig : config.getConfigurations("init"))
    {
        // Plugin is required
        auto api = requirePlugin(initConfig.get("language"));

        // Register program
        addInitializer(api->createInitializer(*this, initConfig.getContent()));
    }

    // Parse plugins
    for (auto&& pluginConfig : config.getConfigurations("plugin"))
    {
        // Returns valid pointer or throws an exception
        requirePlugin(pluginConfig.get("name"))->configure(*this, pluginConfig);
    }

    // Parse modules
    for (auto&& moduleConfig : config.getConfigurations("module"))
    {
        // Create module by given name
        auto module = useModule(
            moduleConfig.get("name"),
            moduleConfig.get("access-name", String{})
        );

        // Configure module
        if (module)
            module->configure(moduleConfig, *this);
    }

    // Parse programs
    for (auto&& programConfig : config.getConfigurations("program"))
    {
        // Plugin is required
        auto api = requirePlugin(programConfig.get("language"));

        // Program name
        const auto name = programConfig.get("name");

        // Register program
        addProgram(name, api->createProgram(*this, name, programConfig.getContent()));
    }

    // Parse objects
    for (auto&& objectConfig : config.getConfigurations("object"))
    {
        // Create object
        auto object = buildObject(
            objectConfig.get("class"),
            objectConfig.get("type", Object::Type::Dynamic)
        );

        if (object)
            object->configure(objectConfig, *this);
    }

    // Parse obstacles
    for (auto&& obstacleConfig : config.getConfigurations("obstacle"))
    {
        // Create object
        auto object = createObject<simulator::Obstacle>();

        if (object)
            object->configure(obstacleConfig, *this);
    }
}

/* ************************************************************************ */

bool Simulation::update()
{
    if (isTimeStepRealTime())
    {
        using clock_type = std::chrono::high_resolution_clock;
        using duration_type = std::chrono::duration<float, std::chrono::seconds::period>;

        // Last update duration
        static clock_type::duration diff{1};

        // Get start time
        auto start = clock_type::now();

        static_assert(std::chrono::treat_as_floating_point<duration_type::rep>::value, "ehm...");
        bool res = update(units::Time(std::chrono::duration_cast<duration_type>(diff).count()));

        // Calculate time that takes to update simulation (then use it in next step)
        diff = clock_type::now() - start;

        return res;
    }
    else
    {
        return update(getTimeStep());
    }
}

/* ************************************************************************ */

#ifdef ENABLE_RENDER
void Simulation::draw(render::Context& context)
{
    context.setStencilBuffer(getWorldSize().getWidth().value(), getWorldSize().getHeight().value());

    // Store modules
    DynamicArray<ViewPtr<Module>> modules;
    for (auto& module : getModules())
        modules.push_back(module.second);

    // Sort modules by rendering order
    std::sort(modules.begin(), modules.end(), [](const ViewPtr<Module>& lhs, const ViewPtr<Module>& rhs) {
        return lhs->getZOrder() < rhs->getZOrder();
    });

    // Render modules
    for (auto& module : modules)
        module->draw(context, *this);

    // Draw objects
    for (const auto& obj : getObjects())
    {
        assert(obj);
        if (obj->isVisible())
            obj->draw(context);
    }

#if ENABLE_RENDER && ENABLE_PHYSICS && ENABLE_PHYSICS_DEBUG
    if (isDrawPhysics())
        m_world.DrawDebugData();
#endif

}
#endif

/* ************************************************************************ */

ViewPtr<PluginApi> Simulation::requirePlugin(const String& name)
{
    // Load plugin
    auto api = loadPlugin(name);

    if (api)
        return api;

    throw InvalidArgumentException("Plugin '" + name + "' not found");
}

/* ************************************************************************ */

ViewPtr<PluginApi> Simulation::loadPlugin(const String& name) noexcept
{
    try
    {
        // Test if plugin is used
        auto it = m_plugins.find(name);

        // Found
        if (it != m_plugins.end())
            return it->second;

        // Load plugin
        ViewPtr<PluginApi> api = PluginManager::load(name);

        if (!api)
            return nullptr;

        // Register API
        m_plugins.emplace(name, api);

        // Plugin loaded
        invoke(&SimulationListener::onPluginLoad, *this, name);

        // Init simulation
        api->initSimulation(*this);

        return api;
    }
    catch (const Exception& e)
    {
        Log::warning(e.what());
    }

    return nullptr;
}

/* ************************************************************************ */

void Simulation::storeDataTables()
{
    for (const auto& item : m_dataTables)
    {
        const auto& table = item.second;

        std::ofstream file(item.first + ".csv");

        Log::info("Saving data table '", item.first, "' into '", item.first, ".csv'");

        // Write headers
        writeCsvLine(file, table.getColumns());

        for (const auto& row : table.getRows())
            writeCsvLine(file, row);

        Log::info("Data table '", item.first, "' saved.");
    }
}

/* ************************************************************************ */

}

/* ************************************************************************ */
