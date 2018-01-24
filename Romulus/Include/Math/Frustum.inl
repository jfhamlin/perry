namespace romulus
{
namespace math
{

inline Frustum::Frustum()
{
}

inline void Frustum::Compute(const Matrix44& transform)
{
    Vector3 normal;
    real_t distance;

    // Extract planes from a projection (and possibly a concatenated
    // affine transform).
    for (int row = 0, i = 0; row < 3; ++row)
    {
        normal[0] = transform[3][0] + transform[row][0];
        normal[1] = transform[3][1] + transform[row][1];
        normal[2] = transform[3][2] + transform[row][2];
        distance  = transform[3][3] + transform[row][3];
        m_planes[i].SetNormal(normal);
        m_planes[i].SetDistanceFromOrigin(-distance);
        Normalize(m_planes[i]);

        ++i;

        normal[0] = transform[3][0] - transform[row][0];
        normal[1] = transform[3][1] - transform[row][1];
        normal[2] = transform[3][2] - transform[row][2];
        distance  = transform[3][3] - transform[row][3];
        m_planes[i].SetNormal(normal);
        m_planes[i].SetDistanceFromOrigin(-distance);
        Normalize(m_planes[i]);

        ++i;
    }

    ComputeNearPlaneIntersectionOffsets();
}

inline real_t Frustum::NearPlaneIntersectionOffset(FrustumPlaneSide plane) const
{
    ASSERT(plane < 6);
    return m_nearPlaneIntersectionOffsets[plane];
}

inline void Frustum::ComputeNearPlaneIntersectionOffsets()
{
    real_t nearDistance = NearDistance();
    const Vector3& nearPlane = m_planes[Plane_Near].Normal();
    for (int i = 0; i < 4; ++i)
    {
        const Vector3& plane = m_planes[i].Normal();
        real_t angle = ACosClamp(Dot(plane, nearPlane));

        m_nearPlaneIntersectionOffsets[i] = tanf(angle) * nearDistance;
    }
    m_nearPlaneIntersectionOffsets[Plane_Near] = 0.f;
    m_nearPlaneIntersectionOffsets[Plane_Far] = Infinity;
}

inline const Plane& Frustum::FrustumPlane(FrustumPlaneSide plane) const
{
    ASSERT(plane < 6);
    return m_planes[plane];
}

inline real_t Frustum::NearDistance() const
{
    return m_planes[Plane_Near].DistanceFromOrigin();
}

inline void Frustum::SetNearDistance(real_t distance)
{
    ASSERT(distance > 0.f);
    ASSERT(distance < FarDistance());
    m_planes[Plane_Near].SetDistanceFromOrigin(distance);
}

inline real_t Frustum::FarDistance() const
{
    return -m_planes[Plane_Far].DistanceFromOrigin();
}

inline void Frustum::SetFarDistance(real_t distance)
{
    ASSERT(distance > 0.f);
    ASSERT(distance > NearDistance());
    m_planes[Plane_Far].SetDistanceFromOrigin(-distance);
}

}
}
