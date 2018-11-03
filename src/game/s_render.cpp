#include <hdr/opengl/gl_openGLWrap.h>
#include <hdr/opengl/gl_shaders.h>
#include <hdr/opengl/gl_fbo.h>
#include <hdr/game/s_renderDebug.h>
#include <hdr/io/io_mouse.h>
#include <hdr/game/s_lightCaster.h>
#include <hdr/opengl/gl_renderSprite.h>
#include <hdr/game/s_hud.h>
#include <hdr/game/s_player.h>
#include "hdr/game/s_render.h"

#define USE_TILE_LOOKUP 1

//#define USE_BLIT 1

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

vector<_tileCoords> tileCoords;
vector<unsigned int> tileCoordsIndex;

vector<float> singleTileTexCoords;

int numTileAcrossInTexture, numTilesDownInTexture;
float tileTextureWidth;
int indexCounter = 0;

//float worldLocationX = 0, worldLocationY = 0;
//float viewWorldLocationX = 0, viewWorldLocationY = 0;
float aspectRatioX, aspectRatioY;;
float g_scaleViewBy = 1.4f;
int g_playFieldSize = 256;

//-----------------------------------------------------------------------------
//
// Set the X Position for each tile in the master texture
void gam_setSingleTileCoords ( int posX, int totalWidth )
//-----------------------------------------------------------------------------
{
	float glPosX = 0.0f;

	glPosX = (float) posX / (float) totalWidth;

	singleTileTexCoords.push_back (glPosX);
}

//-----------------------------------------------------------------------------
//
// Return texture coords for passed in tile
vec2 gam_getTileTexCoords ( int whichTile )
//-----------------------------------------------------------------------------
{
	vec2 returnValue;

	returnValue.x = singleTileTexCoords[whichTile];
	returnValue.y = 0.0f;

	return returnValue;
}

//-----------------------------------------------------------------------------
//
// Blit a screen sized quad from the backing full level texture to display on the screen
void gam_blitFrameBufferToScreen ( string whichShader, const string levelName, GLuint whichTexture, glm::vec2 viewSize )
//-----------------------------------------------------------------------------
{
	_tileCoords tempCoord;
	float startTexX, startTexY, widthTex, heightTex;

	tileCoords.clear ();
	tileCoordsIndex.clear ();
	indexCounter = 0;

	glm::vec2 backingTextureSize;
	backingTextureSize = io_getTextureSize (levelName);

	widthTex = viewSize.x / backingTextureSize.x;
	heightTex = viewSize.y / backingTextureSize.y;

//	startTexX = (viewWorldLocationX / backingTextureSize.x) - (widthTex / 2);
//	startTexY = (viewWorldLocationY / backingTextureSize.y) - (heightTex / 2);

	startTexX = (playerDroid.worldPos.x / backingTextureSize.x) - (widthTex / 2);
	startTexY = (playerDroid.worldPos.y / backingTextureSize.y) - (heightTex / 2);

	//
	// Corner 0
	//
	tempCoord.position.x = 0.0f;
	tempCoord.position.y = viewSize.y;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = startTexX;
	tempCoord.textureCoords.y = startTexY;

	tileCoords.push_back (tempCoord);
	//
	// Corner 1
	//
	tempCoord.position.x = 0.0f;
	tempCoord.position.y = 0.0f;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = startTexX;
	tempCoord.textureCoords.y = startTexY + heightTex;

	tileCoords.push_back (tempCoord);
	//
	// Corner 2
	//
	tempCoord.position.x = viewSize.x;
	tempCoord.position.y = 0.0f;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = startTexX + widthTex;
	tempCoord.textureCoords.y = startTexY + heightTex;

	tileCoords.push_back (tempCoord);
	//
	// Corner 3
	//
	tempCoord.position.x = viewSize.x;
	tempCoord.position.y = viewSize.y;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = startTexX + widthTex;
	tempCoord.textureCoords.y = startTexY;

	tileCoords.push_back (tempCoord);

	//
	// Indexes into the vertex array
	tileCoordsIndex.push_back (0 + indexCounter);
	tileCoordsIndex.push_back (1 + indexCounter);
	tileCoordsIndex.push_back (2 + indexCounter);
	tileCoordsIndex.push_back (2 + indexCounter);
	tileCoordsIndex.push_back (3 + indexCounter);
	tileCoordsIndex.push_back (0 + indexCounter);

	indexCounter += 4;


	static GLuint vao = 0;
	static GLuint vbo = 0;
	static GLuint elementbuffer = 0;

	// create the VAO
	GL_ASSERT (glGenVertexArrays (1, &vao));
	GL_CHECK (glBindVertexArray (vao));


	glGenBuffers (1, &elementbuffer);
	GL_CHECK (glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, elementbuffer));
	GL_CHECK (glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (unsigned int) * tileCoordsIndex.size (), &tileCoordsIndex[0], GL_DYNAMIC_DRAW));

	// Create buffers for the vertex data
	vbo = wrapglGenBuffers (1, __func__);

	GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));

	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBufferData (GL_ARRAY_BUFFER, sizeof (_tileCoords) * tileCoords.size (), &tileCoords[0].position, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray (gl_getAttrib (whichShader, "inPosition"));
	glVertexAttribPointer (gl_getAttrib (whichShader, "inPosition"), 3, GL_FLOAT, GL_FALSE, sizeof (_tileCoords), (GLvoid *) offsetof(_tileCoords, position));

	glEnableVertexAttribArray (gl_getAttrib (whichShader, "inTextureCoords"));
	glVertexAttribPointer (gl_getAttrib (whichShader, "inTextureCoords"), 2, GL_FLOAT, GL_FALSE, sizeof (_tileCoords), (GLvoid *) offsetof(_tileCoords, textureCoords));

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
	GL_CHECK (glUniformMatrix4fv (gl_getUniform (whichShader, "MVP_Matrix"), 1, false, glm::value_ptr (MVP)));
	GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inPosition")));
	GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inTextureCoords")));

	// Index buffer
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

	GL_CHECK (glDrawElements (GL_TRIANGLES, (GLsizei) tileCoordsIndex.size (), GL_UNSIGNED_INT, (void *) 0));

	glUseProgram (0);
	glBindVertexArray (0);

	glDeleteBuffers (1, &elementbuffer);
	glDeleteBuffers (1, &vbo);
	glDeleteVertexArrays (1, &vao);

}

//-----------------------------------------------------------------------------
//
// Draw a single tile from the tile sheet
void inline gam_drawSingleTile ( float destX, float destY, int whichTile )
//-----------------------------------------------------------------------------
{
	static float previousTile = -1;
	static vec2 textureCoords;

	_tileCoords tempCoord;

	if ( previousTile != whichTile )
	{
#ifdef USE_TILE_LOOKUP
		textureCoords = gam_getTileTexCoords (whichTile);
#else
		//		textureCoords.x = (float) (whichTile % numTileAcrossInTexture) * (1.0f / numTileAcrossInTexture);
//		textureCoords.y = (float) (whichTile / numTilesDownInTexture) * (1.0f / numTilesDownInTexture);

		textureCoords.x = (float) (whichTile % 64) * (1.0f / 64);
		textureCoords.y = (float) (whichTile / 1) * (1.0f / 1);
#endif
		previousTile = whichTile;
	}

	tileTextureWidth = 1.0f / 70.0f;    // TODO: Why 70 - double 35??

	//
	// Corner 0
	//
	tempCoord.position.x = destX;
	tempCoord.position.y = destY;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = textureCoords.x;
	tempCoord.textureCoords.y = textureCoords.y;

	tileCoords.push_back (tempCoord);
	//
	// Corner 1
	//
	tempCoord.position.x = destX;
	tempCoord.position.y = destY + TILE_SIZE;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = textureCoords.x;
	tempCoord.textureCoords.y = (textureCoords.y + 1.0f);

	tileCoords.push_back (tempCoord);
	//
	// Corner 2
	//
	tempCoord.position.x = destX + TILE_SIZE;
	tempCoord.position.y = destY + TILE_SIZE;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = (textureCoords.x + tileTextureWidth);
	tempCoord.textureCoords.y = (textureCoords.y + 1.0f);

	tileCoords.push_back (tempCoord);

	//
	// Corner 3
	//
	tempCoord.position.x = destX + TILE_SIZE;
	tempCoord.position.y = destY;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = (textureCoords.x + tileTextureWidth);
	tempCoord.textureCoords.y = textureCoords.y;

	tileCoords.push_back (tempCoord);
	//
	// Indexes into the vertex array
	tileCoordsIndex.push_back (0 + indexCounter);
	tileCoordsIndex.push_back (1 + indexCounter);
	tileCoordsIndex.push_back (2 + indexCounter);
	tileCoordsIndex.push_back (2 + indexCounter);
	tileCoordsIndex.push_back (3 + indexCounter);
	tileCoordsIndex.push_back (0 + indexCounter);

	indexCounter += 4;
}

//-----------------------------------------------------------------------------
//
// Draw the entire level to the texture currently bound to the FBO
void gam_drawAllTiles ( const string whichShader, const string levelName, GLuint whichTexture )
//-----------------------------------------------------------------------------
{
	int countX, countY, index;
	int whichTile = 0;
	cpFloat tilePtr = 0;
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint elementbuffer = 0;
	static bool initDone = false;

	countY = 0;
	countX = 0;

	tileCoords.clear ();
	tileCoordsIndex.clear ();
	indexCounter = 0;

	for ( index = 0; index < levelInfo.at (levelName).levelDimensions.x * levelInfo.at (levelName).levelDimensions.y; index++ )
	{
		tilePtr = static_cast<int>((countY * levelInfo.at (levelName).levelDimensions.x) + countX);

		whichTile = levelInfo.at (levelName).tiles[static_cast<int>(tilePtr)];

		if ((whichTile > 0) && (whichTile < 64))
		{
			switch ( whichTile )
			{
				case ALERT_GREEN_TILE:
				case ALERT_YELLOW_TILE:
				case ALERT_RED_TILE:
					switch ( currentAlertLevel )
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
			gam_drawSingleTile ((countX * TILE_SIZE), (countY * TILE_SIZE), whichTile);
		}
		else if ( whichTile != 0 )
			con_print (CON_ERROR, true, "Invalid tile index [ %i ].", whichTile);

		countX++;
		if ( countX == (int) levelInfo.at (levelName).levelDimensions.x )
		{
			countX = 0;
			countY++;
		}
	}
	//
	// Setup shader and buffer data parameters
	// TODO: will need to reset on level change
	//
	if ( !initDone )
	{
		// create the VAO
		GL_ASSERT (glGenVertexArrays (1, &vao));
		elementbuffer = wrapglGenBuffers (1, __func__);
		// Create buffers for the vertex data
		vbo = wrapglGenBuffers (1, __func__);

		GL_CHECK (glBindVertexArray (vao));

		GL_CHECK (glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, elementbuffer));
		GL_CHECK (glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (unsigned int) * tileCoordsIndex.size (), &tileCoordsIndex[0], GL_DYNAMIC_DRAW));

		GL_CHECK (glBindBuffer (GL_ARRAY_BUFFER, vbo));
		GL_CHECK (glBufferData (GL_ARRAY_BUFFER, sizeof (_tileCoords) * tileCoords.size (), &tileCoords[0].position, GL_DYNAMIC_DRAW));

		GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));

		GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inPosition")));
		GL_CHECK (glVertexAttribPointer (gl_getAttrib (whichShader, "inPosition"), 3, GL_FLOAT, GL_FALSE, sizeof (_tileCoords), (GLvoid *) offsetof (_tileCoords, position)));

		GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inTextureCoords")));
		GL_CHECK (glVertexAttribPointer (gl_getAttrib (whichShader, "inTextureCoords"), 2, GL_FLOAT, GL_FALSE, sizeof (_tileCoords), (GLvoid *) offsetof (_tileCoords, textureCoords)));

		GL_CHECK (glUniform1i (gl_getUniform (whichShader, "inTexture0"), 0));
		GL_CHECK (glUniform1f (gl_getUniform (whichShader, "gamma"), g_gamma));

		glUseProgram (0);
		glBindVertexArray (0);

		initDone = false;
	}

	GL_CHECK (glBindVertexArray (vao));

	GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));
	//
	// Bind texture if it's not already bound as current texture
	GL_CHECK (glActiveTexture (GL_TEXTURE0));
	GL_CHECK (glBindTexture (GL_TEXTURE_2D, whichTexture));

	GL_CHECK (glUniform1i (gl_getUniform (whichShader, "inTexture0"), 0));
	GL_CHECK (glUniform1f (gl_getUniform (whichShader, "gamma"), g_gamma));
	//
	// Enable attribute to hold vertex information
	GL_CHECK (glUniformMatrix4fv (gl_getUniform (whichShader, "MVP_Matrix"), 1, false, glm::value_ptr (MVP)));

	GL_CHECK (glDrawElements (GL_TRIANGLES, (GLsizei) tileCoordsIndex.size (), GL_UNSIGNED_INT, (void *) 0));

	glUseProgram (0);
	glBindVertexArray (0);
}

// TODO: Need a destroy texture - remove from MAP and free ID

//----------------------------------------------------------------------------------------
//
// Create a texture of passed in size and name.
// Store it in the texture Map array
void gam_createBackingTexture ( string textureName, glm::vec2 textureSize )
//----------------------------------------------------------------------------------------
{
	GLuint fullLevelTexture = 0;

	glGenTextures (1, &fullLevelTexture);
	glBindTexture (GL_TEXTURE_2D, fullLevelTexture);
	//
	// Texture is empty
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(textureSize.x), static_cast <GLsizei>(textureSize.y), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);     // TODO: or GL_LINEAR - makes it blurry
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

	io_storeTextureInfoIntoMap (fullLevelTexture, glm::vec2{textureSize.x, textureSize.y}, textureName, false);
}

//----------------------------------------------------------------------------------------
//
// Draw the current level
// 1. Create the texture to hold the entire level
// 2. Create a FBO and bind depth and texture to it
// 3. Render the entire level to the backing texture
// 4. Render viewable playfield texture to another texture
// 5.
void gam_drawFullLevel ( string levelName, string whichShader, GLuint sourceTexture )
//----------------------------------------------------------------------------------------
{
	static bool backingLevel = false;

	static GLuint fullLevel_FBO = 0;

	glm::vec2 backingSize;

//    viewWorldLocationX = playerDroid.worldPos.x; // + (currentVelocity.x * interpolate);
//    viewWorldLocationY = playerDroid.worldPos.y; // + (currentVelocity.y * interpolate);

    backingSize.x = static_cast<float>(levelInfo.at (levelName).levelDimensions.x * TILE_SIZE);
	backingSize.y = static_cast<float>(levelInfo.at (levelName).levelDimensions.y * TILE_SIZE);

	if ( !backingLevel )
	{
		fullLevel_FBO = gl_createFBO (backingSize);
		gam_createBackingTexture (levelName, backingSize);

		// TODO: Destroy texture on level change and relink it
		gl_linkTextureToFBO (io_getTextureID (levelName), fullLevel_FBO);
		backingLevel = true;  // Reset on level change
	}
	//
	// Check it all worked ok
	if ( 0 == fullLevel_FBO )
	{
		con_print (CON_ERROR, true, "Unable to create backing FBO. Critical error.");
		return;
	}
	//
	// Start drawing to backing texture
	glBindFramebuffer (GL_FRAMEBUFFER, fullLevel_FBO);
	gl_linkTextureToFBO (io_getTextureID (levelName), fullLevel_FBO);

	glm::vec2 backingViewPosition;

	backingViewPosition = glm::vec2 ();

	glViewport (0, 0, static_cast<GLsizei>(backingSize.x), static_cast<GLsizei>(backingSize.y)); // Render on the whole backing FBO

	gl_set2DMode (backingViewPosition, backingSize, glm::vec3 (1, 1, 1));
	//
	// Draw tiles to bound texture from 'sourceTexture'
	gam_drawAllTiles (whichShader, levelName, sourceTexture);

	gam_showLineSegments (levelName);
	gam_showWayPoints (levelName);

#ifdef USE_BLIT
	viewPortX = static_cast<GLsizei>(winWidth * aspectRatioX);
	viewPortY = static_cast<GLsizei>(winHeight * aspectRatioY);

	glViewport(0, 0, viewPortX, viewPortY);

	int startPosX, startPosY;

	startPosX = static_cast<int>(viewWorldLocationX);
	startPosY = static_cast<int>(viewWorldLocationY);
	//
	// Copy screen sized quad from backing texture to visible screen
	glBindFramebuffer (GL_READ_FRAMEBUFFER, fullLevel_FBO);   // Read from large full level texture
	glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);                 // Write to default buffer ( screen )

	startPosX = startPosX - (TILE_SIZE * ((winWidth / TILE_SIZE) / 2));                   // Center viewable screen
	startPosY = startPosY - (TILE_SIZE * ((winHeight / TILE_SIZE) / 2));

	GL_CHECK ( glBlitFramebuffer (startPosX, startPosY, startPosX + winWidth, startPosY + winHeight, 0, 0, winWidth, winHeight, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST ) );

#else
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

//	glViewport(0,0,256, 256); // Render on the whole framebuffer

//	light_createLightCaster (levelName, testLightPosition);

//	glm::vec2       lightSize;
//	lightSize = io_getTextureSize ("lightcaster");

//	gl_draw2DQuad (glm::vec2{testLightPosition.x, testLightPosition.y}, glm::vec2{256,256}, "lightmapRender", io_getTextureID ("lightmap"), glm::vec3{0,0,0});

//	light_createLightCaster (vec3(750.0, 400.0, 0.0));

//----------------------------------------------------------------------------
//
// Now copy the view size texture to another texture of the same size
//
//----------------------------------------------------------------------------
	static bool viewTextureCreated = false;
	glm::vec2 viewTextureSize;

	viewTextureSize = glm::vec2{g_playFieldSize, g_playFieldSize};

	if ( !viewTextureCreated )
	{
		gam_createBackingTexture ("viewTexture", viewTextureSize);

		// TODO: Destroy texture on level change and relink it
		gl_linkTextureToFBO (io_getTextureID ("viewTexture"), fullLevel_FBO);

		viewTextureCreated = true;
	}
	//
	// Start drawing to backing texture
	glBindFramebuffer (GL_FRAMEBUFFER, fullLevel_FBO);
	gl_linkTextureToFBO (io_getTextureID ("viewTexture"), fullLevel_FBO);

	glm::vec2 viewTexturePosition;

	viewTexturePosition.x = (winWidth - viewTextureSize.x) / 2;
	viewTexturePosition.y = (winHeight - viewTextureSize.y) / 2;

	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
	glViewport (0, 0, viewTextureSize.x, viewTextureSize.y);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//
	// TODO: Make scale values variable
	gl_set2DMode (viewTexturePosition, viewTextureSize, glm::vec3 (1, 1, 1));
	//
	// Copy screen sized quad from backing texture to visible screen
	gam_blitFrameBufferToScreen ("quad3d", levelName, io_getTextureID (levelName), glm::vec2{winWidth, winHeight});

	drd_renderThisLevel ( levelName );

    s_renderPlayerSprite ();
//----------------------------------------------------------------------------
//
// Take that texture and draw a quad to the default frame buffer ( screen )
//
//----------------------------------------------------------------------------

	//
	// Switch back to rendering to default frame buffer
	gl_renderToScreen ();
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec2 viewSize;
	glm::vec2 viewPortPosition;
	float texCoords[] = {0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0};

	float scaleView = g_scaleViewBy;
//	float screenRatio = static_cast<float>(winWidth) / static_cast<float>(winHeight);

	viewSize = glm::vec2{g_playFieldSize, g_playFieldSize};

	viewSize.x *= scaleView;
	viewSize.y *= scaleView;

	viewPortPosition.x = (winWidth - viewSize.x) / 2;
	viewPortPosition.y = (winHeight - viewSize.y) / 2;

	glViewport (0, 0, winWidth, winHeight);
	gl_set2DMode(glm::vec2{0,0}, glm::vec2{winWidth,winHeight}, glm::vec3(1, 1, 1));

	gl_draw2DQuad (viewPortPosition, viewSize, "quad3d", io_getTextureID ("viewTexture"), glm::vec3{0, 0, 0}, glm::vec3{-1.0, 0.0, 0.0}, texCoords);

#endif
	//
	// Render HUD on top of everything
	s_renderHUD ();

//    s_renderPlayerSprite ();


	io_renderMouseCursor ();
}