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


	const String INIT_DISTRO_TYPE = "init";

	const String APTITUDE_DISTRO_TYPE = "aptitude";

	// Private Structures
private:

		struct Distribution //De momento solo contemplamos distribuciones lineales tanto para fitness como aptitud
		{
			//String distClass;
			RealType k;
			RealType d;
		};

public:

	RealType GetAptitude()
	{
		RealType aptitude = 0.0;
		aptitude = aptitudeDistro.k * fitness + aptitudeDistro.d;
		return aptitude;
	}

	RealType GetFitness()
	{
		return fitness;
	}

	void SetFitness(RealType fitval)
	{
		fitness = fitval;
	}

	int GetNextOffspringAmount(int maxProdAmount)
	{
		int result;
		auto maxDistroRange = aptitudeDistro.k * 100 + aptitudeDistro.d;
		auto intervalRange = maxDistroRange / (maxProdAmount+1); //sin +1 no se generaría nunca el máximo de fagos
		result = (int)(this->GetAptitude() / intervalRange);
		return result;
	}

protected:

    void SetInitialFitness()
    {
    	this->fitness = 0.0;//default fitness value

		auto randomNumber = rand() % 100;
		//Log::debug(randomNumber);
		auto maxFitnessRange = initDistro.k * 100 + initDistro.d;
		auto unnormalizedFitness = initDistro.k * static_cast<RealType>(randomNumber) + initDistro.d;
		this->fitness = (unnormalizedFitness/maxFitnessRange)*100;
    }

    /**
     * @brief class for storing configuration data
     * Just to class are allowed: init or aptitude
     */
    void setFitnessDistribution(String c, RealType k, RealType d)
    {
    	if (c == INIT_DISTRO_TYPE)
		{
    		initDistro.k = k;
    		initDistro.d = d;
		}
    	if(c == APTITUDE_DISTRO_TYPE)
    	{
			aptitudeDistro.k = k;
			aptitudeDistro.d = d;
		}
    }

    bool IsFitnessEnabled()
    {
    	return true;//initDistro != NULL &&  aptitudeDistro != NULL;//TOREVIEW
    }

// Private Data Members
private:

    //DynamicArray<Distribution> m_distributions;
    Distribution initDistro;

    Distribution aptitudeDistro;

    RealType fitness;
};



/* ************************************************************************ */

}
}
}

/* ************************************************************************ */
