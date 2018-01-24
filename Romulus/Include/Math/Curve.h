#ifndef _MATHCURVE_H_
#define _MATHCURVE_H_

//! \file Curve.h
//! Contains curve interface declaration.

#include "Math/Constants.h"
#include "Math/Vector.h"

namespace romulus
{
namespace math
{
//! Curve interface.
class Curve
{
public:

    Curve() { }
    virtual ~Curve() { }

    virtual Vector3 Begin() const { return Sample(0.0); }
    virtual Vector3 End() const { return Sample(1.0); }

    //! Sample the curve.
    virtual Vector3 Sample(real_t t) const = 0;

    //! Default implementations do an approximation based on Sample().
    virtual Vector3 Tangent(real_t t) const
    {
        real_t t0 = t, t1 = t, t2 = t; // Initialized to avoid warnings.
        SelectRepresentativePoints(t, t0, t1, t2);

        Vector3 v0 = Sample(t0), v1 = Sample(t1), v2 = Sample(t2);
        return math::Normal(math::Normal(v1 - v0) + math::Normal(v2 - v1));
    }

    virtual Vector3 Binormal(real_t t) const
    {
        real_t t0 = t, t1 = t, t2 = t; // Initialized to avoid warnings.
        SelectRepresentativePoints(t, t0, t1, t2);

        Vector3 v0 = Sample(t0), v1 = Sample(t1), v2 = Sample(t2);
        return math::Normal(Cross(v2 - v1, v1 - v0));
    }

    virtual Vector3 Normal(real_t t) const
    {
        return Cross(Binormal(t), Tangent(t));
    }

private:

    void SelectRepresentativePoints(real_t t,
                                    real_t& t0, real_t& t1, real_t& t2) const
    {
        if (t > 1.0 || t < 0.0)
            t = fmod(t, real_t(1.0));

        t1 = t;
        t0 = t1 - 2.0 * 0.001;
        t2 = t1 + 2.0 * 0.001;
        if (t0 < 0.0)
        {
            std::swap(t0, t1);
            std::swap(t1, t2);
            t2 = t1 + 2.0 * 0.001;
        }
        else if (t2 > 1.0)
        {
            std::swap(t1, t2);
            std::swap(t0, t1);
            t0 = t1 - 2.0 * 0.001;
        }
    }
};
}
}

#endif // _MATHCURVE_H_
