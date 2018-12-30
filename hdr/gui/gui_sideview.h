#pragma once

#include "hdr/gui/gui_main.h"

//-----------------------------------------------------------------------------
//
// sideview of ship stuff - used for lifts and database view
//
//-----------------------------------------------------------------------------

#define MAX_LEVELS		    29
#define NUM_OF_TUNNELS		8

extern int	currentTunnel;

//-----------------------------------------------------------------------------
//
// Structure for sideview of ship
//
//-----------------------------------------------------------------------------

struct _basicTunnel
{
	int		top;
	int		bottom;
	int		current;
	int		current_deck;
	int		decks[6];
};

extern _basicTunnel tunnel[NUM_OF_TUNNELS];

struct _sideviewBasicLevel
{
	int		x1;
	int		y1;
	int		x2;
	int		y2;
	int		up;
	int		down;
};

struct _sideviewColors
{
	SDL_Color   color;
};

extern _sideviewColors sideviewColors[SIDEVIEW_NUM_COLORS];

extern _sideviewBasicLevel sideviewLevels[MAX_LEVELS];

// Create a color into the sideview array
void gui_createSideViewColor(int index, int red, int green, int blue, int alpha);

// load the sideview data from the external file
bool gui_loadSideViewData( const std::string &sideviewFileName );

// Show the ship in its sideview on the screen
void gui_drawSideView();

// setup the way the tunnels are linked to the levels
void gui_setupTunnels();

// Get the tunnel being used by the lift the player sprite is over
void gam_getTunnelToUse();

// Put the player onto lift on new deck
void gam_putPlayerOnLiftFromTunnel();
