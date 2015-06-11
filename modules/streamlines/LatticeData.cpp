
/* ************************************************************************ */

// Declaration
#include "LatticeData.hpp"

// C++
#include <cassert>

/* ************************************************************************ */

namespace module {
namespace streamlines {

/* ************************************************************************ */

constexpr StaticArray<LatticeData::ValueType, LatticeData::SIZE> LatticeData::DIRECTION_WEIGHTS;

/* ************************************************************************ */

constexpr StaticArray<Vector<int>, LatticeData::SIZE> LatticeData::DIRECTION_VELOCITIES;

/* ************************************************************************ */

constexpr StaticArray<LatticeData::IndexType, LatticeData::SIZE> LatticeData::DIRECTION_OPPOSITES;

/* ************************************************************************ */

void LatticeData::collide(ValueType omega)
{
    if (isObstacle())
    {
        StaticArray<ValueType, SIZE> temp;

        // Move updated values into opposite directions
        for (IndexType i = 0; i < SIZE; ++i)
        {
            temp[i] = m_values[DIRECTION_OPPOSITES[i]];
        }

        // Copy updated values
        m_values = temp;
    }
    else
    {
        // Calculate
        const auto feq = LatticeData::calcEquilibrium(calcVelocityNormalized(), calcRho());

        // Update values
        for (IndexType alpha = 0; alpha < SIZE; ++alpha)
        {
            m_values[alpha] = -omega * (m_values[alpha] - feq[alpha]) + m_values[alpha];
        }
    }
}

/* ************************************************************************ */

StaticArray<LatticeData::ValueType, LatticeData::SIZE>
LatticeData::calcEquilibrium(const Vector<ValueType>& u, ValueType rho) NOEXCEPT
{
    StaticArray<ValueType, SIZE> res;

    for (IndexType alpha = 0; alpha < SIZE; ++alpha)
    {
        const auto weight = DIRECTION_WEIGHTS[alpha];
        const auto velocity = DIRECTION_VELOCITIES[alpha];
        const auto vu = dot(velocity, u);

        res[alpha] = rho * weight * (
              1.f
            + 3.f * vu
            + 9.f / 2.f * vu * vu
            - 3.f / 2.f * u.getLengthSquared()
        );
        assert(res[alpha] >= 0);
    }

    return res;
}

/* ************************************************************************ */

}
}

/* ************************************************************************ */
