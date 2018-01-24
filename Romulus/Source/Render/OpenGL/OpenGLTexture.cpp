#include "Render/OpenGL/OpenGLTexture.h"
#include "Render/OpenGL/TextureManager.h"
#include "Utility/Assertions.h"

namespace romulus
{
namespace render
{
namespace opengl
{

OpenGLTexture::OpenGLTexture(uint_t width, uint_t height,
                             TextureFormat format,
                             TextureInternalType type, uint_t mipCount,
                             TextureMipmapSettings mipSettings,
                             TextureManager* manager):
    Texture(width, height, format, type, mipCount, mipSettings),
    m_manager(manager)
{
    ASSERT(manager);
}

OpenGLTexture::~OpenGLTexture()
{
    m_manager->ReleaseTexture(*this);
}

} // namespace opengl
} // namespace render
} // namespace romulus
