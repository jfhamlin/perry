#include "Intersections.h"
#include <queue>

namespace romulus
{
namespace math
{

namespace
{
typedef std::pair<std::pair<real_t, real_t>, const LineSegment*> Event;
} // namespace

uint_t PSLG::CountIntersections() const
{
    std::priority_queue<Event> q;
    for (uint_t i = 0; i < m_segments.size(); ++i)
    {
        q.push(std::make_pair(
                       std::make_pair(m_segments[i].Begin[0],
                                      m_segments[i].Begin[1]),
                       &m_segments[i]));
        q.push(std::make_pair(
                       std::make_pair(m_segments[i].End[0],
                                      m_segments[i].End[1]),
                       &m_segments[i]));
    }

    uint_t count = 0;
    std::set<const LineSegment*> active;
    while (q.size())
    {
        Event e = q.top();
        q.pop();

        std::set<const LineSegment*>::iterator it = active.find(e.second);
        if (it != active.end())
        {
            active.erase(it);
            continue;
        }

        Vector2 dummy;
        for (it = active.begin(); it != active.end(); ++it)
        {
            if (Intersects(dummy,
                           e.second->Begin, e.second->End,
                           (*it)->Begin, (*it)->End))
                ++count;
        }

        active.insert(e.second);
    }

    return count;
}

} // namespace math
} // namespace romulus
