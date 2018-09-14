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

// Load the font texture
bool gl_loadFontTexture ( const char *fontFilename );

// Set OpenGL to 2d orthographic mode
void gl_set2DMode ();

// OpenGL Debug - Register callback
void gl_registerDebugCallback ();

// Print out all the GLSL error messages
void gl_displayErrors ();