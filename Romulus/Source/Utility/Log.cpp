#include "Utility/Log.h"
#include "Platform/Platform.h"
#include <string>
#include <assert.h>

namespace romulus
{
Log::Log():
    m_creationTime(platform::GetTime()), m_currentLevel(MessageLevel_Info)
{
}

Log::~Log()
{
    FlushBuffer();
    ClearSinks();
}

void Log::RegisterSink(std::ostream* sink, const bool handleDeallocation,
                       const int sinkMessageLevels)
{
    assert(sink != 0);

    SinkInfo info;
    info.Sink = sink;
    info.HandleDeallocation = handleDeallocation;
    info.Levels = sinkMessageLevels;

    m_sinks.push_back(info);
}

void Log::ClearSinks()
{
    for (SinkList::const_iterator iter = m_sinks.begin(); iter != m_sinks.end();
         ++iter)
        if (iter->HandleDeallocation)
            delete iter->Sink;

    m_sinks.clear();
}

void Log::Flush()
{
    FlushBuffer();
}

void Log::FlushBuffer()
{
    const std::string& bufferString = m_buffer.str();
    for (SinkList::const_iterator iter = m_sinks.begin(); iter != m_sinks.end();
         ++iter)
        if (iter->Levels & m_currentLevel) // Write to this sink.
        {
            iter->Sink->write(bufferString.c_str(), static_cast<std::streamsize>(bufferString.size()));
            iter->Sink->flush();
        }

    // Clear the buffer.
    // Probably a better way to do this.
    m_buffer.str("");
}

void Log::ChangeLevel(const MessageLevel level)
{
    if (m_currentLevel == level) // no op.
        return;

    FlushBuffer();

    m_currentLevel = level;
}
}
