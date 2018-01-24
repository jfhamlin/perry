#ifndef _PLATFORMIWINDOW_H_
#define _PLATFORMIWINDOW_H_

//! \file IWindow.h
//! Contains the window interface.

#include "Render/IRenderDevice.h"
#include "Platform/IInputServer.h"

namespace romulus
{
namespace platform
{

class InvalidWindowPreferences : public std::exception
{
};

//! Window properties.
struct WindowPreferences
{
public:

    WindowPreferences():
        WindowWidth(800), WindowHeight(600), Fullscreen(false),
        HideCursor(false), ConstrainCursor(false)
    {
    }

    WindowPreferences(int width, int height, bool fullscreen):
        WindowWidth(width), WindowHeight(height), Fullscreen(fullscreen),
        HideCursor(false), ConstrainCursor(false)
    {
    }

    WindowPreferences(int width, int height, bool fullscreen,
                      bool hideCursor, bool constrainCursor):
        WindowWidth(width), WindowHeight(height), Fullscreen(fullscreen),
        HideCursor(hideCursor), ConstrainCursor(constrainCursor)
    {
    }

    WindowPreferences& operator=(const WindowPreferences& rhs)
    {
        WindowWidth = rhs.WindowWidth;
        WindowHeight = rhs.WindowHeight;
        Fullscreen = rhs.Fullscreen;
        return *this;
    }

    //! Display settings.
    int WindowWidth;
    int WindowHeight;
    bool Fullscreen;

    //! Input settings.
    bool HideCursor;
    bool ConstrainCursor;
};

//! Represents a window.
//! Windows are used for rendering and input gathering.
class IWindow
{
public:

    //! Initialize the window.
    //! Throws InvalidWindowPreferences.
    //! \param preferences - The requested window parameters.
    IWindow(const WindowPreferences& preferences): m_preferences(preferences) { }
    virtual ~IWindow() { }

    //! Retrieve the associated render device.
    virtual render::IRenderDevice* RenderDevice() = 0;
    //! Retrieve the associated input server.
    virtual IInputServer* InputServer() = 0;

    //! The window preferences.
    const WindowPreferences& Preferences() const { return m_preferences; }

    virtual bool QuitRequested() const = 0;

protected:

    WindowPreferences m_preferences;
};

}
}

#endif // _PLATFORMIWINDOW_H_
