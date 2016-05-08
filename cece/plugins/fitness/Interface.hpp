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

// CeCe config
#include "cece/config.hpp"

/* ************************************************************************ */

// C++
#include <utility>

// Declaration
#include "cece/plugins/fitness/Module.hpp"

// CeCe
#include "cece/core/DynamicArray.hpp"
#include "cece/core/String.hpp"
#include "cece/core/Real.hpp"
#include "cece/core/Tuple.hpp"
#include "cece/module/Module.hpp"
#include "cece/config/Configuration.hpp"


/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace fitness {

/* ************************************************************************ */



/**
 * @brief class for interfacing fitness Module.
 */
class IFitness
{

public:

	RealType GetAptitude(String distribution)
	{
		return Module::GetAptitude(distribution, fitness);
	}

	RealType GetFitness()
	{
		return fitness;
	}

	void SetFitness(RealType fitval)
	{
		fitness = fitval;
	}

protected:

    void SetInitialFitness(String distribution)
    {
    	fitness = Module::SetInitialFitness(distribution);
    }

// Private Data Members
private:

    RealType fitness;
};



/* ************************************************************************ */

}
}
}

/* ************************************************************************ */
