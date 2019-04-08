/* FINDER APP (connect to host and broadcast to workers)
 * Start a worker proxy server on tcp port [$1=DEFAULT] to broadcast jobs,
	and connect to host [$2=DEFAULT] on port [$3=DEFAULT] 
						with user [$4=DEFAULT] and password [$5=DEFAULT]
						using as agent [$6=DEFAULT] to get jobs.
 */

#include "Finder.h"

#include "version.h"

#include <signal.h>
#include <stdlib.h>

void my_handler(int s)
{
}

std::string getString(const char* str, std::string def)
{
	if(NULL == str)
	{
		return def;
	}
	return str;
}

int main(int argc, char** argv)
{
	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);

	// Params:
	//
	const int workersport = (argc > 1 ? atoi(argv[1]) : DEFAULT_WORKERS_PORT);
	
	const std::string serverhost = getString(argc > 2 ? argv[2] : NULL, DEFAULT_HOST);
	const int serverport = (argc > 3 ? atoi(argv[3]) : DEFAULT_PORT);
	const std::string user = getString(argc > 4 ? argv[4] : NULL, DEFAULT_USER);
	const std::string pass = getString(argc > 5 ? argv[5] : NULL, DEFAULT_PASS);
	const std::string agent = getString(argc > 6 ? argv[6] : NULL, DEFAULT_AGENT);

	return Finder::Exec(workersport, serverhost, serverport, user, pass, agent);
}
