/* ************************************************************************ */
/* Georgiev Lab (c) 2015                                                    */
/* ************************************************************************ */
/* Department of Cybernetics                                                */
/* Faculty of Applied Sciences                                              */
/* University of West Bohemia in Pilsen                                     */
/* ************************************************************************ */
/* Author: Jiří Fatka <fatkaj@ntis.zcu.cz>                                  */
/* Author: Václav Pelíšek <pelisekv@students.zcu.cz>                        */
/* ************************************************************************ */

// Simulator
#include "simulator/Plugin.hpp"
#include "simulator/PluginApi.hpp"

// Reactions
#include "ReactionParserDiffusive.hpp"

/************************************************************************** */

using namespace simulator;

/* ************************************************************************ */

class StochasticReactionsDiffusiveApi : public PluginApi
{
    Program createProgram(Simulation& simulation, const String& name, String code = {}) override
    {
        return ReactionParserDiffusive(code).parse();
    }
};

/* ************************************************************************ */

DEFINE_PLUGIN(stochastic_reactions_diffusive, StochasticReactionsDiffusiveApi)

/* ************************************************************************ */
