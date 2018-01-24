#ifndef _MATHMATRIX_H_
#define _MATHMATRIX_H_

//! \file Matrix.h
//! Contains Matrix class template, overloaded operators, and utility functions
//! for Matrix.

#include "Math/Constants.h"
#include "Math/Vector.h"
#include <exception>

namespace romulus
{
namespace math
{
//! An MxN matrix. When indexing matrix elements (e.g. m[i][j]), the first index
//! must be in the range [0, M), and the second index in [0, N). Operations on
//! matrices and Vectors interpret Matrices as row-major, so the first index
//! will index the row and the second the column.
template <uint_t M, uint_t N, typename T = real_t>
class Matrix
{
public:

    Matrix() {};
    explicit Matrix(const T& value);
    template <typename S>
    Matrix(const Matrix<M, N, S>& rhs);
    //! This constructs a Matrix from an M*N array of data. The array is
    //! interpreted as an array of N-length rows.
    explicit Matrix(const T data[]);

    template <typename S>
    inline Matrix<M, N, T>& operator=(const Matrix<M, N, S>& rhs);

    inline const T* operator[](uint_t i) const;
    inline T* operator[](uint_t i);

    inline const T* Data() const { return &m_data[0][0]; }

private:

    T m_data[M][N];
};

/* Equality operator and specializations */

template <uint_t M, uint_t N, typename T>
inline bool operator==(const Matrix<M, N, T>& lhs,
                       const Matrix<M, N, T>& rhs);

template <uint_t M, uint_t N>
inline bool operator==(const Matrix<M, N, real_t>& lhs, const Matrix<M,
                       N, real_t>& rhs);

template <uint_t M, uint_t N, typename T>
inline bool operator!=(const Matrix<M, N, T>& lhs,
                       const Matrix<M, N, T>& rhs);

/* Overloaded arithmetic operators */

template <uint_t M, uint_t N, typename T>
inline Matrix<M, N, T> operator+(const Matrix<M, N, T>& lhs,
                                 const Matrix<M, N, T>& rhs);

template <uint_t M, uint_t N, typename T>
inline Matrix<M, N, T>& operator+=(Matrix<M, N, T>& lhs,
                                   const Matrix<M, N, T>& rhs);

template <uint_t M, uint_t N, typename T>
inline Matrix<M, N, T> operator-(const Matrix<M, N, T>& lhs,
                                 const Matrix<M, N, T>& rhs);

template <uint_t M, uint_t N, typename T>
inline Matrix<M, N, T>& operator-=(Matrix<M, N, T>& lhs,
                                   const Matrix<M, N, T>& rhs);

template <uint_t M, uint_t N, uint_t P, typename T>
inline Matrix<M, P, T> operator*(const Matrix<M, N, T>& lhs,
                                 const Matrix<N, P, T>& rhs);

template <uint_t N, typename T>
inline Matrix<N, N, T>& operator*=(Matrix<N, N, T>& lhs,
                                   const Matrix<N, N, T>& rhs);

template <uint_t M, uint_t N, typename T, typename S>
inline Matrix<M, N, T> operator*(const Matrix<M, N, T>& lhs,
                                 const S& rhs);

template <uint_t M, uint_t N, typename T, typename S>
inline Matrix<M, N, T> operator*(const S& lhs,
                                 const Matrix<M, N, T>& rhs);

template <uint_t M, uint_t N, typename T, typename S>
inline Matrix<M, N, T>& operator*=(Matrix<M, N, T>& lhs, const S& rhs);

template <uint_t M, uint_t N, typename T, typename S>
inline Vector<M, S> operator*(const Matrix<M, N, T>& lhs,
                              const Vector<N, S>& rhs);

class BadMatrixInversionException : public std::exception
{
};

//! Set a matrix to the identity matrix
//! \param mat - An NxN matrix.
//! \return A reference to the matrix.
template <uint_t N, typename T>
inline Matrix<N, N, T>& SetIdentity(Matrix<N, N, T>& mat);

//! Return the transpose of a matrix
//! \param mat - An MxN matrix.
//! \return The transpose of mat.
template <uint_t M, uint_t N, typename T>
inline Matrix<N, M, T> Transposition(const Matrix<M, N, T>& mat);

//! Transpose a matrix
//! \param mat - An NxN matrix.
//! \return A reference to the transposed matrix mat.
template <uint_t N, typename T>
inline Matrix<N, N, T>& Transpose(Matrix<N, N, T>& mat);

//! Find a minor of a matrix
//! \param mat    - An NxN matrix.
//! \param row    - the row to omit
//! \param column - the column to omit
//! \return The determinant of the N-1xN-1 submatrix of mat.
template <uint_t N, typename T>
inline T Minor(const Matrix<N, N, T>& mat, uint_t row, uint_t column);

//! Find the minors of a matrix
//! \param mat - An NxN matrix.
//! \return An NxN matrix of the minors of mat.
template <uint_t N, typename T>
inline Matrix<N, N, T> MinorMatrix(const Matrix<N, N, T>& mat);

//! Find the cofactors of a matrix
//! \param mat - An NxN matrix.
//! \return An NxN matrix of the cofactors of mat.
template <uint_t N, typename T>
inline Matrix<N, N, T> CofactorMatrix(const Matrix<N, N, T>& mat);

//! Return the determinant of a matrix. The non-specialized
//! implementation has poor efficiency.
//! \param mat - An MxN matrix.
//! \return The determinant of the matrix.
template <uint_t N, typename T>
inline T Determinant(const Matrix<N, N, T>& mat);

/* Determinant specializations */

template <typename T>
inline T Determinant(const Matrix<1, 1, T>& mat);

template <typename T>
inline T Determinant(const Matrix<2, 2, T>& mat);

template <typename T>
inline T Determinant(const Matrix<3, 3, T>& mat);

//template <typename T>
//inline T Determinant(const Matrix<4, 4, T>& mat);

//! Find and return the inverse of the matrix. The non-specialized
//! implementation has poor time complexity.
//! Throws a BadMatrixInversion exception if the determinant is near
//! zero
//! \param mat - An NxN matrix.
//! \return The inverse of mat.
template <uint_t N, typename T>
inline Matrix<N, N, T> Inverse(const Matrix<N, N, T>& mat);

//! Invert the matrix. The non-specialized
//! implementation has poor time complexity.
//! Throws a BadMatrixInversion exception if the determinant is near
//! zero
//! \param mat - An NxN matrix.
//! \return A reference to the inverted matrix mat.
template <uint_t N, typename T>
Matrix<N, N, T>& Invert(Matrix<N, N, T>& mat);

template <uint_t TargetM, uint_t TargetN, uint_t StartRow, uint_t StartCol,
          uint_t M, uint_t N, typename T>
Matrix<TargetM, TargetN, T> Submatrix(const Matrix<M, N, T>& matrix);

/* Multiplication, Inverse, and Invert specializations */

/* 2x2 Matrix Specializations */

template <typename T>
Matrix<2, 2, T> operator*(const Matrix<2, 2, T>& lhs,
                          const Matrix<2, 2, T>& rhs);

template <typename T>
inline Matrix<2, 2, T> Inverse(const Matrix<2, 2, T>& mat);

template <typename T>
inline Matrix<2, 2, T>& Invert(Matrix<2, 2, T>& mat);

/* 3x3 Matrix Specializations */

template <typename T>
Matrix<3, 3, T> operator*(const Matrix<3, 3, T>& lhs,
                          const Matrix<3, 3, T>& rhs);

template <typename T>
inline Matrix<3, 3, T> Inverse(const Matrix<3, 3, T>& mat);

template <typename T>
inline Matrix<3, 3, T>& Invert(Matrix<3, 3, T>& mat);

/* 4x4 Matrix Specializations - Not yet implemented */

template <typename T>
Matrix<4, 4, T> operator*(const Matrix<4, 4, T>& lhs,
                          const Matrix<4, 4, T>& rhs);

/* Some useful typedefs. */
typedef Matrix<2, 2> Matrix22;
typedef Matrix<3, 3> Matrix33;
typedef Matrix<4, 4> Matrix44;
}
}

#include "Math/Matrix.inl" // #include inline implementations.

#endif // _MATHMATRIX_H_
