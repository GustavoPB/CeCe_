/* ************************************************************************ */
/* Georgiev Lab (c) 2016                                                    */
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

// Qt
#include <QApplication>

// CeCe
#include "cece/core/String.hpp"
#include "cece/core/FilePath.hpp"
#include "cece/plugin/Manager.hpp"

// GUI
#include "MainWindow.hpp"

/* ************************************************************************ */

/**
 * @brief Returns plugins directory.
 *
 * @param app Executable path.
 * @param dir Directory to plugins.
 *
 * @return
 */
cece::String getPluginsDirectory(cece::FilePath app, cece::FilePath dir) noexcept
{
    return (app.remove_filename() / dir).string();
}

/* ************************************************************************ */

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    auto& pluginManager = cece::plugin::Manager::s();

    if (pluginManager.getDirectories().empty())
    {
#ifdef DIR_PLUGINS
        pluginManager.addDirectory(DIR_PLUGINS);
#elif __linux__
        pluginManager.addDirectory(getPluginsDirectory(argv[0], "../lib/cece/plugins"));
#elif __APPLE__ && __MACH__
        pluginManager.addDirectory(getPluginsDirectory(argv[0], "../plugins"));
#elif _WIN32
        pluginManager.addDirectory(getPluginsDirectory(argv[0], "."));
#endif
    }

    // Preload XML plugin
    pluginManager.load("xml");

    app.setOrganizationName("GeorgievLab");
    app.setOrganizationDomain("ccy.zcu.cz");
    app.setApplicationName("cece");
    app.setApplicationVersion("0.4.3");
    app.setApplicationDisplayName("CeCe");

    cece::gui::MainWindow w;
    w.show();

    return app.exec();
}

/* ************************************************************************ */
