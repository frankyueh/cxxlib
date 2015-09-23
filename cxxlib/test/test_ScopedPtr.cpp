
#include "test_ScopedPtr.h"

#include "gtest/gtest.h"
#include "gtest_utilities.h"

using namespace cxxlib;
using namespace ScopedPtrTest;

// TODO: test is not completed

class ScopedPtr_Test : public ::testing::Test
{
protected:
	static const int ARRAY_SIZE = 1000;
	static int s_iObjCount;

	class Obj
	{
	public:
		Obj() : m_iId(s_iObjCount)	{ ++s_iObjCount; }
		Obj(int id) : m_iId(id)		{ ++s_iObjCount; }
		virtual ~Obj()				{ --s_iObjCount; }
		int GetId() const			{ return m_iId; }
	private:
		int m_iId;
	};

	class DerivedObj : public Obj
	{
	public:
		DerivedObj()			{ ++s_iObjCount; }
		virtual ~DerivedObj()	{ --s_iObjCount; }
	};

	static void ObjDeleter(Obj *p) { delete p; }
	static void ObjsDeleter(Obj *p) { delete [] p; }

	virtual void SetUp()	{ s_iObjCount = 0; }
	virtual void TearDown()	{ ASSERT_EQ(0, s_iObjCount); }
};
int ScopedPtr_Test::s_iObjCount = 0;

namespace ScopedPtrTest
{
	class ForwardDeclared
	{
	public:
		ForwardDeclared(int *pi) : m_piCount(pi) { ++*m_piCount; }
		~ForwardDeclared() { --*m_piCount; }
	private:
		int *m_piCount;
	};
	TestForwardDeclared::TestForwardDeclared(int *pi) : m_qcPtr(new ForwardDeclared(pi)) {}
}

TEST_F(ScopedPtr_Test, ForwardDeclaredTest)
{
	TestForwardDeclared cForwardDeclaredObj(&s_iObjCount);
}

TEST_F(ScopedPtr_Test, NullTest)
{
}

TEST_F(ScopedPtr_Test, GerneralTest)
{
	ScopedPtr<Obj> qcObj;
	ASSERT_TRUE(qcObj.IsNull());
	ASSERT_FALSE(qcObj);
	ASSERT_EQ(0, qcObj.Get());
	ASSERT_TRUE(qcObj == ScopedPtr<Obj>::NUL);
	ASSERT_TRUE(0 == qcObj.GetDeleter< Deep::PtrDeleter<Obj> >());

	int id0 = 0;
	Obj *obj = new Obj(id0);
	qcObj.Reset(obj);
	ASSERT_FALSE(qcObj.IsNull());
	ASSERT_TRUE(qcObj);
	ASSERT_TRUE(0 != qcObj.Get());
	ASSERT_EQ(obj, qcObj.Get());
	ASSERT_TRUE(0 != qcObj.GetDeleter< Deep::PtrDeleter<Obj> >());
	
	ASSERT_EQ(id0, qcObj->GetId());
	ASSERT_EQ(id0, (*qcObj.Get()).GetId());

	int id1 = 0;
	qcObj.Reset(new Obj(id1));
	ASSERT_EQ(id1, qcObj->GetId());
	ASSERT_EQ(id1, (*qcObj.Get()).GetId());

	qcObj.Reset();
	ASSERT_TRUE(qcObj.IsNull());
	ASSERT_FALSE(qcObj);
	ASSERT_TRUE(0 == qcObj.Get());
	ASSERT_TRUE(0 == qcObj.GetDeleter< Deep::PtrDeleter<Obj> >());

	Obj *pcObj = new Obj;
	ScopedPtr<void> qcVoidObj(pcObj);
	ASSERT_TRUE(qcVoidObj);
	ASSERT_TRUE(pcObj == qcVoidObj.Get());
}

TEST_F(ScopedPtr_Test, ArrayTest)
{
	ScopedPtr<Obj> qcObj(new Obj[ARRAY_SIZE], Deep::PtrDeleter<Obj[]>());
	ASSERT_EQ(0, qcObj.GetDeleter< Deep::PtrDeleter<Obj> >());
	ASSERT_TRUE(0 != qcObj.GetDeleter< Deep::PtrDeleter<Obj[]> >());
	(*qcObj.GetDeleter< Deep::PtrDeleter<Obj[]> >())(new Obj[ARRAY_SIZE]);

	ScopedPtr<Obj[]> qacObj(new Obj[ARRAY_SIZE]);
	int iLast = -1;
	for (size_t s = 0 ; s < ARRAY_SIZE ; ++s)
	{
		if (iLast >= 0)
			ASSERT_EQ(iLast + 1, qacObj[s].GetId());
		iLast = qacObj[s].GetId();
	}
	ASSERT_TRUE(qacObj != ScopedPtr<Obj[]>::NUL);
	ScopedPtr<Obj[]> qacObjD(new Obj[ARRAY_SIZE], Deep::PtrDeleter<Obj[]>());

	ScopedPtr<Obj[]> qacDerObj(new DerivedObj[ARRAY_SIZE]);
	iLast = -1;
	for (size_t s = 0 ; s < ARRAY_SIZE ; ++s)
	{
		if (iLast >= 0)
			ASSERT_EQ(iLast + 2, qacDerObj[s].GetId());
		iLast = qacDerObj[s].GetId();
	}
}

TEST_F(ScopedPtr_Test, SwapTest)
{
	int id0 = 0;
	Obj *obj0 = new Obj(id0);
	ScopedPtr<Obj> qcObj0(obj0);
	int id1 = 1;
	Obj *obj1 = new Obj(id1);
	ScopedPtr<Obj> qcObj1(obj1);

	ASSERT_EQ(obj0, qcObj0.Get());
	ASSERT_EQ(obj1, qcObj1.Get());
	ASSERT_EQ(id0, qcObj0->GetId());
	ASSERT_EQ(id1, qcObj1->GetId());
	swap(qcObj0, qcObj1);
	ASSERT_EQ(obj1, qcObj0.Get());
	ASSERT_EQ(obj0, qcObj1.Get());
	ASSERT_EQ(id1, qcObj0->GetId());
	ASSERT_EQ(id0, qcObj1->GetId());
	qcObj0.Swap(qcObj1);
	ASSERT_EQ(obj0, qcObj0.Get());
	ASSERT_EQ(obj1, qcObj1.Get());
	ASSERT_EQ(id0, qcObj0->GetId());
	ASSERT_EQ(id1, qcObj1->GetId());
}

TEST_F(ScopedPtr_Test, ConstTest)
{
}

TEST_F(ScopedPtr_Test, DeleterTest)
{
	ScopedPtr<Obj> qcObjFunDel(new Obj, &ObjDeleter);
	ScopedPtr<Obj[]> qcObjsFunDel(new Obj[ARRAY_SIZE], &ObjsDeleter);
}

TEST_F(ScopedPtr_Test, SemanticsTest)
{
	const ScopedPtr<Obj> qcObj(new Obj);
	ASSERT_FALSE(qcObj.IsNull());
	ASSERT_TRUE(qcObj);
	ASSERT_TRUE(0 != qcObj.Get());
	ASSERT_TRUE(0 != qcObj.GetDeleter< Deep::PtrDeleter<Obj> >());
	qcObj->GetId();

	Obj *pc = new Obj;
	const ScopedPtr<Obj> qcObjB(pc);
	//qcObj.Reset();					// expect: won't pass at compile time
	//qcObj.Swap(qcObjB);				// expect: won't pass at compile time
	//qcObj = pc;						// expect: won't pass at compile time

	ScopedPtr<Obj> qcObjC(new Obj), qcObjD(new Obj);
	qcObjC < qcObjD;
	// NOTE: this kind of pointer is not allowed copy and assigned copy for its type
	//qcObjC = pc;						// expect: won't pass at compile time
	//qcObjC = qcObjD;					// expect: won't pass at compile time
	//ScopedPtr<Obj> qcObjE = qcObjD;	// expect: won't pass at compile time
}
