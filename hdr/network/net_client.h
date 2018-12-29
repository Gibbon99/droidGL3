#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/network/net_common.h"

extern bool                    haveServerConnection;  // Have we connected to the server

bool net_startClient(int serverPort);

void net_shutdownClient();

bool net_clientConnectTo( const string &serverName, unsigned short serverPort );

// Send the current level to the server for this client
void net_sendCurrentLevel(std::string whichLevel);

// Got a PONG response from the server
void net_clientGotPong(RakNet::SystemAddress serverAddressReceived);

// Got a connection accepted from the server
void net_clientConnectionAccepted(RakNet::SystemAddress serverAddressReceived, RakNet::RakNetGUID clientGUID);

// Call a timer to start broadcast pinging looking for a server
// Response will be received on the network IN thread
void net_startConnectionToServer(int newState);
