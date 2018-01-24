#ifndef _RENDEROPENGLDIFFUSESCENERENDERER_H_
#define _RENDEROPENGLDIFFUSESCENERENDERER_H_

#include "Render/OpenGL/RenderPipelineStage.h"
#include "Render/OpenGL/ShaderProgram.h"
#include "Render/ISceneRenderer.h"

namespace romulus
{
namespace render
{
namespace opengl
{

class DiffuseSceneRenderer : public RenderPipelineStage, public ISceneRenderer
{
PROHIBIT_COPYING(DiffuseSceneRenderer);
public:

    DiffuseSceneRenderer(const GLInterface& gli);
    virtual ~DiffuseSceneRenderer();

    virtual void Render(const real_t deltaTime, const Camera& viewer,
                        IScene& scene, const Framebuffer& input,
                        Framebuffer& target);

    //! Render the input scene.
    //! \param viewer - The scene viewer.
    //! \param scene - The input scene to render.
    virtual void RenderScene(const real_t deltaTime,
                             const Camera& viewer, IScene& scene);

private:

    void RenderGCI(GeometryChunkInstance* gcip) const;

    ShaderProgram* m_shader;
};

}
}
}

#endif // _RENDEROPENGLDIFFUSESCENERENDERER_H_
