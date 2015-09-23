
#include "ThreadImpl.h"

#ifdef _WIN
#	include <process.h>
#else
#	include <sys/time.h>
#endif

using namespace cxxlib;

Thread::Impl::Impl()
	: _ePriority(PR_NORMAL)
	, _nStackSize(0)
	, m_cHandle(NULL_HANDLE)
	, m_cId(ThreadId::NUL)
	, m_fnRunFunc(0)
	, m_pRunFuncArg(0)
	, m_pcRunnable(0)
	, m_bInitialized(false)
{
}

Thread::Impl::Impl(RunFunc fnRunFunc, void *pRunFuncArg)
	: _ePriority(PR_NORMAL)
	, _nStackSize(0)
	, m_cHandle(NULL_HANDLE)
	, m_cId(ThreadId::NUL)
	, m_fnRunFunc(fnRunFunc)
	, m_pRunFuncArg(pRunFuncArg)
	, m_pcRunnable(0)
	, m_bInitialized(false)
{
	Initialize();
	StartThread();
}

Thread::Impl::Impl(Runnable &cRunnable)
	: _ePriority(PR_NORMAL)
	, _nStackSize(0)
	, m_cHandle(NULL_HANDLE)
	, m_cId(ThreadId::NUL)
	, m_fnRunFunc(0)
	, m_pRunFuncArg(0)
	, m_pcRunnable(&cRunnable)
	, m_bInitialized(false)
{
	Initialize();
	StartThread();
}

void Thread::Impl::Initialize()
{
#ifdef _WIN
#else
	m_bRunning = false;

	pthread_mutexattr_t stMutexAttr;
	CXXLIB_CHECK_X(
		pthread_mutexattr_init(&stMutexAttr) == 0,
		"Thread::Impl::Initialize",
		"pthread_mutexattr_init");
	CXXLIB_CHECK_X(
		pthread_mutexattr_settype(&stMutexAttr, PTHREAD_MUTEX_NORMAL) == 0,
		"Thread::Impl::Initialize",
		"pthread_mutexattr_settype");
	CXXLIB_CHECK_X(
		pthread_mutex_init(&m_stMut, &stMutexAttr) == 0,
		"Thread::Impl::Initialize",
		"pthread_mutex_init");
	CXXLIB_CHECK_X(
		pthread_mutexattr_destroy(&stMutexAttr) == 0,
		"Thread::Impl::Initialize",
		"pthread_mutexattr_destroy");

	CXXLIB_CHECK_X(
		pthread_cond_init(&m_stCond, NULL) == 0,
		"Thread::Impl::Initialize",
		"pthread_cond_init");
#endif

	m_bInitialized = true;
}

Thread::Impl::~Impl()
{
	Terminate();

#ifdef _WIN
#else
	if (m_bInitialized)
	{
		CXXLIB_CHECK_X(
			pthread_cond_destroy(&m_stCond) == 0,
			"Thread::Impl::_destructor",
			"pthread_cond_destroy");
		CXXLIB_CHECK_X(
			pthread_mutex_destroy(&m_stMut) == 0,
			"Thread::Impl::_destructor",
			"pthread_mutex_destroy");
	}
#endif
}

void Thread::Impl::StartThread()
{
#ifdef _WIN

	unsigned int nThreadId;
	m_cHandle = reinterpret_cast<Impl::Handle>(::_beginthreadex(
		NULL,
		_nStackSize != 0 ? _nStackSize : 0,
		Impl::ThreadMain,
		this,
		CREATE_SUSPENDED,
		&nThreadId));

	if (m_cHandle == NULL_HANDLE)
		return;

	m_cId = nThreadId;

	if (_ePriority != PR_NORMAL)
	{
		CXXLIB_CHECK_X(
			::SetThreadPriority(m_cHandle, GetPriorityValue(_ePriority)) != 0,
			"Thread::Impl::StartThread",
			"SetThreadPriority");
	}

	::ResumeThread(m_cHandle);

#else

	pthread_attr_t stAttr;
	CXXLIB_CHECK_X(
		pthread_attr_init(&stAttr) == 0,
		"Thread::Impl::StartThread",
		"pthread_attr_init");
	CXXLIB_CHECK_X(
		pthread_attr_setdetachstate(&stAttr, PTHREAD_CREATE_JOINABLE) == 0,
		"Thread::Impl::StartThread",
		"pthread_attr_setdetachstate");

	if (_nStackSize != 0)
	{
		CXXLIB_CHECK_X(
			pthread_attr_setstacksize(&stAttr, _nStackSize) == 0,
			"Thread::Impl::StartThread",
			"pthread_attr_setstacksize");
	}

	if (_ePriority != PR_NORMAL)
	{
		sched_param stParam;

		CXXLIB_CHECK_X(
			pthread_attr_getschedparam(&stAttr, &stParam) == 0,
			"Thread::Impl::StartThread",
			"pthread_attr_getschedparam");

		stParam.sched_priority = GetPriorityValue(_ePriority);

		CXXLIB_CHECK_X(
			pthread_attr_setschedpolicy(&stAttr, SCHEDULE_POLICY_USE) == 0,
			"Thread::Impl::StartThread",
			"pthread_attr_setschedpolicy");
		CXXLIB_CHECK_X(
			pthread_attr_setschedparam(&stAttr, &stParam) == 0,
			"Thread::Impl::StartThread",
			"pthread_attr_setschedparam");

		/// NOTE:
		///		http://blog.xuite.net/jackie.xie/bluelove/9741811
		///		Note that it is particularly important that you remember to set the
		///		inheritsched attribute to PTHREAD_EXPLICIT_SCHED, or the the policy
		///		and priority that you've set will be ignored!
		///		
		///		but this will cause the system un-respond...
		///
		CXXLIB_CHECK_X(
			pthread_attr_setinheritsched(&stAttr, PTHREAD_EXPLICIT_SCHED) == 0,
			"Thread::Impl::StartThread",
			"pthread_attr_setinheritsched");
	}

	if (0 != pthread_create(&m_cHandle, &stAttr, Impl::ThreadMain, this))
	{
		CXXLIB_CHECK_X(
			pthread_attr_destroy(&stAttr) == 0,
			"Thread::Impl::StartThread",
			"pthread_attr_destroy");
		return;
	}
	CXXLIB_CHECK_X(
		pthread_attr_destroy(&stAttr) == 0,
		"Thread::Impl::StartThread",
		"pthread_attr_destroy");

	m_cId = m_cHandle;

	CXXLIB_CHECK_X(
		pthread_mutex_lock(&m_stMut) == 0,
		"Thread::Impl::StartThread",
		"pthread_mutex_lock");

	m_bRunning = true;

	CXXLIB_CHECK_X(
		pthread_cond_signal(&m_stCond) == 0,
		"Thread::Impl::StartThread",
		"pthread_cond_signal");
	CXXLIB_CHECK_X(
		pthread_mutex_unlock(&m_stMut) == 0,
		"Thread::Impl::StartThread",
		"pthread_mutex_unlock");
#endif
}

void Thread::Impl::ResetHandle()
{
	if (m_cHandle != NULL_HANDLE)
	{
#ifdef _WIN
		CXXLIB_CHECK_X(
			WAIT_OBJECT_0 == ::WaitForSingleObject(m_cHandle, INFINITE),
			"Thread::Impl::ResetHandle",
			"WaitForSingleObject");
		::CloseHandle(m_cHandle);
#else
		CXXLIB_CHECK_X(
			pthread_join(m_cHandle, NULL) == 0,
			"Thread::Impl::ResetHandle",
			"pthread_join");
#endif
		m_cHandle = NULL_HANDLE;
	}
}

bool Thread::Impl::Join(unsigned int nTimeoutMS)
{
	if (!Joinable())
		return false;

#	ifdef _WIN

	bool bJoined = (WAIT_OBJECT_0 == ::WaitForSingleObject(m_cHandle, nTimeoutMS));

	if (bJoined)
		ResetHandle();

	return bJoined;

#	else

	CXXLIB_CHECK_X(
		pthread_mutex_lock(&m_stMut) == 0,
		"Thread::Impl::Join",
		"pthread_mutex_lock");

	if (m_bRunning)
	{
		if (nTimeoutMS != WAIT_INFINITE)
		{
			struct timeval stTimeVal;
			CXXLIB_CHECK_X(
				gettimeofday(&stTimeVal, NULL) == 0,
				"Thread::Impl::Join",
				"gettimeofday");

			struct timespec stTimeSpec;
			stTimeSpec.tv_sec = stTimeVal.tv_sec;
			stTimeSpec.tv_nsec = (stTimeVal.tv_usec * 1000);
			if (nTimeoutMS > 0)
			{
				stTimeSpec.tv_sec += (nTimeoutMS / 1000);
				stTimeSpec.tv_nsec += ((nTimeoutMS % 1000)  * 1000000);
				stTimeSpec.tv_sec += (stTimeSpec.tv_nsec / 1000000000L);
				stTimeSpec.tv_nsec = (stTimeSpec.tv_nsec % 1000000000L);
			}
			pthread_cond_timedwait(&m_stCond, &m_stMut, &stTimeSpec);
		}
		else
		{
			CXXLIB_CHECK_X(
				pthread_cond_wait(&m_stCond, &m_stMut) == 0,
				"Thread::Impl::Join",
				"pthread_cond_wait");
		}
	}

	CXXLIB_CHECK_X(
		pthread_mutex_unlock(&m_stMut) == 0,
		"Thread::Impl::Join",
		"pthread_mutex_unlock");

	if (!m_bRunning)
		ResetHandle();

	return !m_bRunning;

#	endif
}

void Thread::Impl::Terminate()
{
	if (!Joinable())
		return;

#	ifdef _WIN
	CXXLIB_CHECK_X(
		TRUE == ::TerminateThread(m_cHandle, 0),
		"Thread::Impl::Terminate",
		"Terminate");
#	else
	CXXLIB_CHECK_X(
		pthread_cancel(m_cHandle) == 0,
		"Thread::Impl::Terminate",
		"pthread_cancel");
#	endif

	ResetHandle();
}

void Thread::Impl::Detach()
{
	if (!Joinable())
		return;

#	ifdef _WIN
	::CloseHandle(m_cHandle);
#	else
	CXXLIB_CHECK_X(
		pthread_detach(m_cHandle) == 0,
		"Thread::Impl::Detach",
		"pthread_detach");
#	endif

	m_cHandle = NULL_HANDLE;
}

int Thread::Impl::GetPriorityValue(Priority ePriority)
{
#ifdef _WIN
	int iActualPriority = THREAD_PRIORITY_NORMAL;
	switch (ePriority)
	{
	case PR_NORMAL:		iActualPriority = THREAD_PRIORITY_NORMAL;			break;

	case PR_CRITICAL:	iActualPriority = THREAD_PRIORITY_TIME_CRITICAL;	break;
	case PR_HIGH:		iActualPriority = THREAD_PRIORITY_HIGHEST;			break;
	case PR_ABOVE:		iActualPriority = THREAD_PRIORITY_ABOVE_NORMAL;		break;

	case PR_BELOW:		iActualPriority = THREAD_PRIORITY_BELOW_NORMAL;		break;
	case PR_LOW:		iActualPriority = THREAD_PRIORITY_LOWEST;			break;
	case PR_IDLE:		iActualPriority = THREAD_PRIORITY_IDLE;				break;
	}
	return iActualPriority;
#else
	int iMinPriority = sched_get_priority_min(SCHEDULE_POLICY_USE);
	int iMaxPriority = sched_get_priority_max(SCHEDULE_POLICY_USE);

	int iActualPriority = (iMaxPriority + iMinPriority)/2;
	switch (ePriority)
	{
	case PR_NORMAL:		iActualPriority = (iMaxPriority + iMinPriority)/2;		break;

	case PR_CRITICAL:	iActualPriority = iMaxPriority;							break;
	case PR_HIGH:		iActualPriority = (iMaxPriority + iActualPriority)/2;	break;
	case PR_ABOVE:		iActualPriority = iActualPriority + 1;					break;

	case PR_BELOW:		iActualPriority = iActualPriority - 1;					break;
	case PR_LOW:		iActualPriority = (iMinPriority + iActualPriority)/2;	break;
	case PR_IDLE:		iActualPriority = iMinPriority;							break;
	}
	return iActualPriority;
#endif
}

__THREADMAIN_RET_TYPE Thread::Impl::ThreadMain(__THREADMAIN_ARG_TYPE pArg)
{
	Impl *p = reinterpret_cast<Impl *>(pArg);

#ifdef _WIN
#else
	CXXLIB_CHECK_X(
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) == 0,
		"Thread::Impl::ThreadMain",
		"pthread_setcancelstate");
	CXXLIB_CHECK_X(
		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL) == 0,
		"Thread::Impl::ThreadMain",
		"pthread_setcanceltype");

	CXXLIB_CHECK_X(
		pthread_mutex_lock(&p->m_stMut) == 0,
		"Thread::Impl::ThreadMain",
		"pthread_mutex_lock");
	if (!p->m_bRunning)
	{
		CXXLIB_CHECK_X(
			pthread_cond_wait(&p->m_stCond, &p->m_stMut) == 0,
			"Thread::Impl::ThreadMain",
			"pthread_cond_wait");
	}
	CXXLIB_CHECK_X(
		pthread_mutex_unlock(&p->m_stMut) == 0,
		"Thread::Impl::ThreadMain",
		"pthread_mutex_unlock");
#endif

	if (p->m_pcRunnable)
		(*p->m_pcRunnable)();
	else if (p->m_fnRunFunc)
		p->m_fnRunFunc(p->m_pRunFuncArg);

#ifdef _WIN
#else
	CXXLIB_CHECK_X(
		pthread_mutex_lock(&p->m_stMut) == 0,
		"Thread::Impl::ThreadMain",
		"pthread_mutex_lock");

	p->m_bRunning = false;

	CXXLIB_CHECK_X(
		pthread_cond_signal(&p->m_stCond) == 0,
		"Thread::Impl::ThreadMain",
		"pthread_cond_signal");
	CXXLIB_CHECK_X(
		pthread_mutex_unlock(&p->m_stMut) == 0,
		"Thread::Impl::ThreadMain",
		"pthread_mutex_unlock");
#endif
	return NULL;
}
