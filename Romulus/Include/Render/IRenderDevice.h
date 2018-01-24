#ifndef _RENDERIRENDERDEVICE_H_
#define _RENDERIRENDERDEVICE_H_

#include "Math/Matrix.h"
#include "Math/Plane.h"
#include "Render/ISceneRenderer.h"
#include "Render/IOrthographicRenderer.h"

namespace romulus
{
namespace render
{

enum BlendSourceFunction
{
    BlendSource_Source_Alpha
};

enum BlendDestinationFunction
{
    BlendDestination_One_Minus_Source_Alpha
};

//! Enumeration for user-defined clipping planes.
enum ClipPlane
{
    ClipPlane_Plane0 = 0,
    ClipPlane_Plane1,
    ClipPlane_Plane2,
    ClipPlane_Plane3
};

class IRenderDevice
{
public:

    IRenderDevice() { }
    virtual ~IRenderDevice() { }

    virtual void ClearBuffers(bool color, bool depth, bool stencil) = 0;
    virtual void SwapBuffers() = 0;

    virtual void SetProjectionTransform(const math::Matrix44& transform) = 0;

    virtual ISceneRenderer* SceneRenderer() const = 0;
    virtual IOrthographicRenderer* OrthographicRenderer() const = 0;

    /** State management methods. */

    virtual bool BlendState() const = 0;
    virtual void SetBlendState(bool enabled) = 0;
    virtual BlendSourceFunction BlendSource() const = 0;
    virtual void SetBlendSource(BlendSourceFunction function) = 0;
    virtual BlendDestinationFunction BlendDestination() const = 0;
    virtual void SetBlendDestination(BlendDestinationFunction function) = 0;

    /** Texture methods */

    virtual TexturePtr LoadTexture(std::istream& stream) = 0;
    virtual void SetTextureUnitState(int unit, bool enabled) = 0;
    virtual void BindTexture(int unit, const TexturePtr texture) = 0;

    //! Set a user-defined clipping plane.
    //! \param index - The clip plane to set.
    //! \param plane - The plane.
    virtual void SetClippingPlane(ClipPlane index,
                                  const math::Plane& plane) = 0;
    //! Enable a user-defined clipping plane.
    //! \param plane - The plane index.
    virtual void EnableClippingPlane(ClipPlane plane) = 0;
    //! Disable a user-defined clipping plane.
    //! \param plane - The plane index.
    virtual void DisableClippingPlane(ClipPlane plane) = 0;
};

}
}

#endif // _RENDERIRENDERDEVICE_H_
