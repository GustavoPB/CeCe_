/* ************************************************************************ */
/* Department of Cybernetics                                                */
/* Faculty of Applied Sciences                                              */
/* University of West Bohemia in Pilsen                                     */
/* ************************************************************************ */
/* Author: Jiří Fatka <fatkaj@ntis.zcu.cz>                                  */
/* ************************************************************************ */

#pragma once

/* ************************************************************************ */

// Simulator
#include "core/compatibility.hpp"
#include "core/Vector.hpp"
#include "render/Buffer.hpp"
#include "render/GridBase.hpp"

/* ************************************************************************ */

namespace render {

/* ************************************************************************ */

class Context;

/* ************************************************************************ */

/**
 * @brief Rendering grid object.
 */
class DLL_EXPORT Grid : public GridBase
{


// Public Ctors & Dtors
public:


    /**
     * @brief Constructor.
     *
     * @param context Rendering context.
     */
    explicit Grid(Context& context);


// Public Operators
public:


    /**
     * @brief Render at current position.
     *
     * @param context Rendering context.
     */
    void draw(Context& context) NOEXCEPT;


    /**
     * @brief Resize grid.
     *
     * @param size New grid size.
     */
    void resize(Size size) NOEXCEPT;


// Private Data Members
private:

    /// Buffer object.
    Buffer m_buffer;

};

/* ************************************************************************ */

}

/* ************************************************************************ */
