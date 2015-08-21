#pragma once

#include "cxxlib.h"

#include <typeinfo>

namespace cxxlib { namespace Deep {

	class TypeId : NOCOPY
	{
		const char * const NAME;
	public:
		TypeId(const char *sz) : NAME(sz) {}
		bool operator == (const TypeId &rhs) const { return this == &rhs; }
		bool operator != (const TypeId &rhs) const { return this != &rhs; }
		bool operator < (const TypeId &rhs) const { return this < &rhs; }
		const char * Name() const { return NAME; }
	};

	template < typename T >
	struct TypeInfo { static const TypeId V_ID; };
	template < typename T >
	const TypeId TypeInfo<T>::V_ID(typeid(T).name());
} }
