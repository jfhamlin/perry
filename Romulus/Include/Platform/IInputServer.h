#ifndef _PLATFORMIINPUTSERVER_H_
#define _PLATFORMIINPUTSERVER_H_

//! \file IInputServer.h
//! Contains input server interface.

#include "Math/Vector.h"

namespace romulus
{
namespace platform
{

//! Key symbols.
enum Key
{
    Key_A = 0,
    Key_B,
    Key_C,
    Key_D,
    Key_E,
    Key_F,
    Key_G,
    Key_H,
    Key_I,
    Key_J,
    Key_K,
    Key_L,
    Key_M,
    Key_N,
    Key_O,
    Key_P,
    Key_Q,
    Key_R,
    Key_S,
    Key_T,
    Key_U,
    Key_V,
    Key_W,
    Key_X,
    Key_Y,
    Key_Z,

    Key_Enter,
    Key_Escape,
    Key_Space,

    Key_Right,
    Key_Left,
    Key_Up,
    Key_Down,

    Key_LeftShift,
    Key_RightShift,
    Key_LeftControl,
    Key_RightControl,
    Key_LeftAlt,
    Key_RightAlt,

    Key_Unknown
};

//! State of a key.
enum KeyState
{
    KeyState_Up = 0,
    KeyState_Down
};

//! Mouse button sybmols.
enum MouseButton
{
    MouseButton_Left,
    MouseButton_Right,
    MouseButton_Middle,
    MouseButton_WheelDown,
    MouseButton_WheelUp,

    MouseButton_Unknown
};

//! State of a mouse button.
enum ButtonState
{
    Button_Up = 0,
    Button_Down = 1
};

//! A position of the mouse cursor.
typedef math::Vector<2, int> Position;

//! Listens to user input events.
class IInputListener
{
public:

    IInputListener() { }
    virtual ~IInputListener() { }

    //! Handle a keyboard event.
    //! \param key - The key which triggered this event.
    //! \param state - The new key state.
    virtual void HandleEvent(Key key, KeyState state)
    {
    }

    //! Handle a mouse move event.
    //! \param absolutePosition - The absolute position.
    //! \param relativePosition - The position delta, since last move,
    //! of the mouse cursor.
    virtual void HandleEvent(const Position& absolutePosition,
                             const Position& relativePosition)
    {
    }

    //! Handle a mouse button event.
    //! \param button - The button which triggered this event.
    //! \param state - The new button state.
    virtual void HandleEvent(MouseButton button, ButtonState state)
    {
    }
};

//! The input server tracks user input and dispatches input events.
class IInputServer
{
public:

    IInputServer() { }
    virtual ~IInputServer() { }

    //! Update the server with new input.
    virtual void Update() = 0;

    //! Register a listener.
    //! \param listener - The listener to register.
    virtual void RegisterListener(IInputListener* listener) = 0;
    //! Unregister a listener.
    //! \param listener - The listener to unregister.
    virtual void UnregisterListener(IInputListener* listener) = 0;

    //! Query for a key's state.
    //! \param key - The key whose state to query.
    //! \return The key state.
    virtual KeyState QueryKeyState(Key key) = 0;
    //! Query for the mouse cursor's absolute position.
    //! \return The absolute mouse cursor position.
    virtual Position QueryMousePosition() = 0;
    //! Query for a mouse button's state.
    //! \param button - The mouse button whose sate to query.
    //! \return The button state.
    virtual ButtonState QueryMouseButtonState(MouseButton button) = 0;
};

}
}

#endif // _PLATFORMIINPUTSERVER_H_
