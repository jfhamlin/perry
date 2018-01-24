#ifndef _RENDERISCENE_H_
#define _RENDERISCENE_H_

//! \file IScene.h
//! Contains the scene interface.

#include "Math/Frustum.h"
#include "Render/GeometryChunkInstance.h"
#include "Render/Light.h"
#include <set>

namespace romulus
{
namespace render
{

//! The render required interface to a scene.
class IScene
{
public:

    typedef std::set<const GeometryChunkInstance*> GeometryCollection;
    typedef std::set<const Light*> LightCollection;

    IScene() { }
    virtual ~IScene() { }

    //! Collect potentially visible geometry.
    //! \param geometry - The output geometry. Any geometry already in the
    //!                   collection will remain in the collection.
    //! \param viewFrustum - The view frustum.
    virtual void PotentiallyVisibleGeometry(
            GeometryCollection& geometry,
            const math::Frustum& viewFrustum) const = 0;
    virtual void PotentiallyRelevantLights(
            LightCollection& lights,
            const math::Frustum& viewFrustum) const = 0;

    virtual void Geometry(GeometryCollection& geometry) const = 0;
    virtual void Lights(LightCollection& lights) const = 0;
};

}
}

#endif // _RENDERISCENE_H_
