#ifndef _WORKERTHREADPOOL_H_
#define _WORKERTHREADPOOL_H_

//! \file WorkerThreadPool.h
//! Contains the definition of WorkerThreadPool, a class for the distribution
//! of tasks among worker threads.

#include "Core/Types.h"
#include "Utility/Assertions.h"
#include "Utility/Common.h"
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <map>
#include <queue>
#include <set>
#include <utility>
#include <vector>

namespace romulus
{

class TaskGroup;

//! A class that distributes tasks among worker threads. If a task is declared
//! to have dependencies on previously enqueued tasks, it will not be started
//! until those tasks have completed.
class WorkerThreadPool
{
PROHIBIT_COPYING(WorkerThreadPool);
public:
    typedef uint_t taskid_t;

    WorkerThreadPool(uint_t numThreads);

    //! Tells all worker threads to terminate, and then waits for workers to
    //! finish their current tasks.
    ~WorkerThreadPool();

    //! Creates a TaskGroup object that serves as an interface for creating
    //! tasks. The caller takes ownership of the object.
    //! \return A pointer to the new TaskGroup.
    TaskGroup* CreateTaskGroup();

    //! Puts a task on the queue.
    //! \param task - a nullary function returning void which performs the
    //!               desired task.
    //! \return The taskid_t designating this task for this WorkerThreadPool.
    taskid_t EnqueueTask(const boost::function0<void>& task);

    //! Puts a task on the queue. The task will not be executed until after
    //! the tasks designated by the taskid_ts in the dependency sequence have
    //! completed.
    //! \param task - a nullary function returning void which performs the
    //!               desired task.
    //! \param dependenciesStart - an iterator to the start of a sequence of
    //!                            taskid_ts of the dependencies.
    //! \param dependenciesEnd - an iterator one past the end of the sequence
    //!                          of taskid_ts of the dependencies.
    //! \return The taskid_t designating this task for this WorkerThreadPool.
    template <typename TaskIDIterator>
    taskid_t EnqueueTask(const boost::function0<void>& task,
                       TaskIDIterator dependenciesStart,
                       const TaskIDIterator dependenciesEnd);

    //! \return True if the task denoted by taskid has completed. Also returns
    //!         true if there is no such task.
    inline bool IsTaskComplete(taskid_t taskid)
    {
        boost::mutex::scoped_lock lock(m_taskIDSetMutex);
        return !m_unfinishedTasks.count(taskid);
    }

private:

    typedef std::pair<taskid_t, boost::function0<void> > TaskInfo;

    friend class TaskGroup;
    friend void WorkerFunction(WorkerThreadPool* threadPool);

    inline taskid_t GetNewTaskID()
    {
        boost::mutex::scoped_lock lock(m_taskIDMutex);
        return m_nextTaskID++;
    }

    //! Atomically retrieves the front of the queue and pops it if the queue
    //! is not empty. Blocks if the queue is empty until either a task is
    //! available or there will be no more tasks and workers should terminate.
    //! \return true if a task was retrieved and work should continue, or false
    //!         if work should stop.
    bool GetNextTask(TaskInfo& task);

    //! Iterates over the dependents of a task. If taskid denotes the last
    //! dependency for a task, then the task is put on the queue.
    void OnTaskComplete(taskid_t taskid);

    //! Remove a waiting task from the waiting task set, and put it on the
    //! queue.
    inline void EnqueueWaitingTask(taskid_t taskid)
    {
        // Protected by m_dependencyMutex in OnTaskComplete()
        TaskMap::iterator it = m_waitingTasks.find(taskid);
        ASSERT(it != m_waitingTasks.end());

        // If changing code, be careful of introducing the possibility of
        // deadlock here. m_dependencyMutex will already be held, so if some
        // other method locks m_taskQueueMutex followed by m_dependencyMutex
        // trouble could arise.
        boost::mutex::scoped_lock lock(m_taskQueueMutex);
        m_taskQueue.push(*it);
        m_waitingTasks.erase(it);
        m_workAvailableCondition.notify_one();
    }

    inline void InsertUnfinishedTaskID(taskid_t taskid)
    {
        boost::mutex::scoped_lock lock(m_taskIDSetMutex);
        m_unfinishedTasks.insert(taskid);
    }

    inline void EraseUnfinishedTaskID(taskid_t taskid)
    {
        boost::mutex::scoped_lock lock(m_taskIDSetMutex);
        m_unfinishedTasks.erase(taskid);
    }

    taskid_t m_nextTaskID;
    boost::mutex m_taskIDMutex;

    std::set<taskid_t> m_unfinishedTasks;
    boost::mutex m_taskIDSetMutex;

    std::queue<TaskInfo> m_taskQueue;
    boost::mutex m_taskQueueMutex;

    typedef std::map<taskid_t, boost::function0<void> > TaskMap;
    typedef std::map<taskid_t, std::set<taskid_t> > DependencyMap;

    TaskMap m_waitingTasks;
    DependencyMap m_dependentsMap;
    DependencyMap m_dependenciesMap;
    boost::mutex m_dependencyMutex;

    boost::thread_group m_workers;

    bool m_workersStopWorking;

    boost::condition m_workAvailableCondition;
};

class TaskGroup
{
PROHIBIT_COPYING(TaskGroup);
public:
    //! Puts a task on the queue.
    //! \param task - a nullary function returning void which performs the
    //!               desired task.
    //! \return The taskid_t designating this task for this WorkerThreadPool.
    WorkerThreadPool::taskid_t EnqueueTask(const boost::function0<void>& task);

    //! Puts a task on the queue. The task will not be executed until after
    //! the tasks designated by the taskid_ts in the dependency sequence have
    //! completed. This method is not threadsafe, since a TaskGroup object
    //! shouldn't be used on more than one thread at a time anyway.
    //! \param task - a nullary function returning void which performs the
    //!               desired task.
    //! \param dependenciesStart - an iterator to the start of a sequence of
    //!                            taskid_ts of the dependencies.
    //! \param dependenciesEnd - an iterator one past the end of the sequence
    //!                          of taskid_ts of the dependencies.
    //! \return The taskid_t designating this task for this WorkerThreadPool.
    template <typename TaskIDIterator>
    WorkerThreadPool::taskid_t EnqueueTask(
            const boost::function0<void>& task,
            TaskIDIterator dependenciesStart,
            const TaskIDIterator dependenciesEnd);

    //! \return True if all tasks created through this TaskGroup object have
    //!         completed.
    bool TasksDone();

    //! Blocks the current thread until all tasks created through this
    //! TaskGroup have completed.
    //! \todo Implement this more efficiently by making WorkerThreadPool aware
    //!       of TaskGroups. We should sleep and have the WorkerThreadPool
    //!       wake us up when the tasks are completed.
    void WaitForTasks();

private:
    friend class WorkerThreadPool;
    TaskGroup(WorkerThreadPool* pool): m_pool(pool) { ASSERT(pool); }

    WorkerThreadPool* m_pool;

    typedef std::set<WorkerThreadPool::taskid_t> TaskIDSet;

    TaskIDSet m_tasks;
};

//! Implementation of template member EnqueueTask.
template <typename TaskIDIterator>
WorkerThreadPool::taskid_t WorkerThreadPool::EnqueueTask(
        const boost::function0<void>& task, TaskIDIterator dependenciesStart,
        const TaskIDIterator dependenciesEnd)
{
    taskid_t id = GetNewTaskID();
    InsertUnfinishedTaskID(id);

    boost::mutex::scoped_lock lock(m_dependencyMutex);
    bool shouldWait = false;
    for (; dependenciesStart != dependenciesEnd; ++dependenciesStart)
        if (!IsTaskComplete(*dependenciesStart))
        {
            shouldWait = true;
            m_dependentsMap[*dependenciesStart].insert(id);
            m_dependenciesMap[id].insert(*dependenciesStart);
        }

    if (shouldWait)
    {
        m_waitingTasks.insert(make_pair(id, task));
    }
    else
    {
        boost::mutex::scoped_lock lock(m_taskQueueMutex);
        m_taskQueue.push(std::make_pair(id, task));
        m_workAvailableCondition.notify_one();
    }

    return id;
}

//! Implementation of template member EnqueueTask.
template <typename TaskIDIterator>
WorkerThreadPool::taskid_t TaskGroup::EnqueueTask(
        const boost::function0<void>& task, TaskIDIterator dependenciesStart,
        const TaskIDIterator dependenciesEnd)
{
    WorkerThreadPool::taskid_t taskid =
            m_pool->EnqueueTask(task, dependenciesStart, dependenciesEnd);
    m_tasks.insert(taskid);
    return taskid;
}

} // namespace romulus

#endif // _WORKERTHREADPOOL_H_
