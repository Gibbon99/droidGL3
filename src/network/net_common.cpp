#include "hdr/system/sys_main.h"
#include "hdr/network/net_common.h"
#include "hdr/network/net_client.h"
#include "hdr/network/net_server.h"
#include "hdr/system/sys_events.h"

vector<_enetClientInfo>     enetClientInfo;

ENetPacket *enetPacket = nullptr;

int networkPacketCountSentClient = 0;
int networkPacketCountSentServer = 0;

bool enetInitDone = false;

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return True / False on success or not
//
// Start the network library
bool enet_initLibrary ()
//-----------------------------------------------------------------------------------------------------
{
	if ( enet_initialize () != 0 )
	{
		con_print (CON_ERROR, true, "An error occurred while initializing ENet.");
		return false;
	}

	enetInitDone = true;
	return true;
}

//-----------------------------------------------------------------------------
//
// Send the packet over the network
void enet_sendPacket( _networkPacket networkPacket, int packetSource, int whichClient)
//-----------------------------------------------------------------------------
{
	if (enetPacket == nullptr)
		enetPacket = enet_packet_create (&networkPacket, sizeof(_networkPacket), ENET_PACKET_FLAG_RELIABLE);
	else
		memcpy(enetPacket->data, &networkPacket, sizeof(_networkPacket));

//	if (enetPacket)     // Was the packet created ok
	{
		switch ( packetSource )
		{
			case USER_EVENT_NETWORK_FROM_CLIENT:
				enet_peer_send (serverPeer, 0, enetPacket);
				networkPacketCountSentClient++;
				break;

			case USER_EVENT_NETWORK_FROM_SERVER:
				//  Send the packet to the peer over channel id 0.
//				enet_peer_send (&enetClientInfo[whichClient].peer, 0, enetPacket);
				enet_peer_send (&enetClientInfo[0].peer, 0, enetPacket);
				networkPacketCountSentServer++;
//				printf("Sending packet from SERVER to peer [ %s ]\n", enetClientInfo[0].name.c_str());

				break;

			default:
				break;
		}

//		enet_packet_destroy(enetPacket);
	}
//	else
//		printf("Error creating packet to send.\n");
}