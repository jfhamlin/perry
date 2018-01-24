#ifndef _OPENGLTEXTUREMANAGER_H_
#define _OPENGLTEXTUREMANAGER_H_

#include "Render/Texture.h"
#include <istream>
#include <map>
#include <vector>

namespace romulus
{
namespace render
{
namespace opengl
{

class OpenGLTexture;

class TextureManager
{
public:

    TextureManager();
    ~TextureManager();

    TexturePtr LoadTexture(std::istream& stream);
    void ReleaseTexture(OpenGLTexture& texture);
    //! Update the OpenGL texture from updated texture data.
    //! \param texture - the texture to update
    //! \param updatedMipLevels - a bitfield, where a bit is on iff
    //!                           the corresponding mip level should
    //!                           be updated. LSB corresponds to level
    //!                           0 and so on.
    void UpdateTexture(OpenGLTexture& texture, uint_t updatedMipLevels);

    void SetUnit(int unit, bool enabled);
    void BindTexture(const int unit, const OpenGLTexture& texture);

private:

    void CreateTexture(OpenGLTexture* texture) const;

    std::vector<uint_t> m_textureUnits;
};

}
}
}

#endif // _OPENGLTEXTUREMANAGER_H_
