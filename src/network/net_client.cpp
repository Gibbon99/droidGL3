#include "hdr/network/net_client.h"
#include "hdr/network/net_common.h"


#include <string.h>

SDL_Thread              *userEventNetworClientThread;
SDL_Thread              *enetClientThread;

int                     networkClientPacketCount = 0;

ENetHost                *enetClient = nullptr;
ENetPeer                *serverPeer = nullptr;

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
//
// Poll for INCOMING packets to the client
//
int enet_pollClientEvents ( void *ptr )
//-----------------------------------------------------------------------------------------------------
{
	ENetEvent           event;
	string              clientIP;
	_networkPacket      inPacketPtr;

	while ( runThreads )
	{
		SDL_Delay (THREAD_DELAY_MS);

//		printf("Client is polling events.\n");

		if ( enet_host_service (enetClient, &event, 0) > 0 )
		{
			switch ( event.type )
			{
				case ENET_EVENT_TYPE_CONNECT:
					enet_address_get_host_ip (&event.peer->address, (char *) clientIP.c_str (), 16);

					con_print (CON_INFO, true, "Connected with the server - Success -  ID [ %i ] incomingPeerID [ %i ]", event.peer->connectID, event.peer->incomingPeerID);
					break;

				case ENET_EVENT_TYPE_RECEIVE:

					printf("CLIENT got a packet - runThreads is [ %i ].\n", runThreads);

					memcpy ((void *) &inPacketPtr, event.packet->data, sizeof (_networkPacket));

					evt_sendEvent (USER_EVENT_CLIENT_EVENT, inPacketPtr.packetType, inPacketPtr.data1, inPacketPtr.data2, inPacketPtr.data3, inPacketPtr.vec2_1, inPacketPtr.vec2_2, inPacketPtr.text);

					printf("CLIENT - got a packet IN - add to queue\n");

					enet_packet_destroy (event.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					printf ("%s disconnected.\n", event.peer->data);
					/* Reset the peer's client information. */
					event.peer->data = NULL;
					break;

				case ENET_EVENT_TYPE_NONE:
					break;

				default:
					printf ("Unknown event received\n");
					break;
			}
		}
	}
	printf ("ENET CLIENT thread finished.\n");
	return 0;
}

//-----------------------------------------------------------------------------
//
// Thread function to process the network client packets
//
// This function sends packets OUT to server
int net_processNetworkClientQueue ( void *ptr )
//-----------------------------------------------------------------------------
{
	_myEventData    tempEventData;
	_networkPacket  clientPacket;

	while ( runThreads )
	{
		SDL_Delay (THREAD_DELAY_MS);

		if ( !networkClientOutQueue.empty ())   // stuff in the queue to process
		{
			if ( SDL_LockMutex (networkClientOutMutex) == 0 )
			{
				tempEventData = networkClientOutQueue.front ();
				networkClientOutQueue.pop ();
				SDL_UnlockMutex (networkClientOutMutex);
			}

			printf ("CLIENT is sending out packets - queue [ %i ].\n", networkClientOutQueue.size ());

			switch ( tempEventData.eventAction )
			{
				case NET_STATUS:    // Client network state has changed
					break;

				case NETWORK_SEND_DATA:
					clientPacket.packetType = NET_CLIENT_DATA_PACKET;
					clientPacket.data1 = tempEventData.data1;
					clientPacket.data2 = tempEventData.data2;
					clientPacket.data3 = tempEventData.data3;
					clientPacket.vec2_1 = tempEventData.vec2_1;
					clientPacket.vec2_2 = tempEventData.vec2_2;
					clientPacket.timeStamp = static_cast<long>(frameCount);
					clientPacket.sequence = networkClientPacketCount++;
					clientPacket.packetOwner = 0;
					snprintf(clientPacket.text, NET_TEXT_SIZE, "%s", tempEventData.eventString.c_str());    // Don't overflow char array

                    enet_sendPacket (clientPacket, USER_EVENT_NETWORK_FROM_CLIENT, 0 );
                    break;
/*
				case NETWORK_SEND_SYSTEM:
					clientPacket.packetType = NET_SYSTEM_PACKET;
					clientPacket.data1 = tempEventData.data1;
                    if (networkClientCurrentState == NETCODE_CLIENT_STATE_CONNECTED)
					  net_sendClientPacket (clientPacket);
                    else
                      evt_sendEvent (USER_EVENT_NETWORK_FROM_CLIENT, NETWORK_SEND_DATA, tempEventData.data1, tempEventData.data2, tempEventData.data3, tempEventData.vec2_1, tempEventData.vec2_2, tempEventData.eventString);
					break;
*/
				default:
					break;
			}
		}
	}
	printf ("CLIENT thread stopped.\n");
	return 0;
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
bool enet_startClient ( size_t maxNumChannels )
//-----------------------------------------------------------------------------------------------------
{
	string ip;
	int returnValue;

	if ( !enetInitDone )
		if ( !enet_initLibrary ())
			return false;

	enetClient = enet_host_create (nullptr, 1, maxNumChannels, 0, 0);
	if ( enetClient == nullptr )
	{
		con_print (CON_ERROR, true, "Unable to create network client.");
		return false;
	}
	returnValue = enet_address_get_host_ip (&enetClient->address, (char *) ip.c_str (), 16);
	if ( returnValue < 0 )
	{
		con_print (CON_ERROR, true, "Unable to obtain client address.");
		return false;
	}

	con_print (CON_INFO, true, "Client address [ %s ] Port [ %i ]", ip.c_str (), enetClient->address.port);

	//
	// Thread creation
	//
	enetClientThread = SDL_CreateThread (enet_pollClientEvents, "enet_pollClientEvents", (void *) nullptr);
	if ( nullptr == enetClientThread )
	{
		printf ("SDL_CreateThread - enetClientThread - failed: %s\n", SDL_GetError ());
		return false;
	}

	userEventNetworClientThread = SDL_CreateThread (net_processNetworkClientQueue, "net_processNetworkClientQueue", (void *) nullptr);
	if ( nullptr == userEventNetworClientThread )
	{
		printf ("SDL_CreateThread - userEventNetworkClientThread - failed: %s\n", SDL_GetError ());
		return false;
	}

	networkClientOutMutex = SDL_CreateMutex ();
	if ( !networkClientOutMutex )
	{
		printf ("Couldn't create mutex - networkClientOutMutex");
		return false;
	}

	SDL_DetachThread (userEventNetworClientThread);     // Thread to push packets OUT
	SDL_DetachThread (enetClientThread);                // Thread to listen for packets IN

	return true;
}


//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
bool enet_clientConnectTo ( const string &serverName, enet_uint16 serverPort, size_t numChannels, enet_uint32 timeOut )
//-----------------------------------------------------------------------------------------------------
{
	ENetAddress serverAddress;
	ENetEvent event;

	char message[1024];

	enet_address_set_host (&serverAddress, serverName.c_str ());
	serverAddress.port = serverPort;

	// Initiate the connection, allocating the two channels 0 and 1.
	serverPeer = enet_host_connect (enetClient, &serverAddress, numChannels, 0);
	if ( serverPeer == nullptr )
	{
		con_print (CON_ERROR, true, "No available peers for initiating an ENet connection.");
		return false;
	}

	for ( int i = 0; i != 5; i++ )
	{
		con_print (CON_INFO, true, "Client attempting connection to [ %s : %i]", serverName.c_str (), serverPort);

		// Wait up to TIMEOUT seconds for the connection attempt to succeed.
		if ( enet_host_service (enetClient, &event, timeOut) > 0 && event.type == ENET_EVENT_TYPE_CONNECT )
		{
			con_print (CON_INFO, true, "Client connected to server [ %s ]", serverName.c_str ());

//			ENetPacket *packet = enet_packet_create (message, strlen (message) + 1, ENET_PACKET_FLAG_RELIABLE);
//			enet_peer_send (serverPeer, 0, packet);

			return true;
		}
		else
		{
			switch ( event.type )
			{
				case ENET_EVENT_TYPE_NONE:
					con_print (CON_ERROR, true, "Connection attempt generated [ %s ]", "ENET_EVENT_TYPE_NONE ");
					break;

				case ENET_EVENT_TYPE_CONNECT:
					con_print (CON_ERROR, true, "Connection attempt generated [ %s ]", "ENET_EVENT_TYPE_CONNECT ");
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					con_print (CON_ERROR, true, "Connection attempt generated [ %s ]", "ENET_EVENT_TYPE_DISCONNECT ");
					break;

				case ENET_EVENT_TYPE_RECEIVE:
					con_print (CON_ERROR, true, "Connection attempt generated [ %s ]", "ENET_EVENT_TYPE_RECEIVE ");
					break;

				default:
					con_print (CON_ERROR, true, "Connection attempt generated [ %s ]", "UNKNOWN Event type. ");
					break;
			}
			//		return false;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void enet_shutdownClient ()
//-----------------------------------------------------------------------------------------------------
{
	if ( enetClient )
		enet_host_destroy (enetClient);
}

//--------------------------------------------------------------------------------
//
// Send the current level to the server for this client
void net_sendCurrentLevel(string whichLevel)
//--------------------------------------------------------------------------------
{
	evt_sendEvent (USER_EVENT_NETWORK_FROM_CLIENT, NETWORK_SEND_DATA, NET_CLIENT_CURRENT_LEVEL, 0, 0, glm::vec2(), glm::vec2(), whichLevel);
}

