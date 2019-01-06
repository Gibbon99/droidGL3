#include <hdr/game/gam_levels.h>
#include <hdr/io/io_logfile.h>
#include "hdr/gui/gui_main.h"
#include "hdr/gui/gui_sideview.h"


_basicTunnel        tunnel[NUM_OF_TUNNELS];
_sideviewColors     sideviewColors[SIDEVIEW_NUM_COLORS];
_sideviewBasicLevel sideviewLevels[MAX_LEVELS];
int                 drawOffsetX = 0;

//-----------------------------------------------------------------------------
//
// Create a color into the sideview array
void gui_createSideViewColor(int index, int red, int green, int blue, int alpha)
//-----------------------------------------------------------------------------
{
	if ((index < 0) || (index > SIDEVIEW_NUM_COLORS))
		return;

	sideviewColors[index].color = gui_mapRGBA( static_cast<Uint8>(red), static_cast<Uint8>(green), static_cast<Uint8>(blue), static_cast<Uint8>(alpha));
}

//-----------------------------------------------------------------------------
//
// setup the way the tunnels are linked to the levels
void gui_setupTunnels()
//-----------------------------------------------------------------------------
{
	tunnel[0].top = 19;
	tunnel[0].bottom = 3;
	tunnel[0].decks[0] = 3;
	tunnel[0].decks[1] = 4;
	tunnel[0].decks[2] = 15;
	tunnel[0].decks[3] = 16;
	tunnel[0].decks[4] = 19;
	tunnel[0].current_deck = 0;
	tunnel[0].current = tunnel[0].decks[tunnel[0].current_deck];

	tunnel[1].top = 6;
	tunnel[1].bottom = 0;
	tunnel[1].decks[0] = 0;
	tunnel[1].decks[1] = 5;
	tunnel[1].decks[2] = 6;
	tunnel[1].current_deck = 0;
	tunnel[1].current = tunnel[1].decks[tunnel[1].current_deck];

	tunnel[2].top = 17;
	tunnel[2].bottom = 15;
	tunnel[2].decks[0] = 15;
	tunnel[2].decks[1] = 17;
	tunnel[2].current_deck = 0;
	tunnel[2].current = tunnel[2].decks[tunnel[2].current_deck];

	tunnel[3].top = 20;
	tunnel[3].bottom = 12;
	tunnel[3].decks[0] = 12;
	tunnel[3].decks[1] = 13;
	tunnel[3].decks[2] = 14;
	tunnel[3].decks[3] = 17;
	tunnel[3].decks[4] = 20;
	tunnel[3].current_deck = 0;
	tunnel[3].current = tunnel[3].decks[tunnel[3].current_deck];

	tunnel[4].top = 12;
	tunnel[4].bottom = 1;
	tunnel[4].decks[0] = 1;
	tunnel[4].decks[1] = 5;
	tunnel[4].decks[2] = 6;
	tunnel[4].decks[3] = 12;
	tunnel[4].current_deck = 0;
	tunnel[4].current = tunnel[4].decks[tunnel[4].current_deck];

	tunnel[5].top = 9;
	tunnel[5].bottom = 2;
	tunnel[5].decks[0] = 2;
	tunnel[5].decks[1] = 8;
	tunnel[5].decks[2] = 9;
	tunnel[5].current_deck = 0;
	tunnel[5].current = tunnel[5].decks[tunnel[5].current_deck];

	tunnel[6].top = 14;
	tunnel[6].bottom = 9;
	tunnel[6].decks[0] = 9;
	tunnel[6].decks[1] = 10;
	tunnel[6].decks[2] = 11;
	tunnel[6].decks[3] = 12;
	tunnel[6].decks[4] = 13;
	tunnel[6].decks[5] = 14;
	tunnel[6].current_deck = 0;
	tunnel[6].current = tunnel[6].decks[tunnel[6].current_deck];

	tunnel[7].top = 18;
	tunnel[7].bottom = 14;
	tunnel[7].decks[0] = 14;
	tunnel[7].decks[1] = 18;
	tunnel[7].current_deck = 0;
	tunnel[7].current = tunnel[7].decks[tunnel[7].current_deck];
}

// ----------------------------------------------------------------------------
//
// load the sideview data from the external file
bool gui_loadSideViewData( const std::string &sideviewFileName )
// ----------------------------------------------------------------------------
{
	int             fileLength;
	char            *fileBuffer = nullptr;
	int             numberLevels;
	int             count;
	int             buf[1];
	unsigned char   levelCount[1];
	SDL_RWops      *fp;

	int             smallX, largeX;
	float           scale = 1.3f;

	smallX = 1000;
	largeX = 0;

	fileLength = (int)io_getFileSize (sideviewFileName.c_str());
	if ( fileLength < 0 )
	{
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_LEVEL_ERROR, SIDEVIEW_FILE_NOT_FOUND, 0, 0, vec2 (), vec2 (), sideviewFileName);
		return false;
	}

	fileBuffer = (char *) malloc (sizeof (char) * fileLength);
	if ( nullptr == fileBuffer )
	{
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_LEVEL_ERROR, SIDEVIEW_MALLOC_ERROR, 0, 0, vec2 (), vec2 (), sideviewFileName);
		return false;
	}

	if ( -1 == io_getFileIntoMemory (sideviewFileName.c_str(), fileBuffer))
	{
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_LEVEL_ERROR, SIDEVIEW_LOAD_MEMORY_ERROR, 0, 0, vec2 (), vec2 (), sideviewFileName);
		free(fileBuffer);
		fileBuffer = nullptr;
		return false;
	}

	fp = SDL_RWFromMem(fileBuffer, fileLength);
	//
	// Get a handle to the memory file
	if (nullptr == fp)
	{
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_LEVEL_ERROR, SIDEVIEW_FILE_NOT_FOUND, 0, 0, vec2 (), vec2 (), sideviewFileName );
		return false;
	}
	//
	// Check this version is ok to use
	//
	size_t returnCode = SDL_RWread ( fp, &levelCount, sizeof(levelCount), 1);
	if ( 0 == returnCode )
	{
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_LEVEL_ERROR,  SIDEVIEW_LOAD_ERROR_FILESYSTEM, 0, 0, vec2 (), vec2 (), sideviewFileName );
		SDL_RWclose (fp);
		return false;
	}

	numberLevels = levelCount[0];

	if (MAX_LEVELS != numberLevels)
	{
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_LEVEL_ERROR,  SIDEVIEW_LOAD_ERROR_FILESYSTEM, 0, 0, vec2 (), vec2 (), sideviewFileName );
		SDL_RWclose (fp);
		return false;
	}

	for (count = 0; count != numberLevels; count++)
	{
		SDL_RWread ( fp, (void *)&buf, sizeof(sideviewLevels[count].x1), 1);
		sideviewLevels[count].x1 = buf[0];
		sideviewLevels[count].x1 *= scale;

		if (sideviewLevels[count].x1 < smallX)
			smallX = sideviewLevels[count].x1;

		SDL_RWread ( fp, (void *)&buf, sizeof(sideviewLevels[count].y1), 1);
		sideviewLevels[count].y1 = buf[0] - 100;
		sideviewLevels[count].y1 *= scale;

		SDL_RWread ( fp, (void *)&buf, sizeof(sideviewLevels[count].x2), 1);
		sideviewLevels[count].x2 = buf[0];
		sideviewLevels[count].x2 *= scale;

		if (sideviewLevels[count].x2 > largeX)
			largeX = sideviewLevels[count].x2;

		SDL_RWread ( fp, (void *)&buf, sizeof(sideviewLevels[count].y2), 1);
		sideviewLevels[count].y2 = buf[0] - 100;
		sideviewLevels[count].y2 *= scale;
	}
	SDL_RWclose (fp);

	free(fileBuffer);

	gui_setupTunnels();

	drawOffsetX = (winWidth - (largeX - smallX)) / 2;

	return true;
}

// ----------------------------------------------------------------------------
//
// Wrap drawing a rectangle into our own function
void gui_sideviewDrawRect(int x1, int y1, int x2, int y2, SDL_Color whichColor)
// ----------------------------------------------------------------------------
{
	int height = y2 - y1;

	//
	// Swap starting Y positions
	y2 = y2 - height;
	y1 = y1 + height;

	roundedBoxRGBA(renderer, x1 - drawOffsetX, y1, x2 - drawOffsetX, y2, 4, whichColor.r, whichColor.g, whichColor.b, whichColor.a);

	roundedRectangleRGBA(renderer, x1 - drawOffsetX, y1, x2 - drawOffsetX, y2, 4, 0, 0, 0, whichColor.a);
}

// ----------------------------------------------------------------------------
//
// Show the ship in its sideview on the screen
void gui_drawSideView()
// ----------------------------------------------------------------------------
{
	int             count;
	int             lifts = 8;
	int             toLifts = 0;
	int             x1;
	int             y1;
	SDL_Color       tempAlert;
	glm::vec2       textPosition;

	if (currentDeckNumber < 0)
	{
		currentDeckNumber = lvl_getDeckNumber (lvl_getCurrentLevelName ());
	}

	count = 0;
	x1 = sideviewLevels[count].x2;
	y1 = sideviewLevels[count].y2;
	gui_sideviewDrawRect(x1, y1, sideviewLevels[count].x1, sideviewLevels[count].y1, sideviewColors[SIDEVIEW_SHIP_COLOR].color);
	toLifts++;

	for (count = 1; count != MAX_LEVELS - lifts; count++)
	{
		x1 = sideviewLevels[count].x1;
		y1 = sideviewLevels[count].y1;
		gui_sideviewDrawRect(x1, y1, sideviewLevels[count].x2, sideviewLevels[count].y2, sideviewColors[SIDEVIEW_SHIP_COLOR].color);
		toLifts++;
	}

	if (currentMode == MODE_LIFT_VIEW)
	{
		//currentLevel = tunnel[currentTunnel].current;
		// highlite current level
		if (0 == currentDeckNumber)
			gui_sideviewDrawRect(sideviewLevels[currentDeckNumber].x2, sideviewLevels[currentDeckNumber].y2, sideviewLevels[currentDeckNumber].x1, sideviewLevels[currentDeckNumber].y1, sideviewColors[SIDEVIEW_ACTIVE_DECK_COLOR].color);
		else
			gui_sideviewDrawRect(sideviewLevels[currentDeckNumber].x1, sideviewLevels[currentDeckNumber].y1, sideviewLevels[currentDeckNumber].x2, sideviewLevels[currentDeckNumber].y2, sideviewColors[SIDEVIEW_ACTIVE_DECK_COLOR].color);
	}
	else
	{
		switch (currentAlertLevel)
		{
			case ALERT_GREEN_TILE:
				tempAlert = gui_mapRGBA (0, 255, 0, 255);
				break;

			case ALERT_YELLOW_TILE:
				tempAlert = gui_mapRGBA(255, 255, 0, 255);
				break;

			case ALERT_RED_TILE:
				tempAlert = gui_mapRGBA(255, 0, 0, 255);
				break;

			default:
				tempAlert = gui_mapRGBA(255, 255, 255, 255);
				break;
		}

		if (0 == currentDeckNumber)
			gui_sideviewDrawRect(sideviewLevels[currentDeckNumber].x2, sideviewLevels[currentDeckNumber].y2, sideviewLevels[currentDeckNumber].x1, sideviewLevels[currentDeckNumber].y1, tempAlert);
		else
			gui_sideviewDrawRect(sideviewLevels[currentDeckNumber].x1, sideviewLevels[currentDeckNumber].y1, sideviewLevels[currentDeckNumber].x2, sideviewLevels[currentDeckNumber].y2, tempAlert);
	}


	if ((currentTunnel != 3) && (currentTunnel != 6))
	{
		count = 13;
		gui_sideviewDrawRect(sideviewLevels[count].x1, sideviewLevels[count].y1, sideviewLevels[count].x2, sideviewLevels[count].y2, sideviewColors[SIDEVIEW_SHIP_COLOR].color);
	}
	else
	{
		// using tunnel connecting to level 13
		if (currentDeckNumber != 13)
		{
			count = 13;
			gui_sideviewDrawRect(sideviewLevels[count].x1, sideviewLevels[count].y1, sideviewLevels[count].x2, sideviewLevels[count].y2, sideviewColors[SIDEVIEW_SHIP_COLOR].color);
		}
	}

	// fill in engine part
	gui_sideviewDrawRect(sideviewLevels[7].x1, sideviewLevels[7].y1, sideviewLevels[7].x2, sideviewLevels[7].y2, sideviewColors[SIDEVIEW_ENGINE_COLOR].color);
	//
	// draw the lifts
	//
	for (count = 0; count != lifts; count++)
	{
		gui_sideviewDrawRect(sideviewLevels[count + toLifts].x1, sideviewLevels[count + toLifts].y1, sideviewLevels[count + toLifts].x2, sideviewLevels[count + toLifts].y2, sideviewColors[SIDEVIEW_LIFT_COLOR].color);
	}

	if (currentMode == MODE_LIFT_VIEW)
	{
		// highlight the current tunnel
		gui_sideviewDrawRect(sideviewLevels[21 + currentTunnel].x1, sideviewLevels[21 + currentTunnel].y1, sideviewLevels[21 + currentTunnel].x2, sideviewLevels[21 + currentTunnel].y2, sideviewColors[SIDEVIEW_ACTIVE_LIFT_COLOR].color);

		gui_sideviewDrawRect(20, winHeight - 85, 30, winHeight - 65, sideviewColors[SIDEVIEW_ACTIVE_LIFT_COLOR].color);
		gui_renderText( guiFontName, glm::vec2{33, winHeight - 95}, glm::vec3{sideviewColors[SIDEVIEW_ACTIVE_LIFT_COLOR].color.r, sideviewColors[SIDEVIEW_ACTIVE_LIFT_COLOR].color.g, sideviewColors[SIDEVIEW_ACTIVE_LIFT_COLOR].color.b}, guiSurface, "Active lift");

		gui_sideviewDrawRect(20, winHeight - 55, 30, winHeight - 35, sideviewColors[SIDEVIEW_ACTIVE_DECK_COLOR].color);
		gui_renderText( guiFontName, glm::vec2{33, winHeight - 65}, glm::vec3{sideviewColors[SIDEVIEW_ACTIVE_DECK_COLOR].color.r, sideviewColors[SIDEVIEW_ACTIVE_DECK_COLOR].color.g, sideviewColors[SIDEVIEW_ACTIVE_DECK_COLOR].color.b}, guiSurface, "Current Deck");

		gui_sideviewDrawRect(20, winHeight - 25, 30, winHeight - 5, sideviewColors[SIDEVIEW_LIFT_COLOR].color);
		gui_renderText( guiFontName, glm::vec2{33, winHeight - 35}, glm::vec3{sideviewColors[SIDEVIEW_LIFT_COLOR].color.r, sideviewColors[SIDEVIEW_LIFT_COLOR].color.g, sideviewColors[SIDEVIEW_LIFT_COLOR].color.b}, guiSurface, "Inactive Lift");
	}
	else
	{
		textPosition = io_getTextureSize ("hud");
		if (textPosition.y > 0)
		{
			// TODO: Put on HUD instead?
			gui_renderText ( guiFontName, glm::vec2{33, textPosition.y + 10}, glm::vec3{tempAlert.r, tempAlert.g, tempAlert.b}, guiSurface, "You are here - %s", lvl_getCurrentLevelName ().c_str ());
		}
	}
}
