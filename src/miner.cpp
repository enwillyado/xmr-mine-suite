/* MINER
 * Only mine a job [$1], iterate nonce target [$2] from start [$3=0] to end [$4=-1].
 */
#include "Miner.h"

#include "net/Job.h"
#include "net/JobResult.h"

#include "Mem.h"

#include "crypto/CryptoNight.h"

#include <iostream>
#include <iomanip>

int Miner::Exec(const std::string & blob, const std::string & target)
{
	return Miner::Exec(blob, target, NULL, 0);
}

int Miner::Exec(const std::string & blob, const std::string & target, const OnNonce onNonce,
                const size_t & actual)
{
	// Create and set job
	//
	Job job(0, false, xmrig::ALGO_CRYPTONIGHT, xmrig::VARIANT_V1, xmrig::MODE_CPU);
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
	if(false == CryptoNight::init(job.getAlgo(), false, job.getMode()))
	{
		std::cerr << "Algo init fail" << std::endl;
		return 2;
	}

	std::cout << "S:" << std::hex << std::setw(8) << std::setfill('0') << *job.nonce() << ";" << std::endl;

	// Start hashing
	//
	size_t started = actual;
	while(started == actual)
	{
		if(CryptoNight::hash(job, result, context))
		{
			// Share match
			//
			std::cout << "M:" << std::hex << std::setw(8) << std::setfill('0') << *job.nonce() << ";" << std::endl;

			if(NULL != onNonce)
			{
				onNonce(*job.nonce(), result.result);
			}
		}

		if(0 == (++(*job.nonce()) % 0x100))
		{
			std::cout << "X:" << std::hex << std::setw(8) << std::setfill('0') << *job.nonce() << ";" << std::endl;
		}
	}

	std::cout << "E:" << std::hex << std::setw(8) << std::setfill('0') << *job.nonce() << ";" << std::endl;

	return 0;
}
