#pragma once

#include <enet/enet.h>
#include "hdr/system/sys_main.h"
#include "hdr/system/sys_events.h"
#include "hdr/network/net_server.h"



typedef struct
{
	ENetPeer peer;
	std::string name;
	int packetSequenceCount;
} _enetClientInfo;

extern std::vector<_enetClientInfo> enetClientInfo;

typedef struct
{
	long timeStamp;
	long sequence;
	int packetOwner;
	int packetType;
	int data1;
	int data2;
	int data3;
	glm::vec2 vec2_1;
	glm::vec2 vec2_2;
	char text[32];
} _networkPacket;

#define NET_TEXT_SIZE        32

#define NET_SYS_KEEPALIVE         0x501
#define NET_CLIENT_DATA_PACKET    0x502
#define NETWORK_SEND_SYSTEM       0x503
#define NET_SYSTEM_PACKET         0x504
#define NET_STATUS                0x505
#define NET_CLIENT_CURRENT_LEVEL  0x506
#define NET_CLIENT_WORLDPOS       0x507
#define NET_CLIENT_SYSTEM_PACKET  0x508
#define NET_DROID_WORLDPOS        0x509
#define NET_CURRENT_TICK          0x501

extern std::string  serverAddress;

extern int          networkPacketCountSentClient;
extern int          networkPacketCountSentServer;

extern bool         enetInitDone;
extern ENetPeer *serverPeer;

bool enet_initLibrary ();

// Send the packet over the network
void enet_sendPacket ( _networkPacket networkPacket, int packetSource, int whichClient );

