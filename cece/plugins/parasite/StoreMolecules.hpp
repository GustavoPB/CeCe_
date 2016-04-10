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

#pragma once

/* ************************************************************************ */

// CeCe
#include "cece/core/Units.hpp"
#include "cece/program/Program.hpp"

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace parasite {

/* ************************************************************************ */

/**
 * @brief Program for storing parasite molecules amount.
 */
class StoreMolecules : public program::Program
{

// Public Ctors & Dtors
public:


    /**
     * @brief Destructor.
     */
    ~StoreMolecules();


// Public Operations
public:


    /**
     * @brief Clone program.
     *
     * @return
     */
    UniquePtr<program::Program> clone() const override;


    /**
     * @brief Call program for given object.
     *
     * @param simulation Simulation object.
     * @param object     Object.
     * @param dt         Simulation time step.
     */
    void call(simulator::Simulation& simulation, object::Object& object, units::Time dt) override;

};

/* ************************************************************************ */

}
}
}

/* ************************************************************************ */
