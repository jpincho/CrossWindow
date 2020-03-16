#pragma once
#include "CrossWindow.h"
#include <list>

namespace CrossWindow
{
class BackendBase
    {
    protected:
        BackendType Type;
    public:
        std::list <WindowEvent> Events;
        virtual ~BackendBase ( void ) = default;
        virtual WindowHandle CreateWindow ( const WindowCreationFlags &Flags ) = 0;
        virtual bool DestroyWindow ( const WindowHandle Handle ) = 0;
        virtual void SetWindowPosition ( const WindowHandle Handle, const glm::ivec2 NewPosition ) = 0;
        virtual glm::ivec2 GetWindowPosition ( const WindowHandle Handle ) = 0;
        virtual void SetWindowSize ( const WindowHandle Handle, const glm::uvec2 NewSize ) = 0;
        virtual glm::uvec2 GetWindowSize ( const WindowHandle Handle ) = 0;
        virtual void SetWindowTitle ( const WindowHandle Handle, const std::string NewTitle ) = 0;
        virtual std::string GetWindowTitle ( const WindowHandle Handle ) = 0;
        virtual bool SetWindowFullscreen ( const WindowHandle Handle, const bool NewState ) = 0;
        virtual bool IsWindowFullscreen ( const WindowHandle Handle ) const = 0;

        virtual void FetchMessages ( void ) = 0;

        virtual uint32_t GetKeyCode ( const std::string KeyName ) const = 0;
        virtual uint32_t GetKeyScanCode ( const std::string KeyName ) const = 0;
        virtual const char *GetKeyName ( const uint32_t KeyCode ) const = 0;
        virtual glm::ivec2 GetMousePosition ( void ) const = 0;
        virtual uint32_t GetMouseButtonStatus ( void ) const = 0;
        virtual void *GetNativeWindowHandle ( const WindowHandle Handle ) const = 0;
        virtual BackendType GetType ( void ) const
            {
            return Type;
            }
        virtual std::string GetErrorDescription ( void ) = 0;
    };
}
