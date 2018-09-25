#pragma once

#include "hdr/system/sys_main.h"

/// False on Error, True on succesful creattion and linking of texture, depth and framebuffer
bool gl_createFBO ();

// Make the render target the frameBuffer and associated texture
void gl_renderToFrameBuffer ();

// Reset drawing back to the default frameBuffer ( Screen )
void gl_renderToScreen ();

GLuint gl_getFrameBufferTexture ();
