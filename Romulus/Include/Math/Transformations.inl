#ifndef _MATHTRANSFORMATIONS_INL_
#define _MATHTRANSFORMATIONS_INL_

#include "Math/Utilities.h"
#include <cmath>
#include <iostream>
namespace romulus
{
namespace math
{

Rotation::Rotation(const math::Quaternion& rotation):
    m_rotation(rotation)
{
    Normalize(m_rotation);
}

Rotation::Rotation(const Vector3& axis, real_t angle)
{
    SetAxisAngle(axis, angle);
}

Vector3 Rotation::Axis() const
{
    return Normal(Vector3(&m_rotation[1]));
}

real_t Rotation::Angle() const
{
    return ACosClamp(m_rotation[0]) * 2.0;
}

void Rotation::SetAxisAngle(const Vector3& axis, real_t angle)
{
    real_t halfAngle = .5f * angle;
    real_t s = cos(halfAngle);
    Vector3 v = Normal(axis) * sin(halfAngle);

    m_rotation[0] = s;
    m_rotation[1] = v[0];
    m_rotation[2] = v[1];
    m_rotation[3] = v[2];

    ASSERT(ApproxEqual(Magnitude(m_rotation), 1.0));
}

Matrix44 Rotation::Matrix() const
{
    real_t x2 = m_rotation[1] * m_rotation[1];
    real_t y2 = m_rotation[2] * m_rotation[2];
    real_t z2 = m_rotation[3] * m_rotation[3];
    real_t xy = m_rotation[1] * m_rotation[2];
    real_t xz = m_rotation[1] * m_rotation[3];
    real_t yz = m_rotation[2] * m_rotation[3];
    real_t wx = m_rotation[0] * m_rotation[1];
    real_t wy = m_rotation[0] * m_rotation[2];
    real_t wz = m_rotation[0] * m_rotation[3];

    Matrix44 mat;
    mat[0][0] = 1.0 - 2.0 * (y2 + z2);
    mat[1][0] = 2.0 * (xy + wz);
    mat[2][0] = 2.0 * (xz - wy);
    mat[3][0] = 0.0;
    mat[0][1] = 2.0 * (xy - wz);
    mat[1][1] = 1.0 - 2.0 * (x2 + z2);
    mat[2][1] = 2.0 * (yz + wx);
    mat[3][1] = 0.0;
    mat[0][2] = 2.0 * (xz + wy);
    mat[1][2] = 2.0 * (yz - wx);
    mat[2][2] = 1.0 - 2.0 * (x2 + y2);
    mat[3][2] = 0.0;
    mat[0][3] = 0.0;
    mat[1][3] = 0.0;
    mat[2][3] = 0.0;
    mat[3][3] = 1.0;

    return mat;
}

Rotation& Rotation::operator=(const Rotation& r)
{
    m_rotation = r.m_rotation;
    return *this;
}

UniformScale::UniformScale(real_t value):
    m_value(value)
{
}

Matrix44 UniformScale::Matrix() const
{
    Matrix44 mat;
    SetIdentity(mat);
    mat[0][0] = m_value;
    mat[1][1] = m_value;
    mat[2][2] = m_value;
    return mat;
}

UniformScale& UniformScale::operator=(const UniformScale& s)
{
    m_value = s.m_value;
    return *this;
}

Translation::Translation(const Vector3& translation):
    m_translation(translation)
{
}

Translation::Translation(real_t dx, real_t dy, real_t dz):
    m_translation(dx, dy, dz)
{
}

Matrix44 Translation::Matrix() const
{
    Matrix44 mat;
    SetIdentity(mat);
    mat[0][3] = m_translation[0];
    mat[1][3] = m_translation[1];
    mat[2][3] = m_translation[2];
    return mat;
}

Translation& Translation::operator=(const Translation& t)
{
    m_translation = t.m_translation;
    return *this;
}

Transformation::Transformation(const math::Translation& translation)
{
    *this = translation;
}

Transformation::Transformation(const math::Rotation& rotation)
{
    *this = rotation;
}

Transformation::Transformation(const math::UniformScale& scale)
{
    *this = scale;
}

Transformation::Transformation(const math::Translation& translation,
                               const math::Rotation& rotation,
                               const math::UniformScale& scale):
    m_translation(translation), m_rotation(rotation), m_scale(scale)
{
}

Matrix44 Transformation::Matrix() const
{
    return m_translation.Matrix() * m_rotation.Matrix() * m_scale.Matrix();
}

Transformation& Transformation::operator=(const math::UniformScale& s)
{
    SetIdentity(m_translation);
    SetIdentity(m_rotation);
    m_scale = s;
    return *this;
}

Transformation& Transformation::operator=(const math::Translation& t)
{
    m_translation = t;
    SetIdentity(m_rotation);
    SetIdentity(m_scale);
    return *this;
}

Transformation& Transformation::operator=(const math::Rotation& r)
{
    SetIdentity(m_translation);
    m_rotation = r;
    SetIdentity(m_scale);
    return *this;
}

Transformation& Transformation::operator=(const Transformation& t)
{
    m_rotation = t.m_rotation;
    m_translation = t.m_translation;
    m_scale = t.m_scale;
    return *this;
}

Rotation& SetIdentity(Rotation& r)
{
    return r = Rotation(Vector3(1.0, 0.0, 0.0), 0.0);
}

UniformScale& SetIdentity(UniformScale& s)
{
    return s = UniformScale(1);
}

Translation& SetIdentity(Translation& t)
{
    return t = Translation(0, 0, 0);
}

Transformation& SetIdentity(Transformation& t)
{
    Rotation r;
    Translation translation;
    UniformScale s;
    return t = Transformation(SetIdentity(translation), SetIdentity(r),
                              SetIdentity(s));
}

Rotation Inverse(const Rotation& rotation)
{
    ASSERT(ApproxEqual(Magnitude(rotation.Quaternion()), 1.0));
    return Rotation(Conjugate(rotation.Quaternion()));
}

Rotation& Invert(Rotation& rotation)
{
    return rotation = Inverse(rotation);
}

UniformScale Inverse(const UniformScale& scale)
{
    ASSERT(scale.Value() != 0.0);
    return UniformScale(1.0 / scale.Value());
}

UniformScale& Invert(UniformScale& scale)
{
    return scale = Inverse(scale);
}

Translation Inverse(const Translation& translation)
{
    return Translation(-1.0 * translation.Vector());
}

Translation& Invert(Translation& translation)
{
    return translation = Inverse(translation);
}

Transformation Inverse(const Transformation& transformation)
{
    return Concatenate(Concatenate(Inverse(transformation.Scale()),
                                   Inverse(transformation.Rotation())),
                       Inverse(transformation.Translation()));
}

Transformation& Invert(Transformation& transformation)
{
    return transformation = Inverse(transformation);
}

Rotation Concatenate(const Rotation& rot1, const Rotation& rot2)
{
    return Rotation(rot1.Quaternion() * rot2.Quaternion());
}

Transformation Concatenate(const Rotation& rot, const UniformScale& scale)
{
    return Transformation(Translation(0, 0, 0), rot, scale);
}

Transformation Concatenate(const Rotation& rot, const Translation& t)
{
    return Transformation(Translation(TransformPoint(rot, t.Vector())), rot,
                          UniformScale(1));
}

Transformation Concatenate(const Rotation& rot, const Transformation& t)
{
    return Transformation(Translation(TransformPoint(rot,
                                                     t.Translation().Vector())),
                          Concatenate(rot, t.Rotation()),
                          t.Scale());
}

UniformScale Concatenate(const UniformScale& s1, const UniformScale& s2)
{
    return UniformScale(s1.Value() * s2.Value());
}

Transformation Concatenate(const UniformScale& s, const Rotation& r)
{
    return Transformation(Translation(0, 0, 0), r, s);
}

Transformation Concatenate(const UniformScale& s, const Translation& t)
{
    return Transformation(Translation(s.Value() * t.Vector()),
                          Rotation(Vector3(1, 0, 0), 0), s);
}

Transformation Concatenate(const UniformScale& s, const Transformation& t)
{
    const Translation& translation = t.Translation();
    return Transformation(Translation(s.Value() * translation.Vector()),
                          t.Rotation(), Concatenate(s, t.Scale()));
}

Translation Concatenate(const Translation& t1, const Translation& t2)
{
    return Translation(t1.Vector() + t2.Vector());
}

Transformation Concatenate(const Translation& t, const Rotation& rot)
{
    return Transformation(t, rot, UniformScale(1));
}

Transformation Concatenate(const Translation& t, const UniformScale& s)
{
    return Transformation(t, Rotation(Vector3(1, 0, 0), 0), s);
}

Transformation Concatenate(const Translation& translation,
                           const Transformation& transformation)
{
    return Transformation(Concatenate(translation,
                                      transformation.Translation()),
                          transformation.Rotation(),
                          transformation.Scale());
}

Transformation Concatenate(const Transformation& t, const Rotation& rot)
{
    return Transformation(t.Translation(), Concatenate(t.Rotation(), rot),
                          t.Scale());
}

inline Transformation Concatenate(const Transformation& t, const UniformScale& s)
{
    return Transformation(t.Translation(),
                          t.Rotation(),
                          Concatenate(t.Scale(), s));
}

Transformation Concatenate(const Transformation& transformation,
                           const Translation& translation)
{
    const UniformScale& s = transformation.Scale();
    Translation scaledTranslation(s.Value() * translation.Vector());
    return Transformation(
            Concatenate(transformation.Translation(),
                        Translation(
                                TransformPoint(transformation.Rotation(),
                                               scaledTranslation.Vector()))),
            transformation.Rotation(),
            transformation.Scale());
}

Transformation Concatenate(const Transformation& t1,
                           const Transformation& t2)
{
    const UniformScale& s = t1.Scale();
    const Translation& translation = t2.Translation();
    Translation scaledTranslation(s.Value() * translation.Vector());
    return Transformation(
            Concatenate(t1.Translation(),
                        Translation(
                                TransformPoint(t1.Rotation(),
                                               scaledTranslation.Vector()))),
            Concatenate(t1.Rotation(), t2.Rotation()),
            Concatenate(t1.Scale(), t2.Scale()));
}

Vector3 TransformPoint(const Rotation& rotation,
                       const Vector3& point)
{
    ASSERT_EQ_EPSILON(Magnitude(rotation.Quaternion()), 1.0,
                      AcceptableMathEpsilon);

    Quaternion qPoint(0, point[0], point[1], point[2]);
    Quaternion result = rotation.Quaternion() * qPoint *
            Inverse(rotation.Quaternion());
    ASSERT_EQ_EPSILON(result[0], 0.0, 50.0 * AcceptableMathEpsilon);
    return Vector3(&result[1]);
}

Vector3 TransformPoint(const UniformScale& scale,
                       const Vector3& point)
{
    return scale.Value() * point;
}

Vector3 TransformPoint(const Translation& translation,
                       const Vector3& point)
{
    return translation.Vector() + point;
}

Vector3 TransformPoint(const Transformation& transformation,
                       const Vector3& point)
{
    return TransformPoint(transformation.Translation(),
                          TransformPoint(transformation.Rotation(),
                                         TransformPoint(transformation.Scale(),
                                                        point)));
}

Vector3 TransformDirection(const Rotation& rotation,
                           const Vector3& direction)
{
    return TransformPoint(rotation, direction);
}

Vector3 TransformDirection(const Transformation& transformation,
                           const Vector3& direction)
{
    return TransformPoint(transformation.Rotation(), direction);
}

} // namespace math
} // namespace romulus

#endif // _MATHTRANSFORMATIONS_INL_
