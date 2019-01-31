#include "hdr/gui/gui_terminal.h"

GLuint        terminalDroidTexture;
bool          haveTextureID = false;
int           current3DTextureIndex = 0;
int           dataBaseCurrentFrame = 0;
SDL_TimerID   timerDataBaseAnimate;
bool          do3DTextureAnimate = false;
Uint32        dbAnimateSpeed;     // From Script

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
  string terminalFileName;

  terminalFileName = "db_" + droidType + ".jpg";

  gl_removeSprite (droidType);
  io_removeTextureFromMap (droidType);
}

//-----------------------------------------------------------------------------------------------------
//
// Load the 3d texture to display in the terminal
void gui_load3DModelTexture (const std::string &droidType)
//-----------------------------------------------------------------------------------------------------
{
  char *imageBuffer = nullptr;
  int imageLength;
  SDL_Surface *textureSurface;
  SDL_RWops *filePointerMem;
  string terminalFileName;

  terminalFileName = "db_" + droidType + ".jpg";

//	con_print(CON_INFO, true, "Step 1 - load texture file [ %s ]", terminalFileName.c_str());

  imageLength = (int) io_getFileSize (terminalFileName.c_str ());
  if (imageLength < 0)
    {
      evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_ERROR_NOT_FOUND, 0, 0, vec2 (), vec2 (), terminalFileName);
      return;
    }

//	con_print (CON_INFO, true, "Image size [ %i ]", imageLength);

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

  textureSurface = SDL_LoadBMP_RW (filePointerMem, 1);     // free stream
  if (nullptr == textureSurface)
    {
      evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_ERROR_SOIL, 0, 0, vec2 (), vec2 (), terminalFileName);
      return;
    }


  //
  // Upload SDL_Surface into a OpenGL texture
  //
  SDL_LockSurface (textureSurface);

  if (!haveTextureID)
    {
      glGenTextures (1, &terminalDroidTexture);
      haveTextureID = true;
    }

  glBindTexture (GL_TEXTURE_2D, terminalDroidTexture);

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLenum Mode = GL_RGBA;

  glTexImage2D (GL_TEXTURE_2D, 0, Mode, textureSurface->w, textureSurface->h, 0, Mode, GL_UNSIGNED_BYTE, textureSurface->pixels);

  SDL_UnlockSurface (textureSurface);

  glBindTexture (GL_TEXTURE_2D, 0);

  SDL_FreeSurface (textureSurface);

  free (imageBuffer);

  io_storeTextureInfoIntoMap (terminalDroidTexture, glm::vec2 (textureSurface->w, textureSurface->h), terminalFileName, true);

  gl_createSprite (terminalFileName, glm::vec3 (0, 0, 0), 32, glm::vec2 (1.0f, 1.0f), glm::vec3 (1.0, 1.0, 1.0));
}

//-----------------------------------------------------------------------------------------------------
//
// Change to terminal mode
void gui_changeToTerminalMode (const std::string textureName)
//-----------------------------------------------------------------------------------------------------
{
  current3DTextureIndex = gui_get3DTextureIndex (textureName);
  gui_load3DModelTexture (textureName);
}

//-----------------------------------------------------------------------------------------------------
//
// Exit terminal mode - clean up sprite
void gui_exitTerminalMode ()
//-----------------------------------------------------------------------------------------------------
{
  gui_remove3DModelTexture (droidToSpriteLookup[current3DTextureIndex]);
}

//-----------------------------------------------------------------------------------------------------
//
// Render the terminal screen
void gui_renderTerminal ()
//-----------------------------------------------------------------------------------------------------
{
  string _3DTextureFilename;

  _3DTextureFilename = "db_" + droidToSpriteLookup[current3DTextureIndex];

  gl_renderSprite (_3DTextureFilename, glm::vec2{200,200}, 0, dataBaseCurrentFrame, glm::vec3{1, 1, 0});
}