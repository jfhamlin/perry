#include "Math/Utilities.h"
#include "Math/Vector.h"
#include <boost/test/auto_unit_test.hpp>

using namespace romulus::math;

BOOST_AUTO_TEST_CASE(VectorTest)
{
    Vector3 unitx = Vector3(1, 0, 0);
    Vector3 unity = Vector3(0, 1, 0);
    Vector3 unitz = Vector3(0, 0, 1);

    Vector3 vec1(1, 3, 10);
    Vector3 vec2(2, 1.2f, 0.5f);

    BOOST_CHECK(Cross(unitx, unity) == unitz);

    BOOST_CHECK(Cross(unity, unitz) == unitx);

    BOOST_CHECK(Cross(unitz, unitx) == unity);

    BOOST_CHECK(fabs(Dot(Cross(vec1, vec2), vec1)) <= AcceptableMathDelta &&
                fabs(Dot(Cross(vec1, vec2), vec2)) <= AcceptableMathDelta);

    BOOST_CHECK(10 * Vector3(3, 4, 10) == Vector3(30, 40, 100));

    BOOST_CHECK(Normal(5 * unitx) == unitx);

    BOOST_CHECK(Normal(Vector3(5)) == Vector3(.5773503f));

    BOOST_CHECK(ApproxEqual(Magnitude(Normal(Vector3(-1.f, .5f, -2.f))), 1.0));

    vec1 = Vector3(1);
    BOOST_CHECK(Normalize(vec1) == Vector3(.5773503f));

    vec1 = Vector3(3, 2.5, 1);
    BOOST_CHECK(IsUnitLength(Normal(vec1)));

    vec1 = Vector3(3, 2, 1);
    vec2 = Vector3(2, -4, 6);
    BOOST_CHECK(fabs(Dot(vec1, vec2) - 4) <= AcceptableMathDelta);

    vec2 = 10 * unitx;
    BOOST_CHECK(Projection(vec1, vec2) == 3 * unitx);

    vec2 = unity;
    BOOST_CHECK(Projection(vec1, vec2) == 2 * unity);

    vec2 = 20 * unitz;
    BOOST_CHECK(Projection(vec1, vec2) == unitz);
}
