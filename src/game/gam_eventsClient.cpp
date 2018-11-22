#include "hdr/game/gam_player.h"
#include "hdr/game/gam_levels.h"
#include "hdr/game/gam_eventsClient.h"

//----------------------------------------------------------------------------------------
//
// Farm out the events received by the client for action
void gam_handleClientEvent ( _myEventData thisEvent )
//----------------------------------------------------------------------------------------
{
	switch ( thisEvent.data1 )
	{
		case NET_CLIENT_WORLDPOS:
			playerDroid.serverWorldPos.x = thisEvent.vec2_1.x;
			playerDroid.serverWorldPos.y = thisEvent.vec2_1.y;
			break;

		case NET_DROID_WORLDPOS:

			printf("Got droid [ %i ] new position.\n", thisEvent.data3);

			levelInfo.at (lvl_getCurrentLevelName ()).droid[thisEvent.data3].serverWorldPos.x = thisEvent.vec2_1.x;
			levelInfo.at (lvl_getCurrentLevelName ()).droid[thisEvent.data3].serverWorldPos.y = thisEvent.vec2_1.y;
			break;

		default:
			break;
	}
}

//----------------------------------------------------------------------------------------
//
// Process events coming into the client from the network
int gam_processClientEventQueue ( void *ptr )
//----------------------------------------------------------------------------------------
{
	_myEventData tempEventData;

	while ( runThreads )
	{
		SDL_Delay (THREAD_DELAY_MS);

		if ( !clientEventInQueue.empty ())   // stuff in the queue to process
		{
			if ( SDL_LockMutex (clientEventInMutex) == 0 )
			{
				tempEventData = clientEventInQueue.front ();
				clientEventInQueue.pop ();
				SDL_UnlockMutex (clientEventInMutex);
			}

			printf ("CLIENT - process event IN queue\n");

			switch ( tempEventData.eventAction )
			{
				case NET_CLIENT_DATA_PACKET:
					gam_handleClientEvent (tempEventData);
					break;

				case NET_CLIENT_SYSTEM_PACKET:
					break;

				default:
					break;
			}
		}
	}
	printf ("CLIENT EVENTS thread stopped.\n");
	return 0;
}