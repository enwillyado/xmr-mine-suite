/* FINDER
 * Only find a job from provider.
 */
#include "Finder.h"

#include <iostream>
#include <iomanip>

#include "3rdparty/w_tcp/tcpclient.h"

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

public:
	void receive()
	{
		const std::string & str = tcp_client::receive(1024);
		std::vector<std::string> x = Split(str, '"');

		std::cout << "------------------------" << std::endl;
		std::cout << str << std::endl;
		std::cout << "------------------------" << std::endl;

		std::string id;
		std::string job_id;
		std::string blob;
		std::string target;
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
			else
			{

			}
		}

		exec("miner-x64x.exe " + blob + " " + target, blob, target, job_id, id);
	}

	int exec(const std::string & cmd, const std::string & blob, const std::string & target,
	         const std::string & job_id, const std::string & id)
	{
		char buffer[12];
		std::string result = "";
		FILE* pipe = popen(cmd.c_str(), "r");
		if(!pipe)
		{
			return 1;
		}
		try
		{
			while(!feof(pipe))
			{
				if(fgets(buffer, sizeof(buffer), pipe) != NULL)
				{
					const std::string str = buffer;
					const std::size_t posEnd = str.find_first_of(';');           // position of "end of line" in str
					if(std::string::npos != posEnd)
					{
						const std::string comando = str.substr(0, posEnd);       // get from "live" to the end
						const std::size_t pos = comando.find_first_of(':');      // position of "separator" in str
						if(std::string::npos != pos)
						{
							const std::string action = comando.substr(0, pos);   // get from "live" to the end
							const std::string value = comando.substr(pos + 1);   // get from "live" to the end

							if(action == "S")
							{
								std::cout << value << std::endl;
							}
							else if(action == "M")
							{
								//send some data
								send_data("{\"id\":2,\"jsonrpc\":\"2.0\",\"method\":\"submit\",\"params\":{\"id\":\"" + id +
								          " \",\"job_id\":\"" + job_id + "\",\"nonce\":\"" + value + "\",\"result\":\"" + result + "\"}}\n");

								// receive response
								receive();
							}
							else if(action == "E")
							{

							}
							else if(action == "X")
							{

							}
							else
							{
								std::cout << buffer;
							}
						}
						else
						{
							std::cout << buffer;
						}
					}
					else
					{
						std::cout << buffer;
					}

					result += buffer;
				}
			}
		}
		catch(...)
		{
			pclose(pipe);
			return 2;
		}
		pclose(pipe);
		return 0;
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

	//send some data
	c.send_data("{\"id\":1,\"jsonrpc\":\"2.0\",\"method\":\"login\",\"params\":{\"login\":\"" + user +
	            "\",\"pass\":\"" + pass + "\",\"agent\":\"" + agent + "\"}}\n");

	//receive and echo reply
	c.receive();

	//done
	return 0;
}