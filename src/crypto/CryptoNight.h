#ifndef __CRYPTONIGHT_H_INCLUDED
#define __CRYPTONIGHT_H_INCLUDED

#include <stddef.h>

#include "net/Job.h"
#include "net/JobResult.h"

extern "C"
{
	#if defined(__arm__) || defined(_MSC_VER)
	#ifndef NOASM
	#define NOASM
	#endif
	#endif

	#if __GNUC__ <= 5 && __GNUC_MINOR__ <= 4
	#define alignas(x)  
	#define constexpr
	#define static_assert
	#endif

	#include "crypto/c_keccak.h"
	#include "crypto/c_groestl.h"
	#include "crypto/c_blake256.h"
	#include "crypto/c_jh.h"
	#include "crypto/c_skein.h"
	#include "crypto/int-util.h"
	//#include "crypto/hash-ops.h"

	#if USE_INT128

	#if __GNUC__ == 4 && __GNUC_MINOR__ >= 4 && __GNUC_MINOR__ < 6
	typedef unsigned int uint128_t __attribute__ ((__mode__ (TI)));
	#elif defined (_MSC_VER)
	/* only for mingw64 on windows */
	#undef  USE_INT128
	#define USE_INT128 (0)
	#else
	typedef __uint128_t uint128_t;
	#endif

	#endif

	#define LITE 0
	#if LITE /* cryptonight-light */
	#define MEMORY (1 << 20)
	#define ITER   (1 << 19)
	#else
	#define MEMORY (1 << 21) /* 2 MiB */
	#define ITER   (1 << 20)
	#endif

	#define AES_BLOCK_SIZE  16
	#define AES_KEY_SIZE    32 /*16*/
	#define INIT_SIZE_BLK   8
	#define INIT_SIZE_BYTE (INIT_SIZE_BLK * AES_BLOCK_SIZE)
	
	#if defined _MSC_VER || defined XMRIG_ARM
	#define ABI_ATTRIBUTE
	#else
	#define ABI_ATTRIBUTE __attribute__((ms_abi))
	#endif

	#include <unistd.h>

	#ifdef __GNUC__
	#   include <x86intrin.h>
	#else
	#   include <intrin.h>
	#   define __restrict__ __restrict
	#endif

	#include "crypto/c_groestl.h"
	#include "crypto/c_blake256.h"
	#include "crypto/c_jh.h"
	#include "crypto/c_skein.h"

	struct cryptonight_ctx;
	typedef void(*cn_mainloop_fun_ms_abi)(cryptonight_ctx*) ABI_ATTRIBUTE;

	struct cryptonight_r_data
	{
		int variant;
		uint64_t height;

		bool match(const int v, const uint64_t h) const { return (v == variant) && (h == height); }
	};

	struct cryptonight_ctx
	{
		
		cryptonight_ctx();
		
		~cryptonight_ctx();
		
		alignas(16) uint8_t state[224];
		alignas(16) uint8_t *memory;

		uint8_t unused[40];
		const uint32_t* saes_table;

		cn_mainloop_fun_ms_abi generated_code;
		cryptonight_r_data generated_code_data;
	};
}

constexpr const size_t   CRYPTONIGHT_MEMORY = 2 * 1024 * 1024;
constexpr const uint32_t CRYPTONIGHT_MASK   = 0x1FFFF0;
constexpr const uint32_t CRYPTONIGHT_ITER   = 0x80000;

class CryptoNight
{
public:
	static bool selfTest();
	static bool hash(const Job & job, JobResult & result, cryptonight_ctx** ctx);
	
	template<Variant VARIANT>
	static bool hash_t(const Job & job, JobResult & result, cryptonight_ctx** ctx);
	
private:
	static bool cryptonight_test();
	
	template<Variant VARIANT>
	static void cryptonight_hash_ctx(uint8_t *__restrict__ output, const uint8_t *__restrict__ input, size_t size, cryptonight_ctx **__restrict__ ctx, const uint64_t height);
};

#endif

