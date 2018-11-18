#include "hdr/network/net_common.h"
#include "hdr/network/net_client.h"
#include "hdr/network/net_server.h"
#include "hdr/system/sys_events.h"

//-------------------------------------------------------------------------
//
// Set the server port and address string
string net_setNetworkAddress ( int port, string networkAddress )
//-------------------------------------------------------------------------
{
	string fullAddress;

	fullAddress = networkAddress + ":" + std::to_string (port);

	return fullAddress;

	//serverAddress = "[::1]:9991";
}

//-----------------------------------------------------------------------------
//
// Send the passed in packet across the network
void net_sendPacket ( _networkPacket networkPacket, int packetSource, int whichClient )
//-----------------------------------------------------------------------------
{
	lzo_uint inLength;
	lzo_uint outLength;
	void *packetPtr;
	char outPacketPtr[sizeof (_networkPacket)];
	int result;

	packetPtr = &networkPacket;

	inLength = sizeof (networkPacket);

	result = lzo1x_1_compress ((unsigned char *) packetPtr, inLength, (unsigned char *) &outPacketPtr, &outLength, wrkmem);

	if ( result != LZO_E_OK )
	{
		con_print (CON_ERROR, true, "Error compressing network packet . %lu bytes into %lu bytes", (unsigned long) inLength, (unsigned long) outLength);
		return; // Don't send the packet
	}

	switch ( packetSource )
	{
		case USER_EVENT_NETWORK_FROM_CLIENT:
			netcode_client_send_packet (networkClient, (unsigned char *) outPacketPtr, (int) (outLength));
			break;

		case USER_EVENT_NETWORK_FROM_SERVER:
			netcode_server_send_packet (networkServer, whichClient, (unsigned char *) outPacketPtr, (int) (outLength));

			printf("Server packet outLength [ %lu ]\n", outLength);
			break;

		default:
			break;
	}
}
