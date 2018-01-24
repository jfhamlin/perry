#ifndef _MATHVECTOR_INL_
#define _MATHVECTOR_INL_

//! \file Vector.inl
//! Contains implementations of inline methods and functions for the Vector
//! class template.

#include "Math/Utilities.h"

namespace romulus
{
namespace math
{

/* Vector member functions */

template <uint_t N, typename T>
Vector<N, T>::Vector(const T& value)
{
    for (uint_t i = 0; i < N; ++i)
        m_data[i] = value;
}

template <uint_t N, typename T>
Vector<N, T>::Vector(const T& v0, const T& v1)
{
    BOOST_STATIC_ASSERT(N == 2);
    m_data[0] = v0;
    m_data[1] = v1;
}

template <uint_t N, typename T>
Vector<N, T>::Vector(const T& v0, const T& v1, const T& v2)
{
    BOOST_STATIC_ASSERT(N == 3);
    m_data[0] = v0;
    m_data[1] = v1;
    m_data[2] = v2;
}

template <uint_t N, typename T>
Vector<N, T>::Vector(const T& v0, const T& v1, const T& v2, const T& v3)
{
    BOOST_STATIC_ASSERT(N == 4);
    m_data[0] = v0;
    m_data[1] = v1;
    m_data[2] = v2;
    m_data[3] = v3;
}

template <uint_t N, typename T>
Vector<N, T>::Vector(const Vector<3, T>& vec, const T& v3)
{
    BOOST_STATIC_ASSERT(N == 4);
    m_data[0] = vec[0];
    m_data[1] = vec[1];
    m_data[2] = vec[2];
    m_data[3] = v3;
}

template <uint_t N, typename T>
Vector<N, T>::Vector(const T dat[])
{
    for (uint_t i = 0; i < N; ++i)
        m_data[i] = dat[i];
}

template <uint_t N, typename T>
Vector<N, T>::Vector(const Vector<N, T>& rhs)
{
    for (uint_t i = 0; i < N; ++i)
        m_data[i] = rhs[i];
}

template <uint_t N, typename T>
template <typename S>
Vector<N, T>::Vector(const Vector<N, S>& rhs)
{
    for (uint_t i = 0; i < N; ++i)
        m_data[i] = static_cast<T>(rhs[i]);
}

template <uint_t N, typename T>
template <typename S>
Vector<N, T>& Vector<N, T>::operator=(const Vector<N, S>& rhs)
{
    for (uint_t i = 0; i < N; ++i)
        m_data[i] = static_cast<T>(rhs[i]);
    return *this;
}

template <uint_t N, typename T>
const T& Vector<N, T>::operator[](int i) const
{
    ASSERT(i >= 0 && i < (int)N);
    return m_data[i];
}

template <uint_t N, typename T>
T& Vector<N, T>::operator[](int i)
{
    ASSERT(i >= 0 && i < (int)N);
    return m_data[i];
}

/* Vector equality operators and specializations */

template<uint_t N, typename T>
bool operator==(const Vector<N, T>& lhs, const Vector<N, T>& rhs)
{
    for (uint_t i = 0; i < N; ++i)
        if (lhs[i] != rhs[i])
            return false;
    return true;
}

template<uint_t N>
bool operator==(const Vector<N, real_t>& lhs, const Vector<N, real_t>& rhs)
{
    for (uint_t i = 0; i < N; ++i)
        if (!ApproxEqual(lhs[i], rhs[i]))
            return false;
    return true;
}

template<uint_t N, typename T>
bool operator!=(const Vector<N, T>& lhs, const Vector<N, T>& rhs)
{
    return !(lhs == rhs);
}

/* Overloaded arithmetic operators */

template <uint_t N, typename T>
inline Vector<N, T> operator+(const Vector<N, T>& lhs, const Vector<N, T>& rhs)
{
    Vector<N, T> result;
    for (uint_t i = 0; i < N; ++i)
        result[i] = lhs[i] + rhs[i];
    return result;
}

template <uint_t N, typename T>
inline Vector<N, T>& operator+=(Vector<N, T>& lhs, const Vector<N, T>& rhs)
{
    for (uint_t i = 0; i < N; ++i)
        lhs[i] = lhs[i] + rhs[i];
    return lhs;
}

template <uint_t N, typename T>
inline Vector<N, T> operator-(const Vector<N, T>& v)
{
    return static_cast<T>(-1) * v;
}

template <uint_t N, typename T>
inline Vector<N, T> operator-(const Vector<N, T>& lhs, const Vector<N, T>& rhs)
{
    Vector<N, T> result;
    for (uint_t i = 0; i < N; ++i)
        result[i] = lhs[i] - rhs[i];
    return result;
}

template <uint_t N, typename T>
inline Vector<N, T>& operator-=(Vector<N, T>& lhs, const Vector<N, T>& rhs)
{
    for (uint_t i = 0; i < N; ++i)
        lhs[i] = lhs[i] - rhs[i];
    return lhs;
}

template <uint_t N, typename T, typename S>
inline Vector<N, T> operator*(const Vector<N, T>& lhs, const S& scalar)
{
    Vector<N, T> result;
    for (uint_t i = 0; i < N; ++i)
        result[i] = lhs[i] * scalar;
    return result;
}

template <uint_t N, typename T, typename S>
inline Vector<N, T>& operator*=(Vector<N, T>& lhs, const S& scalar)
{
    for (uint_t i = 0; i < N; ++i)
        lhs[i] = lhs[i] * scalar;
    return lhs;
}

template <uint_t N, typename T, typename S>
inline Vector<N, T> operator*(const S& scalar, const Vector<N, T>& rhs)
{
    Vector<N, T> result;
    for (uint_t i = 0; i < N; ++i)
        result[i] = scalar * rhs[i];
    return result;
}

template <uint_t N, typename T, typename S>
inline Vector<N, T> operator/(const Vector<N, T>& lhs, const S& scalar)
{
    Vector<N, T> result;
    for (uint_t i = 0; i < N; ++i)
        result[i] = lhs[i] / scalar;
    return result;
}

template <uint_t N, typename T, typename S>
inline Vector<N, T>& operator/=(Vector<N, T>& lhs, const S& scalar)
{
    for (uint_t i = 0; i < N; ++i)
        lhs[i] = lhs[i] / scalar;
    return lhs;
}

/* Vector utility functions */

template <uint_t N, typename Real>
inline Real MagnitudeSquared(const Vector<N, Real>& vec)
{
    Real result = vec[0] * vec[0];
    for (uint_t i = 1; i < N; ++i)
        result += vec[i] * vec[i];
    return result;
}

template <uint_t N, typename Real>
inline Real Magnitude(const Vector<N, Real>& vec)
{
    return sqrt(MagnitudeSquared(vec));
}

template <uint_t N, typename Real>
inline Vector<N, Real> Normal(const Vector<N, Real>& vec)
{
    Vector<N, Real> result;
    Real magnitude = Magnitude(vec);
    for (uint_t i = 0; i < N; ++i)
        result[i] = vec[i] / magnitude;
    return result;
}

template <uint_t N, typename Real>
inline Vector<N, Real>& Normalize(Vector<N, Real>& vec)
{
    Real magnitude = Magnitude(vec);
    for (uint_t i = 0; i < N; ++i)
        vec[i] = vec[i] / magnitude;
    return vec;
}

template <uint_t N, typename Real>
inline bool IsUnitLength(const Vector<N, Real>& vec)
{
    return (fabs(MagnitudeSquared(vec) - static_cast<Real>(1)) <=
            (AcceptableMathEpsilon * (AcceptableMathEpsilon + 2.0)));
}

template <uint_t N, typename T, typename S>
inline T Dot(const Vector<N, T>& lhs, const Vector<N, S>& rhs)
{
    T result = lhs[0] * rhs[0];
    for (uint_t i = 1; i < N; ++i)
        result += lhs[i] * rhs[i];
    return result;
}

template <typename Real>
inline Vector<3, Real> Cross(const Vector<3, Real>& lhs,
                             const Vector<3, Real>& rhs)
{
    Vector<3, Real> result;
    result[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1];
    result[1] = lhs[2] * rhs[0] - lhs[0] * rhs[2];
    result[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0];
    return result;
}

template <uint_t N, typename Real>
inline Vector<N, Real> Projection(const Vector<N, Real>& projected,
                                  const Vector<N, Real>& onto)
{
    return (Dot(projected, onto) / Dot(onto, onto)) * onto;
}

}
}

#endif // _MATHVECTOR_INL_
