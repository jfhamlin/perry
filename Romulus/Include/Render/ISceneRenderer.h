#ifndef _RENDERISCENERENDERER_H_
#define _RENDERISCENERENDERER_H_

//! \file ISceneRenderer.h
//! Contains the scene renderer interface.

#include "Render/Camera.h"
#include "Render/IScene.h"

namespace romulus
{
namespace render
{

//! The scene renderer interface.
class ISceneRenderer
{
public:

    ISceneRenderer() { }
    virtual ~ISceneRenderer() { }

    //! Render the input scene.
    //! \param viewer - The scene viewer.
    //! \param scene - The input scene to render.
    virtual void RenderScene(const real_t deltaTime,
                             const Camera& viewer, IScene& scene) = 0;
};

}
}

#endif // _RENDERISCENERENDERER_H_
