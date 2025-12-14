/**
 * LumaSort Engine - Main Entry Point
 * 
 * This file bootstraps the application by creating the App instance and entering the run loop.
 * It serves as the bridge between the OS and our engine's lifecycle.
 */

#include "app.h"

int main() {
    // Initialize the engine with a 720p window and a descriptive title.
    // We stick to standard HD resolution as a baseline, but the window is resizable.
    App app("LumaSort Engine", 1280, 720);
    
    // Begin the main application loop.
    // This will block until the window is closed or an exit signal is received.
    app.run();
    
    return 0;
}
