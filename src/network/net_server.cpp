#include <hdr/network/net_common.h>
#include <hdr/io/minilzo/minilzo.h>
#include "hdr/network/net_server.h"

netcode_server_t        *networkServer;
_networkPacket          serverPacket;

bool                    runNetworkServerThread = true;
bool                    runAsServer = true;
long                    networkServerPacketCount = 0;

SDL_Thread              *networkServerThread;
SDL_Thread              *userEventNetworkServerThread;

int                     maxNumClients;  // From script
int                     serverPort;     // From script

char *                  serverAddress; // = "[::1]:9991";

//-----------------------------------------------------------------------------
//
// Thread function to process the network server packets
int net_processNetworkServerQueue (void *ptr)
//-----------------------------------------------------------------------------
{
	_myEventData tempEventData;

	while ( runThreads )
	{
		SDL_Delay (THREAD_DELAY_MS);

		if ( !networkServerQueue.empty ())   // stuff in the queue to process
		{
			if ( SDL_LockMutex (networkServerMutex) == 0 )
			{
				tempEventData = networkServerQueue.front ();
				networkServerQueue.pop ();

				SDL_UnlockMutex (networkServerMutex);
			}

			switch ( tempEventData.eventAction )
			{
				case NETWORK_RECEIVE:
					printf("Got a DATA packet - client says Position [ %3.3f %3.3f ] Text [ %s ]\n", tempEventData.vec2_1.x, tempEventData.vec2_1.y,  tempEventData.eventString.c_str());
					break;

				case NET_SYSTEM_PACKET:
					printf("Got a SYSTEM packet from the client.\n");
					break;

				default:
					break;
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

	printf ("Server - checking for a packet. runNetworkServerThread is [ %i ]\n", runNetworkServerThread);

	while ( runNetworkServerThread )
	{
		SDL_Delay (THREAD_DELAY_MS);

		packet = netcode_server_receive_packet (networkServer, 0, &packet_bytes, &packet_sequence);
		if ( packet != nullptr)
		{
			char            inPacketPtr[sizeof(_networkPacket) * 2];    // Make sure there is enough space to hold decompressed packet
			lzo_uint        newLength;

			int result = lzo1x_decompress(packet, static_cast<lzo_uint>(packet_bytes), (unsigned char *)&inPacketPtr, &newLength, nullptr);
			if (result != LZO_E_OK)
			{
				con_print(CON_ERROR, true, "Internal error - server network packet decompression failed [ %i ]. Packet dropped.", result);
				netcode_server_free_packet (networkServer, packet);
				return -1;      // Ignore the packet
			}

			serverPacket = (_networkPacket *)&inPacketPtr;

			switch (serverPacket->packetType)
			{
				case NET_DATA_PACKET:
					if (serverPacket->sequence != networkServerPacketCount)     // What happens with dropped packets??
					{
						netcode_server_free_packet (networkServer, packet);
						return -1; // bad packet ?
					}

					networkServerPacketCount++;
					evt_sendEvent (USER_EVENT_NETWORK_SERVER, NETWORK_RECEIVE, serverPacket->data1, serverPacket->data2, serverPacket->data3, serverPacket->vec2_1, serverPacket->vec2_2, serverPacket->text);
					break;

				case NET_SYSTEM_PACKET:
					evt_sendEvent (USER_EVENT_NETWORK_SERVER, NET_SYSTEM_PACKET, serverPacket->data1, serverPacket->data2, serverPacket->data3, serverPacket->vec2_1, serverPacket->vec2_2, serverPacket->text);
					break;

				default:
					break;  // Unknown packet type
			}
			netcode_server_free_packet (networkServer, packet);
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
			SDL_Delay (500);
			SDL_DetachThread (networkServerThread);

			printf ("Thread [ %s ] has been started\n", "net_getNetworkServerPackets");
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

	serverAddress = "[::1]:9991";

	if ( netcode_init () != NETCODE_OK )
	{
		con_print(CON_ERROR, true, "Failed to initialize netcode.io. Could not start server.");
		return false;
	}

//	netcode_set_printf_function((const char *)input, con_print);

	netcode_log_level (NETCODE_LOG_LEVEL_INFO);

	netcode_default_server_config (&server_config);
	server_config.protocol_id = PROTOCOL_ID;
	memcpy (&server_config.private_key, private_key, NETCODE_KEY_BYTES);

	networkServer = netcode_server_create (serverAddress, &server_config, time);

	if ( !networkServer )
	{
		con_print (CON_ERROR, true, "Failed to create the network server.");
		return false;
	}

	netcode_server_start (networkServer, maxNumClients);

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
