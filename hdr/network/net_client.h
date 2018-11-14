#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/network/netcode.h"
#include "hdr/network/net_common.h"

extern  netcode_client_t        *networkClient;
extern  bool                    networkClientIsRunning;

// Create the network client
bool net_createNetworkClient ( float time );

// Update the network client
void net_updateNetworkClient ( float time );

// Shutdown the client instance and terminate netcode
void net_shutdownClient ();

// Get the state of the client
void net_networkClientGetState ( int clientState );

// Send the current level to the server for this client
void net_sendCurrentLevel(std::string whichLevel);