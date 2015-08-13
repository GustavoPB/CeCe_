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

#include "IntercellularReactions.hpp"

// C++
#include <cassert>

// Plugin
#include "Diffusion.hpp"

/* ************************************************************************ */

namespace plugin {
namespace stochastic_reactions_diffusive {

/* ************************************************************************ */
/**
 * @brief Computes propensity of given reaction.
 *
 * @param index of row, cell, diffusion
 * @return propensity
 */
IntercellularReactions::PropensityType IntercellularReactions::computePropensity(
    const unsigned int index,
    const plugin::cell::CellBase& cell,
    plugin::diffusion::Module* diffusion,
    const DynamicArray<plugin::diffusion::Module::Coordinate>& coords)
{
    PropensityType local = m_rates[index];
    for (unsigned int i = 0; i < m_rules[index].size(); i++)
    {
        unsigned int number = cell.getMoleculeCount(m_ids[i])
        if (m_rules[index][i].mustnt_have && number > 0)
            return 0;
        if (m_rules[index][i].requirement != 0u)
        {
            if (m_rules[index][i].requirement > number)
                return 0;
            local *= number;
            continue;
        }
        if (m_rules[index][i].env_requirement != 0u)
        {
            // Get signal ID
            auto id = diffusion->getSignalId(m_ids[i]);
            if (id == plugin::diffusion::Module::INVALID_SIGNAL_ID)
                throw InvalidArgumentException("Unknown signal molecule '" + m_ids[i] + "' in diffusion");
            unsigned int number = getAmountOfMolecules(*diffusion, coords, id);
            if (m_rules[index][i].env_requirement > number)
                return 0;
            local *= number;
            continue;
        }
    }
    return local;
}

/* ************************************************************************ */
/**
 * @brief Computes propensities of all reactions.
 *
 * @param cell, diffusion
 * @return
 */
void IntercellularReactions::initializePropensities(
    const plugin::cell::CellBase& cell,
    plugin::diffusion::Module* diffusion,
    const DynamicArray<plugin::diffusion::Module::Coordinate>& coords)
{
    for (unsigned int i = 0; i < m_rules.size(); i++)
    {
        m_propensities.push_back(computePropensity(i, cell, diffusion, coords));
    }
}

/* ************************************************************************ */
/**
 * @brief Refreshes propensities of ractions which have requirements of specific molecule.
 *
 * @param index of column, cell, diffusion
 */
void IntercellularReactions::refreshPropensities(
    const unsigned int index,
    const plugin::cell::CellBase& cell,
    plugin::diffusion::Module* diffusion,
    const DynamicArray<plugin::diffusion::Module::Coordinate>& coords)
{
    for (unsigned int i = 0; i < m_rules.size(); i++)
    {
        if (m_rules[i][index].requirement ||
            m_rules[i][index].env_requirement)
        {
            m_propensities[i] = computePropensity(i, cell, diffusion, coords);
        }
    }
}

/* ************************************************************************ */
/**
 * @brief Executes reaction which ocuured.
 *
 * @param index of rection, cell, diffusion
 */
void IntercellularReactions::executeReaction(
    const unsigned int index,
    plugin::cell::CellBase& cell,
    plugin::diffusion::Module* diffusion,
    const DynamicArray<plugin::diffusion::Module::Coordinate>& coords)
{
    for (unsigned int i = 0; i < m_ids.size(); i++)
    {
        int change = m_rules[index][i].product - m_rules[index][i].requirement;
        if (change != 0)
        {
            cell.changeMoleculeCount(m_ids[i], change);
            refreshPropensities(i, cell, diffusion, coords);
        }
        change = m_rules[index][i].env_product - m_rules[index][i].env_requirement;
        if (change != 0)
        {
            changeMoleculesInEnvironment(m_ids[i], change, diffusion, coords);
            refreshPropensities(i, cell, diffusion, coords);
        }
    }
}

/* ************************************************************************ */
/**
 * @brief Main function of this plugin.
 *
 */
void IntercellularReactions::operator()(simulator::Object& object, simulator::Simulation& simulation, units::Duration step)
{
    auto& cell = getCell(object);
    auto diffusion = simulation.useModule<plugin::diffusion::Module>("diffusion");
    const auto& worldSize = simulation.getWorldSize();
    const auto& coords = getCoordinates(diffusion->getGridSize(), worldSize, cell);

    executeReactions(step, [this, &cell, &diffusion, &coords](unsigned int index)
    {
        executeReaction(index, cell, diffusion, coords);
    }, [this, &cell, &diffusion, &coords] () {
        initializePropensities(cell, diffusion, coords);
    });
}

/* ************************************************************************ */
/**
 * @brief Function that releases or absorbs the molecules outside the cell.
 *
 * @param id of molecule, number of molecules, diffusion
 */
void IntercellularReactions::changeMoleculesInEnvironment(
    const String& name,
    const int change,
    plugin::diffusion::Module* diffusion,
    const DynamicArray<plugin::diffusion::Module::Coordinate>& coords)
{
    assert(change != 0);

    // Get signal ID
    auto id = diffusion->getSignalId(name);

    if (id == plugin::diffusion::Module::INVALID_SIGNAL_ID)
        throw InvalidArgumentException("Unknown signal molecule '" + name + "' in diffusion");

    // Change amount of molecules
    // FIXME: #36
    changeMolecules(*diffusion, coords, id, change);
}

/* ************************************************************************ */
/**
 * @brief Function that extends reaction rule matrix with reaction that absorbs signal from the outside.
 *
 * @param array of IDs, reation rate
 */
void IntercellularReactions::extendAbsorption(const DynamicArray<String>& ids_plus, const RateType rate)
{
    DynamicArray<ReqProd> array;
    if (m_rules.size() > 0)
        array.resize(m_rules[0].size());
    for (unsigned int i = 0; i < ids_plus.size(); i++)
    {
        if (ids_plus[i] == "null")
        {
            Log::warning("NULL tag doesnt make sense here.");
            return;
        }
        unsigned int index = getIndexOfMoleculeColumn(ids_plus[i]);
        if (index == array.size())
        {
            array.push_back(ReqProd{0,1,1,0});
            continue;
        }
        array[index].env_requirement += 1;
        array[index].product += 1;
    }
    m_rates.push_back(rate);
    m_rules.push_back(array);
}

/* ************************************************************************ */
/**
 * @brief Function that extends reaction rule matrix with reaction that releases signal outside the cell.
 *
 * @param array of IDs, reation rate
 */
void IntercellularReactions::extendExpression(const DynamicArray<String>& ids_minus, const RateType rate)
{
    DynamicArray<ReqProd> array;
    if (m_rules.size() > 0)
        array.resize(m_rules[0].size());
    for (unsigned int i = 0; i < ids_minus.size(); i++)
    {
        if (ids_minus[i] == "null")
        {
            Log::warning("NULL tag doesnt make sense here.");
            return;
        }
        unsigned int index = getIndexOfMoleculeColumn(ids_minus[i]);
        if (index == array.size())
        {
            array.push_back(ReqProd{1,0,0,1});
            continue;
        }
        array[index].env_product += 1;
        array[index].requirement += 1;
    }
    m_rates.push_back(rate);
    m_rules.push_back(array);
}

/* ************************************************************************ */
/**
 * @brief Function that extends reaction rule matrix.
 *
 * @param array of molecule IDs reation rate
 */
void IntercellularReactions::extend(const DynamicArray<String>& ids_plus, const DynamicArray<String>& ids_minus, const RateType rate)
{
    if (std::find(ids_minus.begin(), ids_minus.end(), "env") != ids_minus.end())
    {
        if (ids_minus.size() == 1)
            extendAbsorption(ids_plus, rate);
        else
            Log::warning("This reaction is not valid. ENV tag must be alone.");
        return;
    }

    if (std::find(ids_plus.begin(), ids_plus.end(), "env") != ids_plus.end())
    {
        if (ids_plus.size() == 1)
            extendExpression(ids_minus, rate);
        else
            Log::warning("This reaction is not valid. ENV tag must be alone.");
        return;
    }
    extendIntracellular(ids_plus, ids_minus, rate);
}

/* ************************************************************************ */

}
}

/* ************************************************************************ */
