
#pragma once

/* ************************************************************************ */

// C++
#include <cassert>
#include <type_traits>

// Simulator
#include "core/Vector.hpp"
#include "simulator/Shape.hpp"

/* ************************************************************************ */

namespace simulator {

/* ************************************************************************ */

/**
 * @brief Map shape to grid.
 *
 * @tparam OutIt Output iterator type.
 * @tparam T     Coordinate value type.
 * @tparam StepT Step type.
 *
 * @param out    Output iterator. It's required to be valid for all incrementations.
 * @param shape  Circle shape
 * @param center Circle center in grid coordinates.
 * @param max    Maximum coordinates.
 * @param mim    Minimum coordinates (default is {0, 0}).
 */
template<typename OutIt, typename T, typename StepT>
OutIt mapShapeToGrid(OutIt out, const Shape& shape, const Vector<StepT>& steps,
                     const Vector<T>& center,
                     const Vector<T>& max, const Vector<T>& min = {})
{
    switch (shape.type)
    {
    default:
        break;

    case ShapeType::Circle:
        return mapShapeToGrid(out, shape.circle, steps, center, max, min);
    }

    return out;
}

/* ************************************************************************ */

/**
 * @brief Map circle shape to grid.
 *
 * @tparam OutIt Output iterator type.
 * @tparam T     Coordinate value type.
 * @tparam StepT Step type.
 *
 * @param out    Output iterator. It's required to be valid for all incrementations.
 * @param shape  Circle shape
 * @param center Circle center in grid coordinates.
 * @param max    Maximum coordinates.
 * @param mim    Minimum coordinates (default is {0, 0}).
 */
template<typename OutIt, typename T, typename StepT>
OutIt mapShapeToGrid(OutIt out, const ShapeCircle& shape, const Vector<StepT>& steps,
                     const Vector<T>& center,
                     const Vector<T>& max, const Vector<T>& min = {})
{
    // Get signed type
    using Ts = typename std::make_signed<T>::type;

    // Radius steps in grid
    const Vector<float> radiusSteps = shape.radius / steps;
    const Vector<Ts> shapeCenter = center + shape.center / steps;
    const Vector<Ts> maxS = max;
    const Vector<Ts> minS = min;

    // Foreach grid given by radius steps
    for (Ts x = -radiusSteps.getX(); x < radiusSteps.getX(); ++x)
    {
        for (Ts y = -radiusSteps.getY(); y < radiusSteps.getY(); ++y)
        {
            // Calculate normalized length for ellipse
            const Vector<float> xy(x, y);
            const auto len = (xy / radiusSteps).getLengthSquared();

            if (len > 1.f)
                continue;

            // Calculate grid coordinates
            const Vector<T> coord = shapeCenter + xy;

            // Check if coordinates are in range
            if (!coord.inRange(minS, maxS))
                continue;

            // Insert coordinates into output iterator
            *out++ = coord;
        }
    }

    return out;
}

/* ************************************************************************ */

/**
 * @brief Map shape border to grid.
 *
 * @tparam OutIt Output iterator type.
 * @tparam T     Coordinate value type.
 * @tparam StepT Step type.
 *
 * @param out    Output iterator. It's required to be valid for all incrementations.
 * @param shape  Circle shape
 * @param center Circle center in grid coordinates.
 * @param max    Maximum coordinates.
 * @param mim    Minimum coordinates (default is {0, 0}).
 */
template<typename OutIt, typename T, typename StepT>
OutIt mapShapeBorderToGrid(OutIt out, const Shape& shape, const Vector<StepT>& steps,
                           const Vector<T>& center,
                           const Vector<T>& max, const Vector<T>& min = {})
{
    switch (shape.type)
    {
    default:
        break;

    case ShapeType::Circle:
        return mapShapeBorderToGrid(out, shape.circle, steps, center, max, min);
    }

    return out;
}

/* ************************************************************************ */

/**
 * @brief Map circle shape border to grid.
 *
 * @tparam OutIt Output iterator type.
 * @tparam T     Coordinate value type.
 * @tparam StepT Step type.
 *
 * @param out    Output iterator. It's required to be valid for all incrementations.
 * @param shape  Circle shape
 * @param center Circle center in grid coordinates.
 * @param max    Maximum coordinates.
 * @param mim    Minimum coordinates (default is {0, 0}).
 *
 * @see https://web.archive.org/web/20120225095359/http://homepage.smc.edu/kennedy_john/belipse.pdf
 */
template<typename OutIt, typename T, typename StepT>
OutIt mapShapeBorderToGrid(OutIt out, const ShapeCircle& shape, const Vector<StepT>& steps,
                           const Vector<T>& center,
                           const Vector<T>& max, const Vector<T>& min = {})
{
    // Get signed type
    using Ts = typename std::make_signed<T>::type;

    // Radius steps in grid
    const Vector<T> radius = shape.radius / steps;
    const Vector<Ts> shapeCenter = center + shape.center / steps;
    const Vector<Ts> maxS = max;
    const Vector<Ts> minS = min;

    auto putCoord = [&out, &shapeCenter, &minS, &maxS](Vector<Ts> xy) {
        // Calculate grid coordinates
        const Vector<T> coord = shapeCenter + xy;

        // Check if coordinates are in range
        if (coord.inRange(minS, maxS))
            *out++ = coord;
    };

    const auto radiusSquare = radius * radius;
    const auto twoSquare = 2 * radiusSquare;

    Vector<Ts> xy(radius.getX(), 0);
    Vector<Ts> change(
        radiusSquare.getY() * (1 - 2 * radius.getX()),
        radiusSquare.getX()
    );

    Ts error = 0;
    Vector<Ts> stopping(twoSquare.getY() * radius.getX(), 0);

    while (stopping.getX() >= stopping.getY())
    {
        putCoord(xy);
        putCoord(xy * Vector<Ts>(-1,  1));
        putCoord(xy * Vector<Ts>(-1, -1));
        putCoord(xy * Vector<Ts>( 1, -1));

        ++xy.getY();
        stopping.getY() += twoSquare.getX();
        error += change.getY();
        change.getY() += twoSquare.getX();

        if ((2 * error + change.getX()) > 0)
        {
            --xy.getX();
            stopping.getX() -= twoSquare.getY();
            error += change.getX();
            change.getX() += twoSquare.getY();
        }
    }

    xy = Vector<Ts>(0, radius.getY());
    change = Vector<Ts>(
        radiusSquare.getY(),
        radiusSquare.getX() * (1 - 2 * radius.getY())
    );

    error = 0;
    stopping = Vector<Ts>(0, twoSquare.getY() * radius.getY());

    while (stopping.getX() <= stopping.getY())
    {
        putCoord(xy);
        putCoord(xy * Vector<Ts>(-1,  1));
        putCoord(xy * Vector<Ts>(-1, -1));
        putCoord(xy * Vector<Ts>( 1, -1));

        ++xy.getX();
        stopping.getX() += twoSquare.getY();
        error += change.getX();
        change.getX() += twoSquare.getY();

        if ((2 * error + change.getY()) > 0)
        {
            --xy.getY();
            stopping.getY() -= twoSquare.getX();
            error += change.getY();
            change.getY() += twoSquare.getX();
        }
    }

    return out;
}

/* ************************************************************************ */

}

/* ************************************************************************ */
