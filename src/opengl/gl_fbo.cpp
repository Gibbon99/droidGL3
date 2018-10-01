#include "hdr/opengl/gl_fbo.h"

GLuint frameBufferName = 0;
GLuint targetTexture = 0;
GLuint depthTargetBuffer = 0;

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
GLuint gl_getFrameBufferTexture()
//-----------------------------------------------------------------------------------------------------
{
	return frameBufferName;
}

//-----------------------------------------------------------------------------------------------------
//
/// \param None
/// \return None
//
// Make the render target the frameBuffer and associated texture
void gl_renderToFrameBuffer ()
//-----------------------------------------------------------------------------------------------------
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferName);

	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
	glViewport (0, 0, winWidth, winHeight);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//-----------------------------------------------------------------------------------------------------
//
/// \param None
/// \return None
//
// Reset drawing back to the default frameBuffer ( Screen )
void gl_renderToScreen()
//-----------------------------------------------------------------------------------------------------
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//-----------------------------------------------------------------------------------------------------
//
/// \param None
/// False on Error, True on successful creation and linking of texture, depth and framebuffer
bool gl_createFBO()
//-----------------------------------------------------------------------------------------------------
{
	//
	// Generate ID for the FrameBuffer. This encompasses a target texture, and a depth buffer
	glGenFramebuffers(1, &frameBufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferName);
	//
	// Create a window sized texture to associate to the frameBuffer
	glGenTextures(1, &targetTexture);
	glBindTexture(GL_TEXTURE_2D, targetTexture);
	// Texture is empty
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, winWidth, winHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	//
	// TODO: Add filtering options
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//
	// Generate the depth buffer and associate to the frameBuffer
	glGenRenderbuffers(1, &depthTargetBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthTargetBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, winWidth, winHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTargetBuffer);
	//
	// Set the target texture as the color target for the frameBuffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, targetTexture, 0);
	//
	// Set the number of associated draw buffers to the frameBuffer
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};     // Renders to the first frameBuffer - bound as layout = 0 outvec4 color in fragment shader
	glDrawBuffers(1, DrawBuffers);
	//
	// Check the association worked ok
	GLenum Status = glCheckFramebufferStatus (GL_FRAMEBUFFER);

	if ( Status != GL_FRAMEBUFFER_COMPLETE )
	{
		switch ( Status )
		{
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				con_print (CON_ERROR, true, "Not all framebuffer attachment points are framebuffer attachment complete. This means that at least one attachment point with a renderbuffer or texture attached has its attached object no longer in existence or has an attached image with a width or height of zero, or the color attachment point has a non-color-renderable image attached, or the depth attachment point has a non-depth-renderable image attached, or the stencil attachment point has a non-stencil-renderable image attached.");
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				con_print (CON_ERROR, true, "No images are attached to the framebuffer.");
				break;

			case GL_FRAMEBUFFER_UNSUPPORTED:
				con_print (CON_ERROR, true, "The combination of internal formats of the attached images violates an implementation-dependent set of restrictions.");
				break;

			default:
				con_print (CON_ERROR, true, "Error: Failed to create GBuffers - status [ 0x%x ]", Status);
				break;
		}
		return false;
	}
	//
	// Set default frameBuffer ( Screen )
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}