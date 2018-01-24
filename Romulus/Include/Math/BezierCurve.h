#ifndef _MATHBEZIERCURVE_H_
#define _MATHBEZIERCURVE_H_

//! \file BezierCurve.h
//! Contains generalized bezier curve implementation.

#include "Math/Curve.h"
#include "Math/Vector.h"
#include "Math/Utilities.h"
#include <vector>
#include <boost/scoped_array.hpp>

namespace romulus
{
namespace math
{

//! Bezier curve Curve implementation.
class BezierCurve : public Curve
{
public:

    BezierCurve(uint_t degree = 3): m_cps(degree + 1) { }
    virtual ~BezierCurve() { }

    void PushBack(const Vector3& cp) { m_cps.push_back(cp); }
    void Resize(uint_t size) { m_cps.resize(size); }
    uint_t Size() const { return m_cps.size(); }
    uint_t Degree() const { return Size() - 1; }

    inline const Vector3& operator[](int i) const { return m_cps[i]; }
    inline Vector3& operator[](int i) { return m_cps[i]; }

    inline virtual Vector3 Begin() const { return m_cps[0]; }
    inline virtual Vector3 End() const { return m_cps[Degree()]; }

    inline virtual Vector3 Sample(real_t t) const;

private:

    std::vector<Vector3> m_cps;
};

inline Vector3 BezierCurve::Sample(real_t t) const
{
    if (t > 1.0 || t < 0.0)
        t = fmod(t, real_t(1.0));
    if (t < 0.0)
        t = t + 1.0;

    if (Degree() == 0) return m_cps[0];

    boost::scoped_array<Vector3> cps(new Vector3[Degree()]);
    for (uint_t i = 1; i < m_cps.size(); ++i)
        cps[i - 1] = m_cps[i - 1] + t * (m_cps[i] - m_cps[i - 1]);

    for (uint_t degree = Degree() - 1; degree > 0; --degree)
        for (uint_t i = 1; i <= degree; ++i)
            cps[i - 1] = cps[i - 1] + t * (cps[i] - cps[i - 1]);

    return cps[0];
}

} // namespace math
} // namespace romulus


#endif // _MATHBEZIERCURVE_H_
