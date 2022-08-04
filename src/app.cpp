#include "app.h"

#include <mutex>

#include <Fl/Fl.H>

TicApp gApp{};
std::recursive_mutex gAppMutex{};

int main(int argc, char *argv[])
{
    gAppMutex.unlock();

    // Enable multithreading support
    Fl::lock();

    // Set UI theme
    Fl::scheme("gleam");

    // Run FLTK event loop
    return Fl::run();
}

TicApp::TicApp()
{
    mainWin.show();
}