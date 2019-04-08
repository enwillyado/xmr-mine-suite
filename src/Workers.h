/* FINDER
 * Only find a job from provider.
 */
#include <string>
#include <map>

class Workers
{
public:	

	typedef std::string Ip;
	typedef std::string Port;
	struct Worker : public std::pair<Ip, Port>
	{
		Worker() : ip(first), port(second)
		{
		}
		Worker(const Port & iIp, const Port & iPort) : ip(first), port(second)
		{
			ip = iIp;
			port = iPort;
		}
		Ip & ip;
		Port & port;
	};
	struct WorkerData
	{
		WorkerData() : size(0), isDonate(false)
		{
		}
		int size;
		bool isDonate;
	};
	typedef std::map<Worker, WorkerData> WorkersMap;
	
	WorkersMap workersMap;
	
	static Workers & GetInstance();

	void broadcast(const std::string & job, const bool isDonate);
	void add(const Worker & worker);
	void complete(const Worker & worker);
	void remove(const Worker & worker);
	
	const WorkerData & getWorkerData(const Worker & worker) const;
	
	inline size_t size() const
	{
		return workersMap.size();
	}
};
