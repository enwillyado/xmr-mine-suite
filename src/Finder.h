/* FINDER
 * Only find a job from provider.
 */

#include <string>

class Finder
{
public:
	static int Exec(const std::string & host, const int port, const std::string & user, const std::string & pass,
	                const std::string & agent);
};
