#include "Render/IScene.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "Utility/SceneToSTL.h"
#include <map>

namespace romulus
{

using namespace render;
using namespace math;

namespace
{

void GeometryChunkInstanceToSTL(const GeometryChunkInstance& gci,
                                std::ostream& out)
{
    const Matrix44& xform = gci.Transform();
    const GeometryChunk* gc = gci.GeometryChunk();
    const ushort_t* ind = gc->Indices();

    for (uint_t index = 0; index < gc->IndexCount(); index += 3)
    {
        if (ind[index] == ind[index + 1] && ind[index] == ind[index + 2])
            continue;

        Vector3 v0, v1, v2;
        v0 = gc->Vertices()[ind[index]];
        v1 = gc->Vertices()[ind[index + 1]];
        v2 = gc->Vertices()[ind[index + 2]];
        Vector3 normal = Normal(Cross(Normal(v1 - v0), Normal(v2 - v1)));
        normal = Submatrix<3, 3, 0, 0>(xform) * normal;

        out << "  facet normal " << normal[0] << ' ' << normal[1] <<
                ' ' << normal[2] << '\n';
        out << "    outer loop\n";
        for (uint_t i = 0; i < 3; ++i)
        {
            Vector4 vert(gc->Vertices()[ind[index + i]], 1);
            vert = xform * vert;

            out << "      vertex " << vert[0] << ' ' << vert[1] <<
                    ' ' << vert[2] << '\n';
        }
        out << "    endloop\n";
        out << "  endfacet\n";
    }
}

} // namespace

void SceneFrameToSTL(const render::IScene& scene, std::ostream& out)
{
    out << "solid FOO\n";

    IScene::GeometryCollection geo;

    scene.Geometry(geo);

    // Emit the geometry chunk instances.
    for (IScene::GeometryCollection::iterator it = geo.begin();
         it != geo.end(); ++it)
    {
        GeometryChunkInstanceToSTL(**it, out);
    }

    out << "endsolid FOO\n";
}

} // namespace romulus
