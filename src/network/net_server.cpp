#include "hdr/network/net_server.h"
#include "hdr/network/net_common.h"
#include "hdr/game/gam_player.h"

bool                    runAsServer = true;
bool                    networkServerIsRunning = false;

ENetAddress             enetServerAddress;
ENetHost                *enetServer = nullptr;

SDL_Thread              *userEventNetworkServerThread;
SDL_Thread              *enetServerThread;

int                     maxNumClients;  // From script
int                     serverPort;     // From script

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
int enet_pollServerEvents ( void *ptr )
//-----------------------------------------------------------------------------------------------------
{
	ENetEvent           event;
	string              clientIP;
	_networkPacket      inPacketPtr;
	_enetClientInfo     tempClientInfo;

	while ( runThreads )
	{
		SDL_Delay (THREAD_DELAY_MS);

//		printf("Server is polling events.\n");

		if ( enet_host_service (enetServer, &event, 0) > 0 )
		{
			switch ( event.type )
			{
				case ENET_EVENT_TYPE_CONNECT:
					enet_address_get_host_ip (&event.peer->address, (char *) clientIP.c_str (), 16);

					con_print (CON_INFO, true, "Connect ID [ %i ] incomingPeerID [ %i ]", event.peer->connectID, event.peer->incomingPeerID);
					con_print (CON_INFO, true, "A new client [ %s ] connected from %x:%u.\n", clientIP.c_str (), event.peer->address.host, event.peer->address.port);

					memcpy(&tempClientInfo.peer, event.peer, sizeof(_ENetPeer));
					tempClientInfo.packetSequenceCount = 0;
					tempClientInfo.name = "Test Name";

					enetClientInfo.push_back(tempClientInfo);

					con_print (CON_INFO, true, "Number of clients [ %i ]\n", enetClientInfo.size());
					break;

				case ENET_EVENT_TYPE_RECEIVE:

					memcpy ((void *)&inPacketPtr, event.packet->data, sizeof (_networkPacket));

					evt_sendEvent (USER_EVENT_SERVER_EVENT, inPacketPtr.packetType, inPacketPtr.data1, inPacketPtr.data2, inPacketPtr.data3, inPacketPtr.vec2_1, inPacketPtr.vec2_2,
					               inPacketPtr.text);



					// Clean up the packet now that we're done using it.
					enet_packet_destroy (event.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					printf ("%s disconnected.\n", event.peer->data);
					/* Reset the peer's client information. */
					event.peer->data = nullptr;
					break;

				case ENET_EVENT_TYPE_NONE:
					break;

				default:
					printf ("Unknown event received\n");
					break;
			}
		}
	}
	printf ("ENET SERVER thread finished.\n");
	return 0;
}

//-----------------------------------------------------------------------------
//
// Thread function to process the network server packets
//
// This function sends packets OUT to clients
int net_processNetworkServerQueue (void *ptr)
//-----------------------------------------------------------------------------
{
	_networkPacket  clientPacket;
	_myEventData    tempEventData;

	while ( runThreads )
	{
		SDL_Delay (THREAD_DELAY_MS);

		if ( !networkServerOutQueue.empty ())   // stuff in the queue to process
		{
			if ( SDL_LockMutex (networkServerOutMutex) == 0 )
			{
				tempEventData = networkServerOutQueue.front ();
				networkServerOutQueue.pop ();
				SDL_UnlockMutex (networkServerOutMutex);
			}

			switch ( tempEventData.eventAction)
			{
				case NETWORK_SEND_DATA:

					//printf ("SERVER is sending out packets - queue [ %i ].\n", networkServerOutQueue.size());

					enetClientInfo[tempEventData.data2].packetSequenceCount++;      // Client Index
					clientPacket.packetType = NET_CLIENT_DATA_PACKET;
					clientPacket.data1 = tempEventData.data1;
					clientPacket.data2 = tempEventData.data2;
					clientPacket.data3 = tempEventData.data3;
					clientPacket.vec2_1 = tempEventData.vec2_1;
					clientPacket.vec2_2 = tempEventData.vec2_2;
					clientPacket.timeStamp = static_cast<long>(frameCount);
					clientPacket.packetOwner = tempEventData.data2;
					snprintf (clientPacket.text, NET_TEXT_SIZE, "%s", tempEventData.eventString.c_str ());    // Don't overflow char array

					enet_sendPacket (clientPacket, USER_EVENT_NETWORK_FROM_SERVER, tempEventData.data2);

					break;

				default:
					break;  // Unknown packet type
			}
		}
	}
	printf ("SERVER thread stopped.\n");
	return 0;
}


//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
bool enet_startServer ( const string &hostAddress, enet_uint16 hostPort, size_t enetMaxNumClients, size_t enetNumChannels )
//-----------------------------------------------------------------------------------------------------
{
	string ip;
	int returnValue;

	if ( !enetInitDone )
		if ( !enet_initLibrary ())
			return false;

//	enetClientInfo.reserve (enetMaxNumClients);

	enetServerAddress.host = ENET_HOST_ANY;
	enetServerAddress.port = hostPort;

	returnValue = enet_address_set_host (&enetServerAddress, hostAddress.c_str ());
	if ( returnValue < 0 )
	{
		con_print (CON_ERROR, true, "Unable to resolve hostName [ %s ] to create server.", hostAddress);
		return false;
	}

	enetServerAddress.host = ENET_HOST_ANY;

	enetServer = enet_host_create (&enetServerAddress, enetMaxNumClients, enetNumChannels, 0, 0);    // Set numClients
	if ( enetServer == nullptr )
	{
		con_print (CON_ERROR, true, "Unable to create server instance.");
		return false;
	}

	returnValue = enet_address_get_host_ip (&enetServer->address, (char *) ip.c_str (), 16);
	if ( returnValue < 0 )
	{
		con_print (CON_ERROR, true, "Unable to obtain server address.");
		return false;
	}

	con_print (CON_INFO, true, "Server address [ %s : %i] numClients [ %i ]", ip.c_str (), hostPort, enetMaxNumClients);

	enetServerThread = SDL_CreateThread (enet_pollServerEvents, "enet_pollServerEvents", (void *) nullptr);
	if ( nullptr == enetServerThread )
	{
		printf ("SDL_CreateThread - enetServerThread - failed: %s\n", SDL_GetError ());
		return false;
	}

	userEventNetworkServerThread = SDL_CreateThread (net_processNetworkServerQueue, "net_processNetworkServerQueue", (void *) nullptr);
	if ( nullptr == userEventNetworkServerThread )
	{
		printf ("SDL_CreateThread - userEventNetworkServerThread - failed: %s\n", SDL_GetError ());
		return false;
	}

	networkServerOutMutex = SDL_CreateMutex ();
	if ( !networkServerOutMutex )
	{
		printf ("Couldn't create mutex - networkServerOutMutex.\n");
		return false;
	}

	SDL_DetachThread (userEventNetworkServerThread);
	SDL_DetachThread (enetServerThread);

	return true;
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void enet_shutdownServer ()
//-----------------------------------------------------------------------------------------------------
{
	if ( enetServer )
		enet_host_destroy (enetServer);
}

//--------------------------------------------------------------------------------
//
// Send the client world position according to the server
void net_sendPositionUpdate ( int whichClient )
//--------------------------------------------------------------------------------
{
//	evt_sendEvent (USER_EVENT_NETWORK_FROM_SERVER, NETWORK_SEND_DATA, NET_CLIENT_WORLDPOS, whichClient, 0, glm::vec2{playerDroid.worldPos.x, playerDroid.worldPos.y} , glm::vec2 (), "");
}
