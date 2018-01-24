#ifndef _MATHTORUSKNOT_H_
#define _MATHTORUSKNOT_H_

//! \file TorusKnot.h
//! Contains Torus Knot curve implementation.

#include "Math/Curve.h"
#include "Math/Vector.h"

namespace romulus
{
namespace math
{
//! TorusKnot Curve implementation.
class TorusKnot : public Curve
{
public:

    TorusKnot(uint_t p, uint_t q, real_t innerRadius, real_t outerRadius):
        m_p(p), m_q(q), m_innerRadius(innerRadius), m_outerRadius(outerRadius)
    {
        m_start = Vector3(m_outerRadius, 0, 0);
    }
    virtual ~TorusKnot() { }

    virtual Vector3 Begin() const { return m_start; }

    virtual Vector3 End() const { return m_start; }

    virtual Vector3 Sample(const real_t t) const
    {
        real_t theta, phi;
        theta = t * static_cast<real_t>(m_p) * 2.0 * Pi;
        phi = t * static_cast<real_t>(m_q) * 2.0 * Pi;
        real_t radius = 0.5 * (m_outerRadius - m_innerRadius);

        Vector3 v;
        v[0] = radius * cos(phi) + m_innerRadius + radius;
        v[1] = radius * sin(phi);
        v[2] = sin(theta) * v[0];
        v[0] = cos(theta) * v[0];
        return v;
    }

private:

    //! Number of revolutions around the large circle.
    uint_t m_p;
    //! Number of revolutions around the small circle.
    uint_t m_q;
    //! Distance from the center to the innermost part of the torus.
    real_t m_innerRadius;
    //! Distance from the center to the outermost part of the torus.
    real_t m_outerRadius;

    Vector3 m_start;
};

}
}

#endif // _MATHTORUSKNOT_H_
