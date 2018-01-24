#include "Math/Utilities.h"

namespace romulus
{
namespace math
{

inline Quaternion::Quaternion()
{
}

inline Quaternion::Quaternion(real_t w, real_t x, real_t y, real_t z)
{
    m_data[0] = w;
    m_data[1] = x;
    m_data[2] = y;
    m_data[3] = z;
}

inline Quaternion::Quaternion(const real_t data[])
{
    for (int i = 0; i < 4; ++i)
        m_data[i] = data[i];
}

inline Quaternion& Quaternion::operator=(const Quaternion& rhs)
{
    m_data = rhs.m_data;

    return *this;
}

inline const real_t& Quaternion::operator[](int index) const
{
    return m_data[index];
}

inline real_t& Quaternion::operator[](int index)
{
    return m_data[index];
}

bool operator==(const Quaternion& lhs, const Quaternion& rhs)
{
    return lhs.Vector() == rhs.Vector();
}

bool operator!=(const Quaternion& lhs, const Quaternion& rhs)
{
    return !(lhs == rhs);
}

Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs)
{
    return Quaternion(
        (lhs[0]*rhs[0]) - (lhs[1]*rhs[1]) - (lhs[2]*rhs[2]) - (lhs[3]*rhs[3]),
        (lhs[0]*rhs[1]) + (lhs[1]*rhs[0]) + (lhs[2]*rhs[3]) - (lhs[3]*rhs[2]),
        (lhs[0]*rhs[2]) + (lhs[2]*rhs[0]) + (lhs[3]*rhs[1]) - (lhs[1]*rhs[3]),
        (lhs[0]*rhs[3]) + (lhs[3]*rhs[0]) + (lhs[1]*rhs[2]) - (lhs[2]*rhs[1]));
}

Quaternion Conjugate(const Quaternion& quat)
{
    return Quaternion(quat[0], -quat[1], -quat[2], -quat[3]);
}

Quaternion Inverse(const Quaternion& quat)
{
    return Conjugate(Normal(quat));
}

real_t Magnitude(const Quaternion& quat)
{
    return Magnitude(quat.Vector());
}

Quaternion Normal(const Quaternion& quat)
{
    real_t magnitude = Magnitude(quat);
    Quaternion result;
    ASSERT(!ApproxEqual(magnitude, 0.0));
    real_t oneOverMag = 1.0 / magnitude;
    for (int i = 0; i < 4; ++i)
        result[i] = oneOverMag * quat[i];

    return result;
}

Quaternion& Normalize(Quaternion& quat)
{
    real_t magnitude = Magnitude(quat);
    ASSERT(!ApproxEqual(magnitude, 0.0));
    real_t oneOverMag = 1.0 / magnitude;
    for (int i = 0; i < 4; ++i)
        quat[i] *= oneOverMag;
    return quat;
}

}
}
