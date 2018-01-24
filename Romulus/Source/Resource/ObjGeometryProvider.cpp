#include "Resource/ObjGeometryProvider.h"
#include "Resource/ResourceManager.h"
#include "Math/Utilities.h"
#include <boost/lexical_cast.hpp>
#include <map>

namespace romulus
{
ObjGeometryProvider::ObjGeometryProvider()
{
    m_extensions.push_back("obj");
}

ObjGeometryProvider::~ObjGeometryProvider()
{
}

bool ObjGeometryProvider::LoadResource(std::istream& stream,
                                       ResourceHandleBase::id_t& id)
{
    try
    {
        GeometryPtr geometry = ParseStream(stream);

        uint_t index = FindFreeSlot();
        id =  index + 1;
        m_geometry[index] = geometry;
    }
    catch (const std::exception& e)
    {
        return false;
    }

    return true;
}

void ObjGeometryProvider::UnloadResource(ResourceHandleBase::id_t id)
{
    m_geometry[id - 1].reset();
}

namespace
{
std::string Parse(char frontSentinel, char endSentinel, std::string& str)
{
    std::size_t start, end;
    start = str.find_first_of(frontSentinel);
    end = str.find_first_of(endSentinel);
    if (start == std::string::npos || end == std::string::npos)
        return "";

    std::string first = str.substr(start, end - start);
    str = str.substr(end);
    return first;
}

std::string Parse(char endSentinel, std::string& str)
{
    std::size_t end;
    end = str.find_first_of(endSentinel);
    if (end == std::string::npos)
        return "";

    std::string first = str.substr(0, end);
    str = str.substr(end + 1);

    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] != endSentinel)
        {
            str = str.substr(i);
            break;
        }
    }

    return first;
}

math::Vector2 ParseVector2(std::string& line)
{
    math::Vector2 vec;

    vec[0] = boost::lexical_cast<real_t>(Parse(' ', line));
    vec[1] = boost::lexical_cast<real_t>(line);

    return vec;
}

math::Vector3 ParseVector3(std::string& line)
{
    math::Vector3 vec;

    vec[0] = boost::lexical_cast<real_t>(Parse(' ', line));
    vec[1] = boost::lexical_cast<real_t>(Parse(' ', line));
    vec[2] = boost::lexical_cast<real_t>(line);

    return vec;
}

struct ObjVertex
{
    uint_t Vertex;
    uint_t UV;
    uint_t Normal;
};

inline bool operator<(const ObjVertex& a, const ObjVertex& b)
{
    if (a.Vertex < b.Vertex)
        return true;

    if (a.Vertex == b.Vertex)
    {
        if (a.UV < b.UV)
            return true;
        else if (a.UV == b.UV && a.Normal < b.Normal)
            return true;
    }
    return false;
}

struct ObjFace
{
    ObjVertex V[3];
};

struct Vertex
{
    math::Vector3 V;
    math::Vector3 N;
    math::Vector2 UV;
};

ObjVertex ParseVertex(std::string line)
{
    ObjVertex vertex;

    vertex.Vertex = boost::lexical_cast<uint_t>(Parse('/', line));
    vertex.UV = boost::lexical_cast<uint_t>(Parse('/', line));
    vertex.Normal = boost::lexical_cast<uint_t>(line);

    return vertex;
}

ObjFace ParseFace(std::string& line)
{
    ObjFace face;

    face.V[0] = ParseVertex(Parse(' ', line));
    face.V[1] = ParseVertex(Parse(' ', line));
    face.V[2] = ParseVertex(line);

    return face;
}


typedef std::vector<math::Vector3> Vector3List;
typedef std::vector<math::Vector2> Vector2List;
typedef std::vector<ObjFace> FaceList;

void ConstructGeometry(ObjGeometryProvider::GeometryPtr geometry,
                       Vector3List& vertices, Vector3List& normals,
                       Vector2List& texCoords, FaceList& faces)
{

    typedef std::vector<Vertex> VertexList;
    typedef std::map<ObjVertex, ushort_t> VertexMap;
    typedef std::vector<ushort_t> IndexList;

    VertexMap vertexMap;
    VertexList constructed;
    IndexList indices;

    for (FaceList::const_iterator iter = faces.begin(); iter != faces.end();
         ++iter)
    {
        ObjFace face = *iter;

        for (int i = 0; i < 3; ++i)
        {
            ObjVertex objVert = face.V[i];
            VertexMap::iterator index = vertexMap.find(objVert);
            if (index == vertexMap.end())
            {
                Vertex vert;
                vert.V = vertices[objVert.Vertex - 1];
                vert.N = normals[objVert.Normal - 1];
                vert.UV = texCoords[objVert.UV - 1];
                constructed.push_back(vert);
                size_t index = constructed.size() - 1;
                indices.push_back(index);
                vertexMap.insert(std::make_pair(objVert, index));
            }
            else
            {
                indices.push_back(index->second);
            }
        }
    }

    // Now, copy everything into the GC.
    geometry->V.reserve(constructed.size());
    geometry->N.reserve(constructed.size());
    geometry->UV.reserve(constructed.size());
    geometry->I.reserve(indices.size());

    for (VertexList::const_iterator vertex = constructed.begin();
         vertex != constructed.end(); ++vertex)
    {
        geometry->V.push_back(vertex->V);
        geometry->N.push_back(vertex->N);
        geometry->UV.push_back(vertex->UV);
    }

    for (IndexList::const_iterator index = indices.begin();
         index != indices.end(); ++index)
    {
        geometry->I.push_back(*index);
    }
    geometry->T.resize(geometry->VertexCount());
    math::CalculateTangentVectors(geometry->Vertices(), geometry->Normals(),
                                  geometry->TextureCoordinates(),
                                  geometry->VertexCount(), geometry->Indices(),
                                  geometry->IndexCount(), &geometry->T[0]);
    geometry->ComputeBoundingVolume();
}

}

ObjGeometryProvider::GeometryPtr ObjGeometryProvider::ParseStream(
        std::istream& stream)
{
    GeometryPtr geometry(new ObjGeometry);

    Vector3List vertices;
    Vector3List normals;
    Vector2List texCoords;
    FaceList faces;

    std::string line;
    while (!stream.eof())
    {
        std::getline(stream, line);

        std::string command = Parse(' ', line);
        if (command == "v")
        {
            vertices.push_back(ParseVector3(line));
        }
        else if (command == "vt")
        {
            math::Vector2 uv;
            if (line.find_first_of(' ') == line.find_last_of(' '))
            {
                uv = ParseVector2(line);
            }
            else
            {
                math::Vector3 vec = ParseVector3(line);
                uv = math::Vector2(vec[0], vec[1]);
            }

            uv[1] = 1.f - uv[1];

            texCoords.push_back(uv);
        }
        else if (command == "vn")
        {
            normals.push_back(ParseVector3(line));
        }
        else if (command == "f")
        {
            faces.push_back(ParseFace(line));
        }
    }

    ConstructGeometry(geometry, vertices, normals, texCoords, faces);
    return geometry;
}

uint_t ObjGeometryProvider::FindFreeSlot()
{
    for (uint_t i = 0; i < m_geometry.size(); ++i)
    {
        if (!m_geometry[i].get())
            return i;
    }

    m_geometry.push_back(GeometryPtr());
    return m_geometry.size() - 1;
}

}
