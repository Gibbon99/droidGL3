#pragma once

#define MY_INPUT_ACTION_PRESS   0x00
#define MY_INPUT_ACTION_RELEASE 0x01

#define MY_INPUT_LEFT           0x10
#define MY_INPUT_RIGHT          0x11
#define MY_INPUT_UP             0x12
#define MY_INPUT_DOWN           0x13
#define MY_KEY_ACTION         0x14
#define MY_KEY_ESCAPE         0x15
#define MY_INPUT_CONSOLE        0x16
#define MY_INPUT_FORWARD        0x17
#define MY_INPUT_BACKWARD       0x18
#define MY_INPUT_PAUSE          0x19

// Handle a game event - called by thread
int gam_processGameEventQueue ( void *ptr );

