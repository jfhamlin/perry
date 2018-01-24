#ifndef _MATHFRUSTUM_H_
#define _MATHFRUSTUM_H_

//! \file Frustum.h
//! Contains Frustum class definition

#include "Math/Constants.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/Utilities.h"
#include "Math/Plane.h"
#include "Utility/Assertions.h"

namespace romulus
{
namespace math
{
//! Frustum class.
//! Represents a fully general view frustum.
class Frustum
{
public:

    enum FrustumPlaneSide
    {
        Plane_Left = 0,
        Plane_Right,
        Plane_Bottom,
        Plane_Top,
        Plane_Near,
        Plane_Far
    };

    //! Frustum ctor.
    inline Frustum();

    inline void Compute(const Matrix44& transform);

    //! Retrieve one of the frustum planes.
    //! \param plane - The plane to retrieve.
    //! \return The requested plane.
    inline const Plane& FrustumPlane(FrustumPlaneSide plane) const;

    //! Retrieve the near plane distance.
    inline real_t NearDistance() const;
    //! Set the near plane distance.
    //! \param distance - The new distance.
    inline void SetNearDistance(real_t distance);

    //! Retrieve the far plane distance.
    inline real_t FarDistance() const;
    //! Set the far plane distance.
    //! \param distance - The new distance.
    inline void SetFarDistance(real_t distance);

    inline real_t NearPlaneIntersectionOffset(FrustumPlaneSide plane) const;

private:

    inline void ComputeNearPlaneIntersectionOffsets();

    //! The frustum planes.
    Plane m_planes[6];

    real_t m_nearPlaneIntersectionOffsets[6];
};

}
}

#include "Math/Frustum.inl"

#endif // _MATHFRUSTUM_H_
