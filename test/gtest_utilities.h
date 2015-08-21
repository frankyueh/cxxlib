#pragma once

/// test defines
///

#ifdef cxxlib_test_internal_EXPORTED
#	define CXXLIB_TEST_INTERNAL_EXPORTED
#endif

/// headers
///

#include <ctime>
#include <cmath>
#ifdef _WIN
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	undef WIN32_LEAN_AND_MEAN
#else
#	include <unistd.h>
#	include <sys/time.h>
#endif

/// types defines
///

#if (!defined(_BASETSD_H_) || !defined(_WIN))
typedef signed char				INT8, *PINT8;
typedef signed short			INT16, *PINT16;
typedef signed int				INT32, *PINT32;
typedef signed long long int	INT64, *PINT64;
typedef unsigned char			UINT8, *PUINT8;
typedef unsigned short			UINT16, *PUINT16;
typedef unsigned int			UINT32, *PUINT32;
typedef unsigned long long int	UINT64, *PUINT64;
#endif

/// types limit defines
///

#ifndef 	INT8_MIN
#	define 	INT8_MIN		(-127 - 1)
#endif
#ifndef 	INT8_MAX
#	define 	INT8_MAX		127
#endif
#ifndef 	UINT8_MAX
#	define 	UINT8_MAX		0xff
#endif

#ifndef 	INT16_MIN
#	define 	INT16_MIN		(-32767 - 1)
#endif
#ifndef 	INT16_MAX
#	define 	INT16_MAX		32767
#endif
#ifndef 	UINT16_MAX
#	define 	UINT16_MAX		0xffff
#endif

#ifndef 	INT_MIN
#	define 	INT_MIN			(-2147483647L - 1)
#endif
#ifndef 	INT_MAX
#	define 	INT_MAX			2147483647L
#endif
#ifndef 	UINT_MAX
#	define 	UINT_MAX		0xffffffffUL
#endif

#ifndef 	INT64_MIN
#	define 	INT64_MIN		(-9223372036854775807LL - 1)
#endif
#ifndef 	INT64_MAX
#	define 	INT64_MAX		9223372036854775807LL
#endif
#ifndef 	UINT64_MAX
#	define 	UINT64_MAX		0xffffffffffffffffULL
#endif

#ifndef SIZE_MAX
#	if defined(_WIN64) || defined(__LP64__) || defined(_LP64) || (__WORDSIZE == 64)
#		define SIZE_MAX UINT64_MAX
#	else
#		define SIZE_MAX UINT_MAX
#	endif
#endif

/// thread delaying utilities defines
///

#define GUTIL_MAX_WAITING_MS		(7 * 1000)
#define GUTIL_AVG_MEASURE_MS		150
#define GUTIL_MEASURE_ERR_RANGE		0.25

/// execution time testing utilities defines
///

#define GUTIL_ASSERT_USETIME(TIME_MS, ERR_RATE, EVALCODE)							\
	{																				\
		UINT64 _u64ExpectTime_ = gtest_utility::get_utime() / 1000 + (TIME_MS);		\
		EVALCODE;																	\
		UINT64 _u64ActualTime_ = gtest_utility::get_utime() / 1000;					\
		INT64 _i64ErrorRange_ = static_cast<INT64>((TIME_MS) * (ERR_RATE));			\
		INT64 _i64Difference_ = _u64ActualTime_ - _u64ExpectTime_;					\
		ASSERT_GE((TIME_MS) + _i64Difference_, (TIME_MS) + -1 * _i64ErrorRange_)	\
			<< "Actual execution time is too short.";								\
		ASSERT_LE((TIME_MS) + _i64Difference_, (TIME_MS) + _i64ErrorRange_)			\
			<< "Actual execution time is too long.";								\
	}

/// thread execute waiting utility
///

#define GUTIL_ASSERT_LIMIT(WAIT_MS, TEST_RESULT)							\
	{																		\
		UINT64 _u64WaitUntil_ = gtest_utility::get_mtime() + (WAIT_MS);		\
		do																	\
		{																	\
			if (!(TEST_RESULT))												\
			{																\
				if (_u64WaitUntil_ < gtest_utility::get_mtime())			\
				{															\
					ASSERT_TRUE((TEST_RESULT))								\
						<< ":After " << (WAIT_MS) << "ms passed"			\
						<< " expected test result still not pass!";			\
					break;													\
				}															\
				gtest_utility::thread_yield();								\
			}																\
			else break;														\
		} while (1);														\
	}

#define GUTIL_FOREACH_ASSERT_LIMIT(U, LOOP_COUNT, WAIT_MS, TEST_RESULT)		\
	{																		\
		UINT64 _u64WaitUntil_ = gtest_utility::get_mtime() + (WAIT_MS);		\
		for (unsigned int (U) = 0 ; (U) < (LOOP_COUNT) ; ++(U))				\
		{																	\
			do																\
			{																\
				if (!(TEST_RESULT))											\
				{															\
					if (_u64WaitUntil_ < gtest_utility::get_mtime())		\
					{														\
						ASSERT_TRUE((TEST_RESULT))							\
							<< ":After " << (WAIT_MS)						\
							<< "ms (current loop:" << (U) << ") passed"		\
							<< " expected test result still not pass!";		\
						break;												\
					}														\
					gtest_utility::thread_yield();							\
				}															\
				else break;													\
			} while (1);													\
		}																	\
	}

/// random utility
///

namespace gtest_utility
{
	inline void rand_init() { srand(static_cast<unsigned int>(time(NULL))); }
	inline void rand_init(unsigned int nSeed) { srand(nSeed); }
	inline short int rand_bit() { return rand() & 0x01; }
	inline short int rand_i16() { return rand() & INT16_MAX; }
}

/// gtest_utility definitions
///

namespace gtest_utility
{

#ifdef _WIN
	typedef DWORD		ThreadId;
#else
	typedef pthread_t	ThreadId;
#endif

	inline void thread_msleep(UINT32 nMiliSeconds)
	{
#	ifdef _WIN
		::Sleep(nMiliSeconds);
#	else
		::usleep(nMiliSeconds * 1000);
#	endif
	}

	inline ThreadId thread_get_threadid()
	{
#	ifdef _WIN
		return ::GetCurrentThreadId();
#	else
		return pthread_self();
#	endif
	}

	inline void thread_yield()
	{
#ifdef _WIN
		::SwitchToThread();
#else
		sched_yield();
#endif
	}

#ifdef _WIN
	inline UINT64 get_freq()
	{
		static LARGE_INTEGER g_stPerfFreq = { static_cast<LONGLONG>(0) };
		if (g_stPerfFreq.QuadPart == 0)
			EXPECT_EQ(TRUE, ::QueryPerformanceFrequency(&g_stPerfFreq));
		return g_stPerfFreq.QuadPart;
	}
#endif

	inline UINT64 get_utime()
	{
#	ifdef _WIN
		LARGE_INTEGER stPerfTicks;
		EXPECT_EQ(TRUE, ::QueryPerformanceCounter(&stPerfTicks));
		return static_cast<UINT64>((stPerfTicks.QuadPart * 1000000) / get_freq());
#	else
		struct timespec stTime;
		EXPECT_EQ(0, clock_gettime(CLOCK_MONOTONIC, &stTime));
		return
			static_cast<UINT64>(stTime.tv_sec) * 1000000 +
			static_cast<UINT64>(stTime.tv_nsec / 1000);
#	endif
	}

	inline UINT64 get_mtime() { return get_utime() / 1000; }

}
