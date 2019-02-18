#include <hdr/io/io_textures.h>
#include "hdr/opengl/gl_fbo.h"

GLuint frameBufferName = 0;
GLuint targetTexture = 0;
GLuint depthTargetBuffer = 0;

GLuint gl_getReadBuffer()
{
	return frameBufferName;

}
//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
GLuint gl_getFrameBufferTexture()
//-----------------------------------------------------------------------------------------------------
{
	return targetTexture;
}

//-----------------------------------------------------------------------------------------------------
//
/// \param None
/// \return None
//
// Make the render target the frameBuffer and associated texture
//
// Pass in texture name to get the ID and size to use
bool gl_renderToFrameBuffer (string whichTexture)
//-----------------------------------------------------------------------------------------------------
{
	int         textureID;
	glm::vec2   textureSize;

	textureID = io_getTextureID(whichTexture);
	if (0 == textureID)
	{
		con_print(CON_ERROR, true, "Unable to find texture to bind to FBO [ %s ]", whichTexture);
		return false;
	}

	textureSize = io_getTextureSize (whichTexture);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferName);
	//
	// Set the target texture as the color target for the frameBuffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureID, 0);

//	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
	glViewport (0, 0, static_cast<GLsizei>(textureSize.x), static_cast<GLsizei>(textureSize.y));
//	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//
	// Set the number of associated draw buffers to the frameBuffer
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};     // Renders to the first frameBuffer - bound as layout = 0 outvec4 color in fragment shader
	glDrawBuffers(1, DrawBuffers);
	//
	// Check the association worked ok
	GLenum checkStatus = glCheckFramebufferStatus (GL_FRAMEBUFFER);

	if ( checkStatus != GL_FRAMEBUFFER_COMPLETE )
	{
		switch ( checkStatus )
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
				con_print (CON_ERROR, true, "Error: Failed to create GBuffers - status [ 0x%x ]", checkStatus);
				break;
		}
		return false;
	}
	return true;
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
// Link a texture to a FBO
bool gl_linkTextureToFBO (GLuint whichTexture, GLuint whichFBO, GLuint bindOperation)
//-----------------------------------------------------------------------------------------------------
{
	glBindFramebuffer (bindOperation, whichFBO);
	//
	// Set the target texture as the color target for the frameBuffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, whichTexture, 0);
	//
	// Set the number of associated draw buffers to the frameBuffer
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};     // Renders to the first frameBuffer - bound as layout = 0 outvec4 color in fragment shader
	glDrawBuffers (1, DrawBuffers);
	//
	// Check the association worked ok
	GLenum checkStatus = glCheckFramebufferStatus (GL_FRAMEBUFFER);

	if ( checkStatus != GL_FRAMEBUFFER_COMPLETE )
	{
		switch ( checkStatus )
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
				con_print (CON_ERROR, true, "Error: Failed to create GBuffers - status [ 0x%x ]", checkStatus);
				break;
		}
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------------------------------
//
// \param None
// Create a Frame Buffer Object and return it's ID number - create a depth buffer for it as well
GLuint gl_createFBO(glm::vec2 backingSize)
//-----------------------------------------------------------------------------------------------------
{
	GLuint frameBufferID = 0;
	GLuint depthTextureID = 0;
	//
	// Generate ID for the FrameBuffer. This encompasses a target texture, and a depth buffer
	glGenFramebuffers(1, &frameBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
	//
	// Generate the depth buffer and associate to the frameBuffer
	glGenRenderbuffers(1, &depthTextureID);
	glBindRenderbuffer(GL_RENDERBUFFER, depthTextureID);
	//
	// Bind depth texture to FBO
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, static_cast<GLsizei>(backingSize.x), static_cast<GLsizei>(backingSize.y));
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTextureID);

	return frameBufferID;
}

//-----------------------------------------------------------------------------------------------------
//
// Show info on the currently bound frame buffer
//
// Call with GL_DRAW_FRAMEBUFFER_BINDING, frameBufferName
void gl_getFramebufferInfo(GLenum target, GLuint fbo)
//-----------------------------------------------------------------------------------------------------
{
	int res, i = 0;
	GLint buffer;

	glBindFramebuffer(target,fbo);

	do
		{
		glGetIntegerv(GL_DRAW_BUFFER0+i, &buffer);

		if (buffer != GL_NONE)
		{

			con_print(CON_INFO, true, "Shader Output Location [ %d ] - color attachment [ %d ]", i, buffer - GL_COLOR_ATTACHMENT0);

			glGetFramebufferAttachmentParameteriv(target, buffer, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &res);

			con_print(CON_INFO, true, "Attachment Type [ %s ]", res==GL_TEXTURE?"Texture":"Render Buffer");

			glGetFramebufferAttachmentParameteriv(target, buffer, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &res);
			con_print(CON_INFO, true, "Attachment object name [ %d ]",res);
		}
		++i;

	} while (buffer != GL_NONE);
}

//-----------------------------------------------------------------------------------------------------
//
// Shows framebuffer limits
void gl_getFramebufferLimits()
//-----------------------------------------------------------------------------------------------------
{
	int res;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &res);
	con_print(CON_INFO, true, "Max Color Attachments [ %d ]", res);

	glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &res);
	con_print(CON_INFO, true, "Max Framebuffer Width [ %d ]", res);

	glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &res);
	con_print(CON_INFO, true, "Max Framebuffer Height [ %d ]", res);

	glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &res);
	con_print(CON_INFO, true, "Max Framebuffer Samples [ %d ]", res);

	glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &res);
	con_print(CON_INFO, true, "Max Framebuffer Layers [ %d ]", res);

}