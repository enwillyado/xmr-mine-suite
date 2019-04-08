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
	typedef std::pair<const Ip &, const Port &> WorkerId;
	struct Worker
	{
		inline bool operator<(const Worker & rhs) const
		{
			return WorkerId(ip, port) < WorkerId(rhs.ip, rhs.port);
		}
		Worker() : ip(""), port("")
		{
		}
		Worker(const Port & iIp, const Port & iPort) : ip(iIp), port(iPort)
		{
		}
		Ip ip;
		Port port;
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
