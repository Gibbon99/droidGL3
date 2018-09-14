#pragma once

#include "hdr/system/sys_main.h"

// Create texture containing the embedded font data
void fnt_printText ( glm::vec2 position, glm::vec4 lineColor, const char *text, ... );