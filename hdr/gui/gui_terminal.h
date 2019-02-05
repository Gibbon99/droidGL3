#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/io/io_fileSystem.h"
#include "hdr/opengl/gl_renderSprite.h"

extern Uint32           dbAnimateSpeed;     // From Script
extern float            dbImagePositionX, dbImagePositionY;

// Change to terminal mode
void gui_changeToTerminalMode (std::string textureName);

// Enter terminal mode - start timer
void gui_enterTerminalMode ();

// Exit terminal mode - stop timer
void gui_exitTerminalMode ();

// Render the terminal screen
void gui_renderDataBase ();

// Get the initial index into the droidType name
int gui_get3DTextureIndex (std::string droidType);

// Turn on / off the animation for the droid in the database view
void gui_setDatabaseAnimateState(bool newState);

void gam_initDataBaseAnimateTimer ( Uint32 interval );

// Create timer to control the speed of the scrolling intro
void gui_timerScrollSpeedDatabase(int newState, Uint32 scrollSpeed);

// Get the next droid
void gui_getNextDataBaseRecord();

// Get the previous droid
void gui_getPreviousDataBaseRecord();

// Check if the main thread needs to reload the database texture
void gui_checkDataBaseTextureReload();

