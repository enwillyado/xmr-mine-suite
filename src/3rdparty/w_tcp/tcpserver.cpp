/*
	C socket server example
*/

#include "tcpserver.h"

#include <stdio.h>
#include <string.h>		//strlen
#ifdef __GNUC__
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>		//write
#endif

void tcp_server::getMessage(const int client_sock, const std::string & client_sock_ip,
                            const std::string & client_message)
{
	sendMessage(client_sock, client_message);
}

void tcp_server::sendMessage(const int client_sock, const std::string & response)
{
	//Send the message back to client
#ifdef __GNUC__
	if(0 > write(client_sock, response.c_str(), response.size()))
#else
	if(0 > ::send(client_sock, response.c_str(), response.size(), 0))
#endif
	{
		perror("write failed");
		return;
	}
}

void tcp_server::getDisconect(const int client_sock, const std::string & client_sock_ip)
{
}

void tcp_server::disconectClient(const int client_sock)
{
#ifdef __GNUC__
	close(client_sock);
#else
	closesocket(socket_desc);
#endif
}

#ifndef __GNUC__
static WSADATA & InitializeWSA()
{
	// Initialize Winsock
	static WSADATA wsaData;
	if(0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		perror("WSAStartup failed with error");
	}
	return wsaData;
}
#endif

tcp_server::tcp_server()
{
#ifndef __GNUC__
	static WSADATA wsaData = InitializeWSA();
#endif
}

bool tcp_server::create(const int port)
{
	struct sockaddr_in server;

	//Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_desc == -1)
	{
		perror("Could not create socket");
		return false;
	}

#ifndef NDEBUG
	puts("Socket created");
#endif

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	int tr = 1;

#ifdef __GNUC__
	// kill "Address already in use" error message
	if(setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(int)) == -1)
	{
		perror("setsockopt");
		exit(1);
	}
#endif

	//Bind
	if(bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return false;
	}
#ifndef NDEBUG
	puts("bind done");
#endif

	//Listen
	listen(socket_desc, 3);

	//Accept and incoming connection
#ifndef NDEBUG
	puts("Waiting for incoming connections...");
#endif

	return true;
}

// get sockaddr, IPv4 or IPv6:
static void inline* get_in_addr(struct sockaddr* sa)
{
	if(sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void tcp_server::start()
{
	int client_sock, c, read_size;
	struct sockaddr_in client;
	struct sockaddr* clientadd = (struct sockaddr*)(&client);
	char client_message[2048];

	c = sizeof(struct sockaddr_in);

	//accept connection from an incoming client while socket descriptor is valid
	while(socket_desc != -1)
	{
		client_sock = accept(socket_desc, clientadd, (socklen_t*)&c);
		if(client_sock < 0)
		{
			perror("accept failed");
			return;
		}
#ifndef NDEBUG
		puts("Connection accepted");
#endif

		char client_sock_ip[INET6_ADDRSTRLEN + 1];
		memset(client_sock_ip, '\0', sizeof(client_sock_ip));
		inet_ntop(clientadd->sa_family, get_in_addr(clientadd), client_sock_ip, INET6_ADDRSTRLEN);

		//Receive a message from client
		read_size = recv(client_sock, client_message, 2048, 0);

		if(read_size > 0)
		{
			getMessage(client_sock, client_sock_ip, client_message);
		}
		else if(read_size == 0)
		{
#ifndef NDEBUG
			puts("Client disconnected");
#endif

			getDisconect(client_sock, client_sock_ip);
		}
		else if(read_size == -1)
		{
			perror("recv failed");
		}
	}

	return;
}

bool tcp_server::stop()
{
	bool ret = true;

	if(socket_desc == -1)
	{
		return false;
	}

#ifdef __GNUC__
	shutdown(socket_desc, SHUT_RDWR);
	close(socket_desc);
#else
	// shutdown the connection since we're done
	const int iResult = shutdown(socket_desc, SD_SEND);
	if(iResult == SOCKET_ERROR)
	{
		perror("shutdown failed with error");
		ret = false;
	}

	// cleanup
	closesocket(socket_desc);
	WSACleanup();
#endif

	socket_desc = -1;

	return ret;
}
