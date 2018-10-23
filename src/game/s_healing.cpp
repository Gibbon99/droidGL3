#include "hdr/game/s_healing.h"

//-----------------------------------------------------------------------------
//
// structure to hold information for healing tiles
//
//-----------------------------------------------------------------------------
vector<_basicHealing>        healing;

// ----------------------------------------------------------------------------
//
// Animate the healing tiles
void gam_animateHealingTiles()
// ----------------------------------------------------------------------------
{
	if (healing.empty())
		return;

	for ( auto &healingItr : healing )
	{
		healingItr.frameDelay += 1.0f / 3.0f;      // 10fps ( 30 / 3.0 )

		if ( healingItr.frameDelay > 1.0f)
			{
				healingItr.frameDelay = 0.0f;
				healingItr.currentFrame++;
			if ( healingItr.currentFrame > HEALING_TILE + 3)
				healingItr.currentFrame = HEALING_TILE;

			levelInfo.at(lvl_getCurrentLevelName ()).tiles[healingItr.pos] = healingItr.currentFrame;
			}
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