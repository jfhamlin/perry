#include "Render/OpenGL/Framebuffer.h"
#include "Render/OpenGL/GLInterface.h"
#include "Render/OpenGL/GLee.h"
#include "Render/OpenGL/OpenGLTexture.h"
#include "Render/OpenGL/RenderDevice.h"
#include "Render/OpenGL/ShaderProgramManager.h"
#include "Render/OpenGL/ShaderProgram.h"
#include "Render/OpenGL/SimpleGeometryCache.h"
#include "Render/OpenGL/TextureManager.h"
#include "Render/OpenGL/Utilities.h"
#include <SDL/SDL.h>
#include <iostream>

namespace romulus
{
using namespace math;

namespace render
{
namespace opengl
{
RenderDevice::RenderDevice(int width, int height):
    m_width(width), m_height(height)
{
    m_log.RegisterSink(&std::cout, false, Log::MessageLevel_All);

    m_shaderProgramMgr.reset(new ShaderProgramManager);
    m_textureMgr.reset(new TextureManager);
    m_geometryCache.reset(new SimpleGeometryCache);
    GLInterface glInterface(this,
                            m_shaderProgramMgr.get(),
                            m_textureMgr.get(),
                            m_geometryCache.get());

//     m_orthoRenderer.reset(new opengl::OrthographicRenderer(glInterface));
//     m_sceneRenderer.reset(new opengl::PrimitiveSceneRenderer(glInterface));
    //        new opengl::RenderPipeline(glInterface, width, height));
    //RenderPipeline* pipeline =
    //        static_cast<RenderPipeline*>(m_sceneRenderer.get());

    // pipeline->AppendStage(
    //             RenderPipelineStagePtr(
    //                 new DeferredSceneRenderer(glInterface, width, height)));

    //pipeline->AppendStage(
    //        RenderPipelineStagePtr(
    //                new DiffuseSceneRenderer(glInterface)));
//     pipeline->AppendStage(
//             RenderPipelineStagePtr(
//                     new WireframeSceneRenderer(glInterface)));
    // pipeline->AppendStage(
    //         RenderPipelineStagePtr(
    //                 new DeferredSceneRenderer(glInterface, width, height)));

    // pipeline->AppendStage(
    //         RenderPipelineStagePtr(
    //                 new ToneMappingFilter(glInterface, width, height)));

    // pipeline->AppendStage(
    //         RenderPipelineStagePtr(
    //                 new BloomFilter(glInterface, width, height)));

    // pipeline->AppendStage(
    //         RenderPipelineStagePtr(
    //                 new ColorControlFilter(glInterface, width, height)));

    InitializeRenderStates();
}

RenderDevice::~RenderDevice()
{
    m_log.Flush();
    m_log.ClearSinks();
}

void RenderDevice::ClearBuffers(bool color, bool depth, bool stencil)
{
    GLint buffers = 0;
    if (color)
        buffers |= GL_COLOR_BUFFER_BIT;
    if (depth)
        buffers |= GL_DEPTH_BUFFER_BIT;
    if (stencil)
        buffers |= GL_STENCIL_BUFFER_BIT;

    glClear(buffers);
}

void RenderDevice::SwapBuffers()
{
    SDL_GL_SwapBuffers();
    glFlush();
}

void RenderDevice::SetProjectionTransform(const Matrix44& transform)
{
    glMatrixMode(GL_PROJECTION);
    LoadTransposeMatrix(&transform[0][0]);
    glMatrixMode(GL_MODELVIEW);
}

void RenderDevice::InitializeRenderStates()
{
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClearDepth(1.f);

    m_blending = false;
    m_blendSource = BlendSource_Source_Alpha;
    m_blendDestination = BlendDestination_One_Minus_Source_Alpha;

    SetBlendSource(m_blendSource);

    glEnable(GL_LINE_SMOOTH);

    Framebuffer::Unbind();
    ShaderProgram::Unbind();
}

namespace
{
    const GLenum BlendSourceTable[] =
        { GL_SRC_ALPHA };

    const GLenum BlendDestinationTable[] =
        { GL_ONE_MINUS_SRC_ALPHA };
}

void RenderDevice::SetBlendState(bool enabled)
{
    if (m_blending == enabled)
        return;

    if (enabled)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);

    m_blending = enabled;
}

void RenderDevice::SetBlendSource(BlendSourceFunction function)
{
    m_blendSource = function;
    glBlendFunc(BlendSourceTable[m_blendSource],
        BlendDestinationTable[m_blendDestination]);
}

void RenderDevice::SetBlendDestination(BlendDestinationFunction function)
{
    m_blendDestination = function;
    glBlendFunc(BlendSourceTable[m_blendSource],
        BlendDestinationTable[m_blendDestination]);
}

TexturePtr RenderDevice::LoadTexture(std::istream& stream)
{
    return m_textureMgr->LoadTexture(stream);
}

void RenderDevice::SetTextureUnitState(int unit, bool enabled)
{
    m_textureMgr->SetUnit(unit, enabled);
}

void RenderDevice::BindTexture(int unit, const TexturePtr texture)
{
    ASSERT(texture);
    m_textureMgr->BindTexture(
            unit, *static_cast<OpenGLTexture*>(texture.get()));
}

void RenderDevice::SetClippingPlane(ClipPlane index,
                                    const math::Plane& plane)
{
    GLdouble equation[4];
    equation[0] = plane.Normal()[0];
    equation[1] = plane.Normal()[1];
    equation[2] = plane.Normal()[2];
    equation[3] = plane.DistanceFromOrigin();
    glClipPlane(GL_CLIP_PLANE0 + index, equation);
}

void RenderDevice::EnableClippingPlane(ClipPlane index)
{
    glEnable(GL_CLIP_PLANE0 + index);
}

void RenderDevice::DisableClippingPlane(ClipPlane index)
{
    glDisable(GL_CLIP_PLANE0 + index);
}

}
}
}
