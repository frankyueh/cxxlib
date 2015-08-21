#pragma once

#include "cxxlib.h"

#include "deep/PtrDeep.h"
#include "deep/FunctorWrap.h"

namespace cxxlib
{
	template < typename T > class ScopedPtr;

	namespace Deep
	{
		template< typename T, typename U >
		inline FunctorWrapBase<> * CreatePtrFunctorWrap(ScopedPtr<T> *, U *pu)
		{ return new FunctorWrap<U*, PtrDeleter<U> >(); }

		template< typename T, typename U >
		inline FunctorWrapBase<> * CreatePtrFunctorWrap(ScopedPtr<T[]> *, U *pu)
		{ return new FunctorWrap<U*, PtrDeleter<U[]> >(); }
	}

	template < typename T >
	class ScopedPtr : NOCOPY, FINAL, SAFEBOOL
	{
		typedef ScopedPtr<T> ThisType;
		typedef typename Deep::PtrElement<T>::Type EleType;

	public:
		static const ThisType NUL;

		ScopedPtr() : m_pt(0), m_pcDelWrap(0) {}

		template < typename U >
		explicit ScopedPtr(U *pu) : m_pt(pu), m_pcDelWrap(0)
		{ if (pu) m_pcDelWrap = Deep::CreatePtrFunctorWrap(this, pu); }

		template < typename U, typename D >
		ScopedPtr(U *pu, D fDel)
			: m_pt(pu)
			, m_pcDelWrap(pu ? new Deep::FunctorWrap<U*, D>(fDel) : 0) {}

		~ScopedPtr()
		{ if (m_pt) (*m_pcDelWrap)(&m_pt); delete m_pcDelWrap; }

		void Reset()
		{ ThisType().Swap(*this); }

		template < typename U >
		void Reset(U *pu)
		{ ThisType(pu).Swap(*this); }

		template < typename U, typename D >
		void Reset(U *pu, D const &fDel)
		{ ThisType(pu, fDel).Swap(*this); }

		bool IsNull() const { return 0 == m_pt; }
		EleType * Get() const { return m_pt; }
		typename Deep::PtrDereference<T>::Type operator * () const { return *m_pt; }
		typename Deep::PtrMemberAccess<T>::Type operator -> () const { return m_pt; }
		typename Deep::PtrArrayAccess<T>::Type operator [] (std::ptrdiff_t s) const { return *(m_pt + s); }

		template < typename D > D * GetDeleter() const
		{ return m_pcDelWrap ? reinterpret_cast<D *>(m_pcDelWrap->GetFunctor(Deep::TypeInfo<D>::V_ID)) : 0; }

		ScopedPtr & Swap(ScopedPtr &cThat)
		{
			swap(m_pt, cThat.m_pt);
			swap(m_pcDelWrap, cThat.m_pcDelWrap);
			return *this;
		}

	private:
		virtual bool TestBool() const { return 0 != m_pt; }

		EleType *m_pt;
		Deep::FunctorWrapBase<> *m_pcDelWrap;
	};

	template < typename T >
	const ScopedPtr<T> ScopedPtr<T>::NUL;

	template < typename T, typename U >
	inline bool operator == (ScopedPtr<T> const &lhs, ScopedPtr<U> const &rhs)
	{ return lhs.Get() == rhs.Get(); }

	template < typename T, typename U >
	inline bool operator != (ScopedPtr<T> const &lhs, ScopedPtr<U> const &rhs)
	{ return lhs.Get() != rhs.Get(); }

	template < typename T, typename U >
	inline bool operator < (ScopedPtr<T> const &lhs, ScopedPtr<U> const &rhs)
	{ return lhs.Get() < rhs.Get(); }

	template < typename T, typename U >
	inline bool operator <= (ScopedPtr<T> const &lhs, ScopedPtr<U> const &rhs)
	{ return lhs.Get() <= rhs.Get(); }

	template < typename T, typename U >
	inline bool operator > (ScopedPtr<T> const &lhs, ScopedPtr<U> const &rhs)
	{ return lhs.Get() > rhs.Get(); }

	template < typename T, typename U >
	inline bool operator >= (ScopedPtr<T> const &lhs, ScopedPtr<U> const &rhs)
	{ return lhs.Get() >= rhs.Get(); }

	template < typename T >
	inline void swap(ScopedPtr<T> &lhs, ScopedPtr<T> &rhs)
	{ lhs.Swap(rhs); }

}
