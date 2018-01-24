#include "Render/Texture.h"
#include "Math/Utilities.h"

namespace romulus
{
namespace render
{

namespace
{
const uint_t FormatSize[] =
{
    3,
    4,
    1,
    1
};

const uint_t TypeSizeMultiplier[] =
{
    1,
    4
};

}

Texture::Texture(uint_t width, uint_t height,
                 TextureFormat format,
                 TextureInternalType type, uint_t mipCount,
                 TextureMipmapSettings mipSettings):
    m_width(width), m_height(height), m_format(format), m_type(type),
    m_mipCount(mipCount), m_mipSettings(mipSettings)
{
    uint_t elementSize = FormatSize[format] * TypeSizeMultiplier[type];
    if (mipSettings == Mipmap_Use)
    {
        // If these are presupplied mips they should be power of two already.
        ASSERT(math::IsPowerOfTwo(width) && math::IsPowerOfTwo(height));

        // Then we have mipCount mip levels.
        m_levels.reset(new Level[mipCount]);
        for (uint_t i = 0; i < mipCount; ++i)
        {
            ubyte_t* level = new ubyte_t[(width >> i) * (height >> i)
                                         * elementSize];
            m_levels[i].reset(level);
        }
    }
    else
    {
        // We just have a base level and will let users sort out the mips.
        m_levels.reset(new Level[1]);
        m_levels[0].reset(new ubyte_t[width * height * elementSize]);
    }
}

void* Texture::Data(uint_t level)
{
    ASSERT(level <= MipCount());
    return m_levels[level].get();
}

void Texture::SetMipLevel(uint_t level, void* data)
{
    ASSERT(level <= MipCount());
    ASSERT(data);

    uint_t elementSize = FormatSize[Format()]
            * TypeSizeMultiplier[InternalType()];
    if (m_mipSettings == Mipmap_Use)
    {
        // Calculate the level width, height.
        uint_t width = m_width >> level;
        uint_t height = m_height >> level;
        memcpy(m_levels[level].get(), data, elementSize * width * height);
    }
    else
    {
        memcpy(m_levels[0].get(), data, elementSize * m_width * m_height);
    }
}

void Texture::Resize(uint_t width, uint_t height)
{
    uint_t elementSize = FormatSize[Format()] *
            TypeSizeMultiplier[InternalType()];
    if (MipSettings() == Mipmap_Use)
    {
        // If these are presupplied mips they should be power of two already.
        ASSERT(math::IsPowerOfTwo(width) && math::IsPowerOfTwo(height));

        uint_t max = math::Max(m_width, m_height);
        m_mipCount = math::Min(
                static_cast<int>(floorf(logf(max) / logf(2.f))), 1000);

        // Then we have mipCount mip levels.
        m_levels.reset(new Level[m_mipCount]);
        for (uint_t i = 0; i < m_mipCount; ++i)
        {
            ubyte_t* level = new ubyte_t[(width >> i) * (height >> i)
                                         * elementSize];
            m_levels[i].reset(level);
        }
    }
    else
    {
        // We just have a base level and will let users sort out the mips.
        m_levels.reset(new Level[1]);
        m_levels[0].reset(new ubyte_t[m_width * m_height * elementSize]);
    }
}

} // namespace render
} // namespace romulus
