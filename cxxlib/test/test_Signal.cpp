
#include "Thread.h"
#include "EventSignal.h"
#include "Mutex.h"

#include "gtest/gtest.h"
#include "gtest_utilities.h"

using namespace cxxlib;
/*

// Signal_Test
//-----------------------------------------------------------------------------

class Signal_Test : public ::testing::Test
{
public:
	static const unsigned int THREAD_NUMBER =	7;		// at least 2
	static const unsigned int REPEAT_TIMES =	3;		// at least 1

protected:

	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}

private:
};


#define SIGNAL_TEST_ROUTINE(SignalType, ThreadCase, _stShared_)					\
	SignalTestRoutine<SignalType, ThreadCase> _cSignalTestRoutine_;				\
	ThreadCase *_acThread_ = _cSignalTestRoutine_.m_acThread;					\
	SignalSharedRes<SignalType>& _stShared_ = _cSignalTestRoutine_.m_stShared;


#define SIGNAL_THREAD_ROUTINE(SignalType, _cMutex_, _cSignal_, _nSharedCount_, _nCounter_)	\
																				\
	Mutex& _cMutex_ = pstShared->cMutex;										\
	SignalType& _cSignal_ = pstShared->cSignal;									\
																				\
	volatile unsigned int& _nSharedCount_ = pstShared->nSharedCount;			\
	volatile unsigned int& _nCounter_ = pstShared->anCounter[_nSharedCount_];	\
																				\
	_cMutex_.Lock();															\
	const unsigned int _nThreadNum_ = _nSharedCount_++;							\
	_cMutex_.Unlock();


template<class SignalType>
struct SignalSharedRes
{
	SignalSharedRes() : cMutex(Mutex::NONRECURSIVE) {}
	Mutex cMutex;
	SignalType cSignal;
	volatile unsigned int nSharedCount;
	volatile unsigned int anCounter[Signal_Test::THREAD_NUMBER];
	EventSignal cHold;
};


template<class SignalType, class ThreadCase>
class SignalTestRoutine
{
public:
	ThreadCase m_acThread[Signal_Test::THREAD_NUMBER];
	SignalSharedRes<SignalType> m_stShared;

	SignalTestRoutine()
	{
		m_stShared.nSharedCount = 0;
		GUTIL_FOREACH_ASSERT_LIMIT(
			u,
			Signal_Test::THREAD_NUMBER,
			{
				m_stShared.anCounter[u] = 0;
				m_acThread[u].pstShared = &m_stShared;
				EXPECT_TRUE(m_acThread[u].Start());
			},
			Guarded cGuarded(m_stShared.cMutex),
			u + 1 == m_stShared.nSharedCount,
			GUTIL_MAX_WAITING_MS);

		m_stShared.cMutex.Lock();
		m_stShared.nSharedCount = 0;
		m_stShared.cMutex.Unlock();
	}

	~SignalTestRoutine()
	{
		GUTIL_FOREACH_ASSERT_LIMIT(
			u,
			Signal_Test::THREAD_NUMBER,
			NULL,
			Guarded cGuarded(m_stShared.cMutex),
			!m_acThread[u].IsRunning(),
			GUTIL_MAX_WAITING_MS);
	}
};


template<class SignalType>
class ThreadCaseBase : public Thread
{
public:
	SignalSharedRes<SignalType> *pstShared;
};


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CondSetResetTest

class CondSetResetTest_Thread : public ThreadCaseBase<CondSignal>
{
	void Run()
	{
		SIGNAL_THREAD_ROUTINE(CondSignal, cMutex, cSignal, nSharedCount, nCounter);

		EXPECT_FALSE(cSignal.IsSet());
		EXPECT_FALSE(cSignal.Wait(0));
		cMutex.Lock();
		nCounter = 1;
		cMutex.Unlock();

		do
		{
			EXPECT_TRUE(cSignal.Wait());
			cMutex.Lock();
			++nSharedCount;
			++nCounter;
			cMutex.Unlock();

			pstShared->cHold.Wait();
		}
		while (nCounter <= Signal_Test::REPEAT_TIMES);
	}
};

TEST_F(Signal_Test, CondSetResetTest)
{
	SIGNAL_TEST_ROUTINE(CondSignal, CondSetResetTest_Thread, stShared);

	GUTIL_FOREACH_ASSERT_LIMIT(
		u,
		THREAD_NUMBER,
		NULL,
		Guarded cGuarded(stShared.cMutex),
		1 == stShared.anCounter[u],
		GUTIL_MAX_WAITING_MS);

	for (unsigned int n = 0 ; n < REPEAT_TIMES ; n++)
	{
		GTM_THREAD_SLEEPWAIT();

		stShared.cHold.Reset();
		GUTIL_FOREACH_ASSERT_LIMIT(
			u,
			THREAD_NUMBER,
			stShared.cSignal.Set(),
			Guarded cGuarded(stShared.cMutex),
			THREAD_NUMBER * n + u + 1 == stShared.nSharedCount,
			GUTIL_MAX_WAITING_MS);
		stShared.cHold.Set(false);

		GUTIL_FOREACH_ASSERT_LIMIT(
			u,
			THREAD_NUMBER,
			NULL,
			Guarded cGuarded(stShared.cMutex),
			n + 2 == stShared.anCounter[u],
			GUTIL_MAX_WAITING_MS);
	}
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CondIsSetAndWaitTest

class CondIsSetAndWaitTest_Thread : public ThreadCaseBase<CondSignal>
{
	void Run()
	{
		SIGNAL_THREAD_ROUTINE(CondSignal, cMutex, cSignal, nSharedCount, nCounter);

		do
		{
			if (cSignal.IsSet())
			{
				if (cSignal.Wait(0))
				{
					cMutex.Lock();
					++nSharedCount;
					++nCounter;
					cMutex.Unlock();
				}
			}
			else
			{
				GTM_THREAD_YIELD();
			}
		} while (nCounter < Signal_Test::REPEAT_TIMES);
	}
};

TEST_F(Signal_Test, CondIsSetAndWaitTest)
{
	SIGNAL_TEST_ROUTINE(CondSignal, CondIsSetAndWaitTest_Thread, stShared);

	unsigned int nSetCount = 0;
	GTM_WAIT_UNTIL(
		{
			if (nSetCount < THREAD_NUMBER * REPEAT_TIMES && !stShared.cSignal.IsSet())
			{
				++nSetCount;
				stShared.cSignal.Set();
			}
			else
			{
				GTM_THREAD_YIELD();
			}
		}
		Guarded cGuarded(stShared.cMutex),
		THREAD_NUMBER * REPEAT_TIMES == stShared.nSharedCount,
		GUTIL_MAX_WAITING_MS);

	for (unsigned int n = 0 ; n < THREAD_NUMBER ; ++n)
		EXPECT_EQ(+REPEAT_TIMES, stShared.anCounter[n]);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// EventManualReset

class EventManualReset_Thread : public ThreadCaseBase<EventSignal>
{
	void Run()
	{
		SIGNAL_THREAD_ROUTINE(EventSignal, cMutex, cSignal, nSharedCount, nCounter);

		EXPECT_FALSE(cSignal.IsSet());
		EXPECT_FALSE(cSignal.Wait(0));
		cMutex.Lock();
		nCounter = 1;
		cMutex.Unlock();

		do
		{
			EXPECT_TRUE(cSignal.Wait());
			EXPECT_TRUE(cSignal.IsSet());
			EXPECT_TRUE(cSignal.Wait());
			cMutex.Lock();
			++nSharedCount;
			++nCounter;
			cMutex.Unlock();

			pstShared->cHold.Wait();
		}
		while (nCounter <= Signal_Test::REPEAT_TIMES);
	}
};

TEST_F(Signal_Test, EventManualReset)
{
	SIGNAL_TEST_ROUTINE(EventSignal, EventManualReset_Thread, stShared);

	GUTIL_FOREACH_ASSERT_LIMIT(
		u,
		THREAD_NUMBER,
		NULL,
		Guarded cGuarded(stShared.cMutex),
		1 == stShared.anCounter[u],
		GUTIL_MAX_WAITING_MS);

	for (unsigned int n = 0 ; n < REPEAT_TIMES ; n++)
	{
		GTM_THREAD_SLEEPWAIT();

		stShared.cHold.Reset();
		stShared.cSignal.Set(false);
		GTM_WAIT_UNTIL(
			Guarded cGuarded(stShared.cMutex),
			THREAD_NUMBER * (n + 1) == stShared.nSharedCount,
			GUTIL_MAX_WAITING_MS);
		stShared.cSignal.Reset();
		stShared.cHold.Set(false);

		GTM_THREAD_SLEEPWAIT(); // prevent thread starvation from the code below...

		GUTIL_FOREACH_ASSERT_LIMIT(
			u,
			THREAD_NUMBER,
			NULL,
			Guarded cGuarded(stShared.cMutex),
			n + 2 == stShared.anCounter[u],
			GUTIL_MAX_WAITING_MS);
	}
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// EventAutoResetTest

class EventAutoResetTest_Thread : public ThreadCaseBase<EventSignal>
{
	void Run()
	{
		SIGNAL_THREAD_ROUTINE(EventSignal, cMutex, cSignal, nSharedCount, nCounter);

		EXPECT_FALSE(cSignal.IsSet());
		EXPECT_FALSE(cSignal.Wait(0));
		cMutex.Lock();
		nCounter = 1;
		cMutex.Unlock();

		do
		{
			EXPECT_FALSE(cSignal.IsSet());
			EXPECT_TRUE(cSignal.Wait());
			cMutex.Lock();
			++nCounter;
			cMutex.Unlock();
		}
		while (nCounter <= Signal_Test::REPEAT_TIMES);
	}
};

TEST_F(Signal_Test, EventAutoResetTest)
{
	SIGNAL_TEST_ROUTINE(EventSignal, EventAutoResetTest_Thread, stShared);

	GUTIL_FOREACH_ASSERT_LIMIT(
		u,
		THREAD_NUMBER,
		NULL,
		Guarded cGuarded(stShared.cMutex),
		1 == stShared.anCounter[u],
		GUTIL_MAX_WAITING_MS);

	GTM_THREAD_SLEEPWAIT();

	for (unsigned int n = 0 ; n < REPEAT_TIMES ; n++)
	{
		GTM_THREAD_SLEEPWAIT();

		stShared.cSignal.Set(true);

		GTM_THREAD_SLEEPWAIT(); // prevent thread starvation from the code below...

		GUTIL_FOREACH_ASSERT_LIMIT(
			u,
			THREAD_NUMBER,
			NULL,
			Guarded cGuarded(stShared.cMutex),
			n + 2 == stShared.anCounter[u],
			GUTIL_MAX_WAITING_MS);
	}
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// WaitTimeTest

TEST_F(Signal_Test, WaitTimeTest)
{
	CondSignal cCondObject;
	EventSignal cEventObject;

	GUTIL_EXPECT_EXECTIME(
		EXPECT_FALSE(cCondObject.Wait(GUTIL_AVG_MEASURE_MS)),
		GUTIL_AVG_MEASURE_MS,
		GUTIL_MEASURE_ERR_RANGE
		);

	GUTIL_EXPECT_EXECTIME(
		{
			EXPECT_FALSE(cEventObject.IsSet());
			EXPECT_FALSE(cEventObject.Wait(GUTIL_AVG_MEASURE_MS));
		},
		GUTIL_AVG_MEASURE_MS,
		GUTIL_MEASURE_ERR_RANGE
		);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// StateControlTest

TEST_F(Signal_Test, StateControlTest)
{
	CondSignal cCondSignal;
	EXPECT_FALSE(cCondSignal.IsSet());
	EXPECT_FALSE(cCondSignal.IsSet());
	EXPECT_FALSE(cCondSignal.IsSet());
	cCondSignal.Set();
	EXPECT_TRUE(cCondSignal.IsSet());
	EXPECT_TRUE(cCondSignal.IsSet());
	EXPECT_TRUE(cCondSignal.IsSet());
	EXPECT_TRUE(cCondSignal.Wait(0));
	EXPECT_FALSE(cCondSignal.Wait(0));
	EXPECT_FALSE(cCondSignal.IsSet());
	EXPECT_FALSE(cCondSignal.IsSet());
	EXPECT_FALSE(cCondSignal.IsSet());

	EventSignal cEventSignal;
	EXPECT_FALSE(cEventSignal.IsSet());
	EXPECT_FALSE(cEventSignal.IsSet());
	EXPECT_FALSE(cEventSignal.IsSet());
	cEventSignal.Set(false);
	EXPECT_TRUE(cEventSignal.IsSet());
	EXPECT_TRUE(cEventSignal.IsSet());
	EXPECT_TRUE(cEventSignal.IsSet());
	EXPECT_TRUE(cEventSignal.Wait(1));
	EXPECT_TRUE(cEventSignal.Wait(1));
	cEventSignal.Reset();
	EXPECT_FALSE(cEventSignal.IsSet());
	EXPECT_FALSE(cEventSignal.IsSet());
	EXPECT_FALSE(cEventSignal.IsSet());
	cEventSignal.Set(true);
	EXPECT_FALSE(cEventSignal.Wait(0));
	EXPECT_FALSE(cEventSignal.IsSet());
	EXPECT_FALSE(cEventSignal.IsSet());
	EXPECT_FALSE(cEventSignal.IsSet());
	cEventSignal.Reset();
	EXPECT_FALSE(cEventSignal.IsSet());
	EXPECT_FALSE(cEventSignal.IsSet());
	EXPECT_FALSE(cEventSignal.IsSet());
}
*/