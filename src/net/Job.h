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

#ifndef __JOB_H__
#define __JOB_H__

#include <stddef.h>
#include <stdint.h>
#include <string>

#include "3rdparty/align.h"

enum Variant
{
	VARIANT_AUTO = -1, // Autodetect
	VARIANT_0    =  0,  // Original CryptoNight or CryptoNight-Heavy
	VARIANT_1    =  1,  // CryptoNight variant 1 also known as Monero7 and CryptoNightV7
	VARIANT_2    =  2,  // CryptoNight variant 2
	VARIANT_4    =  4, // CryptoNightR (Monero's variant 4)
	VARIANT_MAX
};

class Job
{
public:
	Job();
	~Job();

	const std::string & getBlobStr() const
	{
		return m_blob_str;
	}

	const std::string & getTargetStr() const
	{
		return m_target_str;
	}

	bool setBlob(const char* blob);
	bool setTarget(const char* target);
	bool setHeight(const uint64_t height);
	void setVariant(Variant variant);

	inline const uint32_t* nonce() const
	{
		return reinterpret_cast<const uint32_t*>(m_blob + 39);
	}
	inline const uint8_t* blob() const
	{
		return m_blob;
	}

	inline Variant variant() const
	{
		return (m_variant == VARIANT_AUTO ? (m_blob[0] >= 10) ? VARIANT_4  :
		        ((m_blob[0] >= 8) ? VARIANT_2  :
		         VARIANT_1) : m_variant);
	}
	inline size_t size() const
	{
		return m_size;
	}
	inline uint32_t* nonce()
	{
		return reinterpret_cast<uint32_t*>(m_blob + 39);
	}
	inline uint64_t target() const
	{
		return m_target;
	}
	inline uint64_t height() const
	{
		return m_height;
	}

	static bool fromHex(const char* in, unsigned int len, unsigned char* out);
	static inline uint32_t* nonce(uint8_t* blob)
	{
		return reinterpret_cast<uint32_t*>(blob + 39);
	}
	static inline uint64_t toDiff(uint64_t target)
	{
		return 0xFFFFFFFFFFFFFFFFULL / target;
	}
	static bool toHex(const std::string & in, char* out);
	static bool toHex(const char* const in, const size_t size, char* out);
	static std::string toHex(const unsigned char* const in, const size_t size);

	bool operator==(const Job & other) const;
	bool operator!=(const Job & other) const;

private:
	std::string m_blob_str;
	std::string m_target_str;
	Variant m_variant;
	size_t m_size;
	uint64_t m_diff;
	uint64_t m_target;
	uint64_t m_height;
	VAR_ALIGN(16,
	          uint8_t m_blob[96]); // Max blob size is 84 (75 fixed + 9 variable), aligned to 96.
};

#endif /* __JOB_H__ */
