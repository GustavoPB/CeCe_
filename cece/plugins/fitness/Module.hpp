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

//#ifndef CECE_ENABLE_BOX2D_PHYSICS
//#error fitness requires physics engine
//#endif

/* ************************************************************************ */

// C++
#include <utility>

// CeCe
#include "cece/core/DynamicArray.hpp"
#include "cece/core/String.hpp"
#include "cece/core/Real.hpp"
#include "cece/core/Tuple.hpp"
#include "cece/config/Configuration.hpp"
#include "cece/module/Module.hpp"

// Plugins
//#include "cece/plugins/cell/CellBase.hpp"
//#include "cece/plugins/parasite/ParasiteBase.hpp"
//#include "cece/plugins/object-generator/Module.hpp"


/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace fitness {

/* ************************************************************************ */



/**
 * @brief Module for fitness.
 */
class Module : public module::Module
{
// Public Ctors & Dtors
public:


    using module::Module::Module;

// Private Structures
private:

	struct Distribution //De momento solo contemplamos distribuciones lineales tanto para fitness como aptitud
	{
		String distRef;
		RealType k;
		RealType d;
	};



// Public Operations
public:

    /**
    * @brief Load module configuration.
    *
    * @param config Source configuration.
    */
    void loadConfig(const config::Configuration& config) override;


    /**
    * @brief Store module configuration.
    *
    * @param config Output configuration.
    */
    void storeConfig(config::Configuration& config) const override;

    static RealType GetInitialFitness(String distribution);

    static RealType GetAptitude(String distribution, RealType fitness);

// Private Data Members
private:

 DynamicArray<Distribution> m_distributions;

};



/* ************************************************************************ */

}
}
}

/* ************************************************************************ */