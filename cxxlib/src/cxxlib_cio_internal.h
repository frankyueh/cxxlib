#pragma once

#include "cxxlib.h"

namespace cxxlib_internal
{
	enum FopenShareMode
	{
		FSM_READ	= 0x01,
		FSM_WRITE	= 0x02,
		FSM_DELETE	= 0x04
	};

	// NOTE: option btShareMode is currently support for Windows only
	CXXLIB_INTERNAL_EXPORT FILE * fopen_advanced(
		const char *szName,
		const char *szMode,
		unsigned char btShareMode = 0);
}
