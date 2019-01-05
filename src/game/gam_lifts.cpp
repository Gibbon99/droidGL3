#include "hdr/gui/gui_sideview.h"
#include "hdr/game/gam_player.h"
#include <hdr/system/sys_audio.h>
#include "hdr/game/gam_levels.h"
#include "hdr/game/gam_lifts.h"

int     currentTunnel = 0;
int     currentDeckNumber = -1;

//------------------------------------------------------------------------------
//
// Locate the world position of the passed in lift
cpVect gam_getLiftworldPosition ( string levelName, int whichLift )
//------------------------------------------------------------------------------
{
	cpVect liftPosition;

	liftPosition.x = levelInfo.at ( levelName ).lifts[whichLift].posX;
	liftPosition.y = levelInfo.at ( levelName ).lifts[whichLift].posY;

	return liftPosition;
}

// ----------------------------------------------------------------------------
//
// Get the current deck we are on and are moving around on
void gam_getCurrentDeck ( int checkLevel )
// ----------------------------------------------------------------------------
{
	int i;
	int topDeck = tunnel[currentTunnel].top;

	//
	// Look for the matching deck and current level
	//
	for ( i = 0; i != 10; i++ ) // TODO - why 10?
	{
		if ( checkLevel == tunnel[currentTunnel].decks[i] )
		{
			//
			// Found a match on this tunnel
			//
			tunnel[currentTunnel].current_deck = i;
			return;
		}

		if ( topDeck == tunnel[currentTunnel].decks[i] )
		{
			//
			// we have a problem - didn't find a matching level for this tunnel
			con_print ( CON_ERROR, true, "Error: Couldn't find a matching level for tunnel. Game in unstable state" );
		}
	}
}

// ----------------------------------------------------------------------------
//
// Get the tunnel being used by the lift the player sprite is over
void gam_getTunnelToUse ()
// ----------------------------------------------------------------------------
{
	auto levelIndexItr = lvl_getLevelIndex ( lvl_getCurrentLevelName ());

	switch ( levelIndexItr->second.numLifts )
	{
		case 1: // only one lift on these levels
			currentTunnel = levelIndexItr->second.lifts[0].tunnel;
			break;

		case 2: // two lifts on these levels
			if (((playerDroid.middlePosition.x) > levelIndexItr->second.lifts[0].posX - TILE_SIZE) &&
			    ((playerDroid.middlePosition.x) < levelIndexItr->second.lifts[0].posX + TILE_SIZE))
			{
				currentTunnel = levelIndexItr->second.lifts[0].tunnel;
			}
			else
			{
				currentTunnel = levelIndexItr->second.lifts[1].tunnel;
			}

			break;

		case 3: // only two levels with three lifts on them
			if ( levelIndexItr->second.deckNumber == 12 )
			{
				if ((playerDroid.middlePosition.x) > levelIndexItr->second.lifts[0].posX - TILE_SIZE )
				{
					currentTunnel = levelIndexItr->second.lifts[0].tunnel;
					gam_getCurrentDeck ( levelIndexItr->second.deckNumber );
					return;
				}

				if (((playerDroid.middlePosition.y) > levelIndexItr->second.lifts[1].posY - TILE_SIZE) &&
				    ((playerDroid.middlePosition.y) < levelIndexItr->second.lifts[1].posY + TILE_SIZE))
				{
					currentTunnel = levelIndexItr->second.lifts[1].tunnel;
					gam_getCurrentDeck ( levelIndexItr->second.deckNumber );
					return;
				}

				if ((playerDroid.middlePosition.y) > levelIndexItr->second.lifts[2].posY - TILE_SIZE )
				{
					currentTunnel = levelIndexItr->second.lifts[2].tunnel;
					gam_getCurrentDeck ( levelIndexItr->second.deckNumber );
					return;
				}
			} // end of if level is 12 test
			else
			{
				if ( levelIndexItr->second.deckNumber == 14 )
				{
					if (((playerDroid.middlePosition.x) > levelIndexItr->second.lifts[2].posX - TILE_SIZE) &&
					    ((playerDroid.middlePosition.x) < levelIndexItr->second.lifts[2].posX + TILE_SIZE))
					{
						currentTunnel = levelIndexItr->second.lifts[2].tunnel;
						gam_getCurrentDeck ( levelIndexItr->second.deckNumber );
						return;
					}

					if (((playerDroid.middlePosition.x) > levelIndexItr->second.lifts[1].posX - TILE_SIZE) &&
					    ((playerDroid.middlePosition.x) < levelIndexItr->second.lifts[1].posX + TILE_SIZE))
					{
						currentTunnel = levelIndexItr->second.lifts[1].tunnel;
						gam_getCurrentDeck ( levelIndexItr->second.deckNumber );
						return;
					}

					if (((playerDroid.middlePosition.x) > levelIndexItr->second.lifts[0].posX - TILE_SIZE) &&
					    ((playerDroid.middlePosition.x) < levelIndexItr->second.lifts[0].posX + TILE_SIZE))
					{
						currentTunnel = levelIndexItr->second.lifts[0].tunnel;
						gam_getCurrentDeck ( levelIndexItr->second.deckNumber );
						return;
					}
				} // end of is level 14 test
			} // end of else statement
	} // end of switch statement

	con_print ( CON_INFO, false, "Tunnel in use [ %i ]", currentTunnel );

	//
	// Get the current deck pointer
	gam_getCurrentDeck ( levelIndexItr->second.deckNumber );
} // end of function


// ----------------------------------------------------------------------------
//
// Put the player onto lift on new deck
int gam_putPlayerOnLiftFromTunnel(int currentDeck)
// ----------------------------------------------------------------------------
{
	switch (currentTunnel)
	{
		case 0:
			return 0;
			break;

		case 1:
			return 0;
			break;

		case 2:
		{
			switch (currentDeck)
			{
				case 17:
					return 0;
					break;

				case 15:
					return 1;
					break;

				default:
					break;
			}
			break;
		}

		case 3:
		{
			switch (currentDeck)
			{
				case 12:
				case 13:
				case 14:
				case 17:
					return 1;
					break;

				case 20:
					return 0;
					break;

				default:
					break;
			}
		}

		case 4:
		{
			switch (currentDeck)
			{
				case 12:
					return 2;
					break;

				case 6:
				case 5:
					return 1;
					break;

				case 1:
					return 0;
					break;

				default:
					break;
			}
		}

		case 5:
			return 0;
			break;

		case 6:
		{
			switch (currentDeck)
			{
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
					return 0;
					break;

				case 9:
					return 1;
					break;

				default:
					break;
			}
		}

		case 7:
		{
			switch (currentDeck)
			{
				case 14:
					return 2;
					break;

				case 18:
					return 0;
					break;

				default:
					break;
			}
		}

		default:
			break;
	}

	printf("ERROR: Should not be here\n");

	return -1;
} // end of function


// ----------------------------------------------------------------------------
//
// Move the lift position
void gam_moveLift(int direction)
// ----------------------------------------------------------------------------
{
	switch (direction)
	{
		case MY_INPUT_UP:
			if (currentDeckNumber != tunnel[currentTunnel].top)
			{
				tunnel[currentTunnel].current_deck++;
				tunnel[currentTunnel].current = tunnel[currentTunnel].decks[tunnel[currentTunnel].current_deck];
				currentDeckNumber = tunnel[currentTunnel].current;
				evt_sendEvent (USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_KEYPRESS_GOOD, 0, 0, glm::vec2(), glm::vec2(), "");
			}
			else
			{
				evt_sendEvent (USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_KEYPRESS_BAD, 0, 0, glm::vec2(), glm::vec2(), "");
			}

			break;

		case MY_INPUT_DOWN:
			if (currentDeckNumber != tunnel[currentTunnel].bottom)
			{
				tunnel[currentTunnel].current_deck--;
				tunnel[currentTunnel].current = tunnel[currentTunnel].decks[tunnel[currentTunnel].current_deck];
				currentDeckNumber = tunnel[currentTunnel].current;
				evt_sendEvent (USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_KEYPRESS_GOOD, 0, 0, glm::vec2(), glm::vec2(), "");
			}
			else
			{
				evt_sendEvent (USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_KEYPRESS_BAD, 0, 0, glm::vec2(), glm::vec2(), "");
			}

			break;

		case MY_INPUT_ACTION:

			lvl_changeToLevel ( lvl_returnLevelNameFromDeck(currentDeckNumber), true, gam_putPlayerOnLiftFromTunnel(currentDeckNumber) );

			evt_sendEvent (USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_KEYPRESS_GOOD, 0, 0, glm::vec2(), glm::vec2(), "");

			sys_changeMode ( MODE_GAME );

			break;

		default:
			break;
	}
}

//---------------------------------------------------------
//
// Get the positions of lifts
void gam_getLiftPositions ( const std::string &levelName )
//---------------------------------------------------------
{
	int countX = 0;
	int countY = 0;
	int currentTile = 0;
	int countLift = 0;

	CHECK_LEVEL_NAME

	countLift = 0;
	countX = 0;
	countY = 0;

	auto liftItr = levelInfo.at(levelName).lifts;

	for (int index = 0; index < levelInfo.at(levelName).levelDimensions.x * levelInfo.at(levelName).levelDimensions.y; index++)
	{
		currentTile = levelInfo.at(levelName).tiles[ ( ( countY * ( levelInfo.at(levelName).levelDimensions.x ) ) + countX )];

		if ( LIFT_TILE == currentTile )
		{
			levelInfo.at(levelName).lifts[countLift].posX = countX * TILE_SIZE;
			levelInfo.at(levelName).lifts[countLift].posY = countY * TILE_SIZE;

			countLift++;
		}

		countX++;

		if ( countX == levelInfo.at(levelName).levelDimensions.x )
		{
			countX = 0;
			countY++;
		}
	}
}

//---------------------------------------------------------
//
// Store the lift linking information into vector array
//
// Call a second time to add to vector on same level
void gam_setupLiftsStore ( const string &whichLevel, int whichTunnel )
//---------------------------------------------------------
{
	_liftBasic tempLift;

	tempLift.tunnel = whichTunnel;

//	printf("level [ %s ] Tunnel [ %i ] numLifts [ %i ]\n", whichLevel.c_str(), whichTunnel, levelInfo.at(whichLevel).numLifts);

	levelInfo.at(whichLevel).lifts.push_back(tempLift);
}

//---------------------------------------------------------
//
// Setup lifts
void gam_setupLifts ()
//---------------------------------------------------------
{
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 0 ), 1 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 1 ), 4 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 2 ), 5 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 3 ), 0 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 4 ), 0 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 5 ), 1 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 5 ), 4 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 6 ), 1 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 6 ), 4 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 8 ), 5 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 9 ), 5 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 9 ), 6 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 10 ), 6 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 11 ), 6 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 12 ), 6 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 12 ), 3 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 12 ), 4 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 13 ), 6 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 13 ), 3 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 14 ), 6 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 14 ), 3 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 14 ), 7 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 15 ), 0 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 15 ), 2 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 16 ), 0 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 17 ), 2 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 17 ), 3 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 18 ), 7 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 19 ), 0 );
	gam_setupLiftsStore ( lvl_returnLevelNameFromDeck ( 20 ), 3 );

	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 0 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 1 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 2 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 3 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 4 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 5 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 6 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 8 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 9 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 10 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 11 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 12 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 13 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 14 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 15 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 16 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 17 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 18 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 19 ));
	gam_getLiftPositions ( lvl_returnLevelNameFromDeck ( 20 ));
}
