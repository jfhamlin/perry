#include "Render/OpenGL/GLee.h"
#include "Render/OpenGL/IGeometryCache.h"
#include "Render/OpenGL/Utilities.h"
#include "Render/OpenGL/WireframeSceneRenderer.h"
#include "Utility/Assertions.h"
#include <algorithm>
#include <boost/bind.hpp>

namespace romulus
{
namespace render
{
namespace opengl
{

WireframeSceneRenderer::WireframeSceneRenderer(const GLInterface& gli):
    RenderPipelineStage(gli)
{
}

void WireframeSceneRenderer::Render(const real_t deltaTime,
                                    const Camera& viewer,
                                    IScene& scene, const Framebuffer& input,
                                    Framebuffer& target)
{
    IScene::GeometryCollection geometry;
    scene.PotentiallyVisibleGeometry(geometry, viewer.ViewFrustum());
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Render wires atop input buffer.
    {
        input.Bind();

        // Load projection transform.
        PushLoadProjectionMatrix loadProjectionMatrix(
                viewer.ProjectionTransform());
        // Load view transform.
        PushLoadModelViewMatrix loadModelViewMatrix(viewer.ViewTransform());
        // Enable the GL_VERTEX_ARRAY client state once.
        glEnableClientState(GL_VERTEX_ARRAY);

        glDepthFunc(GL_LEQUAL);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-0.4, 0);

        glColor3f(1.0, 1.0, 1.0);
        std::for_each(geometry.begin(), geometry.end(),
                      boost::bind(&WireframeSceneRenderer::RenderGCI,
                                  this, _1));
        m_glInterface.GeometryCache->UnbindGeometry();

        glDisable(GL_POLYGON_OFFSET_LINE);

        // Disable the GL_VERTEX_ARRAY client state.
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    // Render out to target buffer.
    {
        glPolygonMode(GL_FRONT, GL_FILL);

        PushLoadProjectionMatrix pushProjectionMatrix(
                math::GenerateOrthographicProjectionTransform(
                        0, target.Width(), target.Height(), 0));

        PushLoadModelViewMatrix pushIdentityModelViewMatrix();
        target.Bind();

        m_glInterface.TextureMgr->SetUnit(0, true);
        m_glInterface.TextureMgr->BindTexture(0, input.ColorTextureGLHandle(0));
        RenderQuad(0, 0, target.Width(), target.Height());
        m_glInterface.TextureMgr->SetUnit(0, false);

        ASSERT_OPENGL_STATE();
    }
}

void WireframeSceneRenderer::RenderGCI(GeometryChunkInstance* gcip) const
{
    ASSERT(gcip->GeometryChunk());

    // Push instance's transform.
    PushMultiplyModelViewMatrix pushModelViewMatrix(gcip->Transform());
    // Render geometry chunk.
    const GeometryChunk& gc = *gcip->GeometryChunk();
    ASSERT(gc.IndexCount() % 3 == 0);

    m_glInterface.GeometryCache->BindGeometry(&gc);
    glVertexPointer(3, GL_ROMULUS_REAL, 0,
                    static_cast<ubyte_t*>(0) +
                    m_glInterface.GeometryCache->BufferOffset());

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(gc.IndexCount()),
                   GL_UNSIGNED_SHORT, gc.Indices());
}

} // namespace opengl
} // namespace render
} // namespace romulus
