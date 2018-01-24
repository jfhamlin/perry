#include "Render/SimpleGeometryChunk.h"
#include "Math/Utilities.h"

namespace romulus
{
namespace render
{

uint_t SimpleGeometryChunk::AddVertex(const math::Vector3& vertex,
                                      const math::Vector3& normal,
                                      const math::Vector2& textureCoordinate) {
    uint_t index = VertexCount();
    m_vertices.push_back(vertex);
    m_normals.push_back(normal);
    m_textureCoordinates.push_back(textureCoordinate);
    return index;
}

void SimpleGeometryChunk::AddFace(ushort_t index0, ushort_t index1,
                                  ushort_t index2) {
    ASSERT(index0 < VertexCount());
    ASSERT(index1 < VertexCount());
    ASSERT(index2 < VertexCount());

    m_indices.push_back(index0);
    m_indices.push_back(index1);
    m_indices.push_back(index2);
}

void SimpleGeometryChunk::AddFace(ushort_t index0, ushort_t index1,
                                  ushort_t index2, ushort_t index3) {
    AddFace(index0, index1, index2);
    AddFace(index2, index3, index0);
}

void SimpleGeometryChunk::ComputeTangents()
{
    m_tangents.resize(VertexCount());

    math::CalculateTangentVectors(Vertices(), Normals(), TextureCoordinates(),
                                  VertexCount(), Indices(), IndexCount(),
                                  &m_tangents[0]);
}

} // namespace render
} // namespace romulus
