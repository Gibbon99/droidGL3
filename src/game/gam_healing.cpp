#include "hdr/game/gam_healing.h"

//-----------------------------------------------------------------------------
//
// structure to hold information for healing tiles
//
//-----------------------------------------------------------------------------
vector<_basicHealing>        healing;
bool                        doHealingAnimate = true;
SDL_TimerID                 timerHealingAnimate;
Uint32                      healingAnimateInterval;      // From script

// ----------------------------------------------------------------------------
//
// Animate healing tiles called from timer callback
Uint32 gam_healingAnimateTimerCallback ( Uint32 interval, void *param )
// ----------------------------------------------------------------------------
{
	if (!doHealingAnimate)
		return interval;

	if ( healing.empty ())
		return interval;

	for (auto &healingItr: healing )
	{
		healingItr.currentFrame++;
		if ( healingItr.currentFrame > HEALING_TILE + 3)
			healingItr.currentFrame = HEALING_TILE;

		levelInfo.at (lvl_getCurrentLevelName ()).tiles[healingItr.pos] = healingItr.currentFrame;
	}

	return healingAnimateInterval;
}

// ----------------------------------------------------------------------------
//
// Set the state of the healing tile timer
void gam_setHealingState(bool newState)
// ----------------------------------------------------------------------------
{
	doHealingAnimate = newState;
}

// ----------------------------------------------------------------------------
//
// Initiate the timer to animate the healing tiles
//
// Pass in time in milliseconds
void gam_initHealingAnimateTimer(Uint32 interval)
// ----------------------------------------------------------------------------
{
	timerHealingAnimate = SDL_AddTimer (interval, gam_healingAnimateTimerCallback, nullptr);   // Time in milliseconds
	if (0 == timerHealingAnimate)
	{
		con_print(CON_ERROR, true, "Could not add healing tile animate timer : [ %s ]", SDL_GetError ());
	}
}

// ----------------------------------------------------------------------------
//
// Find out where on the level the healing tiles are
// Remember for this level for animation
void gam_findHealingTiles ( string levelName )
// ----------------------------------------------------------------------------
{
	int                 index;
	_basicHealing       tempHealing;

	CHECK_LEVEL_NAME

	//
	// Clear out from previous use
	if ( !healing.empty() )
		healing.clear();

	for (index = 0; index < levelInfo.at(levelName).levelDimensions.x * levelInfo.at(levelName).levelDimensions.y; index++)
	{
		switch ( levelInfo.at(levelName).tiles[index] )
		{
			case HEALING_TILE:
			case HEALING_TILE + 1:
			case HEALING_TILE + 2:
			case HEALING_TILE + 3:
				tempHealing.pos = index;
				tempHealing.currentFrame = HEALING_TILE;
				tempHealing.frameDelay = 0.0f;
				healing.push_back(tempHealing);
				break;

			default:
				break;
		}
	}
}