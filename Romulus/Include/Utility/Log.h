#ifndef _LOG_H_
#define _LOG_H_

//! \file Log.h
//! Contains the declaration of the Log utility class.

#include <ostream>
#include <vector>
#include <sstream>
#include <boost/thread/recursive_mutex.hpp>

namespace romulus
{
//! Facilitates logging.
//! Handles logging of different level (severity) messages to different streams
//! which are associated with one or more message severity levels.
//! For Log objects shared between threads, instantiate a RAII Log::Lock object
//! with the shared Log object as the constructor parameter.  There is no
//! need to lock a Log object that is not shared between threads.
class Log
{
public:

    class Lock
    {
    public:

        inline Lock(Log& log):
            m_lock(log.m_mutex)
        {
        }

    private:

        boost::recursive_mutex::scoped_lock m_lock;
    };

    enum MessageLevel
    {
        MessageLevel_Info     = 1,
        MessageLevel_Warning  = 2,
        MessageLevel_Error    = 4,
        MessageLevel_Critical = 8,

        MessageLevel_All      = MessageLevel_Info | MessageLevel_Warning
        | MessageLevel_Error | MessageLevel_Critical
    };

    //! Log ctor.
    Log();
    //! Log dtor.
    //! Flushes the current buffer and releases sinks.
    ~Log();

    //! Register a logging sink with the log.
    //! \param sink - The sink to register.
    //! \param handleDeallocation - True to pass deletion responsibilites to the log.
    //! \param sinkMessageLevels - A bitfield containing message severity levels to associate with the sink.
    void RegisterSink(std::ostream* sink, const bool handleDeallocation,
                      const int sinkMessageLevels);
    //! Release any held sinks.
    void ClearSinks();

    //! Flushes the log buffer.
    void Flush();

    //! Write to the log.
    //! \param message - The message to write.
    //! \return A reference to the log.
    template <typename T>
    Log& operator<<(const T& message)
    {
        m_buffer << message;
        return *this;
    }

    //! Change the current message severity level.
    //! \param level - The level to switch to.
    //! \return A reference to the log.
    Log& operator<<(const MessageLevel& level)
    {
        ChangeLevel(level);
        return *this;
    }

private:

    struct SinkInfo
    {
        std::ostream* Sink;
        bool HandleDeallocation;
        int Levels;
    };
    typedef std::vector<SinkInfo> SinkList;

    void FlushBuffer();
    void ChangeLevel(const MessageLevel level);

    SinkList m_sinks;
    double m_creationTime;
    MessageLevel m_currentLevel;
    std::stringstream m_buffer;
    boost::recursive_mutex m_mutex;
};
}

#endif // _LOG_H_
