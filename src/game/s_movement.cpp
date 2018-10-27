#include "hdr/game/s_render.h"
#include "hdr/game/s_movement.h"

//---------------------------------------------------------------------------
//
// Process movement for player
void gam_processMovement ( float interpolate )
//---------------------------------------------------------------------------
{
	viewWorldLocationX = worldLocationX + (currentVelocity.x * interpolate);
	viewWorldLocationY = worldLocationY + (currentVelocity.y * interpolate);
};
