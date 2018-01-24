#include "Platform/BufferedInputListener.h"
#include <boost/bind.hpp>
#include <algorithm>

namespace romulus
{
namespace platform
{

typedef boost::mutex::scoped_lock Lock;

BufferedInputListener::BufferedInputListener()
{
}

BufferedInputListener::~BufferedInputListener()
{
}

void BufferedInputListener::HandleEvent(Key key, KeyState state)
{
    Lock lock(m_lock);

    KeyEvent event;
    event.KeySym = key;
    event.State = state;
    m_keyEvents.push_back(event);
}

void BufferedInputListener::HandleEvent(MouseButton button, ButtonState state)
{
    Lock lock(m_lock);

    MouseEvent event;
    event.Button = button;
    event.State = state;
    m_mouseEvents.push_back(event);
}

void BufferedInputListener::HandleEvent(const Position& absolutePosition,
                                        const Position& relativePosition)
{
    Lock lock(m_lock);

    m_mouseState.AbsolutePosition = absolutePosition;
    m_mouseState.RelativePosition = relativePosition;
}

void BufferedInputListener::PumpEvents()
{
    Lock lock(m_lock);

    std::for_each(m_keyEvents.begin(), m_keyEvents.end(),
                  boost::bind(&BufferedInputListener::OnKeyEvent, this, _1));

    std::for_each(m_mouseEvents.begin(), m_mouseEvents.end(),
                  boost::bind(&BufferedInputListener::OnMouseEvent, this, _1));

    OnMouseMotion(m_mouseState);

    m_mouseState.RelativePosition = Position(0, 0);
    m_keyEvents.clear();
    m_mouseEvents.clear();
}

}
}
