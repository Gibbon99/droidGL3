#pragma once

extern SDL_Window      *mainWindow;

// Swap buffers for displaying screen
void lib_swapBuffers ();

// Shutdown the library windowing system
void lib_destroyWindow ();

// Start the GL Helper library and open the window
bool lib_openWindow ();
