# ######################################################################### #
# LIA UPM (c) 2016                                                          #
# ######################################################################### #
# Department of Artifitial Intelligence                                     #
# Faculty of Informatics                                                    #
# Polytechnic University of Madrid - Spain                                  #
# ######################################################################### #
#                                                                           #
# This file is part of CeCe.                                                #
#                                                                           #
# CeCe is free software: you can redistribute it and/or modify              #
# it under the terms of the GNU General Public License as published by      #
# the Free Software Foundation, either version 3 of the License, or         #
# (at your option) any later version.                                       #
#                                                                           #
# CeCe is distributed in the hope that it will be useful,                   #
# but WITHOUT ANY WARRANTY; without even the implied warranty of            #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             #
# GNU General Public License for more details.                              #
#                                                                           #
# You should have received a copy of the GNU General Public License         #
# along with CeCe.  If not, see <http://www.gnu.org/licenses/>.             #
#                                                                           #
# ######################################################################### #

# Sources
set(SRCS
    Library.cpp
    Module.hpp
    Module.cpp
)

# Build plugin
build_plugin(infection
    SOURCES ${SRCS}
    PLUGINS_REQUIRED cell
)

# ######################################################################### #
