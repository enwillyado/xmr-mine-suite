/* TCP-SERVER
 */

#ifndef __TCP_SERVER_H_
#define __TCP_SERVER_H_

#include <string>

/**
	TCP Server class
*/
class tcp_server
{
public:
	tcp_server();
	bool create(const int port);
	void start();
	
	virtual void getMessage(const int client_sock, const std::string & client_message);
	virtual void getDisconect(const int client_sock);
	
	virtual void sendMessage(const int client_sock, const std::string & message_response);
	virtual void disconectClient(const int client_sock);
	
private:
	int socket_desc;
};

#endif /* __TCP_SERVER_H_ */
