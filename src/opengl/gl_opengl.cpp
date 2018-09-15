#include "hdr/system/sys_main.h"
#include "hdr/opengl/gl_opengl.h"

#ifdef __linux__
	#include <execinfo.h>
#endif

#include <map>

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

#ifdef __linux__

/* Obtain a backtrace and print it to stdout. */
void print_trace ()
{
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
}

#endif

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
