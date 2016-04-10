/* ************************************************************************ */
/* LIA UPM (c) 2016                                                         */
/* ************************************************************************ */
/* Department of Artifitial Intelligence                                    */
/* Faculty of Informatics                                                   */
/* Polytechnic University of Madrid - Spain                                 */
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
#include "cece/plugins/parasite/StoreMolecules.hpp"

// CeCe
#include "cece/core/DataTable.hpp"
#include "cece/object/Object.hpp"
#include "cece/simulator/Simulation.hpp"

// Plugin
#include "cece/plugins/parasite/ParasiteBase.hpp"

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace parasite {

/* ************************************************************************ */

StoreMolecules::~StoreMolecules() = default;

/* ************************************************************************ */

UniquePtr<program::Program> StoreMolecules::clone() const
{
    return makeUnique<StoreMolecules>(*this);
}

/* ************************************************************************ */

void StoreMolecules::call(simulator::Simulation& simulation, object::Object& object, units::Time dt)
{
    // Cast to parasite
    auto& parasite = object.castThrow<ParasiteBase>("Parasite object required");

    // Get data table
    auto& table = simulation.getDataTable("molecules");

    // Create new row
    // iteration;totalTime;id;molecules...
    table.addRow(
        makePair("iteration", simulation.getIteration()),
        makePair("totalTime", simulation.getTotalTime().value()),
        makePair("id", object.getId()),
        parasite.getMolecules()
    );
}

/* ************************************************************************ */

}
}
}

/* ************************************************************************ */
