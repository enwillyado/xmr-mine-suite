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

void Workers::broadcast(const std::string & job, const bool isDonate)
{
#ifndef NDEBUG
	if(isDonate)
	{
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	}
	else
	{
		std::cout << "------------------------" << std::endl;
	}

	std::cout << job << std::endl;
#endif
	
	size_t ini = 0;
	if(workersMap.size() > 0)
	{
		size_t ids = 0;
		size_t id = 0;
		for (WorkersMap::const_iterator it = workersMap.begin(); it != workersMap.end(); ++it)
		{
			const bool normal = (id % 100 < (100 - DONATE_RATIO - 1));
			if((normal && !isDonate) || (!normal && isDonate)) // normal == isDonate
			{
				++ids;
			}
			
			++id;
		}
		
		const size_t steep = DEFAULT_END / ids;
		id = 0;		
		for (WorkersMap::iterator it = workersMap.begin(); it != workersMap.end(); ++it)
		{
			const bool normal = (id % 100 < (100 - DONATE_RATIO - 1));
			if((normal && !isDonate) || (!normal && isDonate)) // normal == isDonate
			{
				const std::string seek = toStr(ini) + " " + toStr(ini + steep);
#ifndef NDEBUG
				std::cout << it->first.ip << "/" << it->first.port << " : " << seek << std::endl;
#endif
				
				const std::string packet = job + " " + seek;
				
				if(system((std::string("./udp_send.sh '" + it->first.ip + "/" + it->first.port + "' '") + packet + "'").c_str()))
				{
					std::cerr << "Fail to send job to worker: " << it->first.ip + "/" + it->first.port << std::endl;
				}
				
				ini += steep + 1;
			}
			else
			{
				it->second.isDonate = !isDonate;
			}
			
			++id;
		}
	}
	
#ifndef NDEBUG
	if(isDonate)
	{
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	}
	else
	{
		std::cout << "------------------------" << std::endl;
	}
#endif
}

void Workers::add(const Worker & worker)
{
	workersMap[worker] = WorkerData();
	workersMap[worker].size = 1;
}
void Workers::complete(const Worker & worker)
{
	workersMap[worker].size++;
}
void Workers::remove(const Worker & worker)
{
	workersMap.erase(worker);
}

const Workers::WorkerData & Workers::getWorkerData(const Worker & worker) const
{
	return workersMap[worker];
}