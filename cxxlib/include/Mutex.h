#pragma once

#include "cxxlib.h"

#include "Guard.h"

namespace cxxlib
{
	class CXXLIB_EXPORT Mutex : GUARD, NOCOPY, FINAL
	{
		DECLARE_IMPL_P

	public :
		enum RecursionMode
		{
			NONRECURSIVE,
			RECURSIVE
		};

		Mutex(RecursionMode eMode = NONRECURSIVE);
		~Mutex();

		void Lock() const;
		bool TryLock() const;
		void Unlock() const;
	};
}
