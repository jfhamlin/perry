#ifndef _PLATFORMBUFFEREDINPUTLISTENER_H_
#define _PLATFORMBUFFEREDINPUTLISTENER_H_

//! \file BufferedInputListener.h
//! Contains the declaration of the buffered input listener abstract class.

#include "Platform/IInputServer.h"
#include <boost/thread/mutex.hpp>
#include <vector>

namespace romulus
{
namespace platform
{

//! Thread safe input listener.
//! Subclasses of BufferedInputListener need to implement OnKeyEvent() and
//! OnMouseEvent().  To process events, call PumpEvents().
class BufferedInputListener : public IInputListener
{
public:

    BufferedInputListener();
    virtual ~BufferedInputListener();

    virtual void HandleEvent(Key key, KeyState state);
    virtual void HandleEvent(MouseButton button, ButtonState state);
    virtual void HandleEvent(const Position& absolutePosition,
                             const Position& relativePosition);

protected:

    struct KeyEvent
    {
        Key KeySym;
        KeyState State;
    };

    struct MouseEvent
    {
        MouseButton Button;
        ButtonState State;
    };

    struct MouseState
    {
        Position AbsolutePosition;
        Position RelativePosition;
    };

    virtual void OnKeyEvent(const KeyEvent& event) { }
    virtual void OnMouseEvent(const MouseEvent& event) { }
    virtual void OnMouseMotion(const MouseState& event) { }

    void PumpEvents();

private:

    typedef std::vector<KeyEvent> KeyEventCollection;
    typedef std::vector<MouseEvent> MouseEventCollection;

    KeyEventCollection m_keyEvents;
    MouseEventCollection m_mouseEvents;
    MouseState m_mouseState;
    boost::mutex m_lock;

};

}
}

#endif //  _PLATFORMBUFFEREDINPUTLISTENER_H_
