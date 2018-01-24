#ifndef _MATHMATRIX_INL_
#define _MATHMATRIX_INL_

//! \file Matrix.inl
//! Contains inline implementations of Matrix methods, helper functions, and
//! template specializations of some for 2x2, 3x3, and 4x4 matrices.

#include "Utility/Assertions.h"
#include <cstring>

namespace romulus
{
namespace math
{
/* Matrix member functions */

template <uint_t M, uint_t N, typename T>
Matrix<M, N, T>::Matrix(const T& value)
{
    for (uint_t i = 0; i < M; ++i)
        for (uint_t j = 0; j < N; ++j)
            m_data[i][j] = value;
}

template <uint_t M, uint_t N, typename T>
template <typename S>
Matrix<M, N, T>::Matrix(const Matrix<M, N, S>& rhs)
{
    for (uint_t i = 0; i < M; ++i)
        for (uint_t j = 0; j < N; ++j)
            m_data[i][j] = static_cast<T>(rhs[i][j]);
}

template <uint_t M, uint_t N, typename T>
Matrix<M, N, T>::Matrix(const T data[])
{
    memcpy(&m_data[0][0], &data[0], sizeof(T) * static_cast<size_t>(M * N));
}

template <uint_t M, uint_t N, typename T>
template <typename S>
Matrix<M, N, T>& Matrix<M, N, T>::operator=(const Matrix<M, N, S>& rhs)
{
    for (uint_t i = 0; i < M; ++i)
        for (uint_t j = 0; j < N; ++j)
            m_data[i][j] = static_cast<T>(rhs[i][j]);
    return *this;
}

template <uint_t M, uint_t N, typename T>
const T* Matrix<M, N, T>::operator[](uint_t i) const
{
    ASSERT(i >= 0 && i < M);
    return m_data[i];
}

template <uint_t M, uint_t N, typename T>
T* Matrix<M, N, T>::operator[](uint_t i)
{
    ASSERT(i >= 0 && i < M);
    return m_data[i];
}

/* Equality operator and specializations */

template <uint_t M, uint_t N, typename T>
bool operator==(const Matrix<M, N, T>& lhs,
                const Matrix<M, N, T>& rhs)
{
    for (uint_t i = 0; i < M; ++i)
        for (uint_t j = 0; j < N; ++j)
            if (lhs[i][j] != rhs[i][j])
                return false;
    return true;
}

template <uint_t M, uint_t N>
bool operator==(const Matrix<M, N, real_t>& lhs, const Matrix<M,
                N, real_t>& rhs)
{
    for (uint_t i = 0; i < M; ++i)
        for (uint_t j = 0; j < N; ++j)
            if (!ApproxEqual(lhs[i][j], rhs[i][j]))
                return false;
    return true;
}

template <uint_t M, uint_t N, typename T>
bool operator!=(const Matrix<M, N, T>& lhs,
                const Matrix<M, N, T>& rhs)
{
    return !(lhs == rhs);
}

/* Overloaded arithmetic operators */

template <uint_t M, uint_t N, typename T>
Matrix<M, N, T> operator+(const Matrix<M, N, T>& lhs,
                          const Matrix<M, N, T>& rhs)
{
    Matrix<M, N, T> result;
    for (uint_t i = 0; i < M; ++i)
        for (uint_t j = 0; j < N; ++j)
            result[i][j] = lhs[i][j] + rhs[i][j];
    return result;
}

template <uint_t M, uint_t N, typename T>
Matrix<M, N, T>& operator+=(Matrix<M, N, T>& lhs,
                            const Matrix<M, N, T>& rhs)
{
    for (uint_t i = 0; i < M; ++i)
        for (uint_t j = 0; j < N; ++j)
            lhs[i][j] = lhs[i][j] + rhs[i][j];
    return lhs;
}

template <uint_t M, uint_t N, typename T>
Matrix<M, N, T> operator-(const Matrix<M, N, T>& lhs,
                          const Matrix<M, N, T>& rhs)
{
    Matrix<M, N, T> result;
    for (uint_t i = 0; i < M; ++i)
        for (uint_t j = 0; j < N; ++j)
            result[i][j] = lhs[i][j] - rhs[i][j];
    return result;
}

template <uint_t M, uint_t N, typename T>
Matrix<M, N, T>& operator-=(Matrix<M, N, T>& lhs,
                            const Matrix<M, N, T>& rhs)
{
    for (uint_t i = 0; i < M; ++i)
        for (uint_t j = 0; j < N; ++j)
            lhs[i][j] = lhs[i][j] - rhs[i][j];
    return lhs;
}

template <uint_t M, uint_t N, uint_t P, typename T>
Matrix<M, P, T> operator*(const Matrix<M, N, T>& lhs,
                          const Matrix<N, P, T>& rhs)
{
    Matrix<M, P, T> result;
    for (uint_t i = 0; i < M; ++i)
        for (uint_t j = 0; j < P; ++j)
        {
            result[i][j] = lhs[i][0] * rhs[0][j];
            for (uint_t k = 1; k < N; ++k)
                result[i][j] += lhs[i][k] * rhs[k][j];
        }
    return result;
}

template <uint_t N, typename T>
Matrix<N, N, T>& operator*=(Matrix<N, N, T>& lhs,
                            const Matrix<N, N, T>& rhs)
{
    return lhs = lhs * rhs;
}

template <uint_t M, uint_t N, typename T, typename S>
Matrix<M, N, T> operator*(const Matrix<M, N, T>& lhs,
                          const S& rhs)
{
    Matrix<M, N, T> result;
    for (uint_t i = 0; i < M; ++i)
        for (uint_t j = 0; j < N; ++j)
            result[i][j] = lhs[i][j] * rhs;
    return result;
}

template <uint_t M, uint_t N, typename T, typename S>
Matrix<M, N, T> operator*(const S& lhs,
                          const Matrix<M, N, T>& rhs)
{
    Matrix<M, N, T> result;
    for (uint_t i = 0; i < M; ++i)
        for (uint_t j = 0; j < N; ++j)
            result[i][j] = lhs * rhs[i][j];
    return result;
}

template <uint_t M, uint_t N, typename T, typename S>
Matrix<M, N, T>& operator*=(Matrix<M, N, T>& lhs, const S& rhs)
{
    for (uint_t i = 0; i < M; ++i)
        for (uint_t j = 0; j < N; ++j)
            lhs[i][j] = lhs[i][j] * rhs;
    return lhs;
}

template <uint_t M, uint_t N, typename T, typename S>
Vector<M, S> operator*(const Matrix<M, N, T>& lhs,
                       const Vector<N, S>& rhs)
{
    Vector<M, S> result;
    for (uint_t i = 0; i < M; ++i)
    {
        result[i] = static_cast<T>(0);
        for (uint_t j = 0; j < N; ++j)
            result[i] += lhs[i][j] * rhs[j];
    }
    return result;
}

/* Matrix utility functions */

template <uint_t N, typename T>
Matrix<N, N, T>& SetIdentity(Matrix<N, N, T>& mat)
{
    for (uint_t i = 0; i < N; ++i)
        for (uint_t j = 0; j < N; ++j)
            if (i == j)
                mat[i][j] = static_cast<T>(1);
            else
                mat[i][j] = static_cast<T>(0);
    return mat;
}

template <uint_t M, uint_t N, typename T>
Matrix<N, M, T> Transposition(const Matrix<M, N, T>& mat)
{
    Matrix<N, M, T> result;
    for (uint_t i = 0; i < M; ++i)
        for (uint_t j = 0; j < N; ++j)
            result[j][i] = mat[i][j];
    return result;
}

template <uint_t N, typename T>
Matrix<N, N, T>& Transpose(Matrix<N, N, T>& mat)
{
    for (uint_t i = 0; i < N - 1; ++i)
        for (uint_t j = i + 1; j < N; ++j)
        {
            T temp = mat[i][j];
            mat[i][j] = mat[j][i];
            mat[j][i] = temp;
        }
    return mat;
}

template <uint_t N, typename T>
T Minor(const Matrix<N, N, T>& mat, uint_t row, uint_t column)
{
    ASSERT(0 <= row && row < N && 0 <= column && column < N);
    Matrix<N-1, N-1, T> subMatrix;
    for (uint_t i = 0; i < row; ++i)
        for (uint_t j = 0; j < column; ++j)
            subMatrix[i][j] = mat[i][j];
    for (uint_t i = 0; i < row; ++i)
        for (uint_t j = column + 1; j < N; ++j)
            subMatrix[i][j-1] = mat[i][j];
    for (uint_t i = row + 1; i < N; ++i)
        for (uint_t j = 0; j < column; ++j)
            subMatrix[i-1][j] = mat[i][j];
    for (uint_t i = row + 1; i < N; ++i)
        for (uint_t j = column + 1; j < N; ++j)
            subMatrix[i-1][j-1] = mat[i][j];
    return Determinant(subMatrix);
}

template <uint_t N, typename T>
Matrix<N, N, T> MinorMatrix(const Matrix<N, N, T>& mat)
{
    Matrix<N, N, T> minorMatrix;
    for (uint_t i = 0; i < N; ++i)
        for (uint_t j = 0; j < N; ++j)
        {
            minorMatrix[i][j] = Minor(mat, i, j);
        }
    return minorMatrix;
}

template <uint_t N, typename T>
Matrix<N, N, T> CofactorMatrix(const Matrix<N, N, T>& mat)
{
    Matrix<N, N, T> cofactorMatrix;
    int rowSign = 1;
    for (uint_t i = 0; i < N; ++i)
    {
        int columnSign = rowSign;
        for (uint_t j = 0; j < N; ++j)
        {
            cofactorMatrix[i][j] = static_cast<T>(columnSign) *
                    Minor(mat, i, j);
            columnSign *= -1;
        }
        rowSign *= -1;
    }
    return cofactorMatrix;
}

template <uint_t N, typename T>
T Determinant(const Matrix<N, N, T>& mat)
{
    int sign = -1;
    T determinant = mat[0][0] * Minor(mat, 0, 0);
    for (uint_t i = 1; i < N; ++i)
    {
        determinant += static_cast<T>(sign) * mat[0][i] *
                Minor(mat, 0, i);
        sign *= -1;
    }
    return determinant;
}

/* Determinant specializations */

template <typename T>
T Determinant(const Matrix<1, 1, T>& mat)
{
    return mat[0][0];
}

template <typename T>
T Determinant(const Matrix<2, 2, T>& mat)
{
    return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
}

template <typename T>
T Determinant(const Matrix<3, 3, T>& mat)
{
    return
            mat[0][0] * (mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1]) -
            mat[0][1] * (mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0]) +
            mat[0][2] * (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]);
}

//template <typename T>
// T Determinant(const Matrix<4, 4, T>& mat)
//{
//    return T();
//}

template <uint_t N, typename T>
Matrix<N, N, T> Inverse(const Matrix<N, N, T>& mat)
{
    T determinant = Determinant(mat);

    if (ApproxEqual(determinant, static_cast<T>(0.0)))
        throw BadMatrixInversionException();

    T inverseDeterminant = static_cast<T>(1.0 / determinant);

    return inverseDeterminant * Transposition(CofactorMatrix(mat));
}

template <uint_t N, typename T>
Matrix<N, N, T>& Invert(Matrix<N, N, T>& mat)
{
    return mat = Inverse(mat);
}

template <uint_t TargetM, uint_t TargetN, uint_t StartRow, uint_t StartCol,
          uint_t M, uint_t N, typename T>
Matrix<TargetM, TargetN, T> Submatrix(const Matrix<M, N, T>& matrix)
{
    STATIC_ASSERT(TargetM + StartRow <= M);
    STATIC_ASSERT(TargetN + StartCol <= N);

    Matrix<TargetM, TargetN, T> submatrix;

    for (uint_t row = 0; row < TargetM; ++row)
    {
        for (uint_t col = 0; col < TargetN; ++col)
        {
            submatrix[row][col] = matrix[row + StartRow][col + StartCol];
        }
    }

    return submatrix;
}

/* Multiplication, Inverse, and Invert specializations */

/* 2x2 Matrix Specializations */

template <typename T>
Matrix<2, 2, T> operator*(const Matrix<2, 2, T>& lhs,
                          const Matrix<2, 2, T>& rhs)
{
    Matrix<2, 2, T> result;
    result[0][0] = lhs[0][0] * rhs[0][0] + lhs[0][1] * rhs[1][0];
    result[0][1] = lhs[0][0] * rhs[0][1] + lhs[0][1] * rhs[1][1];
    result[1][0] = lhs[1][0] * rhs[0][0] + lhs[1][1] * rhs[1][0];
    result[1][1] = lhs[1][0] * rhs[0][1] + lhs[1][1] * rhs[1][1];
    return result;
}


template <typename T>
Matrix<2, 2, T> Inverse(const Matrix<2, 2, T>& mat)
{
    T determinant = Determinant(mat);

    if (ApproxEqual(determinant, static_cast<T>(0.0)))
        throw BadMatrixInversionException();

    T inverseDeterminant = static_cast<T>(1.0 / determinant);

    Matrix<2, 2, T> result;

    result[0][0] =   mat[1][1] * inverseDeterminant;
    result[0][1] = - mat[0][1] * inverseDeterminant;
    result[1][0] = - mat[1][0] * inverseDeterminant;
    result[1][1] =   mat[0][0] * inverseDeterminant;

    return result;
}

template <typename T>
Matrix<2, 2, T>& Invert(Matrix<2, 2, T>& mat)
{
    T determinant = Determinant(mat);

    if (ApproxEqual(determinant, static_cast<T>(0.0)))
        throw BadMatrixInversionException();

    T inverseDeterminant = static_cast<T>(1.0 / determinant);

    T mat00 = mat[0][0];
    mat[0][0] =   mat[1][1] * inverseDeterminant;
    mat[0][1] = - mat[0][1] * inverseDeterminant;
    mat[1][0] = - mat[1][0] * inverseDeterminant;
    mat[1][1] =   mat00 * inverseDeterminant;

    return mat;
}

/* 3x3 Matrix Specializations */

template <typename T>
Matrix<3, 3, T> operator*(const Matrix<3, 3, T>& lhs,
                          const Matrix<3, 3, T>& rhs)
{
    Matrix<3, 3, T> result;
    result[0][0] = lhs[0][0] * rhs[0][0] + lhs[0][1] * rhs[1][0] +
            lhs[0][2] * rhs[2][0];
    result[0][1] = lhs[0][0] * rhs[0][1] + lhs[0][1] * rhs[1][1] +
            lhs[0][2] * rhs[2][1];
    result[0][2] = lhs[0][0] * rhs[0][2] + lhs[0][1] * rhs[1][2] +
            lhs[0][2] * rhs[2][2];

    result[1][0] = lhs[1][0] * rhs[0][0] + lhs[1][1] * rhs[1][0] +
            lhs[1][2] * rhs[2][0];
    result[1][1] = lhs[1][0] * rhs[0][1] + lhs[1][1] * rhs[1][1] +
            lhs[1][2] * rhs[2][1];
    result[1][2] = lhs[1][0] * rhs[0][2] + lhs[1][1] * rhs[1][2] +
            lhs[1][2] * rhs[2][2];

    result[2][0] = lhs[2][0] * rhs[0][0] + lhs[2][1] * rhs[1][0] +
            lhs[2][2] * rhs[2][0];
    result[2][1] = lhs[2][0] * rhs[0][1] + lhs[2][1] * rhs[1][1] +
            lhs[2][2] * rhs[2][1];
    result[2][2] = lhs[2][0] * rhs[0][2] + lhs[2][1] * rhs[1][2] +
            lhs[2][2] * rhs[2][2];
    return result;
}

template <typename T>
Matrix<3, 3, T> Inverse(const Matrix<3, 3, T>& mat)
{
    Matrix<3, 3, T> cofactors;
    cofactors[0][0] = mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1];
    cofactors[1][0] = mat[1][2] * mat[2][0] - mat[1][0] * mat[2][2];
    cofactors[2][0] = mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0];

    T determinant = mat[0][0] * cofactors[0][0] +
            mat[0][1] * cofactors[1][0] +
            mat[0][2] * cofactors[2][0];

    if (ApproxEqual(determinant, static_cast<T>(0.0)))
        throw BadMatrixInversionException();

    T inverseDeterminant = static_cast<T>(1.0 / determinant);

    Matrix<3, 3, T> result;

    cofactors[0][1] = mat[0][2] * mat[2][1] - mat[0][1] * mat[2][2];
    cofactors[0][2] = mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1];
    cofactors[1][1] = mat[0][0] * mat[2][2] - mat[0][2] * mat[2][0];
    cofactors[1][2] = mat[0][2] * mat[1][0] - mat[0][0] * mat[1][2];
    cofactors[2][1] = mat[0][1] * mat[2][0] - mat[0][0] * mat[2][1];
    cofactors[2][2] = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];

    result[0][0] = cofactors[0][0] * inverseDeterminant;
    result[0][1] = cofactors[0][1] * inverseDeterminant;
    result[0][2] = cofactors[0][2] * inverseDeterminant;
    result[1][0] = cofactors[1][0] * inverseDeterminant;
    result[1][1] = cofactors[1][1] * inverseDeterminant;
    result[1][2] = cofactors[1][2] * inverseDeterminant;
    result[2][0] = cofactors[2][0] * inverseDeterminant;
    result[2][1] = cofactors[2][1] * inverseDeterminant;
    result[2][2] = cofactors[2][2] * inverseDeterminant;

    return result;
}

template <typename T>
Matrix<3, 3, T>& Invert(Matrix<3, 3, T>& mat)
{
    Matrix<3, 3, T> cofactors;
    cofactors[0][0] = mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1];
    cofactors[1][0] = mat[1][2] * mat[2][0] - mat[1][0] * mat[2][2];
    cofactors[2][0] = mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0];

    T determinant = mat[0][0] * cofactors[0][0] +
            mat[0][1] * cofactors[1][0] +
            mat[0][2] * cofactors[2][0];

    if (ApproxEqual(determinant, static_cast<T>(0.0)))
        throw BadMatrixInversionException();

    T inverseDeterminant = static_cast<T>(1.0 / determinant);

    cofactors[0][1] = mat[0][2] * mat[2][1] - mat[0][1] * mat[2][2];
    cofactors[0][2] = mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1];
    cofactors[1][1] = mat[0][0] * mat[2][2] - mat[0][2] * mat[2][0];
    cofactors[1][2] = mat[0][2] * mat[1][0] - mat[0][0] * mat[1][2];
    cofactors[2][1] = mat[0][1] * mat[2][0] - mat[0][0] * mat[2][1];
    cofactors[2][2] = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];

    mat[0][0] = cofactors[0][0] * inverseDeterminant;
    mat[0][1] = cofactors[0][1] * inverseDeterminant;
    mat[0][2] = cofactors[0][2] * inverseDeterminant;
    mat[1][0] = cofactors[1][0] * inverseDeterminant;
    mat[1][1] = cofactors[1][1] * inverseDeterminant;
    mat[1][2] = cofactors[1][2] * inverseDeterminant;
    mat[2][0] = cofactors[2][0] * inverseDeterminant;
    mat[2][1] = cofactors[2][1] * inverseDeterminant;
    mat[2][2] = cofactors[2][2] * inverseDeterminant;

    return mat;
}

/* 4x4 Matrix Specializations - Not yet implemented */

template <typename T>
Matrix<4, 4, T> operator*(const Matrix<4, 4, T>& lhs,
                          const Matrix<4, 4, T>& rhs)
{
    Matrix<4, 4, T> result;
    result[0][0] =
            lhs[0][0] * rhs[0][0] + lhs[0][1] * rhs[1][0] +
            lhs[0][2] * rhs[2][0] + lhs[0][3] * rhs[3][0];
    result[0][1] =
            lhs[0][0] * rhs[0][1] + lhs[0][1] * rhs[1][1] +
            lhs[0][2] * rhs[2][1] + lhs[0][3] * rhs[3][1];
    result[0][2] =
            lhs[0][0] * rhs[0][2] + lhs[0][1] * rhs[1][2] +
            lhs[0][2] * rhs[2][2] + lhs[0][3] * rhs[3][2];
    result[0][3] =
            lhs[0][0] * rhs[0][3] + lhs[0][1] * rhs[1][3] +
            lhs[0][2] * rhs[2][3] + lhs[0][3] * rhs[3][3];

    result[1][0] =
            lhs[1][0] * rhs[0][0] + lhs[1][1] * rhs[1][0] +
            lhs[1][2] * rhs[2][0] + lhs[1][3] * rhs[3][0];
    result[1][1] =
            lhs[1][0] * rhs[0][1] + lhs[1][1] * rhs[1][1] +
            lhs[1][2] * rhs[2][1] + lhs[1][3] * rhs[3][1];
    result[1][2] =
            lhs[1][0] * rhs[0][2] + lhs[1][1] * rhs[1][2] +
            lhs[1][2] * rhs[2][2] + lhs[1][3] * rhs[3][2];
    result[1][3] =
            lhs[1][0] * rhs[0][3] + lhs[1][1] * rhs[1][3] +
            lhs[1][2] * rhs[2][3] + lhs[1][3] * rhs[3][3];

    result[2][0] =
            lhs[2][0] * rhs[0][0] + lhs[2][1] * rhs[1][0] +
            lhs[2][2] * rhs[2][0] + lhs[2][3] * rhs[3][0];
    result[2][1] =
            lhs[2][0] * rhs[0][1] + lhs[2][1] * rhs[1][1] +
            lhs[2][2] * rhs[2][1] + lhs[2][3] * rhs[3][1];
    result[2][2] =
            lhs[2][0] * rhs[0][2] + lhs[2][1] * rhs[1][2] +
            lhs[2][2] * rhs[2][2] + lhs[2][3] * rhs[3][2];
    result[2][3] =
            lhs[2][0] * rhs[0][3] + lhs[2][1] * rhs[1][3] +
            lhs[2][2] * rhs[2][3] + lhs[2][3] * rhs[3][3];

    result[3][0] =
            lhs[3][0] * rhs[0][0] + lhs[3][1] * rhs[1][0] +
            lhs[3][2] * rhs[2][0] + lhs[3][3] * rhs[3][0];
    result[3][1] =
            lhs[3][0] * rhs[0][1] + lhs[3][1] * rhs[1][1] +
            lhs[3][2] * rhs[2][1] + lhs[3][3] * rhs[3][1];
    result[3][2] =
            lhs[3][0] * rhs[0][2] + lhs[3][1] * rhs[1][2] +
            lhs[3][2] * rhs[2][2] + lhs[3][3] * rhs[3][2];
    result[3][3] =
            lhs[3][0] * rhs[0][3] + lhs[3][1] * rhs[1][3] +
            lhs[3][2] * rhs[2][3] + lhs[3][3] * rhs[3][3];
    return result;
}

}
}

#endif // _MATHMATRIX_INL_
