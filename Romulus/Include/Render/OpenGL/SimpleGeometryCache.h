#ifndef _RENDEROPENGLSIMPLEGEOMETRYCACHE_H_
#define _RENDEROPENGLSIMPLEGEOMETRYCACHE_H_

#include "Render/OpenGL/IGeometryCache.h"
#include <vector>
#include <map>

namespace romulus
{
namespace render
{
namespace opengl
{

//! A simple geometry cache.
//! This geometry cache uses a VBO per GC, relying on the driver
//! memory management.
class SimpleGeometryCache : public IGeometryCache
{
public:

    SimpleGeometryCache();
    virtual ~SimpleGeometryCache();

    virtual void BindGeometry(const GeometryChunk* gc);
    virtual void UnbindGeometry();
    virtual uint_t BufferOffset();

private:

    typedef std::map<const GeometryChunk*, uint_t> SlotMap;

    uint_t ConstructSlot(const GeometryChunk* gc) const;

    SlotMap m_slots;
};

}
}
}

#endif // _RENDEROPENGLSIMPLEGEOMETRYCACHE_H_
