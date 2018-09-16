#include <hdr/io/io_textures.h>
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
				}

				case USER_EVENT_TEXTURE_LOAD:
					io_loadTextureFile (tempEventData.eventString);
					break;

				case USER_EVENT_TEXTURE_ERROR:
					io_handleTextureFileError(tempEventData.data1, tempEventData.eventString);
					break;

				case USER_EVENT_TEXTURE_UPLOAD_DONE:
					io_storeTextureInfoIntoMap (tempEventData.data1, tempEventData.eventString);
					break;

				default:
					break;
			}
		}
	}
	printf ("GAME thread stopped.\n");
	return 0;
}