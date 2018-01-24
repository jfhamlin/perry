#include "Math/Bounds/BoundingVolumes.h"
#include "Math/Intersections.h"

namespace romulus
{
namespace math
{
/* AABB implementation */

AABB::AABB(): AxisAlignedBox() { }

AABB::AABB(const AxisAlignedBox& aab): AxisAlignedBox(aab) { }

AABB::AABB(const Vector3 minCorner, const Vector3 maxCorner):
    AxisAlignedBox(minCorner, maxCorner) { }

AABB::AABB(real_t minx, real_t miny, real_t minz,
           real_t maxx, real_t maxy, real_t maxz):
    AxisAlignedBox(minx, miny, minz, maxx, maxy, maxz) { }

template <typename Iterator>
AABB::AABB(Iterator startPoints, Iterator endPoints):
    AxisAlignedBox(startPoints, endPoints) { }

IBoundingVolume::VolumeType AABB::Type() const
{
    return VolumeType_AABB;
}

Vector3 AABB::Center() const
{
    return AxisAlignedBox::Center();
}

real_t AABB::Volume() const
{
    return AxisAlignedBox::Volume();
}

bool AABB::Intersects(const AABB& aabb) const
{
    return romulus::math::Intersects(*static_cast<const AxisAlignedBox*>(this),
                                     aabb);
}

bool AABB::Intersects(const BoundingSphere& bs) const
{
    return romulus::math::Intersects(*static_cast<const AxisAlignedBox*>(this),
                                     bs);
}

bool AABB::Intersects(const Frustum& f) const
{
    return romulus::math::Intersects(*static_cast<const AxisAlignedBox*>(this),
                                     f);
}

bool AABB::Contains(const AABB& aabb) const
{
    return AxisAlignedBox::Contains(aabb.MinCorner()) &&
            AxisAlignedBox::Contains(aabb.MaxCorner());
}

bool AABB::Contains(const BoundingSphere& bs) const
{
    if (!AxisAlignedBox::Contains(bs.Center()))
        return false;
    for (uint_t i = 0; i < 3; ++i)
    {
        real_t dist = Min(bs.Center()[i] - MinCorner()[i],
                          MaxCorner()[i] - bs.Center()[i]);
        if (dist < bs.Radius())
            return false;
    }
    return true;
}

bool AABB::ContainedBy(const AABB& aabb) const
{
    return aabb.Contains(*this);
}

bool AABB::ContainedBy(const BoundingSphere& bs) const
{
    return bs.Contains(*this);
}

bool AABB::ContainedBy(const Frustum& frustum) const
{
    for (int p = 0; p < 6; ++p)
    {
        Frustum::FrustumPlaneSide planeSide =
                static_cast<Frustum::FrustumPlaneSide>(p);
        for (int v = 0; v < 8; ++v)
            if (frustum.FrustumPlane(planeSide).WhichSide(Corner(v)) ==
                Plane::PlaneSide_BackOfPlane)
                return false;
    }
    return true;
}

void AABB::Union(AABB& united, const AABB& b) const
{
    const AABB& a = *this;
    united.SetMaxX(Max(a.MaxX(), b.MaxX()));
    united.SetMaxY(Max(a.MaxY(), b.MaxY()));
    united.SetMaxZ(Max(a.MaxZ(), b.MaxZ()));
    united.SetMinX(Min(a.MinX(), b.MinX()));
    united.SetMinY(Min(a.MinY(), b.MinY()));
    united.SetMinZ(Min(a.MinZ(), b.MinZ()));
}

void AABB::Union(BoundingSphere& united, const AABB& b) const
{
    BoundingSphere aSphere, bSphere;
    Copy(aSphere, *this);
    Copy(bSphere, b);
    romulus::math::Union(united, aSphere, bSphere);
}

void AABB::Union(AABB& united, const BoundingSphere& b) const
{
    const AABB& a = *this;
    united.SetMaxX(Max(a.MaxX(), b.Center()[0] + b.Radius()));
    united.SetMaxY(Max(a.MaxY(), b.Center()[1] + b.Radius()));
    united.SetMaxZ(Max(a.MaxZ(), b.Center()[2] + b.Radius()));
    united.SetMinX(Min(a.MinX(), b.Center()[0] - b.Radius()));
    united.SetMinY(Min(a.MinY(), b.Center()[1] - b.Radius()));
    united.SetMinZ(Min(a.MinZ(), b.Center()[2] - b.Radius()));
}

void AABB::Union(BoundingSphere& united, const BoundingSphere& b) const
{
    BoundingSphere aSphere;
    Copy(aSphere, *this);
    romulus::math::Union(united, aSphere, b);
}

namespace
{

struct TransformedCornerIterator
{
    TransformedCornerIterator() { }
    TransformedCornerIterator(const Matrix44& m, const AABB* a, uint_t corner):
        Transform(&m), Box(a), Corner(corner) { }

    TransformedCornerIterator& operator++()
    {
        ++Corner;
        return *this;
    }

    Vector3 operator*()
    {
        Vector3 corner = Box->Corner(Corner);
        Vector4 x = (*Transform) * Vector4(corner[0], corner[1],
                                           corner[2], 1.0);
        ASSERT(ApproxEqual(x[3], 1.0));
        return Vector3(&x[0]);
    }

    TransformedCornerIterator& operator=(const TransformedCornerIterator& c)
    {
        Transform = c.Transform;
        Box = c.Box;
        Corner = c.Corner;
        return *this;
    }

    bool operator==(const TransformedCornerIterator& c)
    {
        return Transform == c.Transform && Box == c.Box && Corner == c.Corner;
    }

    bool operator!=(const TransformedCornerIterator& c)
    {
        return !(*this == c);
    }

    const Matrix44* Transform;
    const AABB* Box;
    uint_t Corner;
};

} // namespace

void AABB::Transform(const Matrix44& transform)
{
    *this = AABB(TransformedCornerIterator(transform, this, 0),
                 TransformedCornerIterator(transform, this, 8));
}

/* BoundingSphere implementation */

BoundingSphere::BoundingSphere(): Sphere() { }

BoundingSphere::BoundingSphere(const Sphere& sphere):
    Sphere(sphere) { }

BoundingSphere::BoundingSphere(const Vector3& center,
                               const real_t radius):
    Sphere(center, radius)
{ }

IBoundingVolume::VolumeType BoundingSphere::Type() const
{
    return VolumeType_Sphere;
}

Vector3 BoundingSphere::Center() const
{
    return Sphere::Center();
}

real_t BoundingSphere::Volume() const
{
    return Sphere::Volume();
}

bool BoundingSphere::Intersects(const AABB& aabb) const
{
    return romulus::math::Intersects(aabb, *static_cast<const Sphere*>(this));
}

bool BoundingSphere::Intersects(const BoundingSphere& bs) const
{
    return romulus::math::Intersects(*static_cast<const Sphere*>(this), bs);
}

bool BoundingSphere::Intersects(const Frustum& f) const
{
    return romulus::math::Intersects(f, *static_cast<const Sphere*>(this));
}

bool BoundingSphere::Contains(const AABB& aabb) const
{
    return Sphere::Contains(aabb.Corner(0)) &&
            Sphere::Contains(aabb.Corner(1)) &&
            Sphere::Contains(aabb.Corner(2)) &&
            Sphere::Contains(aabb.Corner(3)) &&
            Sphere::Contains(aabb.Corner(4)) &&
            Sphere::Contains(aabb.Corner(5)) &&
            Sphere::Contains(aabb.Corner(6)) &&
            Sphere::Contains(aabb.Corner(7));
}

bool BoundingSphere::Contains(const BoundingSphere& bs) const
{
    real_t radiusDifference = Radius() - bs.Radius();
    return MagnitudeSquared(bs.Center() - Center()) <=
            radiusDifference * radiusDifference;
}

bool BoundingSphere::ContainedBy(const AABB& aabb) const
{
    return aabb.Contains(*this);
}

bool BoundingSphere::ContainedBy(const BoundingSphere& bs) const
{
    return bs.Contains(*this);
}

bool BoundingSphere::ContainedBy(const Frustum& frustum) const
{
    for (int p = 0; p < 6; ++p)
    {
        Frustum::FrustumPlaneSide planeSide =
                static_cast<Frustum::FrustumPlaneSide>(p);
        if (frustum.FrustumPlane(planeSide).Distance(Center()) <
            Radius())
            return false;
    }
    return true;
}

void BoundingSphere::Union(AABB& united, const AABB& b) const
{
    romulus::math::Union(united, b, *this);
}

void BoundingSphere::Union(BoundingSphere& united, const AABB& b) const
{
    romulus::math::Union(united, b, *this);
}

void BoundingSphere::Union(AABB& united, const BoundingSphere& b) const
{
    const BoundingSphere& a = *this;
    united.SetMaxX(Max(a.Center()[0] + a.Radius(),
                       b.Center()[0] + b.Radius()));
    united.SetMaxY(Max(a.Center()[1] + a.Radius(),
                       b.Center()[1] + b.Radius()));
    united.SetMaxZ(Max(a.Center()[2] + a.Radius(),
                       b.Center()[2] + b.Radius()));
    united.SetMinX(Min(a.Center()[0] - a.Radius(),
                       b.Center()[0] - b.Radius()));
    united.SetMinY(Min(a.Center()[1] - a.Radius(),
                       b.Center()[1] - b.Radius()));
    united.SetMinZ(Min(a.Center()[2] - a.Radius(),
                       b.Center()[2] - b.Radius()));
}

void BoundingSphere::Union(BoundingSphere& united,
                           const BoundingSphere& b) const
{
    const BoundingSphere& a = *this;

    if (Contains(b))
    {
        united = *this;
    }
    else if (b.Contains(*this))
    {
        united = b;
    }
    else
    {
        Vector3 atob = b.Center() - a.Center();
        real_t dist = Magnitude(atob);
        ASSERT(dist != 0.f);
        atob *= (1.f / dist);
        Vector3 p0 = a.Center() - a.Radius() * atob;
        Vector3 p1 = b.Center() + b.Radius() * atob;
        united.SetCenter(Lerp(p0, p1, 0.5));
        united.SetRadius(.5f * (dist + a.Radius() + b.Radius()));
    }
}

void BoundingSphere::Transform(const Matrix44& transform)
{
    const Vector3& center = this->Sphere::Center();
    Vector4 homogenousCenter(center[0], center[1], center[2], 1.f);

    Vector4 pointOnSphere(center[0], center[1], center[2], 1.f);
    pointOnSphere[2] += Radius();

    SetCenter(Vector3(&(transform * homogenousCenter)[0]));
    SetRadius(Magnitude(Vector3(&(transform * pointOnSphere)[0]) - Center()));
}

}
}
