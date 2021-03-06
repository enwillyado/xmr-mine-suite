/* WORKER APP
 * Only mine a job [$1], iterate nonce target [$2] from start [$3=0] to end [$4=-1].
 */

#include "Miner.h"

#include "version.h"
#include "net/Job.h"

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
#ifdef NONCE_BASE0
	std::stringstream nonce_str;

	nonce_str << std::hex << std::setw(8) << std::setfill('0') << nonce;
	nonce_str.flush();
	
	const std::string nonce_hex_str = nonce_str.str();
#else	
	const std::string nonce_hex_str = Job::toHex(reinterpret_cast<const unsigned char*>(&nonce), 4);
#endif
	const std::string result_hex_str = Job::toHex(reinterpret_cast<const unsigned char*>(result), 32);

#ifndef NDEBUG
	std::cout << "Nonce: " << nonce_hex_str << " & result: " << result_hex_str << std::endl;
#endif
	
	if(system((std::string("./tcp_send.sh /xmrig/proxy?\\&q=nonce\\&nonce=") + nonce_hex_str + "\\&result=" + result_hex_str + "\\&").c_str()))
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
	
	const uint64_t hight = argc > 3 ? atol(argv[3]) : DEFAULT_HIGHT;
	
	const uint32_t ini = argc > 4 ? atol(argv[4]) : DEFAULT_INI;
	const uint32_t end = argc > 5 ? atol(argv[5]) : DEFAULT_END;

	return Miner::Exec(blob, target, hight, &OnNonce, ini, end);
}
