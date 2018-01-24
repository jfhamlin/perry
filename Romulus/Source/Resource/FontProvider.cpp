#include "Resource/FontProvider.h"
#include "Resource/ResourceManager.h"
#include <boost/scoped_array.hpp>

namespace romulus
{
namespace
{
// Conversion from FreeType fixed point.
int FT_Ceiling(int x) { return (x & -64) / 64; }
int FT_Floor(int x) { return ((x + 63) & -64) / 64; }
}

FontProvider::FontProvider(ResourceManager* resourceMgr):
    m_resourceMgr(resourceMgr)
{
    ASSERT(m_resourceMgr);
    m_resourceExtensions.push_back("ttf");

    FT_Init_FreeType(&m_fontLibrary);
}

FontProvider::~FontProvider()
{
    FT_Done_FreeType(m_fontLibrary);
}

bool FontProvider::LoadResource(std::istream& stream,
                                ResourceHandleBase::id_t& id)
{
    // Load our truetype font.
    stream.seekg(0, std::ios::end);
    size_t fileLength = stream.tellg();
    stream.seekg(0, std::ios::beg);

    boost::scoped_array<char> fileContents(new char[fileLength]);
    stream.read(fileContents.get(), static_cast<std::streamsize>(fileLength));

    FT_Face face;
    int error = FT_New_Memory_Face(
            m_fontLibrary, reinterpret_cast<FT_Byte*>(fileContents.get()),
            static_cast<FT_Long>(fileLength), 0, &face);
    if (error)
        return false;

    // Set the font size.
    const int PointSize = 15;
    error = FT_Set_Char_Size(face, 0, PointSize * 64, 0, 0);
    if (error)
    {
        FT_Done_Face(face);
        return false;
    }

    // The range of our text.
    //! \todo Find a better way of doing this.
    const ushort_t FirstGlyph = 0;
    const ushort_t LastGlyph = 255;

    // Create our font object and retrieve properties thereof.
    FT_Fixed scale = face->size->metrics.y_scale;
    FontPtr font(new FontResource);
    font->m_glyphs.resize(LastGlyph - FirstGlyph);
    font->m_startGlyph = FirstGlyph;
    font->m_glyphCount = LastGlyph - FirstGlyph;

    font->m_info.PointSize = PointSize;
    font->m_info.Ascent = FT_Ceiling(FT_MulFix(face->ascender, scale));
    font->m_info.Descent = FT_Ceiling(FT_MulFix(face->descender, scale));
    font->m_info.Height = font->m_info.Ascent - font->m_info.Descent;
    font->m_info.LineSkip = FT_Ceiling(FT_MulFix(face->height, scale));

    // Generate our font texture.
    uint_t width, height;
    DetermineFontTextureDimensions(face, font, FirstGlyph, LastGlyph, width,
                                   height);

    if (width == 0)
    {
        FT_Done_Face(face);
        return false;
    }

    boost::scoped_array<ubyte_t> buffer(new ubyte_t[width * height]);
    GenerateFontTexture(face, font, width,
                        height, buffer.get());


    MutableTextureResourceArguments textureArgs;
    textureArgs.Width = width;
    textureArgs.Height = height;
    textureArgs.Format = TextureResource::Format_Alpha;
    textureArgs.InternalType = TextureResource::Type_UByte;
    textureArgs.MipSettings = TextureResource::Mipmap_Generate;

    m_resourceMgr->LoadResource(textureArgs, font->m_texture);
    font->m_texture.GetResource()->SetMipLevel(0, buffer.get());

    FT_Done_Face(face);

    // font->Texture = texture;
    int slot = static_cast<int>(FindFreeSlot());
    m_fonts[slot] = font;
    id = slot + 1;

    return true;
}

void FontProvider::UnloadResource(ResourceHandleBase::id_t id)
{
    uint_t index = id - 1;
    ASSERT(index >= 0);

    ASSERT(m_fonts[index].get());

    m_fonts[index].reset();
}

size_t FontProvider::FindFreeSlot()
{
    for (size_t i = 0; i < m_fonts.size(); ++i)
    {
        if (!m_fonts[i].get())
            return i;
    }

    m_fonts.push_back(FontPtr());
    return m_fonts.size() - 1;
}

void* FontProvider::GetResource(ResourceHandleBase::id_t id)
{
    uint_t index = id - 1;
    ASSERT(index >= 0);

    FontResource* font = m_fonts[index].get();
    ASSERT(font);

    return reinterpret_cast<void*>(font);
}

void FontProvider::DetermineFontTextureDimensions(
        FT_Face face, FontPtr font, ushort_t firstGlyph, ushort_t lastGlyph,
        uint_t& width, uint_t& height)
{
    const int TextureWidth = 512;
    int rows = 1;
    int offsetX = 0;

    FontResource::GlyphCollection& glyphs = font->m_glyphs;

    for (ushort_t i = firstGlyph; i < lastGlyph; ++i)
    {
        int glyphIndex = FT_Get_Char_Index(face, i);
        if (!glyphIndex)
            continue;

        int error;
        error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
        if (error)
        {
            width = 0;
            return;
        }

        int index = i - firstGlyph;
        FontResource::Glyph& glyph = glyphs[index];

        glyph.GlyphCode = i;
        glyph.Advance = FT_Ceiling(face->glyph->metrics.horiAdvance);

        glyph.MinX = FT_Floor(face->glyph->metrics.horiBearingX);
        glyph.MaxX = glyph.MinX + FT_Ceiling(face->glyph->metrics.width);
        glyph.MaxY = FT_Floor(face->glyph->metrics.horiBearingY);
        glyph.MinY = glyph.MaxY - FT_Ceiling(face->glyph->metrics.height);

        offsetX += glyph.Advance;
        if (offsetX >= TextureWidth)
        {
            ++rows;
            offsetX = 0;
        }
    }

    width = TextureWidth;
    height = (int)powf(2.0f,
                       ceilf(logf(static_cast<float>(rows)
                                  * (font->Info().LineSkip)) / logf(2.0f)));
}

void FontProvider::GenerateFontTexture(
        FT_Face face, FontPtr font, uint_t width, uint_t height,
        ubyte_t* texture)
{
    // ubyte_t* texture = new ubyte_t[width * height];
    memset(texture, 0, width * height);

    int offsetX = 0;
    int row = 0;

    FontResource::GlyphCollection& glyphs = font->m_glyphs;
    const FontResource::FontInfo& info = font->Info();

    for (size_t i = 0; i < font->GlyphCount(); ++i)
    {
        FontResource::Glyph& glyph = glyphs[i];
        FT_Load_Glyph(face, FT_Get_Char_Index(face, glyph.GlyphCode),
                      FT_LOAD_DEFAULT);
        FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

        if ((offsetX + glyph.Advance) > width)
        {
            ++row;
            offsetX = 0;
        }
        int offsetY = (row * (info.LineSkip));

        int topOffset = info.Ascent - glyph.MaxY;
        FT_Bitmap bitmap = face->glyph->bitmap;

        // Copy rendered glyph into texture.
        for (int y = 0; y < bitmap.rows; ++y)
        {
            for (int x = 0; x < bitmap.width; ++x)
            {
                unsigned char value = bitmap.buffer[(y * bitmap.width) + x];

                int yOffset = (offsetY + ((y + topOffset))) * width;

                texture[yOffset
                        + (offsetX + x + glyph.MinX)]
                        = value;
            }
        }

        // Determine glyph texture coordinates.
        float minX = static_cast<real_t>(offsetX) + glyph.MinX;
        float maxX = static_cast<real_t>(offsetX) + glyph.MaxX;

        glyph.MinU = minX / static_cast<real_t>(width);
        glyph.MaxU = maxX / static_cast<real_t>(width);

        float minY = (static_cast<real_t>(row) * info.LineSkip) + topOffset;
        float maxY = minY + (glyph.MaxY - glyph.MinY);

        glyph.MinV = minY / static_cast<real_t>(height);
        glyph.MaxV = maxY / static_cast<real_t>(height);

        offsetX += glyph.Advance;
    }
}
}
