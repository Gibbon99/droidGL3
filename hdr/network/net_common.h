#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/io/minilzo/minilzo.h"
#include "hdr/network/netcode.h"
#include "hdr/system/sys_events.h"
//#include "hdr/game/gam_player.h"
#include "hdr/network/net_server.h"

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

/* Work-memory needed for compression. Allocate memory in units
 * of 'lzo_align_t' (instead of 'char') to make sure it is properly aligned.
 */

#define HEAP_ALLOC( var, size ) \
    lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

static HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);

#define NET_TEXT_SIZE        32
#define CONNECT_TOKEN_EXPIRY 30     // Make variable from script TODO:
#define CONNECT_TOKEN_TIMEOUT 15

#define NET_SYS_KEEPALIVE         0x501
#define NET_CLIENT_DATA_PACKET    0x502
#define NETWORK_SEND_SYSTEM       0x503
#define NET_SYSTEM_PACKET         0x504
#define NET_STATUS                0x505
#define NET_CLIENT_CURRENT_LEVEL  0x506
#define NET_CLIENT_WORLDPOS       0x507
#define NET_CLIENT_SYSTEM_PACKET  0x508
#define NET_DROID_WORLDPOS        0x509

extern std::string serverAddress;

// Send the passed in packet across the network
extern void net_sendPacket (_networkPacket networkPacket, int packetSource, int whichClient);

// Set the server port and address string
extern std::string net_setNetworkAddress ( int port, std::string networkAddress );

#define PROTOCOL_ID     0x1122334455667788

static uint8_t private_key[NETCODE_KEY_BYTES] = {0x60, 0x6a, 0xbe, 0x6e, 0xc9, 0x19, 0x10, 0xea,
												 0x9a, 0x65, 0x62, 0xf6, 0x6f, 0x2b, 0x30, 0xe4,
												 0x43, 0x71, 0xd6, 0x2c, 0xd1, 0x99, 0x27, 0x26,
												 0x6b, 0x3c, 0x60, 0xf4, 0xb7, 0x15, 0xab, 0xa1};




