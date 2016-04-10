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

// CeCe
#include "cece/plugin/definition.hpp"
#include "cece/plugin/Api.hpp"
#include "cece/plugin/Context.hpp"

// Plugin
//#include "Yeast.hpp"
//#include "Cell.hpp"
#include "StoreMolecules.hpp"

/* ************************************************************************ */

using namespace cece;
using namespace cece::plugin::parasite;

/* ************************************************************************ */

/**
 * @brief Parasite plugin API.
 */
class ParasiteApi : public plugin::Api
{

    /**
     * @brief On plugin load.
     *
     * @param context Plugin context.
     */
    void onLoad(plugin::Context& context) override
    {
//        context.registerObject<Cell>("cell.Cell");
//        context.registerObject<Yeast>("cell.Yeast");
//        context.registerProgram<StoreMolecules>("cell.store-molecules");
    }


    /**
     * @brief On plugin unload.
     *
     * @param context Plugin context.
     */
    void onUnload(plugin::Context& context) override
    {
//        context.unregisterProgram("cell.store-molecules");
//        context.unregisterObject("cell.Cell");
//        context.unregisterObject("cell.Yeast");
    }

};

/* ************************************************************************ */

CECE_DEFINE_PLUGIN(parasite, ParasiteApi)

/* ************************************************************************ */
