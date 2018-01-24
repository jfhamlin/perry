#ifndef _RENDEROPENGLRENDERPIPELINE_H_
#define _RENDEROPENGLRENDERPIPELINE_H_

#include "Render/ISceneRenderer.h"
#include "Render/OpenGL/RenderPipelineStage.h"
#include "Render/OpenGL/ShaderProgram.h"
#include "Render/OpenGL/TextureManager.h"
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <vector>

namespace romulus
{
namespace render
{
namespace opengl
{

struct GLInterface;

//! A scene renderer composed of many, discrete, render stages.
class RenderPipeline : public ISceneRenderer
{
public:

    RenderPipeline(const GLInterface& glInterface, int width, int height);
    ~RenderPipeline();

    virtual void RenderScene(const real_t deltaTime, const Camera& viewer,
                             IScene& camera);

    //! Add a stage to the end of the render pipeline.
    //! \param stage - The stage to append.
    void AppendStage(RenderPipelineStagePtr stage);

private:

    void ClearBuffers();

    typedef std::vector<RenderPipelineStagePtr> StageCollection;

    int m_width;
    int m_height;
    TextureManager* m_textureMgr;
    StageCollection m_stages;
    boost::scoped_array<boost::scoped_ptr<Framebuffer> > m_buffers;
};

}
}
}

#endif // _RENDEROPENGLRENDERPIPELINE_H_
