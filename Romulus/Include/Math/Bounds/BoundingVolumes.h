#ifndef _MATHBOUNDINGVOLUMES_H_
#define _MATHBOUNDINGVOLUMES_H_

//! \file BoundingVolumes.h
//! Contains class declarations for supported bounding volumes.

#include "Math/Bounds/IBoundingVolume.h"
#include "Math/AxisAlignedBox.h"
#include "Math/Sphere.h"

namespace romulus
{
namespace math
{

//! AxisAlignedBoundingBox bounding volume class.
class AABB: public IBoundingVolume, public AxisAlignedBox
{
public:

    AABB();
    AABB(const AxisAlignedBox& aab);
    AABB(const Vector3 minCorner, const Vector3 maxCorner);
    AABB(real_t minx, real_t miny, real_t minz,
         real_t maxx, real_t maxy, real_t maxz);
    template <typename Iterator>
    AABB(Iterator startPoints, Iterator endPoints);

    virtual VolumeType Type() const;
    virtual Vector3 Center() const;
    virtual real_t Volume() const;

private:

    friend class BoundingSphere;

    virtual bool Intersects(const IBoundingVolume& bv) const
    { return bv.Intersects(*this); }
    virtual bool Intersects(const AABB& aabb) const;
    virtual bool Intersects(const BoundingSphere& bs) const;
    virtual bool Intersects(const Frustum& f) const;

    virtual bool Contains(const IBoundingVolume& bv) const
    { return bv.ContainedBy(*this); }
    virtual bool Contains(const AABB& aabb) const;
    virtual bool Contains(const BoundingSphere& bs) const;
    virtual bool ContainedBy(const AABB& aabb) const;
    virtual bool ContainedBy(const BoundingSphere& bs) const;
    virtual bool ContainedBy(const Frustum& frustum) const;

    virtual void Union(AABB& united, const IBoundingVolume& bv) const
    { bv.Union(united, *this); }
    virtual void Union(BoundingSphere& united, const IBoundingVolume& bv) const
    { bv.Union(united, *this); }
    virtual void Union(AABB& united, const AABB& b) const;
    virtual void Union(BoundingSphere& united, const AABB& b) const;
    virtual void Union(AABB& united, const BoundingSphere& b) const;
    virtual void Union(BoundingSphere& united, const BoundingSphere& b) const;

    virtual void Transform(const Matrix44& transform);
};

//! BoundingSphere bounding volume class.
class BoundingSphere: public IBoundingVolume, public Sphere
{
public:

    BoundingSphere();
    BoundingSphere(const Sphere& sphere);
    BoundingSphere(const Vector3& center, const real_t radius);
    template <typename Iterator>
    BoundingSphere(Iterator startPoints, Iterator endPoints):
        Sphere(startPoints, endPoints)
    { }

    virtual VolumeType Type() const;
    virtual Vector3 Center() const;
    virtual real_t Volume() const;

private:

    friend class AABB;

    virtual bool Intersects(const IBoundingVolume& bv) const
    { return bv.Intersects(*this); }
    virtual bool Intersects(const AABB& aabb) const;
    virtual bool Intersects(const BoundingSphere& bs) const;
    virtual bool Intersects(const Frustum& f) const;

    virtual bool Contains(const IBoundingVolume& bv) const
    { return bv.ContainedBy(*this); }
    virtual bool Contains(const AABB& aabb) const;
    virtual bool Contains(const BoundingSphere& bs) const;
    virtual bool ContainedBy(const AABB& aabb) const;
    virtual bool ContainedBy(const BoundingSphere& bs) const;
    virtual bool ContainedBy(const Frustum& frustum) const;

    virtual void Union(AABB& united, const IBoundingVolume& bv) const
    { bv.Union(united, *this); }
    virtual void Union(BoundingSphere& united, const IBoundingVolume& bv) const
    { bv.Union(united, *this); }
    virtual void Union(AABB& united, const AABB& b) const;
    virtual void Union(BoundingSphere& united, const AABB& b) const;
    virtual void Union(AABB& united, const BoundingSphere& b) const;
    virtual void Union(BoundingSphere& united, const BoundingSphere& b) const;

    virtual void Transform(const Matrix44& transform);
};

}
}

#endif // _MATHBOUNDINGVOLUMES_H_
