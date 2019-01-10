#include "hdr/game/gam_player.h"
#include "hdr/game/gam_levels.h"
#include "hdr/game/gam_eventsClient.h"

//----------------------------------------------------------------------------------------
//
// Farm out the events received by the client for action
void gam_handleClientInPacket ( RakNet::Packet passPacket )
//----------------------------------------------------------------------------------------
{
	float       varX, varY, varZ;
	int         dataType;
	int         deckNumber;

	// TODO Add time variable and RakNet time to each packet

	RakNet::BitStream bsin ( passPacket.data, passPacket.length, false );

	bsin.IgnoreBytes ( sizeof ( RakNet::MessageID ));
	bsin.Read ( dataType );

	switch ( dataType )
	{
		case NET_CLIENT_WORLDPOS:
			break;

		case NET_DROID_WORLDPOS:

			bsin.Read ( networkServerTick );
			//
			// Which deck are these droids on
			bsin.Read ( deckNumber );

			for ( int index = 0; index != levelInfo.at( lvl_returnLevelNameFromDeck(deckNumber)).numDroids; index++ )   // also send alive flag
			{
				// World position according to the server
				bsin.ReadVector ( varX, varY, varZ );
				levelInfo.at ( lvl_returnLevelNameFromDeck(deckNumber) ).droid[index].serverWorldPos.x = varX;
				levelInfo.at ( lvl_returnLevelNameFromDeck(deckNumber) ).droid[index].serverWorldPos.y = varY;

				// Current speed according to the server
				bsin.ReadVector ( varX, varY, varZ );
				levelInfo.at ( lvl_returnLevelNameFromDeck(deckNumber) ).droid[index].serverVelocity.x = varX;
				levelInfo.at ( lvl_returnLevelNameFromDeck(deckNumber) ).droid[index].serverVelocity.y = varY;


				if (((int) levelInfo.at ( lvl_returnLevelNameFromDeck(deckNumber) ).droid[index].serverWorldPos.x -
				     (int) levelInfo.at ( lvl_returnLevelNameFromDeck(deckNumber) ).droid[index].worldPos.x) > 1 )
				{
					levelInfo.at ( lvl_returnLevelNameFromDeck(deckNumber) ).droid[index].worldPos.x = levelInfo.at ( lvl_returnLevelNameFromDeck(deckNumber) ).droid[index].serverWorldPos.x;
				}

/*
					if (0 == index)
					{
						printf("Diff Droid 0 [ %3.3f %3.3f ]\n", levelInfo.at ( lvl_getCurrentLevelName ()).droid[index].serverWorldPos.x - levelInfo.at ( lvl_getCurrentLevelName ()).droid[index].worldPos.x,
						       levelInfo.at ( lvl_getCurrentLevelName ()).droid[index].serverWorldPos.y - levelInfo.at ( lvl_getCurrentLevelName ()).droid[index].worldPos.y );
					}
					*/
			}
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
		SDL_Delay ( THREAD_DELAY_MS );

		if ( !clientEventInQueue.empty ())   // stuff in the queue to process
		{
			if ( SDL_LockMutex ( clientEventInMutex ) == 0 )
			{
				tempEventData = clientEventInQueue.front ();
				clientEventInQueue.pop ();
				SDL_UnlockMutex ( clientEventInMutex );
			}

			switch ( tempEventData.eventAction )
			{
				case NET_CLIENT_DATA_PACKET:
//					gam_handleClientInPacket (tempEventData);
					break;

				case NET_CLIENT_SYSTEM_PACKET:
					break;

				default:
					break;
			}
		}
	}
	printf ( "CLIENT EVENTS thread stopped.\n" );
	return 0;
}
