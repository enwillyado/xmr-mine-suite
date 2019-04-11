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
		for(WorkersMap::const_iterator it = workersMap.begin(); it != workersMap.end(); ++it)
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
		for(WorkersMap::iterator it = workersMap.begin(); it != workersMap.end(); ++it)
		{
			const bool normal = (id % 100 < (100 - DONATE_RATIO - 1));
			if((normal && !isDonate) || (!normal && isDonate)) // normal == isDonate
			{
				broadcastTo(it->first, job, ini, steep);

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

void Workers::broadcastTo(const Worker & worker, const std::string & job, const size_t ini, const size_t steep)
{
	broadcastToFromTo(worker, job, ini, ini + steep);
}
void Workers::broadcastToFromTo(const Worker & worker, const std::string & job, const size_t ini,
                                const size_t fini)
{
	const std::string seek = toStr(ini) + " " + toStr(fini);
#ifndef NDEBUG
	std::cout << worker.ip << "/" << worker.port << " : " << seek << std::endl;
#endif

	const std::string packet = job + " " + seek;

	if(system((std::string("./udp_send.sh '" + worker.ip + "/" + worker.port + "' '") + packet + "'").c_str()))
	{
		std::cerr << "Fail to send job to worker: " << worker.ip + "/" + worker.port << std::endl;
	}
}

void Workers::add(const Worker & worker)
{
	workersMap[worker] = WorkerData();
	workersMap[worker].size = 1;
}
void Workers::complete(const Worker & worker)
{
	++workersMap[worker].size;
}
void Workers::remove(const Worker & worker)
{
	workersMap.erase(worker);
}
