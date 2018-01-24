#ifndef _RENDEROPENGLOPENGLTEXTURE_H_
#define _RENDEROPENGLOPENGLTEXTURE_H_

#include "Render/Texture.h"

namespace romulus
{
namespace render
{
namespace opengl
{

class TextureManager;

class OpenGLTexture : public Texture
{
public:

    OpenGLTexture(uint_t width, uint_t height, TextureFormat format,
                  TextureInternalType type, uint_t mipCount,
                  TextureMipmapSettings mipSettings,
                  TextureManager* manager);

    virtual ~OpenGLTexture();

    //! This texture's OpenGL texture handle
    //! (i.e. the value assigned by glGenTextures).
    uint_t Handle;

private:

    TextureManager* m_manager;
};

} // namespace opengl
} // namespace render
} // namespace romulus

#endif // _RENDEROPENGLOPENGLTEXTURE_H_
