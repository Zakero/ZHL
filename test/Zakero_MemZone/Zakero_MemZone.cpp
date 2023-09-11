/*
g++ -std=c++23 -Wall -Werror -Wpedantic -o Zakero_MemZone Zakero_MemZone.cpp && ./Zakero_MemZone
 */
#define DOCTEST_CONFIG_IMPLEMENT
#include "../doctest.h"

#define ZAKERO_MEMZONE_DEBUG_ENABLED
#define ZAKERO_MEMZONE_VALIDATE_ENABLED
#define ZAKERO_MEMZONE_IMPLEMENTATION
#define ZAKERO_MEMZONE_IMPLEMENTATION_TEST
#include "../../include/Zakero_MemZone.h"

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
