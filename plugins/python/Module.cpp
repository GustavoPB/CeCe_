/* ************************************************************************ */
/* Georgiev Lab (c) 2015                                                    */
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

// This must be first
#include "Python.hpp"

// Declaration
#include "Module.hpp"

// C++
#include <cassert>
#include <string>

// Simulation
#include "core/Log.hpp"
#include "core/FilePath.hpp"
#include "simulator/Simulation.hpp"
#include "simulator/Configuration.hpp"
#include "simulator/Plugin.hpp"
#include "simulator/PluginManager.hpp"

// Module
#include "Exception.hpp"
#include "Utils.hpp"

/* ************************************************************************ */

namespace plugin {
namespace python {

/* ************************************************************************ */

Module::Module(const std::string& name)
{
    auto ends_with = [](const std::string& value, const std::string& ending) {
        if (ending.size() > value.size()) return false;
        return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
    };

    // External file
    if (ends_with(name, ".py"))
    {
        std::string foundPath;

        // Foreach possible paths
        for (const auto& p : simulator::PluginManager::getDirectories())
        {
            auto path = p + "/" + name;

            if (fileExists(path))
            {
                foundPath = path;
                break;
            }
        }

        if (foundPath.empty())
        {
            Log::warning("Unable to find: " + name);
        }
        else
        {
            m_source.initFile(name);
        }
    }
}

/* ************************************************************************ */

void Module::configure(const simulator::Configuration& config, simulator::Simulation& simulation)
{
    // Check if configuration contains code
    if (config.hasContent())
    {
        if (m_source.isInitialized())
            Log::warning("Overriding external script by internal code");

        // Initialize from source text
        m_source.initSource(config.getContent());
    }

    // Get function pointers
    m_configureFn = m_source.getFunction("configure");
    m_updateFn = m_source.getFunction("update");

#if ENABLE_RENDER
    m_drawFn = m_source.getFunction("draw");
#endif

    if (!m_configureFn)
        return;

    // Call configure
    if (!call(m_configureFn, &config))
        throw Exception();
}

/* ************************************************************************ */

void Module::update(units::Duration dt, simulator::Simulation& simulation)
{
    if (!m_updateFn)
        return;

    // Call update
    if (!call(m_updateFn, dt.value(), &simulation))
        throw Exception();
}

/* ************************************************************************ */

#if ENABLE_RENDER
void Module::draw(render::Context& context, const simulator::Simulation& simulation)
{
    if (!m_drawFn)
        return;

    // Call draw
    if (!call(m_drawFn, &context, &simulation))
        throw Exception();
}
#endif

/* ************************************************************************ */

}
}

/* ************************************************************************ */
