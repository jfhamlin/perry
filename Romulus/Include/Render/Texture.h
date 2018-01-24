#ifndef _RESOURCETEXTURE_H_
#define _RESOURCETEXTURE_H_

//! \file Texture.h
//! Contains the texture resource interface.

#include "Core/Types.h"
#include "Utility/SharedPointer.h"
#include <boost/scoped_array.hpp>

namespace romulus
{
namespace render
{

//! Texture resource access.
class Texture
{
public:

    enum TextureFormat
    {
        Format_RGB = 0,
        Format_RGBA,
        Format_Alpha,
        Format_Luminance
    };

    enum TextureInternalType
    {
        Type_UByte = 0,
        Type_Float
    };

    enum TextureMipmapSettings
    {
        Mipmap_Use,
        Mipmap_Generate
    };

    Texture(uint_t width, uint_t height, TextureFormat format,
            TextureInternalType type, uint_t mipCount,
            TextureMipmapSettings mipSettings);

    virtual ~Texture() { }

    inline uint_t Width() const { return m_width; }
    inline uint_t Height() const { return m_height; }

    inline TextureFormat Format() const { return m_format; }
    inline TextureInternalType InternalType() const { return m_type; }

    inline uint_t MipCount() const { return m_mipCount; }
    void SetMipLevel(uint_t level, void* data);
    TextureMipmapSettings MipSettings() const { return m_mipSettings; }

    void* Data(uint_t mip);

    //! Resize the texture.
    //! This will invalidate any previous texture data.
    void Resize(uint_t width, uint_t height);


protected:

    uint_t m_width;
    uint_t m_height;

    TextureFormat m_format;
    TextureInternalType m_type;

    uint_t m_mipCount;

    typedef boost::scoped_array<ubyte_t> Level;

    TextureMipmapSettings m_mipSettings;
    boost::scoped_array<Level> m_levels;
};

DECLARE_SHARED_PTR(Texture);

} // namespace render
} // namespace romulus

#endif // _RESOURCETEXTURE_H_
