/* ************************************************************************ */
/* Georgiev Lab (c) 2015                                                    */
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
#include "core/Macro.hpp"

/* ************************************************************************ */

// Check major version
#ifndef VERSION_MAJOR
#error Missing VERSION_MAJOR macro.
#endif

// Check minor version
#ifndef VERSION_MINOR
#error Missing VERSION_MINOR macro.
#endif

// Check patch version
#ifndef VERSION_PATCH
#error Missing VERSION_PATCH macro.
#endif

/* ************************************************************************ */

/**
 * @brief Simulator special version.
 */
#ifndef VERSION_SPECIAL
#define VERSION_SPECIAL
#endif

/* ************************************************************************ */

/**
 * @brief Simulator version macro.
 */
#define VERSION \
    XSTR(VERSION_MAJOR) "." \
    XSTR(VERSION_MINOR) "." \
    XSTR(VERSION_PATCH) \
    VERSION_SPECIAL

/* ************************************************************************ */
