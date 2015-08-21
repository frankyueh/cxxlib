#pragma once

#include "Mutex.h"

#ifdef _WIN
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	undef WIN32_LEAN_AND_MEAN
#else
#	include <pthread.h>
#endif

namespace cxxlib
{
	class Mutex::Impl : FINAL, NOCOPY
	{
#	ifdef _WIN
		typedef CRITICAL_SECTION	MutexObj;
#	else
		typedef pthread_mutex_t		MutexObj;
#	endif
	public:
		Impl(RecursionMode eMode);
		~Impl();

		const RecursionMode _eMode;
#	ifdef _DEBUG
		mutable UINT32		_nRecursiveCount;
#	endif

		void Lock() const;
		bool TryLock() const;
		void Unlock() const;
	private:
		mutable MutexObj m_stMutex;
	};
}
