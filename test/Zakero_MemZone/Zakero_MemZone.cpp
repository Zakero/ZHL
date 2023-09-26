/*
g++ -std=c++23 -O0 -Wall -Werror -Wpedantic -g -o Zakero_MemZone Zakero_MemZone.cpp && ./Zakero_MemZone
g++ -std=c++23 -O0 -Wall -Werror -Wpedantic -g -o Zakero_MemZone -coverage Zakero_MemZone.cpp \
	&& ./Zakero_MemZone \
	&& lcov --directory . --capture --demangle-cpp --exclude bits --exclude ext --exclude atomic --exclude bit --exclude cmath --exclude compare --exclude iomanip --exclude limits --exclude new --exclude type_traits --output-file Zakero_MemZone.lcov --test-name Zakero_MemZone \
	&& genhtml -o test-coverage Zakero_MemZone.lcov
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
