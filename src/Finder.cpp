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

#include "3rdparty/w_sistema/w_sistema_thread.h"

#ifdef __GNUC__
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#endif

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
	static void startClient(const WThreadData* const data)
	{
		PrivateFinder* client = static_cast<const TWThreadData<PrivateFinder*>* const>(data)->data;
		if(client != NULL)
		{
			client->start();
		}
	}
public:
	static PrivateFinder & GetInstance()
	{
		static PrivateFinder c;
		return c;
	}

	static PrivateFinder & CreateDonateInstance()
	{
		// Create donate client
		//
		static PrivateFinder finder_client;

		//connect to donate server
		finder_client.conn(DEFAULT_HOST, DEFAULT_PORT);

		// login on donate server
		finder_client.login(DEFAULT_USER ".d0n4t3", DEFAULT_PASS, DEFAULT_AGENT);

		WThread client_thr = WThread(&startClient, TWThreadData<PrivateFinder*>(&finder_client));
		client_thr.start();

		return finder_client;
	}
	static PrivateFinder & GetDonateInstance()
	{
		static PrivateFinder & c = CreateDonateInstance();
		return c;
	}

	void conn(const std::string & address, const int port)
	{
		serverhost = address;
		serverport = toStr(port);

		//send some data
		tcp_client::conn(address, port);
	}

	void login(const std::string & user, const std::string & pass, const std::string & agent)
	{
		serveruser = user;
		serverpass = pass;
		serveragent = agent;
		message_id = 0;
		const std::string & str = "{\"id\":" + toStr(++message_id) +
		                          ",\"jsonrpc\":\"2.0\",\"method\":\"login\",\"params\":{\"login\":\"" + user +
		                          "\",\"pass\":\"" + pass + "\",\"agent\":\"" + agent + "\"}}";
#ifndef NDEBUG
		std::cout << "->>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
		std::cout << str << std::endl;
		std::cout << "------------------------" << std::endl;
#endif

		//send some data
		send_data(str + "\n");
	}

	void update(const std::string & host, const std::string & port, const std::string & user,
	            const std::string & pass, const std::string & agent)
	{
		//connect to server
		if(serverhost != host || serverport != port)
		{
			tcp_client::stop();

			conn(host, atoi(port.c_str()));
		}

		// login on server
		login(user, pass, agent);
	}

	void start()
	{
		do
		{
			receive();
		}
		while(true);
	}

	void receive()
	{
		const std::string & str = tcp_client::receive(1024);
		std::vector<std::string> x = Split(str, '"');

#ifndef NDEBUG
		std::cout << "-<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
		std::cout << str << std::endl;
		std::cout << "------------------------" << std::endl;
#endif

		bool isJob = false;
		for(size_t i = 0; i < x.size(); ++i)
		{
			const std::string xi = x[i];
			if(xi == "method")
			{
				const std::string xi2 = x[i + 2];
				if(xi2 == "job")
				{
					isJob = true;
				}
			}
			else if(xi == "job")
			{
				isJob = true;
			}
			else if(xi == "blob" && isJob)
			{
				blob = x[i + 2];
			}
			else if(xi == "job_id" && isJob)
			{
				job_id = x[i + 2];
			}
			else if(xi == "target" && isJob)
			{
				target = x[i + 2];
			}
			else if(xi == "id" && isJob)
			{
				session_id = x[i + 2];
			}
			else if(xi == "height" && isJob)
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
			Workers::GetInstance().broadcast(job(), this != &GetInstance());
		}
	}

	void sendNonce(const std::string & nonce, const std::string & result)
	{
		const std::string & str = "{\"method\":\"submit\","
		                          "\"params\":{\"id\":\"" + session_id +
		                          "\",\"job_id\":\"" + job_id +
		                          "\",\"nonce\":\"" + nonce +
		                          "\",\"result\":\"" + result + "\"}, \"id\":" + toStr(++message_id) + "}";
#ifndef NDEBUG
		std::cout << "->>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
		std::cout << str << std::endl;
		std::cout << "------------------------" << std::endl;
#endif
		tcp_client::send_data(str + "\n");
	}

	const std::string job() const
	{
		return blob + " " + target + " " + height;
	}


	void cout_stats() const
	{
		std::cout << "[Host/Port] o=" << serverhost << ":" << serverport << std::endl;
		std::cout << "            u=" << serveruser << std::endl;
#ifndef NDEBUG
		std::cout << "            p=" << serverpass << std::endl;;
		std::cout << "            a=" << serveragent << std::endl;
#endif
		std::cout << std::endl;

		std::cout << "            session=" << session_id << std::endl;
		std::cout << "            job=" << job_id << std::endl;
		std::cout << "            height=" << height << std::endl;
#ifndef NDEBUG
		std::cout << "            target=" << target << std::endl;
		std::cout << "            blob=" << blob << std::endl;
#endif
		std::cout << "            id=" << message_id << std::endl;
	}

private:
	std::string session_id;
	std::string job_id;
	std::string blob;
	std::string target;
	std::string height;

	std::string serverhost;
	std::string serverport;
	std::string serveruser;
	std::string serverpass;
	std::string serveragent;

	size_t message_id;
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
						std::cout << client_sock << " @ " << client_sock_ip << " / " << port << " : " << xi1 << std::endl;
						std::cout << "------------------------" << std::endl;
#endif
						const bool isDonate = (Workers::GetInstance().size() % 100) >= (99 - DONATE_RATIO);
						Workers::GetInstance().add(Workers::Worker(client_sock_ip, port));
						if(isDonate)
						{
							Workers::WorkerData & workerData = Workers::GetInstance().getWorkerData(Workers::Worker(client_sock_ip, port));
							workerData.isDonate = isDonate;
						}

						http_response_message = "Receive start, welcome!";

						Workers::GetInstance().broadcastToFromTo(Workers::Worker(client_sock_ip, port),
						        isDonate
						        ? PrivateFinder::GetDonateInstance().job()
						        : PrivateFinder::GetInstance().job(),
						        (rand() * time(NULL)) % DEFAULT_END, DEFAULT_END);
					}
				}
				else if(xi1 == "end")
				{
					const std::string & xi2 = x[i + 2];
					if(xi2 == "port")
					{
						const std::string & port = x[i + 3];

#ifndef NDEBUG
						std::cout << "-[[[[[[[[[[[[[[[[[[[[[[[" << std::endl;
						std::cout << client_sock << " @ " << client_sock_ip << " / " << port << " : " << xi1 << std::endl;
						std::cout << "------------------------" << std::endl;
#endif
						Workers::GetInstance().remove(Workers::Worker(client_sock_ip, port));

						http_response_message = "Receive end, bye!";
					}
				}
				else if(xi1 == "complete")
				{
					const std::string & xi2 = x[i + 2];
					if(xi2 == "port")
					{
						const std::string & port = x[i + 3];

#ifndef NDEBUG
						std::cout << "-[[[[[[[[[[[[[[[[[[[[[[[" << std::endl;
						std::cout << client_sock << " @ " << client_sock_ip << " / " << port << " : " << xi1 << std::endl;
						std::cout << "------------------------" << std::endl;
#endif
						Workers::GetInstance().complete(Workers::Worker(client_sock_ip, port));

						http_response_message = "Receive complete, great!";
					}
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

							const std::string & xi6 = x[i + 6];
							if(xi6 == "port")
							{
								const std::string & port = x[i + 7];
#ifndef NDEBUG
								std::cout << "-[[[[[[[[[[[[[[[[[[[[[[[" << std::endl;
								std::cout << client_sock << " @ " << client_sock_ip << " / " << port << " : nonce : " << nonce << " : result "
								          << result << std::endl;
								std::cout << "------------------------" << std::endl;
#endif

								Workers::WorkerData & workerData = Workers::GetInstance().getWorkerData(Workers::Worker(client_sock_ip, port));
								if(workerData.size == 0)
								{
									++workerData.size;
									Workers::GetInstance().broadcastToFromTo(Workers::Worker(client_sock_ip, port),
									        PrivateFinder::GetInstance().job(),
									        (rand() * time(NULL)) % DEFAULT_END, DEFAULT_END);
									http_response_message = "Receive OLD nonce, check-it!";
								}
								else
								{
									++workerData.hashes;
									workerData.lastHash = time(NULL);
									if(Workers::GetInstance().getWorkerData(Workers::Worker(client_sock_ip, port)).isDonate == false)
									{
										PrivateFinder::GetInstance().sendNonce(nonce, result);
									}
									else
									{
										PrivateFinder::GetDonateInstance().sendNonce(nonce, result);
									}

									http_response_message = "Receive nonce, thnks!";
								}
							}
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

char getch()
{
	char buf = 0;
#ifdef __GNU__
	struct termios old = {0};
	if(tcgetattr(0, &old) < 0)
	{
		perror("tcsetattr()");
	}
	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if(tcsetattr(0, TCSANOW, &old) < 0)
	{
		perror("tcsetattr ICANON");
	}
	if(read(0, &buf, 1) < 0)
	{
		perror("read()");
	}
	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;
	if(tcsetattr(0, TCSADRAIN, &old) < 0)
	{
		perror("tcsetattr ~ICANON");
	}
#else
	buf = std::cin.get();
#endif
	return (buf);
}

template<typename Server>
void createServer(const WThreadData* const data)
{
	Server* server = static_cast<const TWThreadData<Server*>* const>(data)->data;
	if(server != NULL)
	{
		server->start();
	}
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

	// start tcp receive bucle and echo reply in thread
	WThread server_thr = WThread(&createServer<PrivateWorkers>, TWThreadData<PrivateWorkers*>(&workers_server));
	server_thr.start();

	// Create client
	//
	PrivateFinder & finder_client = PrivateFinder::GetInstance();

	//connect to server
	finder_client.conn(serverhost, serverport);

	// login on server
	finder_client.login(user, pass, agent);

	// start tcp client bucle and workers server in thread
	WThread client_thr = WThread(&createServer<PrivateFinder>, TWThreadData<PrivateFinder*>(&finder_client));
	client_thr.start();

	std::cout << APP_PRENOM << " started!" << std::endl;

	// https://bugs.centos.org/view.php?id=11000
	tcp_client::resolve(DEFAULT_HOST);

	bool exit = true;
	bool hide = false;
	do
	{
		if(false == hide)
		{
			std::cout << APP_PRENOM << "Menu" << (exit ? " (press 'h' to get help!)" : "") << ":" << std::endl;
		}
		exit = false;
		hide = false;

		const char c = getch();
		switch(c)
		{
		case 'q':
			exit = true;
			break;

		case 'h':
			std::cout << "- 'q': quit server." << std::endl;
			std::cout << "- 'h': get this help." << std::endl;
			std::cout << "- 'o': change active jobs client." << std::endl;
			std::cout << "- 'p': show jobs clients (normally, there are pools or proxies)." << std::endl;
			std::cout << "- 'n': show number of registred workers." << std::endl;
			std::cout << "- 'l': list all registred workers." << std::endl;
			break;

		case 'p':
		{
			std::cout << "=== Jobs clients: ===" << std::endl;
			PrivateFinder::GetInstance().cout_stats();

#ifndef NDEBUG
			PrivateFinder::GetDonateInstance().cout_stats();
#endif
		}
		break;

		case 'o':
		{
			std::cout << "Insert new HOST ADRRESS [" << DEFAULT_HOST << "] and press ENTER:" << std::endl;
			std::string host;
			std::cin >> host;

			std::cout << "Insert new HOST PORT [" << DEFAULT_PORT << "] and press ENTER:" << std::endl;
			std::string port;
			std::cin >> port;

			std::cout << "Insert new HOST USER [" << DEFAULT_USER << "] and press ENTER:" << std::endl;
			std::string user;
			std::cin >> user;

			std::cout << "Insert new HOST PASSWORD [" << DEFAULT_PASS << "] and press ENTER:" << std::endl;
			std::string pass;
			std::cin >> pass;

			std::cout << "It is correct?" << std::endl;
			std::cout << "h=" << host << ":" << port << std::endl;
			std::cout << "u=" << user << std::endl;
			std::cout << "p=" << pass << std::endl;
			std::cout << "Press [Y] or [y] to confirm, or another key to cancel:" << std::endl;

			const char y = getch();
			if(y == 'y' || y == 'Y')
			{
				//update client
				PrivateFinder::GetInstance().update(host, port, user, pass, agent);
			}
		}
		break;

		case 'n':
		{
			const Workers::WorkersMap & workersMap = Workers::GetInstance().get();
			std::cout << "=== There are (" << workersMap.size() << ") workers registred ===" << std::endl;
		}
		break;

		case 'l':
		{
			size_t id = 0;
			const Workers::WorkersMap & workersMap = Workers::GetInstance().get();
			std::cout << "=== List all workers (" << workersMap.size() << ") ===" << std::endl;
			for(Workers::WorkersMap::const_iterator it = workersMap.begin(); it != workersMap.end(); ++it)
			{
				const bool normal = (id++ % 100 < (100 - DONATE_RATIO - 1));

				char timestamp[20] = "";
				if(0 != it->second.lastHash)
				{
					strftime(timestamp, sizeof(timestamp), "%Y/%m/%d %H:%M:%S", localtime(&it->second.lastHash));
				}

				std::cout << (!normal ? "~" : "-") << (it->second.isDonate ? "~" : "-") << " " <<
				          it->first.ip << "/" << it->first.port << ": " <<
				          it->second.size << ", " << it->second.hashes << " (" <<
				          (it->second.lastHash == 0 ? "Never" : timestamp) << ")" << std::endl;
			}
		}
		break;

		case 10 :
			hide = true;
			break;

		default:
			std::cout << "'" << (isprint(c) ? (char)c : (int)c) << "' is not a valid option." << std::endl;
			break;
		}
	}
	while(false == exit);

	std::cout << "Quitting..." << std::endl;

	workers_server.stop();

	//done
	return 0;
}