
#include "deep/PtrSharedCount.h"

#include "Mutex.h"

using namespace cxxlib;
using namespace cxxlib::Deep;

class PtrSharedCount::Impl : FINAL, NOCOPY
{
public:
	Impl() : m_sCount(1), m_stMutex(Mutex::NONRECURSIVE) {}
	~Impl() {}
	size_t m_sCount;
	Mutex m_stMutex;
};

void PtrSharedCount::InitImpl()
{
	IMPL_P_VAR = new Impl;
}

PtrSharedCount::~PtrSharedCount()
{
	if (m_p) (*m_pcDelWrap)(&m_p);
	delete m_pcDelWrap;
	delete IMPL_P_VAR;
}

void PtrSharedCount::AddRefCopy()
{
	IMPL_P(PtrSharedCount);
	Guarded cGuarded(p->m_stMutex);
	++(p->m_sCount);
}

bool PtrSharedCount::Release()
{
	IMPL_P(PtrSharedCount);
	Guarded cGuarded(p->m_stMutex);
	return --(p->m_sCount) == 0;
}

size_t PtrSharedCount::UseCount() const
{
	IMPL_P(const PtrSharedCount);
	return static_cast<size_t const volatile &>(p->m_sCount);
}
