#include "hdr/system/sys_main.h"
#include "hdr/opengl/gl_opengl.h"
#include "hdr/opengl/gl_openGLWrap.h"
#include "hdr/opengl/gl_shaders.h"
#include "hdr/game/s_shadows.h"

#ifdef __linux__
	#include <execinfo.h>
#endif

#include <map>
#include <hdr/io/io_textures.h>

typedef struct
{
	int counter{};
	int errorID{};
	string source;
	string type;
	string severity;
	string message;
} _errorMessage;

std::map<int, _errorMessage> shaderErrorsMap;
std::map<int, _errorMessage>::iterator it;

int __gl_error_code;

uint quadVBO = 0;
uint quadVAO = 0;

glm::mat4   MVP;

float nearPlane;
float farPlane;
float cameraDistance;

float g_gamma;

//-----------------------------------------------------------------------------
//
// Get all the openGL errors
void gl_getAllGLErrors ( int errorNum, const char *calledFrom, int line )
//-----------------------------------------------------------------------------
{
	errorNum = glGetError ();

	while ( errorNum != GL_NO_ERROR )
	{
		con_print (CON_TEXT, true, "OpenGL Error: [ %i ] - [ %i ] - [ %s ]", line, errorNum, calledFrom);

		switch ( errorNum )
		{
			case GL_INVALID_ENUM:
				con_print (CON_TEXT, true, "[ %s ]", "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.");
				break;

			case GL_INVALID_VALUE:
				con_print (CON_TEXT, true, "[ %s ]", "GL_INVALID_VALUE: A numeric argument is out of range.");
				break;

			case GL_INVALID_OPERATION:
				con_print (CON_TEXT, true, "[ %s ]", "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.");
				break;

			case GL_OUT_OF_MEMORY:
				con_print (CON_TEXT, true, "[ %s ]", "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.");
				break;

			default:
				con_print (CON_TEXT, true, "[ %s ]", "Unknown error.");
				break;
		}

		errorNum = glGetError ();
	}
}

//--------------------------------------------------------------------------------------------
//
// OpenGL Debug - return string of type source
//
//Parsing code from OpenGL Shader Language CookBook SE
//https://github.com/daw42/glslcookbook
//
std::string getStringForType ( GLenum type )
//--------------------------------------------------------------------------------------------
{
	switch ( type )
	{
		case GL_DEBUG_TYPE_ERROR:
			return "Error";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			return "Deprecated behavior";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			return "Undefined behavior";
		case GL_DEBUG_TYPE_PORTABILITY:
			return "Portability issue";
		case GL_DEBUG_TYPE_PERFORMANCE:
			return "Performance issue";
		case GL_DEBUG_TYPE_MARKER:
			return "Stream annotation";
		case GL_DEBUG_TYPE_OTHER:
			return "Other";
		default:
			assert (false);
			return "";
	}
}

//--------------------------------------------------------------------------------------------
//
// OpenGL Debug - return string of error source
std::string getStringForSource ( GLenum source )
//--------------------------------------------------------------------------------------------
{
	switch ( source )
	{
		case GL_DEBUG_SOURCE_API:
			return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			return "Window system";
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			return "Shader compiler";
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			return "Third party";
		case GL_DEBUG_SOURCE_APPLICATION:
			return "Application";
		case GL_DEBUG_SOURCE_OTHER:
			return "Other";
		default:
			assert (false);
			return "";
	}
}

//--------------------------------------------------------------------------------------------
//
// OpenGL Debug - get string for severity
std::string getStringForSeverity ( GLenum severity )
//--------------------------------------------------------------------------------------------
{
	switch ( severity )
	{
		case GL_DEBUG_SEVERITY_HIGH:
			return "High";
		case GL_DEBUG_SEVERITY_MEDIUM:
			return "Medium";
		case GL_DEBUG_SEVERITY_LOW:
			return "Low";
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			return "Notification";
		default:
			assert (false);
			return ("");
	}
}

//--------------------------------------------------------------------------------------------
//
// Obtain a backtrace and print it to stdout.
void print_trace () // TODO Move to better file
//--------------------------------------------------------------------------------------------
{
#ifdef __linux__

	void *array[10];
	size_t size;
	char **strings;
	size_t i;

	size = static_cast<size_t>(backtrace (array, 10));
	strings = backtrace_symbols (array, static_cast<int>(size));

	printf ("Obtained %zd stack frames.\n", size);

	for ( i = 0; i < size; i++ )
		printf ("%s\n", strings[i]);

	free (strings);
#endif
}



//--------------------------------------------------------------------------------------------
//
// Print out all the GLSL error messages
void gl_displayErrors ()
//--------------------------------------------------------------------------------------------
{
	con_print (CON_INFO, true, "---------------------------------------------------");
	for ( it = shaderErrorsMap.begin (); it != shaderErrorsMap.end (); it++ )
	{
		con_print (CON_INFO, true, "Error count [ %i ]", it->second.counter);
		con_print (CON_INFO, true, "Error ID [ %i ]", it->first);
		con_print (CON_INFO, true, "Source [ %s ]", it->second.source.c_str ());
		con_print (CON_INFO, true, "Type [ %s ]", it->second.type.c_str ());
		con_print (CON_INFO, true, "Severity [ %s ]", it->second.severity.c_str ());
		con_print (CON_INFO, true, "Message [ %s ]", it->second.message.c_str ());
		con_print (CON_INFO, true, "---------------------------------------------------");
		con_print (CON_INFO, true, "");
	}
//	print_trace();
}

//--------------------------------------------------------------------------------------------
//
// OpenGL Debug Callback
void APIENTRY gl_DebugCallback ( GLenum source, GLenum type, GLenum id, GLenum severity, GLsizei length, const GLchar *msg, const void *data )
//--------------------------------------------------------------------------------------------
{
//	if ( false == g_debugOpenGL )
//		return;

	it = shaderErrorsMap.find (id);
	if ( it != shaderErrorsMap.end ())
	{
		it->second.counter++;
	}
	else
	{
		_errorMessage errMessage;

		errMessage.counter = 1;
		errMessage.source = getStringForSource (source);
		errMessage.type = getStringForType (type);
		errMessage.severity = getStringForSeverity (severity);
		errMessage.message = msg;

		shaderErrorsMap.insert (std::make_pair (id, errMessage));

		con_print (CON_ERROR, true, "ID [ %i ]", id);
		con_print (CON_ERROR, true, "source [ %s ]", getStringForSource (source).c_str ());
		con_print (CON_ERROR, true, "type [ %s ]", getStringForType (type).c_str ());
		con_print (CON_ERROR, true, "severity [ %s ]", getStringForSeverity (severity).c_str ());
		con_print (CON_ERROR, true, "debug call [ %s ]", msg);
	}
}

//--------------------------------------------------------------------------------------------
//
// OpenGL Debug - Register callback
void gl_registerDebugCallback ()
//--------------------------------------------------------------------------------------------
{
	glEnable (GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback (gl_DebugCallback, nullptr);
	glDebugMessageControl (GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
}

//-----------------------------------------------------------------------------
//
// Draw a 2D quad
void gl_draw2DQuad ( glm::vec2 position, glm::vec2 quadSize, string whichShader, GLuint whichTexture, float interpolation)
//-----------------------------------------------------------------------------
{
	glm::vec2           quadVerts[4];
	static GLuint       vao = 0;
	static GLuint       buffers[2];
	static bool         initDone = false;

	glm::vec2           viewPosition;

	viewPosition.x += position.x + (currentVelocity.x * interpolation);
	viewPosition.y += position.y + (currentVelocity.y * interpolation);

	quadVerts[0].x = viewPosition.x;
	quadVerts[0].y = viewPosition.y;

	quadVerts[1].x = viewPosition.x;
	quadVerts[1].y = viewPosition.y + quadSize.y;

	quadVerts[2].x = viewPosition.x + quadSize.x;
	quadVerts[2].y = viewPosition.y + quadSize.y;

	quadVerts[3].x = viewPosition.x + quadSize.x;
	quadVerts[3].y = viewPosition.y;

	GLfloat quadTexCoords[] = {0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0,};

	if ( !initDone )
	{
		// create the VAO
		GL_ASSERT (glGenVertexArrays (1, &vao));
		GL_CHECK (glBindVertexArray (vao));

		// Create buffers for the vertex data
		buffers[0] = wrapglGenBuffers (1, __func__);
		buffers[1] = wrapglGenBuffers (1, __func__);

		GL_CHECK (glUseProgram (gl_getShaderID(whichShader)));

		// Vertex coordinates buffer
		GL_ASSERT (glBindBuffer (GL_ARRAY_BUFFER, buffers[0]));
		GL_CHECK (glBufferData (GL_ARRAY_BUFFER, sizeof (quadVerts), quadVerts, GL_DYNAMIC_DRAW));
		GL_CHECK (glEnableVertexAttribArray (gl_getAttrib(whichShader, "inPosition")));
		GL_CHECK (glVertexAttribPointer (gl_getAttrib (whichShader, "inPosition"), 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET (0)));

		// Texture coordinates buffer
		GL_ASSERT (glBindBuffer (GL_ARRAY_BUFFER, buffers[1]));
		GL_CHECK (glBufferData (GL_ARRAY_BUFFER, sizeof (quadTexCoords), quadTexCoords, GL_DYNAMIC_DRAW));
		GL_CHECK (glEnableVertexAttribArray (gl_getAttrib(whichShader, "inTextureCoords")));
		GL_CHECK (glVertexAttribPointer (gl_getAttrib (whichShader, "inTextureCoords"), 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET (0)));

		initDone = false;
	}

	GL_CHECK (glUseProgram (gl_getShaderID(whichShader)));
	//
	// Bind texture if it's not already bound as current texture
	GL_CHECK (glActiveTexture (GL_TEXTURE0));

	GL_CHECK (glBindTexture (GL_TEXTURE_2D, whichTexture));

	GL_CHECK (glUniform1i (gl_getUniform (whichShader, "inTexture0"), 0));
	GL_CHECK (glUniform1f (gl_getUniform (whichShader, "gamma"), 1.0));
	GL_CHECK (glUniform1f (gl_getUniform (whichShader, "outerRadius"), 0.2f));

	GL_CHECK (glUniform2f (gl_getUniform (whichShader, "inScreenSize"), (float) winWidth / 2, (float) winHeight / 2));

	GL_CHECK (glBindVertexArray (vao));
	//
	// Enable attribute to hold vertex information
	GL_CHECK (glEnableVertexAttribArray (gl_getAttrib(whichShader, "inPosition")));
	GL_CHECK (glEnableVertexAttribArray (gl_getAttrib(whichShader, "inTextureCoords")));

	GL_CHECK (glDrawArrays (GL_TRIANGLE_FAN, 0, 4));

	glDeleteBuffers (2, buffers);
	glDeleteVertexArrays (1, &vao);
}


//--------------------------------------------------------------------------------------------
//
// Draw a debug line
void gl_drawLine ( const glm::vec3 startPoint, const glm::vec3 endPoint, const string whichShader, glm::vec4 lineColor )
//--------------------------------------------------------------------------------------------
{
	static      GLuint lineVAO = 0;
	static      GLuint buffers[2];
	static bool initDone = false;

	glm::vec3   lineCoords[2];

	lineCoords[0] = startPoint;
	lineCoords[1] = endPoint;

	if ( !initDone )
	{
		GL_ASSERT (glGenVertexArrays (1, &lineVAO));
		GL_CHECK ( glBindVertexArray(lineVAO));

		// Create buffers for the attrib pointers
		buffers[0] = wrapglGenBuffers (1, __func__);

		GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));

		// Vertex coordinates buffer
		GL_ASSERT (glBindBuffer (GL_ARRAY_BUFFER, buffers[0]));
		GL_CHECK (glBufferData (GL_ARRAY_BUFFER, sizeof (lineCoords), lineCoords, GL_DYNAMIC_DRAW));
		GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inPosition")));
		GL_CHECK (glVertexAttribPointer (gl_getAttrib (whichShader, "inPosition"), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET (0)));

		initDone = false;
	}

	GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));

//	GL_CHECK (glUniform2f (gl_getUniform (whichShader, "inScreenSize"), (float) winWidth / 2, (float) winHeight / 2));
	GL_CHECK (glUniform4fv (gl_getUniform (whichShader, "inColor"), 1, glm::value_ptr(lineColor)));
	GL_CHECK (glUniform1f (gl_getUniform (whichShader, "gamma"), 1.0));

	GL_CHECK (glBindVertexArray (lineVAO));

	//
	// Enable attribute to hold vertex information
	GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inPosition")));
	GL_ASSERT (glUniformMatrix4fv (gl_getUniform (whichShader, "MVP_Matrix"), 1, false, glm::value_ptr (MVP)));

	GL_CHECK (glDrawArrays (GL_LINES, 0, 2));

	glUseProgram (0);
	glBindVertexArray (0);

	glDeleteBuffers (1, buffers);
	glDeleteVertexArrays (1, &lineVAO);
}

//-----------------------------------------------------------------------------
//
// Set OpenGL matrices
void gl_setupMatrixes ( float interpolate )
//-----------------------------------------------------------------------------
{
	glm::mat4       projMatrix;
	glm::mat4       viewMatrix;
	glm::mat4       modelMatrix;

	glm::vec3       camPosition;
	glm::vec3       camTarget;
	glm::vec3       upVector = vec3 (0.0f, 1.0f, 0.0f);

	projMatrix = glm::perspective (60.0f, (float) winWidth / (float) winHeight, nearPlane, farPlane);

	camPosition.x = quadPosition.x + (currentVelocity.x * interpolate);
	camPosition.y = quadPosition.y + (currentVelocity.y * interpolate);
	camPosition.z = quadPosition.z + cameraDistance;       // Distance camera is away from the viewpoint

	camTarget.x = quadPosition.x + (currentVelocity.x * interpolate);
	camTarget.y = quadPosition.y + (currentVelocity.y * interpolate);
	camTarget.z = 0.0f;

	viewMatrix = glm::lookAt (camPosition, camTarget, upVector );

	modelMatrix = glm::mat4 ();

	MVP = projMatrix * viewMatrix * modelMatrix;
}


//-----------------------------------------------------------------------------
//
// Draw a polygon
void gl_drawPolygon ( std::set<_shadowHullPoint> const &drawShadowHull, glm::vec3 mousePosition, string whichShader )
//-----------------------------------------------------------------------------
{
	static GLuint vao = 0;
	static GLuint buffers[2];
	static bool initDone = false;

	glm::vec4 lineColor{1,1,0,1};

	vector<glm::vec3>       hullPoints;

	hullPoints.clear();

	hullPoints.push_back (mousePosition);
	for (auto sourceItr : drawShadowHull)
	{
		hullPoints.push_back(sourceItr.position);
	}
	auto sourceItr = drawShadowHull.begin();
	hullPoints.push_back (sourceItr->position);
//	hullPoints.push_back (mousePosition);


	if ( !initDone )
	{
		// create the VAO
		GL_ASSERT (glGenVertexArrays (1, &vao));
		GL_CHECK (glBindVertexArray (vao));

		// Create buffers for the vertex data
		buffers[0] = wrapglGenBuffers (1, __func__);
		buffers[1] = wrapglGenBuffers (1, __func__);

		GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));

		// Vertex coordinates buffer
		GL_ASSERT (glBindBuffer (GL_ARRAY_BUFFER, buffers[0]));
		GL_CHECK (glBufferData (GL_ARRAY_BUFFER, sizeof (glm::vec3) * hullPoints.size(), &hullPoints[0], GL_DYNAMIC_DRAW));
		GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inPosition")));
		GL_CHECK (glVertexAttribPointer (gl_getAttrib (whichShader, "inPosition"), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET (0)));

		initDone = false;
	}

	GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));

//	GL_CHECK (glUniform2f (gl_getUniform (whichShader, "inScreenSize"), (float) winWidth / 2, (float) winHeight / 2));
	GL_CHECK (glUniform4fv (gl_getUniform (whichShader, "inColor"), 1, glm::value_ptr (lineColor)));
	GL_CHECK (glUniform1f (gl_getUniform (whichShader, "gamma"), 1.0));

	GL_CHECK (glBindVertexArray (vao));
	//
	// Enable attribute to hold vertex information
	GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inPosition")));
	GL_ASSERT (glUniformMatrix4fv (gl_getUniform (whichShader, "MVP_Matrix"), 1, false, glm::value_ptr (MVP)));

	GL_CHECK (glDrawArrays (GL_TRIANGLE_FAN, 0, hullPoints.size ()));

	glDeleteBuffers (2, buffers);
	glDeleteVertexArrays (1, &vao);
}
