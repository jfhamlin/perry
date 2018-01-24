#ifndef _RENDERSIMPLEGEOMETRYCHUNK_H_
#define _RENDERSIMPLEGEOMETRYCHUNK_H_

//! \file SimpleGeometryChunk.h
//! Contains declaration of SimpleGeometryChunk class, a straightforward
//! implementation of the GeometryChunk abstract class.

#include "Math/Vector.h"
#include "Render/GeometryChunk.h"
#include <vector>

namespace romulus
{
namespace render
{

class SimpleGeometryChunk : public GeometryChunk
{
public:

    SimpleGeometryChunk() { }
    virtual ~SimpleGeometryChunk() { }

    //! Adds a vertex to the chunk.
    //! \param vertex - the position of the vertex.
    //! \param normal - the vertex normal.
    //! \param textureCoordinate - the (u, v) texture coordinates of the vertex.
    //! \return The index of the added vertex, which is guaranteed to be one
    //!         greater than the previously added vertex, or zero if it is the
    //!         first vertex.
    uint_t AddVertex(const math::Vector3& vertex, const math::Vector3& normal,
                     const math::Vector2& textureCoordinate);

    //! Add a triangle to the chunk, given indices to three vertices. The front
    //! of the face is the side on which [index0, index1, index2] is in counter-
    //! clockwise order.
    void AddFace(ushort_t index0, ushort_t index1, ushort_t index2);

    //! Add a quad to the chunk, given indices to four vertices. The front
    //! of the face is the side on which [index0, index1, index2, index3] is in
    //! counter-clockwise order. This method is sugaring for creating two tris.
    void AddFace(ushort_t index0, ushort_t index1,
                 ushort_t index2, ushort_t index3);

    //! Calculate the tangent vectors.
    //! Presupposes that vertices, texture coordinates, normals, and faces
    //! have been added.
    void ComputeTangents();

    virtual const math::Vector3* Vertices() const { return &m_vertices[0]; }
    virtual const math::Vector3* Normals() const { return &m_normals[0]; }
    virtual const math::Vector3* Tangents() const
    {
        ASSERT(m_tangents.size());
        return &m_tangents[0];
    }
    virtual const math::Vector2* TextureCoordinates() const
    { return &m_textureCoordinates[0]; }
    virtual uint_t VertexCount() const { return m_vertices.size(); }

    virtual uint_t IndexCount() const { return m_indices.size(); }
    virtual const ushort_t* Indices() const { return &m_indices[0]; }

private:

    std::vector<math::Vector3> m_vertices;
    std::vector<math::Vector3> m_normals;
    std::vector<math::Vector3> m_tangents;
    std::vector<math::Vector2> m_textureCoordinates;
    std::vector<ushort_t> m_indices;
};

} // namespace render
} // namespace romulus

#endif // _RENDERSIMPLEGEOMETRYCHUNK_H_
