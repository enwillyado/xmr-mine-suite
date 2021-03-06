/* MINER APP
 * Only mine a job [$1], iterate nonce target [$2] from start [$3=0] to end [$4=-1].
 */

#include "Miner.h"

#include "version.h"

std::string getString(const char* str, std::string def)
{
	if(NULL == str)
	{
		return def;
	}
	return str;
}

int main(int argc, char** argv)
{
	// Params:
	//
	const std::string blob = getString(argc > 1 ? argv[1] : NULL, DEFAULT_BLOB);
	const std::string target = getString(argc > 2 ? argv[2] : NULL, DEFAULT_TARGET);

	const uint64_t hight = argc > 3 ? atol(argv[3]) : DEFAULT_HIGHT;

	return Miner::Exec(blob, target, hight);
}
