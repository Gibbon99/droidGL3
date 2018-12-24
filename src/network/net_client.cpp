#include "hdr/network/net_client.h"
#include "hdr/network/net_common.h"
#include <hdr/network/raknet/RakNetTypes.h>

bool                    haveServerAddress;     // Do we know who to connect to
bool                    haveServerConnection;  // Have we connected to the server
SDL_TimerID             net_clientConnectionTimer;
std::string             serverName;             // Name / address of the discovered server

//------------------------------------------------------------------------
//
// Got a connection accepted from the server
void net_clientConnectionAccepted(RakNet::SystemAddress serverAddressReceived, RakNet::RakNetGUID clientGUID)
//------------------------------------------------------------------------
{


	if (!haveServerConnection)
	{
		net_startConnectionToServer ( USER_EVENT_TIMER_OFF );  // Don't need the timer anymore
		haveServerConnection = true;


		printf ( "ID_CONNECTION_REQUEST_ACCEPTED to %s with GUID %s\n", serverAddressReceived.ToString ( true ),
		         clientGUID.ToString ());
		printf ( "My external address is %s\n", serverAddressReceived.ToString ( true ));
	}
}

//------------------------------------------------------------------------
//
// Got a PONG response from the server
void net_clientGotPong(RakNet::SystemAddress serverAddressReceived)
//------------------------------------------------------------------------
{
	if (!haveServerAddress)
	{
		haveServerAddress = true;

		serverName = serverAddressReceived.ToString(false);     // Don't print port
	}
}

//-----------------------------------------------------------------------------------------------------
//
// Send an unconnected broadcast ping to locate a server
void net_clientSendDiscoverPing(unsigned short serverPort)
//-----------------------------------------------------------------------------------------------------
{
	netClient->Ping( "255.255.255.255", serverPort, false);
	printf("Pinging to locate server on port [ %i ]\n", serverPort);
}

//------------------------------------------------------------------------
//
// Callback for timer function - button focus animate flash
Uint32 net_clientConnectionCallback ( Uint32 interval, void *param )
//------------------------------------------------------------------------
{
	if (!haveServerAddress)
	{
		net_clientSendDiscoverPing ( static_cast<unsigned short>(serverPort));
		return interval;
	}

	if (!haveServerConnection)
	{
		net_clientConnectTo( serverName, static_cast<unsigned short>(serverPort));
		return interval;
	}

	return interval;
}

//------------------------------------------------------------------------
//
// Call a timer to start broadcast pinging looking for a server
// Response will be received on the network IN thread
void net_startConnectionToServer(int newState)
//------------------------------------------------------------------------
{
	switch ( newState )
	{
		case USER_EVENT_TIMER_OFF:
		{
			evt_removeTimer(net_clientConnectionTimer);
			net_clientConnectionTimer = 0;
			break;
		}

		case USER_EVENT_TIMER_ON:
		{
			net_clientConnectionTimer = evt_registerTimer(1000, net_clientConnectionCallback, "Network client discovery");
			break;
		}

		default:
			break;
	}
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
bool net_startClient(int serverPort)
//-----------------------------------------------------------------------------------------------------
{
	RakNet::SocketDescriptor socketDescriptor (serverPort + 1, 0);

	netClient = RakNet::RakPeerInterface::GetInstance ();

	socketDescriptor.socketFamily = AF_INET;    // Only IPv4 supports broadcast on 255.255.255.255
	netClient->Startup (1, &socketDescriptor, 1);
	netClient->SetOccasionalPing(true);

	printf("Client started on [ %s : %i ]\n", netClient->GetExternalID (RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString(), serverPort + 1);

	return true;
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
bool net_clientConnectTo( const string &serverName, unsigned short serverPort )
//-----------------------------------------------------------------------------------------------------
{

	printf("CLIENT: Connect to [ %s : %i ]\n", serverName.c_str(), serverPort);

	RakNet::ConnectionAttemptResult car = netClient->Connect( serverName.c_str(), static_cast<unsigned short>(serverPort), connectionPassword.c_str(), connectionPassword.length());
//	RakNet::ConnectionAttemptResult car = netClient->Connect( serverName.c_str(), static_cast<unsigned short>(serverPort), 0, 0);
	RakAssert( car == RakNet::CONNECTION_ATTEMPT_STARTED);

	unsigned int i;
	for (i=0; i < netClient->GetNumberOfAddresses(); i++)
	{
		printf("Client IP Address [ %i. %s ]\n", i+1, netClient->GetLocalIP(i));
	}

	con_print(CON_INFO, true, "GUID is %s", netClient->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString());

	return true;
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void net_shutdownClient()
//-----------------------------------------------------------------------------------------------------
{
	if (clientRunning)
	{
		// Be nice and let the server know we quit.
		netClient->Shutdown ( 300 );

		// We're done with the network
		RakNet::RakPeerInterface::DestroyInstance ( netClient );
	}
}

//--------------------------------------------------------------------------------
//
// Send the current level to the server for this client
void net_sendCurrentLevel(string whichLevel)
//--------------------------------------------------------------------------------
{
	evt_sendEvent (USER_EVENT_NETWORK_OUT, USER_EVENT_NETWORK_FROM_CLIENT, NET_CLIENT_CURRENT_LEVEL, 0, 0, glm::vec2(), glm::vec2(), whichLevel);
}
