#ifndef _PLATFORMSDLINPUTSERVER_H_
#define _PLATFORMSDLINPUTSERVER_H_

#include "Core/Types.h"
#include "Platform/IInputServer.h"
#include "Utility/Log.h"
#include <vector>

namespace romulus
{
namespace platform
{
namespace sdl
{

class InputServer : public IInputServer
{
public:

    InputServer(Log& log, bool hideCursor, bool constrainCursor);
    virtual ~InputServer();

    virtual void RegisterListener(IInputListener* listener);
    virtual void UnregisterListener(IInputListener* listener);

    virtual void Update();
    virtual KeyState QueryKeyState(Key key);
    virtual Position QueryMousePosition();
    virtual ButtonState QueryMouseButtonState(MouseButton button);

    inline bool QuitRequested() const { return m_quit; }

private:

    typedef std::vector<IInputListener*> ListenerCollection;

    Log& m_log;
    ubyte_t* m_keyState;

    bool m_hideCursor;
    bool m_constrainCursor;

    bool m_haveMouseFocus;
    ubyte_t m_mouseButtonState;
    Position m_absoluteMousePosition;

    ListenerCollection m_listeners;
    bool m_quit;
};

}
}
}

#endif // _PLATFORMSDLINPUTSERVER_H_
