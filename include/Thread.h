#pragma once

#include "cxxlib.h"

namespace cxxlib
{
	struct CXXLIB_EXPORT Runnable
	{
	public:
		Runnable() {}
		virtual ~Runnable() {}
		virtual void operator() () = 0;
	};

	class CXXLIB_EXPORT ThreadId : FINAL
	{
		DECLARE_IMPL_P
	public:
		static const ThreadId NUL;
		ThreadId();
		ThreadId(const ThreadId &cObj);
		ThreadId(unsigned int nId);
		~ThreadId();
		ThreadId & operator = (const ThreadId &cObj);
		ThreadId & operator = (unsigned int nId);
		operator unsigned int () const;
	};

	class CXXLIB_EXPORT Thread : NOCOPY, FINAL
	{
		DECLARE_IMPL_P
	public:
		typedef void (*RunFunc)(void *);

		enum Priority
		{
			PR_CRITICAL =	 3,
			PR_HIGH =		 2,
			PR_ABOVE =		 1,
			PR_NORMAL =		 0,
			PR_BELOW =		-1,
			PR_LOW =		-2,
			PR_IDLE =		-3
		};

		static ThreadId GetCurrentThreadId();
		static void YieldCurrentThread();
		static void Sleep(UINT32 nSeconds);
		static void MSleep(UINT32 nMiliSeconds);

		Thread();
		Thread(RunFunc fnRunFunc, void *pRunFuncArg = 0);
		Thread(Runnable &cRunnable);
		virtual ~Thread();

		Priority GetPriority() const;
		Priority SetPriority(Priority ePrior);
		unsigned int GetStackSize() const;
		unsigned int SetStackSize(unsigned int nSize);

		bool Joinable() const;
		ThreadId GetId() const;
		bool Join(unsigned int nTimeoutMS = WAIT_INFINITE);
		bool Terminate();

		Thread & Swap(Thread &cThat);
	};

	inline void swap(Thread &lhs, Thread &rhs) { lhs.Swap(rhs); }
}
