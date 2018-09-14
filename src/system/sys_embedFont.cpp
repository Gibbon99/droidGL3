#include "hdr/system/sys_main.h"
#include "hdr/system/sys_defines.h"
#include "hdr/opengl/gl_opengl.h"
#include "hdr/opengl/gl_openGLWrap.h"
#include "hdr/system/sys_embedFont.h"

#include "hdr/libGL/freetype-gl/vec234.h"
#include "hdr/libGL/freetype-gl/vector.h"
#include "hdr/libGL/freetype-gl/vera-16.h"
#include "hdr/libGL/freetype-gl/utf8-utils.h"

const char *vertex_src = "#version 330\n"
                         "\n"
                         "in vec2 inPosition;\n"
                         "in vec2 inTextureCoords;\n"
                         "in vec4 inFontColor;\n"
                         "uniform vec2 inScreenSize;\n"
                         "\n"
                         "out vec4 theFontColor;\n"
                         "out vec2 texCoord0;\n"
                         "\n"
                         "void main(void)\n"
                         "{\n"
                         "    texCoord0 = inTextureCoords;\n"
                         "\n"
                         "    theFontColor = inFontColor;\n"
                         "\n"
                         "    vec2 vertexPosition = inPosition.xy - inScreenSize.xy; // [0..800][0..600] -> [-400..400][-300..300]\n"
                         "    vertexPosition /= inScreenSize.xy;\n"
                         "    gl_Position =  vec4(vertexPosition,0,1);\n"
                         "}\n"
                         "";

const char *fragment_src = ""
                           "#version 330\n"
                           "uniform sampler2D   inTexture0;\n"
                           "in      vec2        texCoord0;\n"
                           "in      vec4        theFontColor;\n"
                           "out     vec4        outColor;\n"
                           "\n"
                           "void main(void)\n"
                           "{\n"
                           "	float alpha = texture2D(inTexture0, texCoord0).r;\n"
                           "	outColor = vec4(theFontColor.rgb, theFontColor.a) * alpha;\n"
                           "}\n"
                           "";

typedef struct
{
	glm::vec2 position;
	glm::vec2 textureCoord;
	glm::vec4 color;
} _fnt_vertex;

GLuint embeddedTexID;

GLuint fnt_g_glyphVAO_ID;
GLuint fnt_g_vertVBO_ID;

GLuint fnt_vertexShader_ID;
GLuint fnt_fragmentShader_ID;
GLuint fnt_shaderProgram_ID;

GLuint fnt_inPosition_ID;
GLuint fnt_inTextureCoords_ID;
GLuint fnt_inColor_ID;
GLuint fnt_inTextureUnit_ID;
GLuint fnt_inScreenSize_ID;

vector <_fnt_vertex> fnt_vertex;

// -------------------------------------------------------------------
//
// Show info from compiling shaders
void fnt_printGLInfoLog ( const char *log_title, GLuint handle, const char *shader_name = 0 )
// -------------------------------------------------------------------
{
	GLchar *glString;
	GLint size;

	size = 0;
	glGetShaderiv (handle, GL_INFO_LOG_LENGTH, &size);

	if ( size > 1 )
	{
		glString = new GLchar[size];
		glGetShaderInfoLog (handle, size, &size, glString);
		con_print (CON_INFO, true, "%s info log", log_title);

		if ( shader_name != 0 )
			con_print (CON_INFO, true, " for %s:\n", shader_name);

		con_print (CON_INFO, true, "-----------\n%s\n-----------\n", glString);
		delete[] glString;
	}
}

// -------------------------------------------------------------------
//
// Compile and link the shaders for the embedded font
// Text is in source so no external files are needed
bool fnt_compileLinkShaders ()
// -------------------------------------------------------------------
{
	GLint linked;
	GLint len;

	fnt_vertexShader_ID = glCreateShader (GL_VERTEX_SHADER);
	len = (GLint) strlen (vertex_src);
	glShaderSource (fnt_vertexShader_ID, 1, &vertex_src, &len);
	glCompileShader (fnt_vertexShader_ID);
	fnt_printGLInfoLog ("Compile", fnt_vertexShader_ID, "Embedded Font Vertex Shader");

	fnt_fragmentShader_ID = glCreateShader (GL_FRAGMENT_SHADER);
	len = (GLint) strlen (fragment_src);
	glShaderSource (fnt_fragmentShader_ID, 1, &fragment_src, &len);
	glCompileShader (fnt_fragmentShader_ID);
	fnt_printGLInfoLog ("Compile", fnt_fragmentShader_ID, "Embedded Font Fragment Shader");

	fnt_shaderProgram_ID = glCreateProgram ();
	glAttachShader (fnt_shaderProgram_ID, fnt_vertexShader_ID);
	glAttachShader (fnt_shaderProgram_ID, fnt_fragmentShader_ID);

	glLinkProgram (fnt_shaderProgram_ID);

	glGetProgramiv (fnt_shaderProgram_ID, GL_LINK_STATUS, &linked);

	if ( true == linked )
	{
		con_print (CON_TEXT, true, "INFO: Shaders linked ok - [ %s ]", "Embedded font Shader.");

		glDetachShader (fnt_shaderProgram_ID, fnt_vertexShader_ID);
		glDeleteShader (fnt_vertexShader_ID);

		glDetachShader (fnt_shaderProgram_ID, fnt_fragmentShader_ID);
		glDeleteShader (fnt_fragmentShader_ID);

		glUseProgram (fnt_shaderProgram_ID);

		fnt_inPosition_ID = glGetAttribLocation (fnt_shaderProgram_ID, "inPosition");
		fnt_inTextureCoords_ID = glGetAttribLocation (fnt_shaderProgram_ID, "inTextureCoords");
		fnt_inColor_ID = glGetAttribLocation (fnt_shaderProgram_ID, "inFontColor");

		fnt_inScreenSize_ID = glGetUniformLocation (fnt_shaderProgram_ID, "inScreenSize");
		fnt_inTextureUnit_ID = glGetUniformLocation (fnt_shaderProgram_ID, "inTexture0");
		return true;
	}

	else
	{
		con_print (CON_TEXT, true, "ERROR: Shaders failed to link - [ %s ]", "Embedded font Shader.");
		return false;
	}
}

// -------------------------------------------------------------------
//
// Populate the temporary vector and push into array
void fnt_addVertexInfo ( glm::vec2 position, glm::vec2 texCoord, glm::vec4 color )
// -------------------------------------------------------------------
{
	_fnt_vertex fnt_vertexTemp;

	fnt_vertexTemp.position = position;
	fnt_vertexTemp.textureCoord = texCoord;
	fnt_vertexTemp.color = color;
	fnt_vertex.push_back (fnt_vertexTemp);
}

// -------------------------------------------------------------------
//
// Create texture containing the embedded font data
// Construct the texture and vertex positions and upload to card
// Render buffers using GL_TRIANGLES
void fnt_printText ( glm::vec2 position, glm::vec4 lineColor, const char *text, ... )
// -------------------------------------------------------------------
{
	static bool initDone = false;
	size_t i, j;
	float currentX, currentY;
	glm::vec2 fnt_screenSize;

	va_list args;
	char textLine[MAX_STRING_SIZE];
	//
	// get out the passed in parameters
	//
	va_start (args, text);
	vsnprintf (textLine, MAX_STRING_SIZE, text, args);
	va_end (args);

	currentX = position.x;
	currentY = position.y;

	fnt_vertex.clear ();

	if ( !initDone )
	{
		glGenTextures (1, &embeddedTexID);
		glBindTexture (GL_TEXTURE_2D, embeddedTexID);

		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D (GL_TEXTURE_2D, 0, GL_R8, embeddedFontData.tex_width, embeddedFontData.tex_height, 0, GL_RED, GL_UNSIGNED_BYTE, embeddedFontData.tex_data);
		//
		// Setup the Vertex Array Object that will have the VBO's associated to it
		GL_ASSERT (glGenVertexArrays (1, &fnt_g_glyphVAO_ID));
		//
		// Generate the VBO ID's for each vector
		//GL_ASSERT ( glGenBuffers ( 1, &fnt_g_vertVBO_ID ) );
		fnt_g_vertVBO_ID = wrapglGenBuffers (1, __func__);

		if ( !fnt_compileLinkShaders ())
			return;

		fnt_screenSize.x = winWidth / 2;
		fnt_screenSize.y = winHeight / 2;

		initDone = true;
	}

	//
	// Populate the vectors with the vertex and texture information for this line of text
	for ( i = 0; i != strlen (textLine); i++ )
	{
		texture_glyph_t *glyph;
		float offset_y = 0.0f;

		for ( j = 0; j < embeddedFontData.glyphs_count; ++j )
		{

			if ( embeddedFontData.glyphs[j].codepoint == ftgl::utf8_to_utf32 (textLine + i))
			{
				glyph = &embeddedFontData.glyphs[j];
				offset_y = glyph->height - glyph->offset_y;
				break;
			}
		}

		if ( !glyph )
			continue;
		// Triangle One - First point
		fnt_addVertexInfo (glm::vec2{currentX + glyph->offset_x, currentY - offset_y}, glm::vec2{glyph->s0, glyph->t1}, lineColor);

		fnt_addVertexInfo (glm::vec2{currentX + glyph->offset_x, (currentY + glyph->height) - offset_y}, glm::vec2{glyph->s0, glyph->t0}, lineColor);

		fnt_addVertexInfo (glm::vec2{currentX + glyph->width + glyph->offset_x, (currentY + glyph->height) - offset_y}, glm::vec2{glyph->s1, glyph->t0}, lineColor);
		// Triangle Two - First point
		fnt_addVertexInfo (glm::vec2{currentX + glyph->offset_x, currentY - offset_y}, glm::vec2{glyph->s0, glyph->t1}, lineColor);

		fnt_addVertexInfo (glm::vec2{currentX + glyph->width + glyph->offset_x, (currentY + glyph->height) - offset_y}, glm::vec2{glyph->s1, glyph->t0}, lineColor);

		fnt_addVertexInfo (glm::vec2{currentX + glyph->width + glyph->offset_x, currentY - offset_y}, glm::vec2{glyph->s1, glyph->t1}, lineColor);

		currentX += glyph->advance_x;
		currentY += glyph->advance_y;
	}

	//
	// Now upload and draw the line of text
	//

	//
	// Start using TTF font shader program
	GL_ASSERT (glUseProgram (fnt_shaderProgram_ID));
	//
	// Bind the generated VAO
	GL_ASSERT (glBindVertexArray (fnt_g_glyphVAO_ID));

	wrapglEnable (GL_BLEND);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	wrapglDisable (GL_DEPTH_TEST);
	//
	// Bind texture if it's not already bound as current texture
	wrapglBindTexture (GL_TEXTURE0, embeddedTexID);
	GL_ASSERT (glUniform1i (fnt_inTextureUnit_ID, 0));

	GL_ASSERT (glUniform2f (fnt_inScreenSize_ID, (float) winWidth / 2, (float) winHeight / 2));    // Pass screen size for
	//
	// Bind the vertex info
	GL_ASSERT (glBindBuffer (GL_ARRAY_BUFFER, fnt_g_vertVBO_ID));
	GL_ASSERT (glBufferData (GL_ARRAY_BUFFER, sizeof (_fnt_vertex) * fnt_vertex.size (), &fnt_vertex[0].position, GL_DYNAMIC_DRAW));
	GL_ASSERT (glVertexAttribPointer (fnt_inPosition_ID, 2, GL_FLOAT, GL_FALSE, sizeof (_fnt_vertex), (GLvoid *) offsetof (_fnt_vertex, position)));
	GL_ASSERT (glEnableVertexAttribArray (fnt_inPosition_ID));
	//
	// Bind the texture coordinates
	GL_ASSERT (glVertexAttribPointer (fnt_inTextureCoords_ID, 2, GL_FLOAT, GL_FALSE, sizeof (_fnt_vertex), (GLvoid *) offsetof (_fnt_vertex, textureCoord)));
	GL_ASSERT (glEnableVertexAttribArray (fnt_inTextureCoords_ID));
	//
	// Bind color array
	GL_ASSERT (glVertexAttribPointer (fnt_inColor_ID, 4, GL_FLOAT, GL_FALSE, sizeof (_fnt_vertex), (GLvoid *) offsetof (_fnt_vertex, color)));
	GL_ASSERT (glEnableVertexAttribArray (fnt_inColor_ID));
	//
	// No matrices used - transform is done in the shader
	GL_ASSERT (glDrawArrays (GL_TRIANGLES, 0, fnt_vertex.size ()));
}