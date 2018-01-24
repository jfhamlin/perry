#include "Render/Camera.h"
#include "Render/IScene.h"
#include "Utility/SceneToRIB.h"
#include <boost/lexical_cast.hpp>
#include <map>

namespace romulus
{

using namespace render;

namespace
{

class Indent
{
public:

    Indent(int& ind): m_indentation(ind) { m_indentation += 4; }
    ~Indent() { ASSERT(m_indentation >= 4); m_indentation -= 4; }

private:

    int& m_indentation;
};

std::string Spaces(int numSpaces)
{
    std::string s;
    for (; numSpaces > 0; --numSpaces)
        s += ' ';
    return s;
}

void TransformToRIB(const math::Matrix44& mat, int indent, std::ostream& out)
{
    out << Spaces(indent) << "ConcatTransform [";
    for (int i = 0; i < 4; ++i)
    {
        out << '\n' << Spaces(indent);
        for (int j = 0; j < 4; ++j)
        {
            // Swap i and j, since RenderMan post-multiplies by the
            // transform rather than pre-multiplies.
            out << mat[j][i] << ' ';
        }
    }
    out << '\n' << Spaces(indent) << "]\n";
}

void TransformToRIB(const math::Transformation& x, int indent, std::ostream& out)
{
    out << Spaces(indent) << "Translate " <<
            x.Translation().Vector()[0] << ' ' <<
            x.Translation().Vector()[1] << ' ' <<
            x.Translation().Vector()[2] << '\n';
    out << Spaces(indent) << "Rotate " <<
            math::RadiansToDegrees(x.Rotation().Angle()) << ' ' <<
            x.Rotation().Axis()[0] << ' ' <<
            x.Rotation().Axis()[1] << ' ' <<
            x.Rotation().Axis()[2] << '\n';
    out << Spaces(indent) << "Scale " <<
            x.Scale().Value() << ' ' <<
            x.Scale().Value() << ' ' <<
            x.Scale().Value() << '\n';
}

void CameraToRIB(const Camera& cam, int indent, std::ostream& out)
{
    out << Spaces(indent) << "Projection \"perspective\" \"fov\" " <<
            55 << '\n';
    out << Spaces(indent) << "Scale 1 1 -1\n";
    math::Transformation x = Inverse(cam.Transformation());
    TransformToRIB(x, indent, out);
}

void GeometryChunkToRIB(const GeometryChunk& gc, int id, int indent,
                        std::ostream& out)
{
    out << Spaces(indent) << "ObjectBegin " << id << '\n';
    {
        Indent ind(indent);

        ASSERT(gc.IndexCount() % 3 == 0);

        uint_t numTris = 0;
        for (uint_t i = 0; i < gc.IndexCount(); i += 3)
        {
            // Only count tris with distinct vertices.
            if (gc.Indices()[i + 0] != gc.Indices()[i + 1] ||
                gc.Indices()[i + 0] != gc.Indices()[i + 2])
                ++numTris;
        }

        const uint_t numVerts = gc.VertexCount();

        out << Spaces(indent) << "PointsPolygons [ ";
        for (uint_t i = 0; i < numTris; ++i)
            out << "3 ";
        // We add one for a dummy triangle for reasons described below.
        out << "3 ";
        out << "]\n";

        {
            Indent ind2(indent);

            out << Spaces(indent) << "[ ";
            for (uint_t i = 0; i < gc.IndexCount(); i += 3)
            {
                if (gc.Indices()[i + 0] == gc.Indices()[i + 1] &&
                    gc.Indices()[i + 0] == gc.Indices()[i + 2])
                    continue;
                out << gc.Indices()[i + 0] << ' ';
                out << gc.Indices()[i + 1] << ' ';
                out << gc.Indices()[i + 2] << ' ';
                out << '\n' << Spaces(indent);
            }
            // We add one extra (degenerate) triangle to address the final
            // vertex, since RenderMan expects v+1 verts, where v is the
            // highest number of an indexed vertex.
            out << numVerts - 1 << ' ';
            out << numVerts - 1 << ' ';
            out << numVerts - 1 << ' ';
            out << "]\n";

            out << Spaces(indent) << "\"P\" [ ";
            for (uint_t i = 0; i < numVerts; ++i)
            {
                for (uint_t j = 0; j < 3; ++j)
                    out << gc.Vertices()[i][j] << ' ';
                out << '\n' << Spaces(indent);
            }
            out << "]\n";
            out << Spaces(indent) << "\"N\" [ ";
            for (uint_t i = 0; i < numVerts; ++i)
            {
                for (uint_t j = 0; j < 3; ++j)
                    out << gc.Normals()[i][j] << ' ';
                out << '\n' << Spaces(indent);
            }
            out << "]\n";
        }
    }
    out << "ObjectEnd\n";
}

void MaterialToRIB(const Material& m, int indent, std::ostream& out)
{
    out << Spaces(indent) << "Color [" <<
            m.Color()[0] << ' ' << m.Color()[1] << ' ' << m.Color()[2] <<
            "]\n";
    out << Spaces(indent) << "Surface \"rom_metal\"" <<
            "\"Ka\" [0.5] " <<
            "\"Ks\" [" << m.SpecularAlbedo() << "] " <<
            "\"roughness\" " << 1.0 / m.SpecularExponent() << '\n';
}

void GeometryChunkInstanceToRIB(const GeometryChunkInstance& gci,
                                int id, int indent, std::ostream& out)
{
    out << Spaces(indent) << "AttributeBegin\n";
    {
        Indent ind(indent);

        MaterialToRIB(*gci.SurfaceDescription(), indent, out);

        // Output instance transform.
        TransformToRIB(gci.Transform(), indent, out);

        // Output instance surface description.

        // Reference the geometry chunk object.
        out << Spaces(indent) << "ObjectInstance " << id << '\n';
    }
    out << Spaces(indent) << "AttributeEnd\n";
}

void LightToRIB(const Light& l, const std::string& name,
                int indent, std::ostream& out)
{
    // We multiply intensity by two, since there seems to be a lack of
    // correspondence between light intensity with opengl and in Pixie.
    // \todo - Theres probably an ambient light turned on in opengl.
    //         Will fix later.
    out << Spaces(indent) << "LightSource \"rom_shadowpoint\" \"" << name <<
            "\" \"from\" [" << l.Position()[0] << ' ' << l.Position()[1] <<
            ' ' << l.Position()[2] << "] " << "\"intensity\" [" <<
            2. * l.Intensity() << "] \"lightcolor\" [" << l.Color()[0] << ' ' <<
            l.Color()[1] << ' ' << l.Color()[2] << ']' <<
            " \"shadowname_px\" [\"raytrace\"]" <<
            " \"shadowname_nx\" [\"raytrace\"]" <<
            " \"shadowname_py\" [\"raytrace\"]" <<
            " \"shadowname_ny\" [\"raytrace\"]" <<
            " \"shadowname_pz\" [\"raytrace\"]" <<
            " \"shadowname_nz\" [\"raytrace\"]\n";
}

} // namespace

void SceneFrameToRIB(const render::IScene& scene, const render::Camera& cam,
                     int imageWidth, int imageHeight,
                     const std::string& imageFile, std::ostream& out)
{
    out << "## Renderman RIB output of Romulus scene.\n";

    IScene::GeometryCollection geo;
    IScene::LightCollection lights;

    scene.Geometry(geo);
    scene.Lights(lights);
    std::map<const GeometryChunk*, int> chunkIDMap;

    int indent = 0;
    int objectID = 1;

    for (IScene::GeometryCollection::iterator it = geo.begin();
         it != geo.end(); ++it)
    {
        const GeometryChunk* gc = (*it)->GeometryChunk();
        ASSERT(gc);
        int id = objectID++;
        if (chunkIDMap.find(gc) == chunkIDMap.end())
        {
            GeometryChunkToRIB(*gc, id, indent, out);
            chunkIDMap[gc] = id;
        }
    }

    int frame = 0;

    out << "FrameBegin " << frame << "\n";
    {
        Indent ind(indent);
        out << Spaces(indent) << "Option \"trace\" \"int maxdepth\" [2]\n";
        out << Spaces(indent) << "Format " << imageWidth << ' ' <<
                imageHeight << " 1\n";
        out << '\n';

        out << Spaces(indent) << "PixelSamples 4 4\n";
        out << Spaces(indent) << "ShadingInterpolation \"smooth\"\n";
        out << '\n';

        out << Spaces(indent) << "Display \"" << imageFile <<
                "\" \"framebuffer\" \"rgba\"\n";
        out << Spaces(indent) << "Display \"+" << imageFile <<
                "\" \"file\" \"rgba\"\n";
        out << '\n';

        CameraToRIB(cam, indent, out);
        out << '\n';

        out << Spaces(indent) << "WorldBegin\n";
        {
            Indent ind(indent);

            out << Spaces(indent) <<
                    "Attribute \"visibility\" \"specular\" [1]\n";
            out << Spaces(indent) <<
                    "Attribute \"visibility\" \"diffuse\" [1]\n";
            out << Spaces(indent) <<
                    "Attribute \"visibility\" \"transmission\" [1]\n";
            out << Spaces(indent) <<
                    "Attribute \"trace\" \"bias\" 0.005\n";

            out << Spaces(indent) <<
                    "Attribute \"shade\" \"transmissionhitmode\" "
                    "[\"primitive\"]\n";
            out << '\n';

            // Emit a single ambient light, then the rest.
            out << Spaces(indent) << "LightSource \"ambientocclusion\" "
                    "\"amb0\" \"numSamples\" [256]\n";
            uint_t i = 0;
            for (IScene::LightCollection::iterator it = lights.begin();
                 it != lights.end(); ++it, ++i)
                LightToRIB(**it, "light" + boost::lexical_cast<std::string>(i),
                           indent, out);
            out << '\n';

            // Emit the geometry chunk instances.
            for (IScene::GeometryCollection::iterator it = geo.begin();
                 it != geo.end(); ++it)
            {
                const GeometryChunk* gc = (*it)->GeometryChunk();
                std::map<const GeometryChunk*, int>::iterator chunkIt =
                        chunkIDMap.find(gc);
                ASSERT(chunkIt != chunkIDMap.end());
                GeometryChunkInstanceToRIB(**it, chunkIt->second, indent, out);
            }
            out << '\n';
        }
        out << Spaces(indent) << "WorldEnd\n";
    }
    out << "FrameEnd\n";
}

} // namespace romulus
