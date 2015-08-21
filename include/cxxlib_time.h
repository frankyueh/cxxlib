#pragma once

#include "cxxlib.h"

namespace cxxlib
{
	CXXLIB_EXPORT double tick_freq(double *pdbGetFreq = 0);
	CXXLIB_EXPORT double ticks(double *pdbGetTicks = 0);
	CXXLIB_EXPORT UINT64 utime(UINT64 *pu64GetMicroseconds = 0);
	CXXLIB_EXPORT UINT64 mtime(UINT64 *pu64GetMilliseconds = 0);
	CXXLIB_EXPORT UINT64 time(UINT64 *pu64GetSeconds = 0);
}
