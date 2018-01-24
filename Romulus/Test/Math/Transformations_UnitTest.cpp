#include "Math/Constants.h"
#include "Math/Transformations.h"
#include "Math/Utilities.h"
#include "Math/Vector.h"
#include <boost/test/auto_unit_test.hpp>
#include <cmath>

using namespace romulus;
using namespace romulus::math;

BOOST_AUTO_TEST_CASE(RotationMethodsAndFunctions)
{
    // We test the rotation with a range of axes and angles.
    for (int i = 0; i < 5; ++i)
    {
        Vector3 axis(0.f);
        if (i < 3)
            axis[i] = 1.f;
        else if (i == 3)
            // Test with a non-unit length axis.
            axis = Vector3(1.0, 2.0, 0.5);
        else if (i == 4)
            // Test with a non-axis vector.
            axis = Normal(Vector3(-1, 0.5, -2));
        Vector3 normalAxis = Normal(axis);
        for (real_t theta = - 2.0 * Pi; theta < 2.0 * Pi; theta += .1 * Pi)
        {
            Rotation r(axis, theta);

            // Test Axis/Angle accessors.
            if (r.Axis() == normalAxis)
                BOOST_CHECK(ApproxEqual(fmod(r.Angle(), 2.0 * Pi),
                                        fmod(theta, 2.0 * Pi)));
            else if (r.Axis() == -1.0 * normalAxis)
                BOOST_CHECK(ApproxEqual(fmod(r.Angle(), 2.0 * Pi),
                                        -fmod(theta, 2.0 * Pi)));
            else
                BOOST_CHECK(false);

            Matrix44 m = GenerateRotationTransform(normalAxis, theta);

            // We check that vectors projected by this matrix and the Rotation's
            // generated matrix are identical.
            Vector4 v(1, 0, 0, 1);
            BOOST_CHECK(m * v == r.Matrix() * v);

            v = Vector4(0, 1, 0, 1);
            BOOST_CHECK(m * v == r.Matrix() * v);

            v = Vector4(0, 0, 1, 1);
            BOOST_CHECK(m * v == r.Matrix() * v);

            v = Vector4(1, 2, 3, 1);
            BOOST_CHECK(m * v == r.Matrix() * v);

            v = Vector4(0, 0, 0, 1);
            BOOST_CHECK(m * v == r.Matrix() * v);

            // Test inversion and SetIdentity functions.
            BOOST_CHECK(Inverse(r.Matrix()) == Inverse(r).Matrix());
            Invert(r);
            BOOST_CHECK(Inverse(r.Matrix()) == Inverse(r).Matrix());
            SetIdentity(r);
            Matrix44 identity;
            SetIdentity(identity);
            BOOST_CHECK(r.Matrix() == identity);
        }
    }

    Vector4 v(1, 0, 0, 1);
    Rotation r(Vector3(0, 1, 0), DegreesToRadians(90.0));

    // Rotate around y.
    BOOST_CHECK(r.Matrix() * v == Vector4(0, 0, -1, 1));

    r.SetAxisAngle(r.Axis(), DegreesToRadians(-90.f));
    BOOST_CHECK(r.Matrix() * v == Vector4(0, 0, 1, 1));

    // Rotate around x.
    v = Vector4(0, 1, 0, 1);
    r.SetAxisAngle(Vector3(1, 0, 0), DegreesToRadians(-90.f));
    BOOST_CHECK(r.Matrix() * v == Vector4(0, 0, -1, 1));

    r.SetAxisAngle(Vector3(1, 0, 0), DegreesToRadians(90.f));
    BOOST_CHECK(r.Matrix() * v == Vector4(0, 0, 1, 1));

    // Rotate around z.
    v = Vector4(0, 1, 0, 1);
    r.SetAxisAngle(Vector3(0, 0, 1), DegreesToRadians(-90.f));
    BOOST_CHECK(r.Matrix() * v == Vector4(1, 0, 0, 1));

    r.SetAxisAngle(Vector3(0, 0, 1), DegreesToRadians(90.f));
    BOOST_CHECK(r.Matrix() * v == Vector4(-1, 0, 0, 1));
}

BOOST_AUTO_TEST_CASE(UniformScaleMethodsAndFunctions)
{
    UniformScale s(2);

    BOOST_CHECK(s.Value() == 2);

    Matrix44 m;
    SetIdentity(m);
    m[0][0] = 2;
    m[1][1] = 2;
    m[2][2] = 2;

    BOOST_CHECK(s.Matrix() == m);

    UniformScale s2 = s;
    BOOST_CHECK(s.Value() == s2.Value());
    Invert(s);
    BOOST_CHECK(s.Value() == Inverse(s2).Value());
    BOOST_CHECK(s.Value() == .5f);
    BOOST_CHECK(Inverse(s.Matrix()) == Inverse(s).Matrix());
    SetIdentity(s);
    BOOST_CHECK(s.Value() == 1.f);
}

BOOST_AUTO_TEST_CASE(TranslationMethodsAndFunctions)
{
    Vector3 v(1, 2, 3);
    Translation t(v);

    BOOST_CHECK(t.Vector() == v);

    Matrix44 m;
    SetIdentity(m);
    m[0][3] = v[0];
    m[1][3] = v[1];
    m[2][3] = v[2];

    BOOST_CHECK(t.Matrix() == m);

    Translation t2 = t;
    BOOST_CHECK(t.Vector() == t2.Vector());
    Invert(t);
    BOOST_CHECK(t.Vector() == Inverse(t2).Vector());
    BOOST_CHECK(t.Vector() == -1.f * t2.Vector());
    BOOST_CHECK(Inverse(t.Matrix()) == Inverse(t).Matrix());
    SetIdentity(t);
    BOOST_CHECK(t.Vector() == Vector3(0.f));
}

BOOST_AUTO_TEST_CASE(TransformationMethodsAndFunctions)
{
    Rotation r(Vector3(1, 1, 1), .5f);
    UniformScale s(1.5);
    Translation t(1, 2, 3);
    Transformation x(t, r, s);

    BOOST_CHECK(x.Matrix() == t.Matrix() * r.Matrix() * s.Matrix());

    SetIdentity(x);
    Matrix44 identity;
    SetIdentity(identity);
    BOOST_CHECK(x.Matrix() == identity);
}

BOOST_AUTO_TEST_CASE(TransformationApplication)
{
    Rotation r(Vector3(1, 0, 0), Pi);
    UniformScale s(2);
    Translation t(Vector3(1.f));
    Transformation x(t, r, s);

    Vector3 v(0, 1, 0);
    BOOST_CHECK(TransformPoint(r, v) == Vector3(0, -1, 0));
    BOOST_CHECK(TransformPoint(s, v) == Vector3(0, 2, 0));
    BOOST_CHECK(TransformPoint(t, v) == Vector3(1, 2, 1));
    BOOST_CHECK(TransformPoint(x, v) == Vector3(1, -1, 1));
    BOOST_CHECK(TransformDirection(r, v) == Vector3(0, -1, 0));
    BOOST_CHECK(TransformDirection(x, v) == Vector3(0, -1, 0));
}

#define CHECK_CONCATENATION2(xform1, xform2) \
    BOOST_CHECK(Concatenate(xform1, xform2).Matrix() == \
                xform1.Matrix() * xform2.Matrix())

#define CHECK_CONCATENATION3(xform1, xform2, xform3) \
    BOOST_CHECK(Concatenate(Concatenate(xform1, xform2), xform3).Matrix() == \
                xform1.Matrix() * xform2.Matrix() * xform3.Matrix()); \
    BOOST_CHECK(Concatenate(xform1, Concatenate(xform2, xform3)).Matrix() == \
                xform1.Matrix() * xform2.Matrix() * xform3.Matrix())

BOOST_AUTO_TEST_CASE(TransformationConcatenation)
{
    Translation t(-2, .2f, 0.1f);
    Rotation r(Vector3(1, 1, 1), .5f * Pi);
    UniformScale s(-.6f);
    Transformation x(t, r, s);

    CHECK_CONCATENATION2(t, t);
    CHECK_CONCATENATION2(r, r);
    CHECK_CONCATENATION2(s, s);
    CHECK_CONCATENATION2(x, x);
    CHECK_CONCATENATION2(t, r);
    CHECK_CONCATENATION2(t, s);
    CHECK_CONCATENATION2(t, x);
    CHECK_CONCATENATION2(r, t);
    CHECK_CONCATENATION2(r, s);
    CHECK_CONCATENATION2(r, x);
    CHECK_CONCATENATION2(s, t);
    CHECK_CONCATENATION2(s, r);
    CHECK_CONCATENATION2(s, x);
    CHECK_CONCATENATION2(x, t);
    CHECK_CONCATENATION2(x, r);
    CHECK_CONCATENATION2(x, s);

    CHECK_CONCATENATION3(t, t, t);
    CHECK_CONCATENATION3(r, r, r);
    CHECK_CONCATENATION3(s, s, s);
    CHECK_CONCATENATION3(t, r, s);
    CHECK_CONCATENATION3(t, s, r);
    CHECK_CONCATENATION3(r, s, t);
    CHECK_CONCATENATION3(r, t, s);
    CHECK_CONCATENATION3(s, r, t);
    CHECK_CONCATENATION3(s, t, r);
    CHECK_CONCATENATION3(x, x, x);
    CHECK_CONCATENATION3(s, r, x);
    CHECK_CONCATENATION3(r, s, x);
    CHECK_CONCATENATION3(t, s, x);
}
