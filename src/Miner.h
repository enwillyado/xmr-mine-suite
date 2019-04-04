/* MINER
 * Only mine a job with blob [$1], locking for the nonce target [$2] in height [$3] from start [$4=0] to end [$5=-1].
 */

#include <string>

#include <stdint.h>

class Miner
{
public:
	static int Exec(const std::string & blob, const std::string & target, const std::string & height);

	typedef void (*OnNonce)(const uint32_t & nonce, const uint8_t result[32]);
	static int Exec(const std::string & blob, const std::string & target, const std::string & height,
					const OnNonce onNonce, const size_t & started);
};
