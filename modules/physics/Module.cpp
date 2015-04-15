
// Declaration
#include "Module.hpp"

// Simulator
#include "simulator/Simulation.hpp"
#include "simulator/DynamicObject.hpp"

/* ************************************************************************ */

namespace module {
namespace physics {

/* ************************************************************************ */

Module::Module()
{
    // Nothing to do
}

/* ************************************************************************ */

Module::~Module()
{
    // Nothing to do
}

/* ************************************************************************ */

void Module::update(units::Duration dt, simulator::Simulation& simulation)
{
    for (auto& obj1 : simulation.getObjects())
    {
        for (auto& obj2 : simulation.getObjects())
        {
            if (obj1 == obj2)
                continue;

            if (!obj1->hasFlag(simulator::OBJECT_DYNAMIC) ||
                !obj2->hasFlag(simulator::OBJECT_DYNAMIC))
                continue;

            // Cast
            auto ptr1 = obj1->cast<simulator::DynamicObject>();
            auto ptr2 = obj2->cast<simulator::DynamicObject>();

            // Position difference vector
            auto diff = ptr1->getPosition() - ptr2->getPosition();

            // Distance
            auto length = diff.getLength();

            // TODO: implement
        }
    }
}

/* ************************************************************************ */

}
}

/* ************************************************************************ */
