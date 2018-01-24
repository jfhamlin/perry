#ifndef _MATHIBOUNDINGVOLUME_H_
#define _MATHIBOUNDINGVOLUME_H_

//! \file IBoundingVolume.h
//! \brief Contains bounding volume interface.

#include "Math/Constants.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

namespace romulus
{
namespace math
{

class AABB;
class BoundingSphere;
class Frustum;
class Transformation;

class IBoundingVolume
{
public:

    enum VolumeType
    {
        VolumeType_Sphere,
        VolumeType_AABB,
        VolumeType_OBB
    };

    virtual ~IBoundingVolume() { }

    virtual VolumeType Type() const = 0;

    virtual Vector3 Center() const = 0;

    virtual real_t Volume() const = 0;

private:

    friend class AABB;
    friend class BoundingSphere;

    //! Intersection friend function.
    friend bool Intersects(const IBoundingVolume&, const IBoundingVolume&);
    friend bool Intersects(const Frustum&, const IBoundingVolume&);
    //! Intersection private interface.
    virtual bool Intersects(const IBoundingVolume& bv) const = 0;
    virtual bool Intersects(const AABB& aabb) const = 0;
    virtual bool Intersects(const BoundingSphere& bs) const = 0;
    virtual bool Intersects(const Frustum& f) const = 0;

    //! Contains friend functions.
    friend bool Contains(const IBoundingVolume&, const IBoundingVolume&);
    friend bool Contains(const Frustum&, const IBoundingVolume&);
    //! Contains private interface.
    virtual bool Contains(const IBoundingVolume& bv) const = 0;
    virtual bool Contains(const AABB& aabb) const = 0;
    virtual bool Contains(const BoundingSphere& bs) const = 0;
    virtual bool ContainedBy(const AABB& aabb) const = 0;
    virtual bool ContainedBy(const BoundingSphere& bs) const = 0;
    virtual bool ContainedBy(const Frustum& frustum) const = 0;

    //! Union friend function.
    friend void Union(IBoundingVolume&, const IBoundingVolume&,
                      const IBoundingVolume&);
    //! Union private interface.
    virtual void Union(AABB& united, const IBoundingVolume& bv) const = 0;
    virtual void Union(BoundingSphere& united,
                       const IBoundingVolume& bv) const = 0;
    virtual void Union(AABB& united, const AABB& b) const = 0;
    virtual void Union(BoundingSphere& united, const AABB& b) const = 0;
    virtual void Union(AABB& united, const BoundingSphere& b) const = 0;
    virtual void Union(BoundingSphere& united,
                       const BoundingSphere& b) const = 0;

    //! Tranformation friend function.
    friend void Transform(IBoundingVolume& bv, const Matrix44& transform);

    //! Transformation private interface.
    //! Apply the tranform to this bounding volume, producing a conservative
    //! bounding volume of the same type.
    virtual void Transform(const Matrix44& transform) = 0;
};

IBoundingVolume* Construct(IBoundingVolume::VolumeType type);

void Copy(IBoundingVolume& to, const IBoundingVolume& from);

bool Intersects(const IBoundingVolume& a, const IBoundingVolume& b);

bool Intersects(const Frustum& frustum, const IBoundingVolume& bv);

bool Contains(const IBoundingVolume& a, const IBoundingVolume& b);

bool Contains(const Frustum& frustum, const IBoundingVolume* bv);

void Union(IBoundingVolume& united, const IBoundingVolume& a,
           const IBoundingVolume& b);

void Transform(IBoundingVolume& bv, const Matrix44& transform);

void Transform(IBoundingVolume& bv, const Transformation& transform);

}
}

#endif // _MATHIBOUNDINGVOLUME_H_
