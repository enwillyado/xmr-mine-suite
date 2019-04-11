/* Simple UDP client (to send udp packets)
 * Copyright 2016-2017 XMRig       <support@xmrig.com>
 */
#include "udpclient.h"

#include <iostream>

#include <stdio.h>
#include <string.h>		//strlen

#include<stdio.h>
#include<winsock2.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

extern bool InitialisingWSA();

bool udp_client::send(const std::string & host, const int port, const std::string & message)
{
	static const bool c = InitialisingWSA();
	if(c == false)
	{
		return false;
	}

	//create socket
	const SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(s == SOCKET_ERROR)
	{
		perror("socket failed");
		return false;
	}

	//setup address structure
	struct sockaddr_in si_other;
	const int slen = sizeof(si_other);
	memset((char*) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(port);
	si_other.sin_addr.S_un.S_addr = inet_addr(host.c_str());

	//send the message
	if(sendto(s, message.c_str(), message.size(), 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
	{
		perror("sendto failed");
		return false;
	}

	closesocket(s);

#ifdef ONLY_ONE_UDP_BROADCAST
	// https://docs.microsoft.com/en-us/windows/desktop/api/winsock/nf-winsock-wsacleanup
	WSACleanup();
#endif

	return true;
}
