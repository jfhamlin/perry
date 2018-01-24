#ifndef _RENDEROPENGLORTHOGRAPHICRENDERER_H_
#define _RENDEROPENGLORTHOGRAPHICRENDERER_H_

#include "Render/IOrthographicRenderer.h"

namespace romulus
{
namespace render
{
namespace opengl
{
struct GLInterface;
class TextureManager;

class OrthographicRenderer : public IOrthographicRenderer
{
public:

    OrthographicRenderer(const GLInterface& gli);
    virtual ~OrthographicRenderer();

    virtual void RenderRectangle(const math::Rectangle& rect,
        const Color& diffuse);
    virtual void RenderRectangle(const math::Rectangle& rect,
        const TextureResourceHandleBase& texture);
    virtual void RenderRectangle(const math::Rectangle& rect,
                                 const math::Vector2& u,
                                 const math::Vector2& v,
                                 const Color& diffuse,
                                 const TextureResourceHandleBase& texture);
    virtual void RenderRectangle(const math::Rectangle& rect,
                                 const math::Rectangle& uv,
                                 const Color& diffuse,
                                 const TextureResourceHandleBase& texture);
    virtual void RenderRectangle(const math::Rectangle& rect,
                                 const real_t z,
                                 const math::Rectangle& uv,
                                 const Color& diffuse,
                                 const TextureResourceHandleBase& texture);


    virtual void RenderCurve(const math::Vector2& offset, const math::Curve& curve,
        const Color& diffuse, float thickness, int steps);
    virtual void RenderCurve(const math::Vector2& offset, const math::Curve& curve,
        const Color& diffuse, float thickness, float start, float end, int steps);

private:

    TextureManager* m_textureMgr;
};
}
}
}

#endif // _RENDEROPENGLORTHOGRAPHICRENDERER_H_
