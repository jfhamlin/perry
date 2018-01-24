#ifndef _RESOURCEMUTABLEGEOMETRYPROVIDER_H_
#define _RESOURCEMUTABLEGEOMETRYPROVIDER_H_

#include "Resource/MutableGeometryChunk.h"
#include "Resource/IResourceProvider.h"
#include <boost/shared_ptr.hpp>
#include <vector>

namespace romulus
{

class MutableGeometryResourceArguments : public ProceduralResourceArguments
{
DECLARE_RTTI(ProceduralResourceArguments);
public:

    MutableGeometryResourceArguments(): GC(0) { }

    const render::GeometryChunk* GC;
};

class MutableGeometryProvider : public IProceduralResourceProvider
{
public:

    virtual ~MutableGeometryProvider() { }

    virtual bool GenerateResource(
            const ProceduralResourceArguments& arguments,
            ResourceHandleBase::id_t& id);

    //! Unload the specified resource.
    //! \param id - Identifier of the resource to unload.
    virtual void UnloadResource(ResourceHandleBase::id_t id);

    virtual int HandleType() const
    {
        return MutableGeometryHandle::HandleType();
    }

    virtual const RTTI& HandledArgumentType() const
    {
        return MutableGeometryResourceArguments::TypeInformation;
    }

    virtual void* GetResource(ResourceHandleBase::id_t id);

private:
    typedef boost::shared_ptr<MutableGeometryChunk> MutableGeometryPtr;
    typedef std::vector<MutableGeometryPtr> MeshVector;

    uint_t FindFreeSlot();

    MeshVector m_meshes;
};

}

#endif //_RESOURCEMUTABLEGEOMETRYPROVIDER_H_
