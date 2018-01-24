#ifndef _RESOURCEFONTPROVIDER_H_
#define _RESOURCEFONTPROVIDER_H_

#include "Resource/IResourceProvider.h"
#include "Resource/ResourceHandle.h"
#include "Resource/Texture.h"
#include "Math/Rectangle.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_array.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace romulus
{

class ResourceManager;
class FontProvider;
class FontResource
{
public:

    struct Glyph
    {
        ushort_t GlyphCode;

        int MinX, MaxX;
        int MinY, MaxY;

        real_t MinU, MaxU;
        real_t MinV, MaxV;

        uint_t Advance;

        void Scale(real_t factor)
        {
            MinX = static_cast<int>(MinX * factor);
            MaxX = static_cast<int>(MaxX * factor);
            MinY = static_cast<int>(MinY * factor);
            MaxY = static_cast<int>(MaxY * factor);

            Advance = static_cast<uint_t>(Advance * factor);
        }
    };

    struct FontInfo
    {
        uint_t PointSize;

        uint_t Ascent;
        uint_t Descent;
        uint_t Height;
        uint_t LineSkip;

        void Scale(real_t factor)
        {
            PointSize = static_cast<uint_t>(PointSize * factor);
            Ascent = static_cast<uint_t>(Ascent * factor);
            Descent = static_cast<uint_t>(Descent * factor);
            Height = static_cast<uint_t>(Height * factor);
            LineSkip = static_cast<uint_t>(LineSkip * factor);
        }
    };

    static real_t ComputeScalingFactor(uint_t pointSize, uint_t newSize)
    {
        return static_cast<real_t>(newSize) / static_cast<real_t>(pointSize);
    }

    FontResource() { }
    virtual ~FontResource() { }

    inline ushort_t StartGlyph() const { return m_startGlyph; }
    inline ushort_t GlyphCount() const { return m_glyphCount; }
    inline const Glyph& LookupGlyph(ubyte_t glyph)
    {
        return m_glyphs[glyph - StartGlyph()];
    }

    inline const FontInfo& Info() const { return m_info; }

    inline const MutableTextureHandle& Texture() const { return m_texture; }

protected:

    friend class FontProvider;

    typedef std::vector<Glyph> GlyphCollection;

    ushort_t m_startGlyph;
    ushort_t m_glyphCount;
    MutableTextureHandle m_texture;
    GlyphCollection m_glyphs;
    FontInfo m_info;
};
DECLARE_RESOURCE_HANDLE_TYPE(Font, FontResource);

//! Font resource provider and renderer.
class FontProvider : public IStreamResourceProvider
{
public:

    FontProvider(ResourceManager* resourceMgr);
    virtual ~FontProvider();

    // IResourceProvider implementation.
    virtual bool LoadResource(std::istream& stream,
                              ResourceHandleBase::id_t& id);
    virtual void UnloadResource(ResourceHandleBase::id_t id);
    virtual int HandleType() const
    {
        return FontHandle::HandleType();
    }
    virtual void* GetResource(ResourceHandleBase::id_t id);


    virtual const ExtensionCollection& HandledExtensions() const
    {
        return m_resourceExtensions;
    }

private:

    typedef boost::shared_ptr<FontResource> FontPtr;
    typedef std::vector<FontPtr> FontList;

    size_t FindFreeSlot();
    void DetermineFontTextureDimensions(FT_Face face, FontPtr font,
                                        ushort_t firstGlyph, ushort_t lastGlyph,
                                        uint_t& width, uint_t& height);
    void GenerateFontTexture(FT_Face face, FontPtr font, uint_t width,
                             uint_t height, ubyte_t* texture);

    ExtensionCollection m_resourceExtensions;
    FT_Library m_fontLibrary;

    FontList m_fonts;
    ResourceManager* m_resourceMgr;
};

}

#endif // _RESOURCEFONTPROVIDER_H_

