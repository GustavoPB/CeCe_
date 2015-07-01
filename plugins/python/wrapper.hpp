
#pragma once

/* ************************************************************************ */

// C++
#include <typeindex>

// Simulator
#include "core/compatibility.hpp"
#include "core/Exception.hpp"
#include "core/Units.hpp"
#include "core/Tuple.hpp"
#include "core/String.hpp"
#include "core/IntegerSequence.hpp"

// Plugin
#include "Python.hpp"
#include "Handle.hpp"
#include "View.hpp"
#include "ValueCast.hpp"
#include "Exception.hpp"

/* ************************************************************************ */

namespace plugin {
namespace python {

/* ************************************************************************ */

using HashValueType = unsigned long;

/* ************************************************************************ */

/**
 * @brief C++ type wrapper object.
 */
template<typename T>
struct ObjectWrapper
{
    PyObject_HEAD
    T value;
};

/* ************************************************************************ */

/**
 * @brief Register dynamic type.
 *
 * @param info Type info.
 * @param type Python type.
 */
void registerDynamic(const std::type_info& info, PyTypeObject* type);

/* ************************************************************************ */

/**
 * @brief Find dynamic type.
 *
 * @param info Type info.
 *
 * @return Dynamic type or nullptr.
 */
PyTypeObject* findDynamic(const std::type_info& info);

/* ************************************************************************ */

/**
 * @brief Returns reference to object.
 *
 * @param obj Reference to object.
 *
 * @return
 */
template<typename T>
T& ref(T& obj) NOEXCEPT
{
    return obj;
}

/* ************************************************************************ */

/**
 * @brief Returns reference to object.
 *
 * @param obj Pointer to object.
 *
 * @return
 */
template<typename T>
T& ref(T* obj) NOEXCEPT
{
    return *obj;
}

/* ************************************************************************ */

/**
 * @brief Constructor wrapper.
 *
 * @tparam T    Object type.
 * @tparam Args Argument types.
 */
template<typename T, typename... Args>
struct Constructor
{
    /// Plain object type.
    using ObjectType = typename std::remove_pointer<T>::type;

    /// Remove const reference.
    template<typename Type>
    using RemoveConstRef = typename std::remove_const<typename std::remove_reference<Type>::type>::type;


    /**
     * @brief Convert to PyMethodDef.
     *
     * @return
     */
    static initproc to() NOEXCEPT
    {
        return (initproc) &construct;
    }


    /**
     * @brief Calls constructor (helper).
     *
     * @param self
     * @param args
     *
     * @return
     */
    template<int... I>
    static int construct_inner(ObjectWrapper<T>* self, PyObject* args, IntegerSequence<I...>) NOEXCEPT
    {
        assert(self);
        assert(args);

        // Tuple of args
        const Tuple<RemoveConstRef<Args>...> tupleArgs{
            cast<RemoveConstRef<Args>>(PyTuple_GetItem(args, I))...
        };

        void* ptr = &ref(self->value);
        new (ptr) T(std::get<I>(tupleArgs)...);

        return 0;
    }


    /**
     * @brief Calls constructor.
     *
     * @param self
     * @param args
     * @param kwds
     *
     * @return
     */
    static int construct(ObjectWrapper<T>* self, PyObject* args, PyObject* kwds)
    {
        return construct_inner(self, args, MakeIntegerSequence<0, sizeof...(Args)>{});
    }
};

/* ************************************************************************ */

/**
 * @brief Property wrapper.
 *
 * @tparam T    Object type.
 * @tparam Val  Value type.
 */
template<HashValueType Hash, typename T, typename Val>
struct Property
{
    /// Plain object type.
    using ObjectType = typename std::remove_pointer<T>::type;

    /// Getter function type.
    using Getter = Val(ObjectType::*)() const;

    /// Setter function type.
    using Setter = void(ObjectType::*)(Val);


    /// Property name.
    static const char* name;

    /// Getter function pointer.
    static Getter getFn;

    /// Setter function pointer.
    static Setter setFn;


    /**
     * @brief Convert to PyGetSetDef.
     *
     * @return
     */
    static PyGetSetDef to() NOEXCEPT
    {
        return {const_cast<char*>(name), (getter) get, (setter) set, nullptr, nullptr};
    }


    /**
     * @brief Calls object getter.
     *
     * @param self
     * @param closure
     *
     * @return
     */
    static PyObject* get(ObjectWrapper<T>* self, void* closure) NOEXCEPT
    {
        assert(self);
        assert(getFn);
        return cast((ref(self->value).*getFn)()).release();
    }


    /**
     * @brief Calls object setter.
     *
     * @param self
     * @param closure
     *
     * @return
     */
    static int set(ObjectWrapper<T>* self, PyObject* value, void* closure)
    {
        if (!check<Val>(value))
            throw Exception("Cannot convert value");

        assert(self);
        assert(setFn);
        (ref(self->value).*setFn)(cast<Val>(value));

        return 0;
    }
};

/* ************************************************************************ */

/**
 * @brief Property wrapper (non-member).
 *
 * @tparam T    Object type.
 * @tparam Val  Value type.
 */
template<HashValueType Hash, typename T, typename Val>
struct PropertyAlt
{
    /// Plain object type.
    using ObjectType = typename std::remove_pointer<T>::type;

    /// Getter function type.
    using Getter = Val(*)(T);

    /// Setter function type.
    using Setter = void(*)(T, Val);


    /// Property name.
    static const char* name;

    /// Getter function pointer.
    static Getter getFn;

    /// Setter function pointer.
    static Setter setFn;


    /**
     * @brief Convert to PyGetSetDef.
     *
     * @return
     */
    static PyGetSetDef to() NOEXCEPT
    {
        return {const_cast<char*>(name), (getter) get, (setter) set, nullptr, nullptr};
    }


    /**
     * @brief Calls object getter.
     *
     * @param self
     * @param closure
     *
     * @return
     */
    static PyObject* get(ObjectWrapper<T>* self, void* closure) NOEXCEPT
    {
        assert(self);
        assert(getFn);
        return cast<Val>(getFn(self->value)).release();
    }


    /**
     * @brief Calls object setter.
     *
     * @param self
     * @param closure
     *
     * @return
     */
    static int set(ObjectWrapper<T>* self, PyObject* value, void* closure)
    {
        if (!check<Val>(value))
            throw Exception("Cannot convert value");

        assert(self);
        assert(setFn);
        setFn(self->value, cast<Val>(value));

        return 0;
    }
};

/* ************************************************************************ */

/**
 * @brief Define type property.
 *
 * @param name
 * @param getter
 * @param setter
 *
 * @return
 */
template<HashValueType Hash, typename T, typename Val, typename TR = typename std::remove_pointer<T>::type>
PyGetSetDef defineProperty(const char* name, Val(TR::*getter)() const, void(TR::*setter)(Val) = nullptr) NOEXCEPT
{
    using PropertyType = Property<Hash, T, Val>;

    PropertyType::name = name;
    PropertyType::getFn = getter;
    PropertyType::setFn = setter;

    return PropertyType::to();
}

/* ************************************************************************ */

/**
 * @brief Define type property (non-member functions).
 *
 * @param name
 * @param getter
 * @param setter
 *
 * @return
 */
template<HashValueType Hash, typename T, typename Val>
PyGetSetDef defineProperty(const char* name, Val(*getter)(T), void(*setter)(T, Val) = nullptr) NOEXCEPT
{
    using PropertyType = PropertyAlt<Hash, T, Val>;

    PropertyType::name = name;
    PropertyType::getFn = getter;
    PropertyType::setFn = setter;

    return PropertyType::to();
}

/* ************************************************************************ */

/**
 * @brief Member function wrapper.
 *
 * @tparam T    Object type.
 * @tparam Name Name type.
 * @tparam Ret  Return type.
 * @tparam Args Argument types.
 */
template<HashValueType Hash, typename T, typename Ret, typename... Args>
struct MemberFunction
{
    /// Plain object type.
    using ObjectType = typename std::remove_pointer<T>::type;

    /// Function type.
    using Function = Ret(ObjectType::*)(Args...);

    /// Remove const reference.
    template<typename Type>
    using RemoveConstRef = typename std::remove_const<typename std::remove_reference<Type>::type>::type;

    /// Function name.
    static const char* name;

    /// Function pointer.
    static Function fn;


    /**
     * @brief Convert to PyMethodDef.
     *
     * @return
     */
    static PyMethodDef to() NOEXCEPT
    {
        return {name, (PyCFunction) call, METH_VARARGS, nullptr};
    }


    /**
     * @brief Calls object function (helper).
     *
     * @param self
     * @param args
     *
     * @return
     */
    template<int... I>
    static PyObject* call_inner(ObjectWrapper<T>* self, PyObject* args, IntegerSequence<I...>) NOEXCEPT
    {
        assert(self);
        assert(args);
        assert(fn);

        // Tuple of args
        const Tuple<RemoveConstRef<Args>...> tupleArgs{
            cast<RemoveConstRef<Args>>(PyTuple_GetItem(args, I))...
        };

        return cast<Ret>((ref(self->value).*fn)(std::get<I>(tupleArgs)...)).release();
    }


    /**
     * @brief Calls object function.
     *
     * @param self
     * @param args
     * @param closure
     *
     * @return
     */
    static PyObject* call(ObjectWrapper<T>* self, PyObject* args, void* closure) NOEXCEPT
    {
        return call_inner(self, args, MakeIntegerSequence<0, sizeof...(Args)>{});
    }
};

/* ************************************************************************ */

/**
 * @brief Member function wrapper.
 *
 * @tparam T    Object type.
 * @tparam Name Name type.
 * @tparam Ret  Return type.
 * @tparam Args Argument types.
 */
template<HashValueType Hash, typename T, typename... Args>
struct MemberFunction<Hash, T, void, Args...>
{
    /// Plain object type.
    using ObjectType = typename std::remove_pointer<T>::type;

    /// Function type.
    using Function = void(ObjectType::*)(Args...);

    /// Remove const reference.
    template<typename Type>
    using RemoveConstRef = typename std::remove_const<typename std::remove_reference<Type>::type>::type;

    /// Function name.
    static const char* name;

    /// Function pointer.
    static Function fn;


    /**
     * @brief Convert to PyMethodDef.
     *
     * @return
     */
    static PyMethodDef to() NOEXCEPT
    {
        return {name, (PyCFunction) call, METH_VARARGS, nullptr};
    }


    /**
     * @brief Calls object function (helper).
     *
     * @param self
     * @param args
     *
     * @return
     */
    template<int... I>
    static PyObject* call_inner(ObjectWrapper<T>* self, PyObject* args, IntegerSequence<I...>) NOEXCEPT
    {
        assert(self);
        assert(args);
        assert(fn);

        // Tuple of args
        const Tuple<RemoveConstRef<Args>...> tupleArgs{
            cast<RemoveConstRef<Args>>(PyTuple_GetItem(args, I))...
        };

        (ref(self->value).*fn)(std::get<I>(tupleArgs)...);
        return cast().release();
    }


    /**
     * @brief Calls object function (helper).
     *
     * @param self
     * @param args
     *
     * @return
     */
    static PyObject* call_inner(ObjectWrapper<T>* self, PyObject* args, IntegerSequence<>) NOEXCEPT
    {
        assert(self);
        assert(args);
        assert(fn);

        (ref(self->value).*fn)();
        return cast().release();
    }


    /**
     * @brief Calls object function.
     *
     * @param self
     * @param args
     * @param closure
     *
     * @return
     */
    static PyObject* call(ObjectWrapper<T>* self, PyObject* args, void* closure) NOEXCEPT
    {
        return call_inner(self, args, MakeIntegerSequence<0, sizeof...(Args)>{});
    }
};

/* ************************************************************************ */

/**
 * @brief Member function wrapper.
 *
 * @tparam T    Object type.
 * @tparam Ret  Return type.
 * @tparam Args Argument types.
 */
template<HashValueType Hash, typename T, typename Ret, typename... Args>
struct MemberFunctionConst
{
    /// Plain object type.
    using ObjectType = typename std::remove_pointer<T>::type;

    /// Function type.
    using Function = Ret(ObjectType::*)(Args...) const;

    /// Remove const reference.
    template<typename Type>
    using RemoveConstRef = typename std::remove_const<typename std::remove_reference<Type>::type>::type;


    /// Function name.
    static const char* name;

    /// Function pointer.
    static Function fn;


    /**
     * @brief Convert to PyMethodDef.
     *
     * @return
     */
    static PyMethodDef to() NOEXCEPT
    {
        return {name, (PyCFunction) call, METH_VARARGS, nullptr};
    }


    /**
     * @brief Calls object function (helper).
     *
     * @param self
     * @param args
     *
     * @return
     */
    template<int... I>
    static PyObject* call_inner(ObjectWrapper<T>* self, PyObject* args, IntegerSequence<I...>) NOEXCEPT
    {
        assert(self);
        assert(args);
        assert(fn);

        // Tuple of args
        const Tuple<RemoveConstRef<Args>...> tupleArgs{
            cast<RemoveConstRef<Args>>(PyTuple_GetItem(args, I))...
        };

        return cast<Ret>((ref(self->value).*fn)(std::get<I>(tupleArgs)...)).release();
    }


    /**
     * @brief Calls object function.
     *
     * @param self
     * @param args
     * @param closure
     *
     * @return
     */
    static PyObject* call(ObjectWrapper<T>* self, PyObject* args, void* closure) NOEXCEPT
    {
        return call_inner(self, args, MakeIntegerSequence<0, sizeof...(Args)>{});
    }
};

/* ************************************************************************ */

/**
 * @brief Define type property.
 *
 * @param name
 * @param fn
 *
 * @return
 */
template<HashValueType Hash, typename T, typename Ret, typename... Args, typename TR = typename std::remove_pointer<T>::type>
PyMethodDef defineMemberFunction(const char* name, Ret(TR::*fn)(Args...)) NOEXCEPT
{
    using MemberFunctionType = MemberFunction<Hash, T, Ret, Args...>;

    MemberFunctionType::name = name;
    MemberFunctionType::fn = fn;

    return MemberFunctionType::to();
}

/* ************************************************************************ */

/**
 * @brief Define type property.
 *
 * @param name
 * @param fn
 *
 * @return
 */
template<HashValueType Hash, typename T, typename Ret, typename... Args, typename TR = typename std::remove_pointer<T>::type>
PyMethodDef defineMemberFunction(const char* name, Ret(TR::*fn)(Args...) const) NOEXCEPT
{
    using MemberFunctionType = MemberFunctionConst<Hash, T, Ret, Args...>;

    MemberFunctionType::name = name;
    MemberFunctionType::fn = fn;

    return MemberFunctionType::to();
}

/* ************************************************************************ */

/**
 * @brief Object type definition.
 *
 * @tparam T
 */
template<typename T>
struct TypeDefinition
{

    /// Object type.
    using type = ObjectWrapper<T>;


    /// Type definition
    static PyTypeObject definition;


    /**
     * @brief Define type object.
     *
     * @param name
     * @param base
     *
     * @return
     */
    static void init(const char* name, const char* base = nullptr) NOEXCEPT
    {
        definition = {
            PyObject_HEAD_INIT(NULL)
            0,                              // ob_size
            name,                           // tp_name
            sizeof(type),                   // tp_basicsize
            0,                              // tp_itemsize
            0,                              // tp_dealloc
            0,                              // tp_print
            0,                              // tp_getattr
            0,                              // tp_setattr
            0,                              // tp_compare
            0,                              // tp_repr
            0,                              // tp_as_number
            0,                              // tp_as_sequence
            0,                              // tp_as_mapping
            0,                              // tp_hash
            0,                              // tp_call
            0,                              // tp_str
            0,                              // tp_getattro
            0,                              // tp_setattro
            0,                              // tp_as_buffer
            Py_TPFLAGS_DEFAULT,             // tp_flags
            nullptr,                        // tp_doc
        };

        if (base)
        {
            const String baseFullName(base);
            const auto sep = baseFullName.find('.');
            String module = baseFullName.substr(0, sep);
            String baseName = baseFullName.substr(sep + 1);

            // Get simulator module
            auto simulatorModule = makeHandle(PyImport_ImportModule(module.c_str()));
            auto dict = PyModule_GetDict(simulatorModule);
            auto moduleClass = makeHandle(PyMapping_GetItemString(dict, const_cast<char*>(baseName.c_str())));
            assert(PyType_Check(moduleClass));

            // Base class
            definition.tp_base = (PyTypeObject*) moduleClass.release();
        }
    }


    /**
     * @brief Ready type.
     *
     * @return
     */
    static bool ready() NOEXCEPT
    {
        // Initialize type
        return (PyType_Ready(&definition) >= 0);
    }


    /**
     * @brief Check if definition is valid.
     *
     * @return
     */
    static bool valid() NOEXCEPT
    {
        return definition.tp_name != nullptr;
    }


    /**
     * @brief Wrap value.
     *
     * @param value
     *
     * @return
     */
    static Handle<PyObject> wrap(T value) NOEXCEPT
    {
        auto type = findDynamic(typeid(ref(value)));

        if (!type)
            type = &definition;

        // Create new object
        ObjectWrapper<T>* obj = PyObject_New(ObjectWrapper<T>, type);

        obj->value = std::move(value);

        return Handle<PyObject>(reinterpret_cast<PyObject*>(obj));
    }


    /**
     * @brief Unwrap value.
     *
     * @param value
     *
     * @return
     */
    static T unwrap(View<PyObject> value) NOEXCEPT
    {
        assert(PyObject_TypeCheck(value, &definition));

        // Cast to wrapper type
        ObjectWrapper<T>* obj = reinterpret_cast<ObjectWrapper<T>*>(value.get());

        return obj->value;
    }
};

/* ************************************************************************ */

template<HashValueType Hash, typename T, typename Val>
const char* Property<Hash, T, Val>::name;

/* ************************************************************************ */

template<HashValueType Hash, typename T, typename Val>
typename Property<Hash, T, Val>::Getter Property<Hash, T, Val>::getFn;

/* ************************************************************************ */

template<HashValueType Hash, typename T, typename Val>
typename Property<Hash, T, Val>::Setter Property<Hash, T, Val>::setFn;

/* ************************************************************************ */

template<HashValueType Hash, typename T, typename Val>
const char* PropertyAlt<Hash, T, Val>::name;

/* ************************************************************************ */

template<HashValueType Hash, typename T, typename Val>
typename PropertyAlt<Hash, T, Val>::Getter PropertyAlt<Hash, T, Val>::getFn;

/* ************************************************************************ */

template<HashValueType Hash, typename T, typename Val>
typename PropertyAlt<Hash, T, Val>::Setter PropertyAlt<Hash, T, Val>::setFn;

/* ************************************************************************ */

template<HashValueType Hash, typename T, typename Ret, typename... Args>
const char* MemberFunction<Hash, T, Ret, Args...>::name;

/* ************************************************************************ */

template<HashValueType Hash, typename T, typename Ret, typename... Args>
typename MemberFunction<Hash, T, Ret, Args...>::Function MemberFunction<Hash, T, Ret, Args...>::fn;

/* ************************************************************************ */

template<HashValueType Hash, typename T, typename... Args>
const char* MemberFunction<Hash, T, void, Args...>::name;

/* ************************************************************************ */

template<HashValueType Hash, typename T, typename... Args>
typename MemberFunction<Hash, T, void, Args...>::Function MemberFunction<Hash, T, void, Args...>::fn;

/* ************************************************************************ */

template<HashValueType Hash, typename T, typename Ret, typename... Args>
const char* MemberFunctionConst<Hash, T, Ret, Args...>::name;

/* ************************************************************************ */

template<HashValueType Hash, typename T, typename Ret, typename... Args>
typename MemberFunctionConst<Hash, T, Ret, Args...>::Function MemberFunctionConst<Hash, T, Ret, Args...>::fn;

/* ************************************************************************ */

template<typename T>
PyTypeObject TypeDefinition<T>::definition;

/* ************************************************************************ */

}
}

/* ************************************************************************ */