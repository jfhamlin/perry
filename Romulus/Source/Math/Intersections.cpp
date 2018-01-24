#include "Math/Intersections.h"
#include <cmath>

namespace romulus
{
namespace math
{

bool Intersects(const AxisAlignedBox& aab,
                const Vector3& v0, const Vector3& v1, const Vector3& v2)
{
    if (aab.Contains(v0) || aab.Contains(v1) || aab.Contains(v2))
        return true;

    real_t t;
    Ray r(v0, v1 - v0);
    if (Intersects(t, r, aab) && t >= 0.0 && t <= 1.0)
        return true;

    r.Origin = v1;
    r.Direction = v2 - v1;
    if (Intersects(t, r, aab) && t >= 0.0 && t <= 1.0)
        return true;

    r.Origin = v2;
    r.Direction = v0 - v2;
    if (Intersects(t, r, aab) && t >= 0.0 && t <= 1.0)
        return true;

    for (int x = 0; x < 2; ++x)
    {
        for (int y = 0; y < 2; ++y)
        {
            int z = 0;
            r.Origin = aab.Corner(x, y, z);
            r.Direction = aab.Corner((x+1)%2, (y+1)%2, 1) - r.Origin;
            if (Intersects(t, r, v0, v1, v2) && t >= 0.0 && t <= 1.0)
                return true;
        }
    }

    return false;
}

} // namespace math
} // namespace romulus
