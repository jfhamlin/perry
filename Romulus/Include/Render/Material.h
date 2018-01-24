#ifndef _RENDERMATERIAL_H_
#define _RENDERMATERIAL_H_

//! \file Material.h
//! Contains declaration for material property class.

#include "Render/Color.h"
#include "Render/Texture.h"
#include <boost/shared_ptr.hpp>

namespace romulus
{
namespace render
{

//! Contains parameters for describing a surface.
class Material
{
public:

    Material(): m_color(1.0, 1.0, 1.0, 1.0) { }
    ~Material() { }

    inline void SetColor(const render::Color& color)
    {
        m_color = color;
    }

    inline const render::Color& Color() const
    {
        return m_color;
    }

    inline void SetDiffuseAlbedoTexture(
            boost::shared_ptr<const Texture>& tex)
    {
        m_diffuseAlbedoTexture = tex;
    }

    inline boost::shared_ptr<const Texture> DiffuseAlbedoTexture() const
    {
        return m_diffuseAlbedoTexture;
    }

    inline void SetNormalTexture(boost::shared_ptr<const Texture>& tex)
    {
        m_normalTexture = tex;
    }

    inline boost::shared_ptr<const Texture> NormalTexture() const
    {
        return m_normalTexture;
    }

    inline void SetSpecularAlbedo(real_t specularAlbedo)
    {
        m_specularAlbedo = specularAlbedo;
    }

    inline real_t SpecularAlbedo() const
    {
        return m_specularAlbedo;
    }

    //! Set the specular exponent (shininess) of the material.
    //! \param exponent - The assigned specular exponent.
    inline void SetSpecularExponent(real_t exponent)
    {
        m_specularExponent = exponent;
    }

    inline real_t SpecularExponent() const
    {
        return m_specularExponent;
    }

private:

    render::Color m_color;
    boost::shared_ptr<const Texture> m_diffuseAlbedoTexture;
    boost::shared_ptr<const Texture> m_normalTexture;
    real_t m_specularAlbedo;

    real_t m_specularExponent;
};

}
}

#endif // _RENDERMATERIAL_H_
