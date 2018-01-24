#ifndef _MATHTRANSFORMATIONS_H_
#define _MATHTRANSFORMATIONS_H_

//! \file Transformations.h
//! Contains definitions for transformation classes useful for composing
//! transformations and producing transformation matrices.

#include "Math/Matrix.h"
#include "Math/Quaternion.h"
#include "Math/Vector.h"

namespace romulus
{
namespace math
{

//! Rotation class which represents a rotation as a quaternion.
class Rotation
{
public:
    inline Rotation() { }
    inline explicit Rotation(const math::Quaternion& rotation);
    inline Rotation(const Vector3& axis, real_t angle);

    inline const math::Quaternion& Quaternion() const { return m_rotation; }

    inline Vector3 Axis() const;
    inline real_t Angle() const;

    inline void SetAxisAngle(const Vector3& axis, real_t angle);

    inline Matrix44 Matrix() const;

    inline Rotation& operator=(const Rotation& r);

private:

    math::Quaternion m_rotation;
};

//! A Scale class representing a uniform scale.
class UniformScale
{
public:
    inline UniformScale() { }
    inline explicit UniformScale(real_t value);

    inline real_t Value() const { return m_value; }

    inline void SetValue(real_t s) { m_value = s; }

    inline Matrix44 Matrix() const;

    inline UniformScale& operator=(const UniformScale& s);

private:

    real_t m_value;
};

//! Translation class.
class Translation
{
public:
    inline Translation() { }
    inline explicit Translation(const Vector3& translation);
    inline Translation(real_t dx, real_t dy, real_t dz);

    inline const Vector3& Vector() const { return m_translation; }
    inline void SetVector(const Vector3& v) { m_translation = v; }

    inline real_t TranslateX() const { return m_translation[0]; }
    inline real_t TranslateY() const { return m_translation[1]; }
    inline real_t TranslateZ() const { return m_translation[2]; }

    inline void SetTranslateX(real_t dx) { m_translation[0] = dx; }
    inline void SetTranslateY(real_t dy) { m_translation[1] = dy; }
    inline void SetTranslateZ(real_t dz) { m_translation[2] = dz; }

    inline Matrix44 Matrix() const;

    inline Translation& operator=(const Translation& t);

private:

    Vector3 m_translation;
};

//! A transformation class, which represents a transformation as a unform scale
//! followed by a rotation followed by a translation.
class Transformation
{
public:

    inline Transformation():
        m_translation(0, 0, 0), m_rotation(Vector3(1, 0, 0), 0), m_scale(1) { }
    inline Transformation(const math::Translation& translation);
    inline Transformation(const math::Rotation& rotation);
    inline Transformation(const math::UniformScale& scale);
    inline Transformation(const math::Translation& translation,
                          const math::Rotation& rotation,
                          const math::UniformScale& scale);

    inline const math::Rotation& Rotation() const
    { return m_rotation; }
    inline const math::UniformScale& Scale() const
    { return m_scale; }
    inline const math::Translation& Translation() const
    { return m_translation; }

    inline math::Rotation& Rotation()
    { return m_rotation; }
    inline math::UniformScale& Scale()
    { return m_scale; }
    inline math::Translation& Translation()
    { return m_translation; }

    inline void SetRotation(const math::Rotation& rot)
    { m_rotation = rot; }
    inline void SetScale(const math::UniformScale& scale)
    { m_scale = scale; }
    inline void SetTranslation(const math::Translation& t)
    { m_translation = t; }

    inline Matrix44 Matrix() const;

    inline Transformation& operator=(const math::UniformScale& s);
    inline Transformation& operator=(const math::Translation& t);
    inline Transformation& operator=(const math::Rotation& r);
    inline Transformation& operator=(const Transformation& t);

private:

    math::Translation m_translation;
    math::Rotation m_rotation;
    math::UniformScale m_scale;
};

//! Set the transformation to the identity transformation.
inline Rotation& SetIdentity(Rotation& r);
inline UniformScale& SetIdentity(UniformScale& s);
inline Translation& SetIdentity(Translation& t);
inline Transformation& SetIdentity(Transformation& t);

//! \return the inverse of a transformation.
inline Rotation Inverse(const Rotation& rotation);
inline UniformScale Inverse(const UniformScale& scale);
inline Translation Inverse(const Translation& translation);
inline Transformation Inverse(const Transformation& transformation);

//! Inverts the argument transformation.
//! \return a reference to the inverted transformation.
inline Rotation& Invert(Rotation& rotation);
inline UniformScale& Invert(UniformScale& scale);
inline Translation& Invert(Translation& translation);
inline Transformation& Invert(Transformation& transformation);


//! The Concatenation functions below compose their argument transformations
//! so that the right-hand-side transformation is applied first and the
//! left-hand-side transformation second.
inline Rotation Concatenate(const Rotation& rot1, const Rotation& rot2);
inline Transformation Concatenate(const Rotation& rot, const UniformScale& s);
inline Transformation Concatenate(const Rotation& rot, const Translation& t);
inline Transformation Concatenate(const Rotation& rot, const Transformation& t);
inline UniformScale Concatenate(const UniformScale& s1, const UniformScale& s2);
inline Transformation Concatenate(const UniformScale& s, const Rotation& r);
inline Transformation Concatenate(const UniformScale& s, const Translation& t);
inline Transformation Concatenate(const UniformScale& s,
                                  const Transformation& t);
inline Translation Concatenate(const Translation& t1, const Translation& t2);
inline Transformation Concatenate(const Translation& t, const Rotation& rot);
inline Transformation Concatenate(const Translation& t, const UniformScale& s);
inline Transformation Concatenate(const Translation& translation,
                                  const Transformation& transformation);
inline Transformation Concatenate(const Transformation& t, const Rotation& rot);
inline Transformation Concatenate(const Transformation& t,
                                  const UniformScale& s);
inline Transformation Concatenate(const Transformation& transformation,
                                  const Translation& translation);
inline Transformation Concatenate(const Transformation& t1,
                                  const Transformation& t2);

//! Returns the result of transforming the argument point by the given
//! transformation.
inline Vector3 TransformPoint(const Rotation& rotation,
                              const Vector3& point);
inline Vector3 TransformPoint(const UniformScale& scale,
                              const Vector3& point);
inline Vector3 TransformPoint(const Translation& translation,
                              const Vector3& point);
inline Vector3 TransformPoint(const Transformation& transformation,
                              const Vector3& point);

//! Returns the result of transforming the argument direction by the given
//! transformation. That is, scale and translation are ignored.
inline Vector3 TransformDirection(const Rotation& rotation,
                                  const Vector3& direction);
inline Vector3 TransformDirection(const Transformation& transformation,
                                  const Vector3& direction);

} // namespace math
} // namespace romulus

#include "Math/Transformations.inl"

#endif // _MATHTRANSFORMATIONS_H_
