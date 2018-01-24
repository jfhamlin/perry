#ifndef _MATHRECTANGLE_H_
#define _MATHRECTANGLE_H_

//! \file Rectangle.h
//! Contains Rectangle class definition.

#include "Math/Constants.h"
#include "Math/Vector.h"
#include "Math/Utilities.h"

namespace romulus
{
namespace math
{

class Rectangle
{
public:

    // Constructors.
    inline Rectangle(): m_origin(0, 0), m_extents(0, 0) { }
    inline Rectangle(const Vector2& origin, const Vector2& extents):
        m_origin(origin), m_extents(extents) { }
    inline Rectangle(const Rectangle& rhs):
        m_origin(rhs.Origin()), m_extents(rhs.Extents()) { }

    inline Rectangle& operator=(const Rectangle& rhs)
    {
        m_origin = rhs.Origin();
        m_extents = rhs.Extents();
        return *this;
    }

    //! \return The origin (upper left corner) of the rectangle, where the first
    //!         coordinate increases to rightward and the second coordinate
    //!         increases downward.
    inline const Vector2& Origin() const { return m_origin; }
    inline void SetOrigin(const Vector2& origin) { m_origin = origin; }

    inline real_t Width() const { return m_extents[0]; }
    inline void SetWidth(real_t width) { m_extents[0] = width; }

    inline real_t Height() const { return m_extents[1]; }
    inline void SetHeight(real_t height) { m_extents[1] = height; }

    inline const Vector2& Extents() const { return m_extents; }
    inline void SetExtents(const Vector2& extents) { m_extents = extents; }

private:

    Vector2 m_origin;
    Vector2 m_extents;
};

}
}

#endif // _MATHRECTANGLE_H_
