#include "Utility/WorkerThreadPool.h"
#include <boost/bind.hpp>

namespace romulus
{

void WorkerFunction(WorkerThreadPool* threadPool)
{
    ASSERT(threadPool);
    WorkerThreadPool::TaskInfo task;
    while (threadPool->GetNextTask(task))
    {
        // If we have a task, execute it, then notify the thread pool that
        // it was completed.
        task.second();
        threadPool->OnTaskComplete(task.first);
    }
}

WorkerThreadPool::WorkerThreadPool(uint_t numThreads):
    m_nextTaskID(0), m_workersStopWorking(false)
{
    ASSERT(numThreads > 0);

    boost::function0<void> workFunc(
            boost::bind(WorkerFunction, this));
    for (uint_t i = 0; i < numThreads; ++i)
        m_workers.create_thread(workFunc);
}

WorkerThreadPool::~WorkerThreadPool()
{
    m_workersStopWorking = true;
    {
        boost::mutex::scoped_lock lock(m_taskQueueMutex);
        m_workAvailableCondition.notify_all();
    }
    m_workers.join_all();
}

TaskGroup* WorkerThreadPool::CreateTaskGroup()
{
    return new TaskGroup(this);
}

WorkerThreadPool::taskid_t WorkerThreadPool::EnqueueTask(
        const boost::function0<void>& task)
{
    taskid_t id = GetNewTaskID();
    InsertUnfinishedTaskID(id);

    boost::mutex::scoped_lock lock(m_taskQueueMutex);
    m_taskQueue.push(std::make_pair(id, task));
    m_workAvailableCondition.notify_one();

    return id;
}

bool WorkerThreadPool::GetNextTask(TaskInfo& task)
{
    boost::mutex::scoped_lock lock(m_taskQueueMutex);
    for (;;)
    {
        if (m_taskQueue.size())
        {
            task = m_taskQueue.front();
            m_taskQueue.pop();
            return true;
        }
        else if (m_workersStopWorking)
        {
            return false;
        }
        else
        {
            m_workAvailableCondition.wait(lock);
        }
    }
}

void WorkerThreadPool::OnTaskComplete(taskid_t taskid)
{
    EraseUnfinishedTaskID(taskid);

    boost::mutex::scoped_lock lock(m_dependencyMutex);

    DependencyMap::iterator dependentsIt = m_dependentsMap.find(taskid);
    if (dependentsIt != m_dependentsMap.end())
    {
        std::set<taskid_t>& dependents = dependentsIt->second;

        std::set<taskid_t>::const_iterator dependentsEnd = dependents.end();
        std::set<taskid_t>::iterator dependentIt = dependents.begin();
        while (dependentIt != dependentsEnd)
        {
            DependencyMap::iterator dependenciesIt =
                    m_dependenciesMap.find(*dependentIt);
            ASSERT(dependenciesIt != m_dependenciesMap.end());

            std::set<taskid_t>& dependencies = dependenciesIt->second;

            dependencies.erase(taskid);
            if (dependencies.empty())
            {
                EnqueueWaitingTask(*dependentIt);
                m_dependenciesMap.erase(dependenciesIt);
                std::set<taskid_t>::iterator deleteIt = dependentIt++;
                dependents.erase(deleteIt);
            }
            else
            {
                ++dependentIt;
            }
        }
    }
}

WorkerThreadPool::taskid_t TaskGroup::EnqueueTask(
        const boost::function0<void>& task)
{
    WorkerThreadPool::taskid_t taskid = m_pool->EnqueueTask(task);
    m_tasks.insert(taskid);
    return taskid;
}

bool TaskGroup::TasksDone()
{
    TaskIDSet::iterator end = m_tasks.end();
    TaskIDSet::iterator it = m_tasks.begin();
    while (it != end)
    {
        if (m_pool->IsTaskComplete(*it))
        {
            TaskIDSet::iterator deleteIt = it;
            ++it;
            m_tasks.erase(deleteIt);
        }
        else
        {
            return false;
        }
    }
    return true;
}

void TaskGroup::WaitForTasks()
{
    //! \todo We could do better and set up a condition variable, i.e.:
    // if (!TasksDone()) tasksDoneCondition.wait();
    while (!TasksDone())
        boost::thread::yield();
}

} // namespace romulus
