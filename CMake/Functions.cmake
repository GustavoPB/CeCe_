# ######################################################################### #
# Department of Cybernetics                                                 #
# Faculty of Applied Sciences                                               #
# University of West Bohemia in Pilsen                                      #
# ######################################################################### #

##
## Build plugin helper function
##
## param: NAME      Plugin name.
## param: SOURCES   List of sources.
## param: LIBRARIES A list of required libraries.
## param: PLUGINS_REQUIRED A list of required plugins.
##
## Function uses PLUGINS_BUILDIN_${NAME} variable.
##
function(build_plugin NAME)

    # Get project name
    plugin_project_name(${NAME} FULLNAME)

    cmake_parse_arguments(ARG "" "" "SOURCES;LIBRARIES;DEPENDENCIES;PLUGINS_REQUIRED" ${ARGN})

    # Modify list of plugins (add prefixes)
    set(PLUGINS_DEPENDENCIES "")
    foreach (PLUGIN ARG_PLUGINS_REQUIRED)
        list(APPEND PLUGINS_DEPENDENCIES "${PREFIX}${PLUGIN}")
    endforeach ()

    # Create project
    project(${FULLNAME} CXX)

    # If plugin will be build-in
    if (PLUGINS_BUILDIN_${NAME})
        # Required for Linux
        if (UNIX AND NOT APPLE)
            add_compile_options(-fPIC)
        endif ()

        # Pass macro to inform about build-in version
        # TODO: change to plugin
        add_definitions(-DLIBRARY_BUILDIN=1)

        # Static library
        add_library(${FULLNAME} ${ARG_SOURCES})

        # Setup dependencies
        add_dependencies(${FULLNAME}
            core
            ${DEPENDENCIES}
            ${PLUGINS_DEPENDENCIES}
        )

        # Link libraries
        target_link_libraries(${FULLNAME}
            ${ARG_LIBRARIES}
        )
    else ()
        # Shared library/module
        if (ENABLE_PLUGINS_DIRECT_LINK)
            add_library(${FULLNAME} SHARED ${ARG_SOURCES})
        else ()
            add_library(${FULLNAME} MODULE ${ARG_SOURCES})
        endif ()

        # Setup dependencies
        add_dependencies(${FULLNAME}
            core
            simulator
            ${DEPENDENCIES}
            ${PLUGINS_DEPENDENCIES}
        )

        # Link libraries
        target_link_libraries(${FULLNAME}
            simulator
            ${ARG_LIBRARIES}
        )

        # Install shared library
        install(TARGETS ${FULLNAME}
            DESTINATION lib
        )
    endif ()
endfunction()

# ######################################################################### #

# Get plugin project name.
function(plugin_project_name PLUGIN_NAME PROJECT_NAME)
    # TODO: change to plugin
    set(${PROJECT_NAME} "module-${PLUGIN_NAME}" PARENT_SCOPE)
endfunction ()

# ######################################################################### #

# Get plugins project names.
function(plugin_project_names PLUGIN_NAMES PROJECT_NAMES)

    set(NAMES "")

    foreach (PLUGIN_NAME ${PLUGIN_NAMES})
        plugin_project_name(${PLUGIN_NAME} PROJECT_NAME)
        list(APPEND NAMES ${PROJECT_NAME})
    endforeach ()

    set(${PROJECT_NAMES} ${NAMES} PARENT_SCOPE)
endfunction ()

# ######################################################################### #
