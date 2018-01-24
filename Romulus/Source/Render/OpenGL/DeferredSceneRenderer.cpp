#include "Math/Utilities.h"
#include "Render/OpenGL/DeferredSceneRenderer.h"
#include "Render/OpenGL/GLee.h"
#include "Render/OpenGL/ShaderProgram.h"
#include "Render/OpenGL/ShaderProgramManager.h"
#include "Render/OpenGL/IGeometryCache.h"
#include "Render/OpenGL/Utilities.h"
#include "Render/PointLight.h"
#include <GL/glu.h>
#include <boost/bind.hpp>
#include <iostream>

namespace romulus
{
namespace render
{
namespace opengl
{

DeferredSceneRenderer::DeferredSceneRenderer(const GLInterface& gli,
                                             uint_t width, uint_t height):
    RenderPipelineStage(gli), m_width(width), m_height(height),
    m_gBuffer(width, height), m_lightBuffer(width, height),
    m_shadowMapSize(1024),
    m_shadowMapBuffer(m_shadowMapSize * 2u, m_shadowMapSize)
{
    m_gBuffer.AttachDepthBuffer(false);
    m_gBuffer.AttachColorTexture(0, GL_RGBA16F_ARB, GL_RGBA, GL_FLOAT);
    m_gBuffer.AttachColorTexture(1, GL_RGBA16F_ARB, GL_RGBA, GL_FLOAT);
    m_gBuffer.AttachColorTexture(2, GL_RGBA16F_ARB, GL_RGBA, GL_FLOAT);

    m_lightBuffer.AttachDepthBuffer(m_gBuffer.DepthBufferGLHandle());
    m_lightBuffer.AttachColorTexture(0, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT);

    // Instantiate shaders.
    m_gBufferShader = m_glInterface.ShaderProgramMgr->RequestShaderProgram(
            "GBufferVertexProgram.vp", "GBufferFragmentProgram.fp");

    m_tangentAttributeLocation =
            m_gBufferShader->LookupAttributeLocation("Tangent");

    m_pointLightShader = m_glInterface.ShaderProgramMgr->RequestShaderProgram(
            "SimpleVertexProgram.vp", "PointLightFragmentProgram.fp");

    m_paraboloidShadowMapShader =
            m_glInterface.ShaderProgramMgr->RequestShaderProgram(
                    "ParaboloidProjectionVertexProgram.vp",
                    "SimpleFragmentProgram.fp");

    // Create a shadow map depth texture with space for two shadow maps of size
    // m_shadowMapSize by m_shadowMapSize. A color texture is attached to
    // complete the framebuffer.
    Generate2DTextureBuffer(&m_shadowMapTextureFrontAndBack,
                            m_shadowMapSize * 2u, m_shadowMapSize,
                            GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
                    GL_COMPARE_R_TO_TEXTURE);

    m_shadowMapBuffer.AttachDepthBuffer(m_shadowMapTextureFrontAndBack);
    m_shadowMapBuffer.AttachColorTexture(0, GL_RGBA16F_ARB, GL_RGBA,
                                         GL_FLOAT);
}

DeferredSceneRenderer::~DeferredSceneRenderer()
{
    glDeleteTextures(1, &m_shadowMapTextureFrontAndBack);
}

void DeferredSceneRenderer::Render(const real_t deltaTime, const Camera& viewer,
                                   IScene& scene, const Framebuffer& input,
                                   Framebuffer& target)
{
    IScene::GeometryCollection geometry;
    IScene::LightCollection lights;

    scene.PotentiallyVisibleGeometry(geometry, viewer.ViewFrustum());
    scene.PotentiallyRelevantLights(lights, viewer.ViewFrustum());

    RenderGBuffer(viewer, geometry);
    ApplyLights(viewer, scene, geometry, lights);
    m_glInterface.GeometryCache->UnbindGeometry();

    PushLoadProjectionMatrix pushProjectionMatrix(
            math::GenerateOrthographicProjectionTransform(
                    0, m_width, m_height, 0));

    PushLoadModelViewMatrix pushIdentityModelViewMatrix();

    ShaderProgram::Unbind();
    target.Bind();

    m_glInterface.TextureMgr->SetUnit(0, true);

    m_glInterface.TextureMgr->BindTexture(
            0, m_lightBuffer.ColorTextureGLHandle(0));
    RenderQuad(0, 0, m_width, m_height);

    m_glInterface.TextureMgr->SetUnit(0, false);
}

void DeferredSceneRenderer::RenderGBuffer(const Camera& viewer,
                                     const IScene::GeometryCollection& geometry)
{
    m_gBuffer.Bind();
    m_gBufferShader->Bind();

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    PushAttribute pushColorBufferBit(GL_COLOR_BUFFER_BIT);

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0_EXT,
                             GL_COLOR_ATTACHMENT1_EXT,
                             GL_COLOR_ATTACHMENT2_EXT };
    glDrawBuffers(3, drawBuffers);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    PushLoadProjectionMatrix pushProjectionMatrix(
            viewer.ProjectionTransform());

    PushLoadModelViewMatrix pushModelViewMatrix(viewer.ViewTransform());

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableVertexAttribArray(m_tangentAttributeLocation);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    std::for_each(geometry.begin(), geometry.end(),
                  boost::bind(&DeferredSceneRenderer::RenderGBufferGCI,
                              this, _1));

    glDisable(GL_CULL_FACE);

    glDisableVertexAttribArray(m_tangentAttributeLocation);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

void DeferredSceneRenderer::RenderGBufferGCI(GeometryChunkInstance* gcip)
{
    ASSERT(gcip->GeometryChunk());
    ASSERT(gcip->SurfaceDescription().GetPointer());

    PushMultiplyModelViewMatrix pushModelViewMatrix(gcip->Transform());

    math::Matrix33 objectToWorld =
            math::Submatrix<3, 3, 0, 0>(gcip->Transform());
    Invert(Transpose(objectToWorld));

    m_gBufferShader->SetUniformParameter("ObjectToWorldTransform",
                                         gcip->Transform());
    m_gBufferShader->SetUniformParameter(
            "InverseTransposeObjectToWorldTransform",
            objectToWorld);
    m_gBufferShader->SetUniformParameter("DiffuseTexture", 0u);
    m_gBufferShader->SetUniformParameter("NormalTexture", 1u);
    m_gBufferShader->SetUniformParameter(
            "SpecularAlbedo",
            gcip->SurfaceDescription()->SpecularAlbedo());
    m_gBufferShader->SetUniformParameter(
            "SpecularExponent",
            gcip->SurfaceDescription()->SpecularExponent());

    m_glInterface.TextureMgr->BindTexture(
            0, gcip->SurfaceDescription()->DiffuseAlbedoTexture());
    m_glInterface.TextureMgr->BindTexture(
            1, gcip->SurfaceDescription()->NormalTexture());

    const GeometryChunk& gc = *gcip->GeometryChunk();
    ASSERT(gc.IndexCount() % 3 == 0);

    m_glInterface.GeometryCache->BindGeometry(&gc);
    uint_t offset = m_glInterface.GeometryCache->BufferOffset();

    const uint_t vec3Size = sizeof(math::Vector3) * gc.VertexCount();
    glVertexPointer(3, GL_ROMULUS_REAL, 0, static_cast<ubyte_t*>(0) + offset);
    offset += vec3Size;
    glNormalPointer(GL_ROMULUS_REAL, 0, static_cast<ubyte_t*>(0) + offset);
    offset += vec3Size;
    glVertexAttribPointer(m_tangentAttributeLocation, 3, GL_ROMULUS_REAL,
                          GL_FALSE, 0, static_cast<ubyte_t*>(0) + offset);
    offset += vec3Size;
    glTexCoordPointer(2, GL_ROMULUS_REAL, 0, static_cast<ubyte_t*>(0) + offset);

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(gc.IndexCount()),
                   GL_UNSIGNED_SHORT, gc.Indices());
}

void DeferredSceneRenderer::ApplyLights(
        const Camera& viewer, IScene& scene,
        const IScene::GeometryCollection& geometry,
        const IScene::LightCollection& lights)
{
    m_lightBuffer.Bind();
    glClear(GL_COLOR_BUFFER_BIT);

    glDepthMask(GL_FALSE);

    PushLoadProjectionMatrix pushProjectionMatrix(
            math::GenerateOrthographicProjectionTransform(
                    0, m_width, m_height, 0));

    std::for_each(lights.begin(), lights.end(),
                  boost::bind(&DeferredSceneRenderer::EvaluateLight, this,
                              viewer, boost::ref(scene), _1));

    glDepthMask(GL_TRUE);
}

void DeferredSceneRenderer::EvaluateLight(const Camera& viewer, IScene& scene,
                                          const Light* light)
{
    if (IsExactType<PointLight>(*light))
    {
        EvaluatePointLight(viewer, scene,
                           static_cast<const PointLight*>(light));
    }
}

void DeferredSceneRenderer::EvaluatePointLight(const Camera& viewer,
                                               IScene& scene,
                                               const PointLight* pointLight)
{
    const math::Matrix44 lightViewTransformMatrix(
            math::Translation(-1.f * pointLight->Position()).Matrix());

    // First, we render the point light shadow maps.
    if (pointLight->CastsShadows())
    {
        RenderPointLightShadowMaps(scene, lightViewTransformMatrix,
                                   pointLight->FarAttenuation());
    }
    else
    {
        glViewport(0, 0, m_shadowMapSize * 2, m_shadowMapSize);
        m_shadowMapBuffer.Bind();
        glDepthMask(GL_TRUE);
        glClear(GL_DEPTH_BUFFER_BIT);
        glDepthMask(GL_FALSE);
        glViewport(0, 0, m_width, m_height);
    }

    // Apply lighting to the point light's region of influence.
    m_lightBuffer.Bind();
    m_pointLightShader->Bind();

    m_pointLightShader->SetUniformParameter("WindowWidth",
                                            static_cast<real_t>(m_width));
    m_pointLightShader->SetUniformParameter("WindowHeight",
                                            static_cast<real_t>(m_height));
    m_pointLightShader->SetUniformParameter("GBuffer1", 0u);
    m_pointLightShader->SetUniformParameter("GBuffer2", 1u);
    m_pointLightShader->SetUniformParameter("GBuffer3", 2u);
    m_pointLightShader->SetUniformParameter("LightAccumulation", 3u);

    m_pointLightShader->SetUniformParameter("LightViewTransform",
                                            lightViewTransformMatrix);
    m_pointLightShader->SetUniformParameter("FrontAndBackShadowMap", 4u);
    m_pointLightShader->SetUniformParameter("ShadowMapSize",
                                            static_cast<float>(
                                                    m_shadowMapSize));

    m_glInterface.TextureMgr->BindTexture(0, m_gBuffer.ColorTextureGLHandle(0));
    m_glInterface.TextureMgr->BindTexture(1, m_gBuffer.ColorTextureGLHandle(1));
    m_glInterface.TextureMgr->BindTexture(2, m_gBuffer.ColorTextureGLHandle(2));
    m_glInterface.TextureMgr->BindTexture(
            3, m_lightBuffer.ColorTextureGLHandle(0));
    m_glInterface.TextureMgr->BindTexture(4, m_shadowMapTextureFrontAndBack);

    const math::Vector3& position = pointLight->Position();
    m_pointLightShader->SetUniformParameter("LightPosition", position);
    m_pointLightShader->SetUniformParameter(
            "LightColor",
            math::Vector3(pointLight->Color()[0],
                          pointLight->Color()[1],
                          pointLight->Color()[2]));
    m_pointLightShader->SetUniformParameter("LightIntensity",
                                            pointLight->Intensity());
    m_pointLightShader->SetUniformParameter("ViewerPosition",
                                            viewer.Position());

    m_pointLightShader->SetUniformParameter("FarAttenuation",
                                            pointLight->FarAttenuation());

    ASSERT_OPENGL_STATE();

    const math::Frustum& frustum = viewer.ViewFrustum();
    real_t furthestDistanceToNearPlane = math::Magnitude(
            math::Vector3(frustum.NearDistance(),
                          math::Max(fabs(frustum.NearPlaneIntersectionOffset(
                                                 math::Frustum::Plane_Top)),
                                    fabs(frustum.NearPlaneIntersectionOffset(
                                                 math::Frustum::Plane_Bottom))),
                          math::Max(fabs(frustum.NearPlaneIntersectionOffset(
                                                 math::Frustum::Plane_Left)),
                                    fabs(frustum.NearPlaneIntersectionOffset(
                                                 math::Frustum::Plane_Right))))
                                                         );

    real_t distanceToLight = math::Magnitude(position - viewer.Position());
    bool viewerInLight = distanceToLight < pointLight->FarAttenuation();
    bool viewerMayBeInLight = math::Max(static_cast<real_t>(0.0),
                                        distanceToLight -
                                        furthestDistanceToNearPlane) <
            pointLight->FarAttenuation();
    bool lightWithinFarPlane = distanceToLight + pointLight->FarAttenuation() <
            frustum.FarDistance();

    if (viewerMayBeInLight && !(viewerInLight && lightWithinFarPlane))
    {
        // We can't be sure that a render of the front faces of the
        // light's bounding sphere won't clip the near plane, nor that a render
        // of the back faces won't clip the far plane, so to be safe we apply
        // the light over the whole screen. Attenuation is applied in the
        // shader.
        glDisable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);
        RenderQuad(0, 0, m_width, m_height);
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        if (viewerInLight && lightWithinFarPlane)
        {
            // If we're in the light's bounding sphere and we're sure
            // that the back faces won't be clipped, then we apply lighting to
            // any pixels whose depth is less than that of the light sphere's
            // back faces.
            glDepthFunc(GL_GEQUAL);
            glCullFace(GL_FRONT);
        }
        else
        {
            // Otherwise, we apply the light to any pixels whose depth is
            // greater than that of the light sphere's front faces.
            glCullFace(GL_BACK);
        }

        PushLoadProjectionMatrix pushProjectionMatrix(
                viewer.ProjectionTransform());
        PushLoadModelViewMatrix pushModelViewMatrix(viewer.ViewTransform());

        glTranslatef(position[0], position[1], position[2]);

        glEnable(GL_CULL_FACE);

        GLUquadric* sphere = gluNewQuadric();
        gluSphere(sphere, pointLight->FarAttenuation(), 10, 10);
        gluDeleteQuadric(sphere);

        glDisable(GL_CULL_FACE);
        glDepthFunc(GL_LEQUAL);
    }
}

void DeferredSceneRenderer::RenderPointLightShadowMaps(
        IScene& scene, const math::Matrix44& lightViewTransformMatrix,
        const real_t farAttenuation)
{
    // We use dual paraboloid shadow maps, both rendered to a single wide
    // texture. We use the second depth method to eliminate precision artifacts
    // on lit surfaces.

    // Bind the shadow map buffer, whose depth buffer is the shadow map.
    m_shadowMapBuffer.Bind();

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    // Cull front faces for second depth.
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);

    // Alpha test is used to discard fragments behind the current paraboloid.
    glEnable(GL_ALPHA_TEST);
    // We subtract a small fudge factor from 0.5 to avoid culling pixels at
    // the seams of the two hemispheres.
    glAlphaFunc(GL_GEQUAL, 0.49f);

    m_paraboloidShadowMapShader->Bind();
    // The lighting pass assumes the near distance is zero.
    m_paraboloidShadowMapShader->SetUniformParameter("NearDistance", 0.f);
    m_paraboloidShadowMapShader->SetUniformParameter(
            "FarDistance", farAttenuation);

    ASSERT_OPENGL_STATE();

    {
        PushAttribute pushColorBufferBit(GL_COLOR_BUFFER_BIT);

        // Collect geometry potentially visible to the light. We check against
        // the 6 frusta covering a cube around the light.
        IScene::GeometryCollection geometry;
        math::Frustum f;
        math::Matrix44 lightPerspectiveMatrix(
                math::GeneratePerspectiveProjectionTransform(
                        math::DegreesToRadians(90.f), 1.f, 0.001f,
                        farAttenuation));
        // Negative z axis.
        f.Compute(lightPerspectiveMatrix * lightViewTransformMatrix);
        scene.PotentiallyVisibleGeometry(geometry, f);
        // Positive y axis.
        math::Rotation viewRotation(math::Vector3(1.f, 0, 0),
                                    math::DegreesToRadians(-90.f));
        f.Compute(lightPerspectiveMatrix * viewRotation.Matrix() *
                  lightViewTransformMatrix);
        scene.PotentiallyVisibleGeometry(geometry, f);
        // Negative y axis.
        viewRotation.SetAxisAngle(math::Vector3(1.f, 0, 0),
                                  math::DegreesToRadians(90.f));
        f.Compute(lightPerspectiveMatrix * viewRotation.Matrix() *
                  lightViewTransformMatrix);
        scene.PotentiallyVisibleGeometry(geometry, f);
        // Positive z axis.
        viewRotation.SetAxisAngle(math::Vector3(1.f, 0, 0),
                                  math::DegreesToRadians(180.f));
        f.Compute(lightPerspectiveMatrix * viewRotation.Matrix() *
                  lightViewTransformMatrix);
        scene.PotentiallyVisibleGeometry(geometry, f);
        // Positive x axis.
        viewRotation.SetAxisAngle(math::Vector3(0, 1, 0),
                                  math::DegreesToRadians(90.f));
        f.Compute(lightPerspectiveMatrix * viewRotation.Matrix() *
                  lightViewTransformMatrix);
        scene.PotentiallyVisibleGeometry(geometry, f);
        // Negative x axis.
        viewRotation.SetAxisAngle(math::Vector3(0, 1, 0),
                                  math::DegreesToRadians(-90.f));
        f.Compute(lightPerspectiveMatrix * viewRotation.Matrix() *
                  lightViewTransformMatrix);
        scene.PotentiallyVisibleGeometry(geometry, f);

        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_TRUE);
        glEnableClientState(GL_VERTEX_ARRAY);

        math::Matrix44 identity;
        PushLoadProjectionMatrix pushProjectionMatrix(SetIdentity(identity));

        // Render the front side of the dual shadow map.
        glViewport(0, 0, m_shadowMapBuffer.Width() / 2,
                   m_shadowMapBuffer.Height());
        glClear(GL_DEPTH_BUFFER_BIT);
        {
            PushLoadModelViewMatrix pushModelViewMatrix(
                    lightViewTransformMatrix);

            for (IScene::GeometryCollection::iterator it = geometry.begin();
                 it != geometry.end(); ++it)
            {
                const GeometryChunkInstance* gcip = *it;
                const GeometryChunk& gc = *gcip->GeometryChunk();
                ASSERT(gc.IndexCount() % 3 == 0);

                PushMultiplyModelViewMatrix multiplyModelViewMatrix(
                        gcip->Transform());

                m_glInterface.GeometryCache->BindGeometry(&gc);
                glVertexPointer(3, GL_ROMULUS_REAL, 0,
                                static_cast<ubyte_t*>(0) +
                                m_glInterface.GeometryCache->BufferOffset());

                glDrawElements(GL_TRIANGLES,
                               static_cast<GLsizei>(gc.IndexCount()),
                               GL_UNSIGNED_SHORT, gc.Indices());
            }
        }

        // Render the back side of the dual shadow map.
        glViewport(m_shadowMapBuffer.Width() / 2, 0,
                   m_shadowMapBuffer.Width() / 2, m_shadowMapBuffer.Height());
        {
            PushLoadModelViewMatrix pushModelViewMatrix(
                    math::Rotation(math::Vector3(0, 1, 0), math::Pi).Matrix() *
                    lightViewTransformMatrix);

            for (IScene::GeometryCollection::iterator it = geometry.begin();
                 it != geometry.end(); ++it)
            {
                const GeometryChunkInstance* gcip = *it;
                const GeometryChunk& gc = *gcip->GeometryChunk();
                ASSERT(gc.IndexCount() % 3 == 0);

                PushMultiplyModelViewMatrix multiplyModelViewMatrix(
                        gcip->Transform());

                m_glInterface.GeometryCache->BindGeometry(&gc);
                glVertexPointer(3, GL_ROMULUS_REAL, 0,
                                static_cast<ubyte_t*>(0) +
                                m_glInterface.GeometryCache->BufferOffset());

                glDrawElements(GL_TRIANGLES,
                               static_cast<GLsizei>(gc.IndexCount()),
                               GL_UNSIGNED_SHORT, gc.Indices());
            }
        }

        glDisableClientState(GL_VERTEX_ARRAY);
        glDepthMask(GL_FALSE);
    }

    glDisable(GL_ALPHA_TEST);

    // Reset the viewport.
    glViewport(0, 0, m_width, m_height);
}

}
}
}
