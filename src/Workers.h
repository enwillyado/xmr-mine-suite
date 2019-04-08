/* FINDER
 * Only find a job from provider.
 */
#include <string>
#include <map>

class Workers
{
	typedef std::string Worker;
	typedef std::string Port;
	struct WorkerData
	{
		WorkerData() : port(""), size(0)
		{
		}
		WorkerData(const Port & iPort) : port(iPort), size(1)
		{
		}
		Port port;
		int size;
	};
	typedef std::map<Worker, WorkerData> WorkersMap;
	
	WorkersMap workersMap;
	
public:	
	static Workers & GetInstance();

	void broadcast(const std::string & job, const bool isDonate);
	void add(const Worker & worker, const Port & port);
	void complete(const Worker & worker);
	void remove(const Worker & worker);
	
	inline size_t size() const
	{
		return workersMap.size();
	}
};
