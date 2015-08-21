#pragma once

#include "cxxlib.h"

#ifdef _WIN
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	undef WIN32_LEAN_AND_MEAN
#endif

namespace cxxlib
{

#ifdef _WIN
	typedef HANDLE				CondInternalObj;
	typedef CRITICAL_SECTION	MutexInternalObj;
#else
	typedef pthread_cond_t		CondInternalObj;
	typedef pthread_mutex_t		MutexInternalObj;
#endif

	class CXXLIB_EXPORT BaseSignal : NOCOPY
	{
	public:
		virtual ~BaseSignal();

		virtual bool	IsSet() const;
		virtual void	Set() const = 0;
		virtual void	Set(bool bAutoReset) const;
		virtual void	Reset() const;
		virtual bool	Wait(unsigned int uTimeoutMS = WAIT_INFINITE) const;

	protected:
		explicit BaseSignal(bool bExclusive);

#	ifdef _WIN
		mutable CondInternalObj	m_cEvent;
#	else
		mutable bool	m_bSignaled;
		mutable CondInternalObj		m_cEvent;
		mutable MutexInternalObj	m_cMutex;
		const	bool	m_bExclusive;
		mutable bool	m_bAutoReset;
#	endif
	}; // class BaseSignal


	class CXXLIB_EXPORT CondSignal : public BaseSignal
	{
	public:
#	ifdef _WIN
		CondSignal();
		virtual ~CondSignal();

		bool	IsSet() const;
		void	Set()	const;
		void	Reset() const;
		bool	Wait(unsigned int uTimeoutMS = WAIT_INFINITE) const;
#	else
		CondSignal() : BaseSignal(true) {}

		void	Set()	const { BaseSignal::Set(false); }
#	endif

	private:
#	ifdef _WIN
		mutable MutexInternalObj m_stMutex;
#	endif

	}; // class CondSignal


	class CXXLIB_EXPORT EventSignal : public BaseSignal
	{
	public:

		EventSignal() : BaseSignal(false) {}

		// NOTICE: In the AutoReset Set() mode, IsSet() will always return false.
		//         On the other hand, thread will still blocked by Wait() if you call Set() before Wait().
		inline void Set(bool bAutoReset)	const { BaseSignal::Set(bAutoReset); }
		inline void	Set()					const { BaseSignal::Set(true); }

	};

}
