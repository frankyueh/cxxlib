#pragma once

#include "SharedPtr.h"

namespace SharedPtrTest
{
	class ForwardDeclared;

	class TestForwardDeclared
	{
	public:
		TestForwardDeclared(int *);
		~TestForwardDeclared() {} // expect: without no deleter warning
	private:
		cxxlib::SharedPtr<ForwardDeclared> m_qcPtr;
	};
}
