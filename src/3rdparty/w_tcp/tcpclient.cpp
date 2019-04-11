/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2016-2017 XMRig       <support@xmrig.com>
 *
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "tcpclient.h"

#include <iostream>

#include <stdio.h>
#include <string.h>		//strlen

static bool InitialisingWSA()
{
#ifdef _WINSOCKAPI_
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		perror("Initialising Winsock failed");
		return false;
	}
#endif
	return true;
};

tcp_client::tcp_client()
{
	static const bool c = InitialisingWSA();

	sock = -1;
	port = 0;
	address = "";
	connected = false;
}

std::string resolve_address(const std::string & address)
{
	char client_sock_ip[INET6_ADDRSTRLEN];
	struct addrinfo* addr_info;
	memset(client_sock_ip, '\0', sizeof(client_sock_ip));
	int errcode = getaddrinfo(address.c_str(), NULL, NULL, &addr_info);
	if(errcode != 0)
	{
		perror("fail to get address info");
	}
	else
	{
		for(struct addrinfo* res = addr_info; res != NULL; res = res->ai_next)
		{
			if(res->ai_family == AF_INET)
			{
				if(NULL == inet_ntop(AF_INET, &((struct sockaddr_in*)res->ai_addr)->sin_addr, client_sock_ip,
				                     sizeof(client_sock_ip)))
				{
					perror("inet_ntop");
				}
				else
				{
#ifndef NDEBUG
					std::cout << address << " resolved to " << client_sock_ip << std::endl;
#endif
				}
			}
		}
	}
	//plain ip address
	return client_sock_ip;
}

#include <map>

std::string tcp_client::resolve(const std::string & address)
{
	static std::map<std::string, std::string> dns;
	if(dns[address] != "")
	{
		return dns[address];
	}
	return dns[address] = resolve_address(address);
}

/**
	Connect to a host on a certain port number
*/
bool tcp_client::conn(const std::string & address, const int port)
{
	//create socket if it is not already created
	if(sock == -1)
	{
		//Create socket
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if(sock == -1)
		{
			perror("Could not create socket");
		}

	}

	server.sin_addr.s_addr = inet_addr(resolve(address).c_str());

	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	//Connect to remote server
	if(connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
	{
		perror("Connect failed");
		connected = true;
		return 1;
	}

	return true;
}

/**
	Send data to the connected host
*/
bool tcp_client::send_data(const std::string & data)
{
	//Send some data
	int i;
	if((i = send(sock, data.c_str(), strlen(data.c_str()), 0)) < 0)
	{
		perror("Send failed");
		return false;
	}

	return true;
}

/**
	Receive data from the connected host
*/
std::string tcp_client::receive(const int size)
{
	std::string reply;
	if(sock != -1)
	{
		char* buffer = new char[size];

		//Receive a reply from the server
		const int i = recv(sock, buffer, size, 0);
		if(i > 0)
		{
			reply = std::string(buffer, i);
		}
		else if(i < 0)
		{
			if(connected)
			{
				perror("Recv failed");
			}
#ifdef __GNUC__
			sleep(1);
#else
			Sleep(1);
#endif
		}

		delete [] buffer;
	}
	return reply;
}

bool tcp_client::stop()
{
	bool ret = true;
	connected = false;

	if(sock == -1)
	{
		return false;
	}

#ifndef WIN32
	shutdown(sock, SHUT_RDWR);
	close(sock);
#else
	// shutdown the connection since we're done
	const int iResult = shutdown(sock, SD_SEND);
	if(iResult == SOCKET_ERROR)
	{
		perror("shutdown failed with error");
		ret = false;
	}

	// cleanup
	closesocket(sock);
	WSACleanup();
#endif

	sock = -1;

	return ret;
}
