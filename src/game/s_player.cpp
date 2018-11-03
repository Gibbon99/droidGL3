#include <hdr/game/s_levels.h>
#include <hdr/opengl/gl_renderSprite.h>
#include <hdr/game/s_render.h>
#include "hdr/game/s_player.h"

cpVect      playerWorldPosition;
_droid      playerDroid;

//------------------------------------------------------------------------------
//
// Process the player world position and movement
void s_processPlayerMovement()
//------------------------------------------------------------------------------
{

}

//------------------------------------------------------------------------------
//
// Render the player sprite
void s_renderPlayerSprite()
//------------------------------------------------------------------------------
{
//	gl_renderSprite ("001", glm::vec2{playerDroid.worldPos.x, playerDroid.worldPos.y}, 0, glm::vec3{1.0, 1.0, 1.0} );

    gl_renderSprite ("001", glm::vec2{winWidth / 2, winHeight / 2}, 0, glm::vec3{1.0, 1.0, 1.0} );
}

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

	whichTile = levelInfo.at(levelName).tiles[((int)(posY) * levelInfo.at(levelName).levelDimensions.x) + (int)posX];

	printf("posX [ %i [ PosY [ %i ] Over tile [ %i ]\n", (int)posX, (int)posY, whichTile);
}
