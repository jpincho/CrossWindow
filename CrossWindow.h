#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Events.h"
#include "CrossWindowConfig.h"

#if defined CROSS_WINDOW_LOGGER_SUPPORT
#include <Logger/Logger.h>
#else
#define LOG_ERROR(...) do{}while(0)
#define LOG_DEBUG(...) do{}while(0)
#define LOG_WARNING(...) do{}while(0)
#endif

namespace CrossWindow
{
typedef struct WindowCreationFlags
    {
    WindowCreationFlags ( const glm::ivec2 NewPosition = glm::ivec2 ( 0, 0 ), const glm::uvec2 NewSize = glm::uvec2 ( 640, 480 ) )
        {
        Position = NewPosition;
        Size = NewSize;
        Resizable = true;
        Fullscreen = false;
        }

    glm::uvec2 Size;
    glm::ivec2 Position;
    bool Resizable, Fullscreen;
    std::string Title;
    } WindowCreationFlags;

class BackendBase;
enum class BackendType
    {
    Windows,
    SDL2
    };

class Window
    {
    protected:
        BackendBase *Backend;
        WindowHandle Handle;
        glm::ivec2 Position;
        glm::uvec2 Size;
        std::string Title;

    public:
        Window ( void );
        Window ( const Window &other ) = delete;
        Window &operator = ( const Window &other ) = delete;
        Window ( Window &&other );
        Window &operator = ( Window &&other );
        ~Window ( void );
        bool Create ( const WindowCreationFlags Flags, CrossWindow::BackendBase *NewBackend = nullptr );
        bool Destroy ( void );
        void SetPosition ( const glm::ivec2 NewPosition );
        glm::ivec2 GetPosition ( void ) const;
        void SetSize ( const glm::uvec2 NewSize );
        glm::uvec2 GetSize ( void ) const;
        void SetTitle ( const std::string NewTitle );
        std::string GetTitle ( void ) const;
        bool IsAlive ( void ) const;
        void ProcessMessages ( void );
        void *GetNativeWindowHandle ( void ) const;
        WindowHandle GetWindowHandle ( void ) const;
        BackendBase *GetBackend ( void ) const;
    };

CrossWindow::BackendBase *GetDefaultBackend ( void );
}
