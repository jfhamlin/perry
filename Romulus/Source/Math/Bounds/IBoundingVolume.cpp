#include "Math/Bounds/IBoundingVolume.h"
#include "Math/Bounds/BoundingVolumes.h"
#include "Math/Intersections.h"
#include "Math/Transformations.h"
#include "Utility/Assertions.h"

namespace romulus
{
namespace math
{

IBoundingVolume* Construct(IBoundingVolume::VolumeType type)
{
    switch (type)
    {
        case IBoundingVolume::VolumeType_AABB:
            return new AABB();
        case IBoundingVolume::VolumeType_Sphere:
            return new BoundingSphere();
        default:
            break;
    }
    ASSERT(false);
    return NULL;
}

void Copy(IBoundingVolume& to, const IBoundingVolume& from)
{
    Union(to, from, from);
}

bool Intersects(const IBoundingVolume& a, const IBoundingVolume& b)
{
    return a.Intersects(b);
}

bool Intersects(const Frustum& frustum, const IBoundingVolume& b)
{
    return b.Intersects(frustum);
}

bool Contains(const IBoundingVolume& a, const IBoundingVolume& b)
{
    return a.Contains(b);
}

bool Contains(const Frustum& frustum, const IBoundingVolume& bv)
{
    return bv.ContainedBy(frustum);
}

void Union(IBoundingVolume& united, const IBoundingVolume& a,
           const IBoundingVolume& b)
{
    switch (united.Type())
    {
        case IBoundingVolume::VolumeType_AABB:
            a.Union(static_cast<AABB&>(united), b);
            break;
        case IBoundingVolume::VolumeType_Sphere:
            a.Union(static_cast<BoundingSphere&>(united), b);
            break;
        default:
            ASSERT(false);
            break;
    }
}

void Transform(IBoundingVolume& bv, const Matrix44& transform)
{
    bv.Transform(transform);
}

void Transform(IBoundingVolume& bv, const Transformation& transform)
{
    Transform(bv, transform.Matrix());
}

}
}
