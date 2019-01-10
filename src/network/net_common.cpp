#include <hdr/game/gam_levels.h>
#include <hdr/game/gam_eventsClient.h>
#include "hdr/system/sys_main.h"
#include "hdr/network/net_common.h"
#include "hdr/network/net_client.h"
#include "hdr/network/net_server.h"
#include "hdr/system/sys_events.h"

int networkPacketCountSentClient = 0;
int networkPacketCountSentServer = 0;
int networkPacketCountReceiveServer = 0;

size_t networkOutQueueSize = 0;

RakNet::RakPeerInterface *netClient = nullptr;
RakNet::RakPeerInterface *netServer = nullptr;

RakNet::Packet netServerPacket;

std::string connectionPassword;

bool isServer = true;
//bool            isClient = true;

int serverPort;

bool serverRunning = false;
bool clientRunning = false;
bool clientConnected = false;

SDL_Thread *userEventNetworkInThread;

//-----------------------------------------------------------------------------
//
// Send the packet over the network
void net_sendPacket ( RakNet::BitStream *bitStream, int packetSource, int whichClient )
//-----------------------------------------------------------------------------
{
	unsigned long clientIndex = 0;

	switch ( packetSource )
	{
		case USER_EVENT_NETWORK_FROM_CLIENT:        // From the client to the server
			if ( clientRunning )
			{
				netClient->Send ( bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, netClient->GetSystemAddressFromGuid ( netServerPacket.guid ), false );
				networkPacketCountSentClient++;
			}
			break;

		case NETWORK_SEND_DATA:             // From the server to the client
			if ( serverRunning )
			{
				if ( -1 == whichClient )      // to all clients
				{
					netServer->Send ( bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_RAKNET_GUID, true );    // Broadcast to all connected clients
				}
				else
				{
					if ( netClientInfo[whichClient].inUse )
					{
						if ( netServer->GetConnectionState ( netClientInfo[whichClient].systemAddress ) ==
						     RakNet::IS_CONNECTED )
						{
							netServer->Send ( bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, netClientInfo[whichClient].systemAddress, false );
							networkPacketCountSentServer++;
						}
					}
				}
			}
			break;

		default:
			printf ( "Unknown packet source.\n" );
			break;
	}
}

//-----------------------------------------------------------------------------------------------------
// If the first byte is ID_TIMESTAMP, then we want the 5th byte
// Otherwise we want the 1st byte
unsigned char net_getPacketIdentifier ( RakNet::Packet *p )
//-----------------------------------------------------------------------------------------------------
{
	if ( p == nullptr )
		return 255;

	if ( p->data[0] == ID_TIMESTAMP )
	{
		printf ( "First byte is a ID_TIMESTAMP\n" );

		RakAssert( p->length > sizeof ( RakNet::MessageID ) + sizeof ( RakNet::Time ));
		return p->data[sizeof ( RakNet::MessageID ) + sizeof ( RakNet::Time )];
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
	connectionPassword = "ParaDroid";

	// Not running by default
	serverRunning = false;
	//
	// See if we are even a server
	if ( isServer )
	{
		if ( !net_startServer ( static_cast<unsigned short>(serverPort), static_cast<unsigned short>(maxNumClients)))
			return false;
		// Yes we are, and we are now listening
		serverRunning = true;
	}

	clientRunning = false;
	// We will always have a client ( local for a single player game )
	if ( !net_startClient ( serverPort ))
		return false;
	clientRunning = true;
	clientConnected = false;

	//
	// Now start the listening thread
	userEventNetworkInThread = SDL_CreateThread ( net_processNetworkTraffic, "net_processNetworkTraffic", (void *) nullptr );
	if ( nullptr == userEventNetworkInThread )
	{
		printf ( "SDL_CreateThread - userEventNetworkClientThread - failed: %s\n", SDL_GetError ());
		return false;
	}

	networkInMutex = SDL_CreateMutex ();
	if ( !networkInMutex )
	{
		printf ( "Couldn't create mutex - networkInMutex" );
		return false;
	}

	SDL_DetachThread ( userEventNetworkInThread );     // Thread to receive packets IN bound

	return true;
}

//-----------------------------------------------------------------------------------------------------
//
// Handle all the network IN activity here
int net_processNetworkTraffic ( void *ptr )
//-----------------------------------------------------------------------------------------------------
{
	RakNet::Packet *p;
	RakNet::RakString rs;

//	RakNet::Packet sendPacket;

	unsigned char packetIdentifier;
	_netClientInfo tempNetClientInfo;

	// Record the first client that connects to us so we can pass it to the ping function
	RakNet::SystemAddress clientID = RakNet::UNASSIGNED_SYSTEM_ADDRESS;

	while ( runThreads )
	{
		SDL_Delay ( THREAD_DELAY_MS );

		if ( clientRunning )
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
#ifdef NET_DEBUG
						printf ( "CLIENT: ID_DISCONNECTION_NOTIFICATION\n" );
#endif
						break;
					case ID_ALREADY_CONNECTED:
						// Connection lost normally
#ifdef NET_DEBUG
						printf ( "CLIENT: ID_ALREADY_CONNECTED with guid %" PRINTF_64_BIT_MODIFIER "u\n", p->guid );
#endif
						break;
					case ID_INCOMPATIBLE_PROTOCOL_VERSION:
#ifdef NET_DEBUG
						printf ( "CLIENT: ID_INCOMPATIBLE_PROTOCOL_VERSION\n" );
#endif
						break;
					case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
#ifdef NET_DEBUG
						printf ( "CLIENT: ID_REMOTE_DISCONNECTION_NOTIFICATION\n" );
#endif
						break;
					case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
#ifdef NET_DEBUG
						printf ( "CLIENT: ID_REMOTE_CONNECTION_LOST\n" );
#endif
						break;
					case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
#ifdef NET_DEBUG
						printf ( "CLIENT: ID_REMOTE_NEW_INCOMING_CONNECTION\n" );
#endif
						break;
					case ID_CONNECTION_BANNED: // Banned from this server
#ifdef NET_DEBUG
						printf ( "CLIENT: We are banned from this server.\n" );
#endif
						break;
					case ID_CONNECTION_ATTEMPT_FAILED:
#ifdef NET_DEBUG
						printf ( "CLIENT: Connection attempt failed\n" );
#endif
						break;
					case ID_NO_FREE_INCOMING_CONNECTIONS:
						// Sorry, the server is full.  I don't do anything here but
						// A real app should tell the user
#ifdef NET_DEBUG
						printf ( "CLIENT: ID_NO_FREE_INCOMING_CONNECTIONS\n" );
#endif
						break;

					case ID_INVALID_PASSWORD:
#ifdef NET_DEBUG
						printf ( "CLIENT: ID_INVALID_PASSWORD\n" );
#endif
						break;

					case ID_CONNECTION_LOST:
#ifdef NET_DEBUG
						// Couldn't deliver a reliable packet - i.e. the other system was abnormally
						// terminated
						printf ( "CLIENT: ID_CONNECTION_LOST\n" );
#endif
						break;

					case ID_CONNECTION_REQUEST_ACCEPTED:
						// This tells the client they have connected

						net_clientConnectionAccepted ( p->systemAddress, p->guid );

						netServerPacket.systemAddress = p->systemAddress;
						netServerPacket.guid = p->guid;
						break;

					case ID_CONNECTED_PING:
					case ID_UNCONNECTED_PING:
#ifdef NET_DEBUG
						printf ( "CLIENT: Ping from %s\n", p->systemAddress.ToString ( true ));
#endif
						break;

					case ID_UNCONNECTED_PONG:
						net_clientGotPong ( p->systemAddress );
#ifdef NET_DEBUG
						printf ( "CLIENT: Got pong from %s \n", p->systemAddress.ToString ());
#endif
						break;

					case ID_GAME_MESSAGE_1:
					{
						RakNet::Packet passPacket;

						passPacket = *p;    // Does this work - does it copy the data* from the packet??
						//
						// and pass it by value so we can work with it
						gam_handleClientInPacket ( passPacket );
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
#ifdef NET_DEBUG
						printf ( "SERVER: ID_DISCONNECTION_NOTIFICATION from %s\n", p->systemAddress.ToString ( true ));;
#endif
						break;

					case ID_NEW_INCOMING_CONNECTION:
						// Somebody connected.  We have their IP now

						net_addNewClient ( p->systemAddress, p->guid );
						break;

					case ID_INCOMPATIBLE_PROTOCOL_VERSION:
#ifdef NET_DEBUG
						printf ( "SERVER: ID_INCOMPATIBLE_PROTOCOL_VERSION\n" );
#endif
						break;

					case ID_CONNECTED_PING:
#ifdef NET_DEBUG
						printf ( "SERVER: Ping from %s\n", p->systemAddress.ToString ( true ));
#endif
						break;

					case ID_UNCONNECTED_PING:
#ifdef NET_DEBUG
						printf ( "SERVER: Unconnected discovery Ping from %s\n", p->systemAddress.ToString ( true ));
#endif
						break;

					case ID_CONNECTION_LOST:
						// Couldn't deliver a reliable packet - i.e. the other system was abnormally
						// terminated
#ifdef NET_DEBUG
						printf ( "SERVER: ID_CONNECTION_LOST from %s\n", p->systemAddress.ToString ( true ));
#endif
						//
						// TODO: Remove client from vector array based on systemAddress
						break;

					case ID_GAME_MESSAGE_1:
					{
//						printf ( "Message from client\n" );

						RakNet::BitStream bsin ( p->data, p->length, false );

						bsin.IgnoreBytes ( sizeof ( RakNet::MessageID ));
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
	printf ( "NETWORK IN thread stopped.\n" );
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

	RakNet::BitStream BSOut{};

//	while ( runThreads )
	{
//		SDL_Delay (THREAD_DELAY_MS);

		networkOutQueueSize = networkOutQueue.size ();

		while ( !networkOutQueue.empty ())   // stuff in the queue to process
		{
			if ( SDL_LockMutex ( networkInMutex ) == 0 )
			{
				tempEventData = networkOutQueue.front ();       // back - get the latest one - not the earliest one
				networkOutQueue.pop ();
				SDL_UnlockMutex ( networkInMutex );
			}

			switch ( tempEventData.eventAction )
			{
				case USER_EVENT_NETWORK_OUT:

//					printf ("SERVER is sending out packets - queue [ %i ].\n", networkOutQueue.size());

//					net_sendPacket ( clientPacket, USER_EVENT_NETWORK_OUT, tempEventData.data2 );

					break;

				case USER_EVENT_NETWORK_FROM_CLIENT:
					printf ( "CLIENT is sending out packets - queue [ %lu ].\n", networkOutQueue.size ());

					BSOut.Write ((RakNet::MessageID) ID_GAME_MESSAGE_1 );
					BSOut.Write ( "Hello from the client" );

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

//-----------------------------------------------------------------------------
//
// Console command to show the list of connected clients, if this is the server
void con_listClients ()
//-----------------------------------------------------------------------------
{
	RakNet::ConnectionState isConnected;

	if ( !isServer )
	{
		con_print ( CON_ERROR, true, "You are not the server." );
		return;
	}

	for ( auto &it : netClientInfo )
	{
		isConnected = netServer->GetConnectionState ( it.systemAddress );

		con_print ( CON_INFO, true, "[ %s ] - [ %s ] Connected [ %s ] Avg Ping [ %i ms ]", it.name, it.systemAddress.ToString (),
		            isConnected == RakNet::IS_CONNECTED ? "true"
		                                                : "false", netServer->GetAveragePing ( it.systemAddress ));
	}
}
