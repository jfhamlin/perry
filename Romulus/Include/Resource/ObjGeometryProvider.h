#ifndef _RESOURCEOBJGEOMETRYPROVIDER_H_
#define _RESOURCEOBJGEOMETRYPROVIDER_H_

//! \file ObjGeometryProvider.h
//! Contains the OBJ resource provider declaration.

#include "Resource/IResourceProvider.h"
#include "Render/GeometryChunk.h"
#include <boost/shared_ptr.hpp>
#include <vector>

namespace romulus
{

//! OBJ geometry resource.
class ObjGeometry : public render::GeometryChunk
{
public:

    ObjGeometry() { }
    virtual ~ObjGeometry() { }

    virtual const math::Vector3* Vertices() const
    {
        return &V[0];
    }
    virtual const math::Vector3* Normals() const
    {
        return &N[0];
    }
    virtual const math::Vector3* Tangents() const
    {
        return &T[0];
    }

    virtual const math::Vector2* TextureCoordinates() const
    {
        return &UV[0];
    }
    virtual uint_t VertexCount() const
    {
        return V.size();
    }

    virtual uint_t IndexCount() const
    {
        return I.size();
    }
    virtual const ushort_t* Indices() const
    {
        return  &I[0];
    }

    typedef std::vector<math::Vector3> Vector3List;
    typedef std::vector<math::Vector2> Vector2List;
    typedef std::vector<ushort_t> UshortList;

    Vector3List V, N, T;
    Vector2List UV;
    UshortList I;
};

//! The OBJ geometry provider.
class ObjGeometryProvider : public IStreamResourceProvider
{
public:

    typedef boost::shared_ptr<ObjGeometry> GeometryPtr;

    ObjGeometryProvider();
    virtual ~ObjGeometryProvider();

    virtual bool LoadResource(std::istream& stream,
                              ResourceHandleBase::id_t& id);
    virtual void UnloadResource(ResourceHandleBase::id_t id);

    virtual void* GetResource(ResourceHandleBase::id_t id)
    {
        ASSERT(id > 0);
        render::GeometryChunk* gc = m_geometry[id - 1].get();

        ASSERT(gc);
        return gc;
    }

    virtual int HandleType() const
    {
        return render::GeometryHandle::HandleType();
    }

    virtual const ExtensionCollection& HandledExtensions() const
    {
        return m_extensions;
    }

private:

    uint_t FindFreeSlot();
    GeometryPtr ParseStream(std::istream& stream);

    typedef std::vector<GeometryPtr> GeometryCollection;

    GeometryCollection m_geometry;
    ExtensionCollection m_extensions;
};

}

#endif // _RESOURCEOBJGEOMETRYPROVIDER_H_
