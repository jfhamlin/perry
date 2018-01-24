#include "Math/Utilities.h"
#include <boost/scoped_array.hpp>

namespace romulus
{
namespace math
{

Matrix<4, 4, real_t> GenerateRotationTransform(Vector3 axis, real_t angle)
{
    Matrix44 identity;
    SetIdentity(identity);
    Normalize(axis);
    const real_t& x = axis[0];
    const real_t& y = axis[1];
    const real_t& z = axis[2];

    real_t xx = x * x;
    real_t xy = x * y;
    real_t xz = x * z;
    real_t yy = y * y;
    real_t yz = y * z;
    real_t zz = z * z;
    real_t c = cos(angle);
    real_t s = sin(angle);

    Matrix44 m;
    m[0][0] = xx*(1-c)+c; m[0][1] = xy*(1-c)-z*s; m[0][2] = xz*(1-c)+y*s;
    m[0][3] = 0;
    m[1][0] = xy*(1-c)+z*s; m[1][1] = yy*(1-c)+c; m[1][2] = yz*(1-c)-x*s;
    m[1][3] = 0;
    m[2][0] = xz*(1-c)-y*s; m[2][1] = yz*(1-c)+x*s; m[2][2] = zz*(1-c)+c;
    m[2][3] = 0;
    m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;

    return m;
}

Matrix44 GeneratePerspectiveProjectionTransform(real_t fov, real_t aspectRatio,
                                                real_t near, real_t far)
{
    real_t top = tanf(fov * 0.5f) * near;
    real_t bottom = -top;
    real_t right = top * aspectRatio;
    real_t left = bottom * aspectRatio;

    Matrix44 transform;

    transform[0][0] = (2 * near) / (right - left);
    transform[0][1] = 0;
    transform[0][2] = (right + left) / (right - left);
    transform[0][3] = 0;

    transform[1][0] = 0;
    transform[1][1] = (2 * near) / (top - bottom);
    transform[1][2] = (top + bottom) / (top - bottom);
    transform[1][3] = 0;

    transform[2][0] = 0;
    transform[2][1] = 0;
    transform[2][2] = -((far + near) / (far - near));
    transform[2][3] = -((2 * far * near) / (far - near));

    transform[3][0] = 0;
    transform[3][1] = 0;
    transform[3][2] = -1;
    transform[3][3] = 0;

    return transform;
}

Matrix44 GenerateOrthographicProjectionTransform(real_t left, real_t right,
                                                 real_t bottom, real_t top,
                                                 real_t near, real_t far)
{
    Matrix44 transform;

    transform[0][0] = 2 / (right - left);
    transform[1][0] = 0;
    transform[2][0] = 0;
    transform[3][0] = 0;

    transform[0][1] = 0;
    transform[1][1] = 2 / (top - bottom);
    transform[2][1] = 0;
    transform[3][1] = 0;

    transform[0][2] = 0;
    transform[1][2] = 0;
    transform[2][2] = 2 / (far - near);
    transform[3][2] = 0;

    transform[0][3] = - (right + left) / (right - left);
    transform[1][3] = - (top + bottom) / (top - bottom);
    transform[2][3] = - (far + near) / (far - near);
    transform[3][3] = 1;

    return transform;
}

void CalculateTangentVectors(const Vector3* vertices, const Vector3* normals,
                             const Vector2* texCoords, size_t vertexCount,
                             const ushort_t* indices, size_t indexCount,
                             Vector3* tangents)
{
    ASSERT(vertices);
    ASSERT(normals);
    ASSERT(texCoords);
    ASSERT(indices);
    ASSERT(tangents);
    ASSERT(indexCount % 3 == 0);

    boost::scoped_array<Vector3> tan1(new Vector3[vertexCount]);
    boost::scoped_array<Vector3> tan2(new Vector3[vertexCount]);

    for (uint_t i = 0; i < vertexCount; ++i)
    {
        tan1[i] = Vector3(0.f);
        tan2[i] = Vector3(0.f);
    }

    struct Face
    {
        ushort_t A, B, C;
    };
    const Face* faces = reinterpret_cast<const Face*>(indices);
    size_t faceCount = indexCount / 3;

    for (size_t i = 0; i < faceCount; ++i)
    {
        const Face& face = faces[i];

        const Vector3& v1 = vertices[face.A];
        const Vector3& v2 = vertices[face.B];
        const Vector3& v3 = vertices[face.C];

        const Vector2& w1 = texCoords[face.A];
        const Vector2& w2 = texCoords[face.B];
        const Vector2& w3 = texCoords[face.C];

        Vector3 xyz1(v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2]);
        Vector3 xyz2(v3[0] - v1[0], v3[1] - v1[1], v3[2] - v1[2]);

        Vector2 st1(w2[0] - w1[0], w2[1] - w1[1]);
        Vector2 st2(w3[0] - w1[0], w3[1] - w1[1]);

        real_t r = 1.f / (st1[0] * st2[1] - st2[0] * st1[1]);
        Vector3 sDir((st2[1] * xyz1[0] - st1[1] * xyz2[0]) * r,
                     (st2[1] * xyz1[1] - st1[1] * xyz2[1]) * r,
                     (st2[1] * xyz1[2] - st1[1] * xyz2[2]) * r);

        Vector3 tDir((st1[0] * xyz2[0] - st2[0] * xyz1[0]) * r,
                     (st1[0] * xyz2[1] - st2[0] * xyz1[1]) * r,
                     (st1[0] * xyz2[2] - st2[0] * xyz1[2]) * r);

        tan1[face.A] += sDir;
        tan1[face.B] += sDir;
        tan1[face.C] += sDir;

        tan2[face.A] += tDir;
        tan2[face.B] += tDir;
        tan2[face.C] += tDir;
    }

    for (size_t i = 0; i < vertexCount; ++i)
    {
        const Vector3& normal = normals[i];
        const Vector3& tangent = tan1[i];

        tangents[i] = Normal(tangent - normal * Dot(normal, tangent));

        real_t w = (Dot(Cross(normal, tangent), tan2[i]) < 0.f) ? 1.f : -1.f;
        tangents[i] *= 1.f / w;
        Normalize(tangents[i]);
    }
}

}
}

