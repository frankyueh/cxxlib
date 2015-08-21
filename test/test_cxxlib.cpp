
#include "cxxlib.h"

#include "gtest/gtest.h"
#include "gtest_utilities.h"

using namespace cxxlib;


class cxxlib__polymorphic_downcastBaseClass
{
public:
	virtual ~cxxlib__polymorphic_downcastBaseClass() {}
};
class cxxlib__polymorphic_downcastDeriveClass : public cxxlib__polymorphic_downcastBaseClass {};

TEST(cxxlib__polymorphic_downcastDEATHTest, SharedOpen)
{
#ifdef _DEBUG
	cxxlib__polymorphic_downcastBaseClass *pcBase = new cxxlib__polymorphic_downcastBaseClass();
	EXPECT_DEATH(polymorphic_downcast<cxxlib__polymorphic_downcastDeriveClass *>(pcBase), "");
#endif
}
