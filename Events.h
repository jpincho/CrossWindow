#pragma once
#include <stdint.h>

namespace CrossWindow
{
enum class WindowEventType
    {
    WindowCreated,
    WindowClosed,
    WindowMoved,
    WindowResized,
    KeyPressed,
    KeyReleased,
    ButtonPressed,
    ButtonReleased,
    Quit,
    FrameStarted,
    FrameEnded,
    TextInput,
    EVENTCOUNT
    };

typedef void *WindowHandle;

typedef struct WindowEvent
    {
    WindowEventType EventType;
    WindowHandle OwnerHandle;
    union
        {
        struct
            {
            uint32_t Key, ScanCode;
            } KeyPressed, KeyReleased;
        struct
            {
            uint32_t Button;
            } ButtonPressed, ButtonReleased;
        struct
            {
            uint32_t Data1, Data2;
            } Raw;
        struct
            {
            int32_t X, Y;
            } WindowMoved;
        struct
            {
            uint32_t Width, Height;
            } WindowResized;
        struct
            {
            char Text[32];
            } TextInput;
        void *Pointer;
        } EventData;
    WindowEvent ( void ) = default;
    } WindowEvent;

class EventListener
    {
    public:
        virtual ~EventListener ( void ) = default;
        virtual void OnEvent ( const WindowEvent &Event ) = 0;
    };

void AddEventListener ( void ( *Function ) ( const WindowEvent & ) );
void DeleteEventListener ( void ( *Function ) ( const WindowEvent & ) );
void AddEventListener ( EventListener *Listener );
void DeleteEventListener ( EventListener *Listener );
void AddEventListener ( WindowEventType EventToListen, void ( *Function ) ( const WindowEvent & ) );
void DeleteEventListener ( WindowEventType EventToListen, void ( *Function ) ( const WindowEvent & ) );
void SendWindowEvent ( const WindowEvent &NewEvent );
}
