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
} _netClientInfo;

extern std::vector<_netClientInfo> netClientInfo;

extern int              maxNumClients;  // From script

bool net_startServer( unsigned short hostPort, unsigned short netMaxNumClients );

// Send the client world position according to the server
void net_sendPositionUpdate ( int whichClient );

void net_shutdownServer();
