
#include "cxxlib_conio.h"
#include "cxxlib_conio_internal.h"

#ifdef _WIN
#	include <conio.h>
#else
#	include <sys/select.h>
#endif

namespace cxxlib
{

	int kbhit()
	{
#	ifdef _WIN
		return _kbhit();
#	else
		cxxlib_internal::TerminalRawMode::InitRawMode();

		struct timeval stTimeVal = { 0L, 0L };
		fd_set stFDs;
		FD_ZERO(&stFDs);
		FD_SET(0, &stFDs);
		return select(1, &stFDs, NULL, NULL, &stTimeVal);
#	endif
	}

	int getch()
	{
#	ifdef _WIN
		return _getch();
#	else
		cxxlib_internal::TerminalRawMode::InitRawMode();

		unsigned char btBuff;
		int iRead = read(0, &btBuff, sizeof(btBuff));
		return iRead > 0 ? btBuff : iRead;
#	endif
	}

}
