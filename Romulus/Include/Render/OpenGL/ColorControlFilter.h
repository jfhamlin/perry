#ifndef _RENDEROPENGLCOLORCONTROLFILTER_H_
#define _RENDEROPENGLCOLORCONTROLFILTER_H_

//! \file ColorControlFilter.h
//! Contains the color control render stage.

#include "Render/OpenGL/RenderPipelineStage.h"
#include "Render/OpenGL/ShaderProgram.h"
#include "Render/OpenGL/TextureManager.h"
#include <boost/scoped_ptr.hpp>

namespace romulus
{
namespace render
{
namespace opengl
{

//! Performs color manipulation to the input framebuffer.
//! This should be the final stage of the render pipeline.
class ColorControlFilter : public RenderPipelineStage
{
PROHIBIT_COPYING(ColorControlFilter);
public:

    ColorControlFilter(const GLInterface& gli, int width, int height);
    virtual ~ColorControlFilter();

    virtual void Render(const real_t deltaTime, const Camera& viewer,
                        IScene& scene, const Framebuffer& input,
                        Framebuffer& target);

private:

    ShaderProgram* m_shader;
    int m_width, m_height;
};

}
}
}

#endif // _RENDEROPENGLCOLORCONTROLFILTER_H_

