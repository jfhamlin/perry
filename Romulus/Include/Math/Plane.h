#ifndef _MATHPLANE_H_
#define _MATHPLANE_H_

//! \file Plane.h
//! Contains Plane class definition

#include <cmath>

#include "Math/Constants.h"
#include "Math/Vector.h"

namespace romulus
{
namespace math
{
//! Plane representation.
class Plane
{
public:

    enum PlaneSide
    {
        PlaneSide_BackOfPlane = -1,
        PlaneSide_OnPlane = 0,
        PlaneSide_FrontOfPlane = 1
    };

    //! Default Plane ctor.
    //! Sets the plane to the x-z plane at y=0
    inline Plane(): m_abc(Vector3(0, 1, 0)), m_d(0) {}

    //! Plane ctor
    //! \param a,b,c,d - Coefficients of the plane equation ax+by+cz=d;
    //!                  the coefficients are normalized by the ctor.
    inline Plane(const real_t a, const real_t b, const real_t c, const real_t d);

    //! ctor
    //! \param normal - The direction in which the plane is facing.
    //! \param distanceFromOrigin - The distance from the origin to the plane along normal.
    inline Plane(const Vector3& normal, const real_t distanceFromOrigin);

    //! ctor
    //! \param p1, p2, p3 - Points lying on the plane in counter-clockwise order.
    inline Plane(const Vector3& p1, const Vector3& p2, const Vector3& p3);

    inline Plane& operator=(const Plane& rhs);

    inline const Vector3& Normal() const;
    inline void SetNormal(const Vector3& normal);

    inline real_t DistanceFromOrigin() const;
    inline void SetDistanceFromOrigin(const real_t distanceFromOrigin);

    //! Find the distance from the plane to a point.
    //! \param point - point to find distance from
    //! \return The distance from the plane to the point.
    //!         Distance is positive if the point is on the front side of the plane
    //!         Distance is negative if the point is on the back side of the plane
    inline real_t Distance(const Vector3& point) const;

    //! Find which side of the plane a point is on.
    //! \param point - point to determine the side of
    //! \return The element of the PlaneSide enum corresponding
    //!         to the side of the plane point is on
    inline PlaneSide WhichSide(const Vector3& point) const;

private:

    Vector3 m_abc;

    real_t m_d;
};

/* Plane member functions */

Plane::Plane(const real_t a, const real_t b, const real_t c, const real_t d)
{
    m_abc = Vector3(a, b, c);
    real_t mag = Magnitude(m_abc);
    m_abc /= mag;
    m_d = d / mag;
}

Plane::Plane(const Vector3& normal, const real_t distanceFromOrigin)
{
    m_abc = normal;
    m_d = distanceFromOrigin;
}

Plane::Plane(const Vector3& p1, const Vector3& p2, const Vector3& p3)
{
    m_abc = romulus::math::Normal(Cross(p2 - p1, p3 - p2));
    m_d = Dot(m_abc, p1);
}

Plane& Plane::operator=(const Plane& rhs)
{
    m_abc = rhs.m_abc;
    m_d = rhs.m_d;
    return *this;
}

const Vector3& Plane::Normal() const
{
    return m_abc;
}

void Plane::SetNormal(const Vector3& normal)
{
    m_abc = normal;
}


real_t Plane::DistanceFromOrigin() const
{
    return m_d;
}

void Plane::SetDistanceFromOrigin(const real_t distanceFromOrigin)
{
    m_d = distanceFromOrigin;
}

real_t Plane::Distance(const Vector3& point) const
{
    return Dot(m_abc, point) - m_d;
}

Plane::PlaneSide Plane::WhichSide(const Vector3& point) const
{
    real_t distToPlane = Distance(point);
    if (ApproxEqual(distToPlane, static_cast<real_t>(0.0)))
        return Plane::PlaneSide_OnPlane;

    if (distToPlane > static_cast<real_t>(0.0))
        return Plane::PlaneSide_FrontOfPlane;

    return Plane::PlaneSide_BackOfPlane;
}

/* Plane equality operator */

inline bool operator==(const Plane& lhs, const Plane& rhs)
{
    if (lhs.Normal() != rhs.Normal() ||
        !ApproxEqual(lhs.DistanceFromOrigin(),
                     rhs.DistanceFromOrigin()))
        return false;
    return true;
}

inline bool operator!=(const Plane& lhs, const Plane& rhs)
{
    return !(lhs == rhs);
}

inline Plane& Normalize(Plane& plane)
{
    const Vector3& normal = plane.Normal();
    real_t distance = plane.DistanceFromOrigin();

    real_t magnitudeSquared = Dot(normal, normal);

    real_t inverseMagnitude = 1.f/ sqrtf(magnitudeSquared);
    plane.SetNormal(normal * inverseMagnitude);
    plane.SetDistanceFromOrigin(distance * inverseMagnitude);

    return plane;
}

}
}

#endif // _MATHPLANE_H_
