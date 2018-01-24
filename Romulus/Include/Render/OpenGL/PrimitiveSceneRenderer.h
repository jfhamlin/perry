#ifndef _RENDEROPENGLPRIMITIVESCENERENDERER_H_
#define _RENDEROPENGLPRIMITIVESCENERENDERER_H_

#include "Render/OpenGL/GLInterface.h"
#include "Render/ISceneRenderer.h"

namespace romulus
{
namespace render
{
namespace opengl
{

class PrimitiveSceneRenderer : public ISceneRenderer
{
PROHIBIT_COPYING(PrimitiveSceneRenderer);
public:

    PrimitiveSceneRenderer(const GLInterface& gli);
    virtual ~PrimitiveSceneRenderer();

    //! Render the input scene.
    //! \param viewer - The scene viewer.
    //! \param scene - The input scene to render.
    virtual void RenderScene(const real_t deltaTime,
                             const Camera& viewer, IScene& scene);

private:

    void RenderGCI(GeometryChunkInstance* gcip) const;

    GLInterface m_glInterface;
};

}
}
}

#endif // _RENDEROPENGLPRIMITIVESCENERENDERER_H_
