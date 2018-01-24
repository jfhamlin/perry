#include "Utility/Timer.h"
#include "Platform/Platform.h"

namespace romulus
{
    Timer::Timer()
    {
        Reset();
    }

    void Timer::Reset()
    {
        m_lastTime = platform::GetTime();
    }

    double Timer::Update(const bool reset)
    {
        double time = platform::GetTime();
        double elapsedTime = time - m_lastTime;

        if (reset)
            m_lastTime = time;

        return elapsedTime;
    }
}
