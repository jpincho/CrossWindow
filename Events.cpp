#include "Events.h"
#include <unordered_set>

namespace CrossWindow
{
typedef std::unordered_set <void ( * ) ( const WindowEvent & ) > TWindowEventListeners;
static TWindowEventListeners WindowEventListeners[ ( int ) WindowEventType::EVENTCOUNT];
typedef std::unordered_set <EventListener*> TClassWindowEventListeners;
static TClassWindowEventListeners ClassEventListeners;

void AddEventListener ( void ( *Function ) ( const WindowEvent & ) )
    {
    for ( int EventType = 0; EventType < ( int ) WindowEventType::EVENTCOUNT; ++EventType )
        AddEventListener ( ( WindowEventType ) EventType, Function );
    }

void DeleteEventListener ( void ( *Function ) ( const WindowEvent & ) )
    {
    for ( int EventType = 0; EventType < ( int ) WindowEventType::EVENTCOUNT; ++EventType )
        DeleteEventListener ( ( WindowEventType ) EventType, Function );
    }

void AddEventListener ( EventListener *Listener )
    {
    ClassEventListeners.insert ( Listener );
    }

void DeleteEventListener ( EventListener *Listener )
    {
    ClassEventListeners.erase ( Listener );
    }

void AddEventListener ( WindowEventType EventToListen, void ( *Function ) ( const WindowEvent & ) )
    {
    WindowEventListeners[ ( int ) EventToListen].insert ( Function );
    }

void DeleteEventListener ( WindowEventType EventToListen, void ( *Function ) ( const WindowEvent & ) )
    {
    WindowEventListeners[ ( int ) EventToListen].erase ( Function );
    }

void SendWindowEvent ( const WindowEvent &NewEvent )
    {
    for ( auto Iterator : WindowEventListeners[ ( int ) NewEvent.EventType] )
        {
        Iterator ( NewEvent );
        }
    for ( auto &Iterator : ClassEventListeners )
        Iterator->OnEvent ( NewEvent );
    }
}
