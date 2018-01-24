#include "Platform/SDL/Window.h"
#include "Platform/SDL/InputServer.h"
#include "Render/OpenGL/RenderDevice.h"
#include "Utility/Assertions.h"
#include <SDL/SDL.h>
#include <iostream>
#include <fstream>

namespace romulus
{
namespace platform
{
namespace sdl
{

Window::Window(const WindowPreferences& preferences):
    platform::IWindow(preferences)
{
    m_log.RegisterSink(&std::cout, false, Log::MessageLevel_Warning);
    m_log.RegisterSink(new std::ofstream("SDL_Window.log"), true,
                       Log::MessageLevel_All);

    SDL_Init(SDL_INIT_VIDEO);

    const int red = 8;
    const int green = 8;
    const int blue = 8;
    const int alpha = 8;
    const int depth = 24;
    const int stencil = 8;

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, red);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, green);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, blue);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, alpha);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depth);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, stencil);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    int mode = SDL_OPENGL | (preferences.Fullscreen ? SDL_FULLSCREEN : 0);

    bool success = SDL_SetVideoMode(preferences.WindowWidth,
        preferences.WindowHeight,
        red + green + blue + alpha, mode);
    if (!success)
    {
        m_log << Log::MessageLevel_Error <<
                "Failed to open window with parameters: " <<
                "width=" << preferences.WindowWidth <<
                ", height=" << preferences.WindowHeight <<
                ", bpf=" << red + green + blue + alpha << ", depth=" << depth <<
                ", stencil=" << stencil << ", fullscreen=" <<
                (preferences.Fullscreen ? "true" : "false") << ".\n";
        throw InvalidWindowPreferences();
    }
    SDL_WM_SetCaption("Solstice", 0);

    m_log << Log::MessageLevel_Info << "Window opened with parameters: " <<
            "width=" << preferences.WindowWidth << ", height=" <<
            preferences.WindowHeight        << ", bpf=" <<
            red + green + blue + alpha << ", depth=" << depth << ", stencil=" <<
            stencil << ", fullscreen=" <<
            (preferences.Fullscreen ? "true" : "false") << ".\n";

    m_renderDevice.reset(
            new render::opengl::RenderDevice(preferences.WindowWidth,
                                             preferences.WindowHeight));
    m_inputServer.reset(
            new platform::sdl::InputServer(m_log, preferences.HideCursor,
                                           preferences.ConstrainCursor));
}

Window::~Window()
{
    // Unfortunately, we have to manually terminate these objects
    // prior to terminating SDL.
    m_renderDevice.reset(reinterpret_cast<render::IRenderDevice*>(0));
    m_inputServer.reset(reinterpret_cast<IInputServer*>(0));

    SDL_Quit();
}

render::IRenderDevice* Window::RenderDevice()
{
    return m_renderDevice.get();
}

IInputServer* Window::InputServer()
{
    return m_inputServer.get();
}

bool Window::QuitRequested() const
{
    return static_cast<sdl::InputServer*>(m_inputServer.get())->QuitRequested();
}

}
}
}
