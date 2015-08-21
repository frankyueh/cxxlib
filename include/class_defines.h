#pragma once

#include "cxxlib_defines.h"


namespace cxxlib
{

#define FINAL private virtual ::cxxlib::Final<__COUNTER__>

	template< int T > class Final { protected: Final() {} };

#define NOCOPY private ::cxxlib::NoCopy

	class CXXLIB_EXPORT NoCopy
	{
	public:
		NoCopy() {}
	private:
		NoCopy(const NoCopy& rhs);
		NoCopy& operator=(const NoCopy& rhs);
	};

#define SAFEBOOL public virtual ::cxxlib::SafeBool<>

	template < typename T = void >
	class SafeBool
	{
	protected:
		void NoComparisonForSafeBool() const {}
		SafeBool() {}
		~SafeBool() {}
		SafeBool(const SafeBool&) {}
		SafeBool & operator = (const SafeBool&) { return *this; }
		virtual bool TestBool() const = 0;
	public:
		typedef void (SafeBool::*BoolType)() const;
		operator BoolType () const
		{ return TestBool() ? &SafeBool::NoComparisonForSafeBool : 0; }
	};

	template < typename T >
	inline void operator == (const SafeBool<T> &lhs, const SafeBool<T> &rhs)
	{ lhs.NoComparisonForSafeBool(); return false; }

	template < typename T >
	inline void operator != (const SafeBool<T> &lhs, const SafeBool<T> &rhs)
	{ lhs.NoComparisonForSafeBool(); return false; }
}


#define IMPL_P_VAR __m_pcImplP__

#define DECLARE_IMPL_P											\
	class Impl;													\
	inline Impl * CONCAT(__Get, IMPL_P_VAR)()					\
	{															\
		return IMPL_P_VAR;										\
	}															\
	inline const Impl * CONCAT(__Get, IMPL_P_VAR)() const		\
	{															\
		return IMPL_P_VAR;										\
	}															\
	Impl * IMPL_P_VAR;

#define IMPL_P(_CLASS_)	\
	_CLASS_::Impl * const p = CONCAT(__Get, IMPL_P_VAR)();

#define IMPL_OBJ_P(_CLASS_, _OBJ_)	\
	_CLASS_::Impl * const op = (_OBJ_).CONCAT(__Get, IMPL_P_VAR)();

#define IMPL_S_VAR __m_pcImplS__

#define DECLARE_IMPL_S(_CLASS_)									\
	inline _CLASS_ * CONCAT(__Get, IMPL_S_VAR)()				\
	{															\
		return IMPL_S_VAR;										\
	}															\
	inline const _CLASS_ * CONCAT(__Get, IMPL_S_VAR)() const	\
	{															\
		return IMPL_S_VAR;										\
	}															\
	_CLASS_ * IMPL_S_VAR;

#define IMPL_S(_FULL_CLASS_NAME_)	\
	_FULL_CLASS_NAME_ * const s = CONCAT(__Get, IMPL_S_VAR)();

#define IMPL_OBJ_S(_FULL_CLASS_NAME_, _OBJ_)	\
	_FULL_CLASS_NAME_ * const os = (_OBJ_).CONCAT(__Get, IMPL_S_VAR)();
