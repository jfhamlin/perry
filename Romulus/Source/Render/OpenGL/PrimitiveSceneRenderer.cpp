#include "Render/OpenGL/PrimitiveSceneRenderer.h"
#include "Render/OpenGL/GLee.h"
#include "Render/OpenGL/IGeometryCache.h"
#include "Render/OpenGL/RenderDevice.h"
#include "Render/OpenGL/ShaderProgramManager.h"
#include "Render/OpenGL/ShaderProgram.h"
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

PrimitiveSceneRenderer::PrimitiveSceneRenderer(const GLInterface& gli):
    m_glInterface(gli)
{
}

PrimitiveSceneRenderer::~PrimitiveSceneRenderer()
{
}

void PrimitiveSceneRenderer::RenderScene(const real_t deltaTime,
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

    // Unbind any bound shader program.
    ShaderProgram::Unbind();

    // Set up the lights and materials.
    glEnable(GL_LIGHTING);
    IScene::LightCollection::iterator lit = lights.begin();
    uint_t numLights = 0;
    for (; numLights < 8 && lit != lights.end(); ++numLights, ++lit)
    {
        glEnable(GL_LIGHT0 + numLights);
        Color c = (*lit)->Color() * (*lit)->Intensity();
        math::Vector<4, float> fc(c[0], c[1], c[2], c[3]);
        glLightfv(GL_LIGHT0 + numLights, GL_DIFFUSE, &fc[0]);

        math::Vector3 p((*lit)->Position());
        math::Translation t(p);
        math::Vector<4, float> pos(p[0], p[1], p[2], 1.f);
        PushMultiplyModelViewMatrix pushModelViewMatrix(t.Matrix());
        glLightfv(GL_LIGHT0 + numLights, GL_POSITION, &pos[0]);
    }

    //m_glInterface.TextureMgr->SetUnit(0, true);
    math::Vector<4, float> white(1.f, 1.f, 1.f, 1.f);
    math::Vector<4, float> black(0.f, 0.f, 0.f, 1.f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, &white[0]);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, &white[0]);
    glMaterialfv(GL_FRONT, GL_SPECULAR, &white[0]);
    glMaterialfv(GL_FRONT, GL_EMISSION, &black[1]);

    std::for_each(geometry.begin(), geometry.end(),
                  boost::bind(&PrimitiveSceneRenderer::RenderGCI, this, _1));

    // Disable the lights and materials.
    for (uint_t i = 0; i < numLights; ++i)
        glDisable(GL_LIGHT0 + i);
    glDisable(GL_LIGHTING);


    m_glInterface.GeometryCache->UnbindGeometry();
    //m_glInterface.TextureMgr->SetUnit(0, false);
    // Disable the GL_VERTEX_ARRAY client state.
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

void PrimitiveSceneRenderer::RenderGCI(GeometryChunkInstance* gcip) const
{
    ASSERT(gcip->GeometryChunk());
    ASSERT(gcip->SurfaceDescription().GetPointer());

    // Push instance's transform.
    PushMultiplyModelViewMatrix pushModelViewMatrix(gcip->Transform());

    // Render geometry chunk.
    const GeometryChunk& gc = *gcip->GeometryChunk();
    ASSERT(gc.IndexCount() % 3 == 0);

//     m_glInterface.TextureMgr->BindTexture(
//             0, gcip->SurfaceDescription()->DiffuseAlbedoTexture());

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
