
// Declaration
#include "Library.hpp"

// C++
#include <map>
#include <cstdint>
#include <cassert>
#include <cstdlib>

#if __linux__
// Linux
#include <dlfcn.h>
#elif _WIN32
#include <windows.h>
#else
#error Unsupported platform
#endif

// Simulator
#include "core/Log.hpp"
#include "simulator/LibraryApi.hpp"

/* ************************************************************************ */

namespace {

/* ************************************************************************ */

#if __linux__
const std::string g_prefix = "libmodule-";
#elif _WIN32
const std::string g_prefix = "libmodule-";
#endif

/* ************************************************************************ */

#if __linux__
const std::string g_extension = ".so";
#elif _WIN32
const std::string g_extension = ".dll";
#endif

/* ************************************************************************ */

}

/* ************************************************************************ */

namespace simulator {

/* ************************************************************************ */

std::vector<std::string> Library::s_libraryPaths;

/* ************************************************************************ */

#define ITEM(name, validname) extern "C" simulator::LibraryApi* LIBRARY_PROTOTYPE_NAME_BUILDIN(create, validname)();
BUILDIN_LIBRARIES
#undef ITEM

/* ************************************************************************ */

#define ITEM(name, validname) { # name, LIBRARY_PROTOTYPE_NAME_BUILDIN(create, validname) },
const std::map<std::string, Library::CreateFn> Library::s_buildinLibraries{
    BUILDIN_LIBRARIES
};
#undef ITEM

/* ************************************************************************ */

/**
 * @brief OS dependent library implementation.
 */
struct Library::Impl
{
// Public Ctors & Dtors
public:


    /**
     * @brief Constructor.
     *
     * @param name
     */
    explicit Impl(const std::string& name)
        : m_filename(g_prefix + name + g_extension)
    {
        core::Log::debug("Loading shared library: ", m_filename);
#if __linux__
        m_ptr = dlopen(m_filename.c_str(), RTLD_LAZY);
#elif _WIN32
        m_ptr = LoadLibrary(m_filename.c_str());
#endif
    }


    /**
     * @brief Destructor.
     */
    ~Impl()
    {
        core::Log::debug("Closing shared library: ", m_filename);
#if __linux__
        if (m_ptr)
            dlclose(m_ptr);
#elif _WIN32
        FreeLibrary(m_ptr);
#endif
    }


// Public Accessors
public:


    /**
     * @brief Returns if library is loaded.
     *
     * @return
     */
    bool isLoaded() const NOEXCEPT
    {
        return m_ptr != nullptr;
    }


    /**
     * @brief Returns error string.
     *
     * @return
     */
    std::string getError() const NOEXCEPT
    {
#if __linux__
        return dlerror();
#elif _WIN32
		return "Error code: " + std::to_string(GetLastError());
#endif
    }


    /**
     * @brief Returns address of required symbol.
     *
     * @param name
     */
    void* getAddr(const char* name) const NOEXCEPT
    {
#if __linux__
        return dlsym(m_ptr, name);
#elif _WIN32
        return reinterpret_cast<void*>(reinterpret_cast<std::intptr_t>(GetProcAddress(m_ptr, name)));
#endif
    }


    /**
     * @brief Returns address of required symbol.
     *
     * @param name
     */
    template<typename T>
    T getAddr(const char* name) const NOEXCEPT
    {
        return reinterpret_cast<T>(reinterpret_cast<std::intptr_t>(getAddr(name)));
    }


// Private Data Members
private:

    /// Source file name.
    std::string m_filename;

#if __linux__
    void* m_ptr;
#elif _WIN32
    HMODULE m_ptr;
#endif

};

/* ************************************************************************ */

Library::Library(const std::string& name)
{
    // Try to find in build-in libraries
    auto it = s_buildinLibraries.find(name);

    // Required library is build-in
    if (it != s_buildinLibraries.end())
    {
        // Create library API
        m_api.reset(it->second());
    }
    else
    {
        // Create dynamic implementation
        m_impl.reset(new Impl{name});

        if (!m_impl->isLoaded())
            throw std::runtime_error("Library is not loaded");

        // Check API version
        auto apiVerFn = m_impl->getAddr<ApiVersionFn>("api_version");

        if (!apiVerFn)
            throw std::runtime_error("Library doesn't contains 'api_version' function");

        if (apiVerFn() != LIBRARY_API_VERSION)
            throw std::runtime_error("Library API version is different from the simulator");

        auto fn = m_impl->getAddr<CreateFn>("create");

        if (!fn)
            throw std::runtime_error("Library doesn't contains 'create' function");

        // Create extension object
        m_api.reset(fn());
    }
}

/* ************************************************************************ */

Library::~Library()
{
    // Nothing to do
}

/* ************************************************************************ */

bool Library::isLoaded() const NOEXCEPT
{
    if (!m_impl)
        return true;

    return m_impl->isLoaded();
}
/* ************************************************************************ */

std::string Library::getError() const NOEXCEPT
{
    if (!m_impl)
        return {};

    return m_impl->getError();
}

/* ************************************************************************ */

void Library::addLibraryPath(std::string path)
{
#if __linux__
    // Get previous paths
    std::string paths;
    char* p = getenv("LD_LIBRARY_PATH");

    // Previously set
    if (p)
    {
        paths = p;
        paths.push_back(':');
    }

    // Append new path
    paths.append(path);

    // Update environment value
    setenv("LD_LIBRARY_PATH", paths.c_str(), 1);
#endif

    s_libraryPaths.push_back(std::move(path));
}

/* ************************************************************************ */

std::vector<std::string> Library::getBuildInNames() NOEXCEPT
{
    std::vector<std::string> names;
    names.reserve(s_buildinLibraries.size());

    for (const auto& p : s_buildinLibraries)
        names.push_back(p.first);

    return names;
}

/* ************************************************************************ */

}

/* ************************************************************************ */
