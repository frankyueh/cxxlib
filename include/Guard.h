#pragma once

#include "cxxlib.h"

#define GUARD		public ::cxxlib::Guard

namespace cxxlib
{

	class CXXLIB_EXPORT Guard
	{
	public:
		virtual ~Guard() {}
		virtual void Lock() const = 0;
		virtual void Unlock() const = 0;
	};

	class Guarded : FINAL, NOCOPY
	{
	public:
		Guarded(Guard const &cGuard) : m_cGuard(cGuard) { m_cGuard.Lock(); }
		~Guarded() { m_cGuard.Unlock(); }
	private:
		Guard const &m_cGuard;
	};

}
