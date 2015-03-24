
#pragma once

/* ************************************************************************ */

// Physics
#include "render/Position.hpp"

/* ************************************************************************ */

namespace render {

/* ************************************************************************ */

/**
 * @brief Render camera.
 */
class Camera
{

// Public Ctors & Dtors
public:


    /**
     * @brief Constructor.
     */
    Camera() = default;


// Public Accessors
public:


    /**
     * @brief Get camera position.
     *
     * @return
     */
    const Position& getPosition() const noexcept
    {
        return m_position;
    }


    /**
     * @brief Returns camera zoom.
     *
     * @return
     */
    float getZoom() const noexcept
    {
        return m_zoom;
    }


// Public Mutators
public:


    /**
     * @brief Change camera position.
     *
     * @param pos
     */
    void setPosition(const Position& pos) noexcept
    {
        m_position = pos;
    }


    /**
     * @brief Set camera zoom.
     *
     * @param zoom
     */
    void setZoom(float zoom) noexcept
    {
        m_zoom = zoom;
    }


// Private Data Members
private:

    /// Camera position.
    Position m_position{0, 0};

    /// Zoom value.
    float m_zoom = 1.0;

};

/* ************************************************************************ */

}

/* ************************************************************************ */
