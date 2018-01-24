#ifndef _RESOURCEMUTABLETEXTUREPROVIDER_H_
#define _RESOURCEMUTABLETEXTUREPROVIDER_H_

#include "Core/MessageRouter.h"
#include "Resource/IResourceProvider.h"
#include "Resource/Texture.h"
#include "Resource/SimpleTexture.h"
#include <boost/shared_ptr.hpp>
#include <vector>

namespace romulus
{

class MutableTextureProvider : public IProceduralResourceProvider
{
public:

    MutableTextureProvider(MessageRouter* router);
    virtual ~MutableTextureProvider();

    virtual bool GenerateResource(
            const ProceduralResourceArguments& arguments,
            ResourceHandleBase::id_t& id);

    //! Unload the specified resource.
    //! \param id - Identifier of the resource to unload.
    virtual void UnloadResource(ResourceHandleBase::id_t id);

    virtual int HandleType() const
    {
        return MutableTextureHandle::HandleType();
    }

    virtual const RTTI& HandledArgumentType() const
    {
        return MutableTextureResourceArguments::TypeInformation;
    }

    virtual void* GetResource(ResourceHandleBase::id_t id);

    //! Notifies the mutable texture message group of an updated
    //! simple texture.
    void NotifyUpdate(SimpleTexture* texture, uint_t level);

private:

    uint_t FindFreeSlot();

    typedef boost::shared_ptr<SimpleTexture> TexturePtr;

    typedef std::vector<TexturePtr> TextureCollection;

    MessageRouter* m_router;
    TextureCollection m_textures;
};

}

#endif // _RESOURCEMUTABLETEXTUREPROVIDER_H_
