/* TCP-SERVER
 */

#ifndef __TCP_SERVER_H_
#define __TCP_SERVER_H_

#include <stdio.h>      //printf
#include <string.h>     //strlen
#include <string>       //string

#ifdef _WIN32
#include <winsock2.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>

// Need to link exe with some libs
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#else
#include <sys/socket.h> //socket
#include <arpa/inet.h>  //inet_addr
#include <netdb.h>		//hostent
#include <unistd.h>		//write
#endif

/**
	TCP Server class
*/
class tcp_server
{
public:
	tcp_server();
	bool create(const int port);
	void start();
	bool stop();

	virtual void getMessage(const int client_sock, const std::string & client_sock_ip,
	                        const std::string & client_message);
	virtual void getDisconect(const int client_sock, const std::string & client_sock_ip);

	virtual void sendMessage(const int client_sock, const std::string & message_response);
	virtual void disconectClient(const int client_sock);

private:
	int socket_desc;
};

#endif /* __TCP_SERVER_H_ */
