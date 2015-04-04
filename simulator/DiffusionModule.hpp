
#pragma once

/* ************************************************************************ */

// Simulator
#include "simulator/Module.hpp"
#include "simulator/Grid.hpp"

#ifdef ENABLE_RENDER
#include "render/Context.hpp"
#include "render/GridValue.hpp"
#endif

/* ************************************************************************ */

namespace simulator {

/* ************************************************************************ */

/**
 * @brief Module for diffusion.
 */
class DiffusionModule : public virtual Module
{

// Public Types
public:


    /// Grid type.
    using GridType = Grid<float>;

    /// Grid size type.
    using SizeType = GridType::SizeType;


// Public Ctors & Dtors
public:


    /**
     * @brief Constructor.
     */
    DiffusionModule();


    /**
     * @brief Destructor.
     */
    virtual ~DiffusionModule();


// Public Accessors
public:


    /**
     * @brief Returns grid.
     *
     * @return
     */
    GridType& getGrid() noexcept
    {
        return m_grid;
    }


    /**
     * @brief Returns grid.
     *
     * @return
     */
    const GridType& getGrid() const noexcept
    {
        return m_grid;
    }


// Public Mutators
public:



// Public Operations
public:


    /**
     * @brief Update module state.
     *
     * @param dt    Simulation time step.
     * @param world World object.
     */
    void update(units::Duration dt, World& world) override;


#ifdef ENABLE_RENDER
    /**
     * @brief Initialize module for rendering.
     *
     * @param context
     */
    void renderInit(render::Context& context) override;
#endif


#ifdef ENABLE_RENDER
    /**
     * @brief Render module.
     *
     * @param context
     * @param world
     */
    void render(render::Context& context, const World& world) override;
#endif


// Private Data Members
private:

    /// Velocity grid.
    GridType m_grid;

#ifdef ENABLE_RENDER

    /// Render grid for velocities
    render::GridValue m_renderObject;

#endif

};

/* ************************************************************************ */

}

/* ************************************************************************ */
