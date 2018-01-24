#ifndef _MATHPSLG_H_
#define _MATHPSLG_H_

#include "Math/Vector.h"
#include <vector>

namespace romulus
{
namespace math
{

struct LineSegment
{
    LineSegment(const Vector2& b, const Vector2& e): Begin(b), End(e) { }

    Vector2 Begin;
    Vector2 End;

    bool operator==(const LineSegment& other)
    {
        return (Begin == other.Begin && End == other.End) ||
                (Begin == other.End && End == other.Begin);
    }
};

//! Class for containing a planar straight line graph.
class PSLG
{
public:

    inline void AddSegment(const LineSegment& seg)
    { m_segments.push_back(seg); }

    inline uint_t CountIntersections() const;

private:

    std::vector<LineSegment> m_segments;
};

} // namespace math
} // namespace romulus

#include "PSLG.inl"

#endif // _MATHPSLG_H_
