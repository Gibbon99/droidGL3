#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/network/net_common.h"

extern bool             runAsServer;
extern bool             networkServerIsRunning;

extern int              serverPort;     // From script
extern int              maxNumClients;  // From script

// Create the network server
bool net_createServer ( float time );

// Shutdown the server instance and terminate netcode
void net_shutdownServer ();

// Thread function to process the network server packets
int net_processNetworkServerQueue ( void *ptr );

// Update the network server
void net_updateNetworkServer ( float time );
