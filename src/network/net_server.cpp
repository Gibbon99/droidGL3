#include <hdr/network/net_common.h>
#include <hdr/io/minilzo/minilzo.h>
#include "hdr/network/net_server.h"
#include "hdr/network/net_serverData.h"

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

/*
//-----------------------------------------------------------------------------
//
// Send the passed in packet to the server
void net_sendPacketToClient ( _networkPacket clientPacket )
//-----------------------------------------------------------------------------
{
	lzo_uint inLength;
	lzo_uint outLength;
	void *packetPtr;
	char outPacketPtr[sizeof (clientPacket)];
	int result;

	packetPtr = &clientPacket;

	inLength = sizeof (clientPacket);

	result = lzo1x_1_compress ((unsigned char *) packetPtr, inLength, (unsigned char *) &outPacketPtr, &outLength, wrkmem);

	if ( result != LZO_E_OK )
	{
		con_print (CON_ERROR, true, "Error compressing network packet . %lu bytes into %lu bytes", (unsigned long) inLength, (unsigned long) outLength);
		return; // Don't send the packet
	}

//	if ( netcode_client_state (networkClient) == NETCODE_CLIENT_STATE_CONNECTED )
	if ( networkClientCurrentState == NETCODE_CLIENT_STATE_CONNECTED )
		netcode_client_send_packet (networkClient, (unsigned char *) outPacketPtr, (int) (outLength));
}
*/

//-----------------------------------------------------------------------------
//
// Thread function to process the network server packets
int net_processNetworkServerQueue (void *ptr)
//-----------------------------------------------------------------------------
{
	_networkPacket tempNetworkPacket;

	while ( runThreads )
	{
		SDL_Delay (THREAD_DELAY_MS);

		if ( !networkServerQueue.empty ())   // stuff in the queue to process
		{
			if ( SDL_LockMutex (networkServerMutex) == 0 )
			{
				tempNetworkPacket = networkServerQueue.front ();
				networkServerQueue.pop ();
				SDL_UnlockMutex (networkServerMutex);
			}

			switch ( tempNetworkPacket.packetType)
			{
			  case NET_DATA_PACKET:   // It's a data packet from a client

				clientInfo[tempNetworkPacket.packetOwner].packetSequenceCount++;
				printf("Data packet from CLIENT [ %i ] - Says [ %s ]\n", tempNetworkPacket.packetOwner, tempNetworkPacket.text);
				/*
					if ( tempNetworkPacket->sequence != networkServerPacketCount )     // What happens with dropped packets??
					{
						return -1; // bad packet ?
					}

					networkServerPacketCount++; // need this for each client?
					evt_sendEvent (USER_EVENT_NETWORK_SERVER, NETWORK_RECEIVE, tempNetworkPacket->data1, tempNetworkPacket->data2, tempNetworkPacket->data3, tempNetworkPacket->vec2_1, tempNetworkPacket->vec2_2,
					               tempNetworkPacket->text);
					               */

//                net_handleClientDataPacket(tempNetworkPacket);
					break;

				case NET_SYSTEM_PACKET:     // It's a system packet from a client

				printf("SYSTEM packet from client [ %i ]\n", tempNetworkPacket.packetOwner);
				/*
					evt_sendEvent (USER_EVENT_NETWORK_SERVER, NET_SYSTEM_PACKET, tempNetworkPacket->data1, tempNetworkPacket->data2, tempNetworkPacket->data3, tempNetworkPacket->vec2_1, tempNetworkPacket->vec2_2,
					               tempNetworkPacket->text);
					               */
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
// Decompresses the packet and puts it onto the processing queue
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
						return -1;      // Ignore the packet
					}

				// Cast to our packet structure
				serverPacket = (_networkPacket *) &inPacketPtr;
				serverPacket->packetOwner = clientIndex;

                printf("Client [ %i ] packetCount [ %i ]\n", clientIndex, clientInfo[clientIndex].packetSequenceCount);

				if ( SDL_LockMutex (networkServerMutex) == 0 )
				{
					networkServerQueue.push (*serverPacket);

					SDL_UnlockMutex (networkServerMutex);
				}
				netcode_server_free_packet (networkServer, packet);
			}
            else
              {
              }
		}
	}
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

	networkServerMutex = SDL_CreateMutex ();
	if ( !networkServerMutex )
	{
		printf ("Couldn't create mutex - networkServerMutex.\n");
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

	serverAddress = net_setNetworkAddress (serverPort,"[::1]" );

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
