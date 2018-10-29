#pragma once

#include "hdr/system/sys_main.h"

#define TEXTURE_LOAD_ERROR_NOT_FOUND    0x01
#define TEXTURE_LOAD_ERROR_SOIL         0x02
#define TEXTURE_LOAD_MEMORY_ERROR       0x03
#define TEXTURE_LOAD_MALLOC_ERROR       0x04

extern unsigned int         numTexturesToLoad;
extern uint                 tileTextureID;

// Handle a texture file error event
void io_handleTextureFileError ( int errorCode, std::string fileName );

// Called from main thread on user event
// Upload texture in memory information to OpenGL texture
void io_uploadTextureIntoGL ( intptr_t textureMemoryIndex );

// Return the textureID for a texture name
GLuint io_getTextureID ( std::string fileName );

// Return the image size for a texture name
glm::vec2 io_getTextureSize( std::string fileName);

// Store the new TextureID and fileName into the lookup map
void io_storeTextureInfoIntoMap(int textureID, glm::vec2 imageSize, std::string fileName, bool checkMutex);

// Puts the image information into a queue of memory pointers
void io_loadTextureFile ( std::string fileName );

// Set the texture slot in the map as an error
void io_setTextureError ( std::string fileName );

// Check if all the textures are loaded or not
bool io_allTexturesLoaded();

void io_loadTileTextureFile( std::string fileName);
