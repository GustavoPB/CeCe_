
#pragma once

/* ************************************************************************ */

// Simulator
#include "core/Units.hpp"
#include "core/Grid.hpp"
#include "simulator/Module.hpp"
#include "simulator/Object.hpp"

#if ENABLE_RENDER
#include "render/Context.hpp"
#include "render/GridVector.hpp"
#include "render/Object.hpp"
#endif

/* ************************************************************************ */

namespace module {
namespace streamlines {

/* ************************************************************************ */

/**
 * @brief Module for streamlines.
 */
class Module : public simulator::Module
{

// Public Types
public:


    /// Grid type.
    using GridType = core::Grid<VelocityVector>;

    /// Grid size type.
    using SizeType = GridType::SizeType;


// Public Ctors & Dtors
public:


    /**
     * @brief Constructor.
     */
    Module()
        : Module(300)
    {
        // Nothing to do
    }


    /**
     * @brief Constructor.
     *
     * @param size Signal grid size.
     */
    template<typename... Args>
    Module(Args&&... args)
        : m_grid(std::forward<Args>(args)...)
    {
        // Nothing to do
    }


    /**
     * @brief Destructor.
     */
    virtual ~Module();


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


    /**
     * @brief Returns flow speed.
     *
     * @return
     */
    float getFlowSpeed() const noexcept
    {
        return m_flowSpeed;
    }


// Public Mutators
public:


    /**
     * @brief Mark state to be updated.
     */
    void markUpdate() noexcept
    {
        m_update = true;
    }


    /**
     * @brief Set flow speed.
     *
     * @param speed.
     */
    void setFlowSpeed(float speed) noexcept
    {
        m_flowSpeed = speed;
    }


// Public Operations
public:


    /**
     * @brief Update module state.
     *
     * @param dt         Simulation time step.
     * @param simulation
     */
    void update(units::Duration dt, simulator::Simulation& simulation) override;


    /**
     * @brief Configure module.
     *
     * @param config
     */
    void configure(const simulator::ConfigurationBase& config) override;


#if ENABLE_RENDER
    /**
     * @brief Render module.
     *
     * @param context
     * @param simulation
     */
    void draw(render::Context& context, const simulator::Simulation& simulation) override;
#endif


// Private Data Members
private:

    /// Flow speed.
    float m_flowSpeed = 50.f;

    /// Velocity grid.
    GridType m_grid;

    /// If streams should be updated
    bool m_update = true;

#if ENABLE_RENDER
    /// Render grid for velocities
    render::ObjectPtr<render::GridVector> m_renderObject;
#endif

#if ENABLE_RENDER
    bool m_drawObject = false;
#endif

#if ENABLE_RENDER
    /// If render object should be updated
    bool m_renderUpdate = false;
#endif
};

/* ************************************************************************ */

}
}

/* ************************************************************************ */
