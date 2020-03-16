#include "../CrossWindow.h"
#include <list>

int main ( void )
    {
    std::list <CrossWindow::Window> AllWindows;

    auto CreateNewWindow = [&AllWindows] ( void ) mutable -> void
        {
        CrossWindow::Window NewWindow;
        CrossWindow::WindowCreationFlags Flags;
        Flags.Position = glm::ivec2 ( 100, 100 );
        Flags.Resizable = true;
        Flags.Fullscreen = false;
        Flags.Size = glm::uvec2 ( 800, 600 );
        Flags.Title.assign ( "MyWindow " );
        Flags.Title += std::to_string ( AllWindows.size() + 1 );
        NewWindow.Create ( Flags );
        AllWindows.push_back ( std::move ( NewWindow ) );
        };

    CreateNewWindow();
    while ( AllWindows.size() )
        {
        for ( std::list <CrossWindow::Window>::iterator WindowIterator = AllWindows.begin(); WindowIterator != AllWindows.end(); )
            {
            WindowIterator->ProcessMessages();
            if ( WindowIterator->IsAlive() == false )
                WindowIterator = AllWindows.erase ( WindowIterator );
            else
                ++WindowIterator;
            }
        }
    return 0;
    }
