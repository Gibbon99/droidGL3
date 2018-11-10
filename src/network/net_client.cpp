//#include "hdr/io/minilzo/lzoconf.h"
#include "hdr/io/minilzo/minilzo.h"
#include "hdr/network/net_client.h"

uint64_t                networkClientID = 0;
netcode_client_t        *networkClient;

SDL_Thread              *networkClientThread;
SDL_Thread              *userEventNetworClientThread;

SDL_TimerID             networkClientKeepaliveCheck;

int                     networkClientPacketCount = 0;

bool                    runNetworkClientThread = false;

bool                    haveSeenTraffic = false;

uint8_t                 connect_token[NETCODE_CONNECT_TOKEN_BYTES];

bool net_compressPacket ( _networkPacket inPacket, unsigned char *outPacket, lzo_uint inSize, lzo_uint *outSize )
{
	lzo_uint outLength;
	void *inPacketPtr;
	int result;

	inPacketPtr = &inPacket;

	result = lzo1x_1_compress ((unsigned char *) inPacketPtr, inSize, outPacket, &outLength, wrkmem);

	con_print (CON_INFO, true, "Compressed %lu bytes into %lu bytes", (unsigned long) inSize, (unsigned long) outLength);

	*outSize = outLength;
	return result == LZO_E_OK;
}

//-----------------------------------------------------------------------------
//
// Run a timer to see if we have not sent any traffic in a while
// if not, send a keepalive packet to the server
// Client send function will set this to true each time, if it's false, then
// no packets have been send to the server in 'interval' period
Uint32 net_networkClientKeepaliveCallback( Uint32 interval, void *param )
//-----------------------------------------------------------------------------
{
	if (haveSeenTraffic)        // Have seen traffic go from client
		haveSeenTraffic = !haveSeenTraffic; // Set to false for next check
	else
	{
		haveSeenTraffic = false;
		evt_sendEvent (USER_EVENT_NETWORK_CLIENT, NETWORK_SEND_SYSTEM, NET_SYS_KEEPALIVE, networkClientID, 0, glm::vec2(), glm::vec2(), "");
	}

	return interval;
}

//-----------------------------------------------------------------------------
//
// Send the passed in packet to the server
void net_sendClientPacket(_networkPacket clientPacket)
//-----------------------------------------------------------------------------
{
	lzo_uint        inLength;
	lzo_uint        outLength;
	void            *packetPtr;
	char            outPacketPtr[sizeof (clientPacket)];
	int             result;

	packetPtr = &clientPacket;

	inLength = sizeof (clientPacket);

//						net_compressPacket (clientPacket, (unsigned char*)&outPacketPtr, inLength, &outLength );

	result = lzo1x_1_compress ((unsigned char *) packetPtr, inLength, (unsigned char *) &outPacketPtr, &outLength, wrkmem);

	if ( result != LZO_E_OK )
	{
		con_print (CON_ERROR, true, "Error compressing network packet . %lu bytes into %lu bytes", (unsigned long) inLength, (unsigned long) outLength);
		return; // Don't send the packet
	}

	if ( netcode_client_state (networkClient) == NETCODE_CLIENT_STATE_CONNECTED )
		netcode_client_send_packet (networkClient, (unsigned char *) outPacketPtr, (int) (outLength));
}

//-----------------------------------------------------------------------------
//
// Thread function to process the network client packets
int net_processNetworkClientQueue ( void *ptr )
//-----------------------------------------------------------------------------
{
	_myEventData    tempEventData;
	_networkPacket  clientPacket;

	while ( runThreads )
	{
		SDL_Delay (THREAD_DELAY_MS);

		if ( !networkClientQueue.empty ())   // stuff in the queue to process
		{
			if ( SDL_LockMutex (networkClientMutex) == 0 )
			{
				tempEventData = networkClientQueue.front ();
				networkClientQueue.pop ();

				SDL_UnlockMutex (networkClientMutex);
			}

			switch ( tempEventData.eventAction )
			{
				case NETWORK_RECEIVE:
					printf ("Got a data packet from the server.\n");
					break;

				case NETWORK_SEND_DATA:
					clientPacket.packetType = NET_DATA_PACKET;
					clientPacket.data1 = tempEventData.data1;
					clientPacket.data2 = tempEventData.data2;
					clientPacket.data3 = tempEventData.data3;
					clientPacket.vec2_1 = tempEventData.vec2_1;
					clientPacket.vec2_2 = tempEventData.vec2_2;
					clientPacket.timeStamp = static_cast<long>(frameCount);
					clientPacket.sequence = networkClientPacketCount++;
					snprintf(clientPacket.text, NET_TEXT_SIZE, "%s", tempEventData.eventString.c_str());    // Don't overflow char array
					haveSeenTraffic = true;
					net_sendClientPacket (clientPacket);
					break;

				case NETWORK_SEND_SYSTEM:
					clientPacket.packetType = NET_SYSTEM_PACKET;
					clientPacket.data1 = tempEventData.data1;
					net_sendClientPacket (clientPacket);
					break;

				default:
					break;
			}
		}
	}
	printf ("CLIENT thread stopped.\n");
	return 0;
}


//----------------------------------------------------------
//
// Thread to get the network client packets
int net_getNetworkClientPackets ( void *ptr )
//----------------------------------------------------------
{
	int packet_bytes;
	uint64_t packet_sequence;

	void *packet = netcode_client_receive_packet (networkClient, &packet_bytes, &packet_sequence);
	if ( !packet )
		return 0;

	assert(packet_bytes == sizeof (_networkPacket));

//	assert(memcmp (packet, &clientPacket, sizeof (_networkPacket)) == 0);
	networkClientPacketCount++;
	netcode_client_free_packet (networkClient, packet);

//	evt_sendEvent (USER_EVENT_NETWORK_CLIENT, NETWORK_RECEIVE, clientPacket.data1, clientPacket.data2, clientPacket.data3, clientPacket.vec2_1, clientPacket.vec2_2, "");

	return 0;
}

//----------------------------------------------------------------
//
// Create the thread to run the network server function.
bool net_createNetworkClientThread ()
//----------------------------------------------------------------
{
	if ( runNetworkClientThread )
	{
		con_print (CON_INFO, true, "Network client thread is already running.");
		return true;
	}

	networkClientThread = SDL_CreateThread (net_getNetworkClientPackets, "net_getNetworkClientPackets", (void *) nullptr);

	if ( nullptr == networkClientThread )
	{
		con_print (CON_ERROR, true, "SDL_CreateThread - net_getNetworkClientPackets  - failed : [ %s ]", SDL_GetError ());
		return false;
	}

	return true;
}

//----------------------------------------------------------------
//
// Start the network server thread running
bool net_startNetworkClientThread ()
//----------------------------------------------------------------
{
	userEventNetworClientThread = SDL_CreateThread (net_processNetworkClientQueue, "net_processNetworkClientQueue", (void *) nullptr);
	if ( nullptr == userEventNetworClientThread )
	{
		printf ("SDL_CreateThread - userEventNetworkClientThread - failed: %s\n", SDL_GetError ());
		return false;
	}

	networkClientMutex = SDL_CreateMutex ();
	if ( !networkClientMutex )
	{
		printf ("Couldn't create mutex - networkClientMutex");
		return false;
	}

	SDL_DetachThread (userEventNetworClientThread);

	if ( !runNetworkClientThread )     // Not currently running
	{
		if ( net_createNetworkClientThread ())
		{
			runNetworkClientThread = true;
			SDL_Delay(500);
			SDL_DetachThread (networkClientThread);
		}
	}

	return true;
}

//--------------------------------------------------------------------------------
//
// Create the network client
bool net_createNetworkClient(float time)
//--------------------------------------------------------------------------------
{
	netcode_client_config_t client_config;

	netcode_default_client_config (&client_config);
	networkClient = netcode_client_create ("::", &client_config, time);

	if ( !networkClient )
	{
		con_print(CON_ERROR, true, "Failed to create network client.");
		return false;
	}

	netcode_random_bytes ((uint8_t *) &networkClientID, 8);

	con_print(CON_INFO, true, "Network client id is %.16" PRIx64 "", networkClientID);

	if ( netcode_generate_connect_token (1, (NETCODE_CONST char **) &serverAddress, (NETCODE_CONST char **) &serverAddress, CONNECT_TOKEN_EXPIRY, CONNECT_TOKEN_TIMEOUT, networkClientID, PROTOCOL_ID,
	                                     private_key, connect_token) != NETCODE_OK )
	{
		con_print(CON_ERROR, true, "Failed to generate client connect token.");
		return false;
	}

	netcode_client_connect (networkClient, connect_token);

	net_startNetworkClientThread ();

//	networkClientKeepaliveCheck = SDL_AddTimer (1000, net_networkClientKeepaliveCallback, nullptr);   // Time in milliseconds

	return true;
}

//----------------------------------------------------------------
//
// Stop the network client thread running
void net_stopNetworkClientThread ()
//----------------------------------------------------------------
{
	runNetworkClientThread = false;
}

//----------------------------------------------------------------
//
// Shutdown the client instance and terminate netcode
void net_shutdownClient ()
//----------------------------------------------------------------
{
	netcode_client_destroy (networkClient);

	netcode_term ();

	net_stopNetworkClientThread ();
}

//--------------------------------------------------------------------------------
//
// Update the network client
void net_updateNetworkClient(float time)
//--------------------------------------------------------------------------------
{
	netcode_client_update (networkClient, time);
}



