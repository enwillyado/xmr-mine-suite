/* FINDER
 * Only find a job from provider.
 */
#include "Finder.h"
#include "version.h"

#include <iostream>
#include <iomanip>

#include "3rdparty/w_tcp/tcpclient.h"
#include "3rdparty/w_base/toStr.hpp"

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <stdlib.h>

#ifdef _INC_STDIO
#define popen _popen
#define pclose _pclose
#endif

#include <sstream>
#include <algorithm>
#include <vector>
#include <map>
#include <iterator>

class Workers
{
	typedef std::map<std::string, int> WorkersMap;
	
	WorkersMap workersMap;
	
public:	
	void broadcast(const std::string & job)
	{
		std::cout << "------------------------" << std::endl;
		std::cout << job << std::endl;
		
		size_t ini = 0;
		if(workersMap.size() > 0)
		{
			const size_t steep = DEFAULT_END / workersMap.size();
			
			for (WorkersMap::const_iterator it = workersMap.begin(); it != workersMap.end(); ++it)
			{
				const std::string seek = toStr(ini) + " " + toStr(ini+steep);
				std::cout << it->first << " : " << seek << std::endl;
				
				const std::string packet = job + " " + seek;
				
				if(system((std::string("./udp_send.sh '" + it->first + "' '") + packet + "'").c_str()))
				{
					std::cerr << "Fail to send job to worker: " << it->first << std::endl;
				}
				
				ini += steep + 1;
			}
		}
		
		std::cout << "------------------------" << std::endl;
	}
};


class PrivateFinder : public tcp_client
{
	Workers workers;
	
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

public:
	void receive()
	{
		const std::string & str = tcp_client::receive(1024);
		std::vector<std::string> x = Split(str, '"');

		std::cout << "-<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
		std::cout << str << std::endl;
		std::cout << "------------------------" << std::endl;

		std::string id;
		std::string job_id;
		std::string blob;
		std::string target;
		std::string height;
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
			}
			else if(xi == "target")
			{
				target = x[i + 2];
			}
			else if(xi == "id")
			{
				id = x[i + 2];
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

		const std::string job = blob + " " + target + " " + height + " ";
		workers.broadcast(job);
		
		receive();
	}
};


int Finder::Exec(const std::string & host, const int port, const std::string & user, const std::string & pass,
                 const std::string & agent)
{
	// Create client
	//
	PrivateFinder c;

	//connect to host
	c.conn(host, port);
	
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