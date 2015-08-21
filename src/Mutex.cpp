
#include "Mutex.h"
#include "MutexImpl.h"

using namespace cxxlib;


Mutex::Mutex(RecursionMode eMode)
	: IMPL_P_VAR(NULL)
{
	IMPL_P_VAR = new Impl(eMode);
}

Mutex::~Mutex()
{
	IMPL_P(const Mutex);
#ifdef _DEBUG
	CXXLIB_ASSERT_X(
		p->_nRecursiveCount == 0,
		"Mutex without unlock before destruction");
#endif
	delete IMPL_P_VAR;
}

void Mutex::Lock() const
{
	IMPL_P(const Mutex);

	p->Lock();

#ifdef _DEBUG
	++(p->_nRecursiveCount);

	if (p->_eMode == Mutex::NONRECURSIVE)
	{
		CXXLIB_ASSERT_X(
			p->_nRecursiveCount == 1,
			"Mutex type does not support recursive lock.");
	}
#endif
}

bool Mutex::TryLock() const
{
	IMPL_P(const Mutex);

#ifdef _DEBUG
	if (p->TryLock())
	{
		++(p->_nRecursiveCount);

		if (p->_eMode == Mutex::NONRECURSIVE)
		{
			CXXLIB_ASSERT_X(
				p->_nRecursiveCount == 1,
				"Mutex type does not support recursive lock.");
		}
		return true;
	}
	return false;
#else
	return p->TryLock();
#endif
}

void Mutex::Unlock() const
{
	IMPL_P(const Mutex);

#ifdef _DEBUG
	if (p->TryLock())
	{
		CXXLIB_ASSERT_X(
			p->_nRecursiveCount > 0,
			"Current Mutex does not locked");

		--(p->_nRecursiveCount);

		p->Unlock();
		p->Unlock();
	}
	else
	{
		CXXLIB_ASSERT_X(
			0,
			"Current thread does not owned this Mutex");
	}
#else
	p->Unlock();
#endif
}
