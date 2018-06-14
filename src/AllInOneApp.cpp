/* FINDER
 * Only find a job from provider.
 */
#include "AllInOne.h"

#include "version.h"

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
	const std::string host = getString(argc > 1 ? argv[1] : NULL, DEFAULT_HOST);
	const int port = (argc > 2 ? atoi(argv[2]) : DEFAULT_PORT);
	const std::string user = getString(argc > 3 ? argv[3] : NULL, DEFAULT_USER);
	const std::string pass = getString(argc > 4 ? argv[4] : NULL, DEFAULT_PASS);
	const std::string agent = getString(argc > 5 ? argv[5] : NULL, DEFAULT_AGENT);

	return AllInOne::Exec(host, port, user, pass, agent);
}
