#include "hdr/system/sys_main.h"
#include "hdr/io/io_fileSystem.h"
#include "hdr/opengl/gl_shaders.h"
#include <map>

typedef struct
{
	GLint   locationID;
	int     refCount;
	bool    isFound;
} _shaderVarType;

std::map<string, _shaderVarType> shaderUniformValues;
std::map<string, _shaderVarType> shaderAttribValues;
std::map<string, _shaderVarType>::iterator shaderItr;

std::map<string, int> shaderProgram;

//-----------------------------------------------------------------------------
//
// Get errors from shaders
void gl_getGLSLError ( GLuint object, int objectType )
//-----------------------------------------------------------------------------
{
	int logLength = 0;
	GLint charsWritten;
	GLchar *infoLog;

	switch ( objectType )
	{
		case GLSL_SHADER:
			glGetShaderiv (object, GL_INFO_LOG_LENGTH, &logLength);
			break;

		case GLSL_PROGRAM:
			glGetProgramiv (object, GL_INFO_LOG_LENGTH, &logLength);
			break;

		default:
			break;
	}

	if ( logLength > 0 )
	{
		infoLog = (GLchar *) malloc (logLength);

		if ( nullptr == infoLog )
		{
			con_print(CON_ERROR, true, "Couldn't get memory to store GLSL error log.");
			return;
		}

		switch ( objectType )
		{
			case GLSL_SHADER:
				glGetShaderInfoLog (object, logLength, &charsWritten, infoLog);
				break;

			case GLSL_PROGRAM:
				glGetProgramInfoLog (object, logLength, &charsWritten, infoLog);
				break;

			default:
				con_print(CON_ERROR, true, "ERROR: Invalid object type passed to sys_GetGLSLError");
				return;
				break;
		}

		con_print(CON_ERROR, true, "GLSL ERROR [ %s ]", infoLog);
		free (infoLog);
	}
}

//-----------------------------------------------------------------------------
//
// Load the shader from the text file into memory and compile
// Return shaderObject ID as param
bool gl_shaderLoadAndCompile ( const string fileName, GLuint *returnObject_ID, int shaderType )
//-----------------------------------------------------------------------------
{
	GLint           compiled;
	PHYSFS_sint64   fileLength;
	GLuint          shaderObject_ID;
	char            *sourceText = nullptr;

	fileLength = io_getFileSize ((char *) fileName.c_str ());
	if ( fileLength == -1 )
		return false;

	sourceText = (char *) malloc (sizeof (char) * (fileLength + 1));
	if ( nullptr == sourceText )
		return false;

	if ( -1 == io_getFileIntoMemory ((char *) fileName.c_str (), sourceText))
	{
		free(sourceText);
		return false;
	}
	//
	// Make sure it's null terminated   // Need fileLength + 1 ??
	sourceText[fileLength] = '\0';

	switch ( shaderType )
	{
		case GL_VERTEX_SHADER:
			shaderObject_ID = glCreateShader (GL_VERTEX_SHADER);
			break;

		case GL_FRAGMENT_SHADER:
			shaderObject_ID = glCreateShader (GL_FRAGMENT_SHADER);
			break;

		case GL_GEOMETRY_SHADER:
			shaderObject_ID = glCreateShader (GL_GEOMETRY_SHADER);
			break;

		default:
			con_print (CON_ERROR, true, "Invalid shader type passed.");
			return false;
			break;
	}

	if ( shaderObject_ID == 0 )
	{
		con_print (CON_ERROR, true, "Error: Failed to create shader ID for [ %s ]", fileName.c_str());
		return false;
	}

	glShaderSource (shaderObject_ID, 1, (const GLchar **) &sourceText, nullptr);

	glCompileShader (shaderObject_ID);
	//
	// Check it compiled ok
	glGetShaderiv (shaderObject_ID, GL_COMPILE_STATUS, &compiled);

	if ( !compiled )
	{
		con_print (CON_TEXT, true, "Shader compile failed [ %s]", fileName.c_str ());
		gl_getGLSLError (shaderObject_ID, GLSL_SHADER);
		return false;
	}

	free(sourceText);
	*returnObject_ID = shaderObject_ID;

	return true;
}

//-----------------------------------------------------------------------------
//
// Return the Shader ID
GLint gl_getShaderID(string shaderName)
//-----------------------------------------------------------------------------
{
	std::map<std::string, int>::const_iterator shaderIDIter;

	shaderIDIter = shaderProgram.find(shaderName);
	if (shaderIDIter == shaderProgram.end())
	{
		con_print(CON_ERROR, true, "Unable to find shaderID by name [ %s ]", shaderName);
		return -1;
	}

	return shaderProgram[shaderName];
}

//-----------------------------------------------------------------------------
//
// Set the value of a shader uniform or attribute
//
// If variable is not found, still record, but mark has invalid - count how many
// times it is called.
GLint gl_getAttrib ( string whichShader, string keyName )
//-----------------------------------------------------------------------------
{
	string          u_keyName;
	GLint           keyLocation = 0;
	_shaderVarType  varInfoType;

	u_keyName = whichShader + keyName;

	shaderItr = shaderAttribValues.find (u_keyName);
	if ( shaderItr == shaderAttribValues.end ())   // Key doesn't exist
	{
		con_print (CON_INFO, true, "Key [ %s ] doesn't exist in map.", u_keyName.c_str ());

		keyLocation = glGetAttribLocation (gl_getShaderID(whichShader), keyName.c_str ());

		if ( keyLocation == -1 )
		{
			con_print (CON_ERROR, true, "Shader [ %s ] Value [ %s ] does not exist.", whichShader.c_str(), keyName.c_str ());
			varInfoType.locationID = keyLocation;
			varInfoType.refCount = 0;
			varInfoType.isFound = false;
			shaderAttribValues.insert (std::pair<string, _shaderVarType> (u_keyName, varInfoType));
			return 0;
		}

		varInfoType.locationID = keyLocation;
		varInfoType.refCount = 0;
		varInfoType.isFound = true;

		shaderAttribValues.insert (std::pair<string, _shaderVarType> (u_keyName, varInfoType));
		return keyLocation;
	}
	else    // already used in the map
	{
		if ( shaderItr->second.isFound )
		{
			shaderItr->second.refCount++;
			return shaderItr->second.locationID;
		}
		else
		{
			shaderItr->second.refCount++;
			return -1;  // Count how many times it was called, and not found
		}
	}
}


//-----------------------------------------------------------------------------
//
// Set the value of a shader uniform or attribute
//
// If variable is not found, still record, but mark has invalid - count how many
// times it is called.
GLint gl_getUniform ( string whichShader, string keyName )
//-----------------------------------------------------------------------------
{
	string u_keyName;
	GLint keyLocation = 0;
	_shaderVarType varInfoType;

	u_keyName = whichShader + keyName;

	shaderItr = shaderUniformValues.find (u_keyName);
	if ( shaderItr == shaderUniformValues.end ())   // Key doesn't exist
	{
		con_print (CON_INFO, true, "Key [ %s ] doesn't exist in map.", u_keyName.c_str ());

		keyLocation = glGetUniformLocation (gl_getShaderID (whichShader), keyName.c_str ());

		if ( keyLocation == -1 )
		{
			con_print (CON_ERROR, true, "Shader [ %s ] Value [ %s ] does not exist.", whichShader.c_str (), keyName.c_str ());
			varInfoType.locationID = keyLocation;
			varInfoType.refCount = 0;
			varInfoType.isFound = false;
			shaderUniformValues.insert (std::pair<string, _shaderVarType> (u_keyName, varInfoType));
			return 0;
		}

		varInfoType.locationID = keyLocation;
		varInfoType.refCount = 0;
		varInfoType.isFound = true;

		shaderUniformValues.insert (std::pair<string, _shaderVarType> (u_keyName, varInfoType));
		return keyLocation;
	}
	else    // already used in the map
	{
		if ( shaderItr->second.isFound )
		{
			shaderItr->second.refCount++;
			return shaderItr->second.locationID;
		}
		else
		{
			shaderItr->second.refCount++;
			return -1;  // Count how many times it was called, and not found
		}
	}
}


//-----------------------------------------------------------------------------
//
// Create a shader object from the source files and indexed by enum
bool gl_createShader ( string shaderName, string vertFileName, string fragFileName, string geomFileName )
//-----------------------------------------------------------------------------
{
	GLint shaderID;
	GLint linkedStatus;
	GLuint vertexShaderObject, fragmentShaderObject, geometryShaderObject;

	//
	// Load the shaders and set their object ID
	if ( !gl_shaderLoadAndCompile (vertFileName, &vertexShaderObject, GL_VERTEX_SHADER))
	{
		con_print (CON_ERROR, true, "Error loading and compiling Vertex shader [ %s ]", vertFileName.c_str());
		return false;
	}

	if ( !gl_shaderLoadAndCompile (fragFileName, &fragmentShaderObject, GL_FRAGMENT_SHADER))
	{
		con_print (CON_ERROR, true, "Error loading and compiling Fragment shader [ %s ]", fragFileName.c_str ());
		return false;
	}

	if ( geomFileName.size () > 1 )

		if ( !gl_shaderLoadAndCompile (geomFileName, &geometryShaderObject, GL_GEOMETRY_SHADER))
		{
			con_print (CON_ERROR, true, "Error loading and compiling Geometry shader [ %s ]", geomFileName.c_str ());
			return false;
		}
	//
	// Create the program ID
	shaderID = glCreateProgram ();
	//
	// Attach the object ID to the program ID ready for compiling
	GL_CHECK (glAttachShader (shaderID, vertexShaderObject));
	GL_CHECK (glAttachShader (shaderID, fragmentShaderObject));

	if ( geomFileName.size() > 1 )
	{
		GL_CHECK (glAttachShader (shaderID, geometryShaderObject));
	}
	//
	// Link the shaders
	GL_CHECK (glLinkProgram (shaderID));
	//
	// Get the status
	GL_CHECK (glGetProgramiv (shaderID, GL_LINK_STATUS, &linkedStatus));

	if ( !linkedStatus )   // true
	{
		con_print (CON_ERROR, true, "ERROR: Shader failed to link - [ %s ]", shaderName.c_str());
		gl_getGLSLError (shaderID, GLSL_PROGRAM);
		return false;
	}
	//
	// Store shader name and it's OpenGL ID
	shaderProgram.insert(std::pair<string, int>(shaderName, shaderID));

	return true;
}

//-----------------------------------------------------------------------------
//
// Add all the shaders
bool gl_addShaders ()
//-----------------------------------------------------------------------------
{
	if (!gl_createShader ("quad2d", "quad2D.vert", "quad2D.frag", ""))
		return false;

	if ( !gl_createShader ("colorLine", "colorLine.vert", "colorLine.frag", ""))
		return false;


	return true;
}
