#ifndef _RENDEROPENGLIGEOMETRYCACHE_H_
#define _RENDEROPENGLIGEOMETRYCACHE_H_

//! \file IGeometryCache.h
//! Contains geometry cache interface.

#include "Render/GeometryChunk.h"

namespace romulus
{
namespace render
{
namespace opengl
{

class IGeometryCache
{
public:

    IGeometryCache() { }
    virtual ~IGeometryCache() { }

    virtual void BindGeometry(const GeometryChunk* gc) = 0;
    virtual void UnbindGeometry() = 0;
    virtual uint_t BufferOffset() = 0;
};

}
}
}

#endif // _RENDEROPENGLIGEOMETRYCACHE_H_
