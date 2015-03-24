
#pragma once

/* ************************************************************************ */

// C++
#include <cmath>

// Simulator
#include "core/Units.hpp"
#include "simulator/DynamicObject.hpp"

#ifdef ENABLE_RENDER
// Render
#include "render/Context.hpp"
#endif

/* ************************************************************************ */

namespace simulator {

/* ************************************************************************ */

/**
 * @brief Type for string number of fluorescent proteins.
 */
using FluorescentProteinCount = unsigned int;

/* ************************************************************************ */

/**
 * @brief Cell representation.
 */
class Cell : public DynamicObject
{

// Public Accessors
public:


    /**
     * @brief Get cell volume.
     *
     * @return
     */
    Volume getVolume() const noexcept
    {
        return m_volume;
    }


    /**
     * @brief Returns a number of GFP proteins.
     *
     * @return
     */
    FluorescentProteinCount getGfp() const noexcept
    {
        return m_gfp;
    }


    /**
     * @brief Returns a number of RFP proteins.
     *
     * @return
     */
    FluorescentProteinCount getRfp() const noexcept
    {
        return m_rfp;
    }


    /**
     * @brief Returns a number of YFP proteins.
     *
     * @return
     */
    FluorescentProteinCount getYfp() const noexcept
    {
        return m_yfp;
    }


// Public Mutators
public:


    /**
     * @brief Set cell volume.
     *
     * @param volume
     */
    void setVolume(Volume volume) noexcept
    {
        m_volume = volume;
    }


    /**
     * @brief Set a number of GFP proteins in the cell.
     *
     * @param gfp
     */
    void setGfp(FluorescentProteinCount gfp) noexcept
    {
        m_gfp = gfp;
    }


    /**
     * @brief Set a number of RFP proteins in the cell.
     *
     * @param rfp
     */
    void setRfp(FluorescentProteinCount rfp) noexcept
    {
        m_rfp = rfp;
    }


    /**
     * @brief Set a number of YFP proteins in the cell.
     *
     * @param yfp
     */
    void setYfp(FluorescentProteinCount yfp) noexcept
    {
        m_yfp = yfp;
    }


// Public Operations
public:


    /**
     * @brief Update cell state.
     *
     * @param dt Time step.
     */
    void update(Duration dt) override;

#ifdef ENABLE_RENDER

    /**
     * @brief Render cell.
     *
     * @param context
     */
    virtual void render(render::Context& context);

#endif


    /**
     * @brief Calculate radius for sphere shapes - from cell volume.
     *
     * @param volume Cell volume.
     *
     * @return Radius.
     */
    static Length calcSphereRadius(Volume volume) noexcept
    {
        // 3th root of ((3 / 4 * pi) * volume)
        return Length(0.62035f * std::pow(volume, 0.3333333f));
    }


// Private Data Members
private:

    /// Cell volume.
    Volume m_volume = 1_um;

    /// Number of GFP proteins.
    FluorescentProteinCount m_gfp = 0;

    /// Number of RFP proteins.
    FluorescentProteinCount m_rfp = 0;

    /// Number of YFP proteins.
    FluorescentProteinCount m_yfp = 0;

};

/* ************************************************************************ */

}

/* ************************************************************************ */

