#include "Math/Constants.h"
#include "Math/Vector.h"
#include "Math/Bounds/BoundingVolumes.h"
#include "Resource/SphereGeometryProvider.h"
#include "Render/SimpleGeometryChunk.h"
#include <boost/scoped_array.hpp>
#include <cmath>

namespace romulus
{

using math::Pi;
using math::Vector2;
using math::Vector3;

IMPLEMENT_RTTI(ProceduralSphereArguments);

void SphereGeometryProvider::UnloadResource(ResourceHandleBase::id_t id) {
    ArgumentsMap::iterator argsIt = m_arguments.find(id);
    if (argsIt != m_arguments.end())
    {
        SphereMap::iterator sphereIt = m_spheres.find(argsIt->second);
        ASSERT(sphereIt != m_spheres.end());
        m_arguments.erase(argsIt);
        m_spheres.erase(sphereIt);
    }
}

int SphereGeometryProvider::HandleType() const {
    return render::GeometryHandle::HandleType();
}

void* SphereGeometryProvider::GetResource(ResourceHandleBase::id_t id) {
    ArgumentsMap::const_iterator argsIt = m_arguments.find(id);
    if (argsIt != m_arguments.end())
    {
        SphereMap::const_iterator sphereIt = m_spheres.find(argsIt->second);
        ASSERT(sphereIt != m_spheres.end());
        return static_cast<void*>(
                static_cast<render::GeometryChunk*>(sphereIt->second.get()));
    }
    return 0;
}

bool SphereGeometryProvider::GenerateResource(
        const ProceduralResourceArguments& arguments,
        ResourceHandleBase::id_t& id) {
    if (IsExactType<ProceduralSphereArguments>(arguments))
    {
        const ProceduralSphereArguments& sphereArgs =
                static_cast<const ProceduralSphereArguments&>(arguments);
        if (sphereArgs.LongitudinalSegments < 3 ||
            sphereArgs.LatitudinalSegments < 2)
            return false;

        std::pair<uint_t, uint_t> argKey =
                std::make_pair(sphereArgs.LongitudinalSegments,
                               sphereArgs.LatitudinalSegments);
        IDMap::const_iterator idIt = m_ids.find(argKey);
        if (idIt == m_ids.end())
            id = CreateNewSphere(argKey);
        else
            id = idIt->second;
        return true;
    }
    return false;
}

ResourceHandleBase::id_t SphereGeometryProvider::CreateNewSphere(
        const std::pair<uint_t, uint_t>& lonLatSegments)
{
    const real_t radius = 1.f;

    boost::shared_ptr<render::SimpleGeometryChunk sphere(
            new render::SimpleGeometryChunk);
    ResourceHandleBase::id_t id = m_nextResourceID++;

    // There are (#latitudinal segments)-1 vertices for each longitudinal line,
    // plus the singular pole vertices.
    // There are (#longitudinal segments) longitudinal lines.

    // This constant does not include the pole vertices.
    const uint_t numVerticesPerLongitudeLine = lonLatSegments.second - 1;

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

    const real_t deltaU = 1.f / static_cast<real_t>(lonLatSegments.first);
    const real_t deltaV = 1.f / static_cast<real_t>(lonLatSegments.second);
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
         i < lonLatSegments.first + 1;
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

    m_arguments.insert(make_pair(id, lonLatSegments));
    m_ids.insert(make_pair(lonLatSegments, id));
    m_spheres.insert(make_pair(lonLatSegments, sphere));

    return id;
}

} // namespace romulus
