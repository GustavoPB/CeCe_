/* ************************************************************************ */
/* Georgiev Lab (c) 2015-2016                                               */
/* ************************************************************************ */
/* Department of Cybernetics                                                */
/* Faculty of Applied Sciences                                              */
/* University of West Bohemia in Pilsen                                     */
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

#ifndef CECE_ENABLE_RENDER
#error Background plugin requires enabled rendering.
#endif

/* ************************************************************************ */

// CeCe
#include "cece/core/String.hpp"
#include "cece/module/Module.hpp"
#include "cece/render/Object.hpp"
#include "cece/render/GridColor.hpp"

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace background {

/* ************************************************************************ */

/**
 * @brief Module for rendering background image.
 */
class Module : public module::Module
{

// Public Ctors & Dtors
public:


    using module::Module::Module;


// Public Accessors
public:


    /**
     * @brief Returns background image name.
     *
     * @return
     */
    const String& getImageName() const noexcept
    {
        return m_imageName;
    }


// Public Mutators
public:


    /**
     * @brief Set background image name.
     *
     * @param name
     */
    void setImageName(String name) noexcept
    {
        m_imageName = std::move(name);
    }


// Public Operations
public:


    /**
     * @brief Load module configuration.
     *
     * @param config Source configuration.
     */
    void loadConfig(const config::Configuration& config) override;


    /**
     * @brief Store module configuration.
     *
     * @param config Output configuration.
     */
    void storeConfig(config::Configuration& config) const override;


    /**
     * @brief Initialize module.
     */
    void init() override;


    /**
     * @brief Render module.
     *
     * @param context Rendering context.
     */
    void draw(render::Context& context) override;


// Private Data Members
private:

    /// Image name.
    String m_imageName;

    /// Image data.
    DynamicArray<unsigned char> m_data;

    /// Image size.
    Vector<unsigned int> m_size = Zero;

    // Number of channels.
    int m_channels;

    /// Drawable object.
    render::ObjectPtr<render::GridColor> m_drawable;

};

/* ************************************************************************ */

}
}
}

/* ************************************************************************ */
