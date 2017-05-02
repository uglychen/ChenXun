#include "../src/proc_thread_test.h"
#include "gtest/gtest.h"
#include <string.h>

int main(int argc, char** argv)
{
	printf("-------------------- start ------------------------\n");
	::testing::InitGoogleTest(&argc, argv);

	proc_thread_container& container = g_proc_thread_container();








	printf("-------------------- end ------------------------\n");

	return 0;
}