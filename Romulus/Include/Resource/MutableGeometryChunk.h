#ifndef _MUTABLEGEOMETRYCHUNK_H_
#define _MUTABLEGEOMETRYCHUNK_H_

//! \file MutableGeometryChunk.h
//! Contains MutableGeometryChunk class declaration.

#include "Render/GeometryChunk.h"
#include <map>
#include <set>
#include <vector>

namespace romulus
{

//! This class implements part of the GeometryChunk interface and exposes
//! methods for adding and removing triangles and vertices.
class MutableGeometryChunk : public render::GeometryChunk
{
public:

    MutableGeometryChunk();
    virtual ~MutableGeometryChunk() { }

    virtual const math::Vector3* Vertices() const;
    virtual const math::Vector3* Normals() const;
    virtual const math::Vector3* Tangents() const;
    virtual const math::Vector2* TextureCoordinates() const;
    virtual uint_t VertexCount() const;

    virtual uint_t IndexCount() const;
    virtual const ushort_t* Indices() const;

    //! Non-const vertex attribute interface.
    math::Vector3* Vertices();
    math::Vector3* Normals();
    math::Vector3* Tangents();
    math::Vector2* TextureCoordinates();

    template <typename VertexIterator>
    inline void SetVertices(VertexIterator begin, const VertexIterator end)
    {
        m_vertices.clear();
        m_vertices.insert(m_vertices.begin(), begin, end);
        m_vertexSet.clear();
        for (ushort_t i = 0; i < static_cast<ushort_t>(m_vertices.size()); ++i)
            m_vertexSet.insert(i);
        m_freeVertexHead = -1;
    }

    ushort_t AddVertex(const math::Vector3& v);
    ushort_t AddVertex(const math::Vector3& v, const math::Vector3& normal);
    void RemoveVertex(ushort_t vid);

    void AddFace(ushort_t v0, ushort_t v1, ushort_t v2);
    void RemoveFace(ushort_t v0, ushort_t v1, ushort_t v2);

    void Clear();

    inline const std::set<ushort_t>& VertexSet() const
    {
        return m_vertexSet;
    }

    inline const std::set<size_t>& TriangleIndexSet() const
    {
        return m_triangleIndexSet;
    }

    void ComputeVertexNormals();

private:

    std::vector<math::Vector3> m_vertices;
    std::vector<math::Vector3> m_normals;
    std::vector<math::Vector3> m_tangents;
    std::vector<ushort_t> m_indices;

    struct Triangle
    {
        //! \todo Put the smallest first, but maintain the vertex order.
        Triangle(ushort_t v0, ushort_t v1, ushort_t v2)
        {
            if (v1 < v0)
                std::swap(v0, v1);
            if (v2 < v0)
                std::swap(v0, v2);
            if (v2 < v1)
                std::swap(v1, v2);
            V0 = v0;
            V1 = v1;
            V2 = v2;
        }

        bool operator<(const Triangle& t) const
        {
            if (V0 == t.V0)
                if (V1 == t.V1)
                    return V2 < t.V2;
                else
                    return V1 < t.V1;
            else
                return V0 < t.V0;
        }

        bool operator==(const Triangle& t) const
        {
            return V0 == t.V0 && V1 == t.V1 && V2 == t.V2;
        }

        ushort_t V0, V1, V2;
    };

    ushort_t NextFreeVertex();
    void FreeVertex(ushort_t v);
    real_t m_freeVertexHead;
    std::set<ushort_t> m_vertexSet;

    size_t NextFreeTriangle();
    void FreeTriangle(size_t t);
    std::vector<size_t> m_freeTriangles;
    std::map<Triangle, size_t> m_triangleIndexMap;
    std::set<size_t> m_triangleIndexSet;
};

} // namespace romulus

#endif // _MUTABLEGEOMETRYCHUNK_H_
