#ifndef _MATHAXISALIGNEDBOX_H_
#define _MATHAXISALIGNEDBOX_H_

//! \file AxisAlignedBox.h
//! AxisAlignedBox class definition

#include "Utility/Assertions.h"
#include "Math/Constants.h"
#include "Math/Utilities.h"
#include "Math/Vector.h"
#include <cmath>

namespace romulus
{
namespace math
{

class AxisAlignedBox
{
public:

    //! AxisAlignedBox ctor.
    inline AxisAlignedBox();
    //! AxisAlignedBox ctor.
    inline AxisAlignedBox(const Vector3& minCorner, const Vector3& maxCorner);
    //! AxisAlignedBox ctor.
    inline AxisAlignedBox(real_t minx, real_t miny, real_t minz,
                          real_t maxx, real_t maxy, real_t maxz);
    //! AxisAlignedBox ctor.
    template <typename Iterator>
    inline AxisAlignedBox(Iterator startPoints, Iterator endPoints);

    inline real_t MinX() const;
    inline void SetMinX(const real_t minx);

    inline real_t MinY() const;
    inline void SetMinY(const real_t miny);

    inline real_t MinZ() const;
    inline void SetMinZ(const real_t minz);

    inline real_t MaxX() const;
    inline void SetMaxX(const real_t maxx);

    inline real_t MaxY() const;
    inline void SetMaxY(const real_t maxy);

    inline real_t MaxZ() const;
    inline void SetMaxZ(const real_t maxz);

    inline const Vector3& MinCorner() const;
    inline void SetMinCorner(const Vector3& min);

    inline const Vector3& MaxCorner() const;
    inline void SetMaxCorner(const Vector3& max);

    inline Vector3 Corner(ushort_t x, ushort_t y, ushort_t z) const;
    inline Vector3 Corner(ushort_t corner) const;

    //! Expands the AxisAlignedBox to contain the point.
    //! \param point - A point to be contained by the AxisAlignedBox
    //! \return void
    inline void GrowToContain(const Vector3& point);

    //! Finds the volume of the AxisAlignedBox
    //! \return The volume of the AxisAlignedBox.
    inline real_t Volume() const;

    //! Finds the center of the AxisAlignedBox
    //! \return The center of the AxisAlignedBox.
    inline Vector3 Center() const;

    //! Moves each plane of the AxisAlignedBox outward to expand the
    //! AxisAlignedBox by delta.
    //! \param delta - The distance to move each plane of the box outward.
    //! \return void
    inline void Expand(real_t delta);

    //! Determines whether a point is contained by the AxisAlignedBox.
    //! \param point - the point to test.
    //! \return true if the point is within the bounds, false otherwise.
    inline bool Contains(const Vector3& point) const;

private:

    Vector3 m_minCorner;

    Vector3 m_maxCorner;
};

/* Overloaded equality operator */

inline bool operator==(const AxisAlignedBox& lhs, const AxisAlignedBox& rhs)
{
    if (!ApproxEqual(lhs.MinX(), rhs.MinX()) ||
        !ApproxEqual(lhs.MaxX(), rhs.MaxX()))
        return false;
    if (!ApproxEqual(lhs.MinY(), rhs.MinY()) ||
        !ApproxEqual(lhs.MaxY(), rhs.MaxY()))
        return false;
    if (!ApproxEqual(lhs.MinZ(), rhs.MinZ()) ||
        !ApproxEqual(lhs.MaxZ(), rhs.MaxZ()))
        return false;
    return true;
}

inline bool operator!=(const AxisAlignedBox& lhs, const AxisAlignedBox& rhs)
{
    return !(lhs == rhs);
}

/* AxisAlignedBox method implementations */

AxisAlignedBox::AxisAlignedBox():
    m_minCorner(0, 0, 0), m_maxCorner(0, 0, 0)
{
}

AxisAlignedBox::AxisAlignedBox(const Vector3& minCorner,
                               const Vector3& maxCorner)
{
    m_minCorner = minCorner;
    m_maxCorner = maxCorner;

    m_minCorner[0] = Min(minCorner[0], maxCorner[0]);

    m_minCorner[1] = Min(minCorner[1], maxCorner[1]);

    m_minCorner[2] = Min(minCorner[2], maxCorner[2]);
}

AxisAlignedBox::AxisAlignedBox(real_t minx, real_t miny, real_t minz,
                               real_t maxx, real_t maxy, real_t maxz)
{
    m_maxCorner[0] = maxx;
    m_minCorner[0] = Min(minx, maxx);

    m_maxCorner[1] = maxy;
    m_minCorner[1] = Min(miny, maxy);

    m_maxCorner[2] = maxz;
    m_minCorner[2] = Min(minz, maxz);
}

template <typename Iterator>
AxisAlignedBox::AxisAlignedBox(Iterator startPoints, Iterator endPoints)
{
    if (startPoints != endPoints)
    {
        m_minCorner = m_maxCorner = *startPoints;
        for (++startPoints; startPoints != endPoints; ++startPoints) {
            Vector3 point = *startPoints;
            for (int j = 0; j < 3; ++j)
                if (point[j] < m_minCorner[j])
                    m_minCorner[j] = point[j];
                else if (point[j] > m_maxCorner[j])
                    m_maxCorner[j] = point[j];
        }
    }
}

real_t AxisAlignedBox::MinX() const
{
    return m_minCorner[0];
}

void AxisAlignedBox::SetMinX(const real_t minx)
{
    m_minCorner[0] = minx;
    m_maxCorner[0] = Max(minx, m_maxCorner[0]);
}

real_t AxisAlignedBox::MinY() const
{
    return m_minCorner[1];
}

void AxisAlignedBox::SetMinY(const real_t miny)
{
    m_minCorner[1] = miny;
    m_maxCorner[1] = Max(miny, m_maxCorner[1]);
}

real_t AxisAlignedBox::MinZ() const
{
    return m_minCorner[2];
}

void AxisAlignedBox::SetMinZ(const real_t minz)
{
    m_minCorner[2] = minz;
    m_maxCorner[2] = Max(minz, m_maxCorner[2]);
}

real_t AxisAlignedBox::MaxX() const
{
    return m_maxCorner[0];
}

void AxisAlignedBox::SetMaxX(const real_t maxx)
{
    m_maxCorner[0] = maxx;
    m_minCorner[0] = Min(maxx, m_minCorner[0]);
}

real_t AxisAlignedBox::MaxY() const
{
    return m_maxCorner[1];
}

void AxisAlignedBox::SetMaxY(const real_t maxy)
{
    m_maxCorner[1] = maxy;
    m_minCorner[1] = Min(maxy, m_minCorner[1]);
}

real_t AxisAlignedBox::MaxZ() const
{
    return m_maxCorner[2];
}

void AxisAlignedBox::SetMaxZ(const real_t maxz)
{
    m_maxCorner[2] = maxz;
    m_minCorner[2] = Min(maxz, m_minCorner[2]);
}

const Vector3& AxisAlignedBox::MinCorner() const
{
    return m_minCorner;
}

void AxisAlignedBox::SetMinCorner(const Vector3& min)
{
    m_minCorner = min;
    m_maxCorner[0] = Max(min[0], m_maxCorner[0]);
    m_maxCorner[1] = Max(min[1], m_maxCorner[1]);
    m_maxCorner[2] = Max(min[2], m_maxCorner[2]);
}

const Vector3& AxisAlignedBox::MaxCorner() const
{
    return m_maxCorner;
}

void AxisAlignedBox::SetMaxCorner(const Vector3& max)
{
    m_maxCorner = max;
    m_minCorner[0] = Min(max[0], m_minCorner[0]);
    m_minCorner[1] = Min(max[1], m_minCorner[1]);
    m_minCorner[2] = Min(max[2], m_minCorner[2]);
}

Vector3 AxisAlignedBox::Corner(ushort_t corner) const
{
    ASSERT(corner < 8u);
    return Vector3(corner & 0x4 ? MinX() : MaxX(),
                   corner & 0x2 ? MinY() : MaxY(),
                   corner & 0x1 ? MinZ() : MaxZ());
}

Vector3 AxisAlignedBox::Corner(ushort_t x, ushort_t y, ushort_t z) const
{
    ASSERT(x < 2);
    ASSERT(y < 2);
    ASSERT(z < 2);
    return Vector3(x ? MaxX() : MinX(),
                   y ? MaxY() : MinY(),
                   z ? MaxZ() : MinZ());
}

void AxisAlignedBox::GrowToContain(const Vector3& point)
{
    for (int i = 0; i < 3; ++i)
        if (point[i] < m_minCorner[i])
            m_minCorner[i] = point[i];
        else if (point[i] > m_maxCorner[i])
            m_maxCorner[i] = point[i];
}

real_t AxisAlignedBox::Volume() const
{
    return (MaxX() - MinX()) * (MaxY() - MinY()) * (MaxZ() - MinZ());
}

Vector3 AxisAlignedBox::Center() const
{
    return Vector3((MaxX() + MinX()) / 2.0f,
                   (MaxY() + MinY()) / 2.0f,
                   (MaxZ() + MinZ()) / 2.0f);
}

void AxisAlignedBox::Expand(real_t delta)
{
    for (int i = 0; i < 3; ++i)
        m_minCorner[i] = Min(m_minCorner[i] - delta, m_maxCorner[i]);
    for (int i = 0; i < 3; ++i)
        m_maxCorner[i] = Max(m_maxCorner[i] + delta, m_minCorner[i]);
}

bool AxisAlignedBox::Contains(const Vector3 &point) const
{
    if (point[0] < m_minCorner[0] ||
        point[0] > m_maxCorner[0] ||
        point[1] < m_minCorner[1] ||
        point[1] > m_maxCorner[1] ||
        point[2] < m_minCorner[2] ||
        point[2] > m_maxCorner[2])
        return false;
    return true;
}
}
}

#endif // _MATHAXISALIGNEDBOX_H_
