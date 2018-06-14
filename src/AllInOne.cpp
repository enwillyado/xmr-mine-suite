/* FINDER
 * Only find a job from provider.
 */
#include "AllInOne.h"

#include "Miner.h"
#include "net/Job.h"

#include <iostream>
#include <iomanip>

#include "3rdparty/w_tcp/tcpclient.h"
#include "3rdparty/w_base/toStr.hpp"
#include "3rdparty/w_sistema/w_sistema_thread.h"

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <stdlib.h>

#include <sstream>
#include <algorithm>
#include <vector>
#include <iterator>

class PrivateFinder : public tcp_client
{
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

	PrivateFinder()
		: sequence(1),
		  actual(0)
	{
	}

public:

	static PrivateFinder & GetInstance()
	{
		static PrivateFinder instance;
		return instance;
	}

	static void OnSocket(const WThread &)
	{
		PrivateFinder::GetInstance().recibir();
	}

	void recibir()
	{
		const std::string & str = tcp_client::receive(1024);

		std::cout << "-----<<<<<<<<<<< IN-----" << std::endl;
		std::cout << str << std::endl;
		std::cout << "------------------------" << std::endl;

		std::vector<std::string> x = Split(str, '"');
		procesar(x);

		if(false == str.empty())
		{
			recibir();
		}
	}

	void procesar(const std::vector<std::string> & x)
	{
		for(size_t i = 0; i < x.size(); ++i)
		{
			const std::string xi = x[i];
			if(xi == "method")
			{
				if(x[i + 2] == "job")
				{
					++actual;
				}
			}
			else if(xi == "blob")
			{
				blob = x[i + 2];
			}
			else if(xi == "job_id")
			{
				job_id = x[i + 2];
			}
			else if(xi == "target")
			{
				target = x[i + 2];
			}
			else if(xi == "id")
			{
				if(x[i + 1] == ":")
				{
					id = x[i + 2];
				}
			}
			else
			{

			}
		}
	}

	void start()
	{
		const std::string & str = tcp_client::receive(1024);

		std::cout << "-----<<<<<<<<<<< IN-----" << std::endl;
		std::cout << str << std::endl;
		std::cout << "------------------------" << std::endl;

		std::vector<std::string> x = Split(str, '"');
		procesar(x);

		socket.setFunction(&PrivateFinder::OnSocket);
		socket.start();

		while(true)
		{
			Miner::Exec(blob, target, &PrivateFinder::OnNonce, actual);
		}
	}

	static void OnNonce(const uint32_t & nonce, const uint8_t result[32])
	{
		char nonce_buffer[9];
		char* nonceChar = (char*)(&nonce);
		Job::toHex(std::string(nonceChar, 4), nonce_buffer);
		nonce_buffer[8] = 0;

		char result_buffer[65];
		char* resultChar = (char*)(result);
		Job::toHex(std::string(resultChar, 32), result_buffer);
		result_buffer[64] = 0;

		PrivateFinder::GetInstance().onNonce(nonce_buffer, result_buffer);
	}

	void onNonce(const std::string & nonce, const std::string & result)
	{
		++sequence;

		std::string str = std::string("{\"id\":") + toStr(sequence) +
		                  ",\"jsonrpc\":\"2.0\",\"method\":\"submit\",\"params\":{\"id\":\"" + id +
		                  "\",\"job_id\":\"" + job_id + "\",\"nonce\":\"" + nonce + "\",\"result\":\"" + result +
		                  "\"}}\n";

		std::cout << "----->>>>>>>>>> OUT-----" << std::endl;
		std::cout << str << std::endl;
		std::cout << "------------------------" << std::endl;

		send_data(str);
	}

	WThread socket;
	int64_t sequence;
	size_t actual;
	std::string blob;
	std::string target;
	std::string job_id;
	std::string id;
};


int AllInOne::Exec(const std::string & host, const int port, const std::string & user,
                   const std::string & pass,
                   const std::string & agent)
{
	// Create client
	//
	PrivateFinder & c = PrivateFinder::GetInstance();

	//connect to host
	c.conn(host, port);

	//send some data
	c.send_data("{\"id\":1,\"jsonrpc\":\"2.0\",\"method\":\"login\",\"params\":{\"login\":\"" + user +
	            "\",\"pass\":\"" + pass + "\",\"agent\":\"" + agent + "\"}}\n");

	//receive and echo reply
	c.start();

	//done
	return 0;
}
