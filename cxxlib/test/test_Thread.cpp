
#include "Thread.h"

#include "gtest/gtest.h"
#include "gtest_utilities.h"

using namespace cxxlib;

/// Thread_Test
///

class RunCounter : public Runnable
{
public:
	RunCounter(int iCountTo = 100) : m_iCount(0), m_iCountTo(iCountTo) {}
	void operator() ()
	{
		EXPECT_TRUE(gtest_utility::thread_get_threadid() == Thread::GetCurrentThreadId());
		m_cThreadId = gtest_utility::thread_get_threadid();
		while (++m_iCount != m_iCountTo) gtest_utility::thread_yield();
	}
	int GetCount() const { return m_iCount; }
	int GetCountTo() const { return m_iCountTo; }
	ThreadId GetThreadId() const { return m_cThreadId; }
private:
	int m_iCount;
	int m_iCountTo;
	ThreadId m_cThreadId;
};

TEST(Thread_Test, GerneralTest)
{
	RunCounter cRunCounter;
	ASSERT_EQ(0, cRunCounter.GetCount());
	{
		Thread cThread(cRunCounter);

		ASSERT_TRUE(cThread.Joinable());

		ThreadId cThreadId = cThread.GetId();
		ASSERT_TRUE(ThreadId::NUL != cThreadId);

		cThread.Join();

		ASSERT_TRUE(cThreadId == cRunCounter.GetThreadId());
		ASSERT_FALSE(cThread.Joinable());
	}
	ASSERT_EQ(cRunCounter.GetCountTo(), cRunCounter.GetCount());
}

class RunFunc
{
public:
	static void Run(void *pSelf)
	{
		RunFunc *p = reinterpret_cast<RunFunc *>(pSelf);
		while (++p->m_iCount < 100);
	}
	RunFunc() : m_iCount(0) {}
	int GetCount() const { return m_iCount; }
private:
	volatile int m_iCount;
};

TEST(Thread_Test, RunFuncTest)
{
	RunFunc cRunFunc;
	ASSERT_EQ(0, cRunFunc.GetCount());
	{
		Thread cThread(&RunFunc::Run, &cRunFunc);
		cThread.Join();
	}
	ASSERT_NE(0, cRunFunc.GetCount());
}

TEST(Thread_Test, SwapTest)
{
	const unsigned int NUMBER = 100;
	RunCounter acRunCounter[NUMBER];
	{
		Thread acThreads[NUMBER];
		for (unsigned int n = 0 ; n < NUMBER ; ++n)
		{
			ASSERT_FALSE(acThreads[n].Joinable());
			Thread cThread(acRunCounter[n]);
			acThreads[n].Swap(cThread);
			ASSERT_TRUE(acThreads[n].Joinable());
			ASSERT_FALSE(cThread.Joinable());
		}

		GUTIL_FOREACH_ASSERT_LIMIT(
			n, NUMBER, GUTIL_MAX_WAITING_MS,
			acThreads[n].Join(GUTIL_AVG_MEASURE_MS));
	}

	for (unsigned int n = 0 ; n < NUMBER ; ++n)
		ASSERT_EQ(acRunCounter[n].GetCountTo(), acRunCounter[n].GetCount());
}

class NonStop : public Runnable
{
public:
	NonStop() : m_iCount(0) {}
	void operator() ()
	{
		while (1)
		{
			++m_iCount;
			gtest_utility::thread_msleep(GUTIL_AVG_MEASURE_MS);
		}
	}
	int GetCount() const { return m_iCount; }
private:
	volatile int m_iCount;
};

TEST(Thread_Test, TerminateTest)
{
	NonStop cNonStop;
	{
		Thread cThread(cNonStop);

		ASSERT_TRUE(cThread.Joinable());

		GUTIL_ASSERT_LIMIT(
			GUTIL_MAX_WAITING_MS,
			cNonStop.GetCount() > 0);

		cThread.Terminate();

		int iLastCount = cNonStop.GetCount();
		gtest_utility::thread_msleep(GUTIL_AVG_MEASURE_MS);
		ASSERT_EQ(iLastCount, cNonStop.GetCount());
	}
}

TEST(Thread_Test, DestructTest)
{
	NonStop cNonStop;
	{
		Thread cThread(cNonStop);

		ASSERT_TRUE(cThread.Joinable());

		GUTIL_ASSERT_LIMIT(
			GUTIL_MAX_WAITING_MS,
			cNonStop.GetCount() > 0);
	}
	int iLastCount = cNonStop.GetCount();
	gtest_utility::thread_msleep(GUTIL_AVG_MEASURE_MS);
	ASSERT_EQ(iLastCount, cNonStop.GetCount());
}

class WaitStop : public Runnable
{
public:
	WaitStop() : m_bStop(false) {}
	void operator() ()
	{
		while (!m_bStop)
		{
			gtest_utility::thread_msleep(GUTIL_AVG_MEASURE_MS);
		}
	}
	void Stop() { m_bStop = true; }
private:
	volatile bool m_bStop;
};


TEST(Thread_Test, JoinTest)
{
	WaitStop cWaitStop;
	{
		Thread cThread(cWaitStop);

		GUTIL_ASSERT_USETIME(
			GUTIL_AVG_MEASURE_MS, GUTIL_MEASURE_ERR_RANGE,
		{
			ASSERT_FALSE(cThread.Join(GUTIL_AVG_MEASURE_MS));
		}
		);

		cWaitStop.Stop();

		GUTIL_ASSERT_LIMIT(
			GUTIL_MAX_WAITING_MS,
			true == cThread.Join(GUTIL_AVG_MEASURE_MS));
	}
}

/// Thread_StaticTest
///

TEST(Thread_StaticTest, SleepTime)
{
	GUTIL_ASSERT_USETIME(1000, 0.1, Thread::Sleep(1));
}

TEST(Thread_StaticTest, MSleepTime)
{
	GUTIL_ASSERT_USETIME(
		GUTIL_AVG_MEASURE_MS,
		GUTIL_MEASURE_ERR_RANGE,
		Thread::MSleep(GUTIL_AVG_MEASURE_MS));
}

TEST(Thread_StaticTest, GetCurrentThreadId)
{
	ASSERT_TRUE(Thread::GetCurrentThreadId() == gtest_utility::thread_get_threadid());
}
