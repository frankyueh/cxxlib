
#include "cxxlib_conio_internal.h"

#ifdef _WIN
#else
#	include <unistd.h>

using namespace cxxlib;
using namespace cxxlib_internal;

volatile TerminalRawMode * TerminalRawMode::s_pcInstance = NULL;
cxxlib::Mutex TerminalRawMode::s_cInstanceLock(cxxlib::Mutex::NONRECURSIVE);

void TerminalRawMode::InitRawMode()
{
	if (!s_pcInstance)
	{
		Guarded cGuareded(s_cInstanceLock);
		if (!s_pcInstance)
		{
			s_pcInstance = new TerminalRawMode();
			atexit(&TerminalRawMode::ResetRawMode);
		}
	}
}

void TerminalRawMode::ResetRawMode()
{
	if (s_pcInstance)
	{
		Guarded cGuareded(s_cInstanceLock);
		if (s_pcInstance)
		{
			delete s_pcInstance;
			s_pcInstance = NULL;
		}
	}
}

TerminalRawMode::TerminalRawMode()
{
	tcgetattr(0, &m_stOrgSettings);
	m_stRawSettings = m_stOrgSettings;

	m_stRawSettings.c_lflag &= ~(ECHO | ICANON | ISIG);
	m_stRawSettings.c_cc[VMIN] = sizeof(unsigned char);
	m_stRawSettings.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &m_stRawSettings);
}

TerminalRawMode::~TerminalRawMode()
{
	tcsetattr(0, TCSANOW, &m_stOrgSettings);
}

#endif
