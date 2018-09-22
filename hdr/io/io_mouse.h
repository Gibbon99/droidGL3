#pragma once

#include "hdr/system/sys_main.h"

extern vec3        mousePosition;

// Handle a mouse event
void io_handleMouseEvent ( SDL_Event event );

// Draw the mouse cursor
void io_renderMouseCursor ();

glm::vec3 io_getMousePosition ();