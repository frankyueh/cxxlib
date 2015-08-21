#pragma once

#include "cxxlib.h"

#include "deep/FunctorWrap.h"

namespace cxxlib
{
	template < typename T >
	class ScopedTyp : NOCOPY, FINAL, SAFEBOOL
	{
		typedef ScopedTyp<T> ThisType;

		template < typename TT >
		struct DefIsNul
		{ bool operator ()(T *pt) { return 0 == pt; } };

		template < typename U >
		ScopedTyp(U u, Deep::FunctorWrapBase<> *pcDelWrap , Deep::FunctorWrapBase<bool> *pcIsNulWrap)
			: m_t(u)
			, m_pcDelWrap(pcDelWrap)
			, m_pcIsNulWrap(pcIsNulWrap) {}

		template < typename U, typename D >
		ScopedTyp(U u, D fDel, Deep::FunctorWrapBase<bool> *pcIsNulWrap)
			: m_t(u)
			, m_pcDelWrap(new Deep::FunctorWrap<T, D>(fDel))
			, m_pcIsNulWrap(pcIsNulWrap) {}

	public:

		template < typename U, typename D >
		ScopedTyp(U u, D fDel)
			: m_t(u)
			, m_pcDelWrap(new Deep::FunctorWrap<T, D>(fDel))
			, m_pcIsNulWrap(new Deep::FunctorWrap<T, DefIsNul<T>, bool>()) {}

		template < typename U, typename D, typename N >
		ScopedTyp(U u, D fDel, N fIsNul)
			: m_t(u)
			, m_pcDelWrap(new Deep::FunctorWrap<T, D>(fDel))
			, m_pcIsNulWrap(new Deep::FunctorWrap<T, N, bool>(fIsNul)) {}

		~ScopedTyp()
		{
			if (!(*m_pcIsNulWrap)(&m_t)) (*m_pcDelWrap)(&m_t);
			delete m_pcDelWrap;
			delete m_pcIsNulWrap;
		}

		template < typename U >
		void Reset(U u)
		{ ThisType(u, m_pcDelWrap->Clone(), m_pcIsNulWrap->Clone()).Swap(*this); }

		template < typename U, typename D >
		void Reset(U u, D fDel)
		{ ThisType(u, fDel, m_pcIsNulWrap->Clone()).Swap(*this); }

		template < typename U, typename D, typename N >
		void Reset(T u, D fDel, N fIsNul)
		{ ThisType(u, fDel, fIsNul).Swap(*this); }

		bool IsNull() const { return (*m_pcIsNulWrap)(const_cast<T *>(&m_t)); }
		T Get() const { return m_t; }

		void Swap(ThisType &cThat)
		{
			swap(m_t, cThat.m_t);
			swap(m_pcDelWrap, cThat.m_pcDelWrap);
			swap(m_pcIsNulWrap, cThat.m_pcIsNulWrap);
		}

	private:
		virtual bool TestBool() const { return !(*m_pcIsNulWrap)(const_cast<T *>(&m_t)); }

		T m_t;
		Deep::FunctorWrapBase<> *m_pcDelWrap;
		Deep::FunctorWrapBase<bool> *m_pcIsNulWrap;
	};

	template < typename T >
	inline bool operator == (ScopedTyp<T> const &lhs, ScopedTyp<T> const &rhs)
	{ return lhs.Get() == rhs.Get(); }

	template < typename T >
	inline bool operator != (ScopedTyp<T> const &lhs, ScopedTyp<T> const &rhs)
	{ return lhs.Get() != rhs.Get(); }

	template < typename T >
	inline bool operator < (ScopedTyp<T> const &lhs, ScopedTyp<T> const &rhs)
	{ return lhs.Get() < rhs.Get(); }

	template < typename T >
	inline bool operator <= (ScopedTyp<T> const &lhs, ScopedTyp<T> const &rhs)
	{ return lhs.Get() <= rhs.Get(); }

	template < typename T >
	inline bool operator > (ScopedTyp<T> const &lhs, ScopedTyp<T> const &rhs)
	{ return lhs.Get() > rhs.Get(); }

	template < typename T >
	inline bool operator >= (ScopedTyp<T> const &lhs, ScopedTyp<T> const &rhs)
	{ return lhs.Get() >= rhs.Get(); }

	template < typename T >
	inline void swap(ScopedTyp<T> &lhs, ScopedTyp<T> &rhs)
	{ return lhs.Swap(rhs); }

}