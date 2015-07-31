/* ************************************************************************ */
/* Georgiev Lab (c) 2015                                                    */
/* ************************************************************************ */
/* Department of Cybernetics                                                */
/* Faculty of Applied Sciences                                              */
/* University of West Bohemia in Pilsen                                     */
/* ************************************************************************ */
/* Author: Jiří Fatka <fatkaj@ntis.zcu.cz>                                  */
/* ************************************************************************ */

// C++
#include <cassert>

// Simulator
#include "simulator/Plugin.hpp"
#include "simulator/PluginApi.hpp"
#include "simulator/Simulation.hpp"
#include "simulator/SimulationListener.hpp"

// Plugin
#include "Module.hpp"

// Plugins
#include "plugins/streamlines/StoreState.hpp"

/* ************************************************************************ */

using namespace simulator;

/* ************************************************************************ */

class SteamlinesImageApi : public PluginApi
{

    UniquePtr<Module> createModule(Simulation& simulation, const String& name) noexcept override
    {
        if (name == "store-state")
            return makeUnique<plugin::streamlines::StoreState>(simulation.useModule<plugin::streamlines::Module>("streamlines-image"));

        return makeUnique<plugin::streamlines_image::Module>();
    }

};

/* ************************************************************************ */

DEFINE_PLUGIN(streamlines_image, SteamlinesImageApi)

/* ************************************************************************ */