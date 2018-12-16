#include "hdr/network/net_server.h"
#include "hdr/network/net_common.h"
#include "hdr/game/gam_player.h"

std::vector<_netClientInfo> netClientInfo;

int                     maxNumClients;  // From script

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
bool net_startServer( const string &hostAddress, unsigned short hostPort, unsigned short netMaxNumClients )
//-----------------------------------------------------------------------------------------------------
{
	bool serverStartResult;

	RakNet::SocketDescriptor socketDescriptors[2];

	printf("Server IP: [ %s ]\n", hostAddress.c_str());

	strcpy(socketDescriptors[0].hostAddress, hostAddress.c_str());
	socketDescriptors[0].port = hostPort;
	socketDescriptors[0].socketFamily = AF_INET; // Test out IPV4

	strcpy(socketDescriptors[1].hostAddress, "::1");
	socketDescriptors[1].port = hostPort;
	socketDescriptors[1].socketFamily = AF_INET6; // Test out IPV6

	netServer = RakNet::RakPeerInterface::GetInstance ();
//	netServer->SetIncomingPassword("Rumpelstiltskin", (int)strlen("Rumpelstiltskin"));
	netServer->SetTimeoutTime(30000,RakNet::UNASSIGNED_SYSTEM_ADDRESS);

	serverStartResult = netServer->Startup(netMaxNumClients, socketDescriptors, 1 ) == RakNet::RAKNET_STARTED;  // Only use 1 socketDescriptor ( IPv4)
	netServer->SetMaximumIncomingConnections(netMaxNumClients);

	if (!serverStartResult)
	{
		printf("Failed to start dual IPV4 and IPV6 ports. Trying IPV4 only.\n");

		// Try again, but leave out IPV6
		serverStartResult = netServer->Startup(netMaxNumClients, socketDescriptors, 1 ) == RakNet::RAKNET_STARTED;
		if (!serverStartResult)
		{
			printf("Server failed to start.  Terminating.\n");
			return false;
		}
	}
	netServer->SetOccasionalPing(true);
	netServer->SetUnreliableTimeout(1000);

	printf("Server: Max number of connections [ %i ]\n", netMaxNumClients);

	unsigned int i;
	for (i = 0; i < netServer->GetNumberOfAddresses(); i++)
		printf("Server address : [ %s ]\n", netServer->GetLocalIP (i));

//	printf("External address : [ %s ]\n", netServer->GetExternalID (netServer->))

	netClientInfo.reserve(netMaxNumClients);
	for ( auto &it : netClientInfo )
		it.inUse = false;

	return true;
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void net_shutdownServer()
//-----------------------------------------------------------------------------------------------------
{
	if (serverRunning)
	{
		netServer->Shutdown ( 300 );
		// We're done with the network
		RakNet::RakPeerInterface::DestroyInstance ( netServer );
	}
}

//--------------------------------------------------------------------------------
//
// Send the client world position according to the server
void net_sendPositionUpdate ( int whichClient )
//--------------------------------------------------------------------------------
{
	evt_sendEvent (USER_EVENT_NETWORK_OUT, USER_EVENT_NETWORK_OUT, NET_CLIENT_WORLDPOS, whichClient, 0, glm::vec2{playerDroid.worldPos.x, playerDroid.worldPos.y} , glm::vec2 (), "");
}
