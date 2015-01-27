
/* ************************************************************************ */

// Declaration
#include "physics/World.h"

// C++
#include <algorithm>

// Bullet
#include <btBulletDynamicsCommon.h>

// Library
#include "physics/Ground.h"

/* ************************************************************************ */

namespace physics {

/* ************************************************************************ */

World::World() noexcept
    : m_broadphase(new btDbvtBroadphase())
    , m_collisionConfiguration(new btDefaultCollisionConfiguration())
    , m_dispatcher(new btCollisionDispatcher(m_collisionConfiguration.get()))
    , m_solver(new btSequentialImpulseConstraintSolver)
    , m_dynamicsWorld(new btDiscreteDynamicsWorld(m_dispatcher.get(), m_broadphase.get(), m_solver.get(), m_collisionConfiguration.get()))
{
    m_dynamicsWorld->setGravity(btVector3(0, -10, 0));

    m_ground.reset(new Ground(this));
}

/* ************************************************************************ */

World::~World() noexcept
{
    // Nothing to do
}

/* ************************************************************************ */

std::array<float, 3> World::getGravity() const noexcept
{
    auto grav = m_dynamicsWorld->getGravity();

    return {grav.x(), grav.y(), grav.z()};
}

/* ************************************************************************ */

void World::update(float step) noexcept
{
    m_dynamicsWorld->stepSimulation(step, 5);
}

/* ************************************************************************ */

}

/* ************************************************************************ */