#include "Math/Utilities.h"
#include "Render/OpenGL/OpenGLTexture.h"
#include "Render/OpenGL/TextureManager.h"
#include "Render/OpenGL/Utilities.h"
#include <boost/scoped_array.hpp>
#include "Render/OpenGL/GLee.h"
#include <GL/gl.h>
#include <IL/il.h>

namespace romulus
{
namespace render
{
namespace opengl
{
TextureManager::TextureManager()
{
    GLint textureUnitCount;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &textureUnitCount);
    ASSERT_OPENGL_STATE();
    m_textureUnits.resize(textureUnitCount, 0);

    ilInit();
}

TextureManager::~TextureManager()
{
    ilShutDown();
}

TexturePtr TextureManager::LoadTexture(std::istream& stream)
{
    ILuint image;
    ilGenImages(1, &image);
    ilBindImage(image);

    stream.seekg(0, std::ios::end);
    size_t fileLength = stream.tellg();
    stream.seekg(0, std::ios::beg);

    boost::scoped_array<char> fileContents(new char[fileLength]);
    stream.read(fileContents.get(),
                static_cast<std::streamsize>(fileLength));

    ilLoadL(IL_TYPE_UNKNOWN, fileContents.get(),
            static_cast<ILuint>(fileLength));
    if (ilGetError() != IL_NO_ERROR)
    {
        ilDeleteImages(1, &image);
        return TexturePtr();
    }

    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    ILint width = ilGetInteger(IL_IMAGE_WIDTH);
    ILint height = ilGetInteger(IL_IMAGE_HEIGHT);

    uint_t dataLength = width * height * 4;

    ubyte_t* data = new ubyte_t[dataLength];
    memcpy(data, ilGetData(), dataLength);

    TexturePtr texture(
            new OpenGLTexture(
                    width, height, Texture::Format_RGBA,
                    Texture::Type_UByte, 1, Texture::Mipmap_Generate,
                    this));
    texture->SetMipLevel(0, data);

    ilDeleteImages(1, &image);

    CreateTexture(static_cast<OpenGLTexture*>(texture.get()));

    return texture;
}

void TextureManager::ReleaseTexture(OpenGLTexture& texture)
{
    glDeleteTextures(1, &texture.Handle);
}

void TextureManager::BindTexture(const int unit,
                                 const OpenGLTexture& texture)
{
    ASSERT(static_cast<uint_t>(unit) < m_textureUnits.size());
    //! TODO: Evaluate actual cost of rebinding texture vis-a-vis branch.
    if (m_textureUnits[unit] == texture.Handle)
        return;

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture.Handle);
    m_textureUnits[unit] = texture.Handle;
}

void TextureManager::SetUnit(int unit, bool enabled)
{
    ASSERT(static_cast<uint_t>(unit) < m_textureUnits.size());
    glActiveTexture(GL_TEXTURE0 + unit);
    if (enabled)
        glEnable(GL_TEXTURE_2D);
    else
        glDisable(GL_TEXTURE_2D);
}

namespace
{
const GLenum TextureInternalFormat[] =
{
    GL_RGB,
    GL_RGBA,
    GL_ALPHA8,
    GL_LUMINANCE
};

const GLenum TextureLayout[] =
{
    GL_RGB,
    GL_RGBA,
    GL_ALPHA,
    GL_LUMINANCE
};

const GLenum TextureType[] =
{
    GL_UNSIGNED_BYTE,
    GL_FLOAT
};

uint_t DetermineMipWidth(uint_t width, uint_t height, uint_t level)
{
    uint_t max = math::Max(width, height);
    max = math::NextPowerOfTwo(max);

    return max >> level;
}

}

void TextureManager::UpdateTexture(OpenGLTexture& texture,
                                   uint_t updatedMipLevels)
{
    BindTexture(0, texture);

    GLenum layout = TextureLayout[texture.Format()];
    GLenum type = TextureType[texture.InternalType()];

    if (texture.MipSettings() == Texture::Mipmap_Use)
    {
        uint_t baseWidth = DetermineMipWidth(texture.Width(),
                                             texture.Height(), 0);
        for (uint_t i = 0; i < texture.MipCount(); ++i)
        {
            if ((updatedMipLevels >> i) & 1)
            {
                uint_t width = baseWidth >> i;
                // Update this mip level.
                glTexSubImage2D(GL_TEXTURE_2D, i, 0, 0, width, width,
                                layout, type, texture.Data(i));
            }
        }
    }
    else
    {
        // Assume we are updating the base level.
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture.Width(),
                        texture.Height(), layout, type,
                        texture.Data(0));
    }

}

void TextureManager::CreateTexture(OpenGLTexture* texture) const
{
    uint_t handle;
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);

    GLenum format = TextureInternalFormat[texture->Format()];
    GLenum layout = TextureLayout[texture->Format()];
    GLenum type = TextureType[texture->InternalType()];

    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format,
        texture->Width(), texture->Height(), 0, layout,
        type, texture->Data(0));
    texture->Handle = handle;
}

}
}
}
