#include "Render/OpenGL/ColorControlFilter.h"
#include "Render/OpenGL/ShaderProgramManager.h"
#include "Render/OpenGL/Utilities.h"
#include "Utility/Assertions.h"

namespace romulus
{
namespace render
{
namespace opengl
{

ColorControlFilter::ColorControlFilter(const GLInterface& gli,
                                       int width, int height):
    RenderPipelineStage(gli), m_width(width), m_height(height)
{
    m_shader = m_glInterface.ShaderProgramMgr->RequestShaderProgram(
            "SimpleVertexProgram.vp", "ColorControlFilter.fp");
}

ColorControlFilter::~ColorControlFilter()
{
}

void ColorControlFilter::Render(const real_t deltaTime, const Camera& viewer,
                                IScene& scene, const Framebuffer& input,
                                Framebuffer& target)
{
    target.Bind();

    m_shader->Bind();
    m_shader->SetUniformParameter("SceneTexture", 0u);
    m_shader->SetUniformParameter("ColorFilter", math::Vector3(1));
    m_shader->SetUniformParameter("Brightness", 1.f);
    m_shader->SetUniformParameter("Saturation", 1.f);

    m_glInterface.TextureMgr->BindTexture(0, input.ColorTextureGLHandle(0));

    PushLoadProjectionMatrix pushProjectionMatrix(
            math::GenerateOrthographicProjectionTransform(
                    0, m_width, m_height, 0));

    PushLoadModelViewMatrix pushIdentityModelViewMatrix();

    RenderQuad(0, 0, m_width, m_height);
}

}
}
}
