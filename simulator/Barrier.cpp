
/* ************************************************************************ */

// Declaration
#include "simulator/Barrier.hpp"

// Core
#include "simulator/World.h"

/* ************************************************************************ */

namespace simulator {

/* ************************************************************************ */

Barrier::Barrier(World* world)
    : physics::Barrier(world)
    , m_world(world)
{
    // Nothing to do
}

/* ************************************************************************ */

Barrier::~Barrier()
{
    // Nothing to do
}

/* ************************************************************************ */

}

/* ************************************************************************ */