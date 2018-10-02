#pragma once

#include "hdr/system/sys_main.h"

// Add all the shaders
bool gl_addShaders ();

// Return the Shader ID
GLint gl_getShaderID ( string shaderName );

// If variable is not found, still record, but mark has invalid - count how many
// times it is called.
GLint gl_getUniform ( string whichShader, string keyName );

// If variable is not found, still record, but mark has invalid - count how many
// times it is called.
GLuint gl_getAttrib ( string whichShader, string keyName );