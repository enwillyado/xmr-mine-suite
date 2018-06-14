/* MINER
 * Only mine a job [$1], iterate nonce target [$2] from start [$3=0] to end [$4=-1].
 */

#include <string>

#include <stdint.h>

class Miner
{
public:
	static int Exec(const std::string & blob, const std::string & target);

	typedef void (*OnNonce)(const uint32_t & nonce, const uint8_t result[32]);
	static int Exec(const std::string & blob, const std::string & target, const OnNonce onNonce,
	                const size_t & started);
};
