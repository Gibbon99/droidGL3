//
// Created by dberry on 10/11/18.
//
//#include "hdr/io/minilzo/lzoconf.h"
#include "hdr/network/net_common.h"
#include "hdr/network/net_client.h"

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