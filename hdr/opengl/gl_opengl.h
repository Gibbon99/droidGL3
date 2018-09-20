#pragma once

#include "hdr/system/sys_main.h"

//-----------------------------------------------------------------------------
//
// Structure to hold OpenGL colors
//
//-----------------------------------------------------------------------------
typedef struct
{
	float red;
	float green;
	float blue;
	float alpha;
} _glColor;

// Get all the openGL errors
void gl_getAllGLErrors ( int errorNum, const char *calledFrom, int line );

// Draw a 2D quad
void gl_draw2DQuad ( glm::vec2 position, glm::vec2 quadSize, std::string whichShader, GLuint whichTexture, float interpolation );

// Draw a debug line
void gl_drawLine ( glm::vec2 startPoint, glm::vec2 endPoint, std::string whichShader, glm::vec4 lineColor );

// Load the font texture
bool gl_loadFontTexture ( const char *fontFilename );

// Set OpenGL to 2d orthographic mode
void gl_set2DMode ();

// OpenGL Debug - Register callback
void gl_registerDebugCallback ();

// Print out all the GLSL error messages
void gl_displayErrors ();