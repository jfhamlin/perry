//! \file WorkerThreadPool_UnitTest.cpp
//! Contains a test suite for the WorkerThreadPool class and the related
//! TaskGroup class.

#include "Utility/WorkerThreadPool.h"
#include <boost/bind.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <memory>
#include <vector>

using namespace romulus;

void TestTask1(int* x)
{
    *x = 42;
}

BOOST_AUTO_TEST_CASE(TestTask)
{
    int testVar = 0;
    WorkerThreadPool pool1(1);
    WorkerThreadPool::taskid_t taskid =
            pool1.EnqueueTask(boost::bind(TestTask1, &testVar));
    while (!pool1.IsTaskComplete(taskid))
        ;
    BOOST_CHECK_EQUAL(testVar, 42);
}

BOOST_AUTO_TEST_CASE(TestMultiTask)
{
    int testVar1 = 0;
    int testVar2 = 0;
    int testVar3 = 0;
    int testVar4 = 0;
    WorkerThreadPool pool2(2);
    WorkerThreadPool::taskid_t taskid1 =
            pool2.EnqueueTask(boost::bind(TestTask1, &testVar1));
    WorkerThreadPool::taskid_t taskid2 =
            pool2.EnqueueTask(boost::bind(TestTask1, &testVar2));
    WorkerThreadPool::taskid_t taskid3 =
            pool2.EnqueueTask(boost::bind(TestTask1, &testVar3));
    WorkerThreadPool::taskid_t taskid4 =
            pool2.EnqueueTask(boost::bind(TestTask1, &testVar4));

    while (!pool2.IsTaskComplete(taskid1) || !pool2.IsTaskComplete(taskid2) ||
           !pool2.IsTaskComplete(taskid3) || !pool2.IsTaskComplete(taskid4))
        ;
    BOOST_CHECK_EQUAL(testVar1, 42);
    BOOST_CHECK_EQUAL(testVar2, 42);
    BOOST_CHECK_EQUAL(testVar3, 42);
    BOOST_CHECK_EQUAL(testVar4, 42);
}

void TestTask2(std::vector<int>* result, int x)
{
    result->push_back(x);
}

BOOST_AUTO_TEST_CASE(TestDependentTasks)
{
    std::vector<int> resultVec;
    WorkerThreadPool pool2(2);
    std::vector<WorkerThreadPool::taskid_t> dependencies;
    WorkerThreadPool::taskid_t taskid1 =
            pool2.EnqueueTask(boost::bind(TestTask2, &resultVec, 0),
                              dependencies.begin(), dependencies.end());
    dependencies.push_back(taskid1);
    WorkerThreadPool::taskid_t taskid2 =
            pool2.EnqueueTask(boost::bind(TestTask2, &resultVec, 1),
                              dependencies.begin(), dependencies.end());
    dependencies.push_back(taskid2);
    WorkerThreadPool::taskid_t taskid3 =
            pool2.EnqueueTask(boost::bind(TestTask2, &resultVec, 2),
                              dependencies.begin(), dependencies.end());
    dependencies.push_back(taskid3);
    WorkerThreadPool::taskid_t taskid4 =
            pool2.EnqueueTask(boost::bind(TestTask2, &resultVec, 3),
                              dependencies.begin(), dependencies.end());

    while (!pool2.IsTaskComplete(taskid1) || !pool2.IsTaskComplete(taskid2) ||
           !pool2.IsTaskComplete(taskid3) || !pool2.IsTaskComplete(taskid4))
        ;
    for (int i = 0; i < 4; ++i)
        BOOST_CHECK_EQUAL(resultVec[i], i);
}

BOOST_AUTO_TEST_CASE(TestTaskGroup)
{
    std::vector<int> resultVec;
    WorkerThreadPool pool2(2);
    std::auto_ptr<TaskGroup> taskGroup(pool2.CreateTaskGroup());
    std::vector<WorkerThreadPool::taskid_t> dependencies;
    WorkerThreadPool::taskid_t taskid1 =
            taskGroup->EnqueueTask(boost::bind(TestTask2, &resultVec, 0));
    dependencies.push_back(taskid1);
    WorkerThreadPool::taskid_t taskid2 =
            taskGroup->EnqueueTask(boost::bind(TestTask2, &resultVec, 1),
                                   dependencies.begin(), dependencies.end());
    dependencies.push_back(taskid2);
    WorkerThreadPool::taskid_t taskid3 =
            taskGroup->EnqueueTask(boost::bind(TestTask2, &resultVec, 2),
                                   dependencies.begin(), dependencies.end());
    dependencies.push_back(taskid3);
    taskGroup->EnqueueTask(boost::bind(TestTask2, &resultVec, 3),
                           dependencies.begin(), dependencies.end());

    taskGroup->WaitForTasks();

    BOOST_CHECK(taskGroup->TasksDone());
    BOOST_REQUIRE_EQUAL(resultVec.size(), 4u);
    for (int i = 0; i < 4; ++i)
        BOOST_CHECK_EQUAL(resultVec[i], i);
}
