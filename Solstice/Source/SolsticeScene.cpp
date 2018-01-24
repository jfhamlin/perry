#include "Math/BezierCurve.h"
#include "Math/TorusKnot.h"
#include "Math/Transformations.h"
#include "SolsticeGUI.h"
#include "SolsticeScene.h"
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <vector>

using namespace romulus;
using namespace romulus::math;
using namespace romulus::render;

void SolsticeScene::PotentiallyVisibleGeometry(
        GeometryCollection& geometry,
        const romulus::math::Frustum& viewFrustum) const
{
    Geometry(geometry);
}
void SolsticeScene::Geometry(GeometryCollection& geometry) const
{
    geometry.insert(m_railGCIP.get());
    for (uint_t i = 0; i < m_strutGCIPs.size(); ++i)
        geometry.insert(m_strutGCIPs[i].get());
    if (m_params.RenderGround)
        geometry.insert(m_groundGCIP.get());
}

void SolsticeScene::PotentiallyRelevantLights(
        LightCollection& lights,
        const romulus::math::Frustum& viewFrustum) const
{
    Lights(lights);
}

void SolsticeScene::Lights(LightCollection& lights) const
{
    lights.insert(&m_keyLight);
    lights.insert(&m_fillLight0);
    lights.insert(&m_fillLight1);
}

void SolsticeScene::Initialize()
{
    // Create the universal and ground material.
    m_mat.reset(new Material);
    m_mat->SetColor(m_params.SculptureColor);
    m_mat->SetSpecularAlbedo(2.0);
    m_mat->SetSpecularExponent(10.0);

    m_groundMat.reset(new Material);
    m_groundMat->SetColor(Color(0.8, 0.9, 0.8, 1.0));
    m_groundMat->SetSpecularAlbedo(0.0);
    m_groundMat->SetSpecularExponent(50.0);

    // Allocate the rail sweep (once and for all).
    m_rail.reset(new Sweep);

    // Allocate the ground plane.
    m_ground.reset(new MutableGeometryChunk);

    // Construct the initial scene.
    ConstructSceneObjects();

    // Set up the rail and ground GCIPs once and for all.
    m_railGCIP.reset(new GeometryChunkInstance);
    m_railGCIP->SetGeometryChunk(m_rail->GeometryChunk());
    m_railGCIP->SetSurfaceDescription(m_mat);
    Matrix44 xform;
    SetIdentity(xform);
    m_railGCIP->SetTransform(xform);

    m_groundGCIP.reset(new GeometryChunkInstance);
    m_groundGCIP->SetGeometryChunk(m_ground);
    m_groundGCIP->SetSurfaceDescription(m_groundMat);
    m_groundGCIP->SetTransform(xform);

    // Set the light settings.
    m_keyLight.SetColor(Color(0.7, 0.7, 0.7, 1.0));
    m_keyLight.SetPosition(m_keyPos = Vector3(0, 0, 100));
    m_fillLight0.SetPosition(m_fill0Pos = Vector3(40, -70, 120));
    m_fillLight0.SetColor(0.3 * Color(0.5, 0.5, 0.8, 1.0));
    m_fillLight1.SetPosition(m_fill1Pos = Vector3(-40, 100, 140));
    m_fillLight1.SetColor(0.3 * Color(0.8, 0.5, 0.5, 1.0));
}

void SolsticeScene::Update()
{
    if (m_params != m_oldParams)
    {
        // If the P, Q parameters are now equal,
        // undo the change and don't update.
        if (m_params.PRev != m_oldParams.PRev)
        {
            if (m_params.PRev == m_params.QRev)
            {
                m_params = m_oldParams;
                return;
            }
        }
        else if (m_params.QRev != m_oldParams.QRev)
        {
            if (m_params.PRev == m_params.QRev)
            {
                m_params = m_oldParams;
                return;
            }
        }
        // If the StrutOffset parameter changed,
        // change our goal offset.
        if (m_params.StrutOffset != m_oldParams.StrutOffset)
        {
            m_goalOffset = m_params.StrutOffset;
        }
        // If it's just one of the colors that's changed, continue.
        if (m_params.BackgroundColor != m_oldParams.BackgroundColor)
        {
            m_oldParams = m_params;
            return;
        }
        if (m_params.SculptureColor != m_oldParams.SculptureColor)
        {
            m_mat->SetColor(m_params.SculptureColor);
            m_oldParams = m_params;
            return;
        }
        if (m_params.RenderGround != m_oldParams.RenderGround)
        {
            m_oldParams = m_params;
            return;
        }

        ConstructSceneObjects();
    }
}

uint_t Primes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31 };

class RecursiveTorusKnot : public romulus::math::Curve
{
public:

    virtual ~RecursiveTorusKnot() { }

    void AddLevel(uint_t numRevolutions, real_t radius)
    {
        Level l;
        l.Revolutions = numRevolutions;
        l.Radius = radius;
        if (m_levels.empty())
            l.O = XZ;
        else
            l.O = static_cast<Orientation>(-1 * m_levels.rbegin()->O);
        m_levels.push_back(l);
    }

    virtual Vector3 Begin() const { return Sample(0.0); }
    virtual Vector3 End() const { return Sample(1.0); }

    //! Sample the curve.
    virtual Vector3 Sample(real_t t) const
    {
        std::vector<Level>::const_reverse_iterator rit = m_levels.rbegin();

        // Get the point at the bottom level circle.
        Vector3 p(rit->Radius, 0, 0);
        real_t angle = 2.0 * Pi;
        angle *= t * static_cast<real_t>(rit->Revolutions);
        p[0] = rit->Radius * cos(angle);
        switch (rit->O)
        {
            case XY:
                p[1] = rit->Radius * sin(angle);
                break;
            case XZ:
                p[2] = rit->Radius * sin(angle);
                break;
        }

        // For each level, starting at the second lowest, transform the point
        //  to the position on the curve one level higher.
        for (++rit; rit != m_levels.rend(); ++rit)
        {
            // Translate it out to put the origin on (Radius, 0, 0).
            p += Vector3(rit->Radius, 0, 0);

            // Rotate the point around the circle to where we should be at.
            angle = 2.0 * Pi;
            angle *= t * static_cast<real_t>(rit->Revolutions);
            Rotation rot;
            switch (rit->O)
            {
                default:
                    ASSERT(false);
                case XY:
                    rot.SetAxisAngle(Vector3(0, 0, 1), angle);
                    break;
                case XZ:
                    rot.SetAxisAngle(Vector3(0, 1, 0), angle);
                    break;
            }
            p = TransformPoint(rot, p);
        }

        return p;
    }

private:

    enum Orientation
    {
        XY = -1,
        XZ = 1
    };

    struct Level
    {
        Orientation O;
        real_t Radius;
        uint_t Revolutions;
    };

    std::vector<Level> m_levels;
    uint_t m_depth;
};

inline void CircularRib(real_t angle, Polyline& p, uint_t samples)
{
    p.PushBack(Vector3(0, 0, 0));
    const real_t sinAngle = sin(angle);
    if (ApproxEqual(sinAngle, 0))
    {
        p.PushBack(Vector3(1, 0, 0));
        return;
    }
    const real_t oneOverSinAngle = 1.0 / sinAngle;
    const real_t delta = 1.0 / (samples + 1.0);
    for (real_t i = delta; i < 1.0; i += delta)
    {
        real_t phi = (1.0 - i) * angle;
        real_t mag = sin(i * angle) * oneOverSinAngle;
        Vector3 vec(cos(phi), sin(phi), 0);
        p.PushBack(mag * vec);
    }
    p.PushBack(Vector3(1, 0, 0));
}

struct lenCmp
{
    static real_t RailThickness;
    bool operator()(real_t l1, real_t l2) const
    {
        real_t diff = Abs(l2 - l1);
        if (diff < 0.5 * RailThickness)
            return 0;
        else if (l1 < l2)
            return -1;
        return 1;
    }
};
real_t lenCmp::RailThickness = 0;

void SolsticeScene::ConstructSceneObjects()
{
    math::Polyline path, cross;

    RecursiveTorusKnot rail;
    rail.AddLevel(m_params.PRev, m_params.PRad);
    rail.AddLevel(m_params.QRev, m_params.QRad);
    rail.AddLevel(m_params.RRev, m_params.RRad);
    rail.AddLevel(m_params.SRev, m_params.SRad);
    real_t rd = 1.0 / static_cast<real_t>(1200);
    for (float t = 0.f; t < .99999f; t += rd)
        path.PushBack(rail.Sample(t));

    // Make cross a circle in the x-y plane.
    for (real_t i = 0; i < 15.0; i += 1.0)
        cross.PushBack(m_params.RailThickness *
                       Vector3(-cos((i / 14.0) * 2.0 * Pi),
                               sin((i / 14.0) * 2.0 * Pi), 0));

    // Set up the rail sweep.
    m_rail->SetPath(path);
    m_rail->SetCrosssection(cross);
    m_rail->SetClosed(true);
    m_rail->ConstructSweep();

    // Floor height for ground plane.
    real_t floor = Infinity;
    Vector3 z = Normal(m_params.UpVector);
    Vector3 x(-z[2], z[0], -z[1]);
    x = Normal(Cross(z, x));
    Vector3 y(Normal(Cross(x, z)));
    for (uint_t i = 0; i < path.Size(); ++i)
        floor = Min(floor, Dot(path[i], z));

    // The strut crosssection.
    math::Polyline strutCross;
    real_t strutRadialSections = m_params.StrutRadiusSections;
    real_t den = strutRadialSections - 1.0;
    for (real_t i = 0; i < strutRadialSections; i += 1.0)
        strutCross.PushBack(m_params.StrutThickness *
                       Vector3(-cos((i / den) * 2.0 * Pi),
                               sin((i / den) * 2.0 * Pi), 0));
    uint_t strutsPerP = m_params.NumStruts / m_params.PRev;
    uint_t numStruts = strutsPerP * m_params.PRev;
    real_t delta = 1.0 / static_cast<real_t>(numStruts);
    real_t pRevDelta = 1.0 / static_cast<real_t>(m_params.PRev);

    // Create the strut sweeps.
    m_struts.clear();
    m_strutGCIPs.resize(numStruts);

    m_params.NumStruts = numStruts; // Snap parameter to the actual number.

    // Calculate the parameter offset for the strut destination points.
    real_t destinationParamOffset = (m_goalOffset / 180.0) *
            0.5 * pRevDelta;
    // Snap the offset to the nearest half rib delta multiple.
    const real_t halfDelta = 0.5 * delta;
    int halfDeltaMultiples = destinationParamOffset / halfDelta;
    destinationParamOffset =
            static_cast<real_t>(halfDeltaMultiples) * halfDelta;

    // Snap the offset parameter to the computed value.
    m_params.StrutOffset = (destinationParamOffset / (0.5 * pRevDelta)) * 180.0;
    // Change the step size on the control.
    real_t anglePerHalfDelta = 180. / (strutsPerP);
    m_gui->SetOffsetStep(anglePerHalfDelta);

    real_t interiorAngle = (m_params.PRev - 2) * Pi / m_params.PRev;
    // The angle for circular struts when they hug the torus.
    real_t baseAngle = 0.5 * (Pi - interiorAngle);
    // We construct the single rib curve, to be transformed for the
    // instances.
    math::Polyline basePath;
    real_t angle = baseAngle * m_params.StrutBend;
    CircularRib(-angle, basePath, m_params.StrutLengthSections);
    // A map from chord lengths to strut indices,
    // with a special comparator defined above.
    lenCmp::RailThickness = m_params.RailThickness;
    std::map<real_t, uint_t, lenCmp> lengthStrutMap;

    // Create the struts, including each path.
    for (uint_t i = 0; i < numStruts; ++i)
    {
        Vector3 start = rail.Sample(static_cast<real_t>(i) * delta);
        Vector3 end = rail.Sample(static_cast<real_t>(i) * delta +
                                  pRevDelta + destinationParamOffset);
        Vector3 mid = 0.5 * (start + end);
        Vector3 n_m = Cross(Normal(mid), Vector3(0, 1, 0));
        Vector3 d_r = Normal(end - start);
        Vector3 v = Normal(Cross(n_m, d_r));

        uint_t strutIndex = m_struts.size();
        real_t length = Magnitude(end - start);
        std::map<real_t, uint_t, lenCmp>::iterator it = lengthStrutMap.find(length);
        if (it == lengthStrutMap.end())
        {
            math::Polyline path;
            for (uint_t i = 0; i < basePath.Size(); ++i)
                path.PushBack(length * basePath[i]);
            m_struts.push_back(boost::shared_ptr<Sweep>(new Sweep));
            m_struts[strutIndex]->SetPath(path);
            m_struts[strutIndex]->SetCrosssection(strutCross);
            m_struts[strutIndex]->ConstructSweep();
            lengthStrutMap.insert(std::make_pair(length, strutIndex));
        }
        else
        {
            strutIndex = it->second;
        }

        if (!m_strutGCIPs[i].get())
        {
            m_strutGCIPs[i].reset(new GeometryChunkInstance);
            m_strutGCIPs[i]->SetSurfaceDescription(m_mat);
        }
        m_strutGCIPs[i]->SetGeometryChunk(
            m_struts[strutIndex]->GeometryChunk());
        Matrix44 xform;
        SetIdentity(xform);
        Vector3 zAxis = Cross(d_r, v);
        xform[0][0] = d_r[0]; xform[0][1] = v[0]; xform[0][2] = zAxis[0];
        xform[1][0] = d_r[1]; xform[1][1] = v[1]; xform[1][2] = zAxis[1];
        xform[2][0] = d_r[2]; xform[2][1] = v[2]; xform[2][2] = zAxis[2];
        xform[0][3] = start[0];
        xform[1][3] = start[1];
        xform[2][3] = start[2];
        m_strutGCIPs[i]->SetTransform(xform);

        // Update floor from strut path.
        for (uint_t i = 0; i < basePath.Size(); ++i)
        {
            Vector3 point = length * basePath[i];
            Vector4 p4(point, 1.0);
            p4 = xform * p4;
            point[0] = p4[0]; point[1] = p4[1]; point[2] = p4[2];
            floor = Min(floor, Dot(point, z));
        }
    }

    // Set up the ground plane.
    MutableGeometryChunk* mgc = m_ground.get();
    mgc->Clear();

    floor -= m_params.RailThickness;

    ushort_t v0 = mgc->AddVertex(Vector3(-floor * 100 * x +
                                         -floor * 100 * y +
                                         floor * z));
    ushort_t v1 = mgc->AddVertex(Vector3(floor * 100 * x +
                                         -floor * 100 * y +
                                         floor * z));
    ushort_t v2 = mgc->AddVertex(Vector3(floor * 100 * x +
                                         floor * 100 * y +
                                         floor * z));
    ushort_t v3 = mgc->AddVertex(Vector3(-floor * 100 * x +
                                         floor * 100 * y +
                                         floor * z));
    mgc->AddFace(v0, v1, v2);
    mgc->AddFace(v2, v3, v0);
    mgc->ComputeBoundingVolume();
    mgc->ComputeVertexNormals();

    Matrix33 lightFrame;
    lightFrame[0][0] = x[0];
    lightFrame[1][0] = x[1];
    lightFrame[2][0] = x[2];

    lightFrame[0][1] = y[0];
    lightFrame[1][1] = y[1];
    lightFrame[2][1] = y[2];

    lightFrame[0][2] = z[0];
    lightFrame[1][2] = z[1];
    lightFrame[2][2] = z[2];

    // Update light positions.
    m_keyLight.SetPosition(lightFrame * m_keyPos);
    m_fillLight0.SetPosition(lightFrame * m_fill0Pos);
    m_fillLight1.SetPosition(lightFrame * m_fill1Pos);

    m_oldParams = m_params;
}
