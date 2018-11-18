//#include "hdr/io/minilzo/lzoconf.h"
#include "hdr/io/minilzo/minilzo.h"
#include "hdr/network/net_client.h"


#include <string.h>

uint64_t                networkClientID = 0;
netcode_client_t        *networkClient;
int                     networkClientIndexOnServer;
int                     networkClientCurrentState = -100;
bool                    networkClientIsRunning = false;

SDL_Thread              *networkClientThread;
SDL_Thread              *userEventNetworClientThread;

SDL_TimerID             networkClientKeepaliveCheck;
SDL_TimerID             networkClientStatusCheck;

int                     networkClientPacketCount = 0;

bool                    runNetworkClientThread = true;

bool                    haveSeenTraffic = false;

uint8_t                 connect_token[NETCODE_CONNECT_TOKEN_BYTES];


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
		evt_sendEvent (USER_EVENT_NETWORK_FROM_CLIENT, NETWORK_SEND_SYSTEM, NET_SYS_KEEPALIVE, networkClientID, 0, glm::vec2(), glm::vec2(), "");

		printf("Client sent KEEP ALIVE packet.\n");
	}
	return interval;
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

			switch ( tempEventData.eventAction )
			{
				case NET_STATUS:    // Client network state has changed
					net_networkClientGetState (tempEventData.data1 );
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
					clientPacket.packetOwner = networkClientIndexOnServer;
					snprintf(clientPacket.text, NET_TEXT_SIZE, "%s", tempEventData.eventString.c_str());    // Don't overflow char array

					haveSeenTraffic = true;

                    if (networkClientCurrentState == NETCODE_CLIENT_STATE_CONNECTED)
	                    net_sendPacket (clientPacket, USER_EVENT_NETWORK_FROM_CLIENT, networkClientIndexOnServer );
                    else
						evt_sendEvent (USER_EVENT_NETWORK_FROM_CLIENT, NETWORK_SEND_DATA, tempEventData.data1, tempEventData.data2, tempEventData.data3, tempEventData.vec2_1, tempEventData.vec2_2, tempEventData.eventString);
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


//----------------------------------------------------------
//
// Thread to get the network client packets
//
// This function gets packets IN from server and puts them onto the
// client event queue
int net_getNetworkClientPackets ( void *ptr )
//----------------------------------------------------------
{
	int             packet_bytes;
	uint64_t        packet_sequence;
	uint8_t         *packet;
	_networkPacket  *clientPacket;
	lzo_uint        newLength;
	int             result;
	char            inPacketPtr[sizeof (_networkPacket) * 2]; // Make sure there is enough space to hold decompressed packet

	while (runNetworkClientThread )
	{
		packet = netcode_client_receive_packet (networkClient, &packet_bytes, &packet_sequence);

		if ( packet != nullptr )
		{
			// Got a legitimate packet - now decompress it
			// TODO: Work out compression amount - percentage compressed??
			result = lzo1x_decompress (packet, static_cast<lzo_uint>(packet_bytes), (unsigned char *) &inPacketPtr, &newLength, nullptr);
			if ( result != LZO_E_OK )
			{
				con_print (CON_ERROR, true, "Internal error - server network packet decompression failed [ %i ]. Packet dropped.", result);
				netcode_client_free_packet (networkClient, packet);
			}
			else
			{
				// Cast to our packet structure
				clientPacket = (_networkPacket *) &inPacketPtr;
				clientPacket->packetOwner = -1;     // From Server

				evt_sendEvent (USER_EVENT_CLIENT_EVENT, clientPacket->packetType, clientPacket->data1, clientPacket->data2, clientPacket->data3, clientPacket->vec2_1, clientPacket->vec2_2,
				               clientPacket->text);

				//assert(packet_bytes == sizeof (_networkPacket));

				networkClientPacketCount++;
				netcode_client_free_packet (networkClient, packet);
			}
		}
	}
	printf ("CLIENT NETWORK packet thread finished.\n");
	return 0;
}

//----------------------------------------------------------------
//
// Create the thread to run the network client function.
bool net_createNetworkClientThread ()
//----------------------------------------------------------------
{
	/*
	if ( runNetworkClientThread )
	{
		con_print (CON_INFO, true, "Network client thread is already running.");
		return true;
	}
*/
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

	networkClientOutMutex = SDL_CreateMutex ();
	if ( !networkClientOutMutex )
	{
		printf ("Couldn't create mutex - networkClientOutMutex");
		return false;
	}

	SDL_DetachThread (userEventNetworClientThread);

//	if ( !runNetworkClientThread )     // Not currently running
	{
		if ( net_createNetworkClientThread ())
		{
			runNetworkClientThread = true;
			SDL_DetachThread (networkClientThread);
		}
	}

	return true;
}

//----------------------------------------------------------------
//
// Get the state of the client
void net_networkClientGetState ( int clientState )
//----------------------------------------------------------------
{
	switch ( clientState )
	{
		case NETCODE_CLIENT_STATE_CONNECT_TOKEN_EXPIRED:
			con_print (CON_INFO, true, "Client state [ %s ]", "NETCODE_CLIENT_STATE_CONNECT_TOKEN_EXPIRED");
			break;

		case NETCODE_CLIENT_STATE_INVALID_CONNECT_TOKEN:
			con_print (CON_INFO, true, "Client state [ %s ]", "NETCODE_CLIENT_STATE_INVALID_CONNECT_TOKEN");
			break;

		case NETCODE_CLIENT_STATE_CONNECTION_TIMED_OUT:
			con_print (CON_INFO, true, "Client state [ %s ]", "NETCODE_CLIENT_STATE_CONNECTION_TIMED_OUT");
			break;

		case NETCODE_CLIENT_STATE_CONNECTION_RESPONSE_TIMED_OUT:
			con_print (CON_INFO, true, "Client state [ %s ]", "NETCODE_CLIENT_STATE_CONNECTION_RESPONSE_TIMED_OUT");
			break;

		case NETCODE_CLIENT_STATE_CONNECTION_REQUEST_TIMED_OUT:
			con_print (CON_INFO, true, "Client state [ %s ]", "NETCODE_CLIENT_STATE_CONNECTION_REQUEST_TIMED_OUT");
			break;

		case NETCODE_CLIENT_STATE_CONNECTION_DENIED:
			con_print (CON_INFO, true, "Client state [ %s ]", "NETCODE_CLIENT_STATE_CONNECTION_DENIED");
			break;

		case NETCODE_CLIENT_STATE_DISCONNECTED:
			con_print (CON_INFO, true, "Client state [ %s ]", "NETCODE_CLIENT_STATE_DISCONNECTED");
			break;

		case NETCODE_CLIENT_STATE_SENDING_CONNECTION_REQUEST:
			con_print (CON_INFO, true, "Client state [ %s ]", "NETCODE_CLIENT_STATE_SENDING_CONNECTION_REQUEST");
			break;

		case NETCODE_CLIENT_STATE_SENDING_CONNECTION_RESPONSE:
			con_print (CON_INFO, true, "Client state [ %s ]", "NETCODE_CLIENT_STATE_SENDING_CONNECTION_RESPONSE");
			break;

		case NETCODE_CLIENT_STATE_CONNECTED:
			con_print (CON_INFO, true, "Client state [ %s ]", "NETCODE_CLIENT_STATE_CONNECTED");
			break;

		default:
			break;  // Unknown state
	}
}

//--------------------------------------------------------------------------------
//
// Called from SDL Timer - check the client status and push any changes as an event
Uint32 net_clientCheckStatus( Uint32 interval, void *paramm)
//--------------------------------------------------------------------------------
{
	int newStatus;

	newStatus = netcode_client_state (networkClient);

	if ( networkClientCurrentState != newStatus)
	{
		networkClientCurrentState = newStatus;
		evt_sendEvent (USER_EVENT_NETWORK_FROM_CLIENT, NET_STATUS, newStatus, static_cast<int>(networkClientID), 0, glm::vec2 (), glm::vec2 (), "");
	}
	return interval;
}

//--------------------------------------------------------------------------------
//
// Create the network client
bool net_createNetworkClient(float time)
//--------------------------------------------------------------------------------
{
	netcode_client_config_t client_config{};

	char *clientAddress;

	clientAddress = (char *)serverAddress.c_str();

	netcode_default_client_config (&client_config);
	networkClient = netcode_client_create ("127.0.0.1", &client_config, time);

	if ( !networkClient )
	{
		con_print(CON_ERROR, true, "Failed to create network client.");
		return false;
	}

	netcode_random_bytes ((uint8_t *) &networkClientID, 8);

	con_print(CON_INFO, true, "Network client id is %.16" PRIx64 "", networkClientID);

	if ( netcode_generate_connect_token (1, (NETCODE_CONST char **) &clientAddress, (NETCODE_CONST char **) &clientAddress, CONNECT_TOKEN_EXPIRY, CONNECT_TOKEN_TIMEOUT, networkClientID, PROTOCOL_ID,
	                                     private_key, connect_token) != NETCODE_OK )
	{
		con_print(CON_ERROR, true, "Failed to generate client connect token.");
		return false;
	}

	netcode_client_connect (networkClient, connect_token);

	net_startNetworkClientThread ();

	networkClientKeepaliveCheck = SDL_AddTimer (1000, net_networkClientKeepaliveCallback, nullptr);   // Time in milliseconds

	networkClientStatusCheck = SDL_AddTimer (500, net_clientCheckStatus, nullptr);

	networkClientIndexOnServer = netcode_client_index(networkClient);

	con_print(CON_INFO, true, "Client index [ %i ]", netcode_client_index (networkClient ));

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

//--------------------------------------------------------------------------------
//
// Send the current level to the server for this client
void net_sendCurrentLevel(string whichLevel)
//--------------------------------------------------------------------------------
{
	evt_sendEvent (USER_EVENT_NETWORK_FROM_CLIENT, NETWORK_SEND_DATA, NET_CLIENT_CURRENT_LEVEL, networkClientIndexOnServer, 0, glm::vec2(), glm::vec2(), whichLevel);
}