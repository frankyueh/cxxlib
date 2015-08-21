#pragma once

#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <cassert>
#include <string>

#include "cxxlib_defines.h"
#include "cxxlib_assert.h"
#include "cxxlib_macro.h"
#include "nativetype_defines.h"
#include "class_defines.h"

namespace cxxlib
{
	///
	/// Constants
	///

	const size_t MAX_SZBUFF_LENGTH 		= 8192;
	const size_t SHORT_SZBUFF_LENGTH 	= 260;
	const unsigned int WAIT_INFINITE	= 0xFFFFFFFF;

	///
	/// Pointer functions
	///

	template < typename T, typename S >
	inline T polymorphic_downcast(S *x)
	{
		CXXLIB_ASSERT(dynamic_cast<T>(x) == x);
		return static_cast<T>(x);
	}

	///
	/// Operate functions
	///

	template < typename T >
	inline void swap(T &lhs, T &rhs) { T r = rhs; rhs = lhs; lhs = r; }
}
