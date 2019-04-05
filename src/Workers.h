/* FINDER
 * Only find a job from provider.
 */
#include <string>
#include <map>

class Workers
{
	typedef std::string Worker;
	typedef std::map<Worker, int> WorkersMap;
	
	WorkersMap workersMap;
	
public:	
	static Workers & GetInstance();

	void broadcast(const std::string & job);
	void add(const Worker & worker);
	void remove(const Worker & worker);
};
