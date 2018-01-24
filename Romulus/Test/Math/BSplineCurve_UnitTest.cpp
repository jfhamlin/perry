#include "Math/BSplineCurve.h"
#include <boost/test/auto_unit_test.hpp>

using namespace romulus::math;

BOOST_AUTO_TEST_CASE(TestBSpline)
{
    BSplineCurve c;
    c.SetDegree(3);
    Vector3 p(1, 2, 3);
    for (uint_t i = 0; i < 15; ++i)
        c.PushBack(p);

    BOOST_CHECK(c.Sample(0) == p);

    c.PushBack(Vector3(1.5, 1.5, 1.5));

    c.SetClosed(true);
    BOOST_CHECK(c.Sample(0) == c.Sample(1));
}

BOOST_AUTO_TEST_CASE(TestBSplineClosed)
{
    BSplineCurve c;
    c.SetDegree(3);
    c.PushBack(Vector3(0, 0, 0));
    c.PushBack(Vector3(0, 0, 1));
    c.PushBack(Vector3(0, 1, 0));
    c.PushBack(Vector3(0, 1, 1));
    c.PushBack(Vector3(1, 0, 0));
    c.PushBack(Vector3(1, 0, 1));
    c.PushBack(Vector3(1, 1, 0));
    c.PushBack(Vector3(1, 1, 1));

    c.SetClosed(true);
    BOOST_CHECK(c.Sample(0) == c.Sample(1));
}
