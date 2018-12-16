#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/system/sys_events.h"

// Process events coming into the client from the network
int gam_processClientEventQueue ( void *ptr );

// Farm out the events received by the client for action
void gam_handleClientInPacket( RakNet::Packet passPacket );
