#ifndef _RENDERPOINTLIGHT_H_
#define _RENDERPOINTLIGHT_H_

//! \file PointLight.h
//! Contains PointLight class definition.

#include "Math/Bounds/BoundingVolumes.h"
#include "Math/Vector.h"
#include "Render/Light.h"

namespace romulus
{
namespace render
{

//! Definition of the PointLight class, which represents a point light source.
class PointLight : public Light
{
public:

    inline PointLight():
        m_color(1, 1, 1, 1), m_intensity(1), m_castShadows(false),
        m_boundingSphere(math::Vector3(0.f), 10.f) { }

    inline PointLight(const render::Color& color, real_t intensity,
                      bool castShadows, const math::Vector3& position,
                      real_t farAttenuation):
        m_color(color), m_intensity(intensity), m_castShadows(castShadows),
        m_boundingSphere(position, farAttenuation)
    {
    }

    //! \return The intensity of the light.
    virtual real_t Intensity() const { return m_intensity; }

    //! Set the intensity of the light.
    inline void SetIntensity(real_t intensity) { m_intensity = intensity; }

    //! \return The color of the light.
    virtual const render::Color& Color() const { return m_color; }

    //! Set the color of the light.
    inline void SetColor(const render::Color& color) { m_color = color; }

    //! \return true if this light casts shadows.
    inline bool CastsShadows() const { return m_castShadows; }

    //! Set the shadow casting behavior of the light.
    //! \param cast - If true, this light will cast shadows.
    inline void SetCastsShadows(bool cast) { m_castShadows = cast; }

    //! \return An IBoundingVolume bounding the region of the light's
    //!         influence.
    virtual const math::IBoundingVolume& BoundingVolume() const
    {
        return m_boundingSphere;
    }

    //! \return The light's world position.
    virtual const math::Vector3& Position() const
    {
        return m_boundingSphere.Sphere::Center();
    }

    //! Set the light's world position.
    inline void SetPosition(const math::Vector3& pos)
    {
        m_boundingSphere.SetCenter(pos);
    }

    //! \return The light's far attenuation distance.
    inline real_t FarAttenuation() const
    { return m_boundingSphere.Radius(); }

    //! Set the light's far attenuation distance.
    inline void SetFarAttenuation(real_t farAttenuation)
    {
        m_boundingSphere.SetRadius(farAttenuation);
    }

private:

    render::Color m_color;
    real_t m_intensity;
    bool m_castShadows;
    //! The bounding sphere of the light's region of influence, representing
    //! both its position and far attenuation value.
    math::BoundingSphere m_boundingSphere;
};

}
}

#endif // _RENDERPOINTLIGHT_H_
