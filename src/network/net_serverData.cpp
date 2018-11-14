#include "hdr/network/net_serverData.h"


//-----------------------------------------------------------------------------
//
// Handle a data packet from a client
void net_handleClientDataPacket(_networkPacket thisPacket)
//-----------------------------------------------------------------------------
{
  int whichClient;

  whichClient = thisPacket.packetOwner;


  switch (thisPacket.data1)
    {
      case NET_CLIENT_CURRENT_LEVEL:
        snprintf(clientInfo[whichClient].currentDeck, 32, "%s", thisPacket.text);

        con_print(CON_INFO, true, "Client [ %i ] is on deck [ %s ]", whichClient, clientInfo[whichClient].currentDeck);

        printf ("Client sent level update\n");
        break;

      default:
        break;

    }
}