#include "hdr/system/sys_main.h"
#include "hdr/libGL/soil/soil.h"
#include "hdr/io/io_fileSystem.h"
#include "hdr/io/io_textures.h"
#include <unordered_map>

typedef struct
{
	char    *memPointer;
	int     imageLength;
	string  textureName;
} _textureMemory;

vector<_textureMemory>  textureMemory;

typedef struct
{
	int textureID;
	bool loaded;
} _textureSet;

unordered_map <string, _textureSet> textureSet;

//-----------------------------------------------------------------------------------------------------
//
// Add an entry to the textureMemory list
long io_addTextureInfo(char *memPointer, const int imageLength, const string fileName)
//-----------------------------------------------------------------------------------------------------
{
	_textureMemory  tempMemory;
	long           index;

	tempMemory.memPointer = memPointer;
	tempMemory.textureName = fileName;
	tempMemory.imageLength = imageLength;

	if ( SDL_LockMutex (gameMutex) == 0 )   // Lock it in case main thread is reading it
	{
		textureMemory.push_back(tempMemory);
		index = textureMemory.size() -1;
		SDL_UnlockMutex (gameMutex);
		return index;
	}
	return -1;  // TODO: check if this happens if mutex fails
}

//-----------------------------------------------------------------------------------------------------
//
// Handle a texture file error event
void io_handleTextureFileError(const int errorCode, const string fileName)
//-----------------------------------------------------------------------------------------------------
{
	string errorMessage;

	switch (errorCode)
	{
		case TEXTURE_LOAD_ERROR_NOT_FOUND:
			con_print(CON_ERROR, true, "Could not find texture file [ %s ]",fileName.c_str());
			io_setTextureError (fileName);
			break;

		case TEXTURE_LOAD_ERROR_SOIL:
			con_print(CON_ERROR, true, "Could not load texture from memory [ %s ]", fileName.c_str ());
			io_setTextureError (fileName);
			break;

		case TEXTURE_LOAD_MEMORY_ERROR:
			con_print(CON_ERROR, true, "Memory error loading texture [ %s ]", fileName.c_str ());
			io_setTextureError (fileName);
			break;

		case TEXTURE_LOAD_MALLOC_ERROR:
			con_print (CON_ERROR, true, "MALLOC error loading texture [ %s ]", fileName.c_str ());
			io_setTextureError (fileName);
			break;
		default:
			break;
	}
}

//-----------------------------------------------------------------------------------------------------
//
// Load a texture from a file
//
// Is called from the GAME thread
// Puts the image information into a queue of memory pointers
void io_loadTextureFile(const string fileName)
//-----------------------------------------------------------------------------------------------------
{
	char        *imageBuffer = nullptr;
	int         imageLength;
	long       textureMemoryIndex;

	con_print(CON_INFO, true, "Step 1 - load texture file [ %s ]", fileName.c_str());

	imageLength = (int)io_getFileSize (fileName.c_str());

	if ( imageLength < 0 )
	{
		evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_ERROR_NOT_FOUND, 0, 0, vec2 (), vec2 (), fileName);
		return;
	}

	con_print (CON_INFO, true, "Image size [ %i ]", imageLength);

	imageBuffer = (char *) malloc (sizeof (char) * imageLength);

	if ( nullptr == imageBuffer )
	{
		evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_MALLOC_ERROR, 0, 0, vec2 (), vec2 (), fileName);
		return;
	}

	if ( -1 == io_getFileIntoMemory (fileName.c_str(), imageBuffer))
	{
		evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_MEMORY_ERROR, 0, 0, vec2 (), vec2 (), fileName);
//		if ( nullptr != imageBuffer )
		{
			free(imageBuffer);
			imageBuffer = nullptr;
		}
		return;
	}

	con_print (CON_INFO, true, "File is loaded into memory [ %i ]", imageBuffer);

	textureMemoryIndex = io_addTextureInfo ( imageBuffer, imageLength, fileName );

	con_print (CON_INFO, true, "Index into array for [ %s ] is [ %i ]", fileName.c_str (), textureMemoryIndex);

	con_print (CON_INFO, true, "Send event to main thread to upload into OpenGL.");

	evt_sendSDLEvent (EVENT_TYPE_DO_TEXTURE_UPLOAD, (int)textureMemoryIndex, 0 );
}

//-----------------------------------------------------------------------------------------------------
//
// Called from main thread on user event
// Upload texture in memory information to OpenGL texture
void io_uploadTextureIntoGL(intptr_t textureMemoryIndex)
//-----------------------------------------------------------------------------------------------------
{
	uint textureID;

//	con_print (CON_INFO, true, "Texture index [ %z ] - name [ %s ]", textureMemoryIndex, textureMemory[(size_t)textureMemoryIndex].textureName.c_str ());

	textureID = SOIL_load_OGL_texture_from_memory ((const unsigned char *)textureMemory[(size_t)textureMemoryIndex].memPointer, textureMemory[(size_t)textureMemoryIndex].imageLength, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_INVERT_Y);
	if ( 0 == textureID ) // failed to load texture
	{
		evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_ERROR_SOIL, 0, 0, vec2 (), vec2 (), textureMemory[(size_t)textureMemoryIndex].textureName);
		return;
	}

	con_print (CON_INFO, true, "File is now loaded into OpenGL.");

	con_print (CON_INFO, true, "Send USER EVENT_UPLOAD_DONE.");

	evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_UPLOAD_DONE, textureID, 0, 0, vec2 (), vec2 (), textureMemory[(size_t)textureMemoryIndex].textureName);

	free(textureMemory[(size_t)textureMemoryIndex].memPointer);
}

//-----------------------------------------------------------------------------------------------------
//
// Pass in fileName to find
// Return textureID or -1 if not loaded
//
// Return the textureID for a texture name
int io_getTextureID(const string fileName)
//-----------------------------------------------------------------------------------------------------
{
	unordered_map<string, _textureSet>::const_iterator textureItr;

	textureItr = textureSet.find (fileName);
	if ( textureItr != textureSet.end ())    // Found
		return textureItr->second.loaded ? textureItr->second.textureID : -1;   // Not loaded

	return 1;   // Not found
}

//-----------------------------------------------------------------------------------------------------
//
// Store the new TextureID and fileName into the lookup map
void io_storeTextureInfoIntoMap(int textureID, string fileName)
//-----------------------------------------------------------------------------------------------------
{
	_textureSet tempSet;

	tempSet.textureID = textureID;
	tempSet.loaded = true;
	textureSet.insert (std::pair<string, _textureSet> (fileName, tempSet));

	con_print (CON_INFO, true, "Texture [ %s ] with ID [ %i ] now stored in lookup map", fileName.c_str (), textureID);

	// TODO - free memory from vector ??
}


//-----------------------------------------------------------------------------------------------------
//
// Set the texture slot in the map as an error
void io_setTextureError ( const string fileName )
//-----------------------------------------------------------------------------------------------------
{
	_textureSet tempSet;

	tempSet.textureID = -1;
	tempSet.loaded = false;
	textureSet.insert (std::pair<string, _textureSet> (fileName, tempSet));
}