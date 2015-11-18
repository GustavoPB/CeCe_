/* ************************************************************************ */
/* Georgiev Lab (c) 2015                                                    */
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
#include "cece/plugins/diffusion/StoreState.hpp"

// CeCe
#include "cece/core/TimeMeasurement.hpp"
#include "cece/core/Range.hpp"
#include "cece/core/VectorRange.hpp"
#include "cece/simulator/Simulation.hpp"

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace diffusion {

/* ************************************************************************ */

void StoreState::update(simulator::Simulation& simulation, units::Time dt)
{
    auto _ = measure_time("diffusion.store-state", simulator::TimeMeasurementIterationOutput(simulation));

    // Get data table
    auto& table = simulation.getDataTable("diffusion");

    // Foreach coordinates
    for (auto&& c : range(m_diffusionModule->getGridSize()))
    {
        // Create new row
        auto row = table.addRow(
            makePair("iteration", simulation.getIteration()),
            makePair("totalTime", simulation.getTotalTime().value()),
            makePair("x", c.getX()),
            makePair("y", c.getY())
        );

        // Foreach signals
        for (auto signalId : m_diffusionModule->getSignalIds())
        {
            table.setData(row,
                makePair(
                    m_diffusionModule->getSignalName(signalId),
                    m_diffusionModule->getSignal(signalId, c).value()
                )
            );
        }
    }
}

/* ************************************************************************ */

}
}
}

/* ************************************************************************ */