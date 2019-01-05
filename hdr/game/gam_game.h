#pragma once

#include "hdr/system/sys_main.h"

enum droidState
{
	DROID_MODE_TRANSFER = 0,
	DROID_MODE_HEALING,
	DROID_MODE_TERMINAL,
	DROID_MODE_IN_LIFT,
	DROID_MODE_NORMAL,
};

extern float           baseGameSpeed;

// Start a new game
void gam_startNewGame (int gameType);
