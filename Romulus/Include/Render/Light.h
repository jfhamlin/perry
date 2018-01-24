#ifndef _RENDERLIGHT_H_
#define _RENDERLIGHT_H_

//! \file Light.h
//! Contains Light abstract class declaration.

#include "Core/Object.h"
#include "Core/Types.h"
#include "Math/Vector.h"
#include "Render/Color.h"
#include "Scene/IBounded.h"

namespace romulus
{
namespace render
{

//! Light abstract class.
class Light : public scene::IBounded
{
public:

    virtual ~Light() {}

    //! \return The intensity of the light.
    virtual real_t Intensity() const = 0;

    //! \return The color of the light.
    virtual const render::Color& Color() const = 0;

    //! \return The position of the light.
    virtual const math::Vector3& Position() const = 0;

    //! \return An IBoundingVolume bounding the region of the light's
    //!         influence. Implements the IBounded interface.
    virtual const math::IBoundingVolume& BoundingVolume() const = 0;
};

}
}

#endif // _RENDERLIGHT_H_
