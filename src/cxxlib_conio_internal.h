#pragma once

#include "cxxlib.h"

#ifdef _WIN
#else

#	include "Mutex.h"

#	include <termios.h>

namespace cxxlib_internal
{
	class TerminalRawMode : FINAL, NOCOPY
	{
	public:
		static void InitRawMode();
		static void ResetRawMode();
	private:
		static volatile TerminalRawMode * s_pcInstance;
		static cxxlib::Mutex s_cInstanceLock;

		TerminalRawMode();
		~TerminalRawMode();

		struct termios m_stOrgSettings;
		struct termios m_stRawSettings;
	};
}

#endif