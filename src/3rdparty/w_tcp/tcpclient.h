/* TCP-CLIENT
 */

#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include <stdio.h>      //printf
#include <string.h>     //strlen
#include <string>       //string

#ifdef _WIN32
#include <WinSock.h> //socket

// Need to link exe with some libs
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#else
#include <sys/socket.h> //socket
#include <arpa/inet.h>  //inet_addr
#include <netdb.h>		//hostent
#endif

/**
	TCP Client class
*/
class tcp_client
{
public:
	tcp_client();
	bool conn(const std::string & address, const int port);
	bool send_data(const std::string & data);
	std::string receive(const int = 512);

private:
	int sock;
	std::string address;
	int port;
	struct sockaddr_in server;
};

#endif /* __TCP_CLIENT_H__ */
