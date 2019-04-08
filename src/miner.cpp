/* MINER
 * Only mine a job [$1], iterate nonce target [$2] from start [$3=0] to end [$4=-1].
 */
#include <iostream>
#include <iomanip>

#include "Miner.h"
#include "version.h"

#include "net/Job.h"
#include "net/JobResult.h"

#include "crypto/cryptonight.h"

int Miner::Exec(const std::string & blob, const std::string & target, const uint64_t & height)
{
	return Miner::Exec(blob, target, height, NULL, DEFAULT_INI, DEFAULT_END);
}

int Miner::Exec(const std::string & blob, const std::string & target, const uint64_t & height,
				const OnNonce onNonce, const uint32_t & ini, const uint32_t & end)
{
#ifndef NDEBUG
	CryptoNight::selfTest();
#endif
	
	// Create and set job
	//
	Job job;
	
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

	if(!job.setHeight(height))
	{
		std::cerr << "Height fail" << std::endl;
		return 5;
	}

	struct cryptonight_ctx ctx0, ctx1;
	struct cryptonight_ctx* ctx[2] = {&ctx0, &ctx1};
	
	// Create result from job
	//
	JobResult result(job);

	// Start hashing
	//
	uint32_t & actual = *job.nonce();
	actual = ini;

	std::cout << "S:" << std::hex << std::setw(8) << std::setfill('0') << *job.nonce() << ":" << (int)job.variant() << ";" << std::endl;
	while(actual <= end)
	{
		if(CryptoNight::hash(job, result, ctx))
		{
			// Share match
			//
			std::cout << "M:" << std::hex << std::setw(8) << std::setfill('0') << *job.nonce() << ";" << std::endl;

			if(NULL != onNonce)
			{
				onNonce(*job.nonce(), result.result);
			}
		}

#ifdef NDEBUG
		if(0 == (++(*job.nonce()) % 0x100))
#endif
		{
			std::cout << "X:" << std::hex << std::setw(8) << std::setfill('0') << *job.nonce() << ";" << std::endl;
		}
	}

	std::cout << "E:" << std::hex << std::setw(8) << std::setfill('0') << *job.nonce() << ";" << std::endl;

	return 0;
}
