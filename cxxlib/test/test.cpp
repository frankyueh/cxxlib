
// NOTE:	If enable VLD, may need to prevent expected memory
//			leaks by disable death tests.
// COMMAND:	--gtest_filter=-*DEATH*
#ifdef _WIN
//#	define VLD
#endif
#ifdef VLD
#	include <vld.h>
#endif

#include "gtest/gtest.h"
#include "gtest_utilities.h"

int main(int iArgc, char **pchArgv)
{
	::testing::InitGoogleTest(&iArgc, pchArgv);
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	return RUN_ALL_TESTS();
}
