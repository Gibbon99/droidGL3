#pragma once

#include "hdr/game/gam_droids.h"
#include "hdr/system/sys_main.h"
#include "hdr/network/net_common.h"

typedef struct
{
	bool                                inUse;
	RakNet::SystemAddress               systemAddress;
	RakNet::RakNetGUID                  GUID;
	char                                name[16];
	int                                 packetSequenceCount;
	std::string                         currentLevel;       // Level client is currently on
	int                                 state;              // What is the player doing now
} _netClientInfo;

extern std::vector<_netClientInfo>  netClientInfo;

extern int                          maxNumClients;  // From script


bool net_startServer( unsigned short hostPort, unsigned short netMaxNumClients );

// Send the client world position according to the server
void net_sendPositionUpdate ( int whichClient );

void net_shutdownServer();

// New client connected - record their details
bool net_addNewClient(RakNet::SystemAddress clientAddressReceived, RakNet::RakNetGUID clientGUID );

// Process the world simulation
void net_processWorldStep();
