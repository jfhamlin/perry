#ifndef _MATHSPHERE_H_
#define _MATHSPHERE_H_

//! \file Sphere.h
//! Contains Sphere class definition

#include "Math/Vector.h"

namespace romulus
{
namespace math
{
//! Sphere representation.
class Sphere
{
public:

    inline Sphere() { }
    inline Sphere(const Vector3& center, real_t radius)
        : m_center(center), m_radius(radius) {}
    template <typename Iterator>
    inline Sphere(Iterator startPoints, Iterator endPoints);

    inline const Vector3& Center() const;
    inline void SetCenter(const Vector3& center);

    inline real_t Radius() const;
    inline void SetRadius(real_t radius);

    inline bool Contains(const Vector3& point) const;

    inline real_t Volume() const;

private:

    Vector3 m_center;

    real_t m_radius;
};

inline bool operator==(const Sphere& lhs, const Sphere& rhs);
inline bool operator!=(const Sphere& lhs, const Sphere& rhs);

}
}

#include "Math/Sphere.inl"

#endif // _MATHSPHERE_H_
