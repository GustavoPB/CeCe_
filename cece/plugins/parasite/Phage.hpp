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

#ifdef CECE_ENABLE_RENDER
#  include "cece/render/Context.hpp"
#  include "cece/render/Object.hpp"
#  include "cece/plugins/parasite/DrawablePhage.hpp"
#endif

#ifdef CECE_THREAD_SAFE
#  include "cece/core/Mutex.hpp"
#endif

// Plugin
#include "cece/plugins/parasite/ParasiteBase.hpp"
#include "cece/plugins/fitness/Interface.hpp"

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace parasite {

/* ************************************************************************ */

/**
 * @brief Phage representation.
 */
class Phage : public ParasiteBase, public cece::plugin::fitness::IFitness
{

// Public Ctors & Dtors
public:


    /**
     * @brief Constructor.
     *
     * @param simulation
     * @param type       Parasite type.
     */
    explicit Phage(simulator::Simulation& simulation, String typeName = "parasite.Phage",
                   object::Object::Type type = object::Object::Type::Dynamic) noexcept;


    /**
     * @brief Destructor.
     */
    ~Phage();


// Public Accessors
public:


    /**
     * @brief Returns if phage has bud.
     *
     * @return
     */
    bool hasBud() const noexcept
    {
        return m_hasBud;
    }


    /**
     * @brief Return bud volume.
     *
     * @return
     */
    units::Volume getVolumeBud() const noexcept
    {
        if (hasBud())
            return m_bud.volume;
        else
            return Zero;
    }


    /**
     * @brief Return bud current angle.
     *
     * @return
     */
    units::Angle getAngleBud() const noexcept
    {
        if (hasBud())
            return m_bud.angle;
        else
            return Zero;
    }


    /**
     * @brief Return volume when buds are produced.
     *
     * @return
     */
    units::Volume getVolumeBudCreate() const noexcept
    {
        return m_volumeBudCreate;
    }


    /**
     * @brief Return volume (bud) when buds are released.
     *
     * @return
     */
    units::Volume getVolumeBudRelease() const noexcept
    {
        return m_volumeBudRelease;
    }

    /**
    * @brief Return if fitness is enabled for phage class.
    *
    * @return
    */
    //String getFitnessInitDistribution() const noexcept
    //{
    //	return fitness_init_ref;
    //}

    /**
	* @brief Return if fitness is enabled for phage class.
	*
	* @return
	*/
    //String getFitnessAptitudeDistribution() const noexcept
	//{
	//	return fitness_aptitude_ref;
	//}


// Public Accessors
public:


    /**
     * @brief Set bud volume.
     *
     * @param volume
     */
    void setVolumeBud(units::Volume volume) noexcept
    {
        if (!hasBud() && volume != Zero)
            budCreate();

        m_bud.volume = std::move(volume);
    }


    /**
     * @brief Set bud angle.
     *
     * @param angle
     */
    void setAngleBud(units::Angle angle) noexcept
    {
        if (!hasBud() && angle != Zero)
            budCreate();

        m_bud.angle = std::move(angle);
    }


    /**
     * @brief Set volume when buds are produced.
     *
     * @param volume
     */
    void setVolumeBudCreate(units::Volume volume) noexcept
    {
        m_volumeBudCreate = std::move(volume);
    }


    /**
     * @brief Set volume (bud) when buds are released.
     *
     * @param volume
     */
    void setVolumeBudRelease(units::Volume volume) noexcept
    {
        m_volumeBudRelease = std::move(volume);
    }

    /**
    * @brief Set value to enable or disable fitness.
    *
    * @param String
    */
    //void setFitnessInitDistribution(String ref) noexcept
    //{
    //	fitness_init_ref = ref;
    //}

    /**
    * @brief Set value to enable or disable fitness.
    *
	* @param String
	*/
	//void setFitnessAptitudeDistribution(String ref) noexcept
	//{
	//	fitness_aptitude_ref = ref;
	//}



// Public Operations
public:


    /**
     * @brief Update phage state.
     *
     * @param dt Time step.
     */
    void update(units::Time dt) override;


    /**
     * @brief Configure object.
     *
     * @param config
     * @param simulation
     */
    void configure(const config::Configuration& config, simulator::Simulation& simulation) override;


    /**
     * @brief Create new phage bud.
     */
    void budCreate();


    /**
     * @brief Release bud phage.
     */
    void budRelease();


#ifdef CECE_ENABLE_RENDER

    /**
     * @brief Render phage.
     *
     * @param context
     */
    virtual void draw(render::Context& context) override;

#endif


// Protected Operations
protected:


    /**
     * @brief Update phage physics shape.
     */
    void updateShape();


// Private Data Members
private:


    /// If phage has a bud.
    bool m_hasBud = false;

    /// Reference to the init fitness math expression
    //String fitness_init_ref; //TODELETE

    /// Reference to the aptitude fitness math expression
    //String fitness_aptitude_ref;//TODELETE


    /**
     * @brief phage bud.
     */
    struct
    {
        /// Angle.
        units::Angle angle = Zero;

        /// Bud volume.
        units::Volume volume = Zero;

        /// Last shape update radius.
        units::Length lastRadius = Zero;
    } m_bud;

    /// Volume that is needed to bud creation.
    units::Volume m_volumeBudCreate = units::um3(12);

    /// Volume (bud) that is needed to bud release.
    units::Volume m_volumeBudRelease = units::um3(10);

#ifdef CECE_ENABLE_RENDER
    /// Render object for Phage.
    /// Shared between all instances, it's same for all instances.
    render::ObjectSharedPtr<DrawablePhage> m_renderObject;
#endif

    /// Last shape update radius.
    units::Length m_lastRadius = Zero;

    /// If shape must be updated.
    bool m_shapeForceUpdate = false;

#ifdef CECE_THREAD_SAFE
    /// Access mutex.
    mutable Mutex m_mutex;
#endif

};

/* ************************************************************************ */

}
}
}

/* ************************************************************************ */