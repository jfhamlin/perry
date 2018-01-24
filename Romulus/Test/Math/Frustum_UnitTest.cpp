#include "Math/Frustum.h"
#include "Math/Utilities.h"
#include <boost/test/auto_unit_test.hpp>

using namespace romulus;
using namespace romulus::math;


BOOST_AUTO_TEST_CASE(TestNearDistance)
{
    Frustum frust;
    frust.Compute(GeneratePerspectiveProjectionTransform(
                          DegreesToRadians(80.0), 1.3333, 1.0, 100.0));

    BOOST_CHECK(ApproxEqual(frust.NearDistance(), 1.0));

    real_t newDistance = 10.0;
    frust.SetNearDistance(newDistance);

    BOOST_CHECK(ApproxEqual(frust.NearDistance(), newDistance));
}

BOOST_AUTO_TEST_CASE(TestFarDistance)
{
    Frustum frust;
    frust.Compute(GeneratePerspectiveProjectionTransform(
                          DegreesToRadians(80.0), 1.3333, 1.0, 100.0));

    BOOST_CHECK(ApproxEqual(frust.FarDistance(), 100.0));

    real_t newDistance = 10.f;
    frust.SetFarDistance(newDistance);

    BOOST_CHECK(ApproxEqual(frust.FarDistance(), newDistance));
}

BOOST_AUTO_TEST_CASE(TestPlanes)
{
    Frustum frust;
    //! ctor
    frust.Compute(GeneratePerspectiveProjectionTransform(
                          DegreesToRadians(80.f), 1.3333f, 1.f, 100.f));

    Plane near(Vector3(0, 0, -1), 1);
    Plane far(Vector3(0, 0, 1), -100.f);
    Plane left(Vector3(0.666422367f, 0, -0.745574474f), 0);
    Plane right(Vector3(-0.666422367f, 0, -0.745574474f), 0);
    Plane top(Vector3(0, -0.766044438f, -0.642787635f), 0);
    Plane bottom(Vector3(0, 0.766044438f, -0.642787635f), 0);

    BOOST_CHECK(frust.FrustumPlane(Frustum::Plane_Near) == near);
    BOOST_CHECK(frust.FrustumPlane(Frustum::Plane_Far) == far);
    BOOST_CHECK(frust.FrustumPlane(Frustum::Plane_Left) == left);
    BOOST_CHECK(frust.FrustumPlane(Frustum::Plane_Right) == right);
    BOOST_CHECK(frust.FrustumPlane(Frustum::Plane_Top) == top);
    BOOST_CHECK(frust.FrustumPlane(Frustum::Plane_Bottom) == bottom);

    Matrix44 projection = GeneratePerspectiveProjectionTransform(
            DegreesToRadians(90.f), 1.f, 1.f, 100.f);
    Matrix44 rotation = GenerateRotationTransform(Vector3(0, 1, 0),
                                                  DegreesToRadians(90.f));
    Matrix44 translation;
    SetIdentity(translation);
    translation[1][3] = -10.f;

    // Translate the frustum up ten units.
    frust.Compute(projection * translation);
    near = Plane(Vector3(0, 0, -1), 1);
    far = Plane(Vector3(0, 0, 1), -100.f);
    left = Plane(Normal(Vector3(1, 0, -1)), 0);
    right = Plane(Normal(Vector3(-1, 0, -1)), 0);
    bottom = Plane(Normal(Vector3(0, 1, -1)), 7.071067812);
    top = Plane(Normal(Vector3(0, -1, -1)), -7.071067812);
    BOOST_CHECK(frust.FrustumPlane(Frustum::Plane_Near) == near);
    BOOST_CHECK(frust.FrustumPlane(Frustum::Plane_Far) == far);
    BOOST_CHECK(frust.FrustumPlane(Frustum::Plane_Left) == left);
    BOOST_CHECK(frust.FrustumPlane(Frustum::Plane_Right) == right);
    BOOST_CHECK(frust.FrustumPlane(Frustum::Plane_Top) == top);
    BOOST_CHECK(frust.FrustumPlane(Frustum::Plane_Bottom) == bottom);

    // Rotate frustum to look down +x and translate it up 10 units.
    frust.Compute(projection * translation * rotation);
    near = Plane(Vector3(1, 0, 0), 1);
    far = Plane(Vector3(-1, 0, 0), -100.f);
    left = Plane(Normal(Vector3(1, 0, 1)), 0);
    right = Plane(Normal(Vector3(1, 0, -1)), 0);
    bottom = Plane(Normal(Vector3(1, 1, 0)), 7.071067812);
    top = Plane(Normal(Vector3(1, -1, 0)), -7.071067812);
    BOOST_CHECK(frust.FrustumPlane(Frustum::Plane_Near) == near);
    BOOST_CHECK(frust.FrustumPlane(Frustum::Plane_Far) == far);
    BOOST_CHECK(frust.FrustumPlane(Frustum::Plane_Left) == left);
    BOOST_CHECK(frust.FrustumPlane(Frustum::Plane_Right) == right);
    BOOST_CHECK(frust.FrustumPlane(Frustum::Plane_Top) == top);
    BOOST_CHECK(frust.FrustumPlane(Frustum::Plane_Bottom) == bottom);
}
