#pragma once

#include "cxxlib.h"

namespace cxxlib
{
	CXXLIB_EXPORT size_t vsprintf(char *szDst, size_t mSize, const char *szFormat, va_list pchArgs);
	CXXLIB_EXPORT size_t sprintf(char *szDst, size_t mSize, const char *szFormat, ...);
	CXXLIB_EXPORT size_t strcpy_n(char *szDst, const size_t &mDstLen, const char* szSrc);
	CXXLIB_EXPORT size_t strcpy_m(char *szDst, const size_t &mDstLen, const char* pcSrc, const size_t &mSrcLen);
}
