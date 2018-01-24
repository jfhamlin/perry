#ifndef _RENDEROPENGLRENDERPIPELINESTAGE_H_
#define _RENDEROPENGLRENDERPIPELINESTAGE_H_

#include "Render/OpenGL/GLInterface.h"
#include "Render/OpenGL/Framebuffer.h"
#include "Render/Camera.h"
#include "Render/IScene.h"
#include "Utility/Common.h"
#include <boost/shared_ptr.hpp>

namespace romulus
{
namespace render
{
namespace opengl
{
typedef boost::shared_ptr<class RenderPipelineStage> RenderPipelineStagePtr;

//! A stage in the render pipeline.
//! It takes as parameter the framebuffer output of the previous stage and
//! writes to another framebuffer its own output.
class RenderPipelineStage
{
public:

    RenderPipelineStage(const GLInterface& gli): m_glInterface(gli)
    {
    }

    virtual ~RenderPipelineStage() { }

    //! Execute this render stage, writing to target.
    //! Assume that the target Framebuffer is cleared before invocation of
    //! Render().
    virtual void Render(const real_t deltaTime, const Camera& viewer,
                        IScene& scene, const Framebuffer& input,
                        Framebuffer& target) = 0;

protected:

    GLInterface m_glInterface;
};

}
}
}

#endif // _RENDEROPENGLRENDERPIPELINESTAGE_H_
