/*
g++ -std=c++20 -Wall -Werror -o Zakero_Network Zakero_Network.cpp && ./Zakero_Network
 */
#define DOCTEST_CONFIG_IMPLEMENT
#include "../doctest.h"

#define ZAKERO_NETWORK_IMPLEMENTATION
#define ZAKERO_NETWORK_IMPLEMENTATION_TEST
#include "../../include/Zakero_Network.h"

int main(int argc, char** argv)
{
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

	// --- Misc Stuff --- ///

	// --- Done --- ///
	return result;
}
