#include <hdr/game/s_levels.h>
#include "hdr/game/s_player.h"

cpVect      playerWorldPosition;



//------------------------------------------------------------------------------
//
// Setup initial values for player
void s_initialPlayerSetup()
//------------------------------------------------------------------------------
{

}

//------------------------------------------------------------------------------
//
// Get the tile type the player is currently over
void s_getTileUnderPlayer(string levelName, float posX, float posY)
//------------------------------------------------------------------------------
{
	int whichTile;

	whichTile = levelInfo.at(levelName).tiles[((int)(posY + 17)* levelInfo.at(levelName).levelDimensions.x) + (int)posX];

	printf("Over tile [ %i ]\n", whichTile);
}
