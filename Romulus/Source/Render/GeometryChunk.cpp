#include "Math/Bounds/BoundingVolumes.h"
#include "Render/GeometryChunk.h"

namespace romulus
{
namespace render
{

GeometryChunk::GeometryChunk(): m_modified(true) { }

GeometryChunk::~GeometryChunk() { }

void GeometryChunk::ComputeBoundingVolume()
{
    math::BoundingSphere* sphere =
            static_cast<math::BoundingSphere*>(
                    math::Construct(math::IBoundingVolume::VolumeType_Sphere));
    *sphere = math::BoundingSphere(Vertices(), Vertices() + VertexCount());

    //! \todo For other potential bounding volumes, construct them and choose
    //! the smallest.

    m_boundingVolume.reset(sphere);
}

}
}
