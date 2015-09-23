
#include "Thread.h"
#include "ThreadImpl.h"

using namespace cxxlib;

/// ThreadId
///

const ThreadId ThreadId::NUL = 0;

ThreadId::ThreadId() : IMPL_P_VAR(NULL)
{
	IMPL_P_VAR = new Impl();
}

ThreadId::ThreadId(const ThreadId &cObj) : IMPL_P_VAR(NULL)
{
	IMPL_P_VAR = new Impl();

	IMPL_P(ThreadId);
	IMPL_OBJ_P(const ThreadId, cObj);
	*p = *op;
}

ThreadId::ThreadId(unsigned int nId) : IMPL_P_VAR(NULL)
{
	IMPL_P_VAR = new Impl();

	IMPL_P(ThreadId);
	p->_nId = nId;
}

ThreadId::~ThreadId()
{
	delete IMPL_P_VAR;
}

ThreadId& ThreadId::operator = (const ThreadId &cObj)
{
	IMPL_P(ThreadId);
	IMPL_OBJ_P(const ThreadId, cObj);
	*p = *op;
	return *this;
}

ThreadId& ThreadId::operator = (unsigned int nId)
{
	IMPL_P(ThreadId);
	p->_nId = nId;
	return *this;
}

ThreadId::operator unsigned int () const
{
	IMPL_P(const ThreadId);
	return p->_nId;
}

ThreadId Thread::GetCurrentThreadId()
{
#ifdef _WIN
	return ::GetCurrentThreadId();
#else
	return pthread_self();
#endif
}

/// Thread Static
///

void Thread::YieldCurrentThread()
{
#ifdef _WIN
	::SwitchToThread();
#else
	sched_yield();
#endif
}

void Thread::Sleep(UINT32 nSeconds)
{
#ifdef _WIN
	::Sleep(nSeconds * 1000);
#else
	::sleep(nSeconds);
#endif
}

void Thread::MSleep(UINT32 nMiliSeconds)
{
#ifdef _WIN
	::Sleep(nMiliSeconds);
#else
	::usleep(nMiliSeconds * 1000);
#endif
}

/// Thread
///

Thread::Thread() : IMPL_P_VAR(NULL)
{
	IMPL_P_VAR = new Impl();
}

Thread::Thread(RunFunc fnRunFunc, void *pRunFuncArg) : IMPL_P_VAR(NULL)
{
	IMPL_P_VAR = new Impl(fnRunFunc, pRunFuncArg);
}

Thread::Thread(Runnable &cRunnable) : IMPL_P_VAR(NULL)
{
	IMPL_P_VAR = new Impl(cRunnable);
}

Thread::~Thread()
{
	delete IMPL_P_VAR;
}

Thread::Priority Thread::GetPriority() const
{
	IMPL_P(const Thread);
	return p->_ePriority;
}

Thread::Priority Thread::SetPriority(Priority ePrior)
{
	IMPL_P(Thread);
	Priority eOldPrior = p->_ePriority;
	p->_ePriority = ePrior;
	return eOldPrior;
}

unsigned int Thread::GetStackSize() const
{
	IMPL_P(const Thread);
	return p->_nStackSize;
}

unsigned int Thread::SetStackSize(unsigned int nSize)
{
	IMPL_P(Thread);
	unsigned int nOldSize = p->_nStackSize;
	p->_nStackSize = nSize;
	return nOldSize;
}

bool Thread::Joinable() const
{
	IMPL_P(const Thread);
	return p->Joinable();
}

ThreadId Thread::GetId() const
{
	IMPL_P(const Thread);
	return p->GetId();
}

bool Thread::Join(unsigned int nTimeoutMS)
{
	IMPL_P(Thread);
	return p->Join(nTimeoutMS);
}

bool Thread::Terminate()
{
	IMPL_P(Thread);
	p->Terminate();
	return true;
}

Thread & Thread::Swap(Thread &cThat)
{
	swap(IMPL_P_VAR, cThat.IMPL_P_VAR);
	return *this;
}
