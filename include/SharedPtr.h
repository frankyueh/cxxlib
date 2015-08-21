#pragma once

#include "cxxlib.h"

#include "deep/PtrDeep.h"
#include "deep/PtrSharedCount.h"

namespace cxxlib
{
	template < typename T > class SharedPtr;

	namespace Deep
	{
		template< typename T, typename U >
		inline PtrSharedCount * CreatePtrSharedCount(SharedPtr<T> *, U *pu)
		{ return new PtrSharedCount(pu, PtrDeleter<U>()); }

		template< typename T, typename U >
		inline PtrSharedCount * CreatePtrSharedCount(SharedPtr<T[]> *, U *pu)
		{ return new PtrSharedCount(pu, PtrDeleter<U[]>()); }
	}

	template < typename T >
	class SharedPtr : FINAL, SAFEBOOL
	{
		template < typename U > friend class SharedPtr;
		typedef SharedPtr<T> ThisType;
		typedef typename Deep::PtrElement<T>::Type EleType;

	public:
		static const ThisType NUL;

		SharedPtr() : m_pt(0), m_pcSharedCnt(0) {}

		template < typename U >
		explicit SharedPtr(U *pu) : m_pt(pu), m_pcSharedCnt(0)
		{ if (pu) m_pcSharedCnt = Deep::CreatePtrSharedCount(this, pu); }

		template < typename U, typename D >
		SharedPtr(U *pu, D const &fDel)
			: m_pt(pu)
			, m_pcSharedCnt(pu ? new Deep::PtrSharedCount(pu, fDel) : 0) {}

		SharedPtr(SharedPtr const &rhs)
			: m_pt(rhs.m_pt), m_pcSharedCnt(rhs.m_pcSharedCnt)
		{ if (m_pcSharedCnt) m_pcSharedCnt->AddRefCopy(); }

		template < typename U >
		SharedPtr(SharedPtr<U> const &rhs)
			: m_pt(rhs.m_pt), m_pcSharedCnt(rhs.m_pcSharedCnt)
		{ if (m_pcSharedCnt) m_pcSharedCnt->AddRefCopy(); }

		// for casting usage
		template < typename U >
		SharedPtr(SharedPtr<U> const &rhs, EleType *pt)
			: m_pt(pt), m_pcSharedCnt(rhs.m_pcSharedCnt)
		{ if (m_pcSharedCnt) m_pcSharedCnt->AddRefCopy(); }

		~SharedPtr()
		{ if (m_pcSharedCnt && m_pcSharedCnt->Release()) delete m_pcSharedCnt; }

		void Reset() { ThisType().Swap(*this); }

		template < typename U >
		void Reset(U *pu) { ThisType(pu).Swap(*this); }

		template < typename U, typename D >
		void Reset(U *pu, D const &fDel) { ThisType(pu, fDel).Swap(*this); }

		void Reset(SharedPtr const &rhs) { ThisType(rhs).Swap(*this); }

		template < typename U >
		void Reset(SharedPtr<U> const &rhs) { ThisType(rhs).Swap(*this); }

		SharedPtr & operator = (SharedPtr const &rhs)
		{ ThisType(rhs).Swap(*this); return *this; }

		bool IsNull() const	{ return 0 == m_pt; }
		size_t UseCount() const { return m_pcSharedCnt ? m_pcSharedCnt->UseCount() : 0; }
		bool IsUnique() const { return m_pcSharedCnt && (m_pcSharedCnt->UseCount() == 1); }
		EleType * Get() const { return m_pt; }
		typename Deep::PtrDereference<T>::Type operator * () const { return *m_pt; }
		typename Deep::PtrMemberAccess<T>::Type operator -> () const { return m_pt; }
		typename Deep::PtrArrayAccess<T>::Type operator [] (std::ptrdiff_t s) const { return *(m_pt + s); }

		template < typename D > D * GetDeleter() const
		{ return m_pcSharedCnt ? m_pcSharedCnt->GetDeleter<D>() : 0; }

		SharedPtr & Swap(SharedPtr &cThat)
		{
			swap(m_pt, cThat.m_pt);
			swap(m_pcSharedCnt, cThat.m_pcSharedCnt);
			return *this;
		}

	private:
		virtual bool TestBool() const { return 0 != m_pt; }

		EleType *m_pt;
		Deep::PtrSharedCount *m_pcSharedCnt;
	};

	template < typename T >
	const SharedPtr<T> SharedPtr<T>::NUL;

	template < typename T, typename U >
	inline bool operator == (SharedPtr<T> const &lhs, SharedPtr<U> const &rhs)
	{ return lhs.Get() == rhs.Get(); }

	template < typename T, typename U >
	inline bool operator != (SharedPtr<T> const &lhs, SharedPtr<U> const &rhs)
	{ return lhs.Get() != rhs.Get(); }

	template < typename T, typename U >
	inline bool operator < (SharedPtr<T> const &lhs, SharedPtr<U> const &rhs)
	{ return lhs.Get() < rhs.Get(); }

	template < typename T, typename U >
	inline bool operator <= (SharedPtr<T> const &lhs, SharedPtr<U> const &rhs)
	{ return lhs.Get() <= rhs.Get(); }

	template < typename T, typename U >
	inline bool operator > (SharedPtr<T> const &lhs, SharedPtr<U> const &rhs)
	{ return lhs.Get() > rhs.Get(); }

	template < typename T, typename U >
	inline bool operator >= (SharedPtr<T> const &lhs, SharedPtr<U> const &rhs)
	{ return lhs.Get() >= rhs.Get(); }

	template < typename T >
	inline void swap(SharedPtr<T> &lhs, SharedPtr<T> &rhs)
	{ lhs.Swap(rhs); }

	template < typename T, typename U >
	SharedPtr<T> static_ptr_cast(SharedPtr<U> const &ptr)
	{ return SharedPtr<T>(ptr, static_cast<T *>(ptr.Get())); }

	template < typename T, typename U >
	SharedPtr<T> const_ptr_cast(SharedPtr<U> const &ptr)
	{ return SharedPtr<T>(ptr, const_cast<T *>(ptr.Get())); }

	template < typename T, typename U >
	SharedPtr<T> reinterpret_ptr_cast(SharedPtr<U> const &ptr)
	{ return SharedPtr<T>(ptr, reinterpret_cast<T *>(ptr.Get())); }

	template < typename T, typename U >
	SharedPtr<T> dynamic_ptr_cast(SharedPtr<U> const &ptr)
	{
		T *puCasted = dynamic_cast<T *>(ptr.Get());
		return puCasted ? SharedPtr<T>(ptr, puCasted) : SharedPtr<T>::NUL;
	}

	template < typename T, typename U >
	SharedPtr<T> polymorphic_ptr_downcast(SharedPtr<U> const &ptr)
	{
		CXXLIB_ASSERT(ptr.Get() == dynamic_cast<T *>(ptr.Get()));
		return SharedPtr<T>(ptr, static_cast<T *>(ptr.Get()));
	}
	
}
