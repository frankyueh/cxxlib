
#include "Thread.h"
#include "EventSignal.h"
#include "Mutex.h"

#include "gtest/gtest.h"
#include "gtest_utilities.h"

using namespace cxxlib;

/*
// Macros for test
//-----------------------------------------------------------------------------

#define MUTEXT_TEST_WAIT_FOR_STAGE(_cThread_, _iStage_)		\
	GTM_WAIT_UNTIL(											\
		void(),												\
		(_iStage_) == (_cThread_).Stage(),					\
		GUTIL_MAX_WAITING_MS);


#define MUTEXT_TEST_STILL_IN_STAGE(_cThread_, _iStage_)		\
	gtest_utility::thread_msleep(GUTIL_AVG_MEASURE_MS);		\
	EXPECT_EQ((_iStage_), (_cThread_).Stage());


#define MUTEXT_TEST_PULSE(_cThread_)						\
	GTM_WAIT_UNTIL(											\
		void(),												\
		!(_cThread_).IsPulsed(),							\
		GUTIL_MAX_WAITING_MS);							\
	(_cThread_).Pulse();


// Thread classes for test
//-----------------------------------------------------------------------------

class MutexThreadBase : public Thread
{
public:
	MutexThreadBase() : m_iStage(0) {}
	virtual ~MutexThreadBase()
	{
		GTM_WAIT_UNTIL(
			void(),
			!Joinable(),
			GUTIL_MAX_WAITING_MS);
	}

	void Pulse()				{ m_cSignal.Set(); }
	bool IsPulsed()				{ return m_cSignal.IsSet(); }
	void SetStage(int iStage)	{ m_iStage = iStage; }
	int	Stage()					{ return m_iStage; }

protected:
	void Wait()					{ m_cSignal.Wait(); }

private:
	CondSignal		m_cSignal;
	volatile int	m_iStage;
	volatile UINT32	m_nChecker;
};

class Mutex__NonRecursive_Thread : public MutexThreadBase
{
public:
	Mutex__NonRecursive_Thread() { EXPECT_TRUE(Thread::Start()); }
	const Mutex& GetMutex() const { return s_cMutex; }

protected:
	void Run()
	{
		Wait();
		bool bLocked = s_cMutex.TryLock();
		SetStage(1);
		if (!bLocked)
		{
			Wait();
			s_cMutex.Lock();
			SetStage(2);
		}
		Wait();
		s_cMutex.Unlock();
		SetStage(3);
	}
	static Mutex s_cMutex;
};
Mutex Mutex__NonRecursive_Thread::s_cMutex(Mutex::NONRECURSIVE);

class Mutex__Recursive_Thread : public MutexThreadBase
{
public:
	Mutex__Recursive_Thread() { EXPECT_TRUE(Thread::Start()); }
	const Mutex& GetMutex() const { return s_cMutex; }

protected:
	void Run()
	{
		Wait();
		bool bLocked = s_cMutex.TryLock();
		SetStage(1);
		Wait();
		s_cMutex.Lock();
		SetStage(2);
		Wait();
		s_cMutex.Unlock();
		SetStage(3);

		if (bLocked)
		{
			Wait();
			s_cMutex.Unlock();
			SetStage(4);
		}
	}
	static Mutex s_cMutex;
};
Mutex Mutex__Recursive_Thread::s_cMutex(Mutex::RECURSIVE);

// Mutex_Test
//-----------------------------------------------------------------------------

TEST(Mutex_Test, NonRecursive_GeneralUsage)
{
	Mutex__NonRecursive_Thread cThreadA, cThreadB;

	MUTEXT_TEST_PULSE(cThreadA);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadA, 1);

	MUTEXT_TEST_PULSE(cThreadB);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadB, 1);
	MUTEXT_TEST_PULSE(cThreadB);
	MUTEXT_TEST_STILL_IN_STAGE(cThreadB, 1);

	MUTEXT_TEST_PULSE(cThreadA);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadA, 3);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadB, 2);

	MUTEXT_TEST_PULSE(cThreadB);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadB, 3);
}

TEST(Mutex_Test, Recursive_GeneralUsage)
{
	Mutex__Recursive_Thread cThreadA, cThreadB;

	MUTEXT_TEST_PULSE(cThreadA);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadA, 1);
	MUTEXT_TEST_PULSE(cThreadA);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadA, 2);

	MUTEXT_TEST_PULSE(cThreadB);
	MUTEXT_TEST_PULSE(cThreadB);
	MUTEXT_TEST_STILL_IN_STAGE(cThreadB, 1);

	MUTEXT_TEST_PULSE(cThreadA);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadA, 3);
	MUTEXT_TEST_STILL_IN_STAGE(cThreadB, 1);

	MUTEXT_TEST_PULSE(cThreadA);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadA, 4);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadB, 2);

	MUTEXT_TEST_PULSE(cThreadB);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadB, 3);
	MUTEXT_TEST_PULSE(cThreadB);
	MUTEXT_TEST_STILL_IN_STAGE(cThreadB, 3);
}

// Mutex_DEATHTest
//-----------------------------------------------------------------------------

TEST(Mutex_DEATHTest, NonRecursive_UnlockWithoutLock)
{
#ifdef _DEBUG
	EXPECT_DEATH(
		{
			Mutex cMutex(Mutex::NONRECURSIVE);
			cMutex.Unlock();
		},
		"");
#endif
}

TEST(Mutex_DEATHTest, NonRecursive_UnlockWhichOwnedByOther)
{
#ifdef _DEBUG
	EXPECT_DEATH(
		{
			Mutex__NonRecursive_Thread cThread;

			MUTEXT_TEST_PULSE(cThread);
			MUTEXT_TEST_WAIT_FOR_STAGE(cThread, 1);

			cThread.GetMutex().Unlock();

			MUTEXT_TEST_PULSE(cThread);
			MUTEXT_TEST_WAIT_FOR_STAGE(cThread, 3);
		},
		"");
#endif
}

TEST(Mutex_DEATHTest, NonRecursive_LockWithoutUnlock)
{
#ifdef _DEBUG
	EXPECT_DEATH(
		{
			Mutex cMutex(Mutex::NONRECURSIVE);
			cMutex.Lock();
		},
		"");
#endif
}

TEST(Mutex_DEATHTest, NonRecursive_LockRecursively)
{
#ifdef _DEBUG
	EXPECT_DEATH(
		{
			Mutex cMutex(Mutex::NONRECURSIVE);
			cMutex.Lock();
			cMutex.Lock();
		},
		"");
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Mutex_DEATHTest.Recursive

TEST(Mutex_DEATHTest, Recursive_UnlockWithoutLock)
{
#ifdef _DEBUG
	EXPECT_DEATH(
		{
			Mutex cMutex(Mutex::RECURSIVE);
			cMutex.Unlock();
		},
		"");
#endif
}

TEST(Mutex_DEATHTest, Recursive_UnlockWhichOwnedByOther)
{
#ifdef _DEBUG
	EXPECT_DEATH(
		{
			Mutex__Recursive_Thread cThread;

			MUTEXT_TEST_PULSE(cThread);
			MUTEXT_TEST_WAIT_FOR_STAGE(cThread, 1);
			MUTEXT_TEST_PULSE(cThread);
			MUTEXT_TEST_WAIT_FOR_STAGE(cThread, 2);

			cThread.GetMutex().Unlock();

			MUTEXT_TEST_PULSE(cThread);
			MUTEXT_TEST_WAIT_FOR_STAGE(cThread, 3);
			MUTEXT_TEST_PULSE(cThread);
			MUTEXT_TEST_WAIT_FOR_STAGE(cThread, 4);
		},
		"");
#endif
}

TEST(Mutex_DEATHTest, Recursive_LockWithoutUnlock)
{
#ifdef _DEBUG
	EXPECT_DEATH(
		{
			Mutex cMutex(Mutex::RECURSIVE);
			cMutex.Lock();
		},
		"");
#endif
}

TEST(Mutex_DEATHTest, Recursive_RecursiveLockWithoutUnlockEnough)
{
#ifdef _DEBUG
	EXPECT_DEATH(
		{
			Mutex cMutex(Mutex::RECURSIVE);
			cMutex.Lock();
			cMutex.Lock();
			cMutex.Unlock();
		},
		"");
#endif
}

// Guard_Test
//-----------------------------------------------------------------------------

class GuardObject : GUARD
{
public:
	GuardObject(Mutex::RecursionMode eMode) : m_cMutex(eMode) {}
	void Lock() const;
	bool TryLock() const;
	void Unlock() const;
private:
	const Mutex m_cMutex;
};

void GuardObject::Lock() const { m_cMutex.Lock(); }
bool GuardObject::TryLock() const { return m_cMutex.TryLock(); }
void GuardObject::Unlock() const { m_cMutex.Unlock(); }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Guard_Test.RAII

class Guard__RAII_Thread : public MutexThreadBase
{
public:
	Guard__RAII_Thread() { EXPECT_TRUE(Thread::Start()); }

protected:
	void Run()
	{
		Wait();
		{
			Guarded cGuardedNonRecur(s_cNonRecursiveGuardObject);
			Guarded cGuardedRecur(s_cRecursiveGuardObject);
			SetStage(1);
			Wait();
		}
		Wait();
		{
			Guarded cGuarded(s_cRecursiveGuardObject);
			{
				Guarded cGuarded(s_cRecursiveGuardObject);
				SetStage(2);
				Wait();
			}
		}
		SetStage(3);
	}
	static const GuardObject s_cNonRecursiveGuardObject;
	static const GuardObject s_cRecursiveGuardObject;
};
const GuardObject Guard__RAII_Thread::s_cNonRecursiveGuardObject(Mutex::NONRECURSIVE);
const GuardObject Guard__RAII_Thread::s_cRecursiveGuardObject(Mutex::RECURSIVE);

TEST(Guard_Test, RAII_GeneralUsage)
{
	Guard__RAII_Thread cThreadA, cThreadB;

	MUTEXT_TEST_PULSE(cThreadA);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadA, 1);
	MUTEXT_TEST_PULSE(cThreadB);
	MUTEXT_TEST_STILL_IN_STAGE(cThreadB, 0);

	MUTEXT_TEST_PULSE(cThreadA);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadB, 1);
	MUTEXT_TEST_PULSE(cThreadB);
	MUTEXT_TEST_STILL_IN_STAGE(cThreadA, 1);

	MUTEXT_TEST_PULSE(cThreadB);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadB, 2);
	MUTEXT_TEST_PULSE(cThreadA);
	MUTEXT_TEST_STILL_IN_STAGE(cThreadA, 1);

	MUTEXT_TEST_PULSE(cThreadB);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadB, 3);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadA, 2);
	MUTEXT_TEST_PULSE(cThreadA);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadA, 3);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Guard_Test.Method

class Guard__Method_Thread : public MutexThreadBase
{
public:
	Guard__Method_Thread() { EXPECT_TRUE(Thread::Start()); }

protected:
	void Run()
	{
		Wait();
		s_cNonRecursiveGuardObject.Lock();
		SetStage(1);
		Wait();
		s_cNonRecursiveGuardObject.Unlock();
		SetStage(2);
		Wait();
		s_cRecursiveGuardObject.Lock();
		s_cRecursiveGuardObject.Lock();
		SetStage(3);
		Wait();
		s_cRecursiveGuardObject.Unlock();
		s_cRecursiveGuardObject.Unlock();
		SetStage(4);
	}
	static const GuardObject s_cNonRecursiveGuardObject;
	static const GuardObject s_cRecursiveGuardObject;
};
const GuardObject Guard__Method_Thread::s_cNonRecursiveGuardObject(Mutex::NONRECURSIVE);
const GuardObject Guard__Method_Thread::s_cRecursiveGuardObject(Mutex::RECURSIVE);

TEST(Guard_Test, Method_GeneralUsage)
{
	Guard__Method_Thread cThreadA, cThreadB;

	MUTEXT_TEST_PULSE(cThreadA);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadA, 1);
	MUTEXT_TEST_PULSE(cThreadB);
	MUTEXT_TEST_STILL_IN_STAGE(cThreadB, 0);

	MUTEXT_TEST_PULSE(cThreadA);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadA, 2);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadB, 1);

	MUTEXT_TEST_PULSE(cThreadB);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadB, 2);
	MUTEXT_TEST_PULSE(cThreadB);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadB, 3);
	MUTEXT_TEST_PULSE(cThreadA);
	MUTEXT_TEST_STILL_IN_STAGE(cThreadA, 2);

	MUTEXT_TEST_PULSE(cThreadB);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadB, 4);
	MUTEXT_TEST_PULSE(cThreadA);
	MUTEXT_TEST_WAIT_FOR_STAGE(cThreadB, 4);
}

// Guard_DEATHTest
//-----------------------------------------------------------------------------

TEST(Guard_DEATHTest, NonRecursiveAndRecursive)
{
#ifdef _DEBUG
	EXPECT_DEATH(
		{
			GuardObject cNonRecusiveLockObj(Mutex::NONRECURSIVE);
			Guarded cGuardedFirst(cNonRecusiveLockObj);
			Guarded cGuardedSecond(cNonRecusiveLockObj);
		},
		"");

	EXPECT_DEATH(
		{
			GuardObject cRecusiveLockObj(Mutex::RECURSIVE);
			Guarded cGuarded(cRecusiveLockObj);
			{
				Guarded cGuarded(cRecusiveLockObj);
				cRecusiveLockObj.Unlock();
			}
		},
		"");
#endif
}
*/