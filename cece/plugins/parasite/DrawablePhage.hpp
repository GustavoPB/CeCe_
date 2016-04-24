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

// CeCe
#include "cece/core/Real.hpp"
#include "cece/core/Units.hpp"
#include "cece/render/Color.hpp"
#include "cece/render/Buffer.hpp"
#include "cece/render/Shader.hpp"
#include "cece/render/Program.hpp"

/* ************************************************************************ */

namespace cece { namespace render { class Context; } }

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace parasite {

/* ************************************************************************ */

/**
 * @brief Rendering object for Phage.
 */
class DrawablePhage
{

// Public Ctors & Dtors
public:


    /**
     * @brief Constructor.
     *
     * @param context Rendering context.
     */
    explicit DrawablePhage(render::Context& context);


// Public Operations
public:


    /**
     * @brief Render at given position with given rotation.
     *
     * @param context Render context.
     * @param size    Phage size.
     * @param budSize Bud size.
     * @param color   Phage color.
     * @param idColor Phage identification color.
     */
    void draw(render::Context& context, RealType size, RealType budSize,
        const render::Color& color, const render::Color& idColor) noexcept;


// Private Data Members
private:

    /// Buffer object.
    render::Buffer m_buffer;

    /// Vertex shader.
    render::Shader m_vertexShader;

    /// Fragment shader.
    render::Shader m_fragmentShader;

    /// Shader program.
    render::Program m_program;

    /// Phage color (GFP, YFP, RFP).
    render::Program::UniformId m_uniformColor;

    /// Border color.
    render::Program::UniformId m_uniformIdColor;

    /// If bud should be drawn.
    render::Program::UniformId m_uniformHasBud;

    /// Phage size variable.
    render::Program::UniformId m_uniformSizeMain;

    /// Bud size variable.
    render::Program::UniformId m_uniformSizeBud;
};

/* ************************************************************************ */

}
}
}

/* ************************************************************************ */
