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
#include "cece/plugins/fitness/Module.hpp"

// C++
#include <random>
#include <algorithm>
#include <list>

// CeCe
#include "cece/core/Log.hpp"
#include "cece/core/constants.hpp"
#include "cece/core/TimeMeasurement.hpp"
#include "cece/simulator/TimeMeasurement.hpp"
#include "cece/simulator/Simulation.hpp"

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace fitness {

/* ************************************************************************ */

namespace {

/* ************************************************************************ */

//std::random_device g_rd;

/* ************************************************************************ */

//std::default_random_engine g_gen(g_rd());
}

	void Module::loadConfig(const config::Configuration& config)
	{
		 // Configure parent
		 module::Module::loadConfig(config);

		 for (auto&& c_dist : config.getConfigurations("distribution"))
		 {
			 m_distributions.push_back(Distribution{
		     c_dist.get("distribution-reference"),
		     c_dist.get<RealType>("expression-k"),
			 c_dist.get<RealType>("expression-d")
			 	 });
		  }
	}

	void Module::storeConfig(config::Configuration& config) const
	{
		module::Module::storeConfig(config);

		// Foreach bonds
		for (const auto& distro : m_distributions)
		{
			auto distroConfig = config.addConfiguration("distribution");
		    distroConfig.set("distribution-reference", distro.distRef);
		    distroConfig.set("expression-k", distro.k);
		    distroConfig.set("expression-d", distro.d);
		}
	}

	RealType Module::GetInitialFitness(String distribution)
	{
		RealType result;
		srand(time(NULL));

		/*for (auto& distro : m_distributions)
		{
			if(distro.distRef == distribution)
			{
				auto randomNumber = rand() % 100;
				Log::debug(randomNumber);
				result = distro.k * static_cast<RealType>(randomNumber) + distro.d;
			}
		}*/
		return result;
	}

	RealType Module::GetAptitude(String distribution, RealType fitness)
	{
		RealType result;
		/*for (auto& distro : m_distributions)
		{
			if(distro.distRef == distribution)
			{
				result = distro.k * fitness + distro.d;
			}
		}*/
		return result;
	}

/* ************************************************************************ */
}
}
}

/* ************************************************************************ */