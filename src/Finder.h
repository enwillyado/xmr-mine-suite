/* FINDER
 * Only find a job from provider.
 */

#include <string>

class Finder
{
public:
	static int Exec(const int workers_tcp_port,
					const std::string & serverhost, const int serverport, const std::string & user, const std::string & pass,
	                const std::string & agent);
};
