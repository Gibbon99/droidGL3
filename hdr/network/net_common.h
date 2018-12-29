#pragma once

#include "hdr/network/raknet/RakPeer.h"
#include "hdr/network/raknet/MessageIdentifiers.h"
#include "hdr/network/raknet/RakPeerInterface.h"
#include "hdr/network/raknet/RakNetTypes.h"

#include "hdr/system/sys_main.h"
#include "hdr/system/sys_events.h"
#include "hdr/network/net_server.h"

#define NET_DEBUG   1

enum GameMessages
{
	ID_GAME_MESSAGE_1=ID_USER_PACKET_ENUM+1
};

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
extern int          networkPacketCountReceiveServer;
extern size_t       networkOutQueueSize;

extern RakNet::RakPeerInterface     *netClient;
extern RakNet::RakPeerInterface     *netServer;

extern bool     isServer;
extern bool     isClient;

extern std::string      connectionPassword;
extern std::string   serverName;
extern int      serverPort;

extern bool     serverRunning;
extern bool     clientRunning;

// Start the network library
bool net_initLibrary ();

// Send the packet over the network
void net_sendPacket( RakNet::BitStream *bitStream, int packetSource, int whichClient );

// Handle all the network IN actvity here
int net_processNetworkTraffic( void *ptr );

// This function sends packets OUT from both client and server
int net_processNetworkOutQueue ( void *ptr );

// Connect to the server
void net_consoleStartNetClient();

// Start the server from the console
void net_consoleStartNetServer();

// Console command to show the list of connected clients, if this is the server
void con_listClients();
