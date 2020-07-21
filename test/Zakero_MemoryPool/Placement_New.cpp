/*
g++ -std=c++20 -DCATCH_CONFIG_MAIN -Wall -Werror -o Placement_New Placement_New.cpp && ./Placement_New
 */

#ifdef CATCH_CONFIG_MAIN
#define ZAKERO_MEMORYPOOL_IMPLEMENTATION
#endif

#include "../../include/Zakero_MemoryPool.h"

#include "../catch.hpp"

#include <array>

using namespace zakero;

namespace
{
}


TEST_CASE("Placement New: Array")
{
	MemoryPool memory_pool("Placement_New_Array");

	constexpr size_t count = 100;
	size_t size = sizeof(size_t) * count;

	auto retval = memory_pool.init(size);
	REQUIRE((bool)retval == false);

	off_t offset = memory_pool.alloc(size);
	REQUIRE(offset >= 0);

	size_t* p = (size_t*)memory_pool.addressOf(offset);

	{ // Have the array go out of scope
		// Create "placement new" array
		std::array<size_t, count>* a = new(p) std::array<size_t, count>();

		std::array<size_t, count>& array = *a;
		for(size_t i = 0; i < count; i++)
		{
			array[i] = i;
		}

		// Delete "placement new" array
		// Doesn't work as advertized
		//delete a->~std::array();
	}

	for(size_t i = 0; i < count; i++)
	{
		REQUIRE(p[i] == i);
	}
}
