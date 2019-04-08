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
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>		//write

int create()
{
#ifdef _WINSOCKAPI_
	// Initialize Winsock
	WSADATA wsaData;
	int  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}
#endif

	return 0;
};

tcp_client::tcp_client()
{
	static const int c = create();
	if(c != 0)
	{
		return;
	}
	sock = -1;
	port = 0;
	address = "";
	connected = false;
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

	//setup address structure
	if(inet_addr(address.c_str()) == INADDR_NONE)
	{
		struct hostent* he;
		struct in_addr** addr_list;

		//resolve the hostname, its not an ip address
		if((he = gethostbyname(address.c_str())) == NULL)
		{
			//gethostbyname failed
			perror("Failed to resolve hostname");

			return false;
		}

		//Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
		addr_list = (struct in_addr**) he->h_addr_list;

		for(int i = 0; addr_list[i] != NULL; i++)
		{
			//strcpy(ip , inet_ntoa(*addr_list[i]) );
			server.sin_addr = *addr_list[i];

#ifndef NDEBUG
			std::cout << address << " resolved to " << inet_ntoa(*addr_list[i]) << std::endl;
#endif

			break;
		}
	}

	//plain ip address
	else
	{
		server.sin_addr.s_addr = inet_addr(address.c_str());
	}

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
			sleep(1);
		}

		delete [] buffer;
	}
	return reply;
}

bool tcp_client::stop()
{
	connected = false;
	
	if (sock == -1)
	{
		return false;
	}
	
	shutdown(sock, SHUT_RDWR);
	close(sock);
	sock = -1;
	
	return true;
}
