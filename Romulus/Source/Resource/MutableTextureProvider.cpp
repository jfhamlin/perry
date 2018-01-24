#include "Resource/MutableTextureProvider.h"
#include "Math/Utilities.h"

namespace romulus
{

class LocalResource : public SimpleTexture
{
public:

    inline LocalResource(uint_t width, uint_t height, TextureFormat format,
                         TextureInternalType type, uint_t mipCount,
                         TextureMipmapSettings mipSettings,
                         MutableTextureProvider* provider):
        SimpleTexture(width, height, format, type, mipCount, mipSettings),
        m_provider(provider)
    {
        ASSERT(provider);
    }

    inline virtual ~LocalResource()
    {
    }

    virtual void SetMipLevel(uint_t level, void* data)
    {
        SimpleTexture::SetMipLevel(level, data);
        m_provider->NotifyUpdate(this, level);
    }

private:

    MutableTextureProvider* m_provider;
};


MutableTextureProvider::MutableTextureProvider(MessageRouter* router):
    m_router(router)
{
    ASSERT(router);
}

MutableTextureProvider::~MutableTextureProvider()
{
}

bool MutableTextureProvider::GenerateResource(
            const ProceduralResourceArguments& arguments,
            ResourceHandleBase::id_t& id)
{
    const MutableTextureResourceArguments& textureArgs =
            static_cast<const MutableTextureResourceArguments&>(arguments);

    real_t max = math::Max(textureArgs.Width, textureArgs.Height);
    uint_t mipCount = math::Min(static_cast<int>(
                                        floorf(logf(max) / logf(2.f))), 1000);

    TexturePtr texture(new LocalResource(textureArgs.Width, textureArgs.Height,
                                         textureArgs.Format,
                                         textureArgs.InternalType, mipCount,
                                         textureArgs.MipSettings, this));
    if (textureArgs.SourceTexture.Valid())
            texture->SetMipLevel(
                    0, textureArgs.SourceTexture.GetResource()->Data(0));

    uint_t index = FindFreeSlot();
    m_textures[index] = texture;
    id = index + 1;

    return true;
}

void MutableTextureProvider::UnloadResource(ResourceHandleBase::id_t id)
{
    uint_t index = id - 1;
    ASSERT(index >= 0);

    TextureResource* texture = m_textures[index].get();
    ASSERT(texture);

    m_textures[index].reset();
}

void* MutableTextureProvider::GetResource(ResourceHandleBase::id_t id)
{
    uint_t index = id - 1;
    ASSERT(index >= 0);

    MutableTextureResource* texture = m_textures[index].get();
    ASSERT(texture);

    return reinterpret_cast<void*>(texture);
}

void MutableTextureProvider::NotifyUpdate(SimpleTexture* texture, uint_t level)
{
    MutableTextureMessage* msg = new MutableTextureMessage;
    msg->Texture = texture;
    msg->UpdatedMipLevels = 1 << level;

    m_router->DispatchMessage(MutableResourceMessageGroup, MessagePtr(msg));
}

uint_t MutableTextureProvider::FindFreeSlot()
{
    for (uint_t i = 0; i < m_textures.size(); ++i)
    {
        if (!m_textures[i].get())
            return i;
    }

    m_textures.push_back(TexturePtr());
    return m_textures.size() - 1;
}

}
