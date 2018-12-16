
#include "hdr/libGL/freetype-gl/utf8-utils.h"
#include "hdr/opengl/gl_shaders.h"
#include "hdr/system/sys_sdfFont.h"
#include "hdr/libGL/freetype-gl/vertex-buffer.h"
#include "hdr/libGL/freetype-gl/digitalFont.h"

GLuint            sdfFontTexID;
GLuint            sdfFnt_g_glyphVAO_ID;
GLuint            sdfFnt_g_vertVBO_ID;

GLuint            sdfFnt_inPosition_ID;
GLuint            sdfFnt_inTextureCoords_ID;
GLuint            sdfFnt_inColor_ID;
GLuint            sdfFnt_inOutlineColor_ID;
GLuint            sdfFnt_inTextureUnit_ID;
GLuint            sdfFnt_inScreenSize_ID;

typedef struct
{
	glm::vec2 position;
	glm::vec2 textureCoord;
	glm::vec4 color;
} _fnt_vertex;

vector <_fnt_vertex> sdfFont_vertex;

// -------------------------------------------------------------------
//
// Populate the temporary vector and push into array
void sdfFont_addVertexInfo ( glm::vec2 position, glm::vec2 texCoord, glm::vec4 color )
// -------------------------------------------------------------------
{
	_fnt_vertex fnt_vertexTemp;

	fnt_vertexTemp.position = position;
	fnt_vertexTemp.textureCoord = texCoord;
	fnt_vertexTemp.color = color;
	sdfFont_vertex.push_back (fnt_vertexTemp);
}

// -------------------------------------------------------------------
//
// Return the width of a text string
float sdfFont_getStringWidth(string format, ...)
// -------------------------------------------------------------------
{
	va_list         args, args_copy;
	float           currentWidth = 0.0f;
	string          tempString;

	va_start( args, format );
	va_copy( args_copy, args );

	const auto sz = std::vsnprintf ( nullptr, 0, format.c_str (), args ) + 1;

	try
	{
		std::string result ( sz, ' ' );
		std::vsnprintf ( &result.front (), sz, format.c_str (), args_copy );

		va_end( args_copy );
		va_end( args );

		for ( int i = 0; i != result.size (); i++ )
		{
			texture_glyph_t *glyph = nullptr;

			for ( int j = 0; j < sdfFontData.glyphs_count; ++j )
			{
				tempString[0] = result.at(i);

				if ( sdfFontData.glyphs[j].codepoint == ftgl::utf8_to_utf32 (tempString.c_str()))
				{
					glyph = &sdfFontData.glyphs[j];
					break;
				}
			}

			if ( !glyph )
				continue;
			currentWidth += glyph->width;
//			currentWidth += glyph->advance_x;
		}
		return currentWidth;
	}

	catch ( const std::bad_alloc & )
	{
		va_end( args_copy );
		va_end( args );
		printf ( "Error allocating string parsing in sdfFont_getStringWidth.\n" );
	}

	return 0;
}



// -------------------------------------------------------------------
//
// Create texture containing the embedded font data
// Construct the texture and vertex positions and upload to card
// Render buffers using GL_TRIANGLES
void sdfFont_printText ( glm::vec2 position, glm::vec4 lineColor, glm::vec4 outLineColor, string whichShader, const char *text, ... )
// -------------------------------------------------------------------
{
	static bool         initDone = false;
	size_t              i, j;
	float               currentX, currentY;
	va_list             args;
	char                textLine[MAX_STRING_SIZE];

	//
	// get out the passed in parameters
	//
	va_start (args, text);
	vsnprintf (textLine, MAX_STRING_SIZE, text, args);
	va_end (args);

	currentX = position.x;
	currentY = position.y;

	sdfFont_vertex.clear ();

	if ( !initDone )
	{
		glGenTextures (1, &sdfFontTexID);

		glBindTexture (GL_TEXTURE_2D, sdfFontTexID);

		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D (GL_TEXTURE_2D, 0, GL_R8, static_cast<GLsizei>(sdfFontData.tex_width),
		              static_cast<GLsizei>(sdfFontData.tex_height), 0, GL_RED, GL_UNSIGNED_BYTE, sdfFontData.tex_data);
		//
		// Setup the Vertex Array Object that will have the VBO's associated to it
		GL_ASSERT (glGenVertexArrays (1, &sdfFnt_g_glyphVAO_ID));
		//
		// Generate the VBO ID's for each vector
		//GL_ASSERT ( glGenBuffers ( 1, &fnt_g_vertVBO_ID ) );
		sdfFnt_g_vertVBO_ID = wrapglGenBuffers (1, __func__);

		sdfFnt_inPosition_ID = static_cast<GLuint>(glGetAttribLocation (gl_getShaderID(whichShader), "inPosition"));
		sdfFnt_inTextureCoords_ID = static_cast<GLuint>(glGetAttribLocation (gl_getShaderID(whichShader), "inTextureCoords"));
		sdfFnt_inColor_ID = static_cast<GLuint>(glGetAttribLocation (gl_getShaderID(whichShader), "inFontColor"));
		sdfFnt_inOutlineColor_ID = static_cast<GLuint>(glGetAttribLocation (gl_getShaderID(whichShader), "inFontOutlineColor"));

		sdfFnt_inScreenSize_ID = static_cast<GLuint>(glGetUniformLocation (gl_getShaderID(whichShader), "inScreenSize"));
		sdfFnt_inTextureUnit_ID = static_cast<GLuint>(glGetUniformLocation (gl_getShaderID(whichShader), "inTexture0"));

		initDone = true;
	}

	//
	// Populate the vectors with the vertex and texture information for this line of text
	for ( i = 0; i != strlen (textLine); i++ )
	{
		texture_glyph_t *glyph = nullptr;

		float offset_y = 0.0f;

		for ( j = 0; j < sdfFontData.glyphs_count; ++j )
		{
			if ( sdfFontData.glyphs[j].codepoint == ftgl::utf8_to_utf32 (textLine + i))
			{
				glyph = &sdfFontData.glyphs[j];
				offset_y = float(glyph->height - glyph->offset_y);
				break;
			}
		}

		if ( !glyph )
			continue;

		// Triangle One - First point
		sdfFont_addVertexInfo (glm::vec2{currentX + glyph->offset_x, currentY - offset_y}, glm::vec2{glyph->s0, glyph->t1}, lineColor);

		sdfFont_addVertexInfo (glm::vec2{currentX + glyph->offset_x, (currentY + glyph->height) - offset_y}, glm::vec2{glyph->s0, glyph->t0}, lineColor);

		sdfFont_addVertexInfo (glm::vec2{currentX + glyph->width + glyph->offset_x, (currentY + glyph->height) - offset_y}, glm::vec2{glyph->s1, glyph->t0}, lineColor);
		// Triangle Two - First point
		sdfFont_addVertexInfo (glm::vec2{currentX + glyph->offset_x, currentY - offset_y}, glm::vec2{glyph->s0, glyph->t1}, lineColor);

		sdfFont_addVertexInfo (glm::vec2{currentX + glyph->width + glyph->offset_x, (currentY + glyph->height) - offset_y}, glm::vec2{glyph->s1, glyph->t0}, lineColor);

		sdfFont_addVertexInfo (glm::vec2{currentX + glyph->width + glyph->offset_x, currentY - offset_y}, glm::vec2{glyph->s1, glyph->t1}, lineColor);

		currentX += glyph->advance_x;
		currentY += glyph->advance_y;
	}

	if (sdfFont_vertex.empty())
		return; // No character to display

	//
	// Now upload and draw the line of text
	//

	//
	// Start using TTF font shader program
	GL_ASSERT (glUseProgram (gl_getShaderID(whichShader)));
	//
	// Bind the generated VAO
	GL_ASSERT (glBindVertexArray (sdfFnt_g_glyphVAO_ID));

	wrapglEnable (GL_BLEND);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	wrapglDisable (GL_DEPTH_TEST);
	//
	// Bind texture if it's not already bound as current texture
	GL_CHECK (glActiveTexture (GL_TEXTURE0));
	GL_CHECK (glBindTexture (GL_TEXTURE_2D, sdfFontTexID));
	GL_CHECK (glUniform1i (sdfFnt_inTextureUnit_ID, 0));

	GL_ASSERT (glUniform2f (sdfFnt_inScreenSize_ID, (float) winWidth / 2, (float) winHeight / 2));    // Pass screen size for
	//
	// Bind the vertex info
	GL_ASSERT (glBindBuffer (GL_ARRAY_BUFFER, sdfFnt_g_vertVBO_ID));

	GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, sizeof(_fnt_vertex) * sdfFont_vertex.size(), &sdfFont_vertex[0].position, GL_DYNAMIC_DRAW));

	GL_ASSERT (glVertexAttribPointer (sdfFnt_inPosition_ID, 2, GL_FLOAT, GL_FALSE, sizeof (_fnt_vertex), (GLvoid *) offsetof (_fnt_vertex, position)));
	GL_ASSERT (glEnableVertexAttribArray (sdfFnt_inPosition_ID));
	//
	// Bind the texture coordinates
	GL_ASSERT (glVertexAttribPointer (sdfFnt_inTextureCoords_ID, 2, GL_FLOAT, GL_FALSE, sizeof (_fnt_vertex), (GLvoid *) offsetof (_fnt_vertex, textureCoord)));
	GL_ASSERT (glEnableVertexAttribArray (sdfFnt_inTextureCoords_ID));
	//
	// Bind color array
	GL_ASSERT (glVertexAttribPointer (sdfFnt_inColor_ID, 4, GL_FLOAT, GL_FALSE, sizeof (_fnt_vertex), (GLvoid *) offsetof (_fnt_vertex, color)));
	GL_ASSERT (glEnableVertexAttribArray (sdfFnt_inColor_ID));
	//
	// No matrices used - transform is done in the shader
	GL_ASSERT (glDrawArrays (GL_TRIANGLES, 0, sdfFont_vertex.size ()));
}
