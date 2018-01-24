#ifndef _PLATFORMSDLWINDOW_H_
#define _PLATFORMSDLWINDOW_H_

#include "Platform/IWindow.h"
#include "Utility/Log.h"
#include <boost/scoped_ptr.hpp>

namespace romulus
{
namespace platform
{
namespace sdl
{

//! SDL implementation of platform::Window
class Window : public platform::IWindow
{
public:

    Window(const WindowPreferences& preferences);
    virtual ~Window();

    virtual render::IRenderDevice* RenderDevice();
    virtual IInputServer* InputServer();

    virtual bool QuitRequested() const;

private:

    boost::scoped_ptr<render::IRenderDevice> m_renderDevice;
    boost::scoped_ptr<IInputServer> m_inputServer;
    Log m_log;
};

}
}
}

#endif // _PLATFORMSDLWINDOW_H_
