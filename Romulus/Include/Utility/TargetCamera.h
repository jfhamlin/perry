#ifndef _TARGETCAMERA_H_
#define _TARGETCAMERA_H_

//! \file TargetCamera.h
//! Contains the declaration of the TargetCamera class, which has a constrained
//! view vector.

#include "Render/Camera.h"

namespace romulus
{

class TargetCamera
{
public:

    inline TargetCamera(): m_target(0, 0, 0)
    {
        m_camera.SetPosition(romulus::math::Vector3(0.0, 0.0, 1.0));
    }

    inline const romulus::render::Camera& Camera() const
    {
        return m_camera;
    }

    inline const romulus::math::Vector3& Target() const
    {
        return m_target;
    }

    inline void SetProjectionTransform(const romulus::math::Matrix44& transform)
    {
        m_camera.SetProjectionTransform(transform);
    }

    void Pan(romulus::math::Vector2& direction, real_t angle);

    void Track(romulus::math::Vector2& displacement);

    void Dolly(real_t distance);

    void Roll(real_t angle);

    void Translate(const romulus::math::Vector3& translation);

    void Rotate(const romulus::math::Vector3& axis, real_t angle);

    void SetTarget(const romulus::math::Vector3& target);

    void SetCameraPosition(const romulus::math::Vector3& position);

private:

    void SetViewVector(const romulus::math::Vector3& viewVector);

    romulus::math::Vector3 m_target;
    romulus::render::Camera m_camera;
};

} // namespace romulus

#endif // _TARGETCAMERA_H_
