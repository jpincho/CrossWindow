#include "SDL2Backend.h"
#include <SDL.h>
#include <SDL_syswm.h>
#include <list>
#include <algorithm>

namespace CrossWindow
{
static std::list <SDL_Window*> Windows;
SDL2Backend SDL2BackendInstance;

// Fucking windows with their macros
#if defined CreateWindow
#undef CreateWindow
#endif

SDL2Backend::SDL2Backend ( void )
    {
    Type = BackendType::SDL2;
    }

WindowHandle SDL2Backend::CreateWindow ( const WindowCreationFlags &Flags )
    {
    int CreationFlags = SDL_WINDOW_OPENGL;
    if ( Flags.Resizable )
        CreationFlags |= SDL_WINDOW_RESIZABLE;
    if ( Flags.Fullscreen )
        CreationFlags |= SDL_WINDOW_FULLSCREEN;
    SDL_Window *SDLWindowHandle = SDL_CreateWindow ( "SDL window", Flags.Position.x, Flags.Position.y, Flags.Size.x, Flags.Size.y, CreationFlags );
    if ( SDLWindowHandle == nullptr )
        return nullptr;

    Windows.push_back ( SDLWindowHandle );
    return SDLWindowHandle;
    }

bool SDL2Backend::DestroyWindow ( const WindowHandle Handle )
    {
    if ( !Handle )
        return false;

    SDL_DestroyWindow ( static_cast <SDL_Window*> ( Handle ) );

    std::list<SDL_Window*>::iterator Iterator = std::find ( Windows.begin(), Windows.end(), Handle );
    if ( Iterator != Windows.end() )
        Windows.erase ( Iterator );
    return true;
    }

void SDL2Backend::SetWindowPosition ( const WindowHandle Handle, const glm::ivec2 NewPosition )
    {
    SDL_SetWindowPosition ( static_cast <SDL_Window*> ( Handle ), NewPosition.x, NewPosition.y );
    }

glm::ivec2 SDL2Backend::GetWindowPosition ( const WindowHandle Handle )
    {
    glm::ivec2 Position;
    SDL_GetWindowPosition ( static_cast <SDL_Window*> ( Handle ), &Position.x, &Position.y );
    return Position;
    }

void SDL2Backend::SetWindowSize ( const WindowHandle Handle, const glm::uvec2 NewSize )
    {
    SDL_SetWindowSize ( static_cast <SDL_Window*> ( Handle ), NewSize.x, NewSize.y );
    }

glm::uvec2 SDL2Backend::GetWindowSize ( const WindowHandle Handle )
    {
    glm::uvec2 Size;
    SDL_GetWindowSize ( static_cast <SDL_Window*> ( Handle ), ( int * ) &Size.x, ( int * ) &Size.y );
    return Size;
    }

void SDL2Backend::SetWindowTitle ( const WindowHandle Handle, const std::string NewTitle )
    {
    SDL_SetWindowTitle ( static_cast <SDL_Window*> ( Handle ), NewTitle.c_str() );
    }

std::string SDL2Backend::GetWindowTitle ( const WindowHandle Handle )
    {
    return SDL_GetWindowTitle ( static_cast <SDL_Window*> ( Handle ) );
    }

bool SDL2Backend::SetWindowFullscreen ( const WindowHandle Handle, const bool NewState )
    {
    return ( SDL_SetWindowFullscreen ( static_cast <SDL_Window*> ( Handle ), NewState ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0 ) == 0 );
    }

bool SDL2Backend::IsWindowFullscreen ( const WindowHandle Handle ) const
    {
    uint32_t Flags = SDL_GetWindowFlags ( static_cast <SDL_Window*> ( Handle ) );
    return ( ( Flags & ( SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP ) ) != 0 );
    }

void SDL2Backend::FetchMessages ( void )
    {
    // Process all SDL events
    SDL_Event Event;
    while ( SDL_PollEvent ( &Event ) )
        {
        switch ( Event.type )
            {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                {
                if ( Event.key.repeat != 0 ) break;
                WindowEvent NewEvent;
                NewEvent.EventType = ( Event.type == SDL_KEYDOWN ? WindowEventType::KeyPressed : WindowEventType::KeyReleased );
                NewEvent.EventData.Raw.Data1 = Event.key.keysym.sym;
                NewEvent.EventData.Raw.Data2 = Event.key.keysym.scancode;
                NewEvent.OwnerHandle = *Windows.begin();
                SendWindowEvent ( NewEvent );
                Events.push_back ( NewEvent );
                break;
                }
            case SDL_TEXTINPUT:
                {
                WindowEvent NewEvent;
                NewEvent.EventType = WindowEventType::TextInput;
                memcpy ( NewEvent.EventData.TextInput.Text, Event.text.text, 32 );
                NewEvent.OwnerHandle = *Windows.begin();
                SendWindowEvent ( NewEvent );
                Events.push_back ( NewEvent );
                break;
                }
            case SDL_WINDOWEVENT:
                {
                SDL_Window *window = SDL_GetWindowFromID ( Event.window.windowID );
                if ( window == nullptr )
                    break;
                switch ( Event.window.event )
                    {
                    case SDL_WINDOWEVENT_CLOSE:
                        {
                        WindowEvent NewEvent;
                        NewEvent.EventType = WindowEventType::WindowClosed;
                        NewEvent.OwnerHandle = window;
                        SendWindowEvent ( NewEvent );
                        Events.push_back ( NewEvent );
                        DestroyWindow ( window );
                        break;
                        }

                    case SDL_WINDOWEVENT_MOVED:
                        {
                        glm::ivec2 NewPosition ( Event.window.data1, Event.window.data2 );
                        WindowEvent NewEvent;
                        NewEvent.EventType = WindowEventType::WindowMoved;
                        NewEvent.EventData.WindowMoved.X = NewPosition.x;
                        NewEvent.EventData.WindowMoved.Y = NewPosition.y;
                        NewEvent.OwnerHandle = window;
                        SendWindowEvent ( NewEvent );
                        Events.push_back ( NewEvent );
                        break;
                        }

                    case SDL_WINDOWEVENT_RESIZED:
                        {
                        glm::uvec2 NewSize ( Event.window.data1, Event.window.data2 );
                        WindowEvent NewEvent;
                        NewEvent.EventType = WindowEventType::WindowResized;
                        NewEvent.EventData.WindowResized.Width = NewSize.x;
                        NewEvent.EventData.WindowResized.Height = NewSize.y;
                        NewEvent.OwnerHandle = window;
                        SendWindowEvent ( NewEvent );
                        Events.push_back ( NewEvent );
                        break;
                        }
                    }
                break;
                }
            case SDL_QUIT:
                {
                WindowEvent NewEvent;
                NewEvent.EventType = WindowEventType::Quit;
                NewEvent.OwnerHandle = *Windows.begin();
                SendWindowEvent ( NewEvent );
                Events.push_back ( NewEvent );
                break;
                }
            }
        }
    }

uint32_t SDL2Backend::GetKeyCode ( const std::string KeyName ) const
    {
    SDL_Keycode Result = SDL_GetKeyFromName ( KeyName.c_str() );
    if ( Result == SDLK_UNKNOWN )
        LOG_ERROR ( "Unrecognized key '%s'", KeyName.c_str(), SDL_GetError() );
    return Result;
    }

uint32_t SDL2Backend::GetKeyScanCode ( const std::string KeyName ) const
    {
    SDL_Scancode Result = SDL_GetScancodeFromName ( KeyName.c_str() );
    if ( Result == SDL_SCANCODE_UNKNOWN )
        LOG_ERROR ( "Unrecognized key '%s'", KeyName.c_str(), SDL_GetError() );
    return Result;
    }

const char *SDL2Backend::GetKeyName ( const uint32_t KeyCode ) const
    {
    return SDL_GetKeyName ( KeyCode );
    }

glm::ivec2 SDL2Backend::GetMousePosition ( void ) const
    {
    glm::ivec2 MousePosition;
    SDL_GetMouseState ( &MousePosition.x, &MousePosition.y );
    return MousePosition;
    }

uint32_t SDL2Backend::GetMouseButtonStatus ( void ) const
    {
    return SDL_GetMouseState ( nullptr, nullptr );
    }

void *SDL2Backend::GetNativeWindowHandle ( const WindowHandle Handle ) const
    {
    SDL_SysWMinfo WMInfo;
    SDL_VERSION ( &WMInfo.version );
    SDL_GetWindowWMInfo ( static_cast <SDL_Window*> ( Handle ), &WMInfo );
#if defined CROSS_WINDOW_PLATFORM_WINDOWS
    return WMInfo.info.win.window;
#elif defined CROSS_WINDOW_PLATFORM_DARWIN
    return WMInfo.info.cocoa.window;
#elif defined CROSS_WINDOW_PLATFORM_LINUX
    return ( void* ) WMInfo.info.x11.window;
#else
    assert ( false );
#endif
    }

std::string SDL2Backend::GetErrorDescription ( void )
    {
    return SDL_GetError();
    }
}
