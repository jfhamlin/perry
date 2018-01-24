#include "Resource/MutableGeometryChunk.h"

namespace romulus
{

MutableGeometryChunk::MutableGeometryChunk():
    m_vertices(1), m_indices(3), m_freeVertexHead(0)
{
    m_vertices[0][0] = -1; // Used as a node of a linked list of free verts.
    m_indices[0] = m_indices[1] = m_indices[2] = 0;
    m_freeTriangles.push_back(0);
}

const math::Vector3* MutableGeometryChunk::Vertices() const
{
    return &m_vertices[0];
}

const math::Vector3* MutableGeometryChunk::Normals() const
{
    if (m_normals.size() == m_vertices.size())
        return &m_normals[0];
    else
        return 0;
}

const math::Vector3* MutableGeometryChunk::Tangents() const
{
    return 0;
}

const math::Vector2* MutableGeometryChunk::TextureCoordinates() const
{
    return 0;
}

uint_t MutableGeometryChunk::VertexCount() const
{
    return m_vertices.size();
}

uint_t MutableGeometryChunk::IndexCount() const
{
    return m_indices.size();
}

const ushort_t* MutableGeometryChunk::Indices() const
{
    return &m_indices[0];
}

math::Vector3* MutableGeometryChunk::Vertices()
{
    return &m_vertices[0];
}

math::Vector3* MutableGeometryChunk::Normals()
{
    return 0;
}

math::Vector3* MutableGeometryChunk::Tangents()
{
    return 0;
}

math::Vector2* MutableGeometryChunk::TextureCoordinates()
{
    return 0;
}

ushort_t MutableGeometryChunk::AddVertex(const math::Vector3& v)
{
    ushort_t i = NextFreeVertex();
    m_vertices[i] = v;
    m_vertexSet.insert(i);
    return i;
}

ushort_t MutableGeometryChunk::AddVertex(const math::Vector3& v,
                                         const math::Vector3& normal)
{
    ushort_t index = AddVertex(v);
    if (m_normals.size() <= index)
        m_normals.resize(index + 1);
    m_normals[index] = normal;
    return index;
}

void MutableGeometryChunk::RemoveVertex(ushort_t v)
{
        FreeVertex(v);
        m_vertexSet.erase(v);
}

void MutableGeometryChunk::AddFace(ushort_t v0, ushort_t v1, ushort_t v2)
{
    if (m_triangleIndexMap.find(Triangle(v0, v1, v2)) ==
        m_triangleIndexMap.end())
    {
        size_t i = NextFreeTriangle();

        m_indices[i + 0] = v0;
        m_indices[i + 1] = v1;
        m_indices[i + 2] = v2;
        m_triangleIndexMap.insert(std::make_pair(Triangle(v0, v1, v2), i));

        m_triangleIndexSet.insert(i);
    }
}

void MutableGeometryChunk::RemoveFace(ushort_t v0, ushort_t v1, ushort_t v2)
{
    std::map<Triangle, size_t>::iterator it =
            m_triangleIndexMap.find(Triangle(v0, v1, v2));
    if (it != m_triangleIndexMap.end())
    {
        size_t i = it->second;
        // FreeTriangle makes the triangle entry degenerate which will cause
        // it to be discarded when rendering.
        FreeTriangle(i);
        m_triangleIndexMap.erase(it);

        m_triangleIndexSet.erase(i);
    }
}

void MutableGeometryChunk::Clear()
{
    m_vertices.resize(1); m_indices.resize(3); m_freeVertexHead = 0;
    m_vertices[0][0] = -1; // Used as a node of a linked list of free verts.
    m_indices[0] = m_indices[1] = m_indices[2] = 0;
    m_freeTriangles.clear();
    m_freeTriangles.push_back(0);

    m_vertexSet.clear();
    m_triangleIndexMap.clear();
    m_triangleIndexSet.clear();
}

void MutableGeometryChunk::ComputeVertexNormals()
{
    using namespace math;

    m_normals.resize(m_vertices.size());
    memset(&m_normals[0][0], 0, m_normals.size() * 3 * sizeof(real_t));
    for (uint_t i = 0; i < m_indices.size(); i += 3)
    {
        Vector3 v0 = m_vertices[m_indices[i + 1]] - m_vertices[m_indices[i]];
        Vector3 v1 = m_vertices[m_indices[i + 2]] - m_vertices[m_indices[i]];
        Vector3 faceNormal(Cross(v0, v1));
        Normalize(faceNormal);
        for (int j = 0; j < 3; ++j)
            m_normals[m_indices[i + j]] += faceNormal;
    }
    for (std::vector<math::Vector3>::iterator it = m_normals.begin();
         it != m_normals.end(); ++it)
    {
        Normalize(*it);
    }

    SetModified(true);
}

ushort_t MutableGeometryChunk::NextFreeVertex()
{
    int v;
    if (m_freeVertexHead != -1)
    {
        v = m_freeVertexHead;
        m_freeVertexHead =
                m_vertices[static_cast<int>(m_freeVertexHead)][0];
    }
    else
    {
        v = m_vertices.size();
        ASSERT(v > 0);
        m_vertices.resize(v + 1);
    }
    ASSERT(v < std::numeric_limits<ushort_t>().max());
    return static_cast<ushort_t>(v);
}

void MutableGeometryChunk::FreeVertex(ushort_t v)
{
    m_vertices[v][0] = m_freeVertexHead;
    m_freeVertexHead = v;
}

size_t MutableGeometryChunk::NextFreeTriangle()
{
    size_t i;
    if (m_freeTriangles.begin() != m_freeTriangles.end())
    {
        i = m_freeTriangles.back();
        m_freeTriangles.pop_back();
    }
    else
    {
        i = m_indices.size();
        m_indices.resize(i + 3);
        m_indices[i] = m_indices[i + 1] = m_indices[i + 2] = 0;
    }
    return i;
}

void MutableGeometryChunk::FreeTriangle(size_t t)
{
    m_freeTriangles.push_back(t);
    m_indices[t] =
            m_indices[t + 1] =
            m_indices[t + 2] = 0;
}

} // namespace romulus
