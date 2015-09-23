
#include "cxxlib_cio_internal.h"

#ifdef _WIN
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	undef WIN32_LEAN_AND_MEAN
#	include <fcntl.h>
#	include <io.h>
#endif


namespace cxxlib_internal
{

	// NOTE:
	// reference about sharable open file:
	// http://blog.httrack.com/blog/2013/10/05/creating-deletable-and-movable-files-on-windows/
	//
	FILE * fopen_advanced(
		const char *szName,
		const char *szMode,
		unsigned char btShareMode)
	{
		CXXLIB_ASSERT(szName != NULL);
		CXXLIB_ASSERT(szMode != NULL);

#	ifdef _WIN
		if (btShareMode == 0)
		{
			FILE *pstFile;
			if (::fopen_s(&pstFile, szName, szMode) == 0)
				return pstFile;
		}
		else
		{
			DWORD dwDesiredAccess = 0;
			DWORD dwCreationDisposition = 0;
			DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
			DWORD dwShareMode =
				((btShareMode & FSM_READ) ? FILE_SHARE_READ : 0) |
				((btShareMode & FSM_WRITE) ? FILE_SHARE_WRITE : 0) |
				((btShareMode & FSM_DELETE) ? FILE_SHARE_DELETE : 0);
			int iHndFlags = _O_TEXT;

			for (int i = 0 ; szMode[i] != NULL ; ++i)
			{
				switch (szMode[i])
				{
				case 'r':
					dwDesiredAccess |= GENERIC_READ;
					dwCreationDisposition = OPEN_EXISTING;

					iHndFlags |= _O_RDONLY;
					break;

				case 'a':
					dwDesiredAccess |= GENERIC_WRITE;
					dwCreationDisposition = OPEN_ALWAYS;

					iHndFlags |= _O_APPEND;
					break;

				case 'w':
					dwDesiredAccess |= GENERIC_WRITE;
					dwCreationDisposition = CREATE_ALWAYS;

					iHndFlags |= _O_WRONLY;
					break;

				case 'b':
					iHndFlags &= ~static_cast<int>(_O_TEXT);
					iHndFlags |= _O_BINARY;
					break;

				case 't':
					iHndFlags &= ~static_cast<int>(_O_BINARY);
					iHndFlags |= _O_TEXT;
					break;

				case '+':
					dwDesiredAccess |= GENERIC_READ | GENERIC_WRITE;

					iHndFlags &= ~static_cast<int>(_O_RDONLY);
					iHndFlags &= ~static_cast<int>(_O_WRONLY);
					iHndFlags |= _O_RDWR;
					break;
				}
			}

			HANDLE hndHandle = ::CreateFileA(szName,
				dwDesiredAccess,
				dwShareMode,
				NULL,
				dwCreationDisposition,
				dwFlagsAndAttributes,
				NULL);

			if (hndHandle != INVALID_HANDLE_VALUE)
			{
				int iFD = ::_open_osfhandle(reinterpret_cast<intptr_t>(hndHandle), iHndFlags);
				if (iFD != -1)
					return ::_fdopen(iFD, szMode);
				else
					::CloseHandle(hndHandle);
			}
		}
#	else
		return ::fopen(szName, szMode);
#	endif
		return NULL;
	}
}