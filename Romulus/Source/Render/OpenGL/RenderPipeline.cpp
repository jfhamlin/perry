#include "Render/OpenGL/GLInterface.h"
#include "Render/OpenGL/GLee.h"
#include "Render/OpenGL/RenderPipeline.h"
#include "Render/OpenGL/Utilities.h"
#include "Utility/Assertions.h"

namespace romulus
{
namespace render
{
namespace opengl
{
RenderPipeline::RenderPipeline(const GLInterface& glInterface,
                               int width, int height):
    m_width(width), m_height(height), m_textureMgr(glInterface.TextureMgr),
    m_buffers(new boost::scoped_ptr<Framebuffer>[2])
{
    ASSERT(m_textureMgr);
    m_buffers[0].reset(new Framebuffer(width, height));
    m_buffers[1].reset(new Framebuffer(width, height));

    for (int i = 0; i < 2; ++i)
    {
        m_buffers[i]->AttachColorTexture(0, GL_RGBA32F_ARB, GL_RGB, GL_FLOAT);
        m_buffers[i]->AttachDepthBuffer(true);
    }

    Framebuffer::Unbind();
    ShaderProgram::Unbind();
}

RenderPipeline::~RenderPipeline()
{
}

void RenderPipeline::RenderScene(const real_t deltaTime,
                                 const Camera& viewer, IScene& scene)
{
    // Clear the two buffers.
    ClearBuffers();

    int inputBuffer = 0;
    for (StageCollection::iterator iter = m_stages.begin();
         iter != m_stages.end(); ++iter)
    {
        int targetBuffer = (inputBuffer + 1) % 2;
        Framebuffer& input = *m_buffers[inputBuffer];
        Framebuffer& target = *m_buffers[targetBuffer];

        target.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        (*iter)->Render(deltaTime, viewer, scene, input, target);

        inputBuffer = targetBuffer;

        ASSERT_OPENGL_STATE();
    }

    // Draw the final image to the true framebuffer.
    Framebuffer::Unbind();
    ShaderProgram::Unbind();
    Framebuffer& final = *m_buffers[inputBuffer];

    glPolygonMode(GL_FRONT, GL_FILL);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    PushLoadProjectionMatrix pushProjectionMatrix(
            math::GenerateOrthographicProjectionTransform(
                    0, m_width, m_height, 0));

    PushLoadModelViewMatrix pushIdentityModelViewMatrix();

    m_textureMgr->BindTexture(0, final.ColorTextureGLHandle(0));
    m_textureMgr->SetUnit(0, true);
    RenderQuad(0, 0, m_width, m_height);
    m_textureMgr->SetUnit(0, false);

    ASSERT_OPENGL_STATE();

}

void RenderPipeline::AppendStage(RenderPipelineStagePtr stage)
{
    m_stages.push_back(stage);
}

void RenderPipeline::ClearBuffers()
{
    for (int i = 0; i < 2; ++i)
    {
        m_buffers[i]->Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    ASSERT_OPENGL_STATE();
}

}
}
}
