#include <hdr/game/gam_player.h>
#include "hdr/gui/gui_main.h"
#include "hdr/game/gam_levels.h"
#include "hdr/game/gui_deckView.h"

bool            tileBitmapLoaded = false;
SDL_Surface     *deckBitmap = nullptr;

//-----------------------------------------------------------------------------
//
// Draw the current level onto the entire screen
bool gui_drawDeckView ()
//-----------------------------------------------------------------------------
{
	int             countX, countY, whichTile;
	SDL_Rect        worldPosDraw;
	SDL_Rect        srcTexture;
	cpVect          textureSize;
	cpVect          drawScreenOffset;

	string          tileFileName;

 	PHYSFS_sint64   fileLength = 0;
	char            *fileSource = nullptr;

	tileFileName = g_tileType + "_" + g_tileColor + "_128.bmp";     // small version of the bitmap

	if (!tileBitmapLoaded)
	{
		fileLength = io_getFileSize ((char *) tileFileName.c_str ());
		if ( fileLength == -1 )
			return false;

		fileSource = (char *) malloc (sizeof (char) * (fileLength + 1));
		if ( nullptr == fileSource )
			return false;

		if ( -1 == io_getFileIntoMemory ((char *) tileFileName.c_str (), fileSource))
		{
			free(fileSource);
			return false;
		}

		SDL_RWops *tileFileHandle = SDL_RWFromMem( fileSource, static_cast<int>(fileLength));
		if (nullptr == tileFileHandle)
		{
			free(fileSource);
			return false;
		}

        deckBitmap = SDL_LoadBMP_RW(tileFileHandle, 1);
		if (nullptr == deckBitmap)
		{
			free(fileSource);
			return false;
		}
		tileBitmapLoaded = true;
    }

	textureSize.x = deckBitmap->w / (TILE_SIZE / 2);
	textureSize.y = deckBitmap->h / (TILE_SIZE / 2);

	drawScreenOffset.x = (winWidth - (levelInfo.at(lvl_getCurrentLevelName ()).levelDimensions.x * (TILE_SIZE / 2))) / 2;
	drawScreenOffset.y = (winHeight - (levelInfo.at(lvl_getCurrentLevelName ()).levelDimensions.y * (TILE_SIZE / 2))) / 2;

	for (countY = 0; countY != levelInfo.at(lvl_getCurrentLevelName ()).levelDimensions.y; countY++)
	{
		for (countX = 0; countX != levelInfo.at(lvl_getCurrentLevelName ()).levelDimensions.x; countX++)
		{
			whichTile = levelInfo.at(lvl_getCurrentLevelName ()).tiles[(countY * levelInfo.at(lvl_getCurrentLevelName ()).levelDimensions.x) + countX];

			if (0 != whichTile)
			{
				worldPosDraw.x = static_cast<int>(countX * (TILE_SIZE / 2) + drawScreenOffset.x);
				worldPosDraw.y = static_cast<int>(countY * (TILE_SIZE / 2) + drawScreenOffset.y);

				srcTexture.x = (whichTile % (int) textureSize.x) * (TILE_SIZE / 2);
				srcTexture.y = (whichTile / (int) textureSize.y) * (TILE_SIZE / 2);
				srcTexture.w = (TILE_SIZE / 2);
				srcTexture.h = (TILE_SIZE / 2);

				SDL_BlitSurface(deckBitmap, &srcTexture, guiSurface, &worldPosDraw);
			}
		}
	}

//	sys_printStringExt(33.0f, winHeight - 65, "Deck View - %s", shipLevel[currentLevel].levelName);

//	term_drawIndicator(TILE_SIZE / 2);

return true;
}

//-----------------------------------------------------------------------------
//
// Show the player location on the deck view
void gui_showPlayerLocation()
//-----------------------------------------------------------------------------
{
	float           drawX, drawY;
	glm::vec2       textPosition;
	glm::vec2       linePosition;
	int             fontLineWidth, fontLineHeight;
	glm::vec2       drawScreenOffset;
	int             indicatorSize;

	indicatorSize = 8;

	drawScreenOffset.x = (winWidth - (levelInfo.at(lvl_getCurrentLevelName ()).levelDimensions.x * (TILE_SIZE / 2))) / 2;
	drawScreenOffset.y = (winHeight - (levelInfo.at(lvl_getCurrentLevelName ()).levelDimensions.y * (TILE_SIZE / 2))) / 2;

	drawX = (playerDroid.middlePosition.x / TILE_SIZE)  * (TILE_SIZE / 2);
	drawY = (playerDroid.middlePosition.y / TILE_SIZE)  * (TILE_SIZE / 2);

	drawX += drawScreenOffset.x;
	drawY += drawScreenOffset.y;

	filledCircleRGBA ( renderer, drawX, drawY - (indicatorSize / 2), indicatorSize, 255,255,255,255);

	textPosition = io_getTextureSize ("hud");
	if (textPosition.y > 0)
	{
		TTF_SizeText(ttfFonts[gui_getFontIndex(guiFontName)].ttfFont, "Current Location", &fontLineWidth, &fontLineHeight);

		textPosition.x = (winWidth - fontLineWidth) * 0.5f;
		textPosition.y += fontLineHeight;

		linePosition.x = textPosition.x;
		linePosition.y = textPosition.y + fontLineHeight + 5;

		gui_renderText ( guiFontName, glm::vec2{textPosition.x, textPosition.y}, glm::vec3{255, 255, 255}, guiSurface, "Current Location");
		//
		// Draw line under the text
		thickLineRGBA (renderer, linePosition.x, linePosition.y, linePosition.x + fontLineWidth, linePosition.y, 4, 255,255,255,255);
		//
		// Draw line from middle of underline to location
		thickLineRGBA (renderer, linePosition.x + (fontLineWidth / 2), linePosition.y, drawX, drawY - (indicatorSize / 2), 2, 255,255,255,255);
	}
}
