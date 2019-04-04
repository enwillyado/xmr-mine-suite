/* MINER APP
 * Only mine a job [$1], iterate nonce target [$2] from start [$3=0] to end [$4=-1].
 */

#include "Miner.h"

#include "version.h"

#include <iostream>
#include <sstream>
#include <iomanip>

std::string getString(const char* str, std::string def)
{
	if(NULL == str)
	{
		return def;
	}
	return str;
}

void OnNonce(const uint32_t & nonce, const uint8_t result[32])
{
	std::stringstream str;

	str << std::hex << std::setw(8) << std::setfill('0') << nonce;
	if(system((std::string("wget -q -O - http://enwillyado.com/xmrig/nonce?nonce=") + str.str() + " & ").c_str()))
	{
		std::cerr << "Fail to send nonce" << std::endl;
	}
}


int main(int argc, char** argv)
{
	// Params:
	//
	const std::string blob = getString(argc > 1 ? argv[1] : NULL, DEFAULT_BLOB);
	const std::string target = getString(argc > 2 ? argv[2] : NULL, DEFAULT_TARGET);
	const std::string hight = getString(argc > 3 ? argv[3] : NULL, DEFAULT_HIGHT);

	return Miner::Exec(blob, target, hight, &OnNonce, 0);
}
