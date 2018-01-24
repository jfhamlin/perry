#ifndef _MATHBSPLINECURVE_H_
#define _MATHBSPLINECURVE_H_

//! \file BSplineCurve.h
//! Contains generalized B-Spline curve implementation.

#include "Math/Curve.h"
#include "Math/Vector.h"
#include "Math/Utilities.h"
#include <boost/scoped_array.hpp>
#include <cmath>

namespace romulus
{
namespace math
{

class BSplineCurve : public Curve
{
public:

    BSplineCurve(): m_degree(3), m_closed(false) { }
    virtual ~BSplineCurve() { }

    inline uint_t NumControlPoints() const { return m_cps.size(); }
    inline const Vector3& operator[](int i) const { return m_cps[i]; }
    inline Vector3& operator[](int i) { return m_cps[i]; }

    inline uint_t Size() const { return m_cps.size(); }
    inline uint_t Degree() const { return m_degree; }
    inline void SetDegree(uint_t degree) { m_degree = degree; }
    inline void PushBack(const Vector3& p) { m_cps.push_back(p); }
    inline bool IsClosed() const { return m_closed; }
    inline void SetClosed(bool c) { m_closed = c; }

    inline virtual Vector3 Begin() const { return Sample(0); }
    inline virtual Vector3 End() const { return Sample(1.0); }

    inline virtual Vector3 Sample(real_t t) const;

private:

    std::vector<Vector3> m_cps;
    uint_t m_degree;
    bool m_closed;
};

inline Vector3 BSplineCurve::Sample(real_t t) const
{
    if (t > 1.0 || t < 0.0)
        t = fmod(t, real_t(1.0));
    if (t < 0.0)
        t = t + 1.0;

    const uint_t numCPs = m_closed ? m_cps.size() + m_degree: m_cps.size();
    // Adjust degree down as needed to get a curve,
    // if too few control points and not on closed loop
    uint_t degree = m_degree;
    if (degree >= numCPs)
        degree = numCPs - 1;

    // We map [0, 1] to the region of the parameter space
    // with full support.
    const real_t minSupport = degree;
    const real_t maxSupport = numCPs;
    t = minSupport + t * (maxSupport - minSupport);

    boost::scoped_array<Vector3> bases(new Vector3[degree + 1]);
    int k = static_cast<int>(t);

    for (uint_t i = 0; i <= degree; ++i)
    {
        ASSERT(k - degree >= 0);
        bases[i] = m_cps[(k - degree + i) % m_cps.size()];
    }

    for (uint_t power = 1; power <= degree; ++power)
    {
        for (uint_t i = 0; i <= degree - power; ++i)
        {
            ASSERT(k >= int(degree));
            int knot = k - degree + power + i;
            const real_t u_i = real_t(knot);
            const real_t u_ipr1 = real_t(knot + degree - power + 1);
            const real_t a = (t - u_i) / (u_ipr1 - u_i);
            bases[i] = (1.0 - a) * bases[i] + a * bases[i + 1];
        }
    }

    return bases[0];
}

}
}

#endif // _MATHBSPLINECURVE_H_
