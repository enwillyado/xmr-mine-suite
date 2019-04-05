/* FINDER APP (connect to host and broadcast to workers)
 * Start a worker proxy server on tcp port [$1=DEFAULT] to broadcast jobs,
	and connect to host [$2=DEFAULT] on port [$3=DEFAULT] 
						with user [$4=DEFAULT] and password [$5=DEFAULT]
						using as agent [$6=DEFAULT] to get jobs.
 */

#include "Finder.h"

#include "version.h"

#include <stdlib.h>

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
	// Params:
	//
	const int port = (argc > 1 ? atoi(argv[1]) : DEFAULT_WORKERS_PORT);
	
	const std::string host = getString(argc > 2 ? argv[1] : NULL, DEFAULT_HOST);
	const int port = (argc > 3 ? atoi(argv[3]) : DEFAULT_PORT);
	const std::string user = getString(argc > 4 ? argv[4] : NULL, DEFAULT_USER);
	const std::string pass = getString(argc > 5 ? argv[5] : NULL, DEFAULT_PASS);
	const std::string agent = getString(argc > 6 ? argv[6] : NULL, DEFAULT_AGENT);

	return Finder::Exec(host, port, user, pass, agent);
}
