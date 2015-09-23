
#ifndef _WIN
#	include <sys/time.h>
#endif

#include "EventSignal.h"

namespace cxxlib
{

	// BaseSignal
	//-----------------------------------------------------------------------------

	BaseSignal::BaseSignal(bool bExclusive)
#	ifdef _WIN
#	else
		: m_bSignaled(false)
		, m_bExclusive(bExclusive)
		, m_bAutoReset(false)
#	endif
	{
#	ifdef _WIN
		CXXLIB_CHECK_X(
			(m_cEvent = CreateEvent(0, !bExclusive, false, 0)) != INVALID_HANDLE_VALUE,
			"EventSignal",
			"CreateEvent");
#	else
		pthread_condattr_t stEventAttr;
		pthread_mutexattr_t stMutexAttr;
		CXXLIB_CHECK_X(
			pthread_condattr_init(&stEventAttr) == 0,
			"BaseSignal::_constructor",
			"pthread_condattr_init");
		CXXLIB_CHECK_X(
			pthread_mutexattr_init(&stMutexAttr) == 0,
			"BaseSignal::_constructor",
			"pthread_mutexattr_init");
		CXXLIB_CHECK_X(
			pthread_cond_init(&m_cEvent, &stEventAttr) == 0,
			"BaseSignal::_constructor",
			"pthread_cond_init");
		CXXLIB_CHECK_X(
			pthread_mutex_init(&m_cMutex, &stMutexAttr) == 0,
			"BaseSignal::_constructor",
			"pthread_mutex_init");
		CXXLIB_CHECK_X(
			pthread_condattr_destroy(&stEventAttr) == 0,
			"BaseSignal::_constructor",
			"pthread_condattr_destroy");
		CXXLIB_CHECK_X(
			pthread_mutexattr_destroy(&stMutexAttr) == 0,
			"BaseSignal::_constructor",
			"pthread_mutexattr_destroy");
#	endif
	}

	BaseSignal::~BaseSignal()
	{
#	ifdef _WIN
		CloseHandle(m_cEvent);
#	else
		CXXLIB_CHECK_X(
			pthread_cond_destroy(&m_cEvent) == 0,
			"BaseSignal::_destructor",
			"pthread_cond_destroy");
		CXXLIB_CHECK_X(
			pthread_mutex_destroy(&m_cMutex) == 0,
			"BaseSignal::_destructor",
			"pthread_mutex_destroy");
#	endif
	}

	bool BaseSignal::IsSet() const
	{
#	ifdef _WIN
		return WaitForSingleObject(m_cEvent, 0) == WAIT_OBJECT_0;
#	else
		CXXLIB_CHECK_X(
			pthread_mutex_lock(&m_cMutex) == 0,
			"BaseSignal::IsSet",
			"pthread_mutex_lock");
		bool bRet = m_bSignaled && (m_bExclusive || !m_bAutoReset);
		CXXLIB_CHECK_X(
			pthread_mutex_unlock(&m_cMutex) == 0,
			"BaseSignal::IsSet",
			"pthread_mutex_unlock");
		return bRet;
#	endif
	}

	void BaseSignal::Set(bool bAutoReset) const
	{
#	ifdef _WIN
		if (!bAutoReset)
			SetEvent(m_cEvent);
		else
			PulseEvent(m_cEvent);
#	else
		CXXLIB_CHECK_X(
			pthread_mutex_lock(&m_cMutex) == 0,
			"BaseSignal::Set",
			"pthread_mutex_lock");

		m_bSignaled = true;
		if (m_bExclusive)
		{
			CXXLIB_CHECK_X(
				pthread_cond_signal(&m_cEvent) == 0,
				"BaseSignal::Set",
				"pthread_cond_signal");
		}
		else
		{
			m_bAutoReset = bAutoReset;

			CXXLIB_CHECK_X(
				pthread_cond_broadcast(&m_cEvent) == 0,
				"BaseSignal::Set",
				"pthread_cond_broadcast");
		}

		CXXLIB_CHECK_X(
			pthread_mutex_unlock(&m_cMutex) == 0,
			"BaseSignal::Set",
			"pthread_mutex_unlock");
#	endif
	}

	void BaseSignal::Reset() const
	{
#	ifdef _WIN
		ResetEvent(m_cEvent);
#	else
		CXXLIB_CHECK_X(
			pthread_mutex_lock(&m_cMutex) == 0,
			"BaseSignal::Reset",
			"pthread_mutex_lock");
		m_bSignaled = false;
		CXXLIB_CHECK_X(
			pthread_mutex_unlock(&m_cMutex) == 0,
			"BaseSignal::Reset",
			"pthread_mutex_unlock");
#	endif
	}

	bool BaseSignal::Wait(unsigned int uTimeoutMS) const
	{
#	ifdef _WIN
		bool bRet = WaitForSingleObject(m_cEvent, uTimeoutMS) == WAIT_OBJECT_0;
		CXXLIB_ASSERT((uTimeoutMS == WAIT_INFINITE && bRet) || uTimeoutMS != WAIT_INFINITE);
#	else
		CXXLIB_CHECK_X(
			pthread_mutex_lock(&m_cMutex) == 0,
			"BaseSignal::Wait",
			"pthread_mutex_lock");

		bool bRet = true;
		if (!m_bSignaled || m_bAutoReset)
		{
			if (uTimeoutMS != WAIT_INFINITE)
			{
				struct timeval stTimeVal;
				CXXLIB_CHECK_X(
					gettimeofday(&stTimeVal, NULL) == 0,
					"BaseSignal::Wait",
					"gettimeofday");

				struct timespec stTimeSpec;
				stTimeSpec.tv_nsec = stTimeVal.tv_usec * 1000 + (uTimeoutMS > 0 ? (uTimeoutMS % 1000) * 1000000 : 0);
				stTimeSpec.tv_sec = stTimeVal.tv_sec + (uTimeoutMS > 0 ? uTimeoutMS / 1000 : 0);

				if (stTimeSpec.tv_nsec >= 1000000000)
				{
					stTimeSpec.tv_nsec %= 1000000000;
					stTimeSpec.tv_sec++;
				}

				// NOTE: Sometimes pthread_cond_timedwait() may not
				//		 return 0 right after other thread had signal it.
				//		 (happens when waited time set to 0)
				bRet =
					pthread_cond_timedwait(&m_cEvent, &m_cMutex, &stTimeSpec) == 0 ||
					(m_bExclusive && m_bSignaled);
			}
			else
			{
				CXXLIB_CHECK_X(
					pthread_cond_wait(&m_cEvent, &m_cMutex) == 0,
					"BaseSignal::Wait",
					"pthread_cond_wait(WAIT_INFINITE)");

				bRet = true;
			}
		}
		if (m_bExclusive)
			m_bSignaled = false;

		CXXLIB_CHECK_X(
			pthread_mutex_unlock(&m_cMutex) == 0,
			"BaseSignal::Wait",
			"pthread_mutex_unlock");
#	endif
		return bRet;
	}


	// CondSignal
	//-----------------------------------------------------------------------------

#ifdef _WIN

	CondSignal::CondSignal() : BaseSignal(true)
	{
		InitializeCriticalSection(&m_stMutex);
	}

	CondSignal::~CondSignal()
	{
		DeleteCriticalSection(&m_stMutex);
	}

	bool CondSignal::IsSet() const
	{
		EnterCriticalSection(&m_stMutex);
		bool bRet = WaitForSingleObject(m_cEvent, 0) == WAIT_OBJECT_0;
		if (bRet)
			SetEvent(m_cEvent);
		LeaveCriticalSection(&m_stMutex);
		return bRet;
	}

	void CondSignal::Set() const
	{
		EnterCriticalSection(&m_stMutex);
		SetEvent(m_cEvent);
		LeaveCriticalSection(&m_stMutex);
	}

	void CondSignal::Reset() const
	{
		EnterCriticalSection(&m_stMutex);
		ResetEvent(m_cEvent);
		LeaveCriticalSection(&m_stMutex);
	}

	bool CondSignal::Wait(unsigned int uTimeoutMS) const
	{
		return WaitForSingleObject(m_cEvent, uTimeoutMS) == WAIT_OBJECT_0;
	}

#endif


} // namespace cxxlib
