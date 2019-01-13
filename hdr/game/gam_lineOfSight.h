#pragma once

#include "hdr/system/sys_main.h"

extern float		g_LineOfSightStep;
extern bool			allDroidsVisible;

// Master routine to check visiblity of all valid sprites
void lvl_LOS(const string levelName);
