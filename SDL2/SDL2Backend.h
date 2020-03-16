#pragma once
#include "../CrossWindow.h"
#include "../CrossWindowBackend.h"

namespace CrossWindow
{
class SDL2Backend : public BackendBase
    {
    public:
        SDL2Backend ( void );
        virtual ~SDL2Backend ( void ) = default;
        WindowHandle CreateWindow ( const WindowCreationFlags &Flags );
        bool DestroyWindow ( const WindowHandle Handle );
        void SetWindowPosition ( const WindowHandle Handle, const glm::ivec2 NewPosition );
        glm::ivec2 GetWindowPosition ( const WindowHandle Handle );
        void SetWindowSize ( const WindowHandle Handle, const glm::uvec2 NewSize );
        glm::uvec2 GetWindowSize ( const WindowHandle Handle );
        void SetWindowTitle ( const WindowHandle Handle, const std::string NewTitle );
        std::string GetWindowTitle ( const WindowHandle Handle );
        bool SetWindowFullscreen ( const WindowHandle Handle, const bool NewState );
        bool IsWindowFullscreen ( const WindowHandle Handle ) const;
        void FetchMessages ( void );

        uint32_t GetKeyCode ( const std::string KeyName ) const;
        uint32_t GetKeyScanCode ( const std::string KeyName ) const;
        const char *GetKeyName ( const uint32_t KeyCode ) const;
        glm::ivec2 GetMousePosition ( void ) const;
        uint32_t GetMouseButtonStatus ( void ) const;
        void *GetNativeWindowHandle ( const WindowHandle Handle ) const;

        std::string GetErrorDescription ( void );
    };

extern SDL2Backend SDL2BackendInstance;
}
