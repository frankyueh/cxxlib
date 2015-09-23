
#include "cxxlib_cio.h"

#include "gtest/gtest.h"
#include "gtest_utilities.h"

using namespace cxxlib;

// cxxlib_cio__fopen_unixlikeTest
//-----------------------------------------------------------------------------

TEST(cxxlib_cio__fopen_unixlikeTest, SharedOpen)
{
	const char szFilePath[] = "./cxxlib_cio__fopen_unixlike.SharedOpen.test";

	FILE *pstFileA = fopen_unixlike(szFilePath, "wb");
	ASSERT_TRUE(pstFileA != NULL);

	FILE *pstFileB = fopen_unixlike(szFilePath, "rb");
	ASSERT_TRUE(pstFileB != NULL);

	FILE *pstFileC = fopen_unixlike(szFilePath, "ab");
	ASSERT_TRUE(pstFileC != NULL);

	ASSERT_NE(0, ::remove(szFilePath));

	ASSERT_NE(pstFileA, pstFileB);
	ASSERT_NE(pstFileB, pstFileC);
	ASSERT_NE(pstFileC, pstFileA);

	ASSERT_EQ(0, ::fclose(pstFileA));
	ASSERT_EQ(0, ::fclose(pstFileB));
	ASSERT_EQ(0, ::fclose(pstFileC));

	ASSERT_EQ(0, ::remove(szFilePath));
}

// cxxlib_cio__fopen_unixlikeDEATHTest
//-----------------------------------------------------------------------------

TEST(cxxlib_cio__fopen_unixlikeDEATHTest, NullInput)
{
#if _DEBUG
	ASSERT_DEATH(fopen_unixlike(NULL, NULL), "");
	const char szFilePath[] = "./cxxlib_cio__fopen_unixlike.NullInput.test";
	ASSERT_DEATH(fopen_unixlike(szFilePath, NULL), "");
#endif
	ASSERT_EQ(NULL, fopen_unixlike("", "w"));
}
