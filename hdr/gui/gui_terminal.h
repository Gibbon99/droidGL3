#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/io/io_fileSystem.h"
#include "hdr/opengl/gl_renderSprite.h"

extern Uint32           dbAnimateSpeed;     // From Script

// Change to terminal mode
void gui_changeToTerminalMode (std::string textureName);

// Exit terminal mode - clean up sprite
void gui_exitTerminalMode ();

// Render the terminal screen
void gui_renderTerminal ();

// Get the initial index into the droidType name
int gui_get3DTextureIndex (std::string droidType);

// Turn on / off the animation for the droid in the database view
void gui_setDatabaseAnimateState(bool newState);

void gam_initDataBaseAnimateTimer ( Uint32 interval );