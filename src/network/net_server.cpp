#include "hdr/network/net_common.h"
#include "hdr/game/gam_player.h"

vector<_clientInfo>     clientInfo;

netcode_server_t        *networkServer;

bool                    runNetworkServerThread = true;
bool                    runAsServer = true;
bool                    networkServerIsRunning = false;

SDL_Thread              *networkServerThread;
SDL_Thread              *userEventNetworkServerThread;

int                     maxNumClients;  // From script
int                     serverPort;     // From script

string                  serverAddress;

//-----------------------------------------------------------------------------
//
// Thread function to process the network server packets
//
// This function sends packets OUT to clients
int net_processNetworkServerQueue (void *ptr)
//-----------------------------------------------------------------------------
{
	_networkPacket clientPacket;
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
					clientInfo[tempEventData.data2].packetSequenceCount++;      // Client Index
					clientPacket.packetType = NET_CLIENT_DATA_PACKET;
					clientPacket.data1 = tempEventData.data1;
					clientPacket.data2 = tempEventData.data2;
					clientPacket.data3 = tempEventData.data3;
					clientPacket.vec2_1 = tempEventData.vec2_1;
					clientPacket.vec2_2 = tempEventData.vec2_2;
					clientPacket.timeStamp = static_cast<long>(frameCount);
					clientPacket.packetOwner = tempEventData.data2;
					snprintf (clientPacket.text, NET_TEXT_SIZE, "%s", tempEventData.eventString.c_str ());    // Don't overflow char array

					net_sendPacket (clientPacket, USER_EVENT_NETWORK_FROM_SERVER, tempEventData.data2);

					break;

				default:
					break;  // Unknown packet type
			}
		}
	}
	printf ("SERVER thread stopped.\n");
	return 0;
}

//----------------------------------------------------------
//
// Thread to get the network server packets
//
// Decompresses the INCOMING packet and puts it onto the Server event queue
int net_getNetworkServerPackets ( void *ptr )
//----------------------------------------------------------
{
	int             packet_bytes;
	uint64_t        packet_sequence;
	uint8_t         *packet;
	_networkPacket  *serverPacket;
	lzo_uint        newLength;
	int             result;
	char            inPacketPtr[sizeof (_networkPacket) * 2]; // Make sure there is enough space to hold decompressed packet

	while ( runNetworkServerThread )
	{
		SDL_Delay (THREAD_DELAY_MS);

		for (int clientIndex = 0; clientIndex < maxNumClients; ++clientIndex)
		{
			packet = netcode_server_receive_packet (networkServer, clientIndex, &packet_bytes, &packet_sequence);
			if ( packet != nullptr)
			{
				// Got a legitimate packet - now decompress it
				// TODO: Work out compression amount - percentage compressed??
				 result = lzo1x_decompress (packet, static_cast<lzo_uint>(packet_bytes), (unsigned char *) &inPacketPtr, &newLength, nullptr);
				 if ( result != LZO_E_OK )
					{
						con_print (CON_ERROR, true, "Internal error - server network packet decompression failed [ %i ]. Packet dropped.", result);
						netcode_server_free_packet (networkServer, packet);
					}
				 else
				 {
					 // Cast to our packet structure
					 serverPacket = (_networkPacket *) &inPacketPtr;
					 serverPacket->packetOwner = clientIndex;

					 evt_sendEvent (USER_EVENT_SERVER_EVENT, serverPacket->packetType, serverPacket->data1, serverPacket->data2, serverPacket->data3, serverPacket->vec2_1, serverPacket->vec2_2,
					                serverPacket->text);

					 netcode_server_free_packet (networkServer, packet);
				 }
			}
		}
	}

	printf ("SERVER NETWORK packet thread finished.\n");
	return 0;
}

//----------------------------------------------------------------
//
// Stop the network server thread running
void net_stopNetworkServerThread ()
//----------------------------------------------------------------
{
	runNetworkServerThread = false;
}

//----------------------------------------------------------------
//
// Create the thread to run the network server function.
bool net_createNetworkServerThread ()
//----------------------------------------------------------------
{
	/*
	if ( runNetworkServerThread )
	{
		con_print (CON_INFO, true, "Network server thread is already running.");
		return true;
	}
*/

	networkServerThread = SDL_CreateThread (net_getNetworkServerPackets, "net_getNetworkServerPackets", (void *) nullptr);

	if ( nullptr == networkServerThread )
	{
		con_print (CON_ERROR, true, "SDL_CreateThread - net_getNetworkServerPackets  - failed : [ %s ]", SDL_GetError ());
		return false;
	}

	return true;
}

//----------------------------------------------------------------
//
// Start the network server thread running
bool net_startNetworkServerThread ()
//----------------------------------------------------------------
{
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

	//
	// Now create the thread that will watch for network packets and then place them into the network server queue

//	if ( !runNetworkServerThread )     // Not currently running
	{
		if ( net_createNetworkServerThread ())
		{
			runNetworkServerThread = true;
			SDL_DetachThread (networkServerThread);
		}
	}

	return true;
}

//----------------------------------------------------------------
//
// Create the network server
bool net_createServer ( float time )
//----------------------------------------------------------------
{
	struct netcode_server_config_t server_config{};

	serverAddress = net_setNetworkAddress (serverPort,"127.0.0.1" );

	printf("server [ %s ]\n", serverAddress.c_str());

	if ( netcode_init () != NETCODE_OK )
	{
		con_print(CON_ERROR, true, "Failed to initialize netcode.io. Could not start server.");
		return false;
	}

//	static int (*printf_function) ( NETCODE_CONST char *, ... ) = (int (*) ( NETCODE_CONST char *, ... )) printf;
//	netcode_set_printf_function (int (*function) ( 	NETCODE_CONST char *, ... ))

//	netcode_set_printf_function(testPrint, (NETCODE_CONST char *) (int (*) ( NETCODE_CONST char *, ... )) printf);

	netcode_log_level (NETCODE_LOG_LEVEL_INFO);// NETCODE_LOG_LEVEL_DEBUG

	netcode_default_server_config (&server_config);
	server_config.protocol_id = PROTOCOL_ID;
	memcpy (&server_config.private_key, private_key, NETCODE_KEY_BYTES);

	networkServer = netcode_server_create (serverAddress.c_str(), &server_config, time);

	if ( !networkServer )
	{
		con_print (CON_ERROR, true, "Failed to create the network server.");
		return false;
	}

	netcode_server_start (networkServer, maxNumClients);

	clientInfo.reserve (maxNumClients);
	_clientInfo tempClient;

	for (int index = 0; index < maxNumClients; index++)
      {
        tempClient.packetSequenceCount = 0;
        tempClient.clientName = "Player";
        strcpy(tempClient.currentDeck, "");

        clientInfo.push_back(tempClient);
      }

	net_startNetworkServerThread ();

	return true;
}

//----------------------------------------------------------------
//
// Shutdown the server instance and terminate netcode
void net_shutdownServer()
//----------------------------------------------------------------
{
	netcode_server_destroy( networkServer );

	net_stopNetworkServerThread ();
}

//--------------------------------------------------------------------------------
//
// Update the network server
void net_updateNetworkServer ( float time )
//--------------------------------------------------------------------------------
{
	netcode_server_update (networkServer, time);
}

//--------------------------------------------------------------------------------
//
// Send the client world position according to the server
void net_sendPositionUpdate ( int whichClient )
//--------------------------------------------------------------------------------
{
//	evt_sendEvent (USER_EVENT_NETWORK_FROM_SERVER, NETWORK_SEND_DATA, NET_CLIENT_WORLDPOS, whichClient, 0, glm::vec2{playerDroid.worldPos.x, playerDroid.worldPos.y} , glm::vec2 (), "");
}
