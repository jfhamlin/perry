#ifndef _RENDEROPENGLDEFERREDSCENERENDERER_H_
#define _RENDEROPENGLDEFERREDSCENERENDERER_H_

//! \file DeferredRenderer.h
//! Contains

#include "Core/Types.h"
#include "Render/Camera.h"
#include "Render/OpenGL/Framebuffer.h"
#include "Render/OpenGL/ShaderProgram.h"
#include "Render/OpenGL/RenderPipelineStage.h"
#include "Utility/Common.h"
#include "Render/ISceneRenderer.h"
#include <boost/scoped_ptr.hpp>

namespace romulus
{
namespace render
{

class PointLight;

namespace opengl
{

class ShaderProgram;
class TextureManager;

class DeferredSceneRenderer : public RenderPipelineStage
{
PROHIBIT_COPYING(DeferredSceneRenderer);
public:
    DeferredSceneRenderer(const GLInterface& gli, uint_t width, uint_t height);
    ~DeferredSceneRenderer();

    virtual void Render(const real_t deltaTime, const Camera& viewer,
                        IScene& scene, const Framebuffer& input,
                        Framebuffer& target);

private:

    void RenderGBuffer(const Camera& viewer,
                       const IScene::GeometryCollection& geometry);
    void RenderGBufferGCI(GeometryChunkInstance* gcip);

    void ApplyLights(const Camera& viewer, IScene& scene,
                     const IScene::GeometryCollection& geometry,
                     const IScene::LightCollection& lights);
    void EvaluateLight(const Camera& viewer, IScene& scene, const Light* light);

    void EvaluatePointLight(const Camera& viewer, IScene& scene,
                            const PointLight* pointLight);
    void RenderPointLightShadowMaps(
            IScene& scene, const math::Matrix44& lightViewTransformMatrix,
            const real_t farAttenuation);

    uint_t m_width, m_height;
    Framebuffer m_gBuffer;
    Framebuffer m_lightBuffer;

    uint_t m_shadowMapSize;
    Framebuffer m_shadowMapBuffer;
    uint_t m_shadowMapTextureFrontAndBack;

    ShaderProgram* m_gBufferShader;
    ShaderProgram* m_pointLightShader;
    ShaderProgram* m_paraboloidShadowMapShader;

    int m_tangentAttributeLocation;
};

}
}
}

#endif // _RENDEROPENGLDEFERREDSCENERENDERER_H_
