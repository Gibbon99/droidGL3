#include "hdr/network/net_client.h"
#include "hdr/network/net_common.h"
#include <string.h>

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
//	socketDescriptor.port = serverPort;
//	socketDescriptor.hostAddress = "10.1.1.30";
	netClient->Startup (1, &socketDescriptor, 1);
	netClient->SetOccasionalPing(true);

	return true;
}

//-----------------------------------------------------------------------------------------------------
//
// Send an unconnected broadcast ping to locate a server
void net_clientSendDiscoverPing(unsigned short serverPort)
//-----------------------------------------------------------------------------------------------------
{
	netClient->Ping( "255.255.255.255", serverPort, false);
	printf("Pinging to locate server.\n");
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

//	RakNet::ConnectionAttemptResult car = netClient->Connect( serverName.c_str(), static_cast<unsigned short>(serverPort), "Rumpelstiltskin", (int) strlen( "Rumpelstiltskin"));
	RakNet::ConnectionAttemptResult car = netClient->Connect( serverName.c_str(), static_cast<unsigned short>(serverPort), 0, 0);
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
