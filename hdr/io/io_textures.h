#pragma once

#define TEXTURE_LOAD_ERROR_NOT_FOUND    0x01
#define TEXTURE_LOAD_ERROR_SOIL         0x02
#define TEXTURE_LOAD_MEMORY_ERROR       0x03
#define TEXTURE_LOAD_MALLOC_ERROR       0x04

#include "hdr/system/sys_main.h"

// Handle a texture file error event
void io_handleTextureFileError ( const int errorCode, const string fileName );

// Called from main thread on user event
// Upload texture in memory information to OpenGL texture
void io_uploadTextureIntoGL ( intptr_t textureMemoryIndex );

// Return the textureID for a texture name
int io_getTextureID ( const string fileName );

// Store the new TextureID and fileName into the lookup map
void io_storeTextureInfoIntoMap ( int textureID, string fileName );

// Puts the image information into a queue of memory pointers
void io_loadTextureFile ( const string fileName );

// Set the texture slot in the map as an error
void io_setTextureError ( const string fileName );