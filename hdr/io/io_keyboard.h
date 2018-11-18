#pragma once

#include "hdr/system/sys_main.h"

extern bool eventMoveForward;
extern bool eventMoveBackward;
extern bool eventMoveLeft;
extern bool eventMoveRight;
extern bool eventMoveUp;
extern bool eventMoveDown;

// Handle a keyboard event
void io_handleKeyboardEvent ( SDL_Event event );

// Read a unicode character
void io_readChar ( int character );

// Process keyboard presses
void io_processInputActions ();

void io_processGameInputEvents (_myEventData eventData);
