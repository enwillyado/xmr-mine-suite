/* AllInOne
 * Find a job from provider and start mining.
 */

#include <string>

class AllInOne
{
public:
	static int Exec(const std::string & host, const int port, const std::string & user, const std::string & pass,
	                const std::string & agent);
};
