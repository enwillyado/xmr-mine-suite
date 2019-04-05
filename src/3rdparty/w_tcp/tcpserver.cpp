/*
	C socket server example
*/

#include "tcpserver.h"

#include <stdio.h>
#include <string.h>		//strlen
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>		//write

void tcp_server::getMessage(const int client_sock, const std::string & client_sock_ip, const std::string & client_message)
{
	sendMessage(client_sock, client_message);
}

void tcp_server::sendMessage(const int client_sock, const std::string & response)
{
	//Send the message back to client
	if(0 > write(client_sock, response.c_str(), response.size()))
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
	close(client_sock);
}

tcp_server::tcp_server()
{
}

bool tcp_server::create(const int port)
{
	struct sockaddr_in server;
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
		return false;
	}
	
#ifndef NDEBUG
	puts("Socket created");
#endif

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( port );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return false;
	}
#ifndef NDEBUG
	puts("bind done");
#endif

	//Listen
	listen(socket_desc , 3);
	
	//Accept and incoming connection
#ifndef NDEBUG
	puts("Waiting for incoming connections...");
#endif
	
	return true;
}

// get sockaddr, IPv4 or IPv6:
void inline *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void tcp_server::start()
{
	int client_sock , c , read_size;
	struct sockaddr_in client;
	char client_message[2048];
	
	c = sizeof(struct sockaddr_in);
	
	//accept connection from an incoming client
	while(true)
	{
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if (client_sock < 0)
		{
			perror("accept failed");
			return;
		}
#ifndef NDEBUG
		puts("Connection accepted");
#endif

		char client_sock_ip[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET, get_in_addr((struct sockaddr *)&client), client_sock_ip, sizeof(client_sock_ip));

		//Receive a message from client
		read_size = recv(client_sock , client_message, 2048, 0);

		if(read_size > 0 )
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