#ifndef _MATHRAY_H_
#define _MATHRAY_H_

//! \file Ray.h
//! Contains Ray class definition

#include "Math/Vector.h"

namespace romulus
{
    namespace math
    {
        //! Ray representation.
        class Ray
        {
        public:

            //! Ray ctor.
            Ray(): Origin(0, 0, 0), Direction(0, 0, 1) {}
            //! Ray ctor.
            Ray(const Vector3& origin, const Vector3& direction):
            Origin(origin), Direction(direction) {}

            inline Vector3 PointAt(const real_t time) const
            { return Origin + time * Direction; }

            Vector3 Origin;

            Vector3 Direction;
        };

        inline bool operator==(const Ray& lhs, const Ray& rhs)
        {
            if (lhs.Origin != rhs.Origin ||
                lhs.Direction != rhs.Direction)
                return false;
            return true;
        }

        inline bool operator!=(const Ray& lhs, const Ray& rhs)
        {
            return !(lhs == rhs);
        }
    }
}

#endif // _MATHRAY_H_
