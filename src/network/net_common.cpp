#include <hdr/game/gam_levels.h>
#include "hdr/system/sys_main.h"
#include "hdr/network/net_common.h"
#include "hdr/network/net_client.h"
#include "hdr/network/net_server.h"
#include "hdr/system/sys_events.h"

int         networkPacketCountSentClient = 0;
int         networkPacketCountSentServer = 0;
int         networkPacketCountReceiveServer = 0;

size_t      networkOutQueueSize = 0;

RakNet::RakPeerInterface *netClient = nullptr;
RakNet::RakPeerInterface *netServer = nullptr;

RakNet::Packet          netServerPacket;

bool            isServer = true;
bool            isClient = true;

std::string     serverName;
int             serverPort;

bool            serverRunning = false;
bool            clientRunning = false;

SDL_Thread      *userEventNetworkOutThread;

//-----------------------------------------------------------------------------
//
// Send the packet over the network
void net_sendPacket( RakNet::BitStream *bitStream, int packetSource, int whichClient )
//-----------------------------------------------------------------------------
{
	if ((clientRunning) && (serverRunning))
	{
		switch ( packetSource )
		{
			case USER_EVENT_NETWORK_FROM_CLIENT:        // From the client to the server

//				printf("Client sending to [ %s ]\n",  netServerPacket.systemAddress.ToString ( true ));

				netClient->Send ( bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, netClient->GetSystemAddressFromGuid (netServerPacket.guid), false );
				break;

			case NETWORK_SEND_DATA:             // From the server to the client

				netServer->Send (bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, netClientInfo[whichClient].systemAddress, false);
				break;

			default:
				printf ( "Unknown packet source.\n" );
				break;
		}
	}
}

//-----------------------------------------------------------------------------------------------------
//
// Start the server from the console
void net_consoleStartNetServer()
//-----------------------------------------------------------------------------------------------------
{
	if (!net_startServer ( serverName, serverPort, static_cast<unsigned short>(maxNumClients)))
	{
		con_print(CON_ERROR, true, "Unable to start the server.");
		serverRunning = false;
		return;
	}
	serverRunning = true;
}

//-----------------------------------------------------------------------------------------------------
//
// Connect to the server
void net_consoleStartNetClient()
//-----------------------------------------------------------------------------------------------------
{
	net_startClient (serverPort);

	clientRunning = true;


//	for (int i = 0; i != 5; i++)
	{
		SDL_Delay(500);

		if ( !net_clientConnectTo ( serverName, serverPort ))
		{
			con_print ( CON_ERROR, true, "Unable to connect to the server." );
			clientRunning = false;
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------------
// If the first byte is ID_TIMESTAMP, then we want the 5th byte
// Otherwise we want the 1st byte
unsigned char net_getPacketIdentifier( RakNet::Packet *p )
//-----------------------------------------------------------------------------------------------------
{
	if (p == nullptr)
		return 255;

	if (p->data[0] == ID_TIMESTAMP)
	{
		printf("First byte is a ID_TIMESTAMP\n");

		RakAssert(p->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	else
		return p->data[0];
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return True / False on success or not
//
// Start the network library thread and mutex
bool net_initLibrary ()
//-----------------------------------------------------------------------------------------------------
{

	userEventNetworkOutThread = SDL_CreateThread (net_processNetworkTraffic, "net_processNetworkTraffic", (void *) nullptr);
	if ( nullptr == userEventNetworkOutThread )
	{
		printf ("SDL_CreateThread - userEventNetworkClientThread - failed: %s\n", SDL_GetError ());
		return false;
	}

	networkOutMutex = SDL_CreateMutex ();
	if ( !networkOutMutex )
	{
		printf ("Couldn't create mutex - networkOutMutex");
		return false;
	}

	SDL_DetachThread (userEventNetworkOutThread);     // Thread to push packets IN

	return true;
}

//-----------------------------------------------------------------------------------------------------
//
// Handle all the network IN actvity here
int net_processNetworkTraffic( void *ptr )
//-----------------------------------------------------------------------------------------------------
{
	RakNet::Packet*         p;
	RakNet::RakString       rs;

	unsigned char           packetIdentifier;
	_netClientInfo          tempNetClientInfo;

	int dataType;
	float varX, varY, varZ;


	// Record the first client that connects to us so we can pass it to the ping function
	RakNet::SystemAddress clientID = RakNet::UNASSIGNED_SYSTEM_ADDRESS;

	while ( runThreads )
	{
		SDL_Delay (THREAD_DELAY_MS);

		if (clientRunning)
		{
			for ( p = netClient->Receive (); p; netClient->DeallocatePacket ( p ), p = netClient->Receive ())
			{
				// We got a packet, get the identifier with our handy function
				packetIdentifier = net_getPacketIdentifier ( p );

				// Check if this is a network message packet
				switch ( packetIdentifier )
				{
					case ID_DISCONNECTION_NOTIFICATION:
						// Connection lost normally
						printf ( "ID_DISCONNECTION_NOTIFICATION\n" );
						break;
					case ID_ALREADY_CONNECTED:
						// Connection lost normally
						printf ( "ID_ALREADY_CONNECTED with guid %" PRINTF_64_BIT_MODIFIER "u\n", p->guid );
						break;
					case ID_INCOMPATIBLE_PROTOCOL_VERSION:
						printf ( "ID_INCOMPATIBLE_PROTOCOL_VERSION\n" );
						break;
					case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
						printf ( "ID_REMOTE_DISCONNECTION_NOTIFICATION\n" );
						break;
					case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
						printf ( "ID_REMOTE_CONNECTION_LOST\n" );
						break;
					case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
						printf ( "ID_REMOTE_NEW_INCOMING_CONNECTION\n" );
						break;
					case ID_CONNECTION_BANNED: // Banned from this server
						printf ( "We are banned from this server.\n" );
						break;
					case ID_CONNECTION_ATTEMPT_FAILED:
						printf ( "Connection attempt failed\n" );
						break;
					case ID_NO_FREE_INCOMING_CONNECTIONS:
						// Sorry, the server is full.  I don't do anything here but
						// A real app should tell the user
						printf ( "ID_NO_FREE_INCOMING_CONNECTIONS\n" );
						break;

					case ID_INVALID_PASSWORD:
						printf ( "ID_INVALID_PASSWORD\n" );
						break;

					case ID_CONNECTION_LOST:
						// Couldn't deliver a reliable packet - i.e. the other system was abnormally
						// terminated
						printf ( "ID_CONNECTION_LOST\n" );
						break;

					case ID_CONNECTION_REQUEST_ACCEPTED:
						// This tells the client they have connected
						printf ( "ID_CONNECTION_REQUEST_ACCEPTED to %s with GUID %s\n",
						         p->systemAddress.ToString ( true ), p->guid.ToString ());
						printf ( "My external address is %s\n",
						         netClient->GetExternalID ( p->systemAddress ).ToString ( true ));

						netServerPacket.systemAddress = p->systemAddress;
						netServerPacket.guid = p->guid;
						break;

					case ID_CONNECTED_PING:
					case ID_UNCONNECTED_PING:
						printf ( "Ping from %s\n", p->systemAddress.ToString ( true ));
						break;

					case ID_UNCONNECTED_PONG:
						printf("Got pong from %s \n", p->systemAddress.ToString());
						break;


					case ID_GAME_MESSAGE_1:
					{
						RakNet::BitStream bsin ( p->data, p->length, false );

						bsin.IgnoreBytes ( sizeof ( RakNet::MessageID ));
						bsin.Read ( dataType );
						if ( dataType == NET_DROID_WORLDPOS )
						{
							for ( int index = 0;
							      index != levelInfo.at ( lvl_getCurrentLevelName ()).numDroids; index++ )
							{
								bsin.ReadVector ( varX, varY, varZ );
								levelInfo.at ( lvl_getCurrentLevelName ()).droid[index].serverWorldPos.x = varX;
								levelInfo.at ( lvl_getCurrentLevelName ()).droid[index].serverWorldPos.y = varY;
							}
						}
					}
						break;

					default:
						// It's a client, so just show the message
						printf ( "%s\n", p->data );
						break;
				}
			}
		}
		//
		// Check Server IN packets
		//
		if ( serverRunning )
		{
			// Now process SERVER
			for ( p = netServer->Receive (); p; netServer->DeallocatePacket ( p ), p = netServer->Receive ())
			{
				networkPacketCountReceiveServer++;

				// We got a packet, get the identifier with our handy function
				packetIdentifier = net_getPacketIdentifier ( p );

				printf ( "Server: Got Network traffic\n" );

				// Check if this is a network message packet
				switch ( packetIdentifier )
				{
					case ID_DISCONNECTION_NOTIFICATION:
						// Connection lost normally
						printf ( "ID_DISCONNECTION_NOTIFICATION from %s\n", p->systemAddress.ToString ( true ));;
						break;

					case ID_NEW_INCOMING_CONNECTION:
						// Somebody connected.  We have their IP now

						tempNetClientInfo.systemAddress = p->systemAddress;
						tempNetClientInfo.GUID = p->guid;
						tempNetClientInfo.packetSequenceCount = 0;
						sprintf(tempNetClientInfo.name, "%s", "No Name");

						netClientInfo.push_back(tempNetClientInfo);

						printf ( "ID_NEW_INCOMING_CONNECTION from %s with GUID %s\n", netClientInfo[0].systemAddress.ToString ( true ), netClientInfo[0].GUID.ToString ());

						printf ( "Remote internal IDs:\n" );
						for ( int index = 0; index < MAXIMUM_NUMBER_OF_INTERNAL_IDS; index++ )
						{
							RakNet::SystemAddress internalId = netServer->GetInternalID ( p->systemAddress, index );
							if ( internalId != RakNet::UNASSIGNED_SYSTEM_ADDRESS )
							{
								printf ( "%i. %s\n", index + 1, internalId.ToString ( true ));
							}
						}
						break;

					case ID_INCOMPATIBLE_PROTOCOL_VERSION:
						printf ( "ID_INCOMPATIBLE_PROTOCOL_VERSION\n" );
						break;

					case ID_CONNECTED_PING:
					case ID_UNCONNECTED_PING:
						printf ( "Ping from %s\n", p->systemAddress.ToString ( true ));
						break;

					case ID_CONNECTION_LOST:
						// Couldn't deliver a reliable packet - i.e. the other system was abnormally
						// terminated
						printf ( "ID_CONNECTION_LOST from %s\n", p->systemAddress.ToString ( true ));;
						break;

					case ID_GAME_MESSAGE_1:
					{
//						printf ( "Message from client\n" );

						RakNet::BitStream bsin(p->data,p->length, false);

						bsin.IgnoreBytes(sizeof(RakNet::MessageID));
						bsin.Read ( rs );

//						printf ( "Client says [ %s ]\n", rs.C_String ());
					}
						break;

					default:
						break;
				}
			}
		}
	}
	printf ("NETWORK IN thread stopped.\n");
	return 0;
}


//-----------------------------------------------------------------------------
//
// Thread function to process the network OUT packets
//
// This function sends packets OUT from both client and server
int net_processNetworkOutQueue ( void *ptr )
//-----------------------------------------------------------------------------
{
	_myEventData tempEventData;

	RakNet::BitStream   BSOut{};

//	while ( runThreads )
	{
//		SDL_Delay (THREAD_DELAY_MS);

		networkOutQueueSize = networkOutQueue.size();

		while ( !networkOutQueue.empty ())   // stuff in the queue to process
		{
			if ( SDL_LockMutex (networkOutMutex) == 0 )
			{
				tempEventData = networkOutQueue.front ();       // back - get the latest one - not the earliest one
				networkOutQueue.pop ();
				SDL_UnlockMutex (networkOutMutex);
			}

			switch ( tempEventData.eventAction )
			{
				case USER_EVENT_NETWORK_OUT:

//					printf ("SERVER is sending out packets - queue [ %i ].\n", networkOutQueue.size());

//					net_sendPacket ( clientPacket, USER_EVENT_NETWORK_OUT, tempEventData.data2 );

					break;

				case USER_EVENT_NETWORK_FROM_CLIENT:
					printf ("CLIENT is sending out packets - queue [ %lu ].\n", networkOutQueue.size ());

					BSOut.Write ((RakNet::MessageID)ID_GAME_MESSAGE_1);
					BSOut.Write ("Hello from the client");

					net_sendPacket ( &BSOut, USER_EVENT_NETWORK_FROM_CLIENT, 0 );

//					enetClientInfo[tempEventData.data2].packetSequenceCount++;      // Client Index
					break;

				default:
					break;  // Unknown packet type
			}
		}
	}

//	printf ("NETWORK OUT thread stopped.\n");
	return 0;
}
