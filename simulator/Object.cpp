/* ************************************************************************ */
/* Department of Cybernetics                                                */
/* Faculty of Applied Sciences                                              */
/* University of West Bohemia in Pilsen                                     */
/* ************************************************************************ */
/* Author: Jiří Fatka <fatkaj@ntis.zcu.cz>                                  */
/* ************************************************************************ */

// Declaration
#include "simulator/Object.hpp"

// C++
#include <cassert>
#include <string>
#include <sstream>

// Simulator
#include "core/Units.hpp"
#include "core/Log.hpp"
#include "parser/Parser.hpp"
#include "simulator/Simulation.hpp"

/* ************************************************************************ */

namespace simulator {

/* ************************************************************************ */

namespace {

/* ************************************************************************ */

#if ENABLE_PHYSICS
/**
 * @brief Convert object type into Box2 body type.
 *
 * @param type
 *
 * @return
 */
b2BodyType convert(Object::Type type) NOEXCEPT
{
    switch (type)
    {
    default:                    return b2_staticBody;
    case Object::Type::Static:  return b2_staticBody;
    case Object::Type::Dynamic: return b2_dynamicBody;
    }
}
#endif

/* ************************************************************************ */

#if ENABLE_PHYSICS
/**
 * @brief Convert Box2 body type into object type.
 *
 * @param type
 *
 * @return
 */
Object::Type convert(b2BodyType type) NOEXCEPT
{
    switch (type)
    {
    default:                return Object::Type::Static;
    case b2_staticBody:     return Object::Type::Static;
    case b2_dynamicBody:    return Object::Type::Dynamic;
    }
}
#endif


/* ************************************************************************ */

/**
 * @brief Split string into multiple strings separated by separator.
 *
 * @param value
 * @param separator
 *
 * @return
 */
DynamicArray<String> split(String value, char separator) NOEXCEPT
{
    DynamicArray<String> elems;
    std::istringstream ss(std::move(value));
    String item;

    while (std::getline(ss, item, separator))
    {
        elems.push_back(item);
    }

    return elems;
}

/* ************************************************************************ */

}

/* ************************************************************************ */

Object::IdType s_id = 0;

/* ************************************************************************ */

Object::Object(Simulation& simulation, Type type) NOEXCEPT
    : m_simulation(simulation)
    , m_id(++s_id)
#if !ENABLE_PHYSICS
    , m_type(type)
#endif
{
#if ENABLE_PHYSICS
    b2BodyDef bodyDef;
    bodyDef.type = convert(type);

    // Create body
    m_body = getSimulation().getWorld().CreateBody(&bodyDef);
#endif
}

/* ************************************************************************ */

Object::~Object()
{
#if ENABLE_PHYSICS
    assert(m_body);
    getSimulation().getWorld().DestroyBody(m_body);
#endif
}

/* ************************************************************************ */

Object::Type Object::getType() const NOEXCEPT
{
#if ENABLE_PHYSICS
    assert(m_body);
    return convert(m_body->GetType());
#else
    return m_type;
#endif
}

/* ************************************************************************ */

PositionVector Object::getPosition() const NOEXCEPT
{
#if ENABLE_PHYSICS
    assert(m_body);
    auto pos = m_body->GetPosition();
    return {units::Length(pos.x), units::Length(pos.y)};
#else
    return m_position;
#endif
}

/* ************************************************************************ */

units::Angle Object::getRotation() const NOEXCEPT
{
#if ENABLE_PHYSICS
    assert(m_body);
    return m_body->GetAngle();
#else
    return m_rotation;
#endif
}

/* ************************************************************************ */

VelocityVector Object::getVelocity() const NOEXCEPT
{
#if ENABLE_PHYSICS
    assert(m_body);
    auto vel = m_body->GetLinearVelocity();
    return {units::Velocity(vel.x), units::Velocity(vel.y)};
#else
    return m_velocity;
#endif
}

/* ************************************************************************ */

void Object::setType(Type type) NOEXCEPT
{
#if ENABLE_PHYSICS
    assert(m_body);
    m_body->SetType(convert(type));
#else
    m_type = type;
#endif
}

/* ************************************************************************ */

void Object::setPosition(PositionVector pos) NOEXCEPT
{
#if ENABLE_PHYSICS
    assert(m_body);
    m_body->SetTransform({pos.getX().value(), pos.getY().value()}, m_body->GetAngle());
#else
    m_position = std::move(pos);
#endif
}

/* ************************************************************************ */

void Object::setRotation(units::Angle angle) NOEXCEPT
{
#if ENABLE_PHYSICS
    assert(m_body);
    m_body->SetTransform(m_body->GetPosition(), angle);
#else
    m_rotation = angle;
#endif
}

/* ************************************************************************ */

void Object::setVelocity(VelocityVector vel) NOEXCEPT
{
#if ENABLE_PHYSICS
    assert(m_body);
    m_body->SetLinearVelocity({vel.getX().value(), vel.getY().value()});
#else
    m_velocity = std::move(vel);
#endif
}

/* ************************************************************************ */

void Object::applyForce(const ForceVector& force) NOEXCEPT
{
#if ENABLE_PHYSICS
    assert(m_body);
    m_body->ApplyForceToCenter({force.getX().value(), force.getY().value()}, true);
#else
    // NOTE: This is a little bit weird
    m_velocity = force;
#endif
}

/* ************************************************************************ */

void Object::applyForce(const ForceVector& force, const PositionVector& pos) NOEXCEPT
{
#if ENABLE_PHYSICS
    assert(m_body);
    m_body->ApplyForce({force.getX().value(), force.getY().value()}, {pos.getX().value(), pos.getY().value()}, true);
#else
    m_velocity = force;
#endif
}

/* ************************************************************************ */

void Object::update(units::Duration dt)
{
#if !ENABLE_PHYSICS
    // Calculate new object position
    setPosition(getPosition() + getVelocity() * dt);
#endif

    // Call object programs
    for (auto& program : getPrograms())
        program(*this, getSimulation(), dt);
}

/* ************************************************************************ */

void Object::configure(const Configuration& config, Simulation& simulation)
{
    // Set object position
    config.callIfSetString("position", [this](const String& value) {
        setPosition(parser::parse_vector<units::Length>(value));
    });

    // Set object density
    config.callIfSetString("density", [this](const String& value) {
        setDensity(parser::parse_value<units::Density>(value));
    });

    // Set object programs
    config.callIfSetString("programs", [this, &simulation](const String& value) {
        for (const auto& name : split(value, ' '))
        {
            auto program = simulation.getProgram(name);

            if (program)
                addProgram(std::move(program));
            else
                Log::warning("Unable to create program: ", name);
        }
    });
}

/* ************************************************************************ */

}

/* ************************************************************************ */
