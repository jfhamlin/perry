#ifndef _MATHINTERSECTIONS_H_
#define _MATHINTERSECTIONS_H_

//! \file Intersections.h
//! Contains intersection methods for the various geometric objects

#include "Math/Constants.h"
#include "Math/Vector.h"
#include "Math/Ray.h"
#include "Math/Sphere.h"
#include "Math/Plane.h"
#include "Math/AxisAlignedBox.h"
#include "Math/Frustum.h"

namespace romulus
{
namespace math
{
//! Find the intersection of a ray and a sphere
//! \param hitTime - ray parameter of first hit is stored in hitTime
//! \param ray - the ray from ray.Origin to infinity
//! \param sphere - the sphere
//! \return true iff the ray intersects the sphere
inline bool Intersects(real_t& hitTime, const Ray& ray,
                       const Sphere& sphere);

//! Find the intersection of a ray and a plane
//! \param hitTime - ray parameter of first hit is stored in hitTime
//! \param ray - the ray from ray.Origin to infinity
//! \param sphere - the plane
//! \return true iff the ray intersects the plane
inline bool Intersects(real_t& hitTime, const Ray& ray, const Plane& plane);

//! Find the intersection of a ray and an AxisAlignedBox
//! \param hitTime - ray parameter of first hit is stored in hitTime
//! \param ray - the ray from ray.Origin to infinity
//! \param aab - the axis-aligned bounding box
//! \return true iff the ray intersects the AxisAlignedBox
inline bool Intersects(real_t& hitTime, const Ray& ray,
                       const AxisAlignedBox& aab);

//! Find the intersection of a ray and a triangle
//! \param hitTime - ray parameter of first hit is stored in hitTime
//! \param ray - the ray from ray.Origin to infinity
//! \param v0 - the first vertex of triangle v
//! \param v1 - the second vertex of triangle v
//! \param v2 - the third vertex of triangle v
//! \return true iff the ray intersects the triangle
inline bool Intersects(real_t& hitTime, const Ray& ray,
                       const Vector3& v0, const Vector3& v1, const Vector3& v2);

//! Find the intersection of two AxisAlignedBoxes
//! \param intersection - AxisAlignedBox enclosing the intersection
//! \param aab1 - an AxisAlignedBox
//! \param aab2 - an AxisAlignedBox
//! \return true iff the volumes intersect
inline bool Intersects(AxisAlignedBox& intersection,
                       const AxisAlignedBox& aab1,
                       const AxisAlignedBox& aab2);

//! Determine whether two AxisAlignedBoxs intersect
//! \param aab1 - an AxisAlignedBox
//! \param aab2 - an AxisAlignedBox
//! \return true if the volumes intersect, false otherwise.
inline bool Intersects(const AxisAlignedBox& aab1, const AxisAlignedBox& aab2);

//! Determine whether two spheres intersect
//! \param sphere1 - a sphere
//! \param sphere2 - a sphere
//! \return true if the volumes intersect, false otherwise.
inline bool Intersects(const Sphere& sphere1, const Sphere& sphere2);

//! Determine whether two planes intersect
//! \param plane1 - a plane
//! \param plane2 - a plane
//! \return true if the planes intersect, false otherwise.
inline bool Intersects(const Plane& plane1, const Plane& plane2);

//! Determine whether two triangles intersect.
//! \param v0 - the first vertex of triangle v
//! \param v1 - the second vertex of triangle v
//! \param v2 - the third vertex of triangle v
//! \param u0 - the first vertex of triangle u
//! \param u1 - the second vertex of triangle u
//! \param u2 - the third vertex of triangle u
//! \return true if they intersect, false otherwise.
inline bool Intersects(const Vector3& v0, const Vector3& v1, const Vector3& v2,
                       const Vector3& u0, const Vector3& u1, const Vector3& u2);

//! Determine whether two 2D line segments intersect.
//! \param intersection - the resulting intersection point is stored here.
//! \param s0 - the first vertex of line segment s.
//! \param s1 - the second vertex of line segment s.
//! \param t0 - the first vertex of line segment t.
//! \param t1 - the second vertex of line segment t.
//! \return true if they intersect, false otherwise.
inline bool Intersects(Vector2& intersection,
                       const Vector2& s0, const Vector2& s1,
                       const Vector2& t0, const Vector2& t1);

//! Determine whether an AxisAlignedBox and a sphere intersect
//! \param aab - an AxisAlignedBox
//! \param sphere - a Sphere
//! \return true if the volumes intersect, false otherwise.
inline bool Intersects(const AxisAlignedBox& aab, const Sphere& sphere);

//! Determine whether an AxisAlignedBox and a plane intersect
//! \param aab - an AxisAlignedBox
//! \param plane - a Plane
//! \return true if they intersect, false otherwise.
inline bool Intersects(const AxisAlignedBox& aab, const Plane& plane);

//! Determine whether a plane and sphere intersect.
//! \param plane - a Plane
//! \param sphere - a Sphere
//! \return true if they intersect, false otherwise.
inline bool Intersects(const Plane& plane, const Sphere& sphere);

//! Determine whether an AxisAlignedBox and a triangle intersect.
//! \param aab - an AxisAlignedBox
//! \param v0 - the first vertex of triangle v
//! \param v1 - the second vertex of triangle v
//! \param v2 - the third vertex of triangle v
//! \return true if they intersect, false otherwise.
bool Intersects(const AxisAlignedBox& aab,
                const Vector3& v0, const Vector3& v1, const Vector3& v2);

//! Determine whether a Sphere and a triangle intersect.
//! \param sphere - a Sphere
//! \param v0 - the first vertex of triangle v
//! \param v1 - the second vertex of triangle v
//! \param v2 - the third vertex of triangle v
//! \return true if they intersect, false otherwise.
inline bool Intersects(const Sphere& sphere,
                       const Vector3& v0, const Vector3& v1, const Vector3& v2);

//! Determine whether a Frustum and AxisAlignedBox intersect.
//! \param aab - an AxisAlignedBox
//! \param frustum - a Frustum
//! \return true if they intersect, false otherwise.
inline bool Intersects(const AxisAlignedBox& aab, const Frustum& frustum);

//! Determine whether a Frustum and Sphere intersect.
//! \param frustum - a Frustum
//! \param sphere - a Sphere
//! \return true if they intersect, false otherwise.
inline bool Intersects(const Frustum& frustum, const Sphere& sphere);

}
}

#include "Intersections.inl"

#endif // _MATHINTERSECTIONS_H_
