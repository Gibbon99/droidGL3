#include "hdr/opengl/gl_openGLWrap.h"
#include "hdr/opengl/gl_shaders.h"
#include "hdr/opengl/gl_fbo.h"
#include "hdr/game/gam_renderDebug.h"
#include "hdr/io/io_mouse.h"
#include "hdr/game/gam_lightCaster.h"
#include "hdr/opengl/gl_renderSprite.h"
#include "hdr/game/gam_hud.h"
#include "hdr/game/gam_player.h"
#include "hdr/game/gam_doors.h"
#include "hdr/game/gam_pathFind.h"
#include "hdr/game/gam_render.h"

#define USE_TILE_LOOKUP 1

//#define USE_BLIT 1

//	1--------2
//	|        |
//  |        |
//  |        |
//  0--------3

typedef struct {
  glm::vec3 position;
  glm::vec2 textureCoords;
} _tileCoords;

vector<_tileCoords>       tileCoords;
vector<unsigned int>      tileCoordsIndex;

vector<float>             singleTileTexCoords;

int                       numTileAcrossInTexture, numTilesDownInTexture;
float                     tileTextureWidth;
int                       indexCounter = 0;


float                     g_scaleViewBy = 1.4f;
int                       g_playFieldSize;    // From script

GLuint                    fullLevelTexture = 0;
GLuint                    fullLevel_FBO = 0;
GLuint                    virtualScreen_FBO = 0;

glm::vec2                 backingSize;
glm::vec2                 virtualScreenSize;
bool                      levelInitDone = false;

float                     zDistance = 0.0f;

//-----------------------------------------------------------------------------
//
// Set the X Position for each tile in the master texture
void gam_setSingleTileCoords (int posX, int totalWidth)
//-----------------------------------------------------------------------------
{
  float glPosX = 0.0f;

  glPosX = (float) posX / (float) totalWidth;

  singleTileTexCoords.push_back (glPosX);
}

//-----------------------------------------------------------------------------
//
// Return texture coords for passed in tile
vec2 gam_getTileTexCoords (int whichTile)
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
void gam_blitFrameBufferToBoundTexture (const string &whichShader, const string levelName, GLuint whichTexture, glm::vec2 viewSize, float interpolate)
//-----------------------------------------------------------------------------
{
  _tileCoords tempCoord;
  float startTexX, widthTex, heightTex;
  cpFloat startTexY;

  tileCoords.clear ();
  tileCoordsIndex.clear ();
  indexCounter = 0;

  glm::vec2 backingTextureSize;
  backingTextureSize = io_getTextureSize (levelName);

  widthTex = viewSize.x / backingTextureSize.x;
  heightTex = viewSize.y / backingTextureSize.y;

// TODO: Velocity for player


  startTexX = static_cast<float>((playerDroid.viewWorldPos.x / backingTextureSize.x) - (widthTex / 2));
  startTexY = static_cast<float>((playerDroid.viewWorldPos.y / backingTextureSize.y) - (heightTex / 2));

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
  GL_CHECK (glBufferData (GL_ELEMENT_ARRAY_BUFFER,
                          sizeof (unsigned int) * tileCoordsIndex.size (), &tileCoordsIndex[0], GL_DYNAMIC_DRAW));

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
void inline gam_drawSingleTile (float destX, float destY, int whichTile)
//-----------------------------------------------------------------------------
{
  static float previousTile = -1;
  static vec2 textureCoords;

  _tileCoords tempCoord;

  if (previousTile != whichTile)
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
  tempCoord.position.z = zDistance;

  tempCoord.textureCoords.x = textureCoords.x;
  tempCoord.textureCoords.y = textureCoords.y;

  tileCoords.push_back (tempCoord);
  //
  // Corner 1
  //
  tempCoord.position.x = destX;
  tempCoord.position.y = destY + TILE_SIZE;
  tempCoord.position.z = zDistance;

  tempCoord.textureCoords.x = textureCoords.x;
  tempCoord.textureCoords.y = (textureCoords.y + 1.0f);

  tileCoords.push_back (tempCoord);
  //
  // Corner 2
  //
  tempCoord.position.x = destX + TILE_SIZE;
  tempCoord.position.y = destY + TILE_SIZE;
  tempCoord.position.z = zDistance;

  tempCoord.textureCoords.x = (textureCoords.x + tileTextureWidth);
  tempCoord.textureCoords.y = (textureCoords.y + 1.0f);

  tileCoords.push_back (tempCoord);

  //
  // Corner 3
  //
  tempCoord.position.x = destX + TILE_SIZE;
  tempCoord.position.y = destY;
  tempCoord.position.z = zDistance;

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
void gam_drawAllTiles (const string whichShader, const string levelName, GLuint whichTexture)
//-----------------------------------------------------------------------------
{
  int countX, countY, index;
  int whichTile = 0;
  cpFloat tilePtr = 0;
  GLuint vao = 0;
  GLuint vbo = 0;
  GLuint elementbuffer = 0;
  _levelStruct levelItr;
  static bool initDone = false;
  static string previousLevelName;

  countY = 0;
  countX = 0;

  tileCoords.clear ();
  tileCoordsIndex.clear ();

  indexCounter = 0;

  //
  // Cache iterator to avoid costly levelInfo.at function
  //
  levelItr = levelInfo.at (levelName);

  for (index = 0; index < levelItr.levelDimensions.x * levelItr.levelDimensions.y; index++)
    {
      tilePtr = static_cast<int>((countY * levelItr.levelDimensions.x) + countX);

      whichTile = levelItr.tiles[static_cast<int>(tilePtr)];

      if ((whichTile > 0) && (whichTile < 64))
        {
          switch (whichTile)
            {
              case ALERT_GREEN_TILE:
              case ALERT_YELLOW_TILE:
              case ALERT_RED_TILE:
                switch (currentAlertLevel)
                  {
                    case ALERT_GREEN_TILE: whichTile = ALERT_GREEN_TILE;
                    break;
                    case ALERT_YELLOW_TILE: whichTile = ALERT_YELLOW_TILE;
                    break;
                    case ALERT_RED_TILE: whichTile = ALERT_RED_TILE;
                    break;
                    default: break;
                  }
              break;
              default: break;
            }
          gam_drawSingleTile ((countX * TILE_SIZE), (countY * TILE_SIZE), whichTile);
        }
      else if (whichTile != 0)
        con_print (CON_ERROR, true, "Invalid tile index [ %i ].", whichTile);

      countX++;
      if (countX == (int) levelItr.levelDimensions.x)
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
      elementbuffer = wrapglGenBuffers (1, __func__);
      // Create buffers for the vertex data
      vbo = wrapglGenBuffers (1, __func__);

      GL_CHECK (glBindVertexArray (vao));

      GL_CHECK (glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, elementbuffer));
      GL_CHECK (glBufferData (GL_ELEMENT_ARRAY_BUFFER,
                              sizeof (unsigned int) * tileCoordsIndex.size (), &tileCoordsIndex[0], GL_DYNAMIC_DRAW));

      GL_CHECK (glBindBuffer (GL_ARRAY_BUFFER, vbo));
      GL_CHECK (glBufferData (GL_ARRAY_BUFFER,
                              sizeof (_tileCoords) * tileCoords.size (), &tileCoords[0].position, GL_DYNAMIC_DRAW));

      GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));

      GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inPosition")));
      GL_CHECK (glVertexAttribPointer (gl_getAttrib (whichShader, "inPosition"), 3, GL_FLOAT, GL_FALSE, sizeof (_tileCoords), (GLvoid *) offsetof (_tileCoords, position)));

      GL_CHECK (glEnableVertexAttribArray (gl_getAttrib (whichShader, "inTextureCoords")));
      GL_CHECK (glVertexAttribPointer (gl_getAttrib (whichShader, "inTextureCoords"), 2, GL_FLOAT, GL_FALSE, sizeof (_tileCoords), (GLvoid *) offsetof (_tileCoords, textureCoords)));

      GL_CHECK (glUniform1i (gl_getUniform (whichShader, "inTexture0"), 0));
      GL_CHECK (glUniform1f (gl_getUniform (whichShader, "gamma"), g_gamma));

      glUseProgram (0);
      glBindVertexArray (0);

//		initDone = false;
    }

  GL_CHECK (glBindVertexArray (vao));

  GL_CHECK (glUseProgram (gl_getShaderID (whichShader)));
  //
  // Bind texture if it's not already bound as current texture
  GL_CHECK (glActiveTexture (GL_TEXTURE0));
  GL_CHECK (glBindTexture (GL_TEXTURE_2D, whichTexture));

  GL_CHECK (glUniform1i (gl_getUniform (whichShader, "inTexture0"), 0));
  GL_CHECK (glUniform1f (gl_getUniform (whichShader, "gamma"), g_gamma));

  GL_CHECK (glUniformMatrix4fv (gl_getUniform (whichShader, "MVP_Matrix"), 1, false, glm::value_ptr (MVP)));

  GL_CHECK (glDrawElements (GL_TRIANGLES, (GLsizei) tileCoordsIndex.size (), GL_UNSIGNED_INT, (void *) nullptr));

  glUseProgram (0);
  glBindVertexArray (0);
}

//----------------------------------------------------------------------------------------
//
// Create a texture of passed in size and name.
// Store it in the texture Map array
void gam_createSizedTexture (const string &textureName, glm::vec2 textureSize)
//----------------------------------------------------------------------------------------
{
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
// Init the drawing for a level - called on a level change
void gam_initLevelDrawing (const string levelName)
//----------------------------------------------------------------------------------------
{
  backingSize.x = static_cast<float>(levelInfo.at (levelName).levelDimensions.x * TILE_SIZE);
  backingSize.y = static_cast<float>(levelInfo.at (levelName).levelDimensions.y * TILE_SIZE);

  virtualScreenSize.x = g_playFieldSize;
  virtualScreenSize.y = g_playFieldSize;

  if (0 == fullLevel_FBO)
    {
      fullLevel_FBO = gl_createFBO (backingSize);
      //
      // Check it all worked ok
      if (0 == fullLevel_FBO)
        {
          con_print (CON_ERROR, true, "Unable to create backing FBO. Critical error.");
          return;     // TODO: Do something better here to handle the failure
        }
    }

    if ( 0 == virtualScreen_FBO)
      {
        virtualScreen_FBO = gl_createFBO (virtualScreenSize);
        //
        // Check it was created ok
        if (0 == virtualScreen_FBO)
          {
            con_print (CON_ERROR, true, "Unable to create backing FBO for virtual screen. Critical error.");
            return;
          }
      }

  gam_createSizedTexture (levelName, backingSize);
  //
  // Start drawing to backing texture
//	glBindFramebuffer (GL_FRAMEBUFFER, fullLevel_FBO);

  //  gl_linkTextureToFBO (io_getTextureID (levelName), fullLevel_FBO, GL_FRAMEBUFFER);

  gl_linkTextureToFBO (io_getTextureID ("viewTexture"), fullLevel_FBO, GL_FRAMEBUFFER);

  gam_createSizedTexture ("viewTexture", virtualScreenSize);

  gl_linkTextureToFBO (io_getTextureID ("virtualScreen"), virtualScreen_FBO, GL_DRAW_FRAMEBUFFER);

}

//----------------------------------------------------------------------------------------
//
// Blit the virtual screen from the full texture
void gam_getVirtualScreen (const string levelName)
//----------------------------------------------------------------------------------------
{

  static bool doneFileWrite = false;

  int virtSize = g_playFieldSize;

  if (!gl_linkTextureToFBO (io_getTextureID (levelName), fullLevel_FBO, GL_READ_FRAMEBUFFER))
    {
      con_print (CON_ERROR, true, "Unable to bind texture [ %s ] to FBO.", levelName.c_str ());
      return;
    }

  if (!gl_linkTextureToFBO (io_getTextureID ("viewTexture"), virtualScreen_FBO, GL_DRAW_FRAMEBUFFER))
    {
      con_print (CON_ERROR, true, "Unable to bind texture [ %s ] to FBO.", "viewTexture");
      return;
    }

  glBlitFramebuffer (
      playerDroid.viewWorldPos.x - (virtSize / 2),
      playerDroid.viewWorldPos.y - (virtSize / 2),
      playerDroid.viewWorldPos.x + (virtSize / 2),
      playerDroid.viewWorldPos.y + (virtSize / 2),

      0,
      0,
      virtSize,
      virtSize,

      GL_COLOR_BUFFER_BIT,
      GL_NEAREST);

  /*
  if (!doneFileWrite)
    {
      gl_linkTextureToFBO (io_getTextureID ("viewTexture"), virtualScreen_FBO, GL_READ_FRAMEBUFFER);

      gl_screenShot("virtualScreen.tga");
      doneFileWrite = true;
    }
    */
}

//----------------------------------------------------------------------------------------
//
// Reset the level init
void gam_resetRenderLevelInit ()
//----------------------------------------------------------------------------------------
{
  levelInitDone = false;
}

//----------------------------------------------------------------------------------------
//
// Draw the current level
// 1. Create the texture to hold the entire level
// 2. Create a FBO and bind depth and texture to it
// 3. Render the entire level to the backing texture
// 4. Render viewable playfield texture to another texture
// 5.
void gam_renderFullLevel (const string levelName, const string whichShader, GLuint sourceTexture, float interpolate)
//----------------------------------------------------------------------------------------
{
  static glm::vec2 viewSize;
  static glm::vec2 viewPortPosition;
  static float scaleView;

  float texCoords[] = {0.0, 1.0,
                       0.0, 0.0,
                       1.0, 0.0,
                       1.0, 1.0};

  if (levelName.empty ())
    {
      con_print (CON_ERROR, true, "No levelName passed to [ %s ]", __LINE__);
      return;
    }

  if (!levelInitDone)
    {
      gam_initLevelDrawing (levelName);

      scaleView = (float) winWidth / g_playFieldSize;

      viewSize = glm::vec2{g_playFieldSize, g_playFieldSize};

      viewSize.x *= scaleView;
      viewSize.y *= scaleView;

      viewPortPosition.x = (float) winWidth * 0.5f;
      viewPortPosition.y = (float) winHeight * 0.5f;

      levelInitDone = true;
    }

  gl_linkTextureToFBO (io_getTextureID (levelName), fullLevel_FBO, GL_DRAW_FRAMEBUFFER);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport (0, 0, static_cast<GLsizei>(backingSize.x), static_cast<GLsizei>(backingSize.y)); // Render on the whole backing FBO

  gl_set2DMode (glm::vec2{0.0, 0.0}, backingSize, glm::vec3 (1, 1, 1));
  //
  // Draw tiles to bound texture from 'sourceTexture'
  gam_drawAllTiles (whichShader, levelName, sourceTexture);

  gam_showLineSegments (levelName);
  gam_showWayPoints (levelName);

  for (int index = 0; index != levelInfo.at (levelName).numDroids; index++)
    {
      if (levelInfo.at (levelName).droid[index].aStarPathIndex > -1)
        gam_AStarDebugWayPoints (levelInfo.at (levelName).droid[index].aStarPathIndex);
    }

  gam_debugDoorTriggers (levelName);

  gam_renderDroids (levelName, interpolate);

  gam_renderPlayerSprite ();

  bul_renderBullet (levelName);


  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

  //light_createLightCaster (levelName, glm::vec3{playerDroid.worldPos.x, playerDroid.worldPos.y, 0});

//----------------------------------------------------------------------------
//
// Now copy the view size texture to another texture of the same size
//
//----------------------------------------------------------------------------

  gam_getVirtualScreen (levelName);


//----------------------------------------------------------------------------
//
// Take that texture and draw a quad to the default frame buffer ( screen )
//
//----------------------------------------------------------------------------

  //
  // Switch back to rendering to default frame buffer
  gl_renderToScreen ();
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport (0, 0, winWidth, winHeight);
  gl_set2DMode (glm::vec2{0, 0}, glm::vec2{winWidth, winHeight}, glm::vec3 (1, 1, 1));

  gl_draw2DQuad (viewPortPosition, 0, viewSize, "quad3d", io_getTextureID ("viewTexture"), glm::vec3{0, 0, 0},
                 glm::vec3{-1.0, 0.0, 0.0}, texCoords);


//  glViewport (0, 0, lightSize.x, lightSize.y);
//  gl_set2DMode (glm::vec2{0, 0}, lightSize, glm::vec3 (1, 1, 1));

//  gl_draw2DQuad (glm::vec2{playerDroid.worldPos.x, playerDroid.worldPos.y}, 0, lightSize, "quad3d", io_getTextureID ("lightcaster"), glm::vec3{0,0,0},
//                 glm::vec3{-1.0, 0.0, 0.0}, texCoords);

//  gl_draw2DQuad (glm::vec2{0, 0}, 0, lightSize, "quad3d", io_getTextureID ("lightcaster"), glm::vec3{0,0,0},
//                 glm::vec3{-1.0, 0.0, 0.0}, texCoords);

  //
  // Render HUD on top of everything
  s_renderHUD ();
}
