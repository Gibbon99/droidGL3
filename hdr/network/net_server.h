#pragma once

#include "hdr/network/netcode.h"
#include "hdr/io/minilzo/minilzo.h"
#include "hdr/game/gam_droids.h"
#include "hdr/system/sys_main.h"
#include "hdr/network/net_common.h"

typedef struct
{
  int                       packetSequenceCount = 0;
  std::string               clientName;
  char 			            currentDeck[32];
//  _droid                    clientDroid;
} _clientInfo;

extern std::vector<_clientInfo>     clientInfo;

extern netcode_server_t *networkServer;
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

// Send the client world position according to the server
void net_sendPositionUpdate ( int whichClient );