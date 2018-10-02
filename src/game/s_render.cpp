#include <hdr/opengl/gl_openGLWrap.h>
#include <hdr/opengl/gl_shaders.h>
#include "hdr/game/s_render.h"

//#define USE_TILE_LOOKUP 1

//	1--------2
//	|        |
//  |        |
//  |        |
//  0--------3

typedef struct
{
	glm::vec3 position;
	glm::vec2 textureCoords;
} _tileCoords;

vector<_tileCoords>     tileCoords;

vector<_tileTexCoords>  tileTexCoords;

int                     numTileAcrossInTexture, numTilesDownInTexture;
float                   tileTextureWidth;

//-----------------------------------------------------------------------------
//
// Return texture coords for passed in tile
vec2 gam_getTileTexCoords(int whichTile)
//-----------------------------------------------------------------------------
{
	return tileTexCoords[whichTile].texCoord;
}

//-----------------------------------------------------------------------------
//
// Setup up precalculated coords for the tiles
void gam_calcTileTexCoords(const string textureName)
//-----------------------------------------------------------------------------
{
	int             totalNumTiles;
	vec2            imageSize;
	_tileTexCoords  tempCoords{};   // TODO: Check the affect of this {}

	imageSize = io_getTextureSize(textureName);
	if (imageSize.x < 0)
	{
		con_print(CON_ERROR, true, "Unable to get image size to calculate tile coordinates [ %s ].", textureName.c_str());
		return;
	}

	//
	// How many tiles fit into the texture
	numTileAcrossInTexture = static_cast<int>(imageSize.x / TILE_SIZE);
	numTilesDownInTexture = static_cast<int>(imageSize.y / TILE_SIZE);

	totalNumTiles = numTileAcrossInTexture * numTilesDownInTexture;

	//
	// How wide is a single tile in the texture
	tileTextureWidth = 1.0f /  numTileAcrossInTexture;

	for (int i = 0; i != totalNumTiles; i++)
	{
		tempCoords.texCoord.x = (float) (i % numTileAcrossInTexture) * tileTextureWidth;
		tempCoords.texCoord.y = (float) (i / numTilesDownInTexture) * tileTextureWidth;

		tileTexCoords.push_back (tempCoords);
	}
}

//-----------------------------------------------------------------------------
//
// Draw a single tile from the tile sheet
void inline gam_drawSingleTile(float destX, float destY, int whichTile)
//-----------------------------------------------------------------------------
{
	static float    previousTile = -1;
	static vec2     textureCoords;

	_tileCoords     tempCoord;

	if (previousTile != whichTile)
	{
#ifdef USE_TILE_LOOKUP
		textureCoords = gam_getTileTexCoords(whichTile);
#else
		textureCoords.x = (float) (whichTile % numTileAcrossInTexture) * (1.0f / numTileAcrossInTexture);
		textureCoords.y = (float) (whichTile / numTilesDownInTexture) * (1.0f / numTilesDownInTexture);
#endif
		previousTile = whichTile;
	}

	//
	// Corner 0 - Triangle 1
	//
	tempCoord.position.x = destX;
	tempCoord.position.y = destY;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = textureCoords.x;
	tempCoord.textureCoords.y = textureCoords.y;

	tileCoords.push_back(tempCoord);
	//
	// Corner 1 - Triangle 1
	//
	tempCoord.position.x = destX;
	tempCoord.position.y = destY + TILE_SIZE;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = textureCoords.x;
	tempCoord.textureCoords.y = textureCoords.y + tileTextureWidth;

	tileCoords.push_back(tempCoord);
	//
	// Corner 2 - Triangle 1
	//
	tempCoord.position.x = destX + TILE_SIZE;
	tempCoord.position.y = destY + TILE_SIZE;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = textureCoords.x + tileTextureWidth;
	tempCoord.textureCoords.y = textureCoords.y + tileTextureWidth;

	tileCoords.push_back(tempCoord);

	//
	// Corner 0 - Triangle 2
	//
	tempCoord.position.x = destX;
	tempCoord.position.y = destY;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = textureCoords.x;
	tempCoord.textureCoords.y = textureCoords.y;

	tileCoords.push_back(tempCoord);

	//
	// Corner 2 - Triangle 2
	//
	tempCoord.position.x = destX + TILE_SIZE;
	tempCoord.position.y = destY + TILE_SIZE;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = textureCoords.x + tileTextureWidth;
	tempCoord.textureCoords.y = textureCoords.y + tileTextureWidth;

	tileCoords.push_back(tempCoord);

	//
	// Corner 3 - Triangle 1
	//
	tempCoord.position.x = destX + TILE_SIZE;
	tempCoord.position.y = destY;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = textureCoords.x + tileTextureWidth;
	tempCoord.textureCoords.y = textureCoords.y;

	tileCoords.push_back(tempCoord);
}

//-----------------------------------------------------------------------------
//
// Copy all the tiles that are visible this screen to the array
void gam_drawAllTiles(string whichShader, GLuint whichTexture)
//-----------------------------------------------------------------------------
{
	int across, down;
	int countX, countY;
	int whichTilePtr, whichTile;

	float tilePosY, tilePosX;

	tilePosY = 0.0f;
	tilePosX = 0.0f;

	across = winWidth / TILE_SIZE;
	down = winHeight / TILE_SIZE;

	tileCoords.clear();

	for (countY = 0; countY < down + 1; countY++) // +1 to allow overdraw for fine scroll
	{
		for (countX = 0; countX < across + 1; countX++)
		{
			whichTilePtr = static_cast<int>(((countY + tilePosY) * levelInfo.at(currentLevelName).levelDimensions.x) + (countX + tilePosX));
			//
			// TODO: Going past the edge of the tile array - adding tilePosX and tilePosY is making whichTilePtr too large
			//
			if (whichTilePtr >= levelInfo.at(currentLevelName).levelDimensions.x * levelInfo.at(currentLevelName).levelDimensions.y)
				whichTilePtr = 0;

			if ( levelInfo.at(currentLevelName).tiles.empty() )
			{
				printf ("ERROR: Tile vector array is currently 0.\n");
				return;
			}

			whichTile = levelInfo.at(currentLevelName).tiles[whichTilePtr];


			if ((whichTile < 0) || (whichTile > 64))
				whichTile = 10;

			if (whichTile != 0) // Don't draw the empty tiles
			{
				switch (whichTile)
				{
					case ALERT_GREEN_TILE:
					case ALERT_YELLOW_TILE:
					case ALERT_RED_TILE:
						switch (currentAlertLevel)
						{
							case ALERT_GREEN_TILE:
								whichTile = ALERT_GREEN_TILE;
								break;
							case ALERT_YELLOW_TILE:
								whichTile = ALERT_YELLOW_TILE;
								break;
							case ALERT_RED_TILE:
								whichTile = ALERT_RED_TILE;
								break;
							default:
								break;
						}
						break;
					default:
						break;
				}
//				gam_drawSingleTile((countX * TILE_SIZE) - pixelX, (countY * TILE_SIZE) - pixelY, whichTile);
				gam_drawSingleTile((countX * TILE_SIZE), (countY * TILE_SIZE), whichTile);
			}
		}
	}

	static GLuint       vao = 0;
	static GLuint       vbo = 0;

	// create the VAO
	GL_ASSERT (glGenVertexArrays (1, &vao));
	GL_CHECK (glBindVertexArray (vao));

	// Create buffers for the vertex data
	vbo = wrapglGenBuffers (1, __func__);

	GL_CHECK (glUseProgram (gl_getShaderID(whichShader)));

	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBufferData (GL_ARRAY_BUFFER, sizeof(_tileCoords) * tileCoords.size(), &tileCoords[0].position, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray (gl_getAttrib (whichShader, "inPosition"));
	glVertexAttribPointer     (gl_getAttrib (whichShader, "inPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(_tileCoords), (GLvoid *)offsetof(_tileCoords, position) );

	glEnableVertexAttribArray (gl_getAttrib (whichShader, "inTextureCoords"));
	glVertexAttribPointer     (gl_getAttrib (whichShader, "inTextureCoords"), 2, GL_FLOAT, GL_FALSE, sizeof(_tileCoords), (GLvoid *)offsetof(_tileCoords, textureCoords));

	glUseProgram (0);
	glBindVertexArray (0);



	GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));

	GL_CHECK (glBindVertexArray (vao));

	//
	// Bind texture if it's not already bound as current texture
	GL_CHECK (glActiveTexture (GL_TEXTURE0));

	GL_CHECK (glBindTexture (GL_TEXTURE_2D, whichTexture));

	GL_CHECK (glUniform1i (gl_getUniform (whichShader, "inTexture0"), 0));
	GL_CHECK (glUniform1f (gl_getUniform (whichShader, "gamma"), g_gamma));
	//
	// Enable attribute to hold vertex information
	GL_CHECK (glUniformMatrix4fv           (gl_getUniform (whichShader, "MVP_Matrix"), 1, false, glm::value_ptr (MVP)));
	GL_CHECK (glEnableVertexAttribArray    (gl_getAttrib  (whichShader, "inPosition")));
	GL_CHECK (glEnableVertexAttribArray    (gl_getAttrib  (whichShader, "inTextureCoords")));

	GL_CHECK (glDrawArrays (GL_TRIANGLES, 0, tileCoords.size()));

	glUseProgram (0);
	glBindVertexArray (0);

	glDeleteBuffers (1, &vbo);
	glDeleteVertexArrays (1, &vao);
}
