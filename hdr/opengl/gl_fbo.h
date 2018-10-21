#pragma once

#include "hdr/system/sys_main.h"

/// False on Error, True on successful creation and linking of texture, depth and framebuffer
bool gl_createFBO ();

// Make the render target the frameBuffer and associated texture
bool gl_renderToFrameBuffer (string whichTexture);

// Reset drawing back to the default frameBuffer ( Screen )
void gl_renderToScreen ();

GLuint gl_getFrameBufferTexture ();

// Shows framebuffer limits
void gl_getFramebufferLimits();

// Show info on the currently bound frame buffer
void gl_getFramebufferInfo(GLenum target, GLuint fbo);

