#ifndef _RENDERIORTHOGRAPHICRENDERER_H_
#define _RENDERIORTHOGRAPHICRENDERER_H_

//! \file IOrthographicRenderer.h
//! Contains IOrthographicRenderer renderer interface.

#include "Math/Rectangle.h"
#include "Math/Curve.h"
#include "Render/Color.h"
#include "Render/Texture.h"

namespace romulus
{
namespace render
{

//! Renderer for 2D primitives.
class IOrthographicRenderer
{
public:

    IOrthographicRenderer() { }
    virtual ~IOrthographicRenderer() { }

    //! Render a rectangle with diffuse color.
    //! \param rect - The rectangle extents.
    //! \param diffuse - The color.
    virtual void RenderRectangle(const math::Rectangle& rect,
        const Color& diffuse) = 0;
    //! Render a rectangle with a texture.
    //! \param rect - The rectangle extents.
    //! \param texture - The texture to apply.
    virtual void RenderRectangle(const math::Rectangle& rect,
                                 TexturePtr texture) = 0;

    //! Render a textured rectangle.
    //! \param rect - The rectangle extents.
    //! \param u - The min and max U texture coordinates.
    //! \param v - The min and max V texture coordinates.
    //! \param diffuse - The diffuse color to set.
    //! \param texture - The texture to apply.
    virtual void RenderRectangle(const math::Rectangle& rect,
                                 const math::Vector2& u,
                                 const math::Vector2& v,
                                 const Color& diffuse,
                                 TexturePtr texture) = 0;

    //! Render a texture rectangle.
    //! \param rect - The rectangle extents.
    //! \param uv - The texture coordinate rect.
    //! \param diffuse - The diffuse color to set.
    //! \param texture - The texture to apply.
    virtual void RenderRectangle(const math::Rectangle& rect,
                                 const math::Rectangle& uv,
                                 const Color& diffuse,
                                 TexturePtr texture) = 0;

    //! Render a texture rectangle.
    //! \param rect - The rectangle extents.
    //! \param z - The rectangle depth.
    //! \param uv - The texture coordinate rect.
    //! \param diffuse - The diffuse color to set.
    //! \param texture - The texture to apply.
    virtual void RenderRectangle(const math::Rectangle& rect,
                                 const real_t z,
                                 const math::Rectangle& uv,
                                 const Color& diffuse,
                                 TexturePtr texture) = 0;

    virtual void RenderCurve(const math::Vector2& offset,
        const math::Curve& curve, const Color& diffuse, float thickness,
        int steps) = 0;
    virtual void RenderCurve(const math::Vector2& offset,
        const math::Curve& curve, const Color& diffuse, float thickness,
        float start, float end, int steps) = 0;
};

}
}

#endif  //_RENDERIORTHOGRAPHICRENDERER_H_

