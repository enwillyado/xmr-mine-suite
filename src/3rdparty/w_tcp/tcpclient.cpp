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
			perror("Could not create socket. Error: ");
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
			std::cerr << "Failed to resolve hostname\n";

			return false;
		}

		//Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
		addr_list = (struct in_addr**) he->h_addr_list;

		for(int i = 0; addr_list[i] != NULL; i++)
		{
			//strcpy(ip , inet_ntoa(*addr_list[i]) );
			server.sin_addr = *addr_list[i];

			std::cerr << address << " resolved to " << inet_ntoa(*addr_list[i]) << std::endl;

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
		perror("connect failed. Error: ");
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
		perror("Send failed. Error: ");
		return false;
	}

	return true;
}

/**
	Receive data from the connected host
*/
std::string tcp_client::receive(const int size)
{
	char* buffer = new char[size];
	std::string reply;

	//Receive a reply from the server
	int i;
	if((i = recv(sock, buffer, size, 0)) < 0)
	{
		std::cerr << "recv failed" << std::endl;
	}

	reply = std::string(buffer, i);
	delete [] buffer;
	return reply;
}
