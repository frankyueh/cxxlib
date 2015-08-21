
#include "test_SharedPtr.h"
#include "Thread.h"

#include "gtest/gtest.h"
#include "gtest_utilities.h"

using namespace cxxlib;
using namespace SharedPtrTest;

class SharedPtr_Test : public ::testing::Test
{
public:
	static const int ARRAY_SIZE = 500;
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

protected:

	virtual void SetUp()	{ s_iObjCount = 0; }
	virtual void TearDown()	{ ASSERT_EQ(0, s_iObjCount); }
};

int SharedPtr_Test::s_iObjCount = 0;

namespace SharedPtrTest
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

TEST_F(SharedPtr_Test, ForwardDeclaredTest)
{
	TestForwardDeclared cTestForwardDeclaredObj(&s_iObjCount);
}

TEST_F(SharedPtr_Test, NullTest)
{
	SharedPtr<Obj> qcNullA;
	ASSERT_FALSE(qcNullA);
	ASSERT_TRUE(!qcNullA);
	ASSERT_FALSE(qcNullA.IsUnique());
	ASSERT_TRUE(qcNullA.IsNull());
	ASSERT_EQ(0, qcNullA.UseCount());
	ASSERT_EQ(0, qcNullA.Get());
	ASSERT_TRUE(qcNullA == SharedPtr<Obj>::NUL);
	ASSERT_TRUE(qcNullA == 0);
	ASSERT_TRUE(0 == qcNullA);

	SharedPtr<Obj> qcNullB = qcNullA;
	ASSERT_TRUE(qcNullB.IsNull());
	ASSERT_TRUE(qcNullA == qcNullB);

	Obj *p = new Obj(123);
	SharedPtr<Obj> qcObj(p);
	ASSERT_TRUE(qcObj);
	ASSERT_FALSE(qcObj.IsNull());
	ASSERT_EQ(1, qcObj.UseCount());
	ASSERT_TRUE(qcObj.IsUnique());
	ASSERT_EQ(p, qcObj.Get());
	ASSERT_TRUE(qcObj != SharedPtr<Obj>::NUL);
	ASSERT_TRUE(qcObj != 0);
	ASSERT_TRUE(0 != qcObj);
	ASSERT_EQ(123, qcObj->GetId());
	ASSERT_EQ(123, (*qcObj).GetId());
	ASSERT_EQ(123, (*qcObj.Get()).GetId());

	qcObj.Reset();
	ASSERT_TRUE(qcObj.IsNull());

	qcObj.Reset(new Obj);
	ASSERT_FALSE(qcObj.IsNull());

	qcObj = qcNullA;
	ASSERT_TRUE(qcObj.IsNull());
}

TEST_F(SharedPtr_Test, GeneralTest)
{
	SharedPtr<Obj> qcObjA(new Obj);
	ASSERT_EQ(1, qcObjA.UseCount());

	SharedPtr<Obj> qcObjB = qcObjA;
	ASSERT_EQ(2, qcObjA.UseCount());
	ASSERT_EQ(2, qcObjB.UseCount());
	ASSERT_EQ(qcObjA.Get(), qcObjB.Get());
	ASSERT_TRUE(qcObjA == qcObjB);
	ASSERT_FALSE(qcObjA != qcObjB);

	SharedPtr<Obj> qcObjC(new Obj);
	qcObjC = qcObjB;
	ASSERT_EQ(3, qcObjA.UseCount());
	ASSERT_EQ(3, qcObjB.UseCount());
	ASSERT_EQ(3, qcObjC.UseCount());
	ASSERT_EQ(qcObjA.Get(), qcObjC.Get());
	ASSERT_TRUE(qcObjA == qcObjC);
	ASSERT_FALSE(qcObjA != qcObjC);

	SharedPtr<Obj> qcObjD(new Obj);
	qcObjD.Reset(qcObjA);
	ASSERT_EQ(4, qcObjA.UseCount());
	ASSERT_EQ(4, qcObjB.UseCount());
	ASSERT_EQ(4, qcObjC.UseCount());
	ASSERT_EQ(4, qcObjD.UseCount());
	ASSERT_EQ(qcObjC.Get(), qcObjD.Get());
	ASSERT_TRUE(qcObjC == qcObjD);
	ASSERT_FALSE(qcObjC != qcObjD);

	qcObjA = qcObjA;
	qcObjB = qcObjB;
	qcObjC = qcObjC;
	qcObjD = qcObjD;
	ASSERT_EQ(4, qcObjA.UseCount());
	ASSERT_EQ(4, qcObjB.UseCount());
	ASSERT_EQ(4, qcObjC.UseCount());
	ASSERT_EQ(4, qcObjD.UseCount());

	qcObjA = qcObjB;
	qcObjB = qcObjC;
	qcObjC = qcObjD;
	qcObjD = qcObjA;
	ASSERT_EQ(4, qcObjA.UseCount());
	ASSERT_EQ(4, qcObjB.UseCount());
	ASSERT_EQ(4, qcObjC.UseCount());
	ASSERT_EQ(4, qcObjD.UseCount());

	Obj *pcObj = new Obj;
	SharedPtr<void> qcVoidObj(pcObj);
	ASSERT_TRUE(qcVoidObj);
	ASSERT_TRUE(pcObj == qcVoidObj.Get());
}

TEST_F(SharedPtr_Test, ArrayTest)
{
	SharedPtr<Obj> qcObjs(new Obj[ARRAY_SIZE], Deep::PtrDeleter<Obj[]>());
	SharedPtr<Obj> qcObj(new Obj);
	qcObj = qcObjs;
	qcObjs.Reset();
	qcObj.Reset();

	SharedPtr<Obj[]> qacObj(new Obj[ARRAY_SIZE]);
	int iLast = -1;
	for (size_t s = 0 ; s < ARRAY_SIZE ; ++s)
	{
		if (iLast >= 0)
			ASSERT_EQ(iLast + 1, qacObj[s].GetId());
		iLast = qacObj[s].GetId();
	}
	ASSERT_TRUE(qacObj != SharedPtr<Obj[]>::NUL);

	qacObj.Reset(new DerivedObj[ARRAY_SIZE]);
	iLast = -1;
	for (size_t s = 0 ; s < ARRAY_SIZE ; ++s)
	{
		if (iLast >= 0)
			ASSERT_EQ(iLast + 2, qacObj[s].GetId());
		iLast = qacObj[s].GetId();
	}
}

TEST_F(SharedPtr_Test, SwapTest)
{
	SharedPtr<Obj> qcObj1(new Obj(1));
	SharedPtr<Obj> qcObj2(new Obj(2));
	ASSERT_EQ(1, qcObj1->GetId());
	ASSERT_EQ(2, qcObj2->GetId());
	swap(qcObj1, qcObj2);
	ASSERT_EQ(2, qcObj1->GetId());
	ASSERT_EQ(1, qcObj2->GetId());
	qcObj1.Swap(qcObj2);
	ASSERT_EQ(1, qcObj1->GetId());
	ASSERT_EQ(2, qcObj2->GetId());
	qcObj1.Swap(qcObj2).Swap(qcObj2);
	ASSERT_EQ(1, qcObj1->GetId());
	ASSERT_EQ(2, qcObj2->GetId());

	const int ARRAY_SIZE = 100;
	SharedPtr<Obj> qcObjs(new Obj[ARRAY_SIZE], Deep::PtrDeleter<Obj[]>());
	SharedPtr<Obj> qcObj(new Obj);
	qcObjs.Swap(qcObj);
}

TEST_F(SharedPtr_Test, ConstTest)
{
	const SharedPtr<Obj> qcObjCst(new Obj);
	ASSERT_TRUE(qcObjCst);
	ASSERT_TRUE(qcObjCst != 0);
	ASSERT_TRUE(0 != qcObjCst);
	ASSERT_FALSE(qcObjCst == 0);
	ASSERT_FALSE(0 == qcObjCst);
	ASSERT_FALSE(qcObjCst == SharedPtr<Obj>::NUL);
	ASSERT_FALSE(SharedPtr<Obj>::NUL == qcObjCst);
	qcObjCst.IsNull();
	qcObjCst.IsUnique();
	qcObjCst.UseCount();
	qcObjCst.Get();

	SharedPtr<Obj> qcObjA = qcObjCst;
	SharedPtr<Obj> qcObjB, qcObjC;
	qcObjB.Reset(qcObjCst);
	qcObjC = qcObjCst;

	//qcObjCst = qcObjA;			// expect: won't pass at compile time

	SharedPtr<const Obj> qcCstObjA(new Obj);
	SharedPtr<const Obj> qcCstObjB(new Obj);
	qcCstObjA->GetId();
	qcCstObjA = qcCstObjB;

	const SharedPtr<const Obj> qcCstObjCstA(new Obj), qcCstObjCstB;
	//qcCstObjCstB = qcCstObjCstA;	// expect: won't pass at compile time
	//qcCstObjCst = qcCstObjA;		// expect: won't pass at compile time
}

TEST_F(SharedPtr_Test, DeleterTest)
{
	SharedPtr<Obj> qcObj;
	ASSERT_EQ(0, qcObj.GetDeleter< Deep::PtrDeleter<Obj> >());
	qcObj.Reset(new Obj);
	ASSERT_TRUE(0 != qcObj.GetDeleter< Deep::PtrDeleter<Obj> >());

	SharedPtr<Obj> qcObjs(new Obj[ARRAY_SIZE], Deep::PtrDeleter<Obj[]>());
	ASSERT_EQ(0, qcObjs.GetDeleter< Deep::PtrDeleter<Obj> >());
	ASSERT_TRUE(0 != qcObjs.GetDeleter< Deep::PtrDeleter<Obj[]> >());
	(*qcObjs.GetDeleter< Deep::PtrDeleter<Obj[]> >())(new Obj[ARRAY_SIZE]);

	swap(qcObj, qcObjs);
	ASSERT_TRUE(0 != qcObj.GetDeleter< Deep::PtrDeleter<Obj[]> >());
	ASSERT_TRUE(0 != qcObjs.GetDeleter< Deep::PtrDeleter<Obj> >());

	SharedPtr<Obj> qcObjVDel(qcObjs.Get(), Deep::PtrDeleter<void>());
	ASSERT_EQ(0, qcObjVDel.GetDeleter< Deep::PtrDeleter<Obj> >());
	ASSERT_TRUE(0 != qcObjVDel.GetDeleter< Deep::PtrDeleter<void> >());

	SharedPtr<Obj> qcObjFunDel(new Obj, &ObjDeleter);
	ASSERT_TRUE(&ObjDeleter == *qcObjFunDel.GetDeleter< void (*)(Obj *) >());
	(*qcObjFunDel.GetDeleter< void (*)(Obj *) >())(new Obj);
}

TEST_F(SharedPtr_Test, ContainerTest)
{
	std::set< SharedPtr<Obj> > cObjs;
	for (int i = 0 ; i < ARRAY_SIZE ; ++i)
		ASSERT_TRUE(cObjs.insert(SharedPtr<Obj>(new Obj)).second);
}

class ImplicitA { public: virtual ~ImplicitA () {} int A; };
class ImplicitB : public ImplicitA { public: int B; };

TEST_F(SharedPtr_Test, CastTest)
{
	//SharedPtr<Obj> qcObjA = new Obj;					// expect: won't pass at compile time

	SharedPtr<ImplicitA> cpObjP;
	SharedPtr<ImplicitB> cpObjQ(new ImplicitB);
	cpObjP = cpObjQ;
	//cpObjQ = cpObjP;									// expect: won't pass at compile time
	cpObjQ = static_ptr_cast<ImplicitB>(cpObjP);
	SharedPtr<const ImplicitA> cpObjR;
	//cpObjP = static_ptr_cast<ImplicitA>(cpObjR);		// expect: won't pass at compile time
	cpObjP = const_ptr_cast<ImplicitA>(cpObjR);
	SharedPtr<const ImplicitB> cpObjS;
	cpObjP = const_ptr_cast<ImplicitA>(static_ptr_cast<const ImplicitA>(cpObjS));
	SharedPtr<ImplicitA> cpObjT(new ImplicitA);
	cpObjQ = dynamic_ptr_cast<ImplicitB>(cpObjT);
	ASSERT_TRUE(cpObjQ.IsNull());
	//cpObjP = reinterpret_ptr_cast<ImplicitA>(cpObjR);	// expect: won't pass at compile time
	SharedPtr<int> cpObjU;
	cpObjQ = reinterpret_ptr_cast<ImplicitB>(cpObjU);
}

class BaseA
{
public:
	virtual ~BaseA() {}
};

class BaseB
{
public:
	virtual ~BaseB() {}
};

class BaseC
{
public:
	virtual ~BaseC() {}
};

class Derived : public BaseA, public BaseB, public BaseC
{
public:
	~Derived() {}
};

TEST_F(SharedPtr_Test, MultipleInheritanceTest)
{
	BaseA *pcRawBaseA = new BaseA;
	BaseB *pcRawBaseB = new BaseB;
	BaseC *pcRawBaseC = new BaseC;
	SharedPtr<BaseA> cRawBaseA(pcRawBaseA);
	SharedPtr<BaseB> cRawBaseB(pcRawBaseB);
	SharedPtr<BaseC> cRawBaseC(pcRawBaseC);
	ASSERT_EQ(pcRawBaseA, cRawBaseA.Get());
	ASSERT_EQ(pcRawBaseB, cRawBaseB.Get());
	ASSERT_EQ(pcRawBaseC, cRawBaseC.Get());

	int iUseCount = 1;
	Derived *pcDerived = new Derived;
	SharedPtr<Derived> cDerived(pcDerived);
	ASSERT_EQ(pcDerived, cDerived.Get());
	ASSERT_EQ(iUseCount, cDerived.UseCount());

	// implicit conversion

	++iUseCount;
	BaseA *pcBaseA = pcDerived;
	SharedPtr<BaseA> cBaseA = cDerived;
	ASSERT_EQ(pcBaseA, cBaseA.Get());
	ASSERT_EQ(pcBaseA == pcDerived, cBaseA == cDerived);
	ASSERT_EQ(iUseCount, cDerived.UseCount());

	++iUseCount;
	BaseB *pcBaseB = pcDerived;
	SharedPtr<BaseB> cBaseB = cDerived;
	ASSERT_EQ(pcBaseB, cBaseB.Get());
	ASSERT_EQ(pcBaseB == pcDerived, cBaseB == cDerived);
	ASSERT_EQ(iUseCount, cDerived.UseCount());

	++iUseCount;
	BaseC *pcBaseC = pcDerived;
	SharedPtr<BaseC> cBaseC = cDerived;
	ASSERT_EQ(pcBaseC, cBaseC.Get());
	ASSERT_EQ(pcBaseC == pcDerived, cBaseC == cDerived);
	ASSERT_EQ(iUseCount, cDerived.UseCount());

	// polymorphic downcast conversion

	iUseCount += 3;
	Derived *pcDerivedFromBaseAs = static_cast<Derived*>(pcBaseA);
	Derived *pcDerivedFromBaseAd = dynamic_cast<Derived*>(pcBaseA);
	Derived *pcDerivedFromBaseAp = polymorphic_downcast<Derived*>(pcBaseA);
	SharedPtr<Derived> cDerivedFromBaseAs = static_ptr_cast<Derived>(cBaseA);
	SharedPtr<Derived> cDerivedFromBaseAd = dynamic_ptr_cast<Derived>(cBaseA);
	SharedPtr<Derived> cDerivedFromBaseAp = polymorphic_ptr_downcast<Derived>(cBaseA);
	ASSERT_TRUE(dynamic_ptr_cast<Derived>(cRawBaseA).IsNull());
	ASSERT_EQ(pcDerivedFromBaseAs, cDerivedFromBaseAs.Get());
	ASSERT_EQ(pcDerivedFromBaseAd, cDerivedFromBaseAd.Get());
	ASSERT_EQ(pcDerivedFromBaseAp, cDerivedFromBaseAp.Get());
	ASSERT_EQ(iUseCount, cDerived.UseCount());

	iUseCount += 3;
	Derived *pcDerivedFromBaseBs = static_cast<Derived*>(pcBaseB);
	Derived *pcDerivedFromBaseBd = dynamic_cast<Derived*>(pcBaseB);
	Derived *pcDerivedFromBaseBp = polymorphic_downcast<Derived*>(pcBaseB);
	SharedPtr<Derived> cDerivedFromBaseBs = static_ptr_cast<Derived>(cBaseB);
	SharedPtr<Derived> cDerivedFromBaseBd = dynamic_ptr_cast<Derived>(cBaseB);
	SharedPtr<Derived> cDerivedFromBaseBp = polymorphic_ptr_downcast<Derived>(cBaseB);
	ASSERT_TRUE(dynamic_ptr_cast<Derived>(cRawBaseB).IsNull());
	ASSERT_EQ(pcDerivedFromBaseBs, cDerivedFromBaseBs.Get());
	ASSERT_EQ(pcDerivedFromBaseBd, cDerivedFromBaseBd.Get());
	ASSERT_EQ(pcDerivedFromBaseBp, cDerivedFromBaseBp.Get());
	ASSERT_EQ(iUseCount, cDerived.UseCount());

	iUseCount += 3;
	Derived *pcDerivedFromBaseCs = static_cast<Derived*>(pcBaseC);
	Derived *pcDerivedFromBaseCd = dynamic_cast<Derived*>(pcBaseC);
	Derived *pcDerivedFromBaseCp = polymorphic_downcast<Derived*>(pcBaseC);
	SharedPtr<Derived> cDerivedFromBaseCs = static_ptr_cast<Derived>(cBaseC);
	SharedPtr<Derived> cDerivedFromBaseCd = dynamic_ptr_cast<Derived>(cBaseC);
	SharedPtr<Derived> cDerivedFromBaseCp = polymorphic_ptr_downcast<Derived>(cBaseC);
	ASSERT_TRUE(dynamic_ptr_cast<Derived>(cRawBaseC).IsNull());
	ASSERT_EQ(pcDerivedFromBaseCs, cDerivedFromBaseCs.Get());
	ASSERT_EQ(pcDerivedFromBaseCd, cDerivedFromBaseCd.Get());
	ASSERT_EQ(pcDerivedFromBaseCp, cDerivedFromBaseCp.Get());
	ASSERT_EQ(iUseCount, cDerived.UseCount());

	// dynamic cast

	iUseCount += 2;
	BaseA *pcBaseAFromB = dynamic_cast<BaseA *>(pcBaseB);
	BaseA *pcBaseAFromC = dynamic_cast<BaseA *>(pcBaseC);
	SharedPtr<BaseA> cBaseAFromB = dynamic_ptr_cast<BaseA>(cBaseB);
	SharedPtr<BaseA> cBaseAFromC = dynamic_ptr_cast<BaseA>(cBaseC);
	ASSERT_EQ(pcBaseAFromB, cBaseAFromB.Get());
	ASSERT_EQ(pcBaseAFromC, cBaseAFromC.Get());
	ASSERT_EQ(iUseCount, cDerived.UseCount());

	iUseCount += 2;
	BaseB *pcBaseBFromA = dynamic_cast<BaseB *>(pcBaseA);
	BaseB *pcBaseBFromC = dynamic_cast<BaseB *>(pcBaseC);
	SharedPtr<BaseB> cBaseBFromA = dynamic_ptr_cast<BaseB>(cBaseA);
	SharedPtr<BaseB> cBaseBFromC = dynamic_ptr_cast<BaseB>(cBaseC);
	ASSERT_EQ(pcBaseBFromA, cBaseBFromA.Get());
	ASSERT_EQ(pcBaseBFromC, cBaseBFromC.Get());
	ASSERT_EQ(iUseCount, cDerived.UseCount());

	iUseCount += 2;
	BaseC *pcBaseCFromA = dynamic_cast<BaseC *>(pcBaseA);
	BaseC *pcBaseCFromB = dynamic_cast<BaseC *>(pcBaseB);
	SharedPtr<BaseC> cBaseCFromA = dynamic_ptr_cast<BaseC>(cBaseA);
	SharedPtr<BaseC> cBaseCFromB = dynamic_ptr_cast<BaseC>(cBaseB);
	ASSERT_EQ(pcBaseCFromA, cBaseCFromA.Get());
	ASSERT_EQ(pcBaseCFromB, cBaseCFromB.Get());
	ASSERT_EQ(iUseCount, cDerived.UseCount());
}

class DBaseA
{
public:
	DBaseA(int *piDCount) : m_piDCount(piDCount) {}
	virtual ~DBaseA() { ++*m_piDCount; }
	virtual void DynammicDummy() = 0;
private:
	int *m_piDCount;
};

class DBaseANonDV
{
public:
	DBaseANonDV() {}
private:
	int m_iDummyValue;
};

class DBaseB
{
public:
	DBaseB(int *piDCount) : m_piDCount(piDCount) {}
	virtual ~DBaseB() { ++*m_piDCount; }
	virtual void DynammicDummy() = 0;
private:
	int *m_piDCount;
};

class DBaseBNonDV
{
public:
	DBaseBNonDV() {}
private:
	int m_iDummyValue;
};

class DBaseC
{
public:
	DBaseC(int *piDCount) : m_piDCount(piDCount) {}
	virtual ~DBaseC() { ++*m_piDCount; }
	virtual void DynammicDummy() = 0;
private:
	int *m_piDCount;
};

class DBaseCNonDV
{
public:
	DBaseCNonDV() {}
private:
	int m_iDummyValue;
};

class DDerived : public DBaseA, public DBaseB, public DBaseC
{
public:
	DDerived(int *piDCountA, int *piDCountB, int *piDCountC, int *piDCount)
		: DBaseA(piDCountA), DBaseB(piDCountB), DBaseC(piDCountC), m_piDCount(piDCount) {}
	~DDerived() { ++*m_piDCount; }
	void DynammicDummy() {}
private:
	int *m_piDCount;
};

class DDerivedNonDV : public DBaseANonDV, public DBaseBNonDV, public DBaseCNonDV
{
public:
	DDerivedNonDV(int *piDCount) : m_piDCount(piDCount) {}
	~DDerivedNonDV() { ++*m_piDCount; }
private:
	int *m_piDCount;
};

TEST_F(SharedPtr_Test, MultipleInheritanceDeleteTest)
{
	int iDCountA = 0;
	int iDCountB = 0;
	int iDCountC = 0;
	int iDCount = 0;

	SharedPtr<DBaseA> cDBaseAOfDerived(new DDerived(&iDCountA, &iDCountB, &iDCountC, &iDCount));
	SharedPtr<DBaseB> cDBaseBOfDerived(new DDerived(&iDCountA, &iDCountB, &iDCountC, &iDCount));
	SharedPtr<DBaseC> cDBaseCOfDerived(new DDerived(&iDCountA, &iDCountB, &iDCountC, &iDCount));
	SharedPtr<DDerived> cDDerived(new DDerived(&iDCountA, &iDCountB, &iDCountC, &iDCount));

	cDBaseAOfDerived.Reset();
	cDBaseBOfDerived.Reset();
	cDBaseCOfDerived.Reset();
	cDDerived.Reset();

	ASSERT_EQ(4, iDCountA);
	ASSERT_EQ(4, iDCountB);
	ASSERT_EQ(4, iDCountC);
	ASSERT_EQ(4, iDCount);

	int iDCountNonDV = 0;

	SharedPtr<DBaseANonDV> cDBaseANonDVOfDerivedNonDV(new DDerivedNonDV(&iDCountNonDV), Deep::PtrDeleter<DDerivedNonDV>());
	SharedPtr<DBaseBNonDV> cDBaseBNonDVOfDerivedNonDV(new DDerivedNonDV(&iDCountNonDV), Deep::PtrDeleter<DDerivedNonDV>());
	SharedPtr<DBaseCNonDV> cDBaseCNonDVOfDerivedNonDV(new DDerivedNonDV(&iDCountNonDV), Deep::PtrDeleter<DDerivedNonDV>());
	SharedPtr<DDerivedNonDV> cDerivedNonDV(new DDerivedNonDV(&iDCountNonDV));

	cDBaseANonDVOfDerivedNonDV.Reset();
	cDBaseBNonDVOfDerivedNonDV.Reset();
	cDBaseCNonDVOfDerivedNonDV.Reset();
	cDerivedNonDV.Reset();

	ASSERT_EQ(4, iDCountNonDV);
}

struct LotOfObjs
{
	typedef SharedPtr_Test::Obj Obj;

	static const unsigned int SIZE		= 500;	// >= VAR, number of array size
	static const unsigned int VAR		= 50;	// >= 2, how many different object

	LotOfObjs() {}
	LotOfObjs(bool)
	{
		SharedPtr<Obj> aqOrgs[VAR];
		for (unsigned int n = 0 ; n < VAR ; ++n)
			aqOrgs[n].Reset(new Obj);
		for (unsigned int n = 0 ; n < SIZE ; ++n)
			aqObjs[n] = aqOrgs[n % VAR];
	}
	LotOfObjs & operator = (LotOfObjs const &rhs)
	{
		for (unsigned int n = 0 ; n < SIZE ; ++n)
			aqObjs[n] = rhs.aqObjs[n];
		return *this;
	}
	SharedPtr<Obj> aqObjs[SIZE];
};

class RandomAssign : public Runnable
{
public:
	static const unsigned int RUNS		= 100;	// >= 2, number or assing times for each thread's run
	static const unsigned int NUMBER	= 10;	// >= 2, number of testing thread

	LotOfObjs cLotOfObjs;

	void operator() ()
	{
		for (unsigned int r = 0 ; r < RUNS ; ++r)
		{
			cLotOfObjs.aqObjs[gtest_utility::rand_i16() % LotOfObjs::SIZE].Reset(
				cLotOfObjs.aqObjs[gtest_utility::rand_i16() % LotOfObjs::SIZE]);
			cLotOfObjs.aqObjs[gtest_utility::rand_i16() % LotOfObjs::SIZE] = 
				cLotOfObjs.aqObjs[gtest_utility::rand_i16() % LotOfObjs::SIZE];
			gtest_utility::thread_yield();
		}
	}
};

TEST_F(SharedPtr_Test, ThreadedTest)
{
	gtest_utility::rand_init();

	LotOfObjs cLotOfObjs(true);

	Thread acThreads[RandomAssign::NUMBER];
	RandomAssign acRandomAssign[RandomAssign::NUMBER];

	for (unsigned int n = 0 ; n < RandomAssign::NUMBER ; ++n)
	{
		acRandomAssign[n].cLotOfObjs = cLotOfObjs;
		Thread cThread(acRandomAssign[n]);
		acThreads[n].Swap(cThread);
		ASSERT_TRUE(acThreads[n].Joinable());
	}

	GUTIL_FOREACH_ASSERT_LIMIT(
		n, RandomAssign::NUMBER, GUTIL_MAX_WAITING_MS,
		acThreads[n].Join(GUTIL_AVG_MEASURE_MS));
}
