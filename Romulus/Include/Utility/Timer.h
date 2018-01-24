#ifndef _TIMER_H_
#define _TIMER_H_

//! \file Timer.h
//! The Timer utility class declaration.

namespace romulus
{
    //! A simple utility timer.
    class Timer
    {
    public:

        Timer();

        //! Reset the timer to the current time.
        void Reset();

        //! Update the timer.
        //! \param reset - True to reset the timer.
        //! \return The elapsed time sine the last reset.
        double Update(const bool reset);

    private:

        double m_lastTime;
    };
}

#endif // _TIMER_H_
