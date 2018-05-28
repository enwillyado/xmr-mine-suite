/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2016-2017 XMRig       <support@xmrig.com>
 *
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
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

	// Start hashing
	//
	while(true)
	{
		if(CryptoNight::hash(job, result, context))
		{
			// Share match
			//
			std::cout << "Matched: " << std::hex << std::setw(8) << std::setfill('0') << *job.nonce() << std::endl;
		}

		++(*job.nonce());
	}

	return 0;
}
