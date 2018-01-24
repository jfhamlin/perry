#ifndef _MATHVECTOR_H_
#define _MATHVECTOR_H_

//! \file Vector.h
//! Contains Vector class template, overloaded operators, and utility functions
//! for Vectors.

#include "Math/Constants.h"
#include "Utility/Assertions.h"
#include <cmath>
#include <string>

namespace romulus
{
namespace math
{

//! This is an N-dimensional vector class template, with constructors for
//! one, two, three, and four dimensions.
template <uint_t N, typename T = real_t>
class Vector
{
public:

    //! Vector ctor. Vector elements are not initialized in the default ctor.
    Vector() {}
    //! Vector ctor.
    inline explicit Vector(const T& value);
    //! Vector<2> ctor.
    inline Vector(const T& v0, const T& v1);
    //! Vector<3> ctor.
    inline Vector(const T& v0, const T& v1, const T& v2);
    //! Vector<4> ctors.
    inline Vector(const T& v0, const T& v1, const T& v2, const T& v3);
    inline Vector(const Vector<3, T>& vec, const T& v3);
    //! Vector ctor.
    explicit Vector(const T dat[]);
    //! Copy ctor.
    inline Vector(const Vector& rhs);

    template <typename S>
    inline explicit Vector(const Vector<N, S>& rhs);

    template <typename S>
    inline Vector<N, T>& operator=(const Vector<N, S>& rhs);

    inline const T& operator[](int i) const;

    inline T& operator[](int i);

    inline const T* Data() const { return &m_data[0]; }

private:
    T m_data[N];
};

/* Vector equality operators and specializations */

template<uint_t N, typename T>
bool operator==(const Vector<N, T>& lhs, const Vector<N, T>& rhs);

template<uint_t N>
bool operator==(const Vector<N, real_t>& lhs, const Vector<N, real_t>& rhs);

template<uint_t N, typename T>
bool operator!=(const Vector<N, T>& lhs, const Vector<N, T>& rhs);

/* Overloaded arithmetic operators */

template <uint_t N, typename T>
inline Vector<N, T> operator+(const Vector<N, T>& lhs,
                              const Vector<N, T>& rhs);

template <uint_t N, typename T>
inline Vector<N, T>& operator+=(Vector<N, T>& lhs, const Vector<N, T>& rhs);

template <uint_t N, typename T>
inline Vector<N, T> operator-(const Vector<N, T>& v);

template <uint_t N, typename T>
inline Vector<N, T> operator-(const Vector<N, T>& lhs,
                              const Vector<N, T>& rhs);

template <uint_t N, typename T>
inline Vector<N, T>& operator-=(Vector<N, T>& lhs, const Vector<N, T>& rhs);

template <uint_t N, typename T, typename S>
inline Vector<N, T> operator*(const Vector<N, T>& lhs, const S& scalar);

template <uint_t N, typename T, typename S>
inline Vector<N, T>& operator*=(Vector<N, T>& lhs, const S& scalar);

template <uint_t N, typename T, typename S>
inline Vector<N, T> operator*(const S& scalar, const Vector<N, T>& rhs);

template <uint_t N, typename T, typename S>
inline Vector<N, T> operator/(const Vector<N, T>& lhs, const S& scalar);

template <uint_t N, typename T, typename S>
inline Vector<N, T>& operator/=(Vector<N, T>& lhs, const S& scalar);

/* Vector utility functions */

//! Find the square of the magnitude of a vector.
//! \param vec - A vector of real numbers.
//! \return The square of the magnitude of vec.
template <uint_t N, typename Real>
inline Real MagnitudeSquared(const Vector<N, Real>& vec);

//! Find the magnitude of a vector.
//! \param vec - A vector of real numbers.
//! \return The magnitude of vec.
template <uint_t N, typename Real>
inline Real Magnitude(const Vector<N, Real>& vec);

//! Find the normal of a vector.
//! \param vec - A vector of real numbers.
//! \return The normal of vec.
template <uint_t N, typename Real>
inline Vector<N, Real> Normal(const Vector<N, Real>& vec);

//! Normalize a vector.
//! \param vec - A vector of real numbers.
//! \return A reference to the normalized vector.
template <uint_t N, typename Real>
inline Vector<N, Real>& Normalize(Vector<N, Real>& vec);

//! Find whether a vector is unit-length.
//! \param vec - A vector of real numbers.
//! \return True iff vec's magnitude is approximately 1.0.
template <uint_t N, typename Real>
inline bool IsUnitLength(const Vector<N, Real>& vec);

//! Find the dot product of two vectors.
//! \param lhs - A vector of length N
//! \param rhs - A vector of length N
//! \return The dot product of the two vectors.
template <uint_t N, typename T, typename S>
inline T Dot(const Vector<N, T>& lhs, const Vector<N, S>& rhs);

//! Find the cross product of two vectors.
//! \param lhs - A vector of real numbers.
//! \param rhs - A vector of real numbers.
//! \return The cross product of the two vectors.
template <typename Real>
inline Vector<3, Real> Cross(const Vector<3, Real>& lhs,
                             const Vector<3, Real>& rhs);

//! Find the projection of one vector onto another.
//! \param projected - A vector of real numbers.
//! \param onto - A vector of real numbers.
//! \return The projection of projected onto onto.
template <uint_t N, typename Real>
inline Vector<N, Real> Projection(const Vector<N, Real>& projected,
                                  const Vector<N, Real>& onto);

/* Vector typedefs for common sizes and types */
typedef Vector<2> Vector2;
typedef Vector<3> Vector3;
typedef Vector<4> Vector4;
}
}

#define PRINT_VECTOR(vec) \
    printf("%f, %f, %f\n", (vec)[0], (vec)[1], (vec)[2])

#include "Math/Vector.inl" // #include inline implementations

#endif // _MATHVECTOR_H_
