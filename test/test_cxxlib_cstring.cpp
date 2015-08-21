
#include "cxxlib_cstring.h"

#include "gtest/gtest.h"
#include "gtest_utilities.h"

using namespace cxxlib;

// cxxlib_cstring__sprintfTest
//-----------------------------------------------------------------------------

TEST(cxxlib_cstring__sprintfTest, FittingLength)
{
	char szBuffer[32] = {0};
	EXPECT_EQ(sizeof(szBuffer) - 1, cxxlib::sprintf(szBuffer, sizeof(szBuffer), "0123456789012345678901234567890"));
	EXPECT_STREQ("0123456789012345678901234567890", szBuffer);
}

TEST(cxxlib_cstring__sprintfTest, LengthOverflow)
{
	char szBuffer[32] = {0};
	EXPECT_EQ(sizeof(szBuffer) - 1, cxxlib::sprintf(szBuffer, sizeof(szBuffer), "%s01234567890123456789012345678901", "01234"));
	EXPECT_STREQ("0123401234567890123456789012345", szBuffer);
}

TEST(cxxlib_cstring__sprintfTest, FormatError)
{
	char szBuffer[32] = {0};
	EXPECT_EQ(0, cxxlib::sprintf(szBuffer, sizeof(szBuffer), "%", 0));
	EXPECT_STREQ("", szBuffer);
}

// cxxlib_cstring__strcpy_nTest
//-----------------------------------------------------------------------------

TEST(cxxlib_cstring__strcpy_nTest, FittingLength)
{
	char szBuffer[32];
	cxxlib::strcpy_n(szBuffer, sizeof(szBuffer) - 1, "0123456789012345678901234567890");
	EXPECT_STREQ("0123456789012345678901234567890", szBuffer);
}

TEST(cxxlib_cstring__strcpy_nTest, LengthOverflow)
{
	char szBuffer[32];
	cxxlib::strcpy_n(szBuffer, sizeof(szBuffer) - 1, "01234567890123456789012345678901");
	EXPECT_STREQ("0123456789012345678901234567890", szBuffer);
}

// cxxlib_cstring__strcpy_mTest
//-----------------------------------------------------------------------------

TEST(cxxlib_cstring__strcpy_mTest, FittingLength)
{
	char szBuffer[32], achBuffer[31];
	memcpy(achBuffer, "0123456789012345678901234567890", sizeof(achBuffer));
	cxxlib::strcpy_m(szBuffer, sizeof(szBuffer) - 1, achBuffer, sizeof(achBuffer));
	EXPECT_STREQ("0123456789012345678901234567890", szBuffer);
}

TEST(cxxlib_cstring__strcpy_mTest, LengthOverflow)
{
	char szBuffer[32], achBuffer[32];
	memcpy(achBuffer, "01234567890123456789012345678901", sizeof(achBuffer));
	cxxlib::strcpy_m(szBuffer, sizeof(szBuffer) - 1, achBuffer, sizeof(achBuffer));
	EXPECT_STREQ("0123456789012345678901234567890", szBuffer);
}
