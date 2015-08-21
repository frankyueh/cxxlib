
#include "ScopedTyp.h"

#include <set>

#include "gtest/gtest.h"
#include "gtest_utilities.h"

using namespace cxxlib;

class ScopedTyp_Test : public ::testing::Test
{
protected:
	typedef int Id;
	typedef std::set<Id> IdSet;
	static Id s_iCurId;
	static IdSet s_cIdSet;

	Id CreateId()
	{ EXPECT_TRUE(s_cIdSet.insert(s_iCurId).second); return s_iCurId++; }

	struct ReleaseId
	{ void operator ()(Id i) { ASSERT_EQ(1, s_cIdSet.erase(i)); } };

	struct IsNulId
	{ bool operator ()(Id i) const { return i == -1; } };

	virtual void SetUp() { s_iCurId = 0; }
	virtual void TearDown() { ASSERT_EQ(0, s_cIdSet.size()); }
};
ScopedTyp_Test::Id ScopedTyp_Test::s_iCurId = 0;
ScopedTyp_Test::IdSet ScopedTyp_Test::s_cIdSet;


TEST_F(ScopedTyp_Test, GerneralTest)
{
	ScopedTyp<Id> cScopedTyp(-1, ReleaseId(), IsNulId());
	ASSERT_TRUE(cScopedTyp.IsNull());
	ASSERT_FALSE(cScopedTyp);
	ASSERT_EQ(-1, cScopedTyp.Get());

	Id idA = CreateId();
	cScopedTyp.Reset(idA);
	ASSERT_FALSE(cScopedTyp.IsNull());
	ASSERT_TRUE(cScopedTyp);
	ASSERT_NE(-1, cScopedTyp.Get());
	ASSERT_EQ(idA, cScopedTyp.Get());

	Id idB = CreateId();
	cScopedTyp.Reset(idB);
	ASSERT_EQ(idB, cScopedTyp.Get());

	cScopedTyp.Reset(-1);
	ASSERT_TRUE(cScopedTyp.IsNull());
	ASSERT_FALSE(cScopedTyp);
	ASSERT_EQ(-1, cScopedTyp.Get());
}

TEST_F(ScopedTyp_Test, SwapTest)
{
	Id idA = CreateId();
	ScopedTyp<Id> cScopedTypA(idA, ReleaseId(), IsNulId());
	Id idB = CreateId();
	ScopedTyp<Id> cScopedTypB(idB, ReleaseId(), IsNulId());

	ASSERT_EQ(idA, cScopedTypA.Get());
	ASSERT_EQ(idB, cScopedTypB.Get());
	swap(cScopedTypA, cScopedTypB);
	ASSERT_EQ(idB, cScopedTypA.Get());
	ASSERT_EQ(idA, cScopedTypB.Get());
	cScopedTypA.Swap(cScopedTypB);
	ASSERT_EQ(idA, cScopedTypA.Get());
	ASSERT_EQ(idB, cScopedTypB.Get());
}

TEST_F(ScopedTyp_Test, TypeTest)
{
	const ScopedTyp<Id> cScopedTyp(CreateId(), ReleaseId(), IsNulId());
	ASSERT_FALSE(cScopedTyp.IsNull());
	ASSERT_TRUE(cScopedTyp);
	ASSERT_NE(-1, cScopedTyp.Get());
	cScopedTyp.Get();

	const ScopedTyp<Id> cScopedTypB(CreateId(), ReleaseId(), IsNulId());
	//cScopedTyp.Reset();				// expect: won't pass at compile time
	//cScopedTyp.Swap(cScopedTypB);		// expect: won't pass at compile time
	//cScopedTyp = CreateId();			// expect: won't pass at compile time

	ScopedTyp<Id>
		cScopedTypC(CreateId(), ReleaseId(), IsNulId()),
		cScopedTypD(CreateId(), ReleaseId(), IsNulId());
	cScopedTypC < cScopedTypD;
	// NOTE: this kind of object is not allowed copy and assigned copy for its type
	//cScopedTypC = cScopedTypD;									// expect: won't pass at compile time
	//ScopedTyp<Id, ReleaseId, IsNulId> cScopedTypE = cScopedTypD;	// expect: won't pass at compile time
}
