#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/network/netcode.h"
#include "hdr/network/net_common.h"

// Create the network client
bool net_createNetworkClient ( float time );

// Update the network client
void net_updateNetworkClient ( float time );

// Shutdown the client instance and terminate netcode
void net_shutdownClient ();