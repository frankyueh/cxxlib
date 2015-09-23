#pragma once

#include "ScopedPtr.h"

namespace ScopedPtrTest
{
	class ForwardDeclared;

	class TestForwardDeclared
	{
	public:
		TestForwardDeclared(int *);
		~TestForwardDeclared() {} // expect: without no deleter warning
	private:
		cxxlib::ScopedPtr<ForwardDeclared> m_qcPtr;
	};
}
