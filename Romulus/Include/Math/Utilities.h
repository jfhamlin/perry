#ifndef _MATHUTILITIES_H_
#define _MATHUTILITIES_H_

#include "Math/Matrix.h"
#include "Math/Constants.h"
#include "Utility/Assertions.h"
#include <cmath>
#include <cfloat>
#include <valarray>
#include <vector>

namespace romulus
{
namespace math
{

inline bool IsNaN(real_t r)
{
#ifdef _WIN32
    return _isnan(r);
#else
    return isnan(r);
#endif
}

//! Equality comparison of that allows for some imprecision.
//! This function should be used for all equality comparisons of
//! floating point types.
//! \return true iff the difference between the numbers is less than
//!         or equal to AcceptableEpsilon.
inline bool ApproxEqual(real_t a, real_t b,
                        real_t AcceptableEpsilon = AcceptableMathEpsilon)
{
    real_t difference = a - b;
    return difference <= AcceptableEpsilon &&
            difference >= - AcceptableEpsilon;
}

template <typename T>
inline T Clamp(const T& x, const T& min, const T& max)
{
    if (x < min)
        return min;
    else if (x > max)
        return max;
    else
        return x;
}

template <typename T>
inline T Sign(const T x)
{
    return (x < static_cast<T>(0)) ?
            (static_cast<T>(-1)) :
            (static_cast<T>(1));
}

template <typename T>
inline T Abs(const T x)
{
    if (x < static_cast<T>(0))
        return static_cast<T>(-1) * x;
    return x;
}

template <>
inline float Abs(float x)
{
    return fabs(x);
}

template <>
inline double Abs(double x)
{
    return fabs(x);
}

template <typename T>
inline const T& Min(const T& a, const T& b)
{
    return (a < b) ? a : b;
}

template <typename T>
inline T& Min(T& a, T& b)
{
    return (a < b) ? a : b;
}

template <typename T>
inline const T& Min(const T& a, const T& b, const T& c)
{
    return Min(a, Min(b, c));
}

template <typename T>
inline T& Min(T& a, T& b, T& c)
{
    return Min(a, Min(b, c));
}

template <typename T>
inline const T& Max(const T& a, const T& b)
{
    return (a > b) ? a : b;
}

template <typename T>
inline T& Max(T& a, T& b)
{
    return (a > b) ? a : b;
}

template <typename T>
inline const T& Max(const T& a, const T& b, const T& c)
{
    return Max(a, Max(b, c));
}

template <typename T>
inline T& Max(T& a, T& b, T& c)
{
    return Max(a, Max(b, c));
}

inline float ACos(float c)
{
    return acos(c);
}

inline double ACos(double c)
{
    return acos(c);
}

template <typename T>
inline T ACosClamp(T c)
{
    return ACos(Clamp<T>(c, -1, 1));
}

template <typename T>
inline T Lerp(const T& start, const T& finish, const real_t alpha)
{
    return start + alpha * (finish - start);
}

template <typename T>
inline T Factorial(T x)
{
    ASSERT(x >= static_cast<T>(0));

    T result = static_cast<T>(1);
    for (T i = static_cast<T>(2); i <= x; i = i + static_cast<T>(1))
    {
        result = result * i;
    }
    return result;
}

template <typename T>
inline T Choose(T n, T k)
{
    return Factorial(n) / (Factorial(k) * Factorial(n - k));
}

inline real_t DegreesToRadians(real_t angle)
{
    return angle * Pi / 180.f;
}

inline real_t RadiansToDegrees(real_t angle)
{
    return angle * 180.f / Pi;
}

inline bool IsPowerOfTwo(uint_t x)
{
    return (x & (x - 1)) == 0;
}

inline uint_t NextPowerOfTwo(uint_t x)
{
    ASSERT(x > 0);
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
}

template <uint_t M, uint_t N, typename T>
class Matrix;

Matrix<4, 4, real_t> GenerateRotationTransform(Vector3 axis, real_t angle);

Matrix<4, 4, real_t> GeneratePerspectiveProjectionTransform(real_t fov,
                                                            real_t aspectRatio,
                                                            real_t near,
                                                            real_t far);

Matrix<4, 4, real_t> GenerateOrthographicProjectionTransform(
        real_t left, real_t right,
        real_t bottom, real_t top,
        real_t near = 1.f, real_t far = -1.f);

//! Compute tangent vectors for given geometry.
//! \param vertices - Vertices.
//! \param normals - Vertex normals.
//! \param texCoords - Vertex texture coordinates.
//! \param vertexCount - The number of vertices.
//! \param indices - Face indices.
//! \param indexCount - Number of indices.
//! \param tangents - The output, a pointer to a block of memory
//! that is of 12 * vertexCount bytes.
void CalculateTangentVectors(const Vector3* vertices, const Vector3* normals,
                             const Vector2* texCoords, size_t vertexCount,
                             const ushort_t* indices, size_t indexCount,
                             Vector3* tangents);
}
}

#endif // _MATHUTILITIES_H_
