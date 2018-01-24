#include "Resource/MD5MeshParser.h"
#include <algorithm>
#include <boost/lambda/lambda.hpp>
#include <boost/ref.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <sstream>

using namespace romulus;
using namespace boost::lambda;

bool Parse(MD5MeshParser& p, const char* md5mesh)
{
    std::istringstream iss(md5mesh);
    p.SetInput(iss);
    bool result = p.Parse();
    return result;
}

//! Callback to grab the mesh data structures from the parse.
struct MeshCallback
{
    MeshCallback(std::string* shader,
                 MD5MeshParser::VertContainer* verts,
                 MD5MeshParser::TriContainer* tris,
                 MD5MeshParser::WeightContainer* weights):
        Shader(shader), Verts(verts), Tris(tris), Weights(weights)
    { }

    void operator()(std::string& s, MD5MeshParser::VertContainer& v,
                    MD5MeshParser::TriContainer& t,
                    MD5MeshParser::WeightContainer& w)
    {
        if (Shader) std::swap(*Shader, s);
        if (Verts) std::swap(*Verts, v);
        if (Tris) std::swap(*Tris, t);
        if (Weights) std::swap(*Weights, w);
    }

    std::string* Shader;
    MD5MeshParser::VertContainer* Verts;
    MD5MeshParser::TriContainer* Tris;
    MD5MeshParser::WeightContainer* Weights;
};

char simpleTest[] =
"MD5Version 10\n"
"commandline \"foo\"\n"
"numJoints 0\n"
"numMeshes 1\n"
"joints {\n"
"\n"
"}\n"
"mesh {\n"
"  shader \"mat\"\n"
"  numverts 0\n"
"  numtris 0\n"
"  numweights 0\n"
"}\n";

BOOST_AUTO_TEST_CASE(TestSimpleSuccessfulParse)
{
    int version, numJoints, numMeshes;
    std::string commandline, shader;
    MD5MeshParser p;
    p.SetHeaderCallback((var(version) = _1, var(commandline) = _2));
    p.SetMeshParametersCallback((var(numJoints) = _1, var(numMeshes) = _2));
    p.SetMeshCallback(MeshCallback(&shader, 0, 0, 0));

    BOOST_CHECK(Parse(p, simpleTest));
    BOOST_CHECK_EQUAL(version, 10);
    BOOST_CHECK_EQUAL(commandline, "foo");
    BOOST_CHECK_EQUAL(numJoints, 0);
    BOOST_CHECK_EQUAL(numMeshes, 1);
    BOOST_CHECK_EQUAL(shader, "mat");
}

char jointsTest[] =
"MD5Version 10\n"
"commandline \"bar\"\n"
"numJoints 2\n"
"numMeshes 1\n"
"joints {\n"
"\"bone1\" 0 ( 2.0 1.0 2.0 ) ( 0.0 0.25 1.25)\n"
"\"bone2\" 1 ( 2.0 0.0 1.0 ) ( 0.0 0.5 1.0 )\n"
"}\n"
"mesh {\n"
"  shader \"mat\"\n"
"  numverts 0\n"
"  numtris 0\n"
"  numweights 0\n"
"}\n";

BOOST_AUTO_TEST_CASE(TestJointsParse)
{
    MD5MeshParser p;
    MD5MeshParser::JointContainer joints;
    p.SetJointsCallback(var(joints) = _1);
    BOOST_CHECK(Parse(p, jointsTest));
    BOOST_CHECK_EQUAL(joints.size(), 2u);

    BOOST_CHECK_EQUAL(joints[0].get<0>(), "bone1");
    BOOST_CHECK_EQUAL(joints[0].get<1>(), 0);
    BOOST_CHECK(joints[0].get<2>() == math::Vector3(2.0, 1.0, 2.0));
    BOOST_CHECK(joints[0].get<3>() == math::Vector3(0.0, 0.25, 1.25));

    BOOST_CHECK_EQUAL(joints[1].get<0>(), "bone2");
    BOOST_CHECK_EQUAL(joints[1].get<1>(), 1);
    BOOST_CHECK(joints[1].get<2>() == math::Vector3(2.0, 0.0, 1.0));
    BOOST_CHECK(joints[1].get<3>() == math::Vector3(0.0, 0.5, 1.0));
}

char vertsTest[] =
"MD5Version 10\n"
"commandline \"foo\"\n"
"numJoints 0\n"
"numMeshes 1\n"
"joints {\n"
"\n"
"}\n"
"mesh {\n"
"  shader \"mat\"\n"
"  numverts 2\n"
"  vert 0 ( 0.25 1.0 ) 0 0 \n"
"  vert 1 ( 0.5 0.0 ) 0 0 \n"
"  numtris 0\n"
"  numweights 0\n"
"}\n";

BOOST_AUTO_TEST_CASE(TestVertsParse)
{
    MD5MeshParser::VertContainer v;

    MD5MeshParser p;
    p.SetMeshCallback(MeshCallback(0, &v, 0, 0));
    BOOST_CHECK(Parse(p, vertsTest));
    BOOST_CHECK_EQUAL(v.size(), 2u);

    BOOST_CHECK_EQUAL(v[0].get<0>(), 0);
    BOOST_CHECK(v[0].get<1>() == math::Vector2(0.25, 1.0));
    BOOST_CHECK_EQUAL(v[0].get<2>(), 0);
    BOOST_CHECK_EQUAL(v[0].get<3>(), 0);

    BOOST_CHECK_EQUAL(v[1].get<0>(), 1);
    BOOST_CHECK(v[1].get<1>() == math::Vector2(0.5, 0.0));
    BOOST_CHECK_EQUAL(v[1].get<2>(), 0);
    BOOST_CHECK_EQUAL(v[1].get<3>(), 0);
}

char trisTest[] =
"MD5Version 10\n"
"commandline \"foo\"\n"
"numJoints 0\n"
"numMeshes 1\n"
"joints {\n"
"\n"
"}\n"
"mesh{\n"
"  shader \"mat\"\n"
"  numverts 2\n"
"  vert 0 ( 0.2 1.0 ) 0 0 \n"
"  vert 1 ( 0.1 0.0 ) 0 0 \n"
"  numtris 2\n"
"  tri 0 1 0 1\n"
"  tri 1 0 1 0\n"
"  numweights 0\n"
"}\n";

BOOST_AUTO_TEST_CASE(TestTrisParse)
{
    MD5MeshParser::TriContainer t;

    MD5MeshParser p;
    p.SetMeshCallback(MeshCallback(0, 0, &t, 0));
    BOOST_CHECK(Parse(p, trisTest));
    BOOST_CHECK_EQUAL(t.size(), 2u);

    BOOST_CHECK_EQUAL(t[0].get<0>(), 0);
    BOOST_CHECK_EQUAL(t[0].get<1>(), 1);
    BOOST_CHECK_EQUAL(t[0].get<2>(), 0);
    BOOST_CHECK_EQUAL(t[0].get<3>(), 1);

    BOOST_CHECK_EQUAL(t[1].get<0>(), 1);
    BOOST_CHECK_EQUAL(t[1].get<1>(), 0);
    BOOST_CHECK_EQUAL(t[1].get<2>(), 1);
    BOOST_CHECK_EQUAL(t[1].get<3>(), 0);
}

char weightsTest[] =
"MD5Version 10\n"
"commandline \"foo\"\n"
"numJoints 0\n"
"numMeshes 1\n"
"joints {\n"
"\n"
"}\n"
"mesh {\n"
"  shader \"mat\"\n"
"  numverts 0\n"
"  numtris 0\n"
"  numweights 2\n"
"  weight 0 0 0.25 (1.25  2.0 0.25)\n"
"  weight 1 0 1 (1.25  3.25 0.25)\n"
"}\n";

BOOST_AUTO_TEST_CASE(TestWeightsParse)
{
    MD5MeshParser::WeightContainer w;

    MD5MeshParser p;
    p.SetMeshCallback(MeshCallback(0, 0, 0, &w));
    BOOST_CHECK(Parse(p, weightsTest));
    BOOST_CHECK_EQUAL(w.size(), 2u);

    BOOST_CHECK_EQUAL(w[0].get<0>(), 0);
    BOOST_CHECK_EQUAL(w[0].get<1>(), 0);
    BOOST_CHECK_EQUAL(w[0].get<2>(), 0.25);
    BOOST_CHECK(w[0].get<3>() == math::Vector3(1.25, 2.0, 0.25));

    BOOST_CHECK_EQUAL(w[1].get<0>(), 1);
    BOOST_CHECK_EQUAL(w[1].get<1>(), 0);
    BOOST_CHECK_EQUAL(w[1].get<2>(), 1.0);
    BOOST_CHECK(w[1].get<3>() == math::Vector3(1.25, 3.25, 0.25));
}

char commentTest[] =
"// COMMENT0\n"
"MD5Version 10 // COMMENT1\n"
"commandline \"foo\"\n"
"numJoints 0\n"
"numMeshes 1\n"
"joints {\n"
"\n"
"}\n"
"mesh { // COMMENT2\n"
"  shader \"mat\"\n"
"  numverts 0 // COMMENT3\n"
"  numtris 0\n"
"  numweights 0\n"
"}\n";

BOOST_AUTO_TEST_CASE(TestCommentParse)
{
    MD5MeshParser p;
    BOOST_CHECK(Parse(p, commentTest));
}

char missingNewline[] =
"MD5Version 10  commandline \"foo\"\n"
"numJoints 0\n"
"numMeshes 1\n"
"joints {\n"
"\n"
"}\n"
"mesh{\n"
"  shader \"mat\"\n"
"  numverts 2\n"
"  vert 0 ( 0.2 1.0 ) 0 0 \n"
"  vert 1 ( 0.1 0.0 ) 0 0 \n"
"  numtris 2\n"
"  tri 0 0 1 0\n"
"  tri 1 0 1 0\n"
"  numweights 0\n"
"}\n";

char trailingGarbage[] =
"MD5Version 10  \n"
"commandline \"foo\"\n"
"numJoints 0\n"
"numMeshes 1\n"
"joints {\n"
"\n"
"}\n"
"mesh{\n"
"  shader \"mat\"\n"
"  numverts 2\n"
"  vert 0 ( 0.2 1.0 ) 0 0 \n"
"  vert 1 ( 0.1 0.0 ) 0 0 \n"
"  numtris 2\n"
"  tri 0 0 1 0\n"
"  tri 1 0 1 0\n"
"  numweights 0\n"
"}\n"
"this shouldn't be here.\n";

char badNumVerts[] =
"MD5Version 10  \n"
"commandline \"foo\"\n"
"numJoints 0\n"
"numMeshes 1\n"
"joints {\n"
"\n"
"}\n"
"mesh{\n"
"  shader \"mat\"\n"
"  numverts 5\n"
"  vert 0 ( 0.2 1.0 ) 0 0 \n"
"  vert 1 ( 0.1 0.0 ) 0 0 \n"
"  numtris 2\n"
"  tri 0 0 1 0\n"
"  tri 1 0 1 0\n"
"  numweights 0\n"
"}\n";

char badNumTris[] =
"MD5Version 10  \n"
"commandline \"foo\"\n"
"numJoints 0\n"
"numMeshes 1\n"
"joints {\n"
"\n"
"}\n"
"mesh{\n"
"  shader \"mat\"\n"
"  numverts 2\n"
"  vert 0 ( 0.2 1.0 ) 0 0 \n"
"  vert 1 ( 0.1 0.0 ) 0 0 \n"
"  numtris 2\n"
"  tri 0 0 1 0\n"
"  numweights 0\n"
"}\n";

char badNumWeights[] =
"MD5Version 10  \n"
"commandline \"foo\"\n"
"numJoints 0\n"
"numMeshes 1\n"
"joints {\n"
"\n"
"}\n"
"mesh{\n"
"  shader \"mat\"\n"
"  numverts 2\n"
"  vert 0 ( 0.2 1.0 ) 0 0 \n"
"  vert 1 ( 0.1 0.0 ) 0 0 \n"
"  numtris 2\n"
"  tri 0 0 1 0\n"
"  numweights 2\n"
"}\n";

BOOST_AUTO_TEST_CASE(TestFailedParses)
{
    MD5MeshParser p;
    BOOST_CHECK(!Parse(p, ""));
    BOOST_CHECK(!Parse(p, missingNewline));
    BOOST_CHECK(!Parse(p, trailingGarbage));
    BOOST_CHECK(!Parse(p, badNumVerts));
    BOOST_CHECK(!Parse(p, badNumTris));
    BOOST_CHECK(!Parse(p, badNumWeights));
}
