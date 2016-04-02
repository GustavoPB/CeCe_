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
    /*RealType radius1,
    RealType radius2,
    unsigned int numberOfRec1,
    unsigned int numberOfRec2,*/
    RealType Ka)
{
	//Llamada a la función de evaluzación
    /*TO REMOVE
     *
     return  getRelativeReceptorProportion(radius1, numberOfRec1) *
            getRelativeReceptorProportion(radius2, numberOfRec2) *
            (1 - std::exp(-Ka * step.value()));*/
	return (1 - std::exp(-Ka * step.value()));
}

/*
 * @brief Defines whether the infections is possible (as it was declared in the simulation) or not
 */
//Define si la infección se puede producir atendiendo a la configuración en simulación
bool isInfectionPosible(Module::Bond bondDef, String nameA, String nameB)
{
	//Check whether any of the objects in the contact is a phage
	//then if the other corresponds to a infectable cell (host)
	int result = 0;

	if(!nameA.empty()  && !nameB.empty())
	{
		if (nameA == bondDef.pathogen || nameB == bondDef.pathogen)
		{
			if (nameA == bondDef.host || nameB == bondDef.host)
			{
				result = 1;
			}
		}
	}
	return result;
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

    m_toJoin.clear();

    // Joints to remove
    DynamicArray<b2Joint*> toRemove;

    // Foreach active joints
    for (auto joint = world.GetJointList(); joint != nullptr; joint = joint->GetNext())
    {
        const JointUserData* jUserData = reinterpret_cast<const JointUserData*>(joint->GetUserData());
        // Not our joint
        if (jUserData == nullptr)
            continue;
        if (jUserData->guard != '@')
            continue;

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
            m_toRelease.push_back(JointDef{joint->GetBodyA(), joint->GetBodyB(), 0});
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
        	c_bond.get("pathogen"),
        	c_bond.get("host"),
            c_bond.get<RealType>("association-constant"),
            c_bond.get<RealType>("disassociation-constant")
        });
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
        bondConfig.set("pathogen", bond.pathogen);
        bondConfig.set("host", bond.host);
        bondConfig.set("association-constant", bond.aConst);
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
    if (!oa->is<plugin::cell::CellBase>() || !ob->is<plugin::cell::CellBase>())
        return;
    auto& ca = static_cast<object::Object*>(ba->GetUserData())->castThrow<plugin::cell::CellBase>();
    auto& cb = static_cast<object::Object*>(bb->GetUserData())->castThrow<plugin::cell::CellBase>();
    //auto radius1 = ca.getShapes()[0].getCircle().radius; //TOREMOVE
    //auto radius2 = cb.getShapes()[0].getCircle().radius; //TOREMOVE

    /*EVOLUTIVO:
     * Sustituir getMoleculeCount por una función que evalue el nombre:
     * - Introducir GetName en CellBase.hpp -- Hecho!
     * - Crear aqui funcion para evaluar si coinciden los nombres definidos en la simulación:
     * 		Hay que cambiar la logica para que solo se detecte el virus como celula
     * 		en realidad lo que tengo que dectar es que alguno de los dos elementos que intervienen en el
     * 		contacto sean de tipo virus y el otro celula.
     * 		O que los dos sean de tipo celula y evaluar la adecuación en downstream con una función posterior (no es muy buena
     * 		solución porque para que hacerlo en dos etapas...pero la primera es sencilla?)
     * 		==> crear una funcion de probabilidad que asocie una nula cuando no son del tipo adecuado. No solo esto sino que tb
     * 		tiene que evaluar si los nobres coinciden  para poder instanciar el enlace. Puedo hacerlo en dos etapas?
     * 		primero evaluar los tipos
     * 		y luego ya los nombres dentro de la funcion de probabilidad??
     */

    for (unsigned int i = 0; i < m_bonds.size(); i++)
    {
    	//GPuig
    	if (isInfectionPosible(m_bonds[i], ca.getName(), cb.getName()))
    	{
    		//Nota: es necesario que el fago se introdujera siempre en la primera posicion de m_toJoin
        std::bernoulli_distribution dist1(getAssociationPropensity(m_step, m_bonds[i].aConst));
        if (dist1(g_gen))
        {
            Log::debug("Joined: ", ba, ", ", bb);
            m_toJoin.push_back(JointDef{ba, bb, m_bonds[i].dConst});
            continue;
        }
    	}
        /*std::bernoulli_distribution dist2(
            getAssociationPropensity(m_step, radius1.value(), radius2.value(),
                cb.getMoleculeCount(m_bonds[i].receptor), ca.getMoleculeCount(m_bonds[i].ligand),
                m_bonds[i].aConst));
        if (dist2(g_gen))
        {
            Log::debug("Joined: ", ba, ", ", bb);
            m_toJoin.push_back(JointDef{ba, bb, m_bonds[i].dConst});
            continue;
        }*/
    }
}

/* ************************************************************************ */

void Module::EndContact(b2Contact* contact) //No seria necesario argumento
{
	for(auto releasedjoin : m_toRelease)
	{
		//GPuig
		auto& simulation = getSimulation();
		//Transformo a CellBase para capturar su nombre
		auto& ca = static_cast<object::Object*>(releasedjoin.bodyA->GetUserData())->castThrow<plugin::cell::CellBase>();
		auto& cb = static_cast<object::Object*>(releasedjoin.bodyB->GetUserData())->castThrow<plugin::cell::CellBase>();

		//Consulto configuraciones para determinar el tipo

		// Create object
		auto object = simulation.buildObject("cell.Yeast");


		//object->configure(desc.config, simulation);
		//object->setPosition(pos);
		//GPuig
	}
	m_toRelease.clear();
}

/* ************************************************************************ */
}
}
}

/* ************************************************************************ */
