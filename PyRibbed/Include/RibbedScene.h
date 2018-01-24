#ifndef _RIBBEDSCENE_H_
#define _RIBBEDSCENE_H_

#include "Render/GeometryChunkInstance.h"
#include "Render/IScene.h"
#include "Render/Material.h"
#include "Render/PointLight.h"
#include "Resource/Sweep.h"
#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>

class RibbedScene : public romulus::render::IScene
{
public:

    RibbedScene(): m_bg(0.6, 0.6, 0.6, 1.0) { }
    virtual ~RibbedScene() { }

    //! Collect potentially visible geometry.
    //! \param geometry - The output geometry. Any geometry already in the
    //!                   collection will remain in the collection.
    //! \param viewFrustum - The view frustum.
    virtual void PotentiallyVisibleGeometry(
            GeometryCollection& geometry,
            const romulus::math::Frustum& viewFrustum) const { }
    virtual void PotentiallyRelevantLights(
            LightCollection& lights,
            const romulus::math::Frustum& viewFrustum) const { }

    virtual void Geometry(GeometryCollection& geometry) const { }
    virtual void Lights(LightCollection& lights) const { }

    void SetBackgroundColor(const romulus::math::Vector3& c)
    { m_bg = romulus::render::Color(c[0], c[1], c[2], 1); }
    const romulus::render::Color& BackgroundColor() const { return m_bg; }

    virtual void Update(boost::python::object params) = 0;

    virtual void ApplyUpdate() = 0;

private:

    romulus::render::Color m_bg;
};

#endif // _RIBBEDSCENE_H_
