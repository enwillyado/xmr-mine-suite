/* UDP-CLIENT
 */

#ifndef __UDP_CLIENT_H__
#define __UDP_CLIENT_H__

#include <string>

/**
	UDP Client class
*/
class udp_client
{
public:
	static bool send(const std::string & host, const int port, const std::string & message);
};

#endif /* __UDP_CLIENT_H__ */
