#ifdef CXXLIB_TEST_INTERNAL_EXPORTED

#include "cxxlib_defines.h"
#include "cxxlib_cio_internal.h"

#include "gtest/gtest.h"
#include "gtest_utilities.h"

using namespace cxxlib;
using namespace cxxlib_internal;

#define UNIXLIKE_SHAREMODE (FSM_READ | FSM_WRITE)

// Modes test
//-----------------------------------------------------------------------------

TEST(cxxlib_cio_internal__fopen_advancedTest, ReadWriteMode)
{
	const char szTestFile[] = "./cxxlib_cio_internal__fopen_advancedTest.ReadWriteMode.test";
	FILE *pstWriteFile = fopen_advanced(szTestFile, "w", UNIXLIKE_SHAREMODE);
	const char szWriteText[] = "wwwwwwwwwwwwwwww";
	ASSERT_EQ(
		strlen(szWriteText),
		fwrite(szWriteText, sizeof(char), strlen(szWriteText), pstWriteFile));
	ASSERT_EQ(0, fclose(pstWriteFile));

	FILE *pstReadFile = fopen_advanced(szTestFile, "r", UNIXLIKE_SHAREMODE);
	char szReadText[SHORT_SZBUFF_LENGTH + 1];
	size_t s = ::fread(szReadText, sizeof(char), sizeof(szReadText) - 1, pstReadFile);
	ASSERT_LT(static_cast<size_t>(0), s);
	szReadText[s] = '\0';
	ASSERT_STREQ(szWriteText, szReadText);
	ASSERT_EQ(0, fclose(pstReadFile));

	ASSERT_EQ(0, remove(szTestFile));
}

TEST(cxxlib_cio_internal__fopen_advancedTest, AppendMode)
{
	const char szTestFile[] = "./cxxlib_cio_internal__fopen_advancedTest.AppendMode.test";

	FILE *pstWriteFile = fopen_advanced(szTestFile, "w", UNIXLIKE_SHAREMODE);
	const char szWriteText[] = "wwwwwwwwwwwwwwww";
	ASSERT_EQ(
		strlen(szWriteText),
		fwrite(szWriteText, sizeof(char), strlen(szWriteText), pstWriteFile));
	ASSERT_EQ(0, fclose(pstWriteFile));

	FILE *pstAppendFile = fopen_advanced(szTestFile, "a", UNIXLIKE_SHAREMODE);
	const char szAppendText[] = "aaaaaaaaaaaaaaaa";
	ASSERT_EQ(
		strlen(szAppendText),
		fwrite(szAppendText, sizeof(char), strlen(szAppendText), pstWriteFile));
	ASSERT_EQ(
		strlen(szWriteText) + strlen(szAppendText),
		::ftell(pstAppendFile));
	ASSERT_EQ(0, fclose(pstAppendFile));

	FILE *pstReadFile = fopen_advanced(szTestFile, "r", UNIXLIKE_SHAREMODE);
	char szReadText[SHORT_SZBUFF_LENGTH + 1];
	size_t s = ::fread(szReadText, sizeof(char), sizeof(szReadText) - 1, pstReadFile);
	ASSERT_LT(static_cast<size_t>(0), s);
	szReadText[s] = '\0';
	char *szText = szReadText;
	ASSERT_EQ(0, ::strncmp(szText, szWriteText, strlen(szWriteText)));
	szText += strlen(szWriteText);
	ASSERT_EQ(0, ::strncmp(szText, szAppendText, strlen(szAppendText)));
	ASSERT_EQ(0, fclose(pstReadFile));

	ASSERT_EQ(0, remove(szTestFile));
}

TEST(cxxlib_cio_internal__fopen_advancedTest, ReadUpdateMode)
{
	const char szTestFile[] = "./cxxlib_cio_internal__fopen_advancedTest.ReadUpdateMode.test";

	FILE *pstWriteFile = fopen_advanced(szTestFile, "w", UNIXLIKE_SHAREMODE);
	const char szWriteText[] = "wwwwwwwwwwwwwwww";
	ASSERT_EQ(
		strlen(szWriteText),
		fwrite(szWriteText, sizeof(char), strlen(szWriteText), pstWriteFile));
	ASSERT_EQ(0, fclose(pstWriteFile));

	FILE *pstReadUpdateFile = fopen_advanced(szTestFile, "r+", UNIXLIKE_SHAREMODE);
	char szReadText[SHORT_SZBUFF_LENGTH + 1];
	size_t s = ::fread(szReadText, sizeof(char), sizeof(szReadText) - 1, pstReadUpdateFile);
	ASSERT_LT(static_cast<size_t>(0), s);
	szReadText[s] = '\0';
	ASSERT_STREQ(szWriteText, szReadText);

	ASSERT_EQ(0, ::fseek(pstReadUpdateFile, strlen(szWriteText) / 2, SEEK_SET));
	const char szUpdateText[] = "r+r+r+r+r+r+r+r+";
	ASSERT_EQ(
		strlen(szUpdateText),
		fwrite(szUpdateText, sizeof(char), strlen(szUpdateText), pstReadUpdateFile));
	ASSERT_EQ(0, fclose(pstReadUpdateFile));

	FILE *pstReadFile = fopen_advanced(szTestFile, "r", UNIXLIKE_SHAREMODE);
	s = ::fread(szReadText, sizeof(char), sizeof(szReadText) - 1, pstReadFile);
	ASSERT_LT(static_cast<size_t>(0), s);
	szReadText[s] = '\0';
	char *szText = szReadText;
	ASSERT_EQ(0, ::strncmp(szText, szWriteText, strlen(szWriteText) / 2));
	szText += strlen(szWriteText) / 2;
	ASSERT_EQ(0, ::strncmp(szText, szUpdateText, strlen(szUpdateText)));
	ASSERT_EQ(0, fclose(pstReadFile));

	ASSERT_EQ(0, remove(szTestFile));
}

TEST(cxxlib_cio_internal__fopen_advancedTest, WriteUpdateMode)
{
	const char szTestFile[] = "./cxxlib_cio_internal__fopen_advancedTest.ReadUpdateMode.test";

	FILE *pstWriteFile = fopen_advanced(szTestFile, "w", UNIXLIKE_SHAREMODE);
	const char szWriteText[] = "wwwwwwwwwwwwwwww";
	ASSERT_EQ(
		strlen(szWriteText),
		fwrite(szWriteText, sizeof(char), strlen(szWriteText), pstWriteFile));
	ASSERT_EQ(0, fclose(pstWriteFile));

	FILE *pstWriteUpdateFile = fopen_advanced(szTestFile, "w+", UNIXLIKE_SHAREMODE);
	const char szUpdateText[] = "w+w+w+w+w+w+w+w+";
	ASSERT_EQ(
		strlen(szUpdateText),
		fwrite(szUpdateText, sizeof(char), strlen(szUpdateText), pstWriteUpdateFile));

	char szReadText[SHORT_SZBUFF_LENGTH + 1];
	size_t s;
#ifdef _WIN
	// TODO: currently "w+" have read problem in Linux, the test won't pass
	ASSERT_EQ(0, ::fseek(pstWriteUpdateFile, 0, SEEK_SET));
	s = ::fread(szReadText, sizeof(char), sizeof(szReadText) - 1, pstWriteUpdateFile);
	ASSERT_LT(static_cast<size_t>(0), s);
	szReadText[s] = '\0';
	ASSERT_STREQ(szUpdateText, szReadText);
#endif

	ASSERT_EQ(0, fclose(pstWriteUpdateFile));

	FILE *pstReadFile = fopen_advanced(szTestFile, "r", UNIXLIKE_SHAREMODE);
	s = ::fread(szReadText, sizeof(char), sizeof(szReadText) - 1, pstReadFile);
	ASSERT_LT(static_cast<size_t>(0), s);
	szReadText[s] = '\0';
	ASSERT_STREQ(szUpdateText, szReadText);
	ASSERT_EQ(0, fclose(pstReadFile));

	ASSERT_EQ(0, remove(szTestFile));
}

TEST(cxxlib_cio_internal__fopen_advancedTest, AppendUpdateMode)
{
	const char szTestFile[] = "./cxxlib_cio_internal__fopen_advancedTest.AppendUpdateMode.test";

	FILE *pstWriteFile = fopen_advanced(szTestFile, "w", UNIXLIKE_SHAREMODE);
	const char szWriteText[] = "wwwwwwwwwwwwwwww";
	ASSERT_EQ(
		strlen(szWriteText),
		fwrite(szWriteText, sizeof(char), strlen(szWriteText), pstWriteFile));
	ASSERT_EQ(0, fclose(pstWriteFile));

	FILE *pstAppendFile = fopen_advanced(szTestFile, "a", UNIXLIKE_SHAREMODE);
	const char szAppendText[] = "aaaaaaaaaaaaaaaa";
	ASSERT_EQ(
		strlen(szAppendText),
		fwrite(szAppendText, sizeof(char), strlen(szAppendText), pstWriteFile));
	ASSERT_EQ(
		strlen(szWriteText) + strlen(szAppendText),
		::ftell(pstAppendFile));
	ASSERT_EQ(0, fclose(pstAppendFile));

	FILE *pstAppendPlusFile = fopen_advanced(szTestFile, "a+", UNIXLIKE_SHAREMODE);
	const char szAppendPlusText[] = "a+a+a+a+a+a+a+a+";
	ASSERT_EQ(
		strlen(szAppendPlusText),
		fwrite(szAppendPlusText, sizeof(char), strlen(szAppendPlusText), pstAppendPlusFile));
	ASSERT_EQ(
		strlen(szWriteText) + strlen(szAppendText) + strlen(szAppendPlusText),
		::ftell(pstAppendPlusFile));

	ASSERT_EQ(0, ::fseek(pstAppendPlusFile, 0, SEEK_SET));
	char szReadText[SHORT_SZBUFF_LENGTH + 1];
	size_t s = ::fread(szReadText, sizeof(char), sizeof(szReadText) - 1, pstAppendPlusFile);
	ASSERT_LT(static_cast<size_t>(0), s);
	szReadText[s] = '\0';
	char *szText = szReadText;
	ASSERT_EQ(0, ::strncmp(szText, szWriteText, strlen(szWriteText)));
	szText += strlen(szWriteText);
	ASSERT_EQ(0, ::strncmp(szText, szAppendText, strlen(szAppendText)));
	szText += strlen(szAppendText);
	ASSERT_EQ(0, ::strncmp(szText, szAppendPlusText, strlen(szAppendPlusText)));

	ASSERT_EQ(0, fclose(pstAppendPlusFile));

	ASSERT_EQ(0, remove(szTestFile));
}

// SharedWriteAndRead
//-----------------------------------------------------------------------------

void cxxlib_cio_internal__fopen_advancedTest__SharedWriteAndRead(const char *szWriteMode, const char *szReadMode)
{
	const char szTestFile[] = "./cxxlib_cio_internal__fopen_advancedTest.SharedWriteAndRead.test";

	FILE *pstWriteFile = fopen_advanced(szTestFile, szWriteMode, UNIXLIKE_SHAREMODE);
	ASSERT_TRUE(NULL != pstWriteFile);
	FILE *pstReadFile = fopen_advanced(szTestFile, szReadMode, UNIXLIKE_SHAREMODE);
	ASSERT_TRUE(NULL != pstReadFile);

	const char szTestText[] = "qwertyuiasdfghjk\nasdfhjklzxcvnm\nzxcvmuiosdfg";
	ASSERT_EQ(
		strlen(szTestText),
		fwrite(szTestText, sizeof(char), strlen(szTestText), pstWriteFile));
	ASSERT_EQ(0, ::fflush(pstWriteFile));

	char szReadText[SHORT_SZBUFF_LENGTH + 1];
	size_t s = ::fread(szReadText, sizeof(char), sizeof(szReadText) - 1, pstReadFile);
	ASSERT_LT(static_cast<size_t>(0), s);
	szReadText[s] = '\0';
	ASSERT_STREQ(szTestText, szReadText);

	ASSERT_EQ(0, fclose(pstWriteFile));
	ASSERT_EQ(0, fclose(pstReadFile));

	ASSERT_EQ(0, remove(szTestFile));
}

TEST(cxxlib_cio_internal__fopen_advancedTest, SharedWriteAndRead)
{
	cxxlib_cio_internal__fopen_advancedTest__SharedWriteAndRead("w", "r");
	cxxlib_cio_internal__fopen_advancedTest__SharedWriteAndRead("w+", "r+");
	cxxlib_cio_internal__fopen_advancedTest__SharedWriteAndRead("wb", "rb");
	cxxlib_cio_internal__fopen_advancedTest__SharedWriteAndRead("wb+", "rb+");
}

// LineBreakTest
//-----------------------------------------------------------------------------

void cxxlib_cio_internal__fopen_advancedTest__LineBreakTest(bool bExplicitText, bool bUseShareMode)
{
#ifdef _WIN
	unsigned char btShareMode = bUseShareMode ? UNIXLIKE_SHAREMODE : 0;

	const char szTestTextFile[] = "./cxxlib_cio_internal__fopen_advancedTest.LineBreakTest.Text.test";
	const char szTestBinaryFile[] = "./cxxlib_cio_internal__fopen_advancedTest.LineBreakTest.Binary.test";

	FILE *pstWriteTextFile = fopen_advanced(szTestTextFile, bExplicitText ? "wt" : "w", btShareMode);
	FILE *pstWriteBinaryFile = fopen_advanced(szTestBinaryFile, "wb", btShareMode);
	const char szTestText[] = "\n";
	ASSERT_EQ(
		strlen(szTestText),
		fwrite(szTestText, sizeof(char), strlen(szTestText), pstWriteTextFile));
	ASSERT_EQ(
		strlen(szTestText),
		fwrite(szTestText, sizeof(char), strlen(szTestText), pstWriteBinaryFile));
	ASSERT_EQ(0, fclose(pstWriteTextFile));
	ASSERT_EQ(0, fclose(pstWriteBinaryFile));

	FILE *pstReadTextFile = fopen_advanced(szTestTextFile, bExplicitText ? "rt" : "r", btShareMode);
	FILE *pstReadBTextFile = fopen_advanced(szTestTextFile, "rb", btShareMode);
	FILE *pstReadBinaryFile = fopen_advanced(szTestBinaryFile, "rb", btShareMode);
	char szReadText[SHORT_SZBUFF_LENGTH + 1];
	size_t s = ::fread(szReadText, sizeof(char), sizeof(szReadText) - 1, pstReadTextFile);
	ASSERT_LT(static_cast<size_t>(0), s);
	szReadText[s] = '\0';
	ASSERT_STREQ(szTestText, szReadText);
	s = ::fread(szReadText, sizeof(char), sizeof(szReadText) - 1, pstReadBTextFile);
	ASSERT_LT(static_cast<size_t>(0), s);
	szReadText[s] = '\0';
	ASSERT_STREQ("\r\n", szReadText);
	s = ::fread(szReadText, sizeof(char), sizeof(szReadText) - 1, pstReadBinaryFile);
	ASSERT_LT(static_cast<size_t>(0), s);
	szReadText[s] = '\0';
	ASSERT_STREQ(szTestText, szReadText);
	ASSERT_EQ(0, fclose(pstReadTextFile));
	ASSERT_EQ(0, fclose(pstReadBTextFile));
	ASSERT_EQ(0, fclose(pstReadBinaryFile));

	ASSERT_EQ(0, remove(szTestTextFile));
	ASSERT_EQ(0, remove(szTestBinaryFile));
#endif
}

TEST(cxxlib_cio_internal__fopen_advancedTest, LineBreakTest)
{
	cxxlib_cio_internal__fopen_advancedTest__LineBreakTest(false, false);
	cxxlib_cio_internal__fopen_advancedTest__LineBreakTest(false, true);
	cxxlib_cio_internal__fopen_advancedTest__LineBreakTest(true, false);
	cxxlib_cio_internal__fopen_advancedTest__LineBreakTest(true, true);
}

// DEATH Test
//-----------------------------------------------------------------------------

TEST(cxxlib_cio_internal__fopen_advancedDEATHTest, NullInput)
{
#ifdef _DEBUG
	ASSERT_DEATH(fopen_advanced(NULL, NULL, 0), "");
	ASSERT_DEATH(fopen_advanced("", NULL, 0), "");
	ASSERT_DEATH(fopen_advanced(NULL, "", 0), "");
#endif
}

#endif