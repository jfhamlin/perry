#ifndef _MD5MESHPARSER_H_
#define _MD5MESHPARSER_H_

//! \file MD5MeshParser.h
//! Contains a parser for the md5mesh file type.

#include "Core/Types.h"
#include "Math/Vector.h"
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <istream>
#include <vector>

namespace romulus
{

//! This class parses an md5mesh file. By setting callbacks, one can extract
//! the data during the parse. The parser is not reentrant.
class MD5MeshParser
{
public:
    typedef boost::tuple<std::string, int, math::Vector3, math::Vector3> Joint;
    typedef boost::tuple<int, math::Vector2, int, int> Vert;
    typedef boost::tuple<int, int, int, int> Tri;
    typedef boost::tuple<int, int, real_t, math::Vector3> Weight;

    typedef std::vector<Joint> JointContainer;
    typedef std::vector<Vert> VertContainer;
    typedef std::vector<Tri> TriContainer;
    typedef std::vector<Weight> WeightContainer;

    MD5MeshParser(): m_input(0) {}

    //! Set the input to be parsed.
    //! /param input - An input stream of the ascii md5mesh.
    void SetInput(std::istream& input) { m_input = &input; }

    //! Attempt to parse the input.
    //! /return true if the parse succeeds, false if it fails or if the input
    //!         is not set.
    bool Parse();

    //! /return a string describing any errors from the last call to Parse().
    std::string ErrorString() const { return m_errors; }

    //! Set a callback to retrieve md5mesh header data.
    //! \param headerCallback - a function whose first argument is an int&, the
    //!                         MD5Version, and whose second argument is a
    //!                         string&, the commandline string.
    void SetHeaderCallback(
            const boost::function<void (int, std::string&)> headerCallback)
    { m_headerCallback = headerCallback; }

    //! Set a callback to retrieve the md5mesh-specific parameters.
    //! \param meshParamCallback - a function whose first argument is an int&,
    //!                            the number of joints defined in the file,
    //!                            and whose second argument is an int&, the
    //!                            number of meshes in the file.
    void SetMeshParametersCallback(
            const boost::function<void(int, int)> meshParamCallback)
    { m_meshParametersCallback = meshParamCallback; }

    //! Set a callback to retrieve the joint data, a container of tuples of
    //! the md5mesh joint declaration's data.
    //! \param jointsCallback - a function whose only argument is an
    //!                         MD5MeshParser::JointContainer&.
    void SetJointsCallback(
            const boost::function<void (JointContainer&)> jointsCallback)
    { m_jointsCallback = jointsCallback; }

    //! Set a callback to retrieve mesh data for each mesh.
    //! \param meshCallback - a function whose first argument is a string&
    //!                       representing the shader name, the second argument
    //!                       a VertContainer&, the third a TriContainer&, and
    //!                       the fourth a WeightContainer&, each container
    //!                       holding tuples with the corresponding
    //!                       declaration's data.
    void SetMeshCallback(const boost::function<
                         void (std::string&, VertContainer&,
                               TriContainer&, WeightContainer&)> meshCallback)
    { m_meshCallback = meshCallback; }

private:
    bool ParseHeader();
    bool ParseMeshParams();
    bool ParseOrderedLists();
    bool ParseJoints();
    bool ParseMesh();
    bool ParseVerts(VertContainer& vertVec);
    bool ParseTris(TriContainer& triVec);
    bool ParseWeights(WeightContainer& weightVec);

    bool Scan(const std::string& expected, bool ignoreNewlines);
    bool ScanInt(int& n);
    bool ScanReal(float& real);
    bool ScanReal(double& real);
    bool ScanString(std::string& str);
    bool ScanNewlines();
    bool ScanRealTriple(math::Vector3& triple);
    bool ScanRealDouble(math::Vector2& doubl);
    bool ScanEOF();

    bool IsDelimiter(const char c) const;
    bool IsTokenAndDelimiter(const char c) const;
    bool IsDelimiterAndNotToken(const char c) const;
    void AdvanceToken();

    void Error(std::string error)
    {
        m_errors += boost::lexical_cast<std::string>(m_line) + ":error: " +
                error + "\n";
    }

    //! The pointer to the istream of the md5mesh.
    std::istream* m_input;
    //! The current token.
    std::string m_token;

    boost::function<void (int, std::string&)> m_headerCallback;
    boost::function<void (int, int)> m_meshParametersCallback;
    boost::function<void (JointContainer&)> m_jointsCallback;
    boost::function<void (std::string&,                // shader name
                          VertContainer&,     // verts
                          TriContainer&,      // tris
                          WeightContainer&)>  // weights
    m_meshCallback;

    //! The current line number of the input.
    uint_t m_line;
    //! A string of descriptions of any errors from the last parse.
    std::string m_errors;
};

} // namespace romulus

#endif // _MD5MESHPARSER_H_
