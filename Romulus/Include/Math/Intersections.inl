#ifndef _MATHINTERSECTIONS_INL_
#define _MATHINTERSECTIONS_INL_

//! \file Intersections.inl
//! Contains inline implementations of intersection functions.

#include <cmath>

namespace romulus
{
namespace math
{
/* Ray-Sphere intersecton */

bool Intersects(real_t& hitTime,
                const Ray& ray, const Sphere& sphere)
{
    Vector3 translatedRayOrigin = ray.Origin - sphere.Center();
    real_t a = Dot(ray.Direction, ray.Direction);
    real_t b = Dot(translatedRayOrigin, ray.Direction);
    real_t c = Dot(translatedRayOrigin, translatedRayOrigin) -
            sphere.Radius() * sphere.Radius();

    real_t descriminant = b * b - a * c;

    // If there is no real solution to the line/sphere equation,
    // there is no hit
    if (descriminant < 0) return false;

    real_t descriminantSquareRoot = sqrt(descriminant);

    //! \todo If it can be assumed that the ray direction is normalized,
    //! then the division by a below can continue to be omitted.
    //! See if we can enforce this
    ASSERT(IsUnitLength(ray.Direction));
    real_t intersectTime = (-b - descriminantSquareRoot); // / a;

    if (intersectTime < 0)
    {
        intersectTime = (-b + descriminantSquareRoot); // / a;
        if (intersectTime < 0)
            return false;
    }

    hitTime = intersectTime;
    return true;
}

/* Ray-Plane intersection */

bool Intersects(real_t& hitTime,
                const Ray& ray, const Plane& plane)
{
    real_t denominator = Dot(plane.Normal(), ray.Direction);
    real_t numerator = plane.DistanceFromOrigin() -
            Dot(plane.Normal(), ray.Origin);

    if (ApproxEqual(denominator, static_cast<real_t>(0.0)))
    {
        if (ApproxEqual(numerator, static_cast<real_t>(0.0)))
        {
            hitTime = 0.f;
            return true;   // The ray lies on the plane
        }
        else
        {
            return false;  // The ray is parallel to but not on the plane
        }
    }

    real_t intersectTime = numerator / denominator;

    if (intersectTime < 0) return false;

    hitTime = intersectTime;
    return true;
}

/* Ray-AxisAlignedBox intersection */

bool Intersects(real_t& hitTime,
                const Ray& ray, const AxisAlignedBox& aab)
{
    real_t txmin, txmax, tymin, tymax, tzmin, tzmax;

    real_t a = 1.0f / ray.Direction[0];
    if (a >= 0)     // Then it's moving from min x to max x
    {
        txmin = a * (aab.MinX() - ray.Origin[0]);
        txmax = a * (aab.MaxX() - ray.Origin[0]);
    }
    else
    {
        txmin = a * (aab.MaxX() - ray.Origin[0]);
        txmax = a * (aab.MinX() - ray.Origin[0]);
    }

    a = 1.0f / ray.Direction[1];
    if (a >= 0)     // Then it's moving from min y to max y
    {
        tymin = a * (aab.MinY() - ray.Origin[1]);
        tymax = a * (aab.MaxY() - ray.Origin[1]);
    }
    else
    {
        tymin = a * (aab.MaxY() - ray.Origin[1]);
        tymax = a * (aab.MinY() - ray.Origin[1]);
    }

    if (txmin > tymax || tymin > txmax) return false;

    a = 1.0f / ray.Direction[2];
    if (a >= 0)     // Then it's moving from min y to max y
    {
        tzmin = a * (aab.MinY() - ray.Origin[2]);
        tzmax = a * (aab.MaxY() - ray.Origin[2]);
    }
    else
    {
        tzmin = a * (aab.MaxY() - ray.Origin[2]);
        tzmax = a * (aab.MinY() - ray.Origin[2]);
    }

    real_t tmin = Max(txmin, Max(tymin, tzmin));
    real_t tmax = Min(txmax, Min(tymax, tzmax));

    if (tmax > tmin){
        if (tmin >= 0)
            hitTime = tmin;
        else    // ray begins in the box
            hitTime = tmax;
        return true;
    }

    return false;
}

/* Ray-Triangle intersection */

inline bool Intersects(real_t& hitTime, const Ray& ray,
                       const Vector3& v0, const Vector3& v1,
                       const Vector3& v2)
{
    hitTime = 0;
    real_t u = 0, v = 0;

    // Get vectors for the two edges that share v0
    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;

    Vector3 tvec, pvec, qvec;
    float det, inv_det;

    // Begin calculating determinant
    pvec = Cross(ray.Direction, edge2);

    // If the determinant is near zero, ray lies in plane of triangle
    det = Dot(edge1, pvec);

    // Account for Float rounding errors / inaccuracies.
    if (det > -AcceptableMathEpsilon && det < AcceptableMathEpsilon)
        return false;

    // Get the inverse determinant
    inv_det = 1.0f / det;

    // Calculate distance from v0 to ray origin
    tvec = ray.Origin - v0;

    // Calculate U parameter and test bounds
    u = Dot(tvec, pvec) * inv_det;
    if (u < 0.0f || u > 1.0f)
        return false;

    // Prepare for v
    qvec = Cross(tvec, edge1);

    // Calculate V parameter and test bounds
    v = Dot(ray.Direction, qvec) * inv_det;
    if (v < 0.0f || u + v > 1.0f)
        return false;

    // Calculate t, ray intersects triangle.
    hitTime = Dot(edge2, qvec) * inv_det;

    if (hitTime < 0)
        return false;
    return true;
}

/* AxisAlignedBox-AxisAlignedBox intersection */

bool Intersects(AxisAlignedBox& intersection,
                const AxisAlignedBox& aab1, const AxisAlignedBox& aab2)
{
    Vector3 minCorner, maxCorner;
    minCorner[0] = Max(aab1.MinX(), aab2.MinX());
    maxCorner[0] = Min(aab1.MaxX(), aab2.MaxX());
    if ((maxCorner[0] - minCorner[0]) < 0.0)
        return false;

    minCorner[1] = Max(aab1.MinY(), aab2.MinY());
    maxCorner[1] = Min(aab1.MaxY(), aab2.MaxY());
    if ((maxCorner[1] - minCorner[1]) < 0.0)
        return false;

    minCorner[2] = Max(aab1.MinZ(), aab2.MinZ());
    maxCorner[2] = Min(aab1.MaxZ(), aab2.MaxZ());
    if ((maxCorner[2] - minCorner[2]) < 0.0)
        return false;

    intersection = AxisAlignedBox(minCorner, maxCorner);
    return true;
}

bool Intersects(const AxisAlignedBox& aab1, const AxisAlignedBox& aab2)
{
    if (Min(aab1.MaxX(), aab2.MaxX()) -
        Max(aab1.MinX(), aab2.MinX()) < 0.0)
        return false;

    if (Min(aab1.MaxY(), aab2.MaxY()) -
        Max(aab1.MinY(), aab2.MinY()) < 0.0)
        return false;

    if (Min(aab1.MaxZ(), aab2.MaxZ()) -
        Max(aab1.MinZ(), aab2.MinZ()) < 0.0)
        return false;

    return true;
}

/* Sphere-Sphere intersection */

bool Intersects(const Sphere& sphere1, const Sphere& sphere2)
{
    real_t centerDistSquared = MagnitudeSquared(sphere2.Center() -
                                                sphere1.Center());
    real_t radiusSum = sphere1.Radius() + sphere2.Radius();
    if (centerDistSquared > radiusSum * radiusSum)
        return false;
    return true;
}

/* Plane-Plane intersection */

bool Intersects(const Plane& plane1, const Plane& plane2)
{
    if (!ApproxEqual(static_cast<real_t>(
                             fabs(Dot(plane1.Normal(), plane2.Normal()))),
                     static_cast<real_t>(1.0)) ||
        ApproxEqual(plane1.DistanceFromOrigin(),
                    plane2.DistanceFromOrigin()))
        return true;
    return false;
}

/* Line segment intersection */

inline bool Intersects(Vector2& intersection,
                       const Vector2& s0, const Vector2& s1,
                       const Vector2& t0, const Vector2& t1)
{
    float denom = ((t1[1] - t0[1])*(s1[0] - s0[0])) -
            ((t1[0] - t0[0])*(s1[1] - s0[1]));

    float nume_a = ((t1[0] - t0[0])*(s0[1] - t0[1])) -
            ((t1[1] - t0[1])*(s0[0] - t0[0]));

    float nume_b = ((s1[0] - s0[0])*(s0[1] - t0[1])) -
            ((s1[1] - s0[1])*(s0[0] - t0[0]));

    if(denom == 0.0f)
    {
        if(nume_a == 0.0f && nume_b == 0.0f)
        {
            //! \todo - This is wrong... should pick a point known to be
            //!         in the coincident piece of the segments.
            intersection = s0;
            return true;
        }
        return false;
    }

    float ua = nume_a / denom;
    float ub = nume_b / denom;

    if(ua > 0.0f && ua < 1.0f && ub > 0.0f && ub < 1.0f)
    {
        // Get the intersection point.
        intersection[0] = s0[0] + ua*(s1[0] - s0[0]);
        intersection[1] = s0[1] + ua*(s1[1] - s0[1]);

        return true;
    }

    return false;
}

/* AxisAlignedBox-Sphere intersection */

bool Intersects(const AxisAlignedBox& aab, const Sphere& sphere)
{
    real_t distSquared = 0;
    for (uint_t i = 0; i < 3; ++i)
    {
        if (sphere.Center()[i] < aab.MinCorner()[i])
        {
            real_t dist = sphere.Center()[i] - aab.MinCorner()[i];
            distSquared += dist * dist;
        }
        else if (sphere.Center()[i] > aab.MaxCorner()[i])
        {
            real_t dist = sphere.Center()[i] - aab.MaxCorner()[i];
            distSquared += dist * dist;
        }
    }
    return distSquared <= sphere.Radius() * sphere.Radius();
}

/* AxisAlignedBox-Plane intersection */

bool Intersects(const AxisAlignedBox& aab, const Plane& plane)
{
    real_t dot0 = fabs(Dot(aab.MaxCorner() - aab.MinCorner(),
                           plane.Normal()));

    Vector3 diag1Max(aab.MaxX(), aab.MaxY(), aab.MinZ());
    Vector3 diag1Min(aab.MinX(), aab.MinY(), aab.MaxZ());
    real_t dot1 = fabs(Dot(diag1Max - diag1Min, plane.Normal()));

    Vector3 diag2Max(aab.MinX(), aab.MaxY(), aab.MinZ());
    Vector3 diag2Min(aab.MaxX(), aab.MinY(), aab.MaxZ());
    real_t dot2 = fabs(Dot(diag2Max - diag2Min, plane.Normal()));

    Vector3 diag3Max(aab.MinX(), aab.MaxY(), aab.MaxZ());
    Vector3 diag3Min(aab.MaxX(), aab.MinY(), aab.MinZ());
    real_t dot3 = fabs(Dot(diag3Max - diag3Min, plane.Normal()));

    real_t maxDot = Max(dot0, Max(dot1, Max(dot2, dot3)));

    if (ApproxEqual(dot0, maxDot))
    {
        if (plane.WhichSide(aab.MaxCorner()) !=
            plane.WhichSide(aab.MinCorner()))
            return true;
    }
    else if (ApproxEqual(dot1, maxDot))
    {
        if (plane.WhichSide(diag1Max) != plane.WhichSide(diag1Min))
            return true;
    }
    else if (ApproxEqual(dot2, maxDot))
    {
        if (plane.WhichSide(diag2Max) !=
            plane.WhichSide(diag2Min))
            return true;
    }
    else if (ApproxEqual(dot3, maxDot))
    {
        if (plane.WhichSide(diag3Max) !=
            plane.WhichSide(diag3Min))
            return true;
    }
    else
    {
        ASSERT(false);
    }

    return false;
}

/* Plane-Sphere intersection */

bool Intersects(const Plane& plane, const Sphere& sphere)
{
    if (fabs(plane.Distance(sphere.Center())) <= sphere.Radius())
        return true;
    return false;
}

/* AxisAlignedBox-Frustum intersection */

bool Intersects(const AxisAlignedBox& aab, const Frustum& frustum)
{
    for (int p = 0; p < 6; ++p)
    {
        Frustum::FrustumPlaneSide planeSide =
                static_cast<Frustum::FrustumPlaneSide>(p);
        int numCornersIn = 8;
        for (int v = 0; v < 8; ++v)
            if (frustum.FrustumPlane(planeSide).WhichSide(aab.Corner(v)) ==
                Plane::PlaneSide_BackOfPlane)
                --numCornersIn;
        if (numCornersIn == 0)
            return false;
    }
    return true;
}

/* Frustum-Sphere intersection */

bool Intersects(const Frustum& frustum, const Sphere& sphere)
{
    for (int p = 0; p < 6; ++p)
    {
        Frustum::FrustumPlaneSide planeSide =
                static_cast<Frustum::FrustumPlaneSide>(p);
        if (frustum.FrustumPlane(planeSide).Distance(sphere.Center()) <
            - sphere.Radius())
            return false;
    }
    return true;
}

}
}

#endif // _MATHINTERSECTIONS_INL_
