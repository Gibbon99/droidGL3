#pragma once

#include "hdr/system/sys_main.h"

extern bool keyForwardDown;
extern bool keyBackwardDown;
extern bool keyLeftDown;
extern bool keyRightDown;
extern bool keyUpDown;
extern bool keyDownDown;

// Handle a keyboard event
void io_handleKeyboardEvent ( SDL_Event event );

// Read a unicode character
void io_readChar ( int character );

// Process keyboard presses
void io_processKeyboard ();