#pragma once
#include "hdr/system/sys_main.h"
#include "hdr/network/netcode.h"

#define CONNECT_TOKEN_EXPIRY 30     // Make variable from script TODO:
#define CONNECT_TOKEN_TIMEOUT 5

typedef struct networkPacket
{
	long timeStamp;
	long sequence;
	int data1;
	int data2;
	int data3;
	glm::vec2 vec2_1;
	glm::vec2 vec2_2;
	std::string text;
} _networkPacket;

extern char * serverAddress; // = "[::1]:9991";

#define PROTOCOL_ID     0x1122334455667788

static uint8_t private_key[NETCODE_KEY_BYTES] = {0x60, 0x6a, 0xbe, 0x6e, 0xc9, 0x19, 0x10, 0xea,
												 0x9a, 0x65, 0x62, 0xf6, 0x6f, 0x2b, 0x30, 0xe4,
												 0x43, 0x71, 0xd6, 0x2c, 0xd1, 0x99, 0x27, 0x26,
												 0x6b, 0x3c, 0x60, 0xf4, 0xb7, 0x15, 0xab, 0xa1};
