
#include "cxxlib_time.h"

#include <ctime>
#ifdef _WIN
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	undef WIN32_LEAN_AND_MEAN
#else
#	include <sys/time.h>
#endif

namespace cxxlib
{

	double tick_freq(double *pdbGetFreq)
	{
		double dbGetFreq;

#	ifdef _WIN

		LARGE_INTEGER stPerfFreq;
		BOOL bResult = ::QueryPerformanceFrequency(&stPerfFreq);
		CXXLIB_ASSERT_X(bResult == TRUE, "QueryPerformanceFrequency()");
		dbGetFreq = static_cast<double>(stPerfFreq.QuadPart);

#	else

		dbGetFreq = 1000000000.0;

#	endif

		if (pdbGetFreq)
			*pdbGetFreq = dbGetFreq;

		return dbGetFreq;
	}

	double ticks(double *pdbGetTicks)
	{
		double dbGetTicks;

#	ifdef _WIN

		LARGE_INTEGER stPerfTicks;
		BOOL bResult = ::QueryPerformanceCounter(&stPerfTicks);
		CXXLIB_ASSERT_X(bResult == TRUE, "QueryPerformanceCounter()");
		dbGetTicks = static_cast<double>(stPerfTicks.QuadPart);

#	else

		struct timespec stTime;
		int iResult = clock_gettime(CLOCK_MONOTONIC, &stTime);
		CXXLIB_ASSERT_X(iResult == 0, "clock_gettime()");
		dbGetTicks =
			static_cast<double>(stTime.tv_sec) * 1000000000.0 +
			static_cast<double>(stTime.tv_nsec);

#	endif

		if (pdbGetTicks)
			*pdbGetTicks = dbGetTicks;

		return dbGetTicks;
	}

	UINT64 utime(UINT64 *pu64GetMicroseconds)
	{
		UINT64 u64GetMicroseconds;

#	ifdef _WIN

		// initiate performance tick frequency
		static double s_dbTickFreq = tick_freq(NULL);

		// get system time stamp and performance tick count
		UINT64 u64SysTimeStamp = static_cast<UINT64>(::time(NULL)) * 1000000;
		u64GetMicroseconds = static_cast<UINT64>((ticks(NULL) / s_dbTickFreq) * 1000000.0);

		// initiate time stamp offset with microsecond tick count for first call
		static INT64 s_i64TimeStampOffset =
			u64SysTimeStamp - u64GetMicroseconds + (u64GetMicroseconds % 1000000);

		// get microsecond time stamp value by the calculated time offset
		u64GetMicroseconds += s_i64TimeStampOffset;

		// check does microsecond time stamp value has shifted away from system one,
		// and recalibrate it if does.

		INT64 i64TimeStampDiff =
			(u64GetMicroseconds - (u64GetMicroseconds % 1000000)) - u64SysTimeStamp;

		if (i64TimeStampDiff < -1000000 || i64TimeStampDiff > 1000000)
		{
			u64GetMicroseconds -= s_i64TimeStampOffset;
			s_i64TimeStampOffset = u64SysTimeStamp - u64GetMicroseconds + (u64GetMicroseconds % 1000000);
			u64GetMicroseconds += s_i64TimeStampOffset;
		}

#	else

		struct timeval stNow;
		CXXLIB_CHECK_X(
			gettimeofday(&stNow, NULL) == 0,
			"utime",
			"gettimeofday");
		u64GetMicroseconds = (static_cast<UINT64>(stNow.tv_sec) * 1000000) + stNow.tv_usec;

#	endif

		if (pu64GetMicroseconds)
			*pu64GetMicroseconds = u64GetMicroseconds;

		return u64GetMicroseconds;
	}

	UINT64 mtime(UINT64 *pu64GetMilliseconds)
	{
		UINT64 u64 = (utime(NULL) / 1000);
		if (pu64GetMilliseconds) *pu64GetMilliseconds = u64;
		return u64;
	}

	UINT64 time(UINT64 *pu64GetSeconds)
	{
		UINT64 u64 = (utime(NULL) / 1000000);
		if (pu64GetSeconds) *pu64GetSeconds = u64;
		return u64;
	}

}
