/* ************************************************************************ */
/* Georgiev Lab (c) 2015                                                    */
/* ************************************************************************ */
/* Department of Cybernetics                                                */
/* Faculty of Applied Sciences                                              */
/* University of West Bohemia in Pilsen                                     */
/* ************************************************************************ */

// Declaration
#include "core/Vector.hpp"

/* ************************************************************************ */

#ifndef _MSC_VER
inline namespace core {
#endif

/* ************************************************************************ */

#ifndef __GNUC__
template class DLL_EXPORT Vector<float>;
template class DLL_EXPORT Vector<unsigned int>;
template class DLL_EXPORT Vector<int>;
#endif

/* ************************************************************************ */

#ifndef _MSC_VER
}
#endif

/* ************************************************************************ */
