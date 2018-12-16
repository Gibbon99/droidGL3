#pragma once

#include "hdr/opengl/gl_openGLWrap.h"
#include "hdr/system/sys_main.h"
#include "hdr/libGL/freetype-gl/vertex-buffer.h"

// Create texture containing the embedded font data
// Construct the texture and vertex positions and upload to card
// Render buffers using GL_TRIANGLES
void sdfFont_printText ( glm::vec2 position, glm::vec4 lineColor, glm::vec4 outLineColor, string whichShader, const char *text, ... );

// Return the width of a text string
float sdfFont_getStringWidth(string format, ...);
