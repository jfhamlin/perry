#ifndef _MATHPOLYLINE_H_
#define _MATHPOLYLINE_H_

//! \file Polyline.h
//! Contains polyline curve implementation.

#include "Math/Curve.h"
#include "Math/Vector.h"
#include <algorithm>
#include <limits>
#include <vector>

// Windows header defines this macro,
// trouncing numeric_limits' definition
#undef max

namespace romulus
{
namespace math
{
//! Polyline Curve implementation.
class Polyline : public Curve
{
public:

    Polyline(const Curve& c, uint_t numSamples):
        m_minChangedIndex(std::numeric_limits<uint_t>::max())
    {
        for (uint_t i = 0; i < numSamples; ++i)
        {
            real_t t = real_t(i) / real_t(numSamples - 1);
            PushBack(c.Sample(t));
        }
    }

    Polyline(): m_minChangedIndex(std::numeric_limits<uint_t>::max()) { }
    virtual ~Polyline() { }

    inline const Vector3& operator[](int i) const { return m_points[i]; }
    inline Vector3& operator[](int i)
    {
        m_minChangedIndex = Min(m_minChangedIndex, static_cast<uint_t>(i));
        return m_points[i];
    }
    inline uint_t Size() const { return m_points.size(); }
    inline void PushBack(const Vector3& p);

    virtual Vector3 Begin() const { return m_points[0]; }
    virtual Vector3 End() const { return m_points[m_points.size() - 1]; }
    inline virtual Vector3 Sample(real_t t) const;

    virtual Vector3 Tangent(real_t t) const;

    virtual Vector3 Binormal(real_t t) const;

    inline Polyline& operator=(const Polyline& other)
    {
        m_points.clear();
        m_points.reserve(other.m_points.size());
        m_points.insert(m_points.begin(),
                        other.m_points.begin(), other.m_points.end());
        return *this;
    }

private:

    inline void SelectRepresentativeIndices(
            uint_t index, uint_t& i0, uint_t& i1, uint_t& i2) const
    {
        ASSERT(Size() >= 3);
        ASSERT(index > 0);
        if ((index + 1) >= Size() )
        {
            i0 = index - 2;
            i1 = index - 1;
            i2 = index;
        }
        else
        {
            i0 = index - 1;
            i1 = index;
            i2 = index + 1;
        }
    }

    inline void UpdateParamValues();

    std::vector<Vector3> m_points;
    std::vector<real_t> m_paramValues;
    uint_t m_minChangedIndex;
};

inline void Polyline::PushBack(const Vector3& p)
{
    m_minChangedIndex = Min(m_minChangedIndex,
                            static_cast<uint_t>(m_points.size()));
    m_points.push_back(p);
    m_paramValues.push_back(1.0);
}

inline Vector3 Polyline::Sample(real_t t) const
{
    if (m_minChangedIndex < m_points.size())
        const_cast<Polyline*>(this)->UpdateParamValues();

    if (t > 1.0 || t < 0.0)
        t = fmod(t, real_t(1.0));
    std::vector<real_t>::const_iterator bound =
            std::lower_bound(m_paramValues.begin(), m_paramValues.end(), t);
    uint_t index = distance(m_paramValues.begin(), bound);
    if (index == 0)
        return m_points[0];

    real_t fraction = (t - m_paramValues[index - 1]) /
            (m_paramValues[index] - m_paramValues[index - 1]);

    return m_points[index - 1] +
            fraction * (m_points[index] - m_points[index - 1]);
}

inline Vector3 Polyline::Tangent(real_t t) const
{
    if (m_minChangedIndex < m_points.size())
        const_cast<Polyline*>(this)->UpdateParamValues();

    std::vector<real_t>::const_iterator bound =
            std::lower_bound(m_paramValues.begin(), m_paramValues.end(), t);
    uint_t index = distance(m_paramValues.begin(), bound);

    if (Size() == 2 || index == 0)
        return math::Normal(m_points[1] - m_points[0]);
    else if (Size() <= 1)
        return Vector3(0.0, 0.0, 0.0);

    uint_t i0, i1, i2;
    SelectRepresentativeIndices(index, i0, i1, i2);
    const Vector3& v0 = m_points[i0];
    const Vector3& v1 = m_points[i1];
    const Vector3& v2 = m_points[i2];
    return math::Normal(math::Normal(v1 - v0) + math::Normal(v2 - v1));
}

inline Vector3 Polyline::Binormal(real_t t) const
{
    if (m_minChangedIndex < m_points.size())
        const_cast<Polyline*>(this)->UpdateParamValues();

    std::vector<real_t>::const_iterator bound =
            std::lower_bound(m_paramValues.begin(), m_paramValues.end(), t);
    uint_t index = distance(m_paramValues.begin(), bound);

    if (Size() <= 2)
        return Vector3(0.0, 0.0, 0.0);

    uint_t i0, i1, i2;
    if (index == 0)
    {
        i0 = index;
        i1 = i0 + 1;
        i2 = i1 + 1;
    }
    else
    {
        SelectRepresentativeIndices(index, i0, i1, i2);
    }
    const Vector3& v0 = m_points[i0];
    const Vector3& v1 = m_points[i1];
    const Vector3& v2 = m_points[i2];
    return math::Normal(Cross(v2 - v1, v1 - v0));
}

inline void Polyline::UpdateParamValues()
{
    real_t length = 0.0;
    for (uint_t i = 1; i < m_points.size(); ++i)
        length += Magnitude(m_points[i] - m_points[i - 1]);
    m_paramValues[0] = 0;
    for (uint_t i = 1; i < m_paramValues.size() - 1; ++i)
    {
        m_paramValues[i] = m_paramValues[i - 1] +
                Magnitude(m_points[i] - m_points[i - 1]) / length;
    }
    m_paramValues[m_paramValues.size() - 1] = 1.0;

    m_minChangedIndex = std::numeric_limits<uint_t>::max();
}

}
}

#endif // _MATHPOLYLINE_H_
