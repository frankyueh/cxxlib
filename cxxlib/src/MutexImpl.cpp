
#include "MutexImpl.h"

using namespace cxxlib;


Mutex::Impl::Impl(RecursionMode eMode)
	: _eMode(eMode)
#ifdef _DEBUG
	, _nRecursiveCount(0)
#endif
{
#ifdef _WIN
	InitializeCriticalSection(&m_stMutex);
#else
	pthread_mutexattr_t stAttr;
	CXXLIB_CHECK_X(
		pthread_mutexattr_init(&stAttr) == 0,
		"Mutex::Impl::_constructor",
		"pthread_mutexattr_init");

	// NOTE: need always support recursive lock in debug mode
	CXXLIB_CHECK_X(
		pthread_mutexattr_settype(
			&stAttr,
#		ifdef _DEBUG
			PTHREAD_MUTEX_RECURSIVE
#		else
			eMode == RECURSIVE ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_NORMAL
#		endif
			) == 0,
		"Mutex::Impl::_constructor",
		"pthread_mutexattr_settype");

	CXXLIB_CHECK_X(
		pthread_mutex_init(&m_stMutex, &stAttr) == 0,
		"Mutex::Impl::_constructor",
		"pthread_mutex_init");
	CXXLIB_CHECK_X(
		pthread_mutexattr_destroy(&stAttr) == 0,
		"Mutex::Impl::_constructor",
		"pthread_mutexattr_destroy");
#endif
}

Mutex::Impl::~Impl()
{
#ifdef _WIN
	DeleteCriticalSection(&m_stMutex);
#else
	CXXLIB_CHECK_X(
		pthread_mutex_destroy(&m_stMutex) == 0,
		"Mutex::Impl::_destructor",
		"pthread_mutex_destroy");
#endif
}

void Mutex::Impl::Lock() const
{
#ifdef _WIN
	EnterCriticalSection(&m_stMutex);
#else
	// NOTE: do not handle any error, because we needs handle it from outside
	pthread_mutex_lock(&m_stMutex);
#endif
}

bool Mutex::Impl::TryLock() const
{
#ifdef _WIN
	return TryEnterCriticalSection(&m_stMutex) == TRUE;
#else
	return pthread_mutex_trylock(&m_stMutex) == 0;
#endif
}

void Mutex::Impl::Unlock() const
{
#ifdef _WIN
	LeaveCriticalSection(&m_stMutex);
#else
	// NOTE: do not handle any error, because we needs handle it from outside
	pthread_mutex_unlock(&m_stMutex);
#endif
}