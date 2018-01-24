#include "Math/Intersections.h"
#include "Math/Utilities.h"
#include <boost/test/auto_unit_test.hpp>

using namespace romulus::math;

BOOST_AUTO_TEST_CASE(RayIntersections)
{
    Ray ray1(Vector3(0, 0, -5), Vector3(0, 0, 1));

    Sphere sphere1(Vector3(0, 0, 0), 1);

    Plane plane1(Vector3(0, 0, 1), 0.5);

    AxisAlignedBox aab1(-1, -1, -1, 1, 1, 1);

    real_t hitTime;
    Vector3 hitPoint;

    // Sphere Hit
    BOOST_CHECK(Intersects(hitTime, ray1, sphere1));
    hitPoint = ray1.PointAt(hitTime);
    BOOST_CHECK(hitPoint == Vector3(0, 0, -1));

    // Plane Hit
    BOOST_CHECK(Intersects(hitTime, ray1, plane1));
    hitPoint = ray1.PointAt(hitTime);
    BOOST_CHECK(hitPoint == Vector3(0, 0, 0.5));

    // AAB Hit
    BOOST_CHECK(Intersects(hitTime, ray1, aab1));
    hitPoint = ray1.PointAt(hitTime);
    BOOST_CHECK(hitPoint == Vector3(0, 0, -1));

    ray1 = Ray(Vector3(0, 0, -5), Vector3(1, 0, 0));

    // Sphere Miss
    BOOST_CHECK(!Intersects(hitTime, ray1, sphere1));

    // Plane Miss
    BOOST_CHECK(!Intersects(hitTime, ray1, plane1));

    // AAB Miss
    BOOST_CHECK(!Intersects(hitTime, ray1, aab1));
}

BOOST_AUTO_TEST_CASE(AAB_AABTest)
{
    AxisAlignedBox aab1(-2, -2, -2, 1, 1, 1);
    AxisAlignedBox aab2(-1, -1, -1, 2, 2, 2);
    AxisAlignedBox intersection(-1, -1, -1, 1, 1, 1);

    AxisAlignedBox result;
    BOOST_CHECK(Intersects(result, aab1, aab2));
    BOOST_CHECK(result == intersection);

    BOOST_CHECK(Intersects(aab1, aab2));

    aab2.SetMinX(1.5); aab2.SetMinY(1.5); aab2.SetMinZ(1.5);
    BOOST_CHECK(!Intersects(result, aab1, aab2));
    BOOST_CHECK(!Intersects(aab1, aab2));

}

BOOST_AUTO_TEST_CASE(SphereSphereTest)
{
    Sphere s1(Vector3(0, 0, 0), 1);
    Sphere s2(Vector3(1, 1, 1), 1);

    BOOST_CHECK(Intersects(s1, s1));
    BOOST_CHECK(Intersects(s2, s2));
    BOOST_CHECK(Intersects(s1, s2));

    s2.SetRadius(0.1f);
    BOOST_CHECK(!Intersects(s1, s2));
}

BOOST_AUTO_TEST_CASE(PlanePlaneTest)
{
    Plane p1(Vector3(1, 0, 0), 0);
    Plane p2(Normal(Vector3(1, 1, 1)), 10);
    Plane p3(Normal(Vector3(-1, -1, -1)), -10);

    BOOST_CHECK(Intersects(p1, p1));
    BOOST_CHECK(Intersects(p2, p2));
    BOOST_CHECK(Intersects(p1, p2));
    BOOST_CHECK(Intersects(p1, p3));
    BOOST_CHECK(!Intersects(p2, p3));
}

BOOST_AUTO_TEST_CASE(AAB_SphereTest)
{
    AxisAlignedBox aab1(-1, -1, -1, 1, 1, 1);
    Sphere sphere1(Vector3(1, 1, 1), 1);

    BOOST_CHECK(Intersects(aab1, sphere1));

    sphere1.SetCenter(Vector3(5, 5, 5));
    BOOST_CHECK(!Intersects(aab1, sphere1));
}

BOOST_AUTO_TEST_CASE(AAB_PlaneTest)
{
    AxisAlignedBox aab1(-1, -1, -1, 1, 1, 1);
    Plane plane1(Normal(Vector3(1, 1, 1)), 0);
    BOOST_CHECK(Intersects(aab1, plane1));

    plane1.SetNormal(Normal(Vector3(1, 1, -1)));
    BOOST_CHECK(Intersects(aab1, plane1));

    plane1.SetNormal(Normal(Vector3(-1, 1, -1)));
    BOOST_CHECK(Intersects(aab1, plane1));

    plane1.SetNormal(Normal(Vector3(-1, 1, 1)));
    BOOST_CHECK(Intersects(aab1, plane1));

    plane1.SetDistanceFromOrigin(2.0);
    BOOST_CHECK(!Intersects(aab1, plane1));
}

BOOST_AUTO_TEST_CASE(PlaneSphereTest)
{
    Plane plane(Normal(Vector3(1)), 0);
    Sphere sphere(Vector3(0, 0, 0), 1);

    BOOST_CHECK(Intersects(plane, sphere));

    plane.SetDistanceFromOrigin(1.5);
    BOOST_CHECK(!Intersects(plane, sphere));
}

BOOST_AUTO_TEST_CASE(AAB_TriangleTest)
{
    AxisAlignedBox aab1(-1, -1, -1, 1, 1, 1);
    Vector3 v0(0, 0, 0);
    Vector3 v1(1, 1, 1);
    Vector3 v2(-1, 0, 0);
    BOOST_CHECK(Intersects(aab1, v0, v1, v2));

    v0 = Vector3(0, 0, 2);
    v1 = Vector3(-2, 0, 0);
    v2 = Vector3(0, -2, 0);
    BOOST_CHECK(Intersects(aab1, v0, v1, v2));

    v0 = Vector3(0, .5, 2);
    v1 = Vector3(0, .5, -2);
    v2 = Vector3(0, 2, 0);
    BOOST_CHECK(Intersects(aab1, v0, v1, v2));

    v0 = Vector3(-1, -2, 2);
    v1 = Vector3(-1, 2, 0);
    v2 = Vector3(-1, 0, -2);
    BOOST_CHECK(Intersects(aab1, v0, v1, v2));
}

BOOST_AUTO_TEST_CASE(AAB_FrustumTest)
{
    AxisAlignedBox aab1(-1, -1, -6, 1, 1, -5);
    AxisAlignedBox aab2(-1000, -1000, -110, 1000, 1000, 0);
    Frustum frustum;
    frustum.Compute(GeneratePerspectiveProjectionTransform(
                            DegreesToRadians(80.f), 1.3333f, 1.f, 100.f));

    BOOST_CHECK(Intersects(aab1, frustum));
    BOOST_CHECK(Intersects(aab2, frustum));

    AxisAlignedBox aab3(1, 1, 1, 2, 2, 2);
    AxisAlignedBox aab4(-1000, -1, -6, -900, 1, -5);

    BOOST_CHECK(!Intersects(aab3, frustum));
    BOOST_CHECK(!Intersects(aab4, frustum));
}

BOOST_AUTO_TEST_CASE(FrustumSphereTest)
{
    Sphere sphere1(Vector3(0, 0, 0), 10);
    Sphere sphere2(Vector3(0, 0, -10), 200);
    Frustum frustum;
    frustum.Compute(GeneratePerspectiveProjectionTransform(
                            DegreesToRadians(80.f), 1.3333f, 1.f, 100.f));

    BOOST_CHECK(Intersects(frustum, sphere1));
    BOOST_CHECK(Intersects(frustum, sphere2));

    Sphere sphere3(Vector3(0, 0, 10), 10);
    Sphere sphere4(Vector3(-1000, 0, -10), 200);

    BOOST_CHECK(!Intersects(frustum, sphere3));
    BOOST_CHECK(!Intersects(frustum, sphere4));

    frustum.Compute(GeneratePerspectiveProjectionTransform(
                            DegreesToRadians(80.f), 1.3333f, 1.f, 100.f) *
                    GenerateRotationTransform(Vector3(0, 1, 0), .5f * Pi));

    BOOST_CHECK(Intersects(frustum, sphere1));
    BOOST_CHECK(Intersects(frustum, sphere2));

    sphere3.SetCenter(Vector3(50, 0, 0));
    sphere4.SetCenter(Vector3(200, 0, 0));
    sphere4.SetRadius(99.f);

    BOOST_CHECK(Intersects(frustum, sphere3));
    BOOST_CHECK(!Intersects(frustum, sphere4));

    sphere1.SetCenter(Vector3(-10, 0, 0));

    BOOST_CHECK(!Intersects(frustum, sphere1));
}
