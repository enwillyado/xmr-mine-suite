/* MINER
 * Only mine a job [$1], iterate nonce target [$2] from start [$3=0] to end [$4=-1].
 */

#include "net/Job.h"
#include "net/JobResult.h"

#include "Mem.h"

#include "crypto/CryptoNight.h"

#include <iostream>
#include <iomanip>

#define DEFAULT_BLOB "07079deab1d805e410406e6f8ae09b8392a3fb338700da850378889983dd3b19c86a9822219cfc0000000047fe7a15a44870c21862e6e96eab0208ce79a8f5bff4cd2469dc94ccdbe6485b02"
#define DEFAULT_TARGET "e4a63d00"

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
	std::string blob = getString(argc > 1 ? argv[1] : NULL, DEFAULT_BLOB);
	std::string target = getString(argc > 2 ? argv[2] : NULL, DEFAULT_TARGET);

	// Create and set job
	//
	Job job(0, false, xmrig::ALGO_CRYPTONIGHT, xmrig::VARIANT_V1);
	if(!job.setBlob(blob.c_str()))
	{
		std::cerr << "Blob fail" << std::endl;
		return 3;
	}

	if(!job.setTarget(target.c_str()))
	{
		std::cerr << "Target fail" << std::endl;
		return 4;
	}

	// Create result from job
	//
	JobResult result(job);

	// Allocate memory
	//
	if(false == Mem::allocate(job.getAlgo(), 1, false, true))
	{
		std::cerr << "Mem allocate fail" << std::endl;
		return 1;
	}
	cryptonight_ctx* context = Mem::create(0);

	// Initialice algorithm
	//
	if(false == CryptoNight::init(job.getAlgo(), false, job.getVariant()))
	{
		std::cerr << "Algo init fail" << std::endl;
		return 2;
	}

	std::cout << "S:" << std::hex << std::setw(8) << std::setfill('0') << *job.nonce() << ";" << std::endl;

	// Start hashing
	//
	while(true)
	{
		if(CryptoNight::hash(job, result, context))
		{
			// Share match
			//
			std::cout << "M:" << std::hex << std::setw(8) << std::setfill('0') << *job.nonce() << ";" << std::endl;
		}

		++(*job.nonce());
	}

	std::cout << "E:" << std::hex << std::setw(8) << std::setfill('0') << *job.nonce() << ";" << std::endl;

	return 0;
}
