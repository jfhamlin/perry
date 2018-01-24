#include "Render/OpenGL/ShaderProgramManager.h"
#include "Render/OpenGL/ToneMappingFilter.h"
#include "Render/OpenGL/Utilities.h"
#include "Utility/Assertions.h"
#include "Math/Utilities.h"
#include <cmath>
#include <iostream>

namespace romulus
{
namespace render
{
namespace opengl
{

ToneMappingFilter::ToneMappingFilter(const GLInterface& gli,
                                     int width, int height):
    RenderPipelineStage(gli), m_width(width), m_height(height),
    m_currentAverage(0.0)
{
    m_luminanceBuffer.reset(new Framebuffer(m_width, m_height));
    m_luminanceBuffer->AttachColorTexture(0, GL_LUMINANCE32F_ARB,
                                                  GL_LUMINANCE, GL_FLOAT);
    m_luminanceBuffer->AttachDepthBuffer(true);

    m_luminanceShader = m_glInterface.ShaderProgramMgr->RequestShaderProgram(
            "SimpleVertexProgram.vp", "LuminanceFragmentProgram.fp");
    m_toneMappingShader = m_glInterface.ShaderProgramMgr->RequestShaderProgram(
            "SimpleVertexProgram.vp", "ToneMappingFragmentProgram.fp");

    ASSERT_OPENGL_STATE();

    // Calculate the maximum (smallest) mipmap level of our luminance texture.
    real_t max = math::Max(width, height);
    m_level = math::Min(static_cast<int>(floorf(logf(max) / logf(2.f))), 1000);
}

ToneMappingFilter::~ToneMappingFilter()
{
}

void ToneMappingFilter::Render(const real_t deltaTime, const Camera& viewer,
                               IScene& scene, const Framebuffer& input,
                               Framebuffer& target)
{
    ComputeAverageLuminance(input);

    // Calculate the viewer's 'average luminance'.
    const real_t LuminanceHighVelocity = 0.05f * deltaTime;
    const real_t LuminanceLowVelocity = -0.05f * deltaTime;
    real_t difference = m_targetAverage - m_currentAverage;
    if (difference < 0.0)
    {
        m_currentAverage += math::Clamp(LuminanceLowVelocity, difference,
                                        static_cast<real_t>(0.0));
    }
    else
    {
        m_currentAverage += math::Clamp(LuminanceHighVelocity,
                                        static_cast<real_t>(0.0),
                                        difference);
    }

    ShaderProgram::Unbind();
    target.Bind();

    PushLoadProjectionMatrix pushProjectionMatrix(
            math::GenerateOrthographicProjectionTransform(
                    0, m_width, m_height, 0));
    PushLoadModelViewMatrix pushIdentityModelViewMatrix();

    // Apply the tone mapping filter.
    m_toneMappingShader->Bind();
    m_toneMappingShader->SetUniformParameter("SceneTexture", 0u);
    m_toneMappingShader->SetUniformParameter("Key", 0.36f);
    m_toneMappingShader->SetUniformParameter("Level", m_level);
    m_toneMappingShader->SetUniformParameter("WhiteLuminanceSquared", 4.f);
    m_toneMappingShader->SetUniformParameter("AverageLuminance", m_currentAverage);

    m_glInterface.TextureMgr->BindTexture(0, input.ColorTextureGLHandle(0));

    RenderQuad(0, 0, m_width, m_height);

    ASSERT_OPENGL_STATE();
}

void ToneMappingFilter::ComputeAverageLuminance(const Framebuffer& input)
{
    PushLoadModelViewMatrix pushIdentityModelViewMatrix();

    m_luminanceShader->Bind();
    m_luminanceShader->SetUniformParameter("SourceTexture", 0u);
    m_luminanceBuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    PushLoadProjectionMatrix pushProjectionMatrix(
            math::GenerateOrthographicProjectionTransform(
                    0, m_width, m_height, 0));

    // First render the initial luminance values.
    m_glInterface.TextureMgr->BindTexture(0, input.ColorTextureGLHandle(0));
    RenderQuad(0, 0, m_width, m_height);

    // Next let OpenGL calculate the average luminance by generating
    // a mipmap chain for us.
    m_glInterface.TextureMgr->BindTexture(
            0, m_luminanceBuffer->ColorTextureGLHandle(0));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR);
    glGenerateMipmapEXT(GL_TEXTURE_2D);

    // Read in the target luminance (the average luminance).
    glGetTexImage(GL_TEXTURE_2D, m_level, GL_LUMINANCE, GL_FLOAT,
                  &m_targetAverage);
}

}
}
}

