/* Workers
 * Broadcast jobs.
 */

#include "Workers.h"
#include "version.h"

#include <iostream>
#include <map>
#include <iterator>

#include "3rdparty/w_base/toStr.hpp"

Workers & Workers::GetInstance()
{
	static Workers workers;
	return workers;
}

void Workers::broadcast(const std::string & job)
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
			
			if(system((std::string("./udp_send.sh '" + it->first + "/" + it->second.port + "' '") + packet + "'").c_str()))
			{
				std::cerr << "Fail to send job to worker: " << it->first << std::endl;
			}
			
			ini += steep + 1;
		}
	}
	
	std::cout << "------------------------" << std::endl;
}

void  Workers::add(const Worker & worker, const Port & port)
{
	workersMap[worker] = WorkerData(port);
}
void Workers::complete(const Worker & worker)
{
	workersMap[worker].size++;
}
void Workers::remove(const Worker & worker)
{
	workersMap.erase(worker);
}
