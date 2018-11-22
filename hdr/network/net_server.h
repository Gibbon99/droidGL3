#pragma once

#include "hdr/game/gam_droids.h"
#include "hdr/system/sys_main.h"
#include "hdr/network/net_common.h"

extern bool             runAsServer;
extern bool             networkServerIsRunning;

extern int              serverPort;     // From script
extern int              maxNumClients;  // From script

bool enet_startServer ( const std::string &hostAddress, enet_uint16 hostPort, size_t enetMaxNumClients, size_t enetNumChannels );

// Send the client world position according to the server
void net_sendPositionUpdate ( int whichClient );

void enet_shutdownServer ();