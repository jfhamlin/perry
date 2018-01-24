#ifndef _RENDEROPENGLTONEMAPPINGFILTER_H_
#define _RENDEROPENGLTONEMAPPINGFILTER_H_

//! \file ToneMappingFilter.h
//! Contains the tone mapping filter class.

#include "Render/OpenGL/RenderPipelineStage.h"
#include "Render/OpenGL/TextureManager.h"
#include "Render/OpenGL/ShaderProgram.h"
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>

namespace romulus
{
namespace render
{
namespace opengl
{

//! A tone mapping render stage.
//! Applies a modified version of the Reinhard algorithm.
class ToneMappingFilter : public RenderPipelineStage
{
PROHIBIT_COPYING(ToneMappingFilter);
public:

    ToneMappingFilter(const GLInterface& gli, int width, int height);
    virtual ~ToneMappingFilter();

    virtual void Render(const real_t deltaTime, const Camera& viewer,
                        IScene& scene, const Framebuffer& input,
                        Framebuffer& target);

private:

    void ComputeAverageLuminance(const Framebuffer& input);

    int m_width, m_height;
    int m_level;
    boost::scoped_ptr<Framebuffer> m_luminanceBuffer;

    ShaderProgram* m_luminanceShader;
    ShaderProgram* m_toneMappingShader;

    real_t m_currentAverage;
    real_t m_targetAverage;
};

}
}
}

#endif // _RENDEROPENGLTONEMAPPINGFILTER_H_
