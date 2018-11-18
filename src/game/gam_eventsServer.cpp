#include <hdr/game/gam_levels.h>
#include "hdr/game/gam_eventsServer.h"

//----------------------------------------------------------------------------------------
//
// Farm out the events received by the server for action
void gam_handleServerEvent(_myEventData thisEvent)
//----------------------------------------------------------------------------------------
{
	switch (thisEvent.data1)
	{
		case NET_CLIENT_CURRENT_LEVEL:
			//printf ("SERVER Says - client [ %i ] is on level [ %s ]\n", thisEvent.data2, thisEvent.eventString.c_str());
			break;

		default:
			break;
	}
}

//----------------------------------------------------------------------------------------
//
// Process events coming into the server
int gam_processServerEventQueue ( void *ptr )
//----------------------------------------------------------------------------------------
{
	_myEventData tempEventData;

	while ( runThreads )
	{
		SDL_Delay (THREAD_DELAY_MS);

		if ( !serverEventInQueue.empty ())   // stuff in the queue to process
		{
			if ( SDL_LockMutex (serverEventInMutex) == 0 )
			{
				tempEventData = serverEventInQueue.front ();
				serverEventInQueue.pop ();
				SDL_UnlockMutex (serverEventInMutex);
			}

			switch ( tempEventData.eventAction )
			{
				case NET_CLIENT_DATA_PACKET:
					gam_handleServerEvent(tempEventData);
					break;

				case NET_CLIENT_SYSTEM_PACKET:
					break;

			}
		}
	}
	printf ("SERVER EVENTS thread stopped.\n");
	return 0;
}