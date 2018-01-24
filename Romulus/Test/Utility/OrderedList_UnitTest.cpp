#include "Utility/OrderedList.h"
#include <boost/test/auto_unit_test.hpp>

//! \file OrderedList_UnitTest.cpp
//! Contains a test suite for the OrderedList data structure.

BOOST_AUTO_TEST_CASE(TestInsert)
{
    romulus::OrderedList<int> list;

    BOOST_CHECK_EQUAL(list.Size(), static_cast<unsigned int>(0));
    BOOST_CHECK(list.Empty());

    BOOST_CHECK_EQUAL(*(list.Insert(list.Begin(), 1)), 1);
    list.Insert(list.Begin(), 0);
    list.Insert(list.End(), 10);
    list.PushBack(200);
    list.PushFront(-10);
    list.Insert(list.Find(10), 9);

    BOOST_CHECK_EQUAL(list.Size(), static_cast<unsigned int>(6));
    BOOST_CHECK_EQUAL(list.Front(), -10);
    BOOST_CHECK_EQUAL(list.Back(), 200);
}

BOOST_AUTO_TEST_CASE(TestOrder)
{
    romulus::OrderedList<int> list;

    list.Insert(list.Begin(), 1);
    list.Insert(list.Begin(), 0);

    BOOST_CHECK(list.Order(0, 1));
    BOOST_CHECK(list.Order(list.Find(0), list.Find(1)));
    BOOST_CHECK(!list.Order(1, 0));
    BOOST_CHECK(!list.Order(list.Find(1), list.Find(0)));

    list.Insert(list.End(), 10);
    BOOST_CHECK(list.Order(0, 10));
    BOOST_CHECK(!list.Order(10, 0));

    list.Insert(++(list.Find(1)), 5);
    BOOST_CHECK(list.Order(1, 5));
    BOOST_CHECK(!list.Order(5, 1));

    list.Insert(list.Find(10), 7);
    BOOST_CHECK(list.Order(7, 10));
    BOOST_CHECK(!list.Order(10, 7));

    BOOST_CHECK(!list.Order(10, 10));

    // Check that false is returned for elements not in the list.
    BOOST_CHECK(!list.Order(1000, 10));
    BOOST_CHECK(!list.Order(10, 1000));
}

BOOST_AUTO_TEST_CASE(TestErase)
{
    romulus::OrderedList<int> list;

    list.Insert(list.Begin(), 1);
    list.Insert(list.Begin(), 0);
    list.Insert(list.End(), 10);
    list.PushBack(200);
    list.PushFront(-10);
    list.Insert(list.Find(10), 9);

    BOOST_CHECK_EQUAL(list.Size(), static_cast<unsigned int>(6));

    list.Erase(list.Find(10));

    BOOST_CHECK_EQUAL(list.Size(), static_cast<unsigned int>(5));
    BOOST_CHECK(list.Find(10) == list.End());
    BOOST_CHECK(list.Order(0, 9));

    list.PopFront();

    BOOST_CHECK_EQUAL(list.Size(), static_cast<unsigned int>(4));
    BOOST_CHECK(list.Find(-10) == list.End());
    BOOST_CHECK(list.Order(0, 9));

    list.PopBack();

    BOOST_CHECK_EQUAL(list.Size(), static_cast<unsigned int>(3));
    BOOST_CHECK(list.Find(200) == list.End());
    BOOST_CHECK(list.Order(0, 9));
}

BOOST_AUTO_TEST_CASE(TestIter)
{
    romulus::OrderedList<int> list;

    list.Insert(list.Begin(), 1);
    list.Insert(list.Begin(), 0);
    list.Insert(list.End(), 10);
    list.PushBack(200);
    list.PushFront(-10);
    list.Insert(list.Find(10), 9);

    int i = 0;
    for (romulus::OrderedList<int>::Iterator it = list.Begin();
         it != list.End();
         ++it, ++i)
    {
        switch (i)
        {
            case 0:
                BOOST_CHECK(*it == -10);
                break;
            case 1:
                BOOST_CHECK(*it == 0);
                break;
            case 2:
                BOOST_CHECK(*it == 1);
                break;
            case 3:
                BOOST_CHECK(*it == 9);
                break;
            case 4:
                BOOST_CHECK(*it == 10);
                break;
            case 5:
                BOOST_CHECK(*it == 200);
                break;
        }
    }

    i = 0;
    for (romulus::OrderedList<int>::ReverseIterator it = list.RBegin();
         it != list.REnd();
         ++it, ++i)
    {
        switch (i)
        {
            case 5:
                BOOST_CHECK(*it == -10);
                break;
            case 4:
                BOOST_CHECK(*it == 0);
                break;
            case 3:
                BOOST_CHECK(*it == 1);
                break;
            case 2:
                BOOST_CHECK(*it == 9);
                break;
            case 1:
                BOOST_CHECK(*it == 10);
                break;
            case 0:
                BOOST_CHECK(*it == 200);
                break;
        }
    }
}

BOOST_AUTO_TEST_CASE(TestAccessors)
{
    romulus::OrderedList<int> list;

    list.Insert(list.Begin(), 1);
    list.Insert(list.Begin(), 0);
    list.Insert(list.End(), 10);
    list.PushBack(200);
    list.PushFront(-10);
    list.Insert(list.Find(10), 9);

    BOOST_CHECK(list.Front() == -10);
    BOOST_CHECK(list.Back() == 200);
    BOOST_CHECK(*list.Find(10) == 10);
}

BOOST_AUTO_TEST_CASE(StressTest)
{
    romulus::OrderedList<int> list;

    // Initialize the list with a single record.
    list.PushBack(0);

    // We insert 11000 elements with different policies, triggering relabeling
    // of records. We then check that the expected order is preserved. We
    // keep the records in numerical order, so the expected order is clear.
    for (int i = 0; i < 11000; ++ i)
    {
        switch (i % 4)
        {
            case 0:
                list.PushFront(i + 1);
                break;
            case 1:
                list.PushBack(i + 1);
                break;
            case 2:
                list.Insert(list.Find(i / 2 + i / 4), i + 1);
                break;
            case 3:
                list.Insert(list.Find(i / 2 - i / 4), i + 1);
                break;
        }
    }

    BOOST_CHECK(list.Size() == 11001);

    for (romulus::OrderedList<int>::Iterator it = ++list.Begin();
         it != list.End(); ++it)
    {
        romulus::OrderedList<int>::Iterator prev = it;
        --prev;
        BOOST_CHECK(list.Order(prev, it));
    }
}

BOOST_AUTO_TEST_CASE(EdgeCaseTest)
{
    romulus::OrderedList<int> list;
    // test that reinsertions do not duplicate records.
    list.PushBack(0);
    list.PushFront(0);
    BOOST_CHECK(list.Size() == 1);
    BOOST_CHECK(list.Find(0) != list.End());

    list.Erase(list.End()); // This should not complain.
}
