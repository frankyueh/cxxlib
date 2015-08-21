#pragma once

#include "cxxlib.h"

#include "TypeInfo.h"

namespace cxxlib { namespace Deep {

	template < typename R = void >
	class FunctorWrapBase
	{
	public:
		virtual ~FunctorWrapBase() {}

		virtual R operator () (void *) = 0;
		virtual void * GetFunctor(const TypeId &) = 0;
		virtual FunctorWrapBase<R> * Clone() = 0;
	};

	template < typename T, typename F, typename R = void >
	class FunctorWrap : public FunctorWrapBase<R>
	{
		typedef FunctorWrap<T, F, R> ThisType;
	public:
		FunctorWrap() : m_fFun() {}
		FunctorWrap(F const &fFun) : m_fFun(fFun) {}

		virtual R operator () (void *p)
		{ return m_fFun(*reinterpret_cast<T*>(p)); }

		virtual void * GetFunctor(const TypeId &cTypeId)
		{ return cTypeId == TypeInfo<F>::V_ID ? &reinterpret_cast<char &>(m_fFun) : 0; }

		virtual FunctorWrapBase<R> * Clone()
		{ return new ThisType(m_fFun); }

	private:
		F m_fFun;
	};

} }
