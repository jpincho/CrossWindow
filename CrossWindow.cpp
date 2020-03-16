#include "CrossWindow.h"
#include "CrossWindowBackend.h"
#if defined CROSS_WINDOW_LOGGER_SUPPORT
#include <Logger/Logger.h>
#else
#define LOG_ERROR(...) do{}while(0)
#define LOG_DEBUG(...) do{}while(0)
#endif
#if defined CROSS_WINDOW_WINDOWS_SUPPORT
#include "Win32/Win32Backend.h"
#endif
#if defined CROSS_WINDOW_SDL2_SUPPORT
#include "SDL2/SDL2Backend.h"
#endif
namespace CrossWindow
{
Window::Window ( void )
    {
    Handle = nullptr;
    }

Window::Window ( Window &&other )
    {
    Handle = other.Handle;
    Position = other.Position;
    Size = other.Size;
    Title = other.Title;
    Backend = other.Backend;
    other.Handle = nullptr;
    }

Window &Window::operator= ( Window &&other )
    {
    Handle = other.Handle;
    Position = other.Position;
    Size = other.Size;
    Title = other.Title;
    Backend = other.Backend;
    other.Handle = nullptr;
    return *this;
    }

Window::~Window ( void )
    {
    Destroy();
    }

bool Window::Create ( const WindowCreationFlags Flags, BackendBase *NewBackend )
    {
    if ( Handle )
        return false;
    if ( NewBackend == nullptr )
        NewBackend = GetDefaultBackend();
    if ( NewBackend == nullptr )
        return false;

    Backend = NewBackend;
    Handle = Backend->CreateWindow ( Flags );
    if ( Handle == nullptr )
        return false;
    Backend->SetWindowTitle ( Handle, Flags.Title );
    ProcessMessages();

    Position = Backend->GetWindowPosition ( Handle );
    Size = Backend->GetWindowSize ( Handle );
    Title = Backend->GetWindowTitle ( Handle );
    return true;
    }

bool Window::Destroy ( void )
    {
    if ( !Handle )
        return true;
    Backend->DestroyWindow ( Handle );
    Handle = nullptr;
    return true;
    }

void Window::SetPosition ( const glm::ivec2 NewPosition )
    {
    if ( !Handle ) return;
    Backend->SetWindowPosition ( Handle, NewPosition );
    Position = Backend->GetWindowPosition ( Handle );
    }

glm::ivec2 Window::GetPosition ( void ) const
    {
    return Position;
    }

void Window::SetSize ( const glm::uvec2 NewSize )
    {
    if ( !Handle ) return;
    Backend->SetWindowSize ( Handle, NewSize );
    Size = Backend->GetWindowSize ( Handle );
    }

glm::uvec2 Window::GetSize ( void ) const
    {
    return Size;
    }

void Window::SetTitle ( const std::string NewTitle )
    {
    if ( !Handle ) return;
    Backend->SetWindowTitle ( Handle, NewTitle );
    Title = Backend->GetWindowTitle ( Handle );
    }

std::string Window::GetTitle ( void ) const
    {
    return Title;
    }

bool Window::IsAlive ( void ) const
    {
    return Handle != nullptr;
    }

void Window::ProcessMessages ( void )
    {
    Backend->FetchMessages();
    for ( std::list <WindowEvent> ::iterator EventIterator = Backend->Events.begin(); EventIterator != Backend->Events.end(); )
        {
        if ( EventIterator->OwnerHandle == nullptr ) // If it's meant for all windows
            {
            switch ( EventIterator->EventType )
                {
                case WindowEventType::Quit:
                    Backend->DestroyWindow ( Handle );
                    Handle = nullptr;
                    break;
                default:
                    break;
                }
            }
        if ( EventIterator->OwnerHandle == Handle ) // If it's meant for this window
            {
            switch ( EventIterator->EventType )
                {
                case WindowEventType::WindowClosed:
                    LOG_DEBUG ( "Window being closed" );
                    Backend->DestroyWindow ( Handle );
                    Handle = nullptr;
                    break;
                case WindowEventType::WindowMoved:
                    LOG_DEBUG ( "Window moved from %d %d to %d %d", Position.x, Position.y, EventIterator->EventData.WindowMoved.X, EventIterator->EventData.WindowMoved.Y );
                    Position = glm::ivec2 ( EventIterator->EventData.WindowMoved.X, EventIterator->EventData.WindowMoved.Y );
                    break;
                case WindowEventType::WindowResized:
                    LOG_DEBUG ( "Window resized from %u %u to %u %u", Size.x, Size.y, EventIterator->EventData.WindowResized.Width, EventIterator->EventData.WindowResized.Height );
                    Size = glm::uvec2 ( EventIterator->EventData.WindowResized.Width, EventIterator->EventData.WindowResized.Height );
                    break;
                case WindowEventType::ButtonPressed:
                    LOG_DEBUG ( "Button %u pressed", EventIterator->EventData.ButtonPressed.Button );
                    break;
                case WindowEventType::ButtonReleased:
                    LOG_DEBUG ( "Button %u released", EventIterator->EventData.ButtonReleased.Button );
                    break;
                case WindowEventType::KeyPressed:
                    LOG_DEBUG ( "Key %u pressed", EventIterator->EventData.KeyPressed.Key );
                    break;
                case WindowEventType::KeyReleased:
                    LOG_DEBUG ( "Key %u released", EventIterator->EventData.KeyReleased.Key );
                    break;
                default:
                    break;
                }
            EventIterator = Backend->Events.erase ( EventIterator );
            }
        ++EventIterator;
        }
    }

void *Window::GetNativeWindowHandle ( void ) const
    {
    return Backend->GetNativeWindowHandle ( Handle );
    }

WindowHandle Window::GetWindowHandle ( void ) const
    {
    return Handle;
    }

BackendBase *Window::GetBackend ( void ) const
    {
    return Backend;
    }

BackendBase *GetDefaultBackend ( void )
    {
#if defined CROSS_WINDOW_WINDOWS_SUPPORT
    static Win32Backend WinBackend;
    return &WinBackend;
#endif
#if defined CROSS_WINDOW_SDL2_SUPPORT
    return &SDL2BackendInstance;
#endif
    }
}
