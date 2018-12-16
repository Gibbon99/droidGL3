#pragma once

#include "hdr/system/sys_main.h"

extern vec2        mousePosition;

// Handle a mouse event
void io_handleMouseEvent ( SDL_Event event );

// Draw the mouse cursor
void io_renderMouseCursor ();

// Start the mouse timer
void io_mouseTimerState(bool runState);

// Get the current mouse position - used in GUI for action
SDL_Point io_getMousePointLocation();
