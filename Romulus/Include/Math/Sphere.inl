#ifndef _MATHSPHERE_INL_
#define _MATHSPHERE_INL_

#include "Math/Constants.h"
#include "Math/Utilities.h"

namespace romulus
{
namespace math
{

inline bool operator==(const Sphere& lhs, const Sphere& rhs)
{
    if (lhs.Center() != rhs.Center() ||
        !ApproxEqual(lhs.Radius(), rhs.Radius()))
        return false;
    return true;
}

inline bool operator!=(const Sphere& lhs, const Sphere& rhs)
{
    return !(lhs == rhs);
}

/* Sphere method implementations */

template <typename Iterator>
Sphere::Sphere(Iterator startPoints, Iterator endPoints)
{
    //! \todo Perhaps implement an optimal-bounds-producing algorithm.
    //!       For now, just the minumal sphere whose center is the average.

    real_t count = 0.f;
    m_center = Vector3(0.f);
    for (Iterator point = startPoints; point != endPoints; ++point, ++count)
        m_center += *point;
    m_center *= (1.f / count);

    real_t maxDistSquared = 0.f;
    for (; startPoints != endPoints; ++startPoints)
        maxDistSquared = Max(maxDistSquared,
                             MagnitudeSquared(*startPoints - m_center));
    m_radius = sqrt(maxDistSquared);
}

const Vector3& Sphere::Center() const
{
    return m_center;
}

void Sphere::SetCenter(const Vector3& center)
{
    m_center = center;
}

real_t Sphere::Radius() const
{
    return m_radius;
}

void Sphere::SetRadius(real_t radius)
{
    m_radius = radius;
}

bool Sphere::Contains(const Vector3& point) const
{
    return MagnitudeSquared(point - m_center) < m_radius * m_radius;
}

real_t Sphere::Volume() const
{
    return Pi * m_radius * m_radius;
}

}
}

#endif // _MATHSPHERE_INL_
