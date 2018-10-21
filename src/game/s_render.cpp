#include <hdr/opengl/gl_openGLWrap.h>
#include <hdr/opengl/gl_shaders.h>
#include <hdr/opengl/gl_fbo.h>
#include <hdr/game/s_renderDebug.h>
#include <hdr/io/io_mouse.h>
#include <hdr/game/s_lightCaster.h>
#include <hdr/opengl/s_renderSprite.h>
#include <hdr/game/s_hud.h>
#include "hdr/game/s_render.h"

#define USE_TILE_LOOKUP 1

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
vector<unsigned int>    tileCoordsIndex;

vector<float>           singleTileTexCoords;

int                     numTileAcrossInTexture, numTilesDownInTexture;
float                   tileTextureWidth;
int                     indexCounter = 0;

float                   pixelX = 0, pixelY = 0;
float                   tilePosY = 0, tilePosX = 0;
float                   viewPixelX = 0, viewPixelY = 0;
float                   aspectRatioX, aspectRatioY;;


//-----------------------------------------------------------------------------
//
// Set the X Position for each tile in the master texture
void gam_setSingleTileCoords(int posX, int totalWidth)
//-----------------------------------------------------------------------------
{
	float glPosX = 0.0f;

	glPosX = (float)posX / (float)totalWidth;

	singleTileTexCoords.push_back(glPosX);
}

//-----------------------------------------------------------------------------
//
// Return texture coords for passed in tile
vec2 gam_getTileTexCoords(int whichTile)
//-----------------------------------------------------------------------------
{
	vec2 returnValue;

	returnValue.x = singleTileTexCoords[whichTile];
	returnValue.y = 0.0f;

	return returnValue;
}

void gam_blitFrameBufferToScreen(string whichShader, GLuint whichTexture)
{
	_tileCoords     tempCoord;
	float startTexX, startTexY, widthTex, heightTex;

	tileCoords.clear();
	tileCoordsIndex.clear();
	indexCounter = 0;

	startTexX = viewPixelX / (levelInfo.at(currentLevelName).levelDimensions.x * TILE_SIZE);
	startTexY = viewPixelY / (levelInfo.at(currentLevelName).levelDimensions.y * TILE_SIZE);

	widthTex = winWidth / (levelInfo.at(currentLevelName).levelDimensions.x * TILE_SIZE);
	heightTex = winHeight / (levelInfo.at(currentLevelName).levelDimensions.y * TILE_SIZE);

	//
	// Corner 0
	//
	tempCoord.position.x = 0; //viewPixelX;
	tempCoord.position.y = 0; //viewPixelY;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = startTexX;
	tempCoord.textureCoords.y = startTexY + heightTex;

	tileCoords.push_back(tempCoord);
	//
	// Corner 1
	//
	tempCoord.position.x = 0; //viewPixelX;
	tempCoord.position.y = winHeight; //viewPixelY + winHeight;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = startTexX;
	tempCoord.textureCoords.y = startTexY;

	tileCoords.push_back(tempCoord);
	//
	// Corner 2
	//
	tempCoord.position.x = winWidth;
	tempCoord.position.y = winHeight;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = startTexX + widthTex;
	tempCoord.textureCoords.y = startTexY;

	tileCoords.push_back(tempCoord);
	//
	// Corner 3
	//
	tempCoord.position.x = winWidth;
	tempCoord.position.y = 0;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = startTexX + widthTex;
	tempCoord.textureCoords.y = startTexY + heightTex;

	tileCoords.push_back(tempCoord);

	//
	// Indexes into the vertex array
	tileCoordsIndex.push_back(0 + indexCounter);
	tileCoordsIndex.push_back(1 + indexCounter);
	tileCoordsIndex.push_back(2 + indexCounter);
	tileCoordsIndex.push_back(2 + indexCounter);
	tileCoordsIndex.push_back(3 + indexCounter);
	tileCoordsIndex.push_back(0 + indexCounter);

	indexCounter+= 4;


	static GLuint       vao = 0;
	static GLuint       vbo = 0;
	static GLuint       elementbuffer = 0;

	// create the VAO
	GL_ASSERT (glGenVertexArrays (1, &vao));
	GL_CHECK (glBindVertexArray (vao));


	glGenBuffers(1, &elementbuffer);
	GL_CHECK (glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer));
	GL_CHECK (glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * tileCoordsIndex.size(), &tileCoordsIndex[0], GL_DYNAMIC_DRAW));

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
//	GL_CHECK (glUniform2fv           (gl_getUniform (whichShader, "screenSize"), 1, glm::value_ptr (glm::vec2(winWidth, winHeight))));
	GL_CHECK (glEnableVertexAttribArray    (gl_getAttrib  (whichShader, "inPosition")));
	GL_CHECK (glEnableVertexAttribArray    (gl_getAttrib  (whichShader, "inTextureCoords")));

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

	GL_CHECK (glDrawElements(GL_TRIANGLES, (GLsizei)tileCoordsIndex.size(), GL_UNSIGNED_INT, (void*)0 ));

	glUseProgram (0);
	glBindVertexArray (0);

	glDeleteBuffers (1, &elementbuffer);
	glDeleteBuffers (1, &vbo);
	glDeleteVertexArrays (1, &vao);

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

	tileCoords.push_back(tempCoord);
	//
	// Corner 1
	//
	tempCoord.position.x = destX;
	tempCoord.position.y = destY + TILE_SIZE;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = textureCoords.x;
	tempCoord.textureCoords.y = (textureCoords.y + 1.0f);

	tileCoords.push_back(tempCoord);
	//
	// Corner 2
	//
	tempCoord.position.x = destX + TILE_SIZE;
	tempCoord.position.y = destY + TILE_SIZE;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = (textureCoords.x + tileTextureWidth);
	tempCoord.textureCoords.y = (textureCoords.y + 1.0f);

	tileCoords.push_back(tempCoord);

	//
	// Corner 3
	//
	tempCoord.position.x = destX + TILE_SIZE;
	tempCoord.position.y = destY;
	tempCoord.position.z = 0.0f;

	tempCoord.textureCoords.x = (textureCoords.x + tileTextureWidth);
	tempCoord.textureCoords.y = textureCoords.y;

	tileCoords.push_back(tempCoord);
	//
	// Indexes into the vertex array
	tileCoordsIndex.push_back(0 + indexCounter);
	tileCoordsIndex.push_back(1 + indexCounter);
	tileCoordsIndex.push_back(2 + indexCounter);
	tileCoordsIndex.push_back(2 + indexCounter);
	tileCoordsIndex.push_back(3 + indexCounter);
	tileCoordsIndex.push_back(0 + indexCounter);

	indexCounter+= 4;
}

//-----------------------------------------------------------------------------
//
// Draw the entire level to the texture currently bound to the FBO
void gam_drawAllTiles ( const string whichShader, GLuint whichTexture )
//-----------------------------------------------------------------------------
{
	int         countX, countY, index;
	int         whichTile = 0;
	cpFloat     tilePtr = 0;
	GLuint      vao = 0;
	GLuint      vbo = 0;
	GLuint      elementbuffer = 0;
	static bool initDone = false;

	countY = 0;
	countX = 0;

	tileCoords.clear();
	tileCoordsIndex.clear();
	indexCounter = 0;

	for (index = 0; index < levelInfo.at(currentLevelName).levelDimensions.x * levelInfo.at(currentLevelName).levelDimensions.y; index++)
	{
		tilePtr = static_cast<int>((countY * levelInfo.at(currentLevelName).levelDimensions.x) + countX);

		whichTile = levelInfo.at(currentLevelName).tiles[static_cast<int>(tilePtr)];

		if ((whichTile > 0) && (whichTile < 64))
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
			gam_drawSingleTile ((countX * TILE_SIZE), (countY * TILE_SIZE), whichTile);
		}
		else
			if (whichTile != 0)
				con_print(CON_ERROR, true, "Invalid tile index [ %i ].", whichTile);

		countX++;
		if (countX == (int)levelInfo.at(currentLevelName).levelDimensions.x )
		{
			countX = 0;
			countY++;
		}
	}

	//
	// Setup shader and buffer data parameters
	// TODO: will need to reset on level change
	//
	if (!initDone)
	{
		// create the VAO
		GL_ASSERT (glGenVertexArrays (1, &vao));
//		GL_ASSERT (glGenBuffers(1, &elementbuffer));
		elementbuffer = wrapglGenBuffers (1, __func__);
		// Create buffers for the vertex data
		vbo = wrapglGenBuffers (1, __func__);

		GL_CHECK (glBindVertexArray (vao));

		GL_CHECK (glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer));
		GL_CHECK (glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * tileCoordsIndex.size(), &tileCoordsIndex[0], GL_DYNAMIC_DRAW));

		GL_CHECK (glBindBuffer (GL_ARRAY_BUFFER, vbo));
		GL_CHECK (glBufferData (GL_ARRAY_BUFFER, sizeof(_tileCoords) * tileCoords.size(), &tileCoords[0].position, GL_DYNAMIC_DRAW));

		GL_CHECK (glUseProgram (gl_getShaderID(whichShader)));

		GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inPosition")));
		GL_CHECK (glVertexAttribPointer     (gl_getAttrib (whichShader, "inPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(_tileCoords), (GLvoid *)offsetof(_tileCoords, position) ));

		GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inTextureCoords")));
		GL_CHECK (glVertexAttribPointer     (gl_getAttrib (whichShader, "inTextureCoords"), 2, GL_FLOAT, GL_FALSE, sizeof(_tileCoords), (GLvoid *)offsetof(_tileCoords, textureCoords)));

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
	GL_CHECK (glUniformMatrix4fv           (gl_getUniform (whichShader, "MVP_Matrix"), 1, false, glm::value_ptr (MVP)));

	GL_CHECK (glDrawElements(GL_TRIANGLES, (GLsizei)tileCoordsIndex.size(), GL_UNSIGNED_INT, (void*)0 ));

	glUseProgram (0);
	glBindVertexArray (0);
}

//----------------------------------------------------------------------------------------
//
// Draw the current level
// 1. Create the texture to hold the entire level
// 2. Create a FBO and bind depth and texture to it
void gam_drawFullLevel(string levelName, string whichShader, GLuint whichTexture)
//----------------------------------------------------------------------------------------
{
	static bool textureCreated = false;
	static GLuint fullLevelTexture = 0;
	static GLuint FramebufferName = 0;
	static GLuint depthTargetBuffer = 0;

	float textureWidth, textureHeight;

	if ( !textureCreated )
	{
		textureWidth = static_cast<float>(levelInfo.at(currentLevelName).levelDimensions.x);
		textureHeight = static_cast<float>(levelInfo.at(currentLevelName).levelDimensions.y);

		textureWidth *= TILE_SIZE;
		textureHeight *= TILE_SIZE;

		glGenFramebuffers(1, &FramebufferName);
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

		glGenTextures(1, &fullLevelTexture);
		glBindTexture(GL_TEXTURE_2D, fullLevelTexture);

		// Texture is empty
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(textureWidth), static_cast<GLsizei>(textureHeight), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

		// Set "fullLevelTexture" as our colour attachment #0
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fullLevelTexture, 0);

		//
		// Generate the depth buffer and associate to the frameBuffer
		glGenRenderbuffers(1, &depthTargetBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthTargetBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, static_cast<GLsizei>(textureWidth),
		                      static_cast<GLsizei>(textureHeight));
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTargetBuffer);

		// Set the list of draw buffers.
		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

		// Always check that our framebuffer is ok
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			con_print(CON_ERROR, true, "Error creating frameBuffer for tileLevel.");
			return;
		}

		io_storeTextureInfoIntoMap(fullLevelTexture, glm::vec2{textureWidth,textureHeight}, "fullLevelTexture", false);

		textureCreated = true;  // Reset on level change
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	auto viewPortX = static_cast<GLsizei>((levelInfo.at(currentLevelName).levelDimensions.x * TILE_SIZE));
	auto viewPortY = static_cast<GLsizei>(levelInfo.at(currentLevelName).levelDimensions.y * TILE_SIZE);

	glClearColor (1.0f, 0.0f, 0.0f, 0.0f);
	glViewport(0,0,viewPortX, viewPortY); // Render on the whole framebuffer
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gl_set2DMode(viewPortX, viewPortY, glm::vec3(1, 1, 1));

	//
	// Draw tiles to bound texture from 'whichTexture'
	gam_drawAllTiles ("quad3d", whichTexture);

	gam_showLineSegments();
	gam_showWayPoints();

	io_renderMouseCursor();

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

	glViewport(0,0,256, 256); // Render on the whole framebuffer
	light_createLightCaster (testLightPosition);

	glViewport(0,0,viewPortX, viewPortY); // Render on the whole framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fullLevelTexture, 0);

glm::vec2       lightSize;

lightSize = io_getTextureSize ("lightcaster");

	gl_draw2DQuad (glm::vec2{testLightPosition.x, testLightPosition.y}, glm::vec2{256,256}, "lightmapRender", io_getTextureID ("lightmap"), glm::vec3{0,0,0});
//	gl_draw2DQuad (glm::vec2{testLightPosition.x, testLightPosition.y}, glm::vec2{lightSize.x,lightSize.y}, "lightmapRender", io_getTextureID ("lightmap"), 0.0f);

//	light_createLightCaster (vec3(750.0, 400.0, 0.0));
	//
	// Switch back to rendering to default frame buffer
	gl_renderToScreen ();

	viewPortX = static_cast<GLsizei>(winWidth * aspectRatioX);
	viewPortY = static_cast<GLsizei>(winHeight * aspectRatioY);

	glViewport(0, 0, viewPortX, viewPortY);
	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//
	// TODO: Make scale values variable
	gl_set2DMode(viewPortX, viewPortY, glm::vec3(3, 3, 1));
	//
	// Copy screen sized quad from backing texture to visible screen
	gam_blitFrameBufferToScreen("quad3d", fullLevelTexture);

	gl_renderToScreen ();
	//
	// Render HUD on top of everything
	gl_set2DMode(viewPortX, viewPortY, glm::vec3(1, 1, 1));
	s_renderHUD();
}