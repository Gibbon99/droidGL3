#include "hdr/system/sys_main.h"
#include "hdr/libGL/soil/soil.h"
#include "hdr/io/io_fileSystem.h"
#include "hdr/io/io_textures.h"
#include <unordered_map>
#include <hdr/game/gam_render.h>

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
	int width;
	int height;
} _textureSet;

unordered_map <string, _textureSet> textureSet;

vector<int>         tileTextureSet;

unsigned int numTexturesToLoad = 0;

uint                 tileTextureID = 0;

//-----------------------------------------------------------------------------------------------------
//
// Check if all the textures are loaded or not
bool io_allTexturesLoaded()
//-----------------------------------------------------------------------------------------------------
{
	bool result = false;

	result = textureSet.size() == numTexturesToLoad;

	return result;
}

//-----------------------------------------------------------------------------------------------------
//
// Check if a texture has been loaded or not
bool io_isTextureLoaded(const string fileName)
//-----------------------------------------------------------------------------------------------------
{
	bool result;

	if ( SDL_LockMutex (textureSetMutex) == 0 )
	{
		result = textureSet.at(fileName).loaded;
		SDL_UnlockMutex (textureSetMutex);
		return result;
	}
	return false;
}

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

//	con_print(CON_INFO, true, "Step 1 - load texture file [ %s ]", fileName.c_str());

	imageLength = (int)io_getFileSize (fileName.c_str());

	if ( imageLength < 0 )
	{
		evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_ERROR_NOT_FOUND, 0, 0, vec2 (), vec2 (), fileName);
		return;
	}

//	con_print (CON_INFO, true, "Image size [ %i ]", imageLength);

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

//	con_print (CON_INFO, true, "File is loaded into memory [ %i ]", imageBuffer);

	textureMemoryIndex = io_addTextureInfo ( imageBuffer, imageLength, fileName );

//	con_print (CON_INFO, true, "Index into array for [ %s ] is [ %i ]", fileName.c_str (), textureMemoryIndex);

//	con_print (CON_INFO, true, "Send event to main thread to upload into OpenGL.");

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
	int imageWidth, imageHeight, numChannels;

	//
	// TODO: Does this leak memory?
	if ( nullptr == SOIL_load_image_from_memory	( (const unsigned char *)textureMemory[(size_t)textureMemoryIndex].memPointer,textureMemory[(size_t)textureMemoryIndex].imageLength,
					&imageWidth, &imageHeight, &numChannels, SOIL_LOAD_AUTO))
	{
		evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_ERROR_SOIL, 0, 0, vec2 (), vec2 (), textureMemory[(size_t)textureMemoryIndex].textureName);
	}

	textureID = SOIL_load_OGL_texture_from_memory ((const unsigned char *)textureMemory[(size_t)textureMemoryIndex].memPointer, textureMemory[(size_t)textureMemoryIndex].imageLength,
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y); //SOIL_FLAG_TEXTURE_REPEATS); SOIL_FLAG_MIPMAPS

	if ( 0 == textureID ) // failed to load texture
	{
		evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_ERROR_SOIL, 0, 0, vec2 (), vec2 (), textureMemory[(size_t)textureMemoryIndex].textureName);
		return;
	}

	evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_UPLOAD_DONE, textureID, 0, 0, vec2{imageWidth, imageHeight} , vec2 (), textureMemory[(size_t)textureMemoryIndex].textureName);

	free(textureMemory[(size_t)textureMemoryIndex].memPointer);
}

//-----------------------------------------------------------------------------------------------------
//
// Pass in the fileName to find
// Return texture dimensions in vec2, or -1 in each if not found
//
// Return the image size for a texture name
vec2 io_getTextureSize(const string fileName)
//-----------------------------------------------------------------------------------------------------
{
	vec2        returnResults;

	unordered_map<string, _textureSet>::const_iterator textureItr;

	textureItr = textureSet.find (fileName);
	if ( textureItr != textureSet.end ())    // Found
	{
		returnResults.x = textureItr->second.width;
		returnResults.y = textureItr->second.height;
		return returnResults;
	}

	return vec2{-1,-1};
}

//-----------------------------------------------------------------------------------------------------
//
// Pass in fileName to find
// Return textureID or -1 if not loaded
//
// Return the textureID for a texture name
// Comparision is CASE SENSITIVE
GLuint io_getTextureID(const string fileName)
//-----------------------------------------------------------------------------------------------------
{
	unordered_map<string, _textureSet>::const_iterator textureItr;

	textureItr = textureSet.find (fileName);
	if ( textureItr != textureSet.end ())    // Found
	{
		return textureItr->second.loaded ? textureItr->second.textureID : -1;   // Not loaded
	}

	return 0;   // Not found
}

//-----------------------------------------------------------------------------------------------------
//
// Store the new TextureID and fileName into the lookup map
//
// Strip off any file extension before storing the filename as the key
void io_storeTextureInfoIntoMap(int textureID, vec2 imageSize, string fileName, bool checkMutex)
//-----------------------------------------------------------------------------------------------------
{
	_textureSet     tempSet;
	size_t          position;
	string          extractedName;

	tempSet.textureID = textureID;
	tempSet.loaded = true;
	tempSet.width = static_cast<int>(imageSize.x);
	tempSet.height = static_cast<int>(imageSize.y);
	//
	// Strip any file extension off
	position = fileName.find(".");
	extractedName = (string::npos == position)? fileName : fileName.substr(0, position);

	if (checkMutex)
	{
		if ( SDL_LockMutex (textureSetMutex) == 0 )
		{
			textureSet.insert (std::pair<string, _textureSet> (extractedName, tempSet));
			SDL_UnlockMutex (gameMutex);
		}
	}
	else
		{
			textureSet.insert (std::pair<string, _textureSet> (extractedName, tempSet));
		}
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

//-----------------------------------------------------------------------------------------------------
//
// Load the tile spriteSheet from a file and break it up into a OpenGL texture for each tile
// Put each tile into a larger texture with a 1 pixel border on left and right to stop bleeding from
// linear filtering
//
// Is called from the MAIN thread
// Uses a single texture Atlas, puts coords for each tile into vector array for X position
//
void io_loadTileTextureFile(const string fileName)
//-----------------------------------------------------------------------------------------------------
{
	char                    *imageBuffer = nullptr;
	int                     imageLength;
	SDL_Surface             *spriteSheetSurface;
	SDL_RWops               *filePointerMem;

//	con_print(CON_INFO, true, "Step 1 - load texture file [ %s ]", fileName.c_str());

	imageLength = (int)io_getFileSize (fileName.c_str());

	if ( imageLength < 0 )
	{
		evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_ERROR_NOT_FOUND, 0, 0, vec2 (), vec2 (), fileName);
		return;
	}

//	con_print (CON_INFO, true, "Image size [ %i ]", imageLength);

	imageBuffer = (char *) malloc (sizeof (char) * imageLength);

	if ( nullptr == imageBuffer )
	{
		evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_MALLOC_ERROR, 0, 0, vec2 (), vec2 (), fileName);
		return;
	}

	if ( -1 == io_getFileIntoMemory (fileName.c_str(), imageBuffer))
	{
		evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_MEMORY_ERROR, 0, 0, vec2 (), vec2 (), fileName);
		free(imageBuffer);
		imageBuffer = nullptr;
		return;
	}

	filePointerMem = SDL_RWFromMem(imageBuffer, imageLength);
	if (nullptr == filePointerMem)
	{
		con_print(CON_ERROR, true, "Error getting SDL RWops from bitmap memory file.");
		evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_ERROR_SOIL, 0, 0, vec2 (), vec2 (), fileName);
	}

	spriteSheetSurface = SDL_LoadBMP_RW (filePointerMem, 1);     // free stream
	if (nullptr == spriteSheetSurface)
	{
		evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_ERROR_SOIL, 0, 0, vec2 (), vec2 (), fileName);
		return;
	}

	//
	// Now get each tile and create a border around the left and right of the tile
	//
	// Then blit to new large tile image
	//
	SDL_Surface         *singleTile;
	SDL_Rect            srcRect;
	SDL_Rect            destRect;
	SDL_Surface         *allTiles;

#define NUM_BITS 32

	Uint32 rmask, gmask, bmask, amask;

	// SDL interprets each pixel as a 32-bit number, so our masks must depend  on the endianness (byte order) of the machine
	//
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif



	allTiles = SDL_CreateRGBSurface(0, 34*64, TILE_SIZE, NUM_BITS, rmask, gmask, bmask, amask);
	if (nullptr == allTiles)
	{
		con_print(CON_ERROR, true, "Error creating allTiles surface [ %s ]", __func__);
		return;
	}

	singleTile = SDL_CreateRGBSurface(0, TILE_SIZE, TILE_SIZE, NUM_BITS, rmask, gmask, bmask, amask);
	if (nullptr == singleTile)
	{
		con_print(CON_ERROR, true, "Error creating singleTile surface [ %s ]", __func__);
		return;
	}

	int indexCount = 0;

	for (int i= 0; i != 64; i++)
	{
		//
		// Single Tile
		//
		srcRect.x = (i % 8) * TILE_SIZE;
		srcRect.y = (i / 8) * TILE_SIZE;
		srcRect.w = TILE_SIZE;
		srcRect.h = TILE_SIZE;

		destRect.x = 0;
		destRect.y = 0;
		destRect.w = TILE_SIZE;
		destRect.h = TILE_SIZE;

		if (SDL_BlitSurface(spriteSheetSurface, &srcRect, singleTile, &destRect) < 0)
		{
			con_print(CON_ERROR, true, "Blit tile surface failed [ %s ]", SDL_GetError ());
			return;
		}
		//
		// LEFT Edge
		//
		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = 1;
		srcRect.h = TILE_SIZE;

		destRect.x = (i * singleTile->w) + indexCount;
		destRect.y = 0;
		destRect.w = 1;
		destRect.h = TILE_SIZE;

		if (SDL_BlitSurface(singleTile, &srcRect, allTiles, &destRect) < 0)
		{
			con_print(CON_ERROR, true, "Blit tile surface failed [ %s ]", SDL_GetError ());
			return;
		}

		indexCount++;

		//
		// Tile
		//
		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = TILE_SIZE;
		srcRect.h = TILE_SIZE;

		destRect.x = (i * singleTile->w) + indexCount;
		destRect.y = 0;
		destRect.w = TILE_SIZE;
		destRect.h = TILE_SIZE;

		gam_setSingleTileCoords(destRect.x , allTiles->w);

		if (SDL_BlitSurface(singleTile, &srcRect, allTiles, &destRect) < 0)
		{
			con_print(CON_ERROR, true, "Blit tile surface failed [ %s ]", SDL_GetError ());
			return;
		}

		//
		// RIGHT edge
		//
		indexCount++;

		srcRect.x = singleTile->w - 1;
		srcRect.y = 0;
		srcRect.w = TILE_SIZE;
		srcRect.h = TILE_SIZE;

		destRect.x = (i * singleTile->w) + ((singleTile->w - 1) + indexCount);
		destRect.y = 0;
		destRect.w = TILE_SIZE;
		destRect.h = TILE_SIZE;

		if (SDL_BlitSurface(singleTile, &srcRect, allTiles, &destRect) < 0)
		{
			con_print(CON_ERROR, true, "Blit tile surface failed [ %s ]", SDL_GetError ());
			return;
		}
	}

	//
	// Upload SDL_Surface into a OpenGL texture
	//
	SDL_LockSurface (allTiles);

//	tileTextureID = SOIL_load_OGL_texture_from_memory ((const unsigned char *)allTiles->pixels, allTiles->h * allTiles->pitch, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y | SOIL_FLAG_MIPMAPS); //SOIL_FLAG_TEXTURE_REPEATS);


	glGenTextures( 1, &tileTextureID );
	glBindTexture( GL_TEXTURE_2D, tileTextureID );

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum Mode = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, Mode, allTiles->w, allTiles->h, 0, Mode, GL_UNSIGNED_BYTE, allTiles->pixels);

	SDL_UnlockSurface (allTiles);

	SDL_SaveBMP(allTiles, "tilesTest.bmp");

	if (0 == tileTextureID)
	{
		con_print(CON_ERROR, true, "Soil load error [ %s ]", SOIL_last_result ());
		evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_ERROR, TEXTURE_LOAD_ERROR_SOIL, 0, 0, vec2 (), vec2 (), fileName);
		free(imageBuffer);
		return;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

//	SDL_SaveBMP(allTiles, "tilesTest.bmp");

	SDL_FreeSurface (spriteSheetSurface);

	free(imageBuffer);
}