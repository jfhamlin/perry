#include "Render/OpenGL/SimpleGeometryCache.h"
#include "Render/OpenGL/GLee.h"
#include "Utility/Assertions.h"

namespace romulus
{
namespace render
{
namespace opengl
{

SimpleGeometryCache::SimpleGeometryCache()
{
}

SimpleGeometryCache::~SimpleGeometryCache()
{
    for (SlotMap::iterator iter = m_slots.begin(); iter != m_slots.end();
         ++iter)
    {
        glDeleteBuffers(1, &iter->second);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SimpleGeometryCache::BindGeometry(const GeometryChunk* gc)
{
    ASSERT(gc);

    SlotMap::iterator iter = m_slots.find(gc);
    if (iter != m_slots.end())
    {
        if (gc->IsModified())
        {
            glDeleteBuffers(1, &iter->second);
            iter->second = ConstructSlot(gc);
        }
        glBindBuffer(GL_ARRAY_BUFFER, iter->second);
    }
    else
    {
        uint_t slot = ConstructSlot(gc);
        m_slots[gc] = slot;
    }

    const_cast<GeometryChunk*>(gc)->SetModified(false);
}

void SimpleGeometryCache::UnbindGeometry()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

uint_t SimpleGeometryCache::BufferOffset()
{
    return 0;
}

uint_t SimpleGeometryCache::ConstructSlot(const GeometryChunk* gc) const
{
    const uint_t vector3Size = sizeof(math::Vector3) * gc->VertexCount();
    const uint_t vector2Size = sizeof(math::Vector2) * gc->VertexCount();

    uint_t vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, vector3Size * 3 + vector2Size, 0,
                 GL_STATIC_DRAW);

    uint_t offset = 0;
    glBufferSubData(GL_ARRAY_BUFFER, offset, vector3Size, gc->Vertices());
    offset += vector3Size;

    glBufferSubData(GL_ARRAY_BUFFER, offset, vector3Size, gc->Normals());
    offset += vector3Size;

    glBufferSubData(GL_ARRAY_BUFFER, offset, vector3Size, gc->Tangents());
    offset += vector3Size;

    glBufferSubData(GL_ARRAY_BUFFER, offset, vector2Size,
                    gc->TextureCoordinates());

    return vbo;
}

}
}

}
