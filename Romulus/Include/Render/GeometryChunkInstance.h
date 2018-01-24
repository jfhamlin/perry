#ifndef _GEOMETRYCHUNKINSTANCE_H_
#define _GEOMETRYCHUNKINSTANCE_H_

//! \file GeometryChunkInstance.h
//! Contains declaration of GeometryChunkInstnace.

#include "Math/Matrix.h"
#include "Math/Bounds/IBoundingVolume.h"
#include "Render/GeometryChunk.h"
#include "Render/Material.h"
#include "Scene/IBounded.h"
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace romulus
{
namespace render
{
//! An instance of geometry.
//! Contains all information necessary for a render operation.
class GeometryChunkInstance : public scene::IBounded
{
public:

    GeometryChunkInstance() { }
    virtual ~GeometryChunkInstance() { }

    inline const render::GeometryChunk* GeometryChunk() const
    {
        return m_gc.get();
    }

    inline void SetGeometryChunk(
            boost::shared_ptr<const render::GeometryChunk> gc)
    {
        ASSERT(gc);
        m_gc = gc;
        m_boundingVolume.reset(
                math::Construct(m_gc->BoundingVolume().Type()));
        math::Copy(*m_boundingVolume, m_gc->BoundingVolume());
        math::Transform(*m_boundingVolume, m_transform);
    }

    inline boost::shared_ptr<const Material> SurfaceDescription() const
    {
        return m_surfaceDescription;
    }

    inline void SetSurfaceDescription(boost::shared_ptr<const Material> mp)
    {
        m_surfaceDescription = mp;
    }

    inline const math::Matrix44& Transform() const
    {
        return m_transform;
    }

    inline void SetTransform(const math::Matrix44& transform)
    {
        m_transform = transform;
        math::Copy(*m_boundingVolume, m_gc->BoundingVolume());
        math::Transform(*m_boundingVolume, m_transform);
    }

    virtual const math::IBoundingVolume& BoundingVolume() const
    {
        return *m_boundingVolume;
    }

private:

    boost::shared_ptr<const render::GeometryChunk> m_gc;

    boost::shared_ptr<const Material> m_surfaceDescription;

    math::Matrix44 m_transform;

    boost::scoped_ptr<math::IBoundingVolume> m_boundingVolume;
};

}
}

#endif // _GEOMETRYCHUNKINSTANCE_H_
