
#include "cxxlib_conio.h"

#include "gtest/gtest.h"
#include "gtest_utilities.h"

using namespace cxxlib;

TEST(DISABLED_cxxlib_conio__kbhit_getchUserTest, General)
{
	std::cout << "====== kbhit/getch user test begin ======" << std::endl;

	std::cout
		<< "kbhit() waiting any key hit (without any hit 3 second for ignoring the test)" << std::flush;

	int iWaited = 0;
	while (!cxxlib::kbhit())
	{
		gtest_utility::thread_msleep(250);
		std::cout << "." << std::flush;

		iWaited += 250;
		if (iWaited >= 3000)
		{
			std::cout
				<< std::endl
				<< "====== kbhit/getch user test ignored ======" << std::endl;
			return;
		}
	}

	std::cout
		<< std::endl
		<< "getch() fetch key immediately right after kbhit(): " << std::flush;
	std::cout
		<< cxxlib::getch() << std::endl << std::flush;

	std::cout
		<< "getch() blocked until any key hit: " << std::flush;
	std::cout
		<< cxxlib::getch() << std::endl << std::flush;

	std::cout << "====== kbhit/getch user test end ======" << std::endl;
}
