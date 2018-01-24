#ifndef _RENDERCAMERA_H_
#define _RENDERCAMERA_H_

//! \file Camera.h
//! Contains the camera class declaration.

#include "Math/Matrix.h"
#include "Math/Frustum.h"
#include "Math/Transformations.h"

namespace romulus
{
namespace render
{

//! Render required camera.
class Camera
{
public:

    inline Camera():
        m_transformation(math::Translation(0.f, 0.f, 0.f),
                         math::Rotation(math::Vector3(0.f, 1.f, 0.f), 0.f),
                         math::UniformScale(1.f)) {
        m_viewTransform = Inverse(m_transformation).Matrix();
    }

    inline void SetPosition(const math::Vector3& position)
    {
        m_transformation.SetTranslation(math::Translation(position));
        m_viewTransform = Inverse(m_transformation).Matrix();
        ComputeFrustum();
    }

    inline void SetOrientation(const math::Rotation& orientation)
    {
        m_transformation.SetRotation(orientation);
        m_viewTransform = Inverse(m_transformation).Matrix();
        ComputeFrustum();
    }

    inline void SetProjectionTransform(const math::Matrix44& transform)
    {
        m_projectionTransform = transform;
        ComputeFrustum();
    }

    inline const math::Transformation& Transformation() const
    {
        return m_transformation;
    }

    inline const math::Vector3& Position() const
    {
        return m_transformation.Translation().Vector();
    }

    inline const math::Rotation& Orientation() const
    {
        return m_transformation.Rotation();
    }

    inline const math::Matrix44& ViewTransform() const
    {
        return m_viewTransform;
    }

    inline const math::Matrix44& ProjectionTransform() const
    {
        return m_projectionTransform;
    }

    inline math::Vector3 ViewVector() const
    {
        return math::TransformDirection(m_transformation,
                                        math::Vector3(0, 0, -1));
    }

    inline math::Vector3 UpVector() const
    {
        return math::TransformDirection(m_transformation,
                                        math::Vector3(0, 1, 0));
    }

    inline math::Vector3 RightVector() const
    {
        return math::TransformDirection(m_transformation,
                                        math::Vector3(1, 0, 0));
    }

    inline const math::Frustum& ViewFrustum() const { return m_viewFrustum; }

private:

    inline void ComputeFrustum()
    {
        m_viewFrustum.Compute(m_projectionTransform * m_viewTransform);
    }

    math::Transformation m_transformation;
    math::Matrix44 m_viewTransform;
    math::Matrix44 m_projectionTransform;
    math::Frustum m_viewFrustum;
};

}
}

#endif // _RENDERCAMERA_H_
