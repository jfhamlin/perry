#include "Render/OpenGL/DiffuseSceneRenderer.h"
#include "Render/OpenGL/GLee.h"
#include "Render/OpenGL/IGeometryCache.h"
#include "Render/OpenGL/RenderDevice.h"
#include "Render/OpenGL/ShaderProgramManager.h"
#include "Render/OpenGL/TextureManager.h"
#include "Render/OpenGL/Utilities.h"
#include "Utility/Assertions.h"
#include <algorithm>
#include <boost/bind.hpp>

namespace romulus
{
namespace render
{
namespace opengl
{

DiffuseSceneRenderer::DiffuseSceneRenderer(const GLInterface& gli):
    RenderPipelineStage(gli)
{
    m_shader = m_glInterface.ShaderProgramMgr->RequestShaderProgram(
            "DiffuseVertexProgram.vp", "DiffuseFragmentProgram.fp");
}

DiffuseSceneRenderer::~DiffuseSceneRenderer()
{
}

void DiffuseSceneRenderer::Render(const real_t deltaTime, const Camera& viewer,
                                  IScene& scene, const Framebuffer& input,
                                  Framebuffer& target)
{
    target.Bind();
    RenderScene(deltaTime, viewer, scene);
}

void DiffuseSceneRenderer::RenderScene(const real_t deltaTime,
                                       const Camera& viewer, IScene& scene)
{
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    IScene::GeometryCollection geometry;
    IScene::LightCollection lights;
    scene.PotentiallyVisibleGeometry(geometry, viewer.ViewFrustum());
    scene.PotentiallyRelevantLights(lights, viewer.ViewFrustum());

    glPolygonMode(GL_FRONT, GL_FILL);
    // Push projection transform.
    PushLoadProjectionMatrix loadProjectionMatrix(viewer.ProjectionTransform());
    // Push view transform.
    PushLoadModelViewMatrix loadModelViewMatrix(viewer.ViewTransform());

    // Enable the GL_VERTEX_ARRAY client state once.
    glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glColor3f(1.0f, 1.0f, 1.0f);
    m_glInterface.TextureMgr->SetUnit(0, true);

    // Set the fragment shader light.
    m_shader->SetUniformParameter("DiffuseTexture", 0u);

    // Render with the first light to set up the depth buffer.
    IScene::LightCollection::iterator lit = lights.begin();
    m_shader->SetUniformParameter("LightPosition", (*lit)->Position());
    m_shader->SetUniformParameter("LightColor", (*lit)->Color());

    m_shader->Bind();

    std::for_each(geometry.begin(), geometry.end(),
                  boost::bind(&DiffuseSceneRenderer::RenderGCI, this, _1));

    // Save and set blend state.
    bool prevBlendState = m_glInterface.Device->BlendState();
    //m_glInterface.Device->SetBlendState(true);

    //for (++lit; lit != lights.end(); ++lit)
    //{
    //    m_shader->SetUniformParameter("LightPosition", (*lit)->Position());
    //    m_shader->SetUniformParameter("LightColor", (*lit)->Color());

    //    std::for_each(geometry.begin(), geometry.end(),
    //                  boost::bind(&DiffuseSceneRenderer::RenderGCI, this, _1));
    //}

    // Restore blend state.
    m_glInterface.Device->SetBlendState(prevBlendState);

    m_glInterface.GeometryCache->UnbindGeometry();
    m_glInterface.TextureMgr->SetUnit(0, false);
    // Disable the GL_VERTEX_ARRAY client state.
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    ShaderProgram::Unbind();
}

void DiffuseSceneRenderer::RenderGCI(GeometryChunkInstance* gcip) const
{
    ASSERT(gcip->GeometryChunk());
    ASSERT(gcip->SurfaceDescription().GetPointer());

    // Push instance's transform.
    PushMultiplyModelViewMatrix pushModelViewMatrix(gcip->Transform());

    // Render geometry chunk.
    const GeometryChunk& gc = *gcip->GeometryChunk();
    ASSERT(gc.IndexCount() % 3 == 0);

    m_glInterface.TextureMgr->BindTexture(
            0, gcip->SurfaceDescription()->DiffuseAlbedoTexture());

    // Set the object-specific shader parameters.
    m_shader->SetUniformParameter("ObjectToWorldTransform",
                                  gcip->Transform());
    math::Matrix33 objectToWorld(
            math::Submatrix<3, 3, 0, 0>(gcip->Transform()));
    Invert(Transpose(objectToWorld));
    m_shader->SetUniformParameter(
            "InverseTransposeObjectToWorldTransform",
            objectToWorld);

    // Bind the geometry and render.
    m_glInterface.GeometryCache->BindGeometry(&gc);
    uint_t offset = m_glInterface.GeometryCache->BufferOffset();
    const uint_t vec3Size = sizeof(math::Vector3) * gc.VertexCount();
    glVertexPointer(3, GL_ROMULUS_REAL, 0, static_cast<ubyte_t*>(0) + offset);
    offset += vec3Size;
    glNormalPointer(GL_ROMULUS_REAL, 0, static_cast<ubyte_t*>(0) + offset);
    offset += vec3Size * 2;
    glTexCoordPointer(2, GL_ROMULUS_REAL, 0, static_cast<ubyte_t*>(0) + offset);

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(gc.IndexCount()),
                   GL_UNSIGNED_SHORT, gc.Indices());
}

} // namespace opengl
} // namespace render
} // namespace romulus
