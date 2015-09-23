
#include "cxxlib_cio.h"
#include "cxxlib_cio_internal.h"

namespace cxxlib
{

	FILE * fopen_unixlike(const char *szName, const char *szMode)
	{
		return cxxlib_internal::fopen_advanced(
			szName,
			szMode,
			cxxlib_internal::FSM_READ | cxxlib_internal::FSM_WRITE);
	}

}