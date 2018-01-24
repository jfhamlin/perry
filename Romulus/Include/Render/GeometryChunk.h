#ifndef _RENDERGEOMETRYCHUNK_H_
#define _RENDERGEOMETRYCHUNK_H_

//! \file GeometryChunk.h
//! Contains geometry chunk declaration.

#include "Math/Bounds/IBoundingVolume.h"
#include "Math/Vector.h"
#include <boost/scoped_ptr.hpp>

namespace romulus
{
namespace render
{

class GeometryChunk
{
public:

    GeometryChunk();
    virtual ~GeometryChunk();

    virtual const math::Vector3* Vertices() const = 0;
    virtual const math::Vector3* Normals() const = 0;
    virtual const math::Vector3* Tangents() const = 0;
    virtual const math::Vector2* TextureCoordinates() const = 0;
    virtual uint_t VertexCount() const = 0;

    virtual uint_t IndexCount() const = 0;
    virtual const ushort_t* Indices() const = 0;

    bool IsModified() const { return m_modified; }
    void SetModified(bool m) { m_modified = m; }

    //! This method should be used when a good bounding volume can be
    //! constructed easily and cheaply by the constructor of this GC.
    //! Otherwise, use of ComputeBoundingVolume() is preferred.
    inline void SetBoundingVolume(const math::IBoundingVolume& bv)
    {
        m_boundingVolume.reset(math::Construct(bv.Type()));
        math::Copy(*m_boundingVolume, bv);
    }

    inline const math::IBoundingVolume& BoundingVolume() const
    {
        ASSERT(m_boundingVolume);
        return *m_boundingVolume;
    }

    void ComputeBoundingVolume();

private:

    bool m_modified;

    boost::scoped_ptr<math::IBoundingVolume> m_boundingVolume;
};

}
}

#endif // _RENDERGEOMETRYCHUNK_H_
