#include <hdr/game/s_player.h>
#include "hdr/game/s_render.h"
#include "hdr/game/s_movement.h"

//---------------------------------------------------------------------------
//
// Process movement for player
void gam_processMovement ( float interpolate )
//---------------------------------------------------------------------------
{
  playerDroid.middlePosition.x = playerDroid.worldPos.x + 16;
  playerDroid.middlePosition.y = playerDroid.worldPos.y + 16;
};
