#pragma once

#include "hdr/system/sys_main.h"

// Create a Frame Buffer Object and return it's ID number - create a depth buffer for it as well
GLuint gl_createFBO ( glm::vec2 backingSize );

// Make the render target the frameBuffer and associated texture
bool gl_renderToFrameBuffer (string whichTexture);

// Reset drawing back to the default frameBuffer ( Screen )
void gl_renderToScreen ();

GLuint gl_getFrameBufferTexture ();

// Shows framebuffer limits
void gl_getFramebufferLimits();

// Show info on the currently bound frame buffer
void gl_getFramebufferInfo(GLenum target, GLuint fbo);

GLuint gl_getReadBuffer ();

// Link a texture to a FBO
bool gl_linkTextureToFBO ( GLuint whichTexture, GLuint whichFBO );