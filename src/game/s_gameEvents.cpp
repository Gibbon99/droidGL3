#include <hdr/io/io_textures.h>
#include <hdr/game/s_levels.h>
#include <hdr/system/sys_events.h>
#include "hdr/game/s_gameEvents.h"
#include "hdr/system/sys_events.h"

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
					io_storeTextureInfoIntoMap (tempEventData.data1, tempEventData.vec2_1, tempEventData.eventString);
					break;
				}

				case USER_EVENT_GAME_LOAD_LEVEL:
				{
					gam_loadLevelFromFile(tempEventData.eventString);
				}

				case USER_EVENT_LEVEL_ERROR:
				{
					gam_handleLevelFileError(tempEventData.data1, tempEventData.eventString);
					break;
				}

				case USER_EVENT_LEVEL_EXTRAS:
				{
					lvl_addPaddingToLevel(tempEventData.eventString);
					break;
				}

				case USER_EVENT_LEVEL_LOAD_DONE:    // Levels all loaded - do lifts, droids etc
				{
					allLevelsLoaded = true;
					con_print(CON_INFO, true, "All levels are loaded. Run physics, droid etc setup here.");
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