#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/network/net_common.h"

bool net_startClient(int serverPort);

void net_shutdownClient();

bool net_clientConnectTo( const string &serverName, unsigned short serverPort );

// Send the current level to the server for this client
void net_sendCurrentLevel(std::string whichLevel);

// Send an unconnected broadcast ping to locate a server
void net_clientSendDiscoverPing(unsigned short serverPort);
