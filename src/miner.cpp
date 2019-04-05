/* MINER
 * Only mine a job [$1], iterate nonce target [$2] from start [$3=0] to end [$4=-1].
 */
#include "Miner.h"

#include "net/Job.h"
#include "net/JobResult.h"

#include "crypto/cryptonight.h"

#include <iostream>
#include <iomanip>

int Miner::Exec(const std::string & blob, const std::string & target, const std::string & height)
{
	return Miner::Exec(blob, target, height, NULL, 0);
}

int Miner::Exec(const std::string & blob, const std::string & target, const std::string & height,
				const OnNonce onNonce, const size_t & actual)
{
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

	if(!job.setHeight(atoi(height.c_str())))
	{
		std::cerr << "Height fail" << std::endl;
		return 5;
	}

	struct cryptonight_ctx ctx0, ctx1;
	struct cryptonight_ctx* ctx[2] = {&ctx0, &ctx1};
	
	// Create result from job
	//
	JobResult result(job);

	std::cout << "S:" << std::hex << std::setw(8) << std::setfill('0') << *job.nonce() << ":" << (int)job.variant() << ";" << std::endl;

	// Start hashing
	//
	size_t started = actual;
	while(started == actual)
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

		if(0 == (++(*job.nonce()) % 0x100))
		{
			std::cout << "X:" << std::hex << std::setw(8) << std::setfill('0') << *job.nonce() << ";" << std::endl;
		}
	}

	std::cout << "E:" << std::hex << std::setw(8) << std::setfill('0') << *job.nonce() << ";" << std::endl;

	return 0;
}
