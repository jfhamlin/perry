#ifndef _MATHQUATERNION_H_
#define _MATHQUATERNION_H_

//! \file Quaternion.h
//! Contains the quaternion type declaration.

#include "Core/Types.h"
#include "Math/Vector.h"

namespace romulus
{
namespace math
{

class Quaternion
{
public:

    inline Quaternion();
    inline Quaternion(real_t w, real_t x, real_t y, real_t z);
    inline explicit Quaternion(const real_t data[]);

    inline Quaternion& operator=(const Quaternion& rhs);

    inline const real_t& operator[](int index) const;
    inline real_t& operator[](int index);

    inline const Vector4& Vector() const { return m_data; }

private:

    Vector4 m_data;
};

inline bool operator==(const Quaternion& lhs, const Quaternion& rhs);
inline bool operator!=(const Quaternion& lhs, const Quaternion& rhs);

inline Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs);
inline Quaternion Conjugate(const Quaternion& quat);
inline Quaternion Inverse(const Quaternion& quat);

inline real_t Magnitude(const Quaternion& quat);
inline Quaternion Normal(const Quaternion& quat);
inline Quaternion& Normalize(Quaternion& quat);

}
}

#include "Math/Quaternion.inl"

#endif // _MATHQUATERNION_H_
