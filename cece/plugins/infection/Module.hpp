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

#ifndef CECE_ENABLE_BOX2D_PHYSICS
#error Infection requires physics engine
#endif

/* ************************************************************************ */

// C++
#include <utility>

// CeCe
#include "cece/core/DynamicArray.hpp"
#include "cece/core/String.hpp"
#include "cece/core/Real.hpp"
#include "cece/core/Tuple.hpp"
#include "cece/module/Module.hpp"
#include "cece/config/Configuration.hpp"

// Plugins
#include "cece/plugins/cell/CellBase.hpp"
#include "cece/plugins/object-generator/Module.hpp"

// Physics
#include <Box2D/Box2D.h>

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace infection {

/* ************************************************************************ */



/**
 * @brief Module for infection.
 */
class Module : public module::Module, public b2ContactListener
{

// Private Structures
private:

	/**
	* @brief Structure for storing bonds.
	*/
	struct Bond
	{
		String bondRef;
		String pathogen;
		String host;
		RealType maxProdAmount;
		RealType aConst;
		RealType dConst;
	};


    /**
	 * @brief Structure for storing created object parameters.
	 */
	struct ObjectDesc
	{
		/// Object configuration
		String bondRef;
		String objectClass;
		config::Configuration config;
	};

	/**
	 * @brief User data for joint.
	 */
	struct JointUserData
	{
		const char guard = '@';
		Module* module;
		RealType Kd;
	};

	/**
	 * @brief Joint definition.
	 */
	struct JointDef
	{
		String bondRef;
		b2Body* bodyA;
		b2Body* bodyB;
		RealType dConst;
	};

// Public Ctors & Dtors
public:


    using module::Module::Module;


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


    /**
     * @brief Update module state.
     */
    void update() override;


    /**
     * @brief Called when two fixtures begin to touch.
     *
     * @param contact
     */
    void BeginContact(b2Contact* contact) override;


    /**
     * @brief Called when two fixtures cease to touch.
     *
     * @param contact
     */
    void EndContact(b2Contact* contact) override;

    /*
	 * @brief Gets the bond's Reference according to the bond's definition in simulation file
	 * @note if the names of the objects does not correspond to a bond definition "NO_DEFINED tag will be returned"
	 */
    String getBondRefFromObjects(b2Body* bodyA, b2Body* bodyB)
    {
    	auto& ca = static_cast<object::Object*>(bodyA->GetUserData())->castThrow<plugin::cell::CellBase>();
    	auto& cb = static_cast<object::Object*>(bodyB->GetUserData())->castThrow<plugin::cell::CellBase>();

    	String nameA = ca.getName();
    	String nameB = cb.getName();

    	String result = "NO_DEFINED";

    	for(auto&& bond : m_bonds)
    	{
    		if (isInfectionDefined(bond, nameA, nameB))
    		{
    			result = bond.bondRef;
    			break;
    		}
    	}

    	return result;
    }

    /*
     * @brief Defines whether the infections is possible (as it was declared in the simulation) or not
     */
    //Define si la infección se puede producir atendiendo a la configuración en simulación
    bool isInfectionDefined(Bond bondDef, String nameA, String nameB)
    {
    	//Check whether any of the objects in the contact is a phage
    	//then if the other corresponds to a infectable cell (host)
    	int result = false;

    	if(!nameA.empty()  && !nameB.empty())
    	{
    		if (nameA == bondDef.pathogen || nameB == bondDef.pathogen)
    		{
    			if (nameA == bondDef.host || nameB == bondDef.host)
    			{
    				result = true;
    			}
    		}
    	}
    	return result;
    }


// Private Data Members
private:

    /// List of bodies to join
    DynamicArray<JointDef> m_toJoin;

    //List of bodies to release
    DynamicArray<JointDef> m_toRelease;

    /// Used time step.
    units::Duration m_step;

    /// List of created bonds.
    DynamicArray<Bond> m_bonds;

    /// List of objects
    DynamicArray<ObjectDesc> m_objects;
};



/* ************************************************************************ */

}
}
}

/* ************************************************************************ */
