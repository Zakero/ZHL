/*
g++ -std=c++20 -Wall -Werror -o Zakero_MessagePack Zakero_MessagePack.cpp && ./Zakero_MessagePack
 */
#define DOCTEST_CONFIG_IMPLEMENT
#include "../doctest.h"

#define ZAKERO_MESSAGEPACK_IMPLEMENTATION
#define ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST
#include "../../include/Zakero_MessagePack.h"

int main(int argc, char** argv)
{
	printf("size: %lu\n", sizeof(long));
	doctest::Context context;

	// --- Configuration: Defaults --- //
	context.setOption("order-by" , "name");
	context.setOption("rand-seed", "0"   );

	// --- Configuration: Parse Command-Line --- //
	context.applyCommandLine(argc, argv);

	// --- Configuration: Overrides --- //

	// --- Run Tests --- //
	int result = context.run();

	// --- Get Results --- ///
	if(context.shouldExit())
	{
		return result;
	}

	// --- Do Misc Stuff --- ///

	// --- Do Done --- ///
	return result;
}
