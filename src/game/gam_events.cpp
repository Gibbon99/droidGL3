#include "hdr/io/io_textures.h"
#include "hdr/game/gam_levels.h"
#include "hdr/system/sys_events.h"
#include "hdr/io/io_keyboard.h"
#include "hdr/gui/gui_main.h"
#include "hdr/game/gam_events.h"
#include "hdr/system/sys_events.h"
#include "hdr/game/gam_doors.h"

//----------------------------------------------------------------
//
// Handle the events that need to be run in the main loop
// eg: Changes to physics world
int gam_processMainLoopEventQueue()
//----------------------------------------------------------------
{
	_myEventData tempEventData;

	if ( !mainLoopEventQueue.empty() )
	{
		if ( SDL_LockMutex ( mainLoopMutex ) == 0)
		{
			tempEventData = mainLoopEventQueue.front();
			mainLoopEventQueue.pop ();
			SDL_UnlockMutex (mainLoopMutex);
		}

		switch ( tempEventData.eventAction )
		{
			case MAIN_LOOP_EVENT_ADD_BULLET:

//				evt_sendEvent ( MAIN_LOOP_EVENT, MAIN_LOOP_EVENT_ADD_BULLET, dataBaseEntry[0].bulletType, -1, false, glm::vec2{playerDroid.middlePosition.x, playerDroid.middlePosition.y}, glm::vec2{destPosition.x, destPosition.y}, lvl_getCurrentLevelName () );
				bul_newBullet ( cpVect{tempEventData.vec2_1.x, tempEventData.vec2_1.y},
				                cpVect{tempEventData.vec2_2.x, tempEventData.vec2_2.y},
				                tempEventData.data1, tempEventData.data2, tempEventData.eventString );
				break;

			case MAIN_LOOP_EVENT_REMOVE_BULLET:
				bul_removeBullet (tempEventData.data1, tempEventData.data2, tempEventData.data3);
				break;

		  case MAIN_LOOP_EVENT_REMOVE_DROID_PHYSICS:
                sys_removeEnemyPhysics(tempEventData.data1, tempEventData.eventString);
		        break;

		  default:
		        break;
		}
	}
	return 0;
}

//----------------------------------------------------------------
//
// Handle a game event - called by thread
int gam_processGameEventQueue ( void *ptr )
//----------------------------------------------------------------
{
	_myEventData tempEventData;

	while ( runThreads )
	{
		SDL_Delay (THREAD_DELAY_MS);

		if ( !gameEventQueue.empty ())   // stuff in the queue to process
		{
			if ( SDL_LockMutex (gameMutex) == 0 )
			{
				tempEventData = gameEventQueue.front ();
				gameEventQueue.pop ();
				SDL_UnlockMutex (gameMutex);
			}

			switch ( tempEventData.eventAction )
			{
				case USER_EVENT_GAME_TIMER:
				{
					switch ( tempEventData.data1 )
					{
						case USER_EVENT_GAME_TIMER_CONSOLE:
						{
							// Change the state of the console timer cursor animation
							evt_cursorChangeState (tempEventData.data2);
						}
						break;

						default:
							break;
					}
					break;
				}

				case USER_EVENT_TEXTURE_LOAD:
				{
					numTexturesToLoad++;    // Count how many textures should be loaded - one per event
					io_loadTextureFile (tempEventData.eventString);
					break;
				}

				case USER_EVENT_TEXTURE_ERROR:
				{
					io_handleTextureFileError (tempEventData.data1, tempEventData.eventString);
					break;
				}

				case USER_EVENT_TEXTURE_UPLOAD_DONE:
				{
					io_storeTextureInfoIntoMap (tempEventData.data1, tempEventData.vec2_1, tempEventData.eventString, true);
					break;
				}

				case USER_EVENT_GAME_LOAD_LEVEL:
				{
					lvl_loadLevelFromFile (tempEventData.eventString, tempEventData.data1);
				}

				case USER_EVENT_LEVEL_ERROR:
				{
					lvl_handleLevelFileError (tempEventData.data1, tempEventData.eventString);
					break;
				}

				case USER_EVENT_LEVEL_EXTRAS:
				{
					lvl_addPaddingToLevel(tempEventData.eventString);
					levelInfo.at(tempEventData.eventString).lifts.reserve ( levelInfo.at(tempEventData.eventString).numLifts );
					levelInfo.at(tempEventData.eventString).bullet.reserve ( 16 );
					bul_initArray ( tempEventData.eventString );
                    gam_doorTriggerSetup ( tempEventData.eventString );
					break;
				}

				case USER_EVENT_LEVEL_LOAD_DONE:    // Levels all loaded - do lifts, droids etc
				{
					allLevelsLoaded = true;
					gam_setupLifts ();
					break;
				}

				case USER_EVENT_KEY_EVENT:
				{
					// Handle key press events
                    io_processGameInputEvents (tempEventData);
					break;
				}
				case USER_EVENT_TTF_ERROR:
				{
					gui_handleTTFError (tempEventData.data1, tempEventData.eventString);
					break;

				}

				default:
					break;
			}
		}
	}
	printf ("GAME thread stopped.\n");
	return 0;
}
