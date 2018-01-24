//! \file MD5MeshParser.inl
//! Contains the inline method definitions of MD5MeshParser.

#include "Resource/MD5MeshParser.h"
#include "Utility/Assertions.h"

namespace romulus
{

bool MD5MeshParser::IsDelimiter(const char c) const
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '(' ||
            c == ')' || c == '{' || c == '}';
}

bool MD5MeshParser::IsTokenAndDelimiter(const char c) const
{
    return c == '\n' || c == '(' || c == ')' || c == '{' || c == '}';
}

bool MD5MeshParser::IsDelimiterAndNotToken(const char c) const
{
    return c == ' ' || c == '\t' || c == '\r';
}

void MD5MeshParser::AdvanceToken()
{
    ASSERT(m_input);
    // Find the start of the token.
    while ((m_input->good() && IsDelimiterAndNotToken(m_input->peek())) ||
           m_input->peek() == '/')
    {
        if (m_input->peek() == '/')
        {
            char nextTerm = static_cast<char>(m_input->get());
            if (m_input->good() && m_input->peek() == '/')
            {
                // If we encounter a comment, shift past the comment.
                while (m_input->good() && m_input->peek() != '\n')
                    m_input->ignore(1);
            }
            else
            {
                m_input->putback(nextTerm);
                break;
            }
        }
        else
        {
            m_input->ignore(1);
        }
    }

    m_token.clear();
    // Get all characters of the token.
    if (m_input->good())
    {
        m_token += static_cast<char>(m_input->get());
        if (m_input->good() && !IsTokenAndDelimiter(m_token[0]))
            while (m_input->good() && !IsDelimiter(m_input->peek()))
                m_token += static_cast<char>(m_input->get());
    }
}

bool MD5MeshParser::Scan(const std::string& expected, bool ignoreNewlines)
{
    if (ignoreNewlines)
        ScanNewlines();
    if (expected == m_token)
    {
        AdvanceToken();
        return true;
    }
    return false;
}

bool MD5MeshParser::ScanInt(int& n)
{
    if (sscanf(m_token.c_str(), "%d", &n) == 1)
    {
        AdvanceToken();
        return true;
    }
    return false;
}

bool MD5MeshParser::ScanReal(float& real)
{
    if (sscanf(m_token.c_str(), "%f", &real) == 1)
    {
        AdvanceToken();
        return true;
    }
    return false;
}

bool MD5MeshParser::ScanReal(double& real)
{
    if (sscanf(m_token.c_str(), "%lf", &real) == 1)
    {
        AdvanceToken();
        return true;
    }
    return false;
}

bool MD5MeshParser::ScanString(std::string& str)
{
    uint_t index = 0;
    if (m_token[index] == '"')
    {
        str.clear();
        for (++index; m_token.size() > index && m_token[index] != '"';
             ++index)
            str += m_token[index];

        if (m_token.size() == index)
            // Didn't see a closing '"' before end of token.
            return false;
        AdvanceToken();
        return true;
    }
    return false;
}

bool MD5MeshParser::ScanNewlines()
{
    bool scannedNewline = false;
    while (m_token == "\n")
    {
        ++m_line;
        scannedNewline = true;
        AdvanceToken();
    }
    return scannedNewline;
}

bool MD5MeshParser::ScanRealTriple(math::Vector3& triple)
{
    return Scan("(", false) && ScanReal(triple[0]) && ScanReal(triple[1]) &&
            ScanReal(triple[2]) && Scan(")", false);
}

bool MD5MeshParser::ScanRealDouble(math::Vector2& doubl)
{
    return Scan("(", false) && ScanReal(doubl[0]) && ScanReal(doubl[1]) &&
            Scan(")", false);
}

bool MD5MeshParser::ScanEOF()
{
    ASSERT(m_input);
    ScanNewlines();
    return m_input->eof();
}

bool MD5MeshParser::Parse()
{
    if (m_input && m_input->good())
    {
        AdvanceToken();
        m_line = 1;
        m_errors.clear();
        return ParseHeader() && ParseMeshParams() && ParseOrderedLists() &&
                ScanEOF();
    }
    m_errors = "error: Input has not been set or is empty.\n";
    return false;
}

bool MD5MeshParser::ParseHeader()
{
    int version;
    std::string str;
    if (Scan("MD5Version", true) && ScanInt(version) && ScanNewlines() &&
        Scan("commandline", true) && ScanString(str) && ScanNewlines())
    {
        if (m_headerCallback)
            m_headerCallback(version, str);
        return true;
    }
    Error("Missing or invalid header.");
    return false;
}

bool MD5MeshParser::ParseMeshParams()
{
    int numJoints, numMeshes;
    if (Scan("numJoints", true) && ScanInt(numJoints) && ScanNewlines() &&
        Scan("numMeshes", true) && ScanInt(numMeshes) && ScanNewlines())
    {
        if (m_meshParametersCallback)
            m_meshParametersCallback(numJoints, numMeshes);
        return true;
    }
    Error("Missing or invalid mesh parameters.");
    return false;
}

bool MD5MeshParser::ParseOrderedLists()
{
    if (ParseJoints())
    {
        while (ParseMesh());
        return true;
    }
    return false;
}

bool MD5MeshParser::ParseJoints()
{
    if (Scan("joints", true) && Scan("{", true))
    {
        JointContainer jointVector;
        std::string boneName;
        int parentIndex;
        math::Vector3 position;
        math::Vector3 orientation;
        while ((ScanNewlines() || true) && ScanString(boneName) &&
               ScanInt(parentIndex) && ScanRealTriple(position) &&
               ScanRealTriple(orientation) && ScanNewlines())
            jointVector.push_back(
                    Joint(boneName, parentIndex, position, orientation));
        if (m_jointsCallback)
            m_jointsCallback(jointVector);
        return Scan("}", true);
    }
    return false;
}

bool MD5MeshParser::ParseMesh()
{
    std::string shaderName;
    VertContainer verts;
    TriContainer tris;
    WeightContainer weights;
    if (Scan("mesh", true) && Scan("{", true) && Scan("shader", true) &&
        ScanString(shaderName) && ScanNewlines() && ParseVerts(verts) &&
        ParseTris(tris) && ParseWeights(weights))
    {
        if (m_meshCallback)
            m_meshCallback(shaderName, verts, tris, weights);
        return Scan("}", true);
    }
    return false;
}

bool MD5MeshParser::ParseVerts(VertContainer& vertVec)
{
    int numVerts;
    vertVec.clear();
    if (Scan("numverts", true) && ScanInt(numVerts) && ScanNewlines())
    {
        int index, weightIndex, weightElement;
        math::Vector2 tex;
        while (Scan("vert", true) && ScanInt(index) && ScanRealDouble(tex) &&
               ScanInt(weightIndex) && ScanInt(weightElement) &&
               ScanNewlines())
            vertVec.push_back(Vert(index, tex, weightIndex,
                                   weightElement));
        if (numVerts != static_cast<int>(vertVec.size()))
        {
            Error("numverts declaration does not match number of verts.");
            return false;
        }
        return true;
    }
    return false;
}

bool MD5MeshParser::ParseTris(TriContainer& triVec)
{
    int numTris;
    triVec.clear();
    if (Scan("numtris", true) && ScanInt(numTris) && ScanNewlines())
    {
        int index, vertIndex1, vertIndex2, vertIndex3;
        while (Scan("tri", true) && ScanInt(index) && ScanInt(vertIndex1) &&
               ScanInt(vertIndex2) && ScanInt(vertIndex3) &&
               ScanNewlines())
            triVec.push_back(Tri(index, vertIndex1, vertIndex2, vertIndex3));
        if (numTris != static_cast<int>(triVec.size()))
        {
            Error("numtris declaration does not match number of tris.");
            return false;
        }
        return true;
    }
    return false;
}

bool MD5MeshParser::ParseWeights(WeightContainer& weightVec)
{
    int numWeights;
    if (Scan("numweights", true) && ScanInt(numWeights) && ScanNewlines())
    {
        int index, jointIndex;
        real_t weightValue;
        math::Vector3 position;
        while (Scan("weight", true) && ScanInt(index) && ScanInt(jointIndex) &&
               ScanReal(weightValue) && ScanRealTriple(position) &&
               ScanNewlines())
            weightVec.push_back(Weight(index, jointIndex, weightValue,
                                       position));
        if (numWeights != static_cast<int>(weightVec.size()))
        {
            Error("numweights declaration does not match number of weights.");
            return false;
        }
        return true;
    }
    return false;
}

} // namespace romulus
