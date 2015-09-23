#pragma once

#include "Thread.h"

#ifdef _WIN
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	include <process.h>
#	undef WIN32_LEAN_AND_MEAN
#	define __THREADMAIN_ARG_TYPE	void *
#	define __THREADMAIN_RET_TYPE	unsigned int WINAPI
#else
#	include <pthread.h>
#	include <errno.h>
#	define __THREADMAIN_RET_TYPE	void *
#	define __THREADMAIN_ARG_TYPE	void *
#endif

// NOTE: Schedule priority and policy setting in Linux
//		 http://linux.about.com/library/cmd/blcmdl2_sched_setscheduler.htm
#ifdef _WIN
#else
#	define SCHEDULE_POLICY_USE SCHED_RR
#endif

namespace cxxlib
{
	class ThreadId::Impl : FINAL
	{
#	ifdef _WIN
		typedef DWORD		Id;
#	else
		typedef pthread_t	Id;
#	endif
	public:
		Impl() : _nId(0) {}
		Id _nId;
	};

	class Thread::Impl : FINAL, NOCOPY
	{
#	ifdef _WIN
		typedef HANDLE Handle;
#	else
		typedef pthread_t Handle;
#	endif
		static const unsigned int NULL_HANDLE = 0;

	public:

		Impl();
		Impl(RunFunc fnRunFunc, void *pRunFuncArg);
		Impl(Runnable &cRunnable);
		~Impl();

		Priority				_ePriority;
		unsigned int			_nStackSize;

		bool Joinable() const	{ return m_cHandle != NULL_HANDLE; }
		ThreadId GetId() const	{ return m_cId; }
		bool Join(unsigned int nTimeoutMS = WAIT_INFINITE);
		void Detach();
		void Terminate();

	private:

		static int GetPriorityValue(Priority ePriority);
		static __THREADMAIN_RET_TYPE ThreadMain(__THREADMAIN_ARG_TYPE pArgument);

		void Initialize();
		void StartThread();
		void ResetHandle();

		Handle			 m_cHandle;
		ThreadId		 m_cId;
		RunFunc			 m_fnRunFunc;
		void			*m_pRunFuncArg;
		Runnable		*m_pcRunnable;

		bool			m_bInitialized;
#	ifdef _WIN
#	else
		bool			m_bRunning;
		pthread_cond_t	m_stCond;
		pthread_mutex_t m_stMut;
#	endif
	};
}
