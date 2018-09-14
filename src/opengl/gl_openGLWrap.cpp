
#include "hdr/opengl/gl_openGLWrap.h"
#include <map>

std::map <int, string> glBuffersMap;
std::map <int, string>::iterator it;

//----------------------------------------------------------------------------
//
// Wrap OpenGL calls to minimise state changes
//
//----------------------------------------------------------------------------
int     wrapCountglBindTextureCalls = 0;
int     wrapCountglBindTextureChanges = 0;
//
// GL_CULL_FACE
int     wrapCountglEnableCullFaceCalls = 0;
int     wrapCountglEnableCullFaceChanges = 0;
int     wrapCountglDisableCullFaceCalls = 0;
int     wrapCountglDisableCullFaceChanges = 0;
//
// GL_BLEND
int     wrapCountglEnableBlendCalls = 0;
int     wrapCountglEnableBlendChanges = 0;
int     wrapCountglDisableBlendCalls = 0;
int     wrapCountglDisableBlendChanges = 0;
//
// GL_DEPTH_TEST
int     wrapCountglEnableDepthCalls = 0;
int     wrapCountglEnableDepthChanges = 0;
int     wrapCountglDisableDepthCalls = 0;
int     wrapCountglDisableDepthChanges = 0;

bool    wrap_GL_BLEND_enabled = false;
bool    wrap_GL_DEPTH_TEST_enabled = false;
bool    wrap_GL_CULL_FACE_enabled = false;

//----------------------------------------------------------------------------
//
// Display all the buffer IDs generated and their text
void wrapShowBufferInfo()
//----------------------------------------------------------------------------
{
	con_print (CON_INFO, true, "---------------Buffer IDs---------------------");
	for ( it = glBuffersMap.begin (); it != glBuffersMap.end (); it++ )
	{
		con_print (CON_INFO, true, "Buffer [ %i ] Text [ %s ]", it->first, it->second.c_str());
	}
}

//----------------------------------------------------------------------------
//
// Wrap the call to generate a buffer ID
// so we can print out some text with it
uint wrapglGenBuffers(int numBuffers, const string bufferText)
//----------------------------------------------------------------------------
{
	GLuint			bufferID;

	glGenBuffers(numBuffers, &bufferID);
	//
	// Add the generated buffer ID and the text to a STD::MAP
	glBuffersMap.insert (std::make_pair( bufferID, bufferText));

	return bufferID;
}

//----------------------------------------------------------------------------
//
// Display openGLWrap counters
void wrapShowCounters ( bool writeToLog )
//----------------------------------------------------------------------------
{
	con_print ( CON_TEXT, writeToLog, "glBindTexture - calls [ %i ] changes [ %i ]", wrapCountglBindTextureCalls, wrapCountglBindTextureChanges );
	con_print ( CON_TEXT, writeToLog, "glBlend - enable - calls [ %i ] changes [ %i ]", wrapCountglEnableBlendCalls, wrapCountglEnableBlendChanges );
	con_print ( CON_TEXT, writeToLog, "glBlend - disable - calls [ %i ] changes [ %i ]", wrapCountglDisableBlendCalls, wrapCountglDisableBlendChanges );
	con_print ( CON_TEXT, writeToLog, "glDepthTest - enable - calls [ %i ] changes [ %i ]", wrapCountglEnableDepthCalls, wrapCountglEnableDepthChanges );
	con_print ( CON_TEXT, writeToLog, "glDepthTest - disable - calls [ %i ] changes [ %i ]", wrapCountglDisableDepthCalls, wrapCountglDisableDepthChanges );
	con_print ( CON_TEXT, writeToLog, "glCullFace - enable - calls [ %i ] changes [ %i ]", wrapCountglEnableCullFaceCalls, wrapCountglEnableCullFaceChanges );
	con_print ( CON_TEXT, writeToLog, "glCullFace - disable - calls [ %i ] changes [ %i ]", wrapCountglDisableCullFaceCalls, wrapCountglDisableCullFaceChanges );
}

//----------------------------------------------------------------------------
//
// Wrap glDisable
void wrapglDisable ( int whichState )
//----------------------------------------------------------------------------
{
	switch ( whichState )
		{

		case GL_CULL_FACE:
		{
			wrapCountglDisableCullFaceCalls++;

			if ( true == wrap_GL_CULL_FACE_enabled )
				{
					glDisable ( GL_CULL_FACE );
					wrapCountglDisableCullFaceChanges++;
					wrap_GL_CULL_FACE_enabled = false;
				}
		}
		break;

		case GL_BLEND:
		{
			wrapCountglDisableBlendCalls++;

			if ( true == wrap_GL_BLEND_enabled )
				{
					glDisable ( GL_BLEND );
					wrapCountglDisableBlendChanges++;
					wrap_GL_BLEND_enabled = false;
				}
		}
		break;

		case GL_DEPTH_TEST:
			wrapCountglDisableDepthCalls++;

			if ( true == wrap_GL_DEPTH_TEST_enabled )
				{
					glDisable ( GL_DEPTH_TEST );
					wrapCountglDisableDepthChanges++;
					wrap_GL_DEPTH_TEST_enabled = false;
				}

			break;
		}
}

//----------------------------------------------------------------------------
//
// Warap glEnable
void wrapglEnable ( int whichState )
//----------------------------------------------------------------------------
{
	switch ( whichState )
		{
		case GL_BLEND:
			wrapCountglEnableBlendCalls++;

			if ( false == wrap_GL_BLEND_enabled )
				{
					glEnable ( GL_BLEND );
					wrapCountglEnableBlendChanges++;
					wrap_GL_BLEND_enabled = true;
				}

			break;

		case GL_DEPTH_TEST:
			wrapCountglDisableDepthCalls++;

			if ( false == wrap_GL_DEPTH_TEST_enabled )
				{
					glEnable ( GL_DEPTH_TEST );
					wrapCountglDisableDepthChanges++;
					wrap_GL_DEPTH_TEST_enabled = true;
				}

			break;
		}
}


//----------------------------------------------------------------------------
//
// Wrap glBindTexture
void wrapglBindTexture ( int whichTextureUnit, int whichTexture )
//----------------------------------------------------------------------------
{
	static int boundTexture0, boundTexture1, boundTexture2, boundTexture3 = 0;

	wrapCountglBindTextureCalls++;

	switch ( whichTextureUnit )
		{
		case GL_TEXTURE0:
			if ( whichTexture != boundTexture0 )
				{
					glActiveTexture ( GL_TEXTURE0 );
					glBindTexture ( GL_TEXTURE_2D, whichTexture );
					boundTexture0 = whichTexture;
					wrapCountglBindTextureChanges++;
				}

			break;

		case GL_TEXTURE1:
			if ( whichTexture != boundTexture1 )
				{
					glActiveTexture ( GL_TEXTURE1 );
					glBindTexture ( GL_TEXTURE_2D, whichTexture );
					boundTexture1 = whichTexture;
					wrapCountglBindTextureChanges++;
				}

			break;

		case GL_TEXTURE2:
			if ( whichTexture != boundTexture2 )
				{
					glActiveTexture ( GL_TEXTURE2 );
					glBindTexture ( GL_TEXTURE_2D, whichTexture );
					boundTexture2 = whichTexture;
					wrapCountglBindTextureChanges++;
				}

			break;

		case GL_TEXTURE3:
			if ( whichTexture != boundTexture3 )
				{
					glActiveTexture ( GL_TEXTURE3 );
					glBindTexture ( GL_TEXTURE_2D, whichTexture );
					boundTexture3 = whichTexture;
					wrapCountglBindTextureChanges++;
				}

			break;

		default:
			con_print ( CON_ERROR, true, "Invalid parameters passed to [ %s ]", __func__ );
			break;
		}
}
