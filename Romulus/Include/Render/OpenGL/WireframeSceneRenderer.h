#ifndef _RENDEROPENGLWIREFRAMESCENERENDERER_H_
#define _RENDEROPENGLWIREFRAMESCENERENDERER_H_

#include "Render/OpenGL/RenderPipelineStage.h"

namespace romulus
{
namespace render
{
namespace opengl
{

class WireframeSceneRenderer : public RenderPipelineStage
{
public:

    WireframeSceneRenderer(const GLInterface& gli);
    virtual ~WireframeSceneRenderer() { }

    //! Execute this render stage, writing to target.
    //! Assume that the target Framebuffer is cleared before invocation of
    //! Render().
    virtual void Render(const real_t deltaTime, const Camera& viewer,
                        IScene& scene, const Framebuffer& input,
                        Framebuffer& target);

private:

    void RenderGCI(GeometryChunkInstance* gcip) const;
};

} // namespace opengl
} // namespace render
} // namespace romulus

#endif // _RENDEROPENGLWIREFRAMESCENERENDERER_H_
