#include <hdr/gui/gui_scrollBox.h>
#include <hdr/game/gam_player.h>
#include <hdr/gui/gui_language.h>
#include <hdr/libGL/soil/soil.h>
#include "hdr/gui/gui_terminal.h"

GLuint        terminalDroidTexture;
bool          haveTextureID = false;
int           current3DTextureIndex = 0;
int           dataBaseCurrentFrame = 0;
SDL_TimerID   timerDataBaseAnimate;
bool          do3DTextureAnimate = false;
Uint32        dbAnimateSpeed;     // From Script
SDL_TimerID   timerDataBaseScroll = 0;
bool          doDataBaseScroll = false;
float         dbImagePositionX, dbImagePositionY;
string        _3DTextureFilename;
bool          reloadDataBaseTexture = false;

//------------------------------------------------------------------------
//
// Fine scroll the database scrollbox - called from Timer
Uint32 gui_scrollDataBaseText(Uint32 interval, void *param )
//------------------------------------------------------------------------
{
  if (!doDataBaseScroll)
    return interval;

  //
  // Next pixel line
  databaseScrollBox.scrollY++;

  if (databaseScrollBox.scrollY > ttfFonts[gui_getFontIndex (guiFontName)].fontHeight)
    {
      databaseScrollBox.scrollY = 0;
      gui_getNextTextLine(&databaseScrollBox);
      databaseScrollBox.linesToPrint++;
      if (databaseScrollBox.linesToPrint > databaseScrollBox.numVisibleLinesHeight)
        databaseScrollBox.linesToPrint = databaseScrollBox.numVisibleLinesHeight;
    }

  return interval;
}


//------------------------------------------------------------------------
//
// Create timer to control the speed of the scrolling intro
void gui_timerScrollSpeedDatabase(int newState, Uint32 scrollSpeed)
//------------------------------------------------------------------------
{
  switch ( newState )
    {
      case USER_EVENT_TIMER_OFF:
        {
          doDataBaseScroll = false;
          break;
        }
      case USER_EVENT_TIMER_ON:
        {
          if (timerDataBaseScroll == 0)
            timerDataBaseScroll = evt_registerTimer(scrollSpeed, gui_scrollDataBaseText, "Database scroll speed");

          doDataBaseScroll = true;
          break;
        }
      default:
        break;
    }
}

// ----------------------------------------------------------------------------
//
// Turn on / off the animation for the droid in the database view
void gui_setDatabaseAnimateState( bool newState )
// ----------------------------------------------------------------------------
{
  do3DTextureAnimate = newState;
}

// ----------------------------------------------------------------------------
//
// Animate the player sprite - called from timer callback
// Runs at tick rate set by 'playerAnimateInterval' - derived from current health
Uint32 gam_dataBaseAnimateTimerCallback ( Uint32 interval, void *param )
// ----------------------------------------------------------------------------
{
  if ( !do3DTextureAnimate )
    return interval;

  dataBaseCurrentFrame++;
  if ( dataBaseCurrentFrame == 32)  //sprites.at("db_001").numberOfFrames )
    dataBaseCurrentFrame = 0;

  return interval;
}

// ----------------------------------------------------------------------------
//
// Initiate the timer to animate the player sprite
//
// Pass in time in milliseconds
void gam_initDataBaseAnimateTimer ( Uint32 interval )
// ----------------------------------------------------------------------------
{
  timerDataBaseAnimate = evt_registerTimer(interval, gam_dataBaseAnimateTimerCallback, "Database animation");
}

//-----------------------------------------------------------------------------------------------------
//
// Get the initial index into the droidType name
int gui_get3DTextureIndex (const std::string droidType)
//-----------------------------------------------------------------------------------------------------
{
  for (int i = 0; i != droidToSpriteLookup.size (); i++)
    {
      if (droidType == droidToSpriteLookup[i])
        {
          return i;
        }
    }
}

//-----------------------------------------------------------------------------------------------------
//
// Remove the current 3d texture
void gui_remove3DModelTexture (const std::string &droidType)
//-----------------------------------------------------------------------------------------------------
{

  printf ("Delete database texture [ %s ]\n", droidType.c_str());

  gl_removeSprite (droidType);
  io_removeTextureFromMap (droidType);
}

//-----------------------------------------------------------------------------------------------------
//
// Load the 3d texture to display in the terminal
void gui_load3DModelTexture (const std::string &terminalFileName)
//-----------------------------------------------------------------------------------------------------
{
  char            *imageBuffer = nullptr;
  int             imageLength;
  SDL_Surface     *textureSurface;
  SDL_RWops       *filePointerMem;

  con_print(CON_INFO, true, "Step 1 - load texture file [ %s ]", terminalFileName.c_str());

  imageLength = (int) io_getFileSize (terminalFileName.c_str ());
  if (imageLength < 0)
    {
      evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_ERROR_NOT_FOUND, 0, 0, vec2 (), vec2 (), terminalFileName);
      return;
    }

con_print (CON_INFO, true, "Image size [ %i ]", imageLength);

  imageBuffer = (char *) malloc (sizeof (char) * imageLength);
  if (nullptr == imageBuffer)
    {
      evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_MALLOC_ERROR, 0, 0, vec2 (), vec2 (), terminalFileName);
      return;
    }

  if (-1 == io_getFileIntoMemory (terminalFileName.c_str (), imageBuffer))
    {
      evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_MEMORY_ERROR, 0, 0, vec2 (), vec2 (), terminalFileName);
      free (imageBuffer);
      imageBuffer = nullptr;
      return;
    }

  filePointerMem = SDL_RWFromMem (imageBuffer, imageLength);
  if (nullptr == filePointerMem)
    {
      con_print (CON_ERROR, true, "Error getting SDL RWops from bitmap memory file.");
      evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_ERROR_SOIL, 0, 0, vec2 (), vec2 (), terminalFileName);
    }

  textureSurface = SDL_LoadBMP_RW (filePointerMem, 1);
  if (nullptr == textureSurface)
    {
      evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_ERROR_SOIL, 0, 0, vec2 (), vec2 (), terminalFileName);
      return;
    }

SDL_SaveBMP (textureSurface, "testDroid.bmp");

  /*
  glm::vec2   imageDimensions = io_getTextureSize ("db_001");

  glBindTexture (GL_TEXTURE_2D, terminalDroidTexture);
  glTexSubImage2D(terminalDroidTexture, 0, 0, 0,  textureSurface->w,  textureSurface->h,
                       GL_RGBA,  GL_UNSIGNED_BYTE,  textureSurface->pixels);

  glBindTexture (GL_TEXTURE_2D, 0);
*/

  //
//  terminalDroidTexture = SOIL_load_OGL_texture_from_memory ((const unsigned char *)textureSurface->pixels, imageLength, SOIL_LOAD_AUTO, terminalDroidTexture, SOIL_FLAG_INVERT_Y ); //SOIL_FLAG_TEXTURE_REPEATS); SOIL_FLAG_MIPMAPS





  // TODO Tell main thread to replace the texture date

  //
  // Upload SDL_Surface into a OpenGL texture
  //
  SDL_LockSurface (textureSurface);

  terminalDroidTexture = io_getTextureID ( "db_001" );

  glBindTexture (GL_TEXTURE_2D, terminalDroidTexture);

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int Mode = GL_RGB;

  if (textureSurface->format->BytesPerPixel == 4)
    {
      Mode = GL_RGBA;
    }

  GL_CHECK(glTexImage2D (GL_TEXTURE_2D, 0, Mode, textureSurface->w, textureSurface->h, 0, Mode, GL_UNSIGNED_BYTE, textureSurface->pixels));

  SDL_UnlockSurface (textureSurface);

  glBindTexture (GL_TEXTURE_2D, 0);

  SDL_FreeSurface (textureSurface);

  free (imageBuffer);

//  io_storeTextureInfoIntoMap (terminalDroidTexture, glm::vec2 (textureSurface->w, textureSurface->h), terminalFileName, true);

//  gl_createSprite (terminalFileName, glm::vec3 (0, 0, 0), 32, glm::vec2 (1.0f, 1.0f), glm::vec3 (1.0, 1.0, 1.0));
}

//-----------------------------------------------------------------------------------------------------
//
// Check if the main thread needs to reload the database texture
void gui_checkDataBaseTextureReload()
//-----------------------------------------------------------------------------------------------------
{
  if (reloadDataBaseTexture)
    {
      gui_load3DModelTexture ("db_" + droidToSpriteLookup[current3DTextureIndex] + ".bmp");

      reloadDataBaseTexture = false;
    }
}

//-----------------------------------------------------------------------------------------------------
//
// Enter terminal mode - start timer
void gui_enterTerminalMode ()
//-----------------------------------------------------------------------------------------------------
{
  reloadDataBaseTexture = true;

  current3DTextureIndex = playerDroid.droidType;

  databaseScrollBox.charPtr = 0;                       // start of the text
  databaseScrollBox.scrollY = 0;

  for (int i = 0; i != databaseScrollBox.numVisibleLinesHeight; i++)
    {
      databaseScrollBox.textLine[i] = "";
    }
  databaseScrollBox.sourceText = gui_getString("db_" + droidToSpriteLookup[current3DTextureIndex]);

  gui_timerScrollSpeedDatabase(USER_EVENT_TIMER_ON, dbScrollSpeed);
}

//-----------------------------------------------------------------------------------------------------
//
// Exit terminal mode - stop timer
void gui_exitTerminalMode ()
//-----------------------------------------------------------------------------------------------------
{
  gui_timerScrollSpeedDatabase(USER_EVENT_TIMER_OFF, dbScrollSpeed);
}

//-----------------------------------------------------------------------------------------------------
//
// Render the terminal screen
void gui_renderDataBase ()
//-----------------------------------------------------------------------------------------------------
{
  int startX, startY, width, height, lineWidth;
  int framePadding;

  glm::vec2 imageSize;

  if (timerDataBaseScroll == 0)
    gui_timerScrollSpeedDatabase(USER_EVENT_TIMER_ON, dbScrollSpeed);

  imageSize = io_getTextureSize ("db_001");

  framePadding = 10;

  startX = dbStartX + dbWidth + framePadding;
  startY = (dbStartY - dbHeight) + (framePadding * 2) + 4;
  width = dbWidth + (framePadding * 2);
  height = dbHeight + (framePadding * 2);
  lineWidth = 4;

  roundedBoxRGBA (renderer, startX, startY, startX - width, startY + height, 8, 255, 255, 255, 255);

  roundedBoxRGBA (renderer, startX - lineWidth, startY + lineWidth, (startX - width) + lineWidth, (startY + height) - lineWidth, 8, 0, 0, 0, 255);

  width = sprites.at("db_001").textureSize.x / sprites.at("db_001").numberOfFrames;
  height = sprites.at("db_001").frameHeight;

  startX = dbImagePositionX + width + 100;    // Start drawing off screen
  startY = dbImagePositionY - (height / 2);

  width *= 2;
  width += 100;

  lineWidth = 1;

  roundedRectangleRGBA (renderer, startX, startY, startX - width, startY + height, 8, 255, 255, 255, 255);

  roundedRectangleRGBA (renderer, startX - lineWidth, startY + lineWidth, (startX - width) + lineWidth, (startY + height) - lineWidth, 8, 255, 255, 255, 255);

  gui_drawScrollBox (&databaseScrollBox);

  gl_renderSprite ("db_001", glm::vec2{dbImagePositionX,dbImagePositionY}, 0, dataBaseCurrentFrame, glm::vec3{1, 1, 0});
}

//------------------------------------------------------------------------
//
// Get the next droid
void gui_getNextDataBaseRecord()
//------------------------------------------------------------------------
{
  if (current3DTextureIndex == playerDroid.droidType)
    return;

  if (current3DTextureIndex < 23 )
    {
      current3DTextureIndex++;

      databaseScrollBox.charPtr = 0;   // start of the text
      databaseScrollBox.scrollY = 0;

      for (int i = 0; i != databaseScrollBox.numVisibleLinesHeight; i++)
        {
          databaseScrollBox.textLine[i] = "";
        }
      databaseScrollBox.sourceText = gui_getString("db_" + droidToSpriteLookup[current3DTextureIndex]);

      reloadDataBaseTexture = true;
    }
}

//------------------------------------------------------------------------
//
// Get the previous droid
void gui_getPreviousDataBaseRecord()
//------------------------------------------------------------------------
{
  if (current3DTextureIndex > 0)
    {
      current3DTextureIndex--;

      databaseScrollBox.charPtr = 0;
      databaseScrollBox.scrollY = 0;

      for (int i = 0; i != databaseScrollBox.numVisibleLinesHeight; i++)
        {
          databaseScrollBox.textLine[i] = "";
        }
      databaseScrollBox.sourceText = gui_getString("db_" + droidToSpriteLookup[current3DTextureIndex]);

      reloadDataBaseTexture = true;
    }
}