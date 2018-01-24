#ifndef _SOLSTICESCENE_H_
#define _SOLSTICESCENE_H_

#include "Render/GeometryChunkInstance.h"
#include "Render/IScene.h"
#include "Render/Material.h"
#include "Render/PointLight.h"
#include "Resource/Sweep.h"
#include <boost/shared_ptr.hpp>

class SolsticeGUI;

struct SceneParameters
{
    SceneParameters(): PRev(3), QRev(2), RRev(0), SRev(0),
                       PRad(6.5), QRad(6.0), RRad(0.0), SRad(0.0),
                       NumStruts(300), RailThickness(0.15),
                       StrutThickness(0.1), RailSections(500),
                       StrutOffset(1.8), StrutBend(0.5),
                       BackgroundColor(.7, .85, 1, 1),
                       SculptureColor(0.8, 0.8, 0.2, 1),
                       StrutLengthSections(15), StrutRadiusSections(8),
                       UpVector(0, 0, 1), RenderGround(false) { }
    uint_t PRev;
    uint_t QRev;
    uint_t RRev;
    uint_t SRev;
    real_t PRad;
    real_t QRad;
    real_t RRad;
    real_t SRad;
    uint_t NumStruts;
    real_t RailThickness;
    real_t StrutThickness;
    uint_t RailSections;
    real_t StrutOffset;
    real_t StrutBend;
    romulus::render::Color BackgroundColor;
    romulus::render::Color SculptureColor;
    uint_t StrutLengthSections;
    uint_t StrutRadiusSections;
    romulus::math::Vector3 UpVector;
    int RenderGround;

    bool operator==(const SceneParameters& p) const
    {
        return PRev == p.PRev && QRev == p.QRev &&
                RRev == p.RRev && SRev == p.SRev &&
                PRad == p.PRad && QRad == p.QRad &&
                RRad == p.RRad && SRad == p.SRad &&
                NumStruts == p.NumStruts &&
                RailThickness == p.RailThickness &&
                StrutThickness == p.StrutThickness &&
                RailSections == p.RailSections &&
                StrutOffset == p.StrutOffset &&
                StrutBend == p.StrutBend &&
                BackgroundColor == p.BackgroundColor &&
                SculptureColor == p.SculptureColor &&
                StrutLengthSections == p.StrutLengthSections &&
                StrutRadiusSections == p.StrutRadiusSections &&
                UpVector == p.UpVector &&
                RenderGround == p.RenderGround;
    }

    bool operator!=(const SceneParameters& p) const
    {
        return !(*this == p);
    }
};

class SolsticeScene : public romulus::render::IScene
{
public:

    SolsticeScene(SolsticeGUI* gui, SceneParameters& params):
        m_params(params), m_goalOffset(params.StrutOffset), m_gui(gui) { }
    virtual ~SolsticeScene() { }

    //! Collect potentially visible geometry.
    //! \param geometry - The output geometry. Any geometry already in the
    //!                   collection will remain in the collection.
    //! \param viewFrustum - The view frustum.
    virtual void PotentiallyVisibleGeometry(
            GeometryCollection& geometry,
            const romulus::math::Frustum& viewFrustum) const;
    virtual void PotentiallyRelevantLights(
            LightCollection& lights,
            const romulus::math::Frustum& viewFrustum) const;

    virtual void Geometry(GeometryCollection& geometry) const;
    virtual void Lights(LightCollection& lights) const;

    const romulus::render::Color& BackgroundColor() const
    { return m_params.BackgroundColor; }

    void Initialize();

    void Update();

    void OutputScene() const;

private:

    void OutputMGC(const romulus::MutableGeometryChunk& mgc,
                   const std::string& fileName) const;

    void ConstructSceneObjects();

    boost::shared_ptr<romulus::Sweep> m_rail;
    std::vector<boost::shared_ptr<romulus::Sweep> > m_struts;
    boost::shared_ptr<romulus::render::GeometryChunkInstance> m_railGCIP;
    std::vector<boost::shared_ptr<
                    romulus::render::GeometryChunkInstance> > m_strutGCIPs;

    boost::shared_ptr<romulus::render::Material> m_mat;
    romulus::math::Vector3 m_keyPos;
    romulus::render::PointLight m_keyLight;
    romulus::math::Vector3 m_fill0Pos;
    romulus::render::PointLight m_fillLight0;
    romulus::math::Vector3 m_fill1Pos;
    romulus::render::PointLight m_fillLight1;

    boost::shared_ptr<romulus::render::GeometryChunkInstance> m_groundGCIP;
    boost::shared_ptr<romulus::render::Material> m_groundMat;
    boost::shared_ptr<romulus::MutableGeometryChunk> m_ground;

    SceneParameters& m_params;
    SceneParameters m_oldParams;
    real_t m_goalOffset;

    SolsticeGUI* m_gui;
};

#endif // _SOLSTICESCENE_H_
