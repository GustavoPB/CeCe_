/* ************************************************************************ */
/* Georgiev Lab (c) 2016                                                    */
/* ************************************************************************ */
/* Department of Cybernetics                                                */
/* Faculty of Applied Sciences                                              */
/* University of West Bohemia in Pilsen                                     */
/* ************************************************************************ */
/*                                                                          */
/* This file is part of CeCe.                                               */
/*                                                                          */
/* CeCe is free software: you can redistribute it and/or modify             */
/* it under the terms of the GNU General Public License as published by     */
/* the Free Software Foundation, either version 3 of the License, or        */
/* (at your option) any later version.                                      */
/*                                                                          */
/* CeCe is distributed in the hope that it will be useful,                  */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/* GNU General Public License for more details.                             */
/*                                                                          */
/* You should have received a copy of the GNU General Public License        */
/* along with CeCe.  If not, see <http://www.gnu.org/licenses/>.            */
/*                                                                          */
/* ************************************************************************ */

// Declaration
#include "cece/plugins/streamlines-channel/Module.hpp"

// CeCe
#include "cece/core/Assert.hpp"
#include "cece/simulator/Simulation.hpp"

// Plugin
#include "cece/plugins/streamlines-channel/Utils.hpp"
#include "cece/plugins/streamlines-channel/BgkDynamics.hpp"
#include "cece/plugins/streamlines-channel/ZouHeDynamics.hpp"

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace streamlines_channel {

/* ************************************************************************ */

void Module::init(simulator::Simulation& simulation)
{
    // Initialize model
    Utils::initModel(convertLength(getHeight()));

    // Initialize streamlines
    streamlines::Module::init(simulation);
}

/* ************************************************************************ */

void Module::loadConfig(simulator::Simulation& simulation, const config::Configuration& config)
{
    // Configure parent
    streamlines::Module::loadConfig(simulation, config);

    // Channel height
    setHeight(config.get("height", getHeight()));
}

/* ************************************************************************ */

UniquePtr<streamlines::Dynamics> Module::createFluidDynamics() const
{
    return makeUnique<BgkDynamics>(calculateOmega());
}

/* ************************************************************************ */

UniquePtr<streamlines::Dynamics> Module::createBorderDynamics(LayoutPosition pos) const
{
    const auto omega = calculateOmega();

    switch (pos)
    {
    case LayoutPosTop:
        return makeUnique<ZouHeDynamics>(omega, ZouHeDynamics::Position::Top);

    case LayoutPosBottom:
        return makeUnique<ZouHeDynamics>(omega, ZouHeDynamics::Position::Bottom);

    case LayoutPosLeft:
        return makeUnique<ZouHeDynamics>(omega, ZouHeDynamics::Position::Left);

    case LayoutPosRight:
        return makeUnique<ZouHeDynamics>(omega, ZouHeDynamics::Position::Right);

    default:
        Assert(false && "No way!");
        break;
    }

    return nullptr;
}

/* ************************************************************************ */

void Module::printInfo(const simulator::Simulation& simulation)
{
    streamlines::Module::printInfo(simulation);

    Log::info("[streamlines] Channel height: ", getHeight(), " um");
}

/* ************************************************************************ */

}
}
}

/* ************************************************************************ */
