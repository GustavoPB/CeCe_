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

// CeCe
#include "cece/simulator/Plugin.hpp"
#include "cece/simulator/PluginApi.hpp"

// Reactions
#include "ReactionsParser.hpp"
#include "IntracellularReactions.hpp"

/************************************************************************** */

using namespace cece;
using namespace cece::simulator;

/* ************************************************************************ */

class StochasticReactionsIntracellularApi : public PluginApi
{
    Program createProgram(Simulation& simulation, const String& name, String code = {}) override
    {
        using namespace plugin::stochastic_reactions;
        return parseReactions<IntracellularReactions>(code, simulation.getParameters());
    }
};

/* ************************************************************************ */

DEFINE_PLUGIN(stochastic_reactions_intracellular, StochasticReactionsIntracellularApi)

/* ************************************************************************ */