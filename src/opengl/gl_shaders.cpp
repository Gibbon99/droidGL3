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

typedef struct
{
	GLuint      shaderID = 0;
	bool        compiled = false;
} _shaderVars;

std::map<string, _shaderVarType>            shaderUniformValues;
std::map<string, _shaderVarType>            shaderAttribValues;
std::map<string, _shaderVarType>::iterator  shaderItr;

std::map<string, _shaderVars>               shaderProgram;

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
		free(sourceText);       // Stop memory leak if shader fails to compile
		*returnObject_ID = shaderObject_ID;
		return false;
	}

	free(sourceText);
	*returnObject_ID = shaderObject_ID;

	return true;
}

//-----------------------------------------------------------------------------
//
// Return the Shader ID
GLuint gl_getShaderID( std::string shaderName)
//-----------------------------------------------------------------------------
{
	auto shaderIDIter = shaderProgram.find(shaderName);

	if (shaderIDIter == shaderProgram.end())
	{
		con_print(CON_ERROR, true, "Unable to find shaderID by name [ %s ]", shaderName.c_str());
		return 0;
	}

	if ( !shaderIDIter->second.compiled )
	{
		con_print(CON_ERROR, true, "Shader [ %s ] is not ready for use.", shaderName.c_str());
		return 0;
	}

	return shaderProgram[shaderName].shaderID;
}

//-----------------------------------------------------------------------------
//
// Set the value of a shader uniform or attribute
//
// If variable is not found, still record, but mark has invalid - count how many
// times it is called.
int gl_getAttrib ( string whichShader, string keyName )
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

		keyLocation = glGetAttribLocation (static_cast<GLuint>(gl_getShaderID(whichShader)), keyName.c_str ());

		if ( keyLocation == -1 )
		{
			con_print (CON_ERROR, true, "Shader Attrib [ %s ] Value [ %s ] does not exist.", whichShader.c_str(), keyName.c_str ());
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
			con_print (CON_ERROR, true, "Shader Uniform [ %s ] Value [ %s ] does not exist.", whichShader.c_str (), keyName.c_str ());
			varInfoType.locationID = keyLocation;
			varInfoType.refCount = 0;
			varInfoType.isFound = false;

			shaderUniformValues.emplace(u_keyName, varInfoType);

//			shaderUniformValues.insert (std::pair<string, _shaderVarType> (u_keyName, varInfoType));
			return 0;
		}

		varInfoType.locationID = keyLocation;
		varInfoType.refCount = 0;
		varInfoType.isFound = true;

		shaderUniformValues.emplace(u_keyName, varInfoType);

		//shaderUniformValues.insert (std::pair<string, _shaderVarType> (u_keyName, varInfoType));
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
bool gl_createShader ( const string &shaderName, const string &vertFileName, const string &fragFileName, const string &geomFileName )
//-----------------------------------------------------------------------------
{
	GLuint      shaderID = 0;
	GLint       linkedStatus = false;
	GLuint      vertexShaderObject, fragmentShaderObject, geometryShaderObject;
	_shaderVars tmpShaderVars;

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
	if (0 == shaderID)
	{
		con_print(CON_ERROR, true, "Unable to create GLSL program object for shader [ %s ]", vertFileName.c_str());
		return false;
	}
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
		tmpShaderVars.compiled = false;
		tmpShaderVars.shaderID = 0;
		shaderProgram.insert(std::pair<string, _shaderVars>(shaderName, tmpShaderVars));
		return false;
	}
	//
	// Store shader name and it's OpenGL ID
	tmpShaderVars.compiled = true;
	tmpShaderVars.shaderID = shaderID;

	shaderProgram.insert(std::pair<string, _shaderVars>(shaderName, tmpShaderVars));

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

	if (!gl_createShader ("quad3d", "quad3D.vert", "quad2D.frag", ""))
		return false;

	if ( !gl_createShader ("colorLine", "colorLine.vert", "colorLine.frag", ""))
		return false;

	if ( !gl_createShader ("colorDisc", "colorDisc.vert", "colorDisc.frag", ""))
		return false;

	if ( !gl_createShader ("lightmapRender", "lightmapRender.vert", "lightmapRender.frag", ""))
		return false;

	if ( !gl_createShader ("colorKey", "colorKey.vert", "colorKey.frag", ""))
		return false;

	if ( !gl_createShader ("sdfFont", "distanceField.vert", "distanceField.frag", ""))
		return false;

  if ( !gl_createShader ("colorOnly", "colorOnly.vert", "colorOnly.frag", ""))
    return false;

	return true;
}
