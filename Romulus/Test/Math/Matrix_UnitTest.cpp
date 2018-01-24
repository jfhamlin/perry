#include "Math/Matrix.h"
#include <boost/test/auto_unit_test.hpp>

using namespace romulus::math;

BOOST_AUTO_TEST_CASE(Test3By3)
{
    Matrix33 I3 = Matrix33();
    SetIdentity(I3);

    Matrix33 matrixA = Matrix33();
    matrixA[0][0] = 1; matrixA[0][1] = 2; matrixA[0][2] = 3;
    matrixA[1][0] = 2; matrixA[1][1] = 4; matrixA[1][2] = 2;
    matrixA[2][0] = 0; matrixA[2][1] = 1; matrixA[2][2] = 4;

    Matrix33 matrixB = Matrix33();
    matrixB[0][0] = 1; matrixB[0][1] = 2; matrixB[0][2] = 0;
    matrixB[1][0] = 2; matrixB[1][1] = 4; matrixB[1][2] = 1;
    matrixB[2][0] = 3; matrixB[2][1] = 2; matrixB[2][2] = 4;

    Matrix33 result = Matrix33();
    result[0][0] = 14; result[0][1] = 16; result[0][2] = 14;
    result[1][0] = 16; result[1][1] = 24; result[1][2] = 12;
    result[2][0] = 14; result[2][1] = 12; result[2][2] = 17;

    BOOST_CHECK(matrixA == matrixA);
    BOOST_CHECK(I3 != matrixA);
    BOOST_CHECK(matrixA * matrixB == result);
    BOOST_CHECK(Transposition(matrixA) == matrixB);
    BOOST_CHECK(Transpose(matrixA) == matrixB);

    Transpose(matrixA);
    BOOST_CHECK(Inverse(matrixA) * matrixA == I3);
    BOOST_CHECK(Inverse(matrixB) * matrixB == I3);
    BOOST_CHECK(Inverse(I3) == I3);

    matrixB = matrixA;
    BOOST_CHECK(Invert(matrixA) * matrixB == I3);
    BOOST_CHECK(Invert(I3) == I3);

    matrixA[0][0] = 1; matrixA[0][1] = 5; matrixA[0][2] = 2;
    matrixA[1][0] = 1; matrixA[1][1] = 1; matrixA[1][2] = 7;
    matrixA[2][0] = 0; matrixA[2][1] = -3; matrixA[2][2] = 4;

    matrixB[0][0] = -25; matrixB[0][1] = 26; matrixB[0][2] = -33;
    matrixB[1][0] = 4; matrixB[1][1] = -4; matrixB[1][2] = 5;
    matrixB[2][0] = 3; matrixB[2][1] = -3; matrixB[2][2] = 4;

    BOOST_CHECK(Inverse(matrixA) == matrixB);
    BOOST_CHECK(Inverse(matrixB) == matrixA);
    BOOST_CHECK(matrixA * matrixB == I3);
    BOOST_CHECK(Invert(matrixA) == matrixB);
}

BOOST_AUTO_TEST_CASE(Test4By4)
{
    Matrix44 I4 = Matrix44();
    SetIdentity(I4);

    Matrix44 matrixA = Matrix44();
    matrixA[0][0] = 1; matrixA[0][1] = 2; matrixA[0][2] = 3; matrixA[0][3] = -4;
    matrixA[1][0] = 2; matrixA[1][1] = 4; matrixA[1][2] = 2; matrixA[1][3] = 1;
    matrixA[2][0] = 0; matrixA[2][1] = -1; matrixA[2][2] = 4; matrixA[2][3] = 5;
    matrixA[3][0] = 2; matrixA[3][1] = 2; matrixA[3][2] = 2; matrixA[3][3] = 2;

    Matrix44 matrixB = Matrix44();
    matrixB[0][0] = 1; matrixB[0][1] = 2; matrixB[0][2] = 0; matrixB[0][3] = 2;
    matrixB[1][0] = 2; matrixB[1][1] = -4; matrixB[1][2] = 1; matrixB[1][3] = 1;
    matrixB[2][0] = 3; matrixB[2][1] = 2; matrixB[2][2] = 4; matrixB[2][3] = -5;
    matrixB[3][0] = 4; matrixB[3][1] = -2.3f; matrixB[3][2] = 0; matrixB[3][3] = 1;

    BOOST_CHECK(matrixA == matrixA && I4 != matrixA);
    BOOST_CHECK(Inverse(matrixA) * matrixA == I4);
    BOOST_CHECK(Inverse(matrixB) * matrixB == I4);
    BOOST_CHECK(Inverse(I4) == I4);

    matrixB = matrixA;
    BOOST_CHECK(Invert(matrixA) * matrixB == I4);
    BOOST_CHECK(Invert(I4) == I4);

    matrixA[0][0] = 1; matrixA[0][1] = 2; matrixA[0][2] = 3; matrixA[0][3] = -4;
    matrixA[1][0] = 1; matrixA[1][1] = 2; matrixA[1][2] = 3; matrixA[1][3] = -4;
    matrixA[2][0] = 0; matrixA[2][1] = -1; matrixA[2][2] = 4; matrixA[2][3] = 5;
    matrixA[3][0] = 2; matrixA[3][1] = 2; matrixA[3][2] = 2; matrixA[3][3] = 2;

    struct EmptyPredicate
    {
        bool operator()(const std::exception& e) { return true; }
    };

    BOOST_CHECK_EXCEPTION(Invert(matrixA);, BadMatrixInversionException,
                          EmptyPredicate());
}
