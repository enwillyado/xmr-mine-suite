/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2018 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2018      Lee Clagett <https://github.com/vtnerd>
 * Copyright 2016-2018 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
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

#include "crypto/cryptonight.h"

#include "net/Job.h"
#include "net/JobResult.h"

#include <cstdio>

bool CryptoNight::hash(const Job & job, JobResult & result, cryptonight_ctx** ctx)
{
	switch(job.variant())
	{
	case VARIANT_0:
		return hash_t<VARIANT_0>(job, result, ctx);
	case VARIANT_1:
		return hash_t<VARIANT_1>(job, result, ctx);
	case VARIANT_2:
		return hash_t<VARIANT_2>(job, result, ctx);
	case VARIANT_4:
		return hash_t<VARIANT_4>(job, result, ctx);

	case VARIANT_AUTO:
	default:
		fprintf(stderr, "Invalid VARIANT!\n");
		abort();
	}

	return false;
}

template<Variant VARIANT>
bool CryptoNight::hash_t(const Job & job, JobResult & result, cryptonight_ctx** ctx)
{
#ifndef NDEBUG
	fprintf(stderr, "Hash with %d variant.\n", VARIANT);
#endif

	CryptoNight::cryptonight_hash_ctx<VARIANT>(result.result, job.blob(), job.size(), ctx, job.height());

	return *reinterpret_cast<uint64_t*>(result.result + 24) < job.target();
}

bool CryptoNight::selfTest()
{
	return cryptonight_test();
}
