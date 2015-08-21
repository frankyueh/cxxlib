#pragma once

#include "cxxlib.h"

#include "deep/TypeInfo.h"
#include "deep/FunctorWrap.h"

namespace cxxlib { namespace Deep {

	class CXXLIB_EXPORT PtrSharedCount : NOCOPY
	{
		DECLARE_IMPL_P
		void InitImpl();

	public:
		template < typename T, typename D >
		PtrSharedCount(T *pt, D fDel)
			: IMPL_P_VAR(0)
			, m_p(pt)
			, m_pcDelWrap(new Deep::FunctorWrap<T*, D>(fDel)) { InitImpl(); }

		template < typename D > D * GetDeleter() const
		{ return reinterpret_cast<D *>(m_pcDelWrap->GetFunctor(Deep::TypeInfo<D>::V_ID)); }

		~PtrSharedCount();
		void AddRefCopy();
		bool Release();
		size_t UseCount() const;
	private:
		void *m_p;
		Deep::FunctorWrapBase<> *m_pcDelWrap;
	};

} }
