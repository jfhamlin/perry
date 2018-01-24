#include "Math/Transformations.h"
#include "Resource/MutableGeometryChunk.h"
#include "Resource/Sweep.h"
#include <cmath>

namespace romulus
{

using namespace math;

Sweep::Sweep():
    m_isClosed(false), m_minimizeTorsion(true), m_globalTwist(0),
    m_globalAzimuth(0)
{
    m_mgc.reset(new MutableGeometryChunk);
}

void Sweep::ConstructSweep()
{
    DestroySweep();

    // Remove duplicate consecutive points.
    Polyline newPath;
    newPath.PushBack(m_path[0]);
    for (uint_t i = 1; i < m_path.Size() - 1; ++i)
        if (m_path[i] != newPath[i - 1])
            newPath.PushBack(m_path[i]);

    // Only add the last vertex if it's not the same as the previously added
    // vertex AND if either the sweep isn't closed or if it does not repeat
    // the initial vertex.
    if ((m_path[m_path.Size() - 1] != newPath[m_path.Size() - 2]) &&
        (!m_isClosed || m_path[0] != m_path[m_path.Size() - 1]))
        newPath.PushBack(m_path[m_path.Size() - 1]);

    if (newPath.Size() == 1)
        newPath.PushBack(m_path[m_path.Size() - 1]);
    m_path = newPath;

    CalculateFrenetFrames();
    if (m_minimizeTorsion)
        MinimizeTorsion();

    m_crossIsClosed = m_cross[0] == m_cross[m_cross.Size() - 1];

    ASSERT(m_cross.Size() > 1);
    // If the crosssection is closed, we use the same vertex for the first
    // and last in every slice.
    // \todo - This will have to change when texture coordinates are
    //         implemented.
    uint_t numCrossVertsToUse = m_crossIsClosed ?
        m_cross.Size() - 1 : m_cross.Size();

    // For a closed path, we want numRows to be one greater than m_path.Size().
    uint_t numRows = m_path.Size() + (m_isClosed ? 1 : 0);

    // Create crosssection vertices at each node of the path.
    m_meshVertices.reset(new boost::scoped_array<ushort_t>[numRows]);
    for (uint_t row = 0; row < numRows; ++row)
    {
        math::Polyline transformedCross = TransformCrosssection(row);

        m_meshVertices[row].reset(new ushort_t[numCrossVertsToUse]);
        for (uint_t v = 0; v < transformedCross.Size(); ++v)
            m_meshVertices[row][v] = m_mgc->AddVertex(transformedCross[v]);
    }

    // Create triangles connecting crosssection vertices.
    for (uint_t row = 1; row < numRows; ++row)
    {
        uint_t curRow = row;
        uint_t prevRow = row - 1;
        for (uint_t vert = 1; vert < numCrossVertsToUse; ++vert)
        {
            m_mgc->AddFace(m_meshVertices[curRow][vert],
                           m_meshVertices[curRow][vert - 1],
                           m_meshVertices[prevRow][vert - 1]);
            m_mgc->AddFace(m_meshVertices[curRow][vert],
                           m_meshVertices[prevRow][vert - 1],
                           m_meshVertices[prevRow][vert]);
        }

        if (m_crossIsClosed)
        {
            uint_t prevVert = numCrossVertsToUse - 1;
            m_mgc->AddFace(m_meshVertices[curRow][0],
                           m_meshVertices[curRow][prevVert],
                           m_meshVertices[prevRow][prevVert]);
            m_mgc->AddFace(m_meshVertices[curRow][0],
                           m_meshVertices[prevRow][prevVert],
                           m_meshVertices[prevRow][0]);
        }
    }

    // Create temporary, bad end caps to close sweeps for STL.
    // \todo Good, optional end caps.
    if (!m_isClosed)
    {
        int row = 0;
        boost::scoped_array<ushort_t> verts(new ushort_t[numCrossVertsToUse]);
        {
            Vector3 vertex(0, 0, 0);
            for (uint_t i = 0; i < numCrossVertsToUse; ++i)
                vertex += m_mgc->Vertices()[m_meshVertices[row][i]];
            vertex *= 1.0 / numCrossVertsToUse;
            ushort_t center = m_mgc->AddVertex(vertex);

            for (uint_t i = 0; i < numCrossVertsToUse; ++i)
                verts[i] = m_mgc->AddVertex(
                        m_mgc->Vertices()[m_meshVertices[row][i]]);

            for (uint_t i = 1; i < numCrossVertsToUse; ++i)
                m_mgc->AddFace(center, verts[i], verts[i - 1]);
            if (m_crossIsClosed)
                m_mgc->AddFace(center, verts[0], verts[numCrossVertsToUse - 1]);
        }

        row = numRows - 1;
        {
            Vector3 vertex(0, 0, 0);
            for (uint_t i = 0; i < numCrossVertsToUse; ++i)
                vertex += m_mgc->Vertices()[m_meshVertices[row][i]];
            vertex *= 1.0 / numCrossVertsToUse;
            ushort_t center = m_mgc->AddVertex(vertex);

            for (uint_t i = 0; i < numCrossVertsToUse; ++i)
                verts[i] = m_mgc->AddVertex(
                        m_mgc->Vertices()[m_meshVertices[row][i]]);

            for (uint_t i = 1; i < numCrossVertsToUse; ++i)
                m_mgc->AddFace(center, verts[i - 1], verts[i]);
            if (m_crossIsClosed)
                m_mgc->AddFace(center, verts[numCrossVertsToUse - 1], verts[0]);
        }
    }

    // Set the vertex normals.
    m_mgc->ComputeVertexNormals();
    m_mgc->ComputeBoundingVolume();

    m_mgc->SetModified(true);
}


Matrix33 Sweep::CalculateFrenetFrame(
        romulus::math::Vector3 p0, romulus::math::Vector3 p1,
        romulus::math::Vector3 p2, int prevIndex)
{
    Vector3 t, n, b;

    b = Cross(p2 - p1, p0 - p1);

    for (uint_t i = 0; i < 3; ++i)
        ASSERT(!IsNaN(b[i]));

    // Handle collinear points.
    if (ApproxEqual(MagnitudeSquared(b), static_cast<real_t>(0)))
    {
        // Tangent
        t = p2 - p1;

        // Check for a zero tangent, i.e. a repeated point.
        if (ApproxEqual(MagnitudeSquared(t), static_cast<real_t>(0)))
        {
            if (prevIndex > 1)
            {
                t[0] = m_frenetFrames[prevIndex][0][2];
                t[1] = m_frenetFrames[prevIndex][1][2];
                t[2] = m_frenetFrames[prevIndex][2][2];
            }
            else
            {
                // Use an arbitrary direction.
                t = Vector3(1, 0, 0);
            }
        }

        Normalize(t);

        // Binormal
        Vector3 v(0, 1, 0);
        if (ApproxEqual(abs(Dot(v, t)), static_cast<real_t>(1)))
            v = Vector3(1, 0, 0);

        b = v - Dot(v, t) * t;
        Normalize(b);
        for (uint_t i = 0; i < 3; ++i)
            ASSERT(!IsNaN(b[i]));

        // If t == (0 1 0), try (1 0 0).
        // Not the most elegant solution.
        if (ApproxEqual(MagnitudeSquared(b), static_cast<real_t>(0)))
        {
            Vector3 v(1, 0, 0);
            b = v - Dot(v, t) * t;
            Normalize(b);
        }

        // Normal
        n = Cross(b, t);
        Normalize(n);
    }
    else
    {
        // Binormal
        Normalize(b);
        for (uint_t i = 0; i < 3; ++i)
            ASSERT(!IsNaN(b[i]));

        // Normal
        Vector3 v(p0 - p1);
        Normalize(v);
        n = p2 - p1;
        Normalize(n);
        n = 0.5 * (v + n);
        Normalize(n);

        // Tangent
        t = Cross(n, b);
        Normalize(t);
    }

    Matrix33 m;
    m[0][0] = b[0];
    m[1][0] = b[1];
    m[2][0] = b[2];

    m[0][1] = n[0];
    m[1][1] = n[1];
    m[2][1] = n[2];

    m[0][2] = t[0];
    m[1][2] = t[1];
    m[2][2] = t[2];

    return m;
}

void Sweep::CalculateFrenetFrames()
{
    m_frenetFrames.resize(m_path.Size());

    // Two-point path.
    if (m_path.Size() == 2)
    {
        m_frenetFrames[0] =
                CalculateFrenetFrame(m_path[0], m_path[0], m_path[1], -1);
        m_frenetFrames[1] = m_frenetFrames[0];
        return;
    }

    // All frames but the first and the last.
    for (uint_t i = 1; i < m_path.Size() - 1; ++i)
    {
        // The last argument gives the index of the previous frame/path node,
        // whose tangent is used if for some reason the calculated one is
        // degenerate. This scheme breaks down if the first tangent has this
        // problem. We'll just let it break for now.
        // \todo - properly handle this.
        m_frenetFrames[i] = CalculateFrenetFrame(m_path[i - 1], m_path[i],
                                                 m_path[i + 1], i - 1);
    }

    // If closed, calculate the first and last frenet frames using
    // circular connectivity. Otherwise, calculate them independently.
    const uint_t last = m_path.Size() - 1;
    if (m_isClosed)
    {
        m_frenetFrames[0] =
                CalculateFrenetFrame(m_path[last], m_path[0],
                                     m_path[1], last);
        m_frenetFrames[last] =
                CalculateFrenetFrame(m_path[last - 1], m_path[last],
                                     m_path[0], last - 1);
    }
    else
    {
        m_frenetFrames[0] =
                CalculateFrenetFrame(m_path[0] - (m_path[1] - m_path[0]),
                                     m_path[0], m_path[1], 1);
        m_frenetFrames[last] =
                CalculateFrenetFrame(
                        m_path[last - 1], m_path[last],
                        m_path[last] + (m_path[last] - m_path[last - 1]),
                        last - 1);
    }
}

void Sweep::MinimizeTorsion()
{
    for (uint_t i = 0; i < m_path.Size() - 1; ++i)
    {
        Vector3 p1 = m_path[i];
        Vector3 p2 = m_path[i + 1];
        if (p1 != p2)
        {
            Vector3 vT2, vN1, vN2;
            // The tangent to the path at p2.
            vT2[0] = m_frenetFrames[i + 1][0][2];
            vT2[1] = m_frenetFrames[i + 1][1][2];
            vT2[2] = m_frenetFrames[i + 1][2][2];

            // The normal to the path at p1.
            vN1[0] = m_frenetFrames[i][0][1];
            vN1[1] = m_frenetFrames[i][1][1];
            vN1[2] = m_frenetFrames[i][2][1];

            // The normal to the path at p2.
            vN2[0] = m_frenetFrames[i + 1][0][1];
            vN2[1] = m_frenetFrames[i + 1][1][1];
            vN2[2] = m_frenetFrames[i + 1][2][1];

            // The normalized vector from p1 to p2.
            Vector3 v = p2 - p1;
            Normalize(vN1);
            Vector3 c1 = p1 + vN1;

            // Coefficients of the plane equation for the plane with normal
            // in the direction of the tangent at p2 and passing through p2.
            Vector4 vPlane(vT2[0], vT2[1], vT2[2], 0);
            Normalize(vPlane);
            vPlane[3] = -Dot(vPlane, Vector4(p2[0], p2[1], p2[2], 1.0));

            real_t t = -((vPlane[0] * c1[0] + vPlane[1] * c1[1] +
                          vPlane[2] * c1[2] + vPlane[3]) /
                         (vPlane[0] * v[0] + vPlane[1] * v[1] +
                          vPlane[2] * v[2]));
            ASSERT(!IsNaN(t));

            Vector3 c2 = c1 + t * v;
            Vector3 v2 = c2 - p2;

            real_t dot = Dot(Normal(vN2), Normal(v2));
            real_t dTheta = ACosClamp(dot);
            v = Cross(vN2, v2);
            if (Dot(v, vT2) < 0.0)
                dTheta *= -1.0;
            Rotation rot(vT2, dTheta);
            Matrix44 adjust0 = rot.Matrix();
            Matrix33 adjust = Submatrix<3, 3, 0, 0>(adjust0);
            Matrix33 newFrame = adjust * m_frenetFrames[i + 1];

            m_frenetFrames[i + 1] = newFrame;
        }
    }
    // Minimize torsion for closed curves.
    if (m_isClosed)
    {
        // Find the angle difference between the first and last normals
        // (projected onto the plane tangent to the line between the
        //  points). Distribute this change across the sweep.
        Vector3 vN1, vN2;
        vN1[0] = m_frenetFrames[0][0][1];
        vN1[1] = m_frenetFrames[0][1][1];
        vN1[2] = m_frenetFrames[0][2][1];
        uint_t last = m_path.Size() - 1;
        vN2[0] = m_frenetFrames[last][0][1];
        vN2[1] = m_frenetFrames[last][1][1];
        vN2[2] = m_frenetFrames[last][2][1];
        Vector3 line = m_path[last] - m_path[0];
        real_t mag = Magnitude(line);
        if (ApproxEqual(mag, real_t(0)))
            return;
        line *= 1.0 / mag;
        // The desired angle is preserved when we take the cross
        // product with the line vector.
        vN1 = Cross(line, Normal(vN1));
        vN2 = Cross(line, Normal(vN2));

        real_t dot = Dot(Normal(vN1), Normal(vN2));
        real_t dTheta = ACosClamp(dot);
        dTheta = dTheta / static_cast<real_t>(m_path.Size() + 1);
        for (uint_t i = 0; i < m_path.Size() - 1; ++i)
        {
            Vector3 tangent;
            tangent[0] = m_frenetFrames[i + 1][0][2];
            tangent[1] = m_frenetFrames[i + 1][1][2];;
            tangent[2] = m_frenetFrames[i + 1][2][2];;
            Rotation rot(tangent, static_cast<real_t>(i) * dTheta);
            Matrix44 adjust0 = rot.Matrix();
            Matrix33 adjust = Submatrix<3, 3, 0, 0>(adjust0);
            Matrix33 newFrame = adjust * m_frenetFrames[i + 1];

            m_frenetFrames[i + 1] = newFrame;
        }
    }
}

math::Polyline Sweep::TransformCrosssection(uint_t index)
{
    index = index % m_path.Size();
    ASSERT(index < m_frenetFrames.size());

    Polyline result;
    uint_t numVerts = m_crossIsClosed ?
        m_cross.Size() - 1: m_cross.Size();

    const real_t sliceTwist = m_sliceAzimuths[index] + m_globalAzimuth +
            m_globalTwist *
            static_cast<real_t>(index) / static_cast<real_t>(m_path.Size());
    const Rotation twist(Vector3(0, 0, 1), sliceTwist);

    if (!m_isClosed && (index == 0 || index == m_path.Size() - 1))
    {
        for (uint_t i = 0; i < numVerts; ++i)
        {
            Vector3 orientedPoint =
                    m_frenetFrames[index] * TransformPoint(twist, m_cross[i]);
            Vector3 p = orientedPoint + m_path[index];
            result.PushBack(p);
        }
    }
    else
    {
        // A scale factor to keep the crosssection correct regardless
        // of the angle at a corner.
        ASSERT(m_path.Size() > 1);
        uint_t i0, i1, i2;
        i1 = index;
        if (m_isClosed)
        {
            if (i1 == 0)
            {
                i0 = m_path.Size() - 1;
            }
            else
            {
                i0 = i1 - 1;
            }
            if (i1 == m_path.Size() - 1)
            {
                i2 = 0;
            }
            else
            {
                i2 = i1 + 1;
            }
        }
        else
        {
            i0 = i1 - 1;
            i2 = i1 + 1;
        }
        const Vector3& p0 = m_path[i0];
        const Vector3& p1 = m_path[i1];
        const Vector3& p2 = m_path[i2];

        const Vector3 leg0 = Normal(p0 - p1);
        const Vector3 leg1 = Normal(p2 - p1);

        Vector3 bisector = leg0 + leg1;
        // If the legs are in opposing directions, just choose any
        // perpendicular vector as bisector.
        if (ApproxEqual(Magnitude(bisector), 0))
            bisector = Cross(leg0, Vector3(leg0[1], leg0[2], leg0[0]));
        Normalize(bisector);
        real_t angle = ACosClamp(Dot(leg0, leg1));
        real_t scalar = 1.0 / Max(cos((Pi / 2.0) - 0.5 * angle), 0.1);
        scalar -= 1.0;
        for (uint_t i = 0; i < numVerts; ++i)
        {
            Vector3 orientedPoint =
                    m_frenetFrames[index] * TransformPoint(twist, m_cross[i]);
            Vector3 scaledPoint = orientedPoint +
                    scalar * (Dot(orientedPoint, bisector) * bisector);
            Vector3 p = scaledPoint + m_path[index];
            result.PushBack(p);
        }
    }
    return result;
}

void Sweep::DestroySweep()
{
    m_mgc->Clear();
}

} // namespace romulus
