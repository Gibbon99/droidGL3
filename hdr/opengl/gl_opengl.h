#pragma once

#include <hdr/game/s_shadows.h>
#include "hdr/system/sys_main.h"

extern glm::mat4            MVP;
extern float                nearPlane;
extern float                farPlane;
extern float                cameraDistance;
extern float                g_gamma;

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
void gl_drawLine ( glm::vec3 startPoint, glm::vec3 endPoint, std::string whichShader, glm::vec4 lineColor );

void gl_drawShadowHull ( std::set<_shadowHullPoint> const &drawShadowHull, glm::vec3 mousePosition, std::string whichShader );

void gl_drawQuad ();

// Set OpenGL to 2d orthographic mode
void gl_set3DMode ( float interpolate );

// Load the font texture
bool gl_loadFontTexture ( const char *fontFilename );

// OpenGL Debug - Register callback
void gl_registerDebugCallback ();

// Print out all the GLSL error messages
void gl_displayErrors ();

void gl_drawAllQuads ( std::string shaderName, std::string textureName, float interpolation );