#ifndef _RENDEROPENGLRENDERDEVICE_H_
#define _RENDEROPENGLRENDERDEVICE_H_

#include "Render/IRenderDevice.h"
#include "Utility/Log.h"
#include <boost/scoped_ptr.hpp>

namespace romulus
{
namespace render
{
namespace opengl
{
class ShaderProgramManager;
class TextureManager;
class IGeometryCache;

//! OpenGL render device implementation.
class RenderDevice : public IRenderDevice
{
public:

    explicit RenderDevice(int width, int height);
    virtual ~RenderDevice();

    virtual void ClearBuffers(bool color, bool depth, bool stencil);
    virtual void SwapBuffers();

    virtual void SetProjectionTransform(const math::Matrix44& transform);

    virtual IOrthographicRenderer* OrthographicRenderer() const
    {
        return m_orthoRenderer.get();
    }

    virtual ISceneRenderer* SceneRenderer() const
    {
        return m_sceneRenderer.get();
    }

    /** State management methods. */

    virtual bool BlendState() const { return m_blending; }
    virtual void SetBlendState(bool enabled);
    virtual BlendSourceFunction BlendSource() const { return m_blendSource; }
    virtual void SetBlendSource(BlendSourceFunction function);
    virtual BlendDestinationFunction BlendDestination() const
        { return m_blendDestination; }
    virtual void SetBlendDestination(BlendDestinationFunction function);

    /** Texture methods */

    virtual TexturePtr LoadTexture(std::istream& stream);
    virtual void SetTextureUnitState(int unit, bool enabled);
    virtual void BindTexture(int unit, const TexturePtr texture);

    virtual void SetClippingPlane(ClipPlane index,
                                  const math::Plane& plane);
    virtual void EnableClippingPlane(ClipPlane plane);
    virtual void DisableClippingPlane(ClipPlane plane);

private:

    void InitializeRenderStates();

    Log m_log;

    boost::scoped_ptr<ShaderProgramManager> m_shaderProgramMgr;
    boost::scoped_ptr<TextureManager> m_textureMgr;
    boost::scoped_ptr<IOrthographicRenderer> m_orthoRenderer;
    boost::scoped_ptr<ISceneRenderer> m_sceneRenderer;
    boost::scoped_ptr<IGeometryCache> m_geometryCache;

    int m_width, m_height;

    //! Render state members.
    bool m_blending;
    BlendSourceFunction m_blendSource;
    BlendDestinationFunction m_blendDestination;
};
}
}
}

#endif // _RENDEROPENGLRENDERDEVICE_H_
