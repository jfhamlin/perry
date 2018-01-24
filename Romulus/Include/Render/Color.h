#ifndef _COLOR_H_
#define _COLOR_H_

//! \file Color.h
//! Contains Color class definition.

#include "Core/Types.h"

namespace romulus
{
namespace render
{

class Color
{
public:

    inline Color()
    {
        for (int i = 0; i < 4; ++i)
            m_channels[i] = 0;
    }
    inline Color(real_t red, real_t green, real_t blue, real_t alpha)
    {
        m_channels[0] = red;
        m_channels[1] = green;
        m_channels[2] = blue;
        m_channels[3] = alpha;
    }
    inline explicit Color(const real_t color[])
    {
        for (int i = 0; i < 4; ++i)
            m_channels[i] = color[i];
    }

    inline real_t& operator[](int index) { return m_channels[index]; }
    inline const real_t& operator[](int index) const { return m_channels[index]; }

    inline const real_t* Data() const { return &m_channels[0]; }
private:

    real_t m_channels[4];
};

inline bool operator==(const Color& c0, const Color& c1)
{
    return c0[0] == c1[0] &&
           c0[1] == c1[1] &&
           c0[2] == c1[2] &&
           c0[3] == c1[3];
}

inline bool operator!=(const Color& c0, const Color& c1)
{
    return !(c0 == c1);
}

inline Color operator*(real_t s, const Color& c)
{
    return Color(s * c[0], s * c[1], s * c[2], c[3]);
}
inline Color operator*(const Color& c, real_t s)
{
    return Color(s * c[0], s * c[1], s * c[2], c[3]);
}

inline Color& operator*=(real_t s, Color& c)
{
    c[0] *= s;
    c[1] *= s;
    c[2] *= s;
    return c;
}

}
}

#endif // _COLOR_H_
