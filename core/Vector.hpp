
#pragma once

/* ************************************************************************ */

// C++
#include <cmath>
#include <type_traits>

// Simulator
#include "core/Units.hpp"

/* ************************************************************************ */

inline namespace core {

/* ************************************************************************ */

/**
 * @brief 2D vector.
 */
template<typename T>
class Vector
{

// Public Types
public:


    /**
     * @brief Common type helper.
     */
    template<typename T2>
    using CT = typename std::common_type<T, T2>::type;


// Public Ctors
public:


    /**
     * @brief Default constructor.
     */
    constexpr Vector() = default;


    /**
     * @brief Constructor.
     *
     * @param val Value for all vector coordinates.
     */
    explicit constexpr Vector(T val) noexcept
        : x(val), y(val)
    {
        // Nothing to do
    }


    /**
     * @brief Constructor.
     *
     * @param x
     * @param y
     */
    constexpr Vector(T x, T y) noexcept
        : x(x), y(y)
    {
        // Nothing to do
    }


    /**
     * @brief Constructor.
     *
     * @param v Source vector.
     */
    template<typename T2>
    constexpr Vector(const Vector<T2>& v) noexcept
        : x(v.getX()), y(v.getY())
    {
        // Nothing to do
    }


// Public Operators
public:


    /**
     * @brief Assignment operator.
     *
     * @param v
     *
     * @return
     */
    Vector& operator=(const Vector& v) noexcept
    {
        x = v.x;
        y = v.y;
        return *this;
    }


    /**
     * @brief Assignment operator.
     *
     * @param v
     *
     * @return
     */
    template<typename T2>
    Vector& operator=(const Vector<T2>& v) noexcept
    {
        x = v.x;
        y = v.y;
        return *this;
    }


    /**
     * @brief Compare operator.
     *
     * @param rhs
     *
     * @return
     */
    constexpr bool operator==(const Vector& rhs) const noexcept
    {
        return x == rhs.x && y == rhs.y;
    }


    /**
     * @brief Compare operator.
     *
     * @param rhs
     *
     * @return
     */
    constexpr bool operator!=(const Vector& rhs) const noexcept
    {
        return !operator==(rhs);
    }


    /**
     * @brief Inverse operator.
     *
     * @return
     */
    constexpr Vector operator-() const noexcept
    {
        return Vector{-x, -y};
    }


    /**
     * @brief Add operator.
     *
     * @param v
     *
     * @return
     */
    constexpr Vector operator+(const Vector& v) const noexcept
    {
        return Vector{x + v.x, y + v.y};
    }


    /**
     * @brief Add operator.
     *
     * @param v
     *
     * @return
     */
    template<typename T2>
    constexpr Vector<CT<T2>> operator+(const Vector<T2>& v) const noexcept
    {
        return Vector<CT<T2>>{x + v.getX(), y + v.getY()};
    }


    /**
     * @brief Add operator.
     *
     * @param v
     *
     * @return
     */
    Vector& operator+=(const Vector& v) noexcept
    {
        x += v.x;
        y += v.y;
        return *this;
    }


    /**
     * @brief Add operator.
     *
     * @param v
     *
     * @return
     */
    template<typename T2>
    Vector& operator+=(const Vector<T2>& v) noexcept
    {
        x += v.getX();
        y += v.getY();
        return *this;
    }


    /**
     * @brief Substract operator.
     *
     * @param v
     *
     * @return
     */
    constexpr Vector operator-(const Vector& v) const noexcept
    {
        return Vector{x - v.x, y - v.y};
    }


    /**
     * @brief Substract operator.
     *
     * @param v
     *
     * @return
     */
    template<typename T2>
    constexpr Vector<CT<T2>> operator-(const Vector<T2>& v) const noexcept
    {
        return Vector<CT<T2>>{x - v.getX(), y - v.getY()};
    }


    /**
     * @brief Substract operator.
     *
     * @param v
     *
     * @return
     */
    Vector& operator-=(const Vector& v) noexcept
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }


    /**
     * @brief Substract operator.
     *
     * @param v
     *
     * @return
     */
    template<typename T2>
    Vector& operator-=(const Vector<T2>& v) noexcept
    {
        x -= v.getX();
        y -= v.getY();
        return *this;
    }


    /**
     * @brief Multiple operator.
     *
     * @param v
     *
     * @return
     */
    constexpr Vector operator*(const Vector& val) const noexcept
    {
        return Vector{x * val.x, y * val.y};
    }


    /**
     * @brief Multiple operator.
     *
     * @param v
     *
     * @return
     */
    template<typename T2>
    constexpr Vector<CT<T2>> operator*(const Vector<T2>& val) const noexcept
    {
        return Vector<CT<T2>>{x * val.getX(), y * val.getY()};
    }


    /**
     * @brief Multiple operator.
     *
     * @param v
     *
     * @return
     */
    Vector& operator*=(const Vector& val) noexcept
    {
        x *= val.x;
        y *= val.y;
        return *this;
    }


    /**
     * @brief Multiple operator.
     *
     * @param v
     *
     * @return
     */
    template<typename T2>
    Vector& operator*=(const Vector<T2>& val) noexcept
    {
        x *= val.getX();
        y *= val.getY();
        return *this;
    }


    /**
     * @brief Divide operator.
     *
     * @param v
     *
     * @return
     */
    constexpr Vector operator/(const Vector& val) const noexcept
    {
        return Vector{x / val.x, y / val.y};
    }


    /**
     * @brief Divide operator.
     *
     * @param v
     *
     * @return
     */
    template<typename T2>
    constexpr Vector<CT<T2>> operator/(const Vector<T2>& val) const noexcept
    {
        return Vector<CT<T2>>{x / val.getX(), y / val.getY()};
    }


    /**
     * @brief Divide operator.
     *
     * @param v
     *
     * @return
     */
    Vector& operator/=(const Vector& val) noexcept
    {
        x /= val.x;
        y /= val.y;
        return *this;
    }


    /**
     * @brief Divide operator.
     *
     * @param v
     *
     * @return
     */
    template<typename T2>
    Vector& operator/=(const Vector<T2>& val) noexcept
    {
        x /= val.x;
        y /= val.y;
        return *this;
    }


// Public Accessors
public:


    /**
     * @brief Returns X coordinate.
     *
     * @return
     */
    T& getX() noexcept
    {
        return x;
    }


    /**
     * @brief Returns X coordinate.
     *
     * @return
     */
    T getX() const noexcept
    {
        return x;
    }


    /**
     * @brief Returns Y coordinate.
     *
     * @return
     */
    T& getY() noexcept
    {
        return y;
    }


    /**
     * @brief Returns Y coordinate.
     *
     * @return
     */
    T getY() const noexcept
    {
        return y;
    }


    /**
     * @brief Returns width.
     *
     * @return
     */
    T& getWidth() noexcept
    {
        return x;
    }


    /**
     * @brief Returns width.
     *
     * @return
     */
    T getWidth() const noexcept
    {
        return x;
    }


    /**
     * @brief Returns height.
     *
     * @return
     */
    T& getHeight() noexcept
    {
        return y;
    }


    /**
     * @brief Returns height.
     *
     * @return
     */
    T getHeight() const noexcept
    {
        return y;
    }


// Public Operations
public:


    /**
     * @brief Calculate vector length.
     *
     * @return
     */
    T getLength() const noexcept
    {
        return std::sqrt(getLengthSquared());
    }


    /**
     * @brief Calculate vector length - squared.
     *
     * @return
     */
    T getLengthSquared() const noexcept
    {
        return x * x + y * y;
    }


    /**
     * @brief Calculate dot of two vectors.
     *
     * @param rhs Second vector.
     *
     * @return
     */
    T dot(const Vector<T> rhs) const noexcept
    {
        return x * rhs.x + y * rhs.y;
    }


    /**
     * @brief Calculate dot of two vectors.
     *
     * @param rhs Second vector.
     *
     * @return
     */
    template<typename T2>
    CT<T2> dot(const Vector<T2>& rhs) const noexcept
    {
        return x * rhs.getX() + y * rhs.getY();
    }


    /**
     * @brief Rotate current vector and return rotated version.
     *
     * @param angle
     *
     * @return
     */
    Vector rotated(units::Angle angle) const noexcept
    {
        return Vector(
            x * cos(angle) - y * sin(angle),
            x * sin(angle) + y * cos(angle)
        );
    }


// Private Data Members
private:


    /// X coordinate.
    T x;

    /// Y coordinate.
    T y;

};

/* ************************************************************************ */

/**
 * @brief Add single value to vector.
 *
 * @param lhs
 * @param rhs
 *
 * @return New vector.
 */
template<typename T>
inline Vector<T> operator+(const Vector<T>& lhs, T rhs) noexcept
{
    return lhs + Vector<T>{rhs};
}

/* ************************************************************************ */

/**
 * @brief Add single value to vector.
 *
 * @param lhs
 * @param rhs
 *
 * @return New vector
 */
template<typename T>
inline Vector<T> operator+(T lhs, const Vector<T>& rhs) noexcept
{
    return Vector<T>{lhs} + rhs;
}

/* ************************************************************************ */

/**
 * @brief Add single value to vector.
 *
 * @param lhs
 * @param rhs
 *
 * @return New vector.
 */
template<typename T1, typename T2>
inline Vector<typename std::common_type<T1, T2>::type> operator+(const Vector<T1>& lhs, T2 rhs) noexcept
{
    return lhs + Vector<T2>{rhs};
}

/* ************************************************************************ */

/**
 * @brief Add single value to vector.
 *
 * @param lhs
 * @param rhs
 *
 * @return New vector
 */
template<typename T1, typename T2>
inline Vector<typename std::common_type<T1, T2>::type> operator+(T1 lhs, const Vector<T2>& rhs) noexcept
{
    return Vector<T1>{lhs} + rhs;
}

/* ************************************************************************ */

/**
 * @brief Substract single value from vector.
 *
 * @param lhs
 * @param rhs
 *
 * @return New vector
 */
template<typename T>
inline Vector<T> operator-(const Vector<T>& lhs, T rhs) noexcept
{
    return lhs - Vector<T>{rhs};
}

/* ************************************************************************ */

/**
 * @brief Substract single value from vector.
 *
 * @param lhs
 * @param rhs
 *
 * @return New vector
 */
template<typename T>
inline Vector<T> operator-(T lhs, const Vector<T>& rhs) noexcept
{
    return Vector<T>{lhs} - rhs;
}

/* ************************************************************************ */

/**
 * @brief Substract single value from vector.
 *
 * @param lhs
 * @param rhs
 *
 * @return New vector
 */
template<typename T1, typename T2>
inline Vector<typename std::common_type<T1, T2>::type> operator-(const Vector<T1>& lhs, T2 rhs) noexcept
{
    return lhs - Vector<T2>{rhs};
}

/* ************************************************************************ */

/**
 * @brief Substract single value from vector.
 *
 * @param lhs
 * @param rhs
 *
 * @return New vector
 */
template<typename T1, typename T2>
inline Vector<typename std::common_type<T1, T2>::type> operator-(T1 lhs, const Vector<T2>& rhs) noexcept
{
    return Vector<T1>{lhs} - rhs;
}

/* ************************************************************************ */

/**
 * @brief Multiple single value from vector.
 *
 * @param lhs
 * @param rhs
 *
 * @return New vector
 */
template<typename T>
inline Vector<T> operator*(const Vector<T>& lhs, T rhs) noexcept
{
    return lhs * Vector<T>{rhs};
}

/* ************************************************************************ */

/**
 * @brief Multiple single value from vector.
 *
 * @param lhs
 * @param rhs
 *
 * @return New vector
 */
template<typename T>
inline Vector<T> operator*(T lhs, const Vector<T>& rhs) noexcept
{
    return Vector<T>{lhs} * rhs;
}

/* ************************************************************************ */

/**
 * @brief Multiple single value from vector.
 *
 * @param lhs
 * @param rhs
 *
 * @return New vector
 */
template<typename T1, typename T2>
inline Vector<typename std::common_type<T1, T2>::type> operator*(const Vector<T1>& lhs, T2 rhs) noexcept
{
    return lhs * Vector<T2>{rhs};
}

/* ************************************************************************ */

/**
 * @brief Multiple single value from vector.
 *
 * @param lhs
 * @param rhs
 *
 * @return New vector
 */
template<typename T1, typename T2>
inline Vector<typename std::common_type<T1, T2>::type> operator*(T1 lhs, const Vector<T2>& rhs) noexcept
{
    return Vector<T1>{lhs} * rhs;
}

/* ************************************************************************ */

/**
 * @brief Divide vector by one value.
 *
 * @param lhs
 * @param rhs
 *
 * @return New vector
 */
template<typename T>
inline Vector<T> operator/(const Vector<T>& lhs, T rhs) noexcept
{
    return lhs / Vector<T>{rhs};
}

/* ************************************************************************ */

/**
 * @brief Divide single value by vector.
 *
 * @param lhs
 * @param rhs
 *
 * @return New vector.
 */
template<typename T>
inline Vector<T> operator/(T lhs, const Vector<T>& rhs) noexcept
{
    return Vector<T>{lhs} / rhs;
}

/* ************************************************************************ */

/**
 * @brief Divide vector by one value.
 *
 * @param lhs
 * @param rhs
 *
 * @return New vector
 */
template<typename T1, typename T2>
inline Vector<typename std::common_type<T1, T2>::type> operator/(const Vector<T1>& lhs, T2 rhs) noexcept
{
    return lhs / Vector<T2>{rhs};
}

/* ************************************************************************ */

/**
 * @brief Divide single value by vector.
 *
 * @param lhs
 * @param rhs
 *
 * @return New vector.
 */
template<typename T1, typename T2>
inline Vector<typename std::common_type<T1, T2>::type> operator/(T1 lhs, const Vector<T2>& rhs) noexcept
{
    return Vector<T1>{lhs} / rhs;
}

/* ************************************************************************ */

extern template class Vector<float>;

/* ************************************************************************ */

}

/* ************************************************************************ */
