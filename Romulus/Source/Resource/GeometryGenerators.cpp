#include "Math/Bounds/BoundingVolumes.h"
#include "Math/Vector.h"
#include "Render/SimpleGeometryChunk.h"
#include "Resource/GeometryGenerators.h"
#include <boost/scoped_array.hpp>

namespace romulus
{
boost::shared_ptr<render::GeometryChunk> CreateNewSphere(
        uint_t lonSegments, uint_t latSegments)
{
    using namespace math;

    const real_t radius = 1.f;

    boost::shared_ptr<render::SimpleGeometryChunk> sphere(
            new render::SimpleGeometryChunk);

    // There are (#latitudinal segments)-1 vertices for each longitudinal line,
    // plus the singular pole vertices.
    // There are (#longitudinal segments) longitudinal lines.

    // This constant does not include the pole vertices.
    const uint_t numVerticesPerLongitudeLine = latSegments - 1;

    // Make "North" pole, which has index 0.
    sphere->AddVertex(radius * Vector3(0, 1, 0), Vector3(0, 1, 0),
                      Vector2(.5f, 1));
    const uint_t northIndex = 0;
    // Make "South" pole, which has index 1.
    sphere->AddVertex(radius * Vector3(0, -1, 0), Vector3(0, -1, 0),
                      Vector2(.5f, 0));
    const uint_t southIndex = 1;

    // We call longitude theta, ranging from 0 to 2*pi radians. We call
    // latitude phi, ranging from -pi/2 to pi/2 radians.
    // x = cos(theta) * cos(phi)
    // y = sin(phi)
    // z = - sin(theta) * cos(phi)
    // Since we only create unit spheres, the normal is equal to the vertex.
    // For texture coordinates, U increases with theta, V with phi.

    const real_t deltaU = 1.f / static_cast<real_t>(lonSegments);
    const real_t deltaV = 1.f / static_cast<real_t>(latSegments);
    real_t u = 0.f;
    real_t v = deltaV;

    const real_t deltaTheta = 2.f * Pi * deltaU;
    const real_t deltaPhi = Pi * deltaV;
    real_t theta = 0.f;
    real_t phi = - .5f * Pi + deltaPhi; // Omit the south pole vertex.

    uint_t i, j;
    // Create the first longitudinal row of vertices, where theta=0.
    for (i = 0; i < numVerticesPerLongitudeLine;
         ++i, phi += deltaPhi, v += deltaV)
    {
        const real_t cp = cos(phi);
        const real_t sp = sin(phi);
        Vector3 vertex(cp, sp, 0);
        sphere->AddVertex(radius * vertex, vertex, Vector2(u, v));
    }

    for (i = 1, u = deltaU, theta = deltaTheta;
         i < lonSegments + 1;
         ++i, u += deltaU, theta += deltaTheta)
    {
        const real_t ct = cos(theta);
        const real_t st = sin(theta);
        for (j = 0, phi = - .5f * Pi + deltaPhi, v = deltaV;
             j <  numVerticesPerLongitudeLine;
             ++j, phi += deltaPhi, v += deltaV)
        {
            const real_t cp = cos(phi);
            const real_t sp = sin(phi);
            Vector3 vertex(ct * cp, sp, - st * cp);
            sphere->AddVertex(radius * vertex, vertex, Vector2(u, v));
        }

        const uint_t firstCurrentLonIndex = 2 + i * numVerticesPerLongitudeLine;
        const uint_t firstLastLonIndex =
                firstCurrentLonIndex - numVerticesPerLongitudeLine;
        // Fill in the faces between these vertices and the last set.
        // Start with the tri incident on the south pole.
        sphere->AddFace(southIndex, firstCurrentLonIndex, firstLastLonIndex);
        // Fill in the quads.
        for (j = 1; j < numVerticesPerLongitudeLine; ++j)
            sphere->AddFace(firstCurrentLonIndex + j,
                            firstLastLonIndex + j,
                            firstLastLonIndex + j - 1,
                            firstCurrentLonIndex + j - 1);
        // End with the tri incident on the north pole.
        sphere->AddFace(northIndex,
                        firstLastLonIndex + numVerticesPerLongitudeLine - 1,
                        firstCurrentLonIndex + numVerticesPerLongitudeLine - 1);
    }

    sphere->ComputeTangents();
    sphere->SetBoundingVolume(math::BoundingSphere(Vector3(0.f), radius));

    return sphere;
}
} // namespace romulus
