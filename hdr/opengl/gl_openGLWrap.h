#pragma once

#include "hdr/system/sys_main.h"

//----------------------------------------------------------------------------
//
// Wrap OpenGL calls to minmise state changes
//
//----------------------------------------------------------------------------

extern int wrapCountglBindTexture;
extern int wrapCountglBindTextureChanges;

//----------------------------------------------------------------------------
//
// Display openGLWrap counters
void wrapShowCounters ( bool writeToLog );

// Warap glEnable
void wrapglEnable ( int whichState );

// Wrap glDisable
void wrapglDisable ( int whichState );

// Wrap glBindTexture
void wrapglBindTexture ( int whichTextureUnit, int whichTexture );

// Wrap the call to generate a buffer ID
// so we can print out some text with it
uint wrapglGenBuffers(int numBuffers, string bufferText);

// Display all the buffer IDs generated and their text
void wrapShowBufferInfo();