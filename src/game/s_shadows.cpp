#include <hdr/io/io_textures.h>
#include <hdr/opengl/gl_openGLWrap.h>
#include "hdr/game/s_shadows.h"
#include "hdr/opengl/gl_shaders.h"

static bool objectsAdded = false;

typedef struct {
	glm::vec3 position;
	glm::vec2 textureCoords;
} _tile;

vector<_tile>       tiles;

//-----------------------------------------------------------------------------
//
// Draw a 2D quad
void gl_draw2DQuad ( string whichShader, GLuint whichTexture, float interpolation )
//-----------------------------------------------------------------------------
{
	static GLuint vao = 0;
	static GLuint buffers[2];
	static bool initDone = false;

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

		GL_CHECK (glBufferData (GL_ARRAY_BUFFER, sizeof(_tile) * tiles.size(), &tiles[0], GL_DYNAMIC_DRAW));

		GL_CHECK (glVertexAttribPointer (gl_getAttrib (whichShader, "inPosition"), 3, GL_FLOAT, GL_FALSE, sizeof (_tile), (const GLvoid *) (offsetof (_tile, position))));
		GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inPosition")));

		// Texture coordinates buffer
		GL_CHECK (glVertexAttribPointer (gl_getAttrib (whichShader, "inTextureCoords"), 2, GL_FLOAT, GL_FALSE, sizeof (_tile), (const GLvoid *) (offsetof(_tile, textureCoords))));
		GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inTextureCoords")));

		glBindVertexArray(0);

		initDone = true;
	}

	GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));

	glUniformMatrix4fv (glGetUniformLocation (gl_getShaderID (whichShader), "MVP_Matrix"), 1, GL_FALSE, glm::value_ptr (MVP));
	//
	// Bind texture if it's not already bound as current texture
	GL_CHECK (glActiveTexture (GL_TEXTURE0));

	GL_CHECK (glBindTexture (GL_TEXTURE_2D, whichTexture));

	GL_CHECK (glUniform1i (gl_getUniform (whichShader, "inTexture0"), 0));
	GL_CHECK (glUniform1f (gl_getUniform (whichShader, "gamma"), g_gamma));

	GL_CHECK (glBindVertexArray (vao));
	//
	// Enable attribute to hold vertex information
	GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inPosition")));
	GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inTextureCoords")));

	GL_CHECK (glDrawArrays (GL_TRIANGLES, 0, 6 * tiles.size()));

	glBindVertexArray(0);

//	glDeleteBuffers (2, buffers);
//	glDeleteVertexArrays (1, &vao);
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void gam_createTileGeometry(glm::vec2 position, int tileID, int textureID)
//-----------------------------------------------------------------------------------------------------
{
	float tileSize = 32.0f;

	_tile tempTile;

	// Point zero - Triangle one
	tempTile.position.x = position.x - (tileSize * 0.5f);
	tempTile.position.y = position.y - (tileSize * 0.5f);
	tempTile.position.z = 0.0f;
	// Point zero - Triangle one
	tempTile.textureCoords.x = 0.0f;
	tempTile.textureCoords.y = 0.0f;
	tiles.push_back(tempTile);

	// Point one - Triangle one
	tempTile.position.x = position.x + (tileSize * 0.5f);
	tempTile.position.y = position.y - (tileSize * 0.5f);
	tempTile.position.z = 0.0f;
	// Point one - Triangle one
	tempTile.textureCoords.x = 1.0f;
	tempTile.textureCoords.y = 0.0f;
	tiles.push_back (tempTile);

	// Point two - Triangle one
	tempTile.position.x = position.x - (tileSize * 0.5f);
	tempTile.position.y = position.y + (tileSize * 0.5f);
	tempTile.position.z = 0.0f;
	// Point two - Triangle two
	tempTile.textureCoords.x = 0.0f;
	tempTile.textureCoords.y = 1.0f;
	tiles.push_back (tempTile);

	// Point zero - Triangle two
	tempTile.position.x = position.x - (tileSize * 0.5f);
	tempTile.position.y = position.y + (tileSize * 0.5f);
	tempTile.position.z = 0.0f;
	// Point zero - Triangle two
	tempTile.textureCoords.x = 0.0f;
	tempTile.textureCoords.y = 1.0f;
	tiles.push_back (tempTile);

	// Point one - Triangle two
	tempTile.position.x = position.x + (tileSize * 0.5f);
	tempTile.position.y = position.y - (tileSize * 0.5f);
	tempTile.position.z = 0.0f;
	// Point one - Triangle two
	tempTile.textureCoords.x = 1.0f;
	tempTile.textureCoords.y = 0.0f;
	tiles.push_back (tempTile);

	// Point two - Triangle two
	tempTile.position.x = position.x + (tileSize * 0.5f);
	tempTile.position.y = position.y + (tileSize * 0.5f);
	tempTile.position.z = 0.0f;
	// Point two - Triangle two
	tempTile.textureCoords.x = 1.0f;
	tempTile.textureCoords.y = 1.0f;
	tiles.push_back(tempTile);
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void gam_addObjects()
//-----------------------------------------------------------------------------------------------------
{
	gam_createTileGeometry (vec2 (0,  0),  0, 1);
	gam_createTileGeometry (vec2 (40, 0),  0, 1);
	gam_createTileGeometry (vec2 (0,  40), 0, 1);
	gam_createTileGeometry (vec2 (40, 40), 0, 1);

	objectsAdded = true;
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void gam_drawAllObjects(float interpolate)
//-----------------------------------------------------------------------------------------------------
{
	if (!objectsAdded)
		gam_addObjects ();

	gl_set2DMode (interpolate);

	gl_draw2DQuad ("quad2d", io_getTextureID ("splash.png"), interpolate );
}