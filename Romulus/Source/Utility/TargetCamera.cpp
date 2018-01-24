#include "Utility/TargetCamera.h"

namespace romulus
{

using namespace math;

void TargetCamera::Pan(Vector2& direction, real_t angle)
{
    Vector3 direction3 =
            direction[0] * m_camera.RightVector() +
            direction[1] * m_camera.UpVector();

    if (direction3 == Vector3(0, 0, 0))
        return;

    Normalize(direction3);
    Rotation r(Cross(direction3, m_camera.ViewVector()), angle);
    Transformation t = m_camera.Transformation();
    t.Translation() = Translation(m_camera.Position() - m_target);

    t = Concatenate(r, t);
    m_camera.SetOrientation(t.Rotation());
    m_camera.SetPosition(t.Translation().Vector() + m_target);
}

void TargetCamera::Track(Vector2& displacement)
{
    const Vector3 displacement3 =
            displacement[0] * m_camera.RightVector() +
            displacement[1] * m_camera.UpVector();

    m_camera.SetPosition(m_camera.Position() + displacement3);
    m_target += displacement3;
}

void TargetCamera::Dolly(real_t distance)
{
    ASSERT_EQ_EPSILON(Magnitude(m_camera.ViewVector()), 1.0,
                      AcceptableMathEpsilon);
    m_camera.SetPosition(m_camera.Position() +
                         m_camera.ViewVector() * distance);

    // If the camera has passed the target, place it back at the target.
    if (Dot(m_target - m_camera.Position(), m_camera.ViewVector()) < 0.0)
        m_camera.SetPosition(Target());
}

void TargetCamera::Roll(real_t angle)
{
    Rotation r(m_camera.ViewVector(), -angle);
    m_camera.SetOrientation(Concatenate(r, m_camera.Orientation()));
}

void TargetCamera::Translate(const romulus::math::Vector3& translation)
{
    m_target += translation;
    m_camera.SetPosition(m_camera.Position() + translation);
}

void TargetCamera::Rotate(const romulus::math::Vector3& axis,
                                  real_t angle)
{
    Vector3 normAx = Normal(axis);
    Rotation r(normAx, angle);
    Transformation t = m_camera.Transformation();
    t.Translation() = Translation(m_camera.Position() - m_target);

    t = Concatenate(r, t);
    m_camera.SetOrientation(t.Rotation());
    m_camera.SetPosition(t.Translation().Vector() + m_target);
}

void TargetCamera::SetTarget(const Vector3& target)
{
    m_target = target;
    if (m_camera.Position() != target)
        SetViewVector(Normal(target - m_camera.Position()));
}

void TargetCamera::SetCameraPosition(const Vector3& position)
{
    m_camera.SetPosition(position);
    if (position != m_target)
        SetViewVector(Normal(m_target - position));
}

void TargetCamera::SetViewVector(const Vector3& viewVector)
{
    Vector3 axis;
    real_t angle;

    if (viewVector == m_camera.ViewVector())
    {
        return;
    }
    else if (viewVector == -1.f * m_camera.ViewVector())
    {
        axis = m_camera.UpVector();
        angle = Pi;
    }
    else
    {
        axis = Normal(Cross(m_camera.ViewVector(), viewVector));
        ASSERT(IsUnitLength(axis));
        ASSERT(IsUnitLength(m_camera.ViewVector()));
        angle = ACosClamp(Dot(m_camera.ViewVector(), viewVector));
    }

    m_camera.SetOrientation(Concatenate(Rotation(axis, angle),
                                        m_camera.Orientation()));
}

} // namespace romulus
