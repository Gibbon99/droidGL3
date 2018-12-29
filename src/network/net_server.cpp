#include "hdr/network/net_server.h"
#include "hdr/network/net_common.h"
#include "hdr/game/gam_player.h"

std::vector<_netClientInfo> netClientInfo;

int                     maxNumClients;  // From script

//-----------------------------------------------------------------------------------------------------
//
// New client connected - record their details
bool net_addNewClient(RakNet::SystemAddress clientAddressReceived, RakNet::RakNetGUID clientGUID )
//-----------------------------------------------------------------------------------------------------
{
	_netClientInfo          tempNetClientInfo;
	unsigned long           newClientIndex = 0;

	if (maxNumClients == netClientInfo.size())
	{
		// Server is full
		return false;       // Send to client 'server is full' message
	}

	tempNetClientInfo.inUse = true;
	tempNetClientInfo.systemAddress = clientAddressReceived;
	tempNetClientInfo.GUID = clientGUID;
	tempNetClientInfo.packetSequenceCount = 0;
	sprintf ( tempNetClientInfo.name, "%s", "No Name" );

	netClientInfo.push_back ( tempNetClientInfo );
	newClientIndex = netClientInfo.size() - 1;

	printf ( "SERVER: ID_NEW_INCOMING_CONNECTION from %s with GUID %s\n", netClientInfo[newClientIndex].systemAddress.ToString ( true ), netClientInfo[newClientIndex].GUID.ToString ());

	return true;
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
bool net_startServer( unsigned short hostPort, unsigned short netMaxNumClients )
//-----------------------------------------------------------------------------------------------------
{
	bool serverStartResult;

	netServer = RakNet::RakPeerInterface::GetInstance();

	RakNet::SocketDescriptor socketDescriptor(hostPort, nullptr);       // Just tell is a port to listen on
	socketDescriptor.socketFamily = AF_INET;                            // Use IPv4 for auto discovery for clients

	netServer = RakNet::RakPeerInterface::GetInstance ();
	netServer->SetIncomingPassword(connectionPassword.c_str(), connectionPassword.length());
	netServer->SetTimeoutTime(30000,RakNet::UNASSIGNED_SYSTEM_ADDRESS);

	serverStartResult = netServer->Startup(netMaxNumClients, &socketDescriptor, 1 ) == RakNet::RAKNET_STARTED;  // Only use 1 socketDescriptor ( IPv4)
	netServer->SetMaximumIncomingConnections(netMaxNumClients);

	if (!serverStartResult)
	{
		con_print(CON_ERROR, true, "Server failed to start. Is the port [ %i ] already in use?", hostPort);
		return false;
	}

	netServer->SetOccasionalPing(true);
	netServer->SetUnreliableTimeout(1000);

	con_print(CON_INFO, true, "Server: Max number of connections [ %i ]", netMaxNumClients);

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
	if (netServer)
	{
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
