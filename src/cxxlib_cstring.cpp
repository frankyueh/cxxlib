
#include "cxxlib_cstring.h"

#include <cstdarg>
#include <errno.h>

#ifdef _WIN
#	pragma warning (disable: 4996)
#endif

namespace cxxlib
{
	size_t vsprintf(char *szDst, size_t mSize, const char *szFormat, va_list pchArgs)
	{
		if (mSize <= 0)
			return 0;
#	ifdef _WIN
		// NOTE: _vsnprintf_s is not safe at all, it will rise assertion if input format error...
		int iCnt = ::vsnprintf(szDst, mSize, szFormat, pchArgs);
		if (iCnt < 0)
		{
			iCnt = (errno == ERANGE ? static_cast<int>(mSize - 1) : 0);
			szDst[iCnt] = 0;
		}
#	else
		int iCnt = ::vsnprintf(szDst, mSize, szFormat, pchArgs);
		if (iCnt < 0)
		{
			iCnt = 0;
			szDst[iCnt] = 0;
		}
		else if (static_cast<size_t>(iCnt) > mSize - 1)
		{
			iCnt = static_cast<int>(mSize - 1);
		}
#	endif
		return static_cast<size_t>(iCnt);
	}

	size_t sprintf(char *szDst, size_t mSize, const char *szFormat, ...)
	{
		va_list pchArgs;
		va_start(pchArgs, szFormat);
		size_t mCnt = cxxlib::vsprintf(szDst, mSize, szFormat, pchArgs);
		va_end(pchArgs);
		return mCnt;
	}

	size_t strcpy_n(char *szDst, const size_t &mDstLen, const char *szSrc)
	{
		size_t mSrcLen = strlen(szSrc);
		if (mSrcLen <= mDstLen)
		{
			memcpy(szDst, szSrc, mSrcLen + 1);
		}
		else
		{
			memcpy(szDst, szSrc, mDstLen);
			szDst[mDstLen] = 0;
		}
		return mSrcLen;
	}

	size_t strcpy_m(char *szDst, const size_t &mDstLen, const char *pcSrc, const size_t &mSrcLen)
	{
		size_t mLen = mSrcLen < mDstLen ? mSrcLen : mDstLen;
		memcpy(szDst, pcSrc, mLen);
		szDst[mDstLen] = 0;
		return mLen;
	}
}