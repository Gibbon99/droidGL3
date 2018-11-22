#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/network/net_common.h"

extern ENetPeer            *serverPeer;

bool enet_startClient ( size_t maxNumChannels );

bool enet_clientConnectTo ( const string &serverName, enet_uint16 serverPort, size_t numChannels, enet_uint32 timeOut );

// Send the current level to the server for this client
void net_sendCurrentLevel(std::string whichLevel);

void enet_shutdownClient ();