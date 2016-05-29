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
#include "cece/plugins/infection/Module.hpp"

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
namespace infection {

/* ************************************************************************ */

namespace {

/* ************************************************************************ */

std::random_device g_rd;

/* ************************************************************************ */

std::default_random_engine g_gen(g_rd());

/* ************************************************************************ */

/*TOREMOVE
 * RealType getRelativeReceptorProportion(
    RealType radius,
    unsigned int numberOfRec)
{
    return 1 - std::exp(- numberOfRec / (4 * core::constants::PI * radius * radius));
}
*/
/* ************************************************************************ */

RealType getAssociationPropensity(
    units::Duration step,
    RealType Ka)
{
	return (1 - std::exp(-Ka * step.value()));
}


/* ************************************************************************ */

RealType getDisassociationPropensity(
    units::Duration step,
    RealType Kd)
{
    return 1 - std::exp(-Kd * step.value());
}

/* ************************************************************************ */

}

/* ************************************************************************ */

void Module::update()
{
    // Store time step
    m_step = getSimulation().getTimeStep();

    auto _ = measure_time("infection", simulator::TimeMeasurement(getSimulation()));

    // Get physics world
    auto& world = getSimulation().getWorld();

    // Foreach pending bodies
    for (const auto& p : m_toJoin)
    {
        b2WeldJointDef joint;
        joint.Initialize(p.bodyA, p.bodyB, p.bodyA->GetWorldCenter());
        JointUserData* jUserData = new JointUserData();
        jUserData->module = this;
        jUserData->Kd = p.dConst;
        joint.userData = jUserData;
        world.CreateJoint(&joint);
    }

    // flush jointArray
    m_toJoin.clear();

    // Joints to remove
    DynamicArray<b2Joint*> toRemove;

    // Foreach active joints
    for (auto joint = world.GetJointList(); joint != nullptr; joint = joint->GetNext())
    {
        const JointUserData* jUserData = reinterpret_cast<const JointUserData*>(joint->GetUserData());
        // Not our joint
        if (jUserData == nullptr)
        {
        	continue;
        }

        if (jUserData->guard != '@')
        {
			continue;
		}

        std::bernoulli_distribution dist(
            getDisassociationPropensity(
                m_step,
                jUserData->Kd
            )
        );

        if (dist(g_gen))
        {
            Log::debug("Released: ", joint->GetBodyA(), ", ", joint->GetBodyB());
            //GPuig -- Stores the bodies whose contact is released
            String bondRef = getBondRefFromObjects(joint->GetBodyA(), joint->GetBodyB());
            m_toRelease.push_back(JointDef{bondRef, joint->GetBodyA(), joint->GetBodyB(), 0});
            //GPuig
            toRemove.push_back(joint);
            delete jUserData;
        }
    }

    // Destroy joints
    for (auto joint : toRemove)
        world.DestroyJoint(joint);
}

/* ************************************************************************ */

void Module::loadConfig(const config::Configuration& config)
{
    // Configure parent
    module::Module::loadConfig(config);

    getSimulation().getWorld().SetContactListener(this);

    for (auto&& c_bond : config.getConfigurations("bond"))
    {
        m_bonds.push_back(Bond{
        	c_bond.get("bond-reference"),
        	c_bond.get("pathogen"),
        	c_bond.get("host"),
			c_bond.get<RealType>("max-production-amount"),
            //c_bond.get<RealType>("association-constant"),
            c_bond.get<RealType>("disassociation-constant")
        });
    }

    //PoC - Store object types
    for (auto&& cfg : config.getConfigurations("production"))
        {
    		/*ObjectDesc desc;
    		desc.config = cfg.toMemory();*/
    		m_objects.push_back(ObjectDesc{
    			cfg.get("bond-reference"),
    			cfg.get("class"),
    			cfg.toMemory()});
        }
}

/* ************************************************************************ */

void Module::storeConfig(config::Configuration& config) const
{
    module::Module::storeConfig(config);

    // Foreach bonds
    for (const auto& bond : m_bonds)
    {
        auto bondConfig = config.addConfiguration("bond");
        bondConfig.set("bond-reference", bond.pathogen);
        bondConfig.set("pathogen", bond.pathogen);
        bondConfig.set("host", bond.host);
        bondConfig.set("max-production-amount", bond.pathogen);
        //bondConfig.set("association-constant", bond.aConst);
        bondConfig.set("disassociation-constant", bond.dConst);
    }
}

/* ************************************************************************ */

void Module::BeginContact(b2Contact* contact)
{
    auto ba = contact->GetFixtureA()->GetBody();
    auto bb = contact->GetFixtureB()->GetBody();
    auto oa = static_cast<object::Object*>(ba->GetUserData());
    auto ob = static_cast<object::Object*>(bb->GetUserData());

    //TOREVIEW: Need for distinct types (!= xor doesnt work ???)
    if (oa->is<plugin::cell::CellBase>() == ob->is<plugin::cell::CellBase>() ||
		oa->is<plugin::parasite::ParasiteBase>() == ob->is<plugin::parasite::ParasiteBase>())
            return;

    plugin::cell::CellBase* ca = NULL;
    plugin::parasite::ParasiteBase* cb = NULL;
    plugin::parasite::Phage* cbb = NULL; //Needed to call GetAptitude from phage
    if(oa->is<plugin::cell::CellBase>())
    {
    	ca = dynamic_cast<plugin::cell::CellBase*>(oa);
 	}
    else
    {
    	cb = dynamic_cast<plugin::parasite::ParasiteBase*>(oa);
    	cbb = dynamic_cast<plugin::parasite::Phage*>(oa); //Needed to call GetAptitude from phage
    }
  	if(ob->is<plugin::cell::CellBase>())
    {
    	ca = dynamic_cast<plugin::cell::CellBase*>(ob);
    }
    else
    {
    	cb = dynamic_cast<plugin::parasite::ParasiteBase*>(ob);
    	cbb = dynamic_cast<plugin::parasite::Phage*>(ob); //Needed to call GetAptitude from phage
    }

    for (unsigned int i = 0; i < m_bonds.size(); i++)
    {
    	//GPuig
    	if (isInfectionDefined(m_bonds[i], ca->getName(), cb->getName()))
    	{
    		//Nota: es necesario que el fago se introdujera siempre en la primera posicion de m_toJoin
        std::bernoulli_distribution dist1(getAssociationPropensity(m_step, /*m_bonds[i].aConst*/cbb->GetAptitude()));
        if (dist1(g_gen))
        {
            Log::debug("Joined: ", ba, ", ", bb);
            m_toJoin.push_back(JointDef{m_bonds[i].bondRef, ba, bb, m_bonds[i].dConst});
            continue;
        }
    	}
    }
}

/* ************************************************************************ */

void Module::EndContact(b2Contact* contact)
{

	for(auto releasedjoin : m_toRelease)
	{
		//GPuig
		auto& simulation = getSimulation();

		auto oa = static_cast<object::Object*>(releasedjoin.bodyA->GetUserData());
		auto ob = static_cast<object::Object*>(releasedjoin.bodyB->GetUserData());

		plugin::cell::CellBase* ca = NULL;
		plugin::parasite::Phage* cb = NULL;
		if(oa->is<plugin::cell::CellBase>())
		{
			ca = dynamic_cast<plugin::cell::CellBase*>(oa);
		}
		else
		{
			cb = dynamic_cast<plugin::parasite::Phage*>(oa);
		}
		if(ob->is<plugin::cell::CellBase>())
		{
			ca = dynamic_cast<plugin::cell::CellBase*>(ob);
		}
		else
		{
			cb = dynamic_cast<plugin::parasite::Phage*>(ob);
		}

		for(auto&& bond : m_bonds)
		{
			if (bond.bondRef == releasedjoin.bondRef)
			{
				for(auto&& obj : m_objects)
				{
					if (bond.bondRef == obj.bondRef)
					{
						auto phageAmount = cb->GetNextOffspringAmount(bond.maxProdAmount);

						for (unsigned int i = 0; i < phageAmount; i++)
						{
							auto object = simulation.buildObject(obj.objectClass);

							object->configure(obj.config, simulation);

							PositionVector pos;
							pos = ca->getPosition();//ca is always the host

							object->setPosition(pos);

							//fitness offspring transmission
							auto newPhage = dynamic_cast<plugin::parasite::Phage*>(object);
							newPhage->SetFitness(cb->GetFitness());
						}
					}
				}
			}
		}
	}
	m_toRelease.clear();
	//GPuig
}

/* ************************************************************************ */
}
}
}

/* ************************************************************************ */
