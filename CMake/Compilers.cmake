# ######################################################################### #
# Department of Cybernetics                                                 #
# Faculty of Applied Sciences                                               #
# University of West Bohemia in Pilsen                                      #
# ######################################################################### #

# Options specific for different compilers
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # using Clang
    option(CLANG_ASAN               "If address sanitizer should be used" Off)
    option(CLANG_MSAN               "If memory sanitizer should be used" Off)
    option(CLANG_USE_LIBCPP         "If libc++ should be used" Off)

    # C++11 required
    add_compile_options(-Wall -pedantic-errors -pedantic -std=c++11)

    # Use libc++
    if (CLANG_USE_LIBCPP)
        add_compile_options(-stdlib=libc++)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
    endif()

    # Debug flags
    if (CLANG_ASAN)
        message("-- Using address sanitizer")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
    elseif (CLANG_MSAN)
        message("-- Using memory sanitizer")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=memory -fno-omit-frame-pointer")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=memory -fno-omit-frame-pointer")
    endif ()
elseif (CMAKE_COMPILER_IS_GNUCXX)
    # using GCC

    # C++11 required
    add_compile_options(-Wall -pedantic-errors -pedantic -std=c++11)

elseif (MSVC)
    # using Visual Studio C++
    add_compile_options(/W3)
endif()

# ######################################################################### #