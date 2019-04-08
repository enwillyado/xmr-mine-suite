/* FINDER
 * Only find a job from provider.
 */
#include "Finder.h"
#include "Workers.h"
#include "version.h"

#include <iostream>
#include <iomanip>

#include "3rdparty/w_tcp/tcpserver.h"
#include "3rdparty/w_tcp/tcpclient.h"

#include "3rdparty/w_base/toStr.hpp"

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <stdlib.h>

#include <sstream>
#include <algorithm>
#include <vector>
#include <map>
#include <iterator>

#include <pthread.h>     /* pthread functions and data structures */

static std::vector<std::string> Split(const std::string & s, char delim)
{
	std::vector<std::string> elems;
	std::stringstream ss(s);
	std::string item;
	while(std::getline(ss, item, delim))
	{
		*(std::back_inserter(elems)++) = item;
	}
	return elems;
}

class PrivateFinder : public tcp_client
{	
public:
	static PrivateFinder & GetInstance()
	{
		static PrivateFinder c;
		return c;
	}
	
	void receive()
	{
		const std::string & str = tcp_client::receive(1024);
		std::vector<std::string> x = Split(str, '"');

		std::cout << "-<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
		std::cout << str << std::endl;
		std::cout << "------------------------" << std::endl;

		bool isJob = false;
		for(size_t i = 0; i < x.size(); ++i)
		{
			const std::string xi = x[i];
			if(xi == "blob")
			{
				blob = x[i + 2];
			}
			else if(xi == "job_id")
			{
				job_id = x[i + 2];
				isJob = true;
			}
			else if(xi == "target")
			{
				target = x[i + 2];
			}
			else if(xi == "id")
			{
				session_id = x[i + 2];
			}
			else if(xi == "height")
			{
				for(size_t iii = 0; iii < x[i + 1].size(); ++iii)
				{
					if(x[i + 1][iii] < '0' || x[i + 1][iii] > '9')
					{
						x[i + 1][iii] = ' ';
					}
				}
				height = x[i + 1];
			}
			else
			{

			}
		}

		if(isJob == true)
		{
			const std::string job = blob + " " + target + " " + height + " ";
			Workers::GetInstance().broadcast(job);
		}
		
		receive();
	}
	
	void sendNonce(const std::string & nonce, const std::string & result)
	{
		static int id = 1;
		
		const std::string & str = "{\"method\":\"submit\","
										"\"params\":{\"id\":\"" + session_id +
											"\",\"job_id\":\"" + job_id +
											"\",\"nonce\":\"" + nonce + 
											"\",\"result\":\"" + result + "\"}, \"id\":" + toStr(++id) + "}";
		std::cout << "->>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
		std::cout << str << std::endl;
		std::cout << "------------------------" << std::endl;
		
		tcp_client::send_data(str + "\n");
	}
	
	std::string session_id;
	std::string job_id;
	std::string blob;
	std::string target;
	std::string height;
};

class PrivateWorkers : public tcp_server
{
public:	
	void getMessage(const int client_sock, const std::string & client_sock_ip, const std::string & client_message)
	{
#ifndef NDEBUG
		std::cout << "-]]]]]]]]]]]]]]]]]]]]]]]" << std::endl;
		std::cout << client_sock << " @ " << client_sock_ip << " : " << client_message << std::endl;
		std::cout << "------------------------" << std::endl;
#endif
		std::string str = client_message;
		for(size_t iii = 0; iii < str.size(); ++iii)
		{
			if(str[iii] == '=')
			{
				str[iii] = '&';
			}
		}
		std::vector<std::string> x = Split(str, '&');
		
		//Send the message back to client
		std::string http_response_message = "Bad message";
		for(size_t i = 0; i < x.size(); ++i)
		{
			const std::string & xi = x[i];
			if(xi == "q")
			{
				const std::string & xi1 = x[i + 1];
				if(xi1 == "start")
				{
					const std::string & xi2 = x[i + 2];
					if(xi2 == "port")
					{
						const std::string & port = x[i + 3];
						
#ifndef NDEBUG
						std::cout << "-[[[[[[[[[[[[[[[[[[[[[[[" << std::endl;
						std::cout << client_sock << " @ " << client_sock_ip << " : port : " << port << std::endl;
						std::cout << "------------------------" << std::endl;
#endif
						Workers::GetInstance().add(client_sock_ip, port);
						
						http_response_message = "Receive start, welcome!";
					}
				}
				else if(xi1 == "end")
				{
#ifndef NDEBUG
					std::cout << "-[[[[[[[[[[[[[[[[[[[[[[[" << std::endl;
					std::cout << client_sock << " @ " << client_sock_ip << " : end" << std::endl;
					std::cout << "------------------------" << std::endl;
#endif
					Workers::GetInstance().remove(client_sock_ip);
						
					http_response_message = "Receive end, bye!";
				}
				else if(xi1 == "complete")
				{
#ifndef NDEBUG
					std::cout << "-[[[[[[[[[[[[[[[[[[[[[[[" << std::endl;
					std::cout << client_sock << " @ " << client_sock_ip << " : complete" << std::endl;
					std::cout << "------------------------" << std::endl;
#endif
					Workers::GetInstance().complete(client_sock_ip);
						
					http_response_message = "Receive complete, great!";
				}
				else if(xi1 == "nonce")
				{
					const std::string & xi2 = x[i + 2];
					if(xi2 == "nonce")
					{
						const std::string & nonce = x[i + 3];
						
						const std::string & xi4 = x[i + 4];
						if(xi4 == "result")
						{
							const std::string & result = x[i + 5];
							
#ifndef NDEBUG
							std::cout << "-[[[[[[[[[[[[[[[[[[[[[[[" << std::endl;
							std::cout << client_sock << " @ " << client_sock_ip << " : nonce : " << nonce << " : result " << result << std::endl;
							std::cout << "------------------------" << std::endl;
#endif
							
							PrivateFinder::GetInstance().sendNonce(nonce, result);
							
							http_response_message = "Receive nonce, thnks!";
						}
					}
				}
			}
		}
		
		const std::string response_message = 
			"HTTP/1.1 200 OK" "\r\n"\
			"Content-Length: " + toStr(http_response_message.size() + 2) + "\r\n"\
			"Content-Type: text/plain; charset=utf-18" "\r\n"\
			"\r\n" + http_response_message + "\r\n";

#ifndef NDEBUG
		std::cout << "-[[[[[[[[[[[[[[[[[[[[[[[" << std::endl;
		std::cout << client_sock << " @ " << client_sock_ip << " : " << response_message << std::endl;
		std::cout << "------------------------" << std::endl;
#endif
		
		sendMessage(client_sock, response_message);
		
		disconectClient(client_sock);
	}
	
	void getDisconect(const int client_sock, const std::string & client_sock_ip)
	{
#ifndef NDEBUG
		std::cout << "-[[[[[[[[[[[[[[[[[[[[[[[" << std::endl;
		std::cout << "Disconected : " << client_sock << " @ " << client_sock_ip << std::endl;
		std::cout << "------------------------" << std::endl;
#endif
	}
};

void* createServer(void* data)
{
	PrivateWorkers* server = static_cast<PrivateWorkers*>(data);
	if(server != NULL)
	{
		server->start();
	}
	return data;
}

int Finder::Exec(const int workers_tcp_port,
				 const std::string & serverhost, const int serverport, const std::string & user, const std::string & pass,
                 const std::string & agent)
{
	// create workers
	Workers::GetInstance();
	
	// create proxy server
	PrivateWorkers workers_server;
	if(false == workers_server.create(workers_tcp_port))
	{
		std::cerr << "Fail to create workers_server" << std::endl;
		return -1;
	}
	
    pthread_t server_p_thread;
    int server_thr_id = pthread_create(&server_p_thread, NULL, createServer, (void*)&workers_server);
	if(server_thr_id < 0)
	{
		std::cerr << "Fail to start workers_server" << std::endl;
		return -1;
	}
	
	// Create client
	//
	PrivateFinder & c = PrivateFinder::GetInstance();

	//connect to server
	c.conn(serverhost, serverport);
	
	const std::string & str = "{\"id\":1,\"jsonrpc\":\"2.0\",\"method\":\"login\",\"params\":{\"login\":\"" + user +
							"\",\"pass\":\"" + pass + "\",\"agent\":\"" + agent + "\"}}";
	std::cout << "->>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
	std::cout << str << std::endl;
	std::cout << "------------------------" << std::endl;
	
	//send some data
	c.send_data(str + "\n");

	//receive and echo reply
	c.receive();

	//done
	return 0;
}