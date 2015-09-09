/* ************************************************************************ */
/* Georgiev Lab (c) 2015                                                    */
/* ************************************************************************ */
/* Department of Cybernetics                                                */
/* Faculty of Applied Sciences                                              */
/* University of West Bohemia in Pilsen                                     */
/* ************************************************************************ */

// Declaration
#include "Yeast.hpp"

// C++
#include <cmath>
#include <random>

// Simulator
#include "core/constants.hpp"
#include "simulator/Simulation.hpp"
#include "simulator/Shape.hpp"

#if ENABLE_RENDER
#include "render/Color.hpp"
#endif

/* ************************************************************************ */

namespace plugin {
namespace cell {

/* ************************************************************************ */

static std::random_device g_rd;

/* ************************************************************************ */

Yeast::Yeast(simulator::Simulation& simulation, simulator::Object::Type type) noexcept
    : CellBase(simulation, type)
{
	setVolume(units::um3(37));
    setDensity(units::kg(1200) / units::m3(1));

    // Max 2 shapes
    auto& shapes = getMutableShapes();
    shapes.reserve(2);
    shapes.push_back(simulator::Shape::makeCircle(calcSphereRadius(getVolume())));
}

/* ************************************************************************ */

Yeast::~Yeast()
{
    // Nothing to do
}

/* ************************************************************************ */

void Yeast::update(units::Duration dt)
{
#if THREAD_SAFE
    // Lock access
    MutexGuard guard(m_mutex);
#endif

    const auto volume0 = getVolume();
    CellBase::update(dt);
    const auto volume1 = getVolume();

    // Volume change
    const auto volumeDiff = volume1 - volume0;

    if (hasBud())
    {
        m_bud->volume += volumeDiff;
        setVolume(getVolume() - volumeDiff);

        if (m_bud->volume >= getVolumeBudRelease())
        {
            budRelease();
        }
    }
    else if (getVolume() >= getVolumeBudCreate())
    {
        budCreate();
    }

#if ENABLE_PHYSICS
    // Update cell shape
    updateShape();
#endif

}

/* ************************************************************************ */

void Yeast::configure(const simulator::Configuration& config,
                      simulator::Simulation& simulation)
{
    CellBase::configure(config, simulation);

    setVolumeBudCreate(config.get("volume-bud-create", getVolumeBudCreate()));
    setVolumeBudRelease(config.get("volume-bud-release", getVolumeBudRelease()));
}

/* ************************************************************************ */

void Yeast::budCreate()
{
    assert(!hasBud());

    std::random_device rd;
    std::default_random_engine eng(g_rd());
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    m_bud = Bud{};
    m_bud->rotation = 2 * constants::PI * dist(eng);

#if ENABLE_PHYSICS
    m_shapeForceUpdate = true;
#endif
}

/* ************************************************************************ */

void Yeast::budRelease()
{
    assert(hasBud());

    // Calculate bud position
    const auto angle = getRotation();
#if ENABLE_PHYSICS
    const auto offset = PositionVector(units::Length(m_bud->shape.m_p.x), units::Length(m_bud->shape.m_p.y));
#else
    const auto offset = m_bud->offset;
#endif

    // Get current position
    const auto pos = getPosition() + offset.rotated(angle);

    // Release bud into the world
    auto bud = getSimulation().createObject<Yeast>();
    bud->setVolume(m_bud->volume);
    bud->setPosition(pos);
    bud->setVelocity(getVelocity());
    bud->setPrograms(getPrograms());
    bud->setDensity(getDensity());
    bud->setGrowthRate(getGrowthRate());

    // TODO: When yeast is rotating and bud is released it should be throw
    // away by some force.

    // Split molecules between yeast and bud

    // Total volume
    const auto totalVolume = getVolume() + m_bud->volume;

    // Copy old state
    const auto molecules = getMolecules();

    // Foreach molecules
    for (const auto& p : molecules)
    {
        // Molecules per volume unit
        const auto concentration = p.second / totalVolume;

        // Set molecule count to Yeast and bud
        setMoleculeCount(p.first, concentration * getVolume());
        bud->setMoleculeCount(p.first, concentration * bud->getVolume());
    }

    // Release bud
    m_bud.reset();

#if ENABLE_PHYSICS
    m_shapeForceUpdate = true;
#endif
}

/* ************************************************************************ */

#if ENABLE_RENDER
void Yeast::draw(render::Context& context)
{
    if (!m_renderObject)
        m_renderObject.create(context);

    PositionVector pos;
    units::Length radius;
    units::Angle angle;
    units::Length budRadius;
    render::Color color;

    {
#if THREAD_SAFE
        // Lock access
        MutexGuard guard(m_mutex);
#endif

        pos = getPosition();
        radius = calcSphereRadius(getVolume());
        angle = getRotation() - (m_bud ? m_bud->rotation : units::Angle(0));
        budRadius = m_bud ? calcSphereRadius(m_bud->volume) : Zero;
        color = calcFluorescentColor(getVolume() + m_bud->volume);
    }

    // Transform
    context.matrixPush();
    context.matrixTranslate(pos);
    context.matrixRotate(angle);
    context.matrixScale(2 * radius.value());
    context.colorPush();
    context.enableAlpha();
    m_renderObject->draw(context, 0.5f, 0.5f * (budRadius / radius), color, getIdentificationColor());
    context.disableAlpha();
    context.colorPop();
    context.matrixPop();
}
#endif

/* ************************************************************************ */

#if ENABLE_PHYSICS
void Yeast::updateShape()
{
    static constexpr auto MIN_CHANGE = units::Length(0.1f);

    // Alias for yeast shapes
    auto& shapes = getMutableShapes();

    // Calculate new radius
    const auto newRadius = calcSphereRadius(getVolume());
    const auto oldRadius = units::Length(m_shape.m_radius);
    const auto newBudRadius = m_bud ? calcSphereRadius(m_bud->volume) : Zero;
    const auto oldBudRadius = m_bud ? units::Length(m_bud->shape.m_radius) : Zero;

    const bool needs_update = m_shapeForceUpdate ||
        ((newRadius - oldRadius) > MIN_CHANGE) ||
        ((newBudRadius - oldBudRadius) > MIN_CHANGE)
    ;

    // Update main shape
    assert(shapes.size() >= 1);
    shapes[0].getCircle().radius = newRadius;

    // If bud shape is missing, create one.
    if (hasBud())
    {
        const auto center = PositionVector(Zero, 0.9 * (newRadius + newBudRadius)).rotated(-m_bud->rotation);

        if (shapes.size() != 2)
        {
            shapes.push_back(simulator::Shape::makeCircle(newBudRadius, center));
        }
        else
        {
            shapes[1].getCircle().radius = newBudRadius;
            shapes[1].getCircle().center = center;
        }
    }
    else
    {
        shapes.resize(1);
    }

    if (!needs_update)
        return;

    // Delete old fixtures
    for (b2Fixture* fixture = getBody()->GetFixtureList();
         fixture != nullptr;
         //fixture = fixture->GetNext())
         fixture = getBody()->GetFixtureList())
    {
        getBody()->DestroyFixture(fixture);
    }

    assert(getBody()->GetFixtureList() == nullptr);

    // Update main yeast shape
    {
        m_shape.m_radius = newRadius.value();
        getBody()->CreateFixture(&m_shape, getDensity().value());
    }

    // Update bud shape
    if (m_bud)
    {
        m_bud->shape.m_radius = newBudRadius.value();

        // Distance between yeast and bud
        const float distance = 0.9 * (m_shape.m_radius + m_bud->shape.m_radius);

        m_bud->shape.m_p = b2Vec2(
            distance * std::sin(m_bud->rotation),
            distance * std::cos(m_bud->rotation)
        );
        getBody()->CreateFixture(&m_bud->shape, getDensity().value());
    }

    m_shapeForceUpdate = false;
}
#endif

/* ************************************************************************ */

}
}

/* ************************************************************************ */
