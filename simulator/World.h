
#ifndef _SIMULATOR_WORLD_H_
#define _SIMULATOR_WORLD_H_

/* ************************************************************************ */

// C++
#include <vector>
#include <memory>
#include <cassert>

// Physics
#include "physics/World.h"

// Simulator
#include "simulator/Cell.h"
#include "simulator/Barrier.hpp"

#ifdef ENABLE_RENDER
// Render
#include "render/Context.h"
#endif

/* ************************************************************************ */

namespace simulator {

/* ************************************************************************ */

class Barrier;

/* ************************************************************************ */

/**
 * @brief World for cells.
 */
class World : public physics::World
{
public:


    /// Cell container type.
    using CellContainer = std::vector<std::unique_ptr<Cell>>;

    /// Barrier container type.
    using BarrierContainer = std::vector<std::unique_ptr<Barrier>>;

    /// Log callback function.
    using LogFunction = std::function<void(const std::string&)>;


// Public Ctors
public:


    /**
     * @brief Constructor.
     */
    World() noexcept;


    /**
     * @brief Virtual destructor.
     */
    virtual ~World();


// Public Accessors
public:


    /**
     * @brief Return cell container.
     *
     * @return
     */
    const CellContainer& getCells() const noexcept
    {
        return m_cells;
    }


    /**
     * @brief Return barriers container.
     *
     * @return
     */
    const BarrierContainer& getBarriers() const noexcept
    {
        return m_barriers;
    }


    /**
     * @brief Returns world width.
     *
     * @return
     */
    Length getWidth() const noexcept
    {
        return m_width;
    }


    /**
     * @brief Returns world height.
     *
     * @return
     */
    Length getHeight() const noexcept
    {
        return m_height;
    }


    /**
     * @brief Returns log callback function.
     *
     * @return
     */
    const LogFunction& getLogFunction() const noexcept
    {
        return m_logFunction;
    }


// Public Mutators
public:


    /**
     * @brief Set log callback function.
     *
     * @param fn
     */
    void setLogFunction(LogFunction fn) noexcept
    {
        m_logFunction = std::move(fn);
    }


    /**
     * @brief Resize world.
     *
     * @param width
     * @param height
     */
    void resize(Length width, Length height) noexcept
    {
        m_width = width;
        m_height = height;
    }


    /**
     * @brief Add a new cell to population.
     *
     * @param cell
     */
    template<typename T>
    T* addCell(std::unique_ptr<T> cell)
    {
        assert(cell);
        m_cells.push_back(std::move(cell));
        return static_cast<T*>(m_cells.back().get());
    }


    /**
     * @brief Create a new cell to population.
     *
     * @param cell
     *
     * @return
     */
    template<typename T, typename... Args>
    T* newCell(Args&&... args)
    {
        return addCell(std::unique_ptr<T>(new T(this, std::forward<Args>(args)...)));
    }


    /**
     * @brief Add a new barrier.
     *
     * @param barrier
     */
    Barrier* addBarrier(std::unique_ptr<Barrier> barrier)
    {
        assert(barrier);
        m_barriers.push_back(std::move(barrier));
        return m_barriers.back().get();
    }


    /**
     * @brief Create a new barrier.
     *
     * @param args...
     */
    template<typename T, typename... Args>
    T* newBarrier(Args&&... args)
    {
        return addBarrier(std::unique_ptr<T>(new T(this, std::forward<Args>(args)...)));
    }


// Public Operations
public:


    /**
     * @brief Clean world data.
     */
    virtual void clean();


    /**
     * @brief Reset world.
     */
    virtual void reset();


    /**
     * @brief Load world code.
     *
     * @param source
     */
    virtual void load(std::string source) = 0;


    /**
     * @brief Update world.
     *
     * @param step
     */
    virtual void update(float step) noexcept;

#ifdef ENABLE_RENDER

    /**
     * @brief Render world.
     *
     * @param context
     */
    virtual void draw(render::Context& context);

#endif

// Private Data Members
private:

    /// World width.
    Length m_width{400_um};

    /// World height.
    Length m_height{400_um};

    /// Cell population
    CellContainer m_cells;

    /// Barriers
    BarrierContainer m_barriers;

    /// Log function.
    LogFunction m_logFunction;

};

/* ************************************************************************ */

}

/* ************************************************************************ */

#endif // _SIMULATOR_WORLD_H_
