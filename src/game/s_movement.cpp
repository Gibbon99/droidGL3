#include "hdr/game/s_render.h"
#include "hdr/game/s_movement.h"

//---------------------------------------------------------------------------
//
// Process movement for player
void gam_processMovement ( float interpolate )
//---------------------------------------------------------------------------
{
	viewPixelX = pixelX + (currentVelocity.x * interpolate);
	viewPixelY = pixelY + (currentVelocity.y * interpolate);
};
