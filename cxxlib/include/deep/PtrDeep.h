#pragma once

#include "cxxlib.h"

namespace cxxlib { namespace Deep {

	/// pointer element
	///

	template < typename T > struct PtrElement { typedef T Type; };
	template < typename T > struct PtrElement<T[]> { typedef T Type; };

	/// pointer deleter
	///

	template < typename T >
	struct PtrDeleter { void operator ()(T *pt) { delete pt; } };

	template < typename T > struct PtrDeleter<T[]>
	{ void operator ()(T *pat) { delete [] pat; } };

	template <> struct PtrDeleter<void> { void operator ()(void *) {} };

	/// pointer dereference type
	///

	template < typename T > struct PtrDereference { typedef T & Type; };
	template < typename T > struct PtrDereference<T[]> { typedef void Type; };
	template <> struct PtrDereference<void> { typedef void Type; };

	/// pointer member access type
	///

	template < typename T > struct PtrMemberAccess { typedef T * Type; };
	template < typename T > struct PtrMemberAccess<T[]> { typedef void Type; };

	/// pointer array access type
	///

	template < typename T > struct PtrArrayAccess { typedef void Type; };
	template < typename T > struct PtrArrayAccess<T[]> { typedef T & Type; };
} }
