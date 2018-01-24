#include "Platform/SDL/InputServer.h"
#include "Utility/Assertions.h"
#include <SDL/SDL.h>
#include <algorithm>

namespace romulus
{
namespace platform
{
namespace sdl
{

namespace
{

const SDLKey TranslateKey(Key key)
{
    switch (key)
    {
        case Key_Enter:
            return SDLK_RETURN;
            break;

        case Key_Escape:
            return SDLK_ESCAPE;
            break;

        case Key_Space:
            return SDLK_SPACE;
            break;

        case Key_LeftShift:
            return SDLK_LSHIFT;
            break;

        case Key_RightShift:
            return SDLK_RSHIFT;
            break;

        case Key_LeftControl:
            return SDLK_LCTRL;
            break;

        case Key_RightControl:
            return SDLK_RCTRL;
            break;

        case Key_LeftAlt:
            return SDLK_LALT;
            break;

        case Key_RightAlt:
            return SDLK_RALT;
            break;

        case Key_Right:
            return SDLK_RIGHT;
            break;

        case Key_Left:
            return SDLK_LEFT;
            break;

        case Key_Up:
            return SDLK_UP;
            break;

        case Key_Down:
            return SDLK_DOWN;
            break;

        case Key_A:
        case Key_B:
        case Key_C:
        case Key_D:
        case Key_E:
        case Key_F:
        case Key_G:
        case Key_H:
        case Key_I:
        case Key_J:
        case Key_K:
        case Key_L:
        case Key_M:
        case Key_N:
        case Key_O:
        case Key_P:
        case Key_Q:
        case Key_R:
        case Key_S:
        case Key_T:
        case Key_U:
        case Key_V:
        case Key_W:
        case Key_X:
        case Key_Y:
        case Key_Z:
            return (SDLKey)(SDLK_a + static_cast<const SDLKey>(key - Key_A));
            break;

        default:
            return SDLK_EURO;
            break;
    }
}

Key TranslateKey(SDLKey key)
{
    switch (key)
    {
        case SDLK_a:
        case SDLK_b:
        case SDLK_c:
        case SDLK_d:
        case SDLK_e:
        case SDLK_f:
        case SDLK_g:
        case SDLK_h:
        case SDLK_i:
        case SDLK_j:
        case SDLK_k:
        case SDLK_l:
        case SDLK_m:
        case SDLK_n:
        case SDLK_o:
        case SDLK_p:
        case SDLK_q:
        case SDLK_r:
        case SDLK_s:
        case SDLK_t:
        case SDLK_u:
        case SDLK_v:
        case SDLK_w:
        case SDLK_x:
        case SDLK_y:
        case SDLK_z:
            return static_cast<Key>(Key_A + (key - SDLK_a));
            break;

        case SDLK_ESCAPE:
            return Key_Escape;
            break;

        case SDLK_RETURN:
            return Key_Enter;
            break;

        case SDLK_SPACE:
            return Key_Space;
            break;

        case SDLK_LSHIFT:
            return Key_LeftShift;
            break;

        case SDLK_RSHIFT:
            return Key_RightShift;
            break;

        case SDLK_LALT:
            return Key_LeftAlt;
            break;

        case SDLK_RALT:
            return Key_RightAlt;
            break;

        case SDLK_LCTRL:
            return Key_LeftControl;
            break;

        case SDLK_RCTRL:
            return Key_RightControl;
            break;

        case SDLK_RIGHT:
            return Key_Right;
            break;

        case SDLK_LEFT:
            return Key_Left;
            break;

        case SDLK_UP:
            return Key_Up;
            break;

        case SDLK_DOWN:
            return Key_Down;
            break;

        default:
            return Key_Unknown;
            break;
    }
}

MouseButton TranslateMouseButton(Uint8 button)
{
    switch (button)
    {
        case SDL_BUTTON_LEFT:
            return MouseButton_Left;
            break;

        case SDL_BUTTON_RIGHT:
            return MouseButton_Right;
            break;

        case SDL_BUTTON_MIDDLE:
            return MouseButton_Middle;
            break;

        case SDL_BUTTON_WHEELUP:
            return MouseButton_WheelUp;
            break;

        case SDL_BUTTON_WHEELDOWN:
            return MouseButton_WheelDown;
            break;

        default:
            return MouseButton_Unknown;
            break;
    }
}

}

InputServer::InputServer(Log& log, bool hideCursor, bool constrainCursor):
    m_log(log), m_hideCursor(hideCursor), m_constrainCursor(constrainCursor),
    m_haveMouseFocus(true), m_mouseButtonState(0), m_quit(false)
{
    if (m_hideCursor)
        SDL_ShowCursor(SDL_DISABLE);
    Update();

    const SDL_VideoInfo* videoInfo = SDL_GetVideoInfo();
    m_absoluteMousePosition[0] = videoInfo->current_w / 2;
    m_absoluteMousePosition[1] = videoInfo->current_h / 2;
}

InputServer::~InputServer()
{
}


void InputServer::RegisterListener(IInputListener* listener)
{
    ASSERT(listener);

    m_listeners.push_back(listener);
}

void InputServer::UnregisterListener(IInputListener* listener)
{
    ASSERT(listener);

    m_listeners.erase(std::find(m_listeners.begin(), m_listeners.end(), listener));
}

void InputServer::Update()
{
    const SDL_VideoInfo* videoInfo = SDL_GetVideoInfo();

    SDL_PumpEvents();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_KEYDOWN:
                for (ListenerCollection::const_iterator iter = m_listeners.begin();
                     iter != m_listeners.end(); ++iter)
                {
                    Key key = TranslateKey(event.key.keysym.sym);
                    if (key == Key_Unknown)
                        m_log << "Unknown key down event.\n";
                    else
                        (*iter)->HandleEvent(key, KeyState_Down);
                }
                break;

            case SDL_KEYUP:
                for (ListenerCollection::const_iterator iter = m_listeners.begin();
                     iter != m_listeners.end(); ++iter)
                {
                    Key key = TranslateKey(event.key.keysym.sym);
                    if (key == Key_Unknown)
                        m_log << "Unknown key up event.\n";
                    else
                        (*iter)->HandleEvent(key, KeyState_Up);
                }
                break;

            case SDL_MOUSEMOTION:
            {
                if (!m_haveMouseFocus)
                    break;

                Position delta = Position(event.motion.xrel, event.motion.yrel);

                if (m_constrainCursor)
                {
                    if (event.motion.x == (videoInfo->current_w / 2)
                        && event.motion.y == (videoInfo->current_h / 2))
                        break;

                    m_absoluteMousePosition += delta;

                    m_absoluteMousePosition[0] = math::Clamp(
                            m_absoluteMousePosition[0],
                            0, videoInfo->current_w);

                    m_absoluteMousePosition[1] = math::Clamp(
                            m_absoluteMousePosition[1],
                            0, videoInfo->current_h);
                }
                else
                {
                    m_absoluteMousePosition = Position(event.motion.x,
                                                       event.motion.y);
                }

                for (ListenerCollection::const_iterator iter =
                             m_listeners.begin(); iter != m_listeners.end();
                     ++iter)
                {
                    (*iter)->HandleEvent(m_absoluteMousePosition, delta);
                }
                break;
            }

            case SDL_MOUSEBUTTONDOWN:
            {
                MouseButton button = TranslateMouseButton(event.button.button);
                if (button == MouseButton_Unknown)
                {
                    m_log << "Unknown mouse button down event.\n";
                    break;
                }
                for (ListenerCollection::const_iterator iter = m_listeners.begin();
                     iter != m_listeners.end(); ++iter)
                {
                    (*iter)->HandleEvent(button, Button_Down);
                }
                break;
            }

            case SDL_MOUSEBUTTONUP:
            {
                MouseButton button = TranslateMouseButton(event.button.button);
                if (button == MouseButton_Unknown)
                {
                    m_log << "Unknown mouse button up event.\n";
                    break;
                }
                for (ListenerCollection::const_iterator iter = m_listeners.begin();
                     iter != m_listeners.end(); ++iter)
                {
                    (*iter)->HandleEvent(button, Button_Up);
                }
                break;
            }

            case SDL_QUIT:
                m_quit = true;
                return;
                break;

            case SDL_ACTIVEEVENT:
                if (event.active.state | SDL_APPINPUTFOCUS)
                {
                    m_haveMouseFocus = event.active.gain == 1;
                }
                break;
        }
    }

    m_keyState = SDL_GetKeyState(0);
    m_mouseButtonState = SDL_GetMouseState(0, 0);

    if (m_haveMouseFocus && m_constrainCursor)
        SDL_WarpMouse(videoInfo->current_w / 2, videoInfo->current_h / 2);
}

KeyState InputServer::QueryKeyState(Key key)
{
    return static_cast<KeyState>(m_keyState[TranslateKey(key)]);
}

Position InputServer::QueryMousePosition()
{
    return m_absoluteMousePosition;
}

ButtonState InputServer::QueryMouseButtonState(MouseButton button)
{
    return (m_mouseButtonState & SDL_BUTTON((static_cast<int>(button) + 1)))
            ? Button_Down : Button_Up;
}

}
}
}
