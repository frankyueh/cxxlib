
#include "cxxlib_time.h"

#include "gtest/gtest.h"
#include "gtest_utilities.h"

using namespace cxxlib;

// cxxlib_timeTest
//-----------------------------------------------------------------------------

TEST(cxxlib_timeTest, tick_freq_Getter)
{
	EXPECT_LT(0.0, tick_freq());
}

TEST(cxxlib_timeTest, ticks_LeastSensitive)
{
	double dbPrevTick = ticks();
	EXPECT_LT(0.0, tick_freq());
	double dbNextTick = ticks();
	EXPECT_LT(dbPrevTick, dbNextTick);
}

TEST(cxxlib_timeTest, ticks_Timing)
{
	double dbStartTicks;
	double dbStartTicks2 = ticks(&dbStartTicks);
	EXPECT_EQ(dbStartTicks, dbStartTicks2);

	gtest_utility::thread_msleep(GUTIL_AVG_MEASURE_MS);

	UINT64 u64Duration = static_cast<UINT64>((ticks() - dbStartTicks) * 1000 / tick_freq());
	UINT64 u64ErrorRange =
		static_cast<UINT64>(GUTIL_AVG_MEASURE_MS * GUTIL_MEASURE_ERR_RANGE);

	EXPECT_LE(u64Duration, GUTIL_AVG_MEASURE_MS + u64ErrorRange);
	EXPECT_GE(u64Duration, GUTIL_AVG_MEASURE_MS - u64ErrorRange);
}

TEST(cxxlib_timeTest, utime_Timing)
{
	UINT64 u64StartTime;
	UINT64 u64StartTime2 = utime(&u64StartTime);
	EXPECT_EQ(u64StartTime, u64StartTime2);

	gtest_utility::thread_msleep(GUTIL_AVG_MEASURE_MS);

	UINT64 u64Duration = (utime() - u64StartTime) / 1000;
	UINT64 u64ErrorRange =
		static_cast<UINT64>(GUTIL_AVG_MEASURE_MS * GUTIL_MEASURE_ERR_RANGE);

	EXPECT_LE(u64Duration, GUTIL_AVG_MEASURE_MS + u64ErrorRange);
	EXPECT_GE(u64Duration, GUTIL_AVG_MEASURE_MS - u64ErrorRange);
}

TEST(cxxlib_timeTest, mtime_Timing)
{
	UINT64 u64StartTime;
	UINT64 u64StartTime2 = mtime(&u64StartTime);
	EXPECT_EQ(u64StartTime, u64StartTime2);

	gtest_utility::thread_msleep(GUTIL_AVG_MEASURE_MS);

	UINT64 u64Duration = mtime() - u64StartTime;
	UINT64 u64ErrorRange =
		static_cast<UINT64>(GUTIL_AVG_MEASURE_MS * GUTIL_MEASURE_ERR_RANGE);

	EXPECT_LE(u64Duration, GUTIL_AVG_MEASURE_MS + u64ErrorRange);
	EXPECT_GE(u64Duration, GUTIL_AVG_MEASURE_MS - u64ErrorRange);
}

TEST(cxxlib_timeTest, time_Timing)
{
	UINT64 u64StartTime;
	UINT64 u64StartTime2 = time(&u64StartTime);
	EXPECT_EQ(u64StartTime, u64StartTime2);

	gtest_utility::thread_msleep(1000 + GUTIL_AVG_MEASURE_MS);

	EXPECT_LE(time() - u64StartTime, 2);
	EXPECT_GE(time() - u64StartTime, 1);
}
