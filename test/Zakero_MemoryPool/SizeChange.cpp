/*
g++ -std=c++20 -DCATCH_CONFIG_MAIN -Wall -Werror -o SizeChange SizeChange.cpp && ./SizeChange
 */

#ifdef CATCH_CONFIG_MAIN
#define ZAKERO_MEMORYPOOL_IMPLEMENTATION
#endif

#include "../../include/Zakero_MemoryPool.h"

#include "../catch.hpp"

using namespace zakero;

namespace
{
	constexpr size_t pool_size = 128;
}


TEST_CASE("SizeChange")
{
	MemoryPool memory_pool("Zakero_MemoryPool_Test_SizeChange");

	SECTION("Bad Offset - Uninitialized MemoryPool")
	{
		uint8_t* ptr = nullptr;

		ptr = memory_pool.addressOf(-1);
		REQUIRE(ptr == nullptr);

		ptr = memory_pool.addressOf(0);
		REQUIRE(ptr == nullptr);

		ptr = memory_pool.addressOf(MemoryPool::Size_Max + 1);
		REQUIRE(ptr == nullptr);
	}

	SECTION("Bad Offset - Nothing Allocated")
	{
		memory_pool.init(1024);

		uint8_t* ptr = nullptr;

		ptr = memory_pool.addressOf(-1);
		REQUIRE(ptr == nullptr);

		ptr = memory_pool.addressOf(0);
		REQUIRE(ptr == nullptr);

		ptr = memory_pool.addressOf(MemoryPool::Size_Max + 1);
		REQUIRE(ptr == nullptr);
	}

	SECTION("Good Offset")
	{
		memory_pool.init(1024
			, false
			, MemoryPool::Alignment::Bits_8
			);

		off_t offset_1 = memory_pool.alloc(256);
		off_t offset_2 = memory_pool.alloc(256);

		uint8_t* ptr_1 = memory_pool.addressOf(offset_1);
		uint8_t* ptr_2 = memory_pool.addressOf(offset_2);

		REQUIRE(ptr_1 != nullptr);
		REQUIRE(ptr_2 != nullptr);
		REQUIRE(ptr_1 != ptr_2);
	}
}


TEST_CASE("Remap")
{
	MemoryPool memory_pool("Zakero_MemoryPool_Test_SizeChange");

	off_t offset_1 = -1;
	off_t offset_2 = -1;

	uint8_t* ptr_1 = nullptr;
	uint8_t* ptr_2 = nullptr;

	uint8_t* ptr_1_new = nullptr;
	uint8_t* ptr_2_new = nullptr;
	
	memory_pool.onRemap([&](const MemoryPool::AddressMap& map)
	{
		REQUIRE(map.contains(ptr_1) == true);
		REQUIRE(map.contains(ptr_2) == true);

		ptr_1_new = map.at(ptr_1);
		ptr_2_new = MemoryPool::remap(map, ptr_2);
	});

	memory_pool.init(2
		, true
		, MemoryPool::Alignment::Bits_8
		);

	offset_1 = memory_pool.alloc(1);
	offset_2 = memory_pool.alloc(1);

	ptr_1 = memory_pool.addressOf(offset_1);
	ptr_2 = memory_pool.addressOf(offset_2);

	*ptr_1 = 111;
	*ptr_2 = 222;

	// This should trigger the remap
	off_t offset = memory_pool.alloc(1024000);
	REQUIRE(offset != -1);

	REQUIRE(ptr_1_new != nullptr);
	REQUIRE(ptr_2_new != nullptr);

	REQUIRE(ptr_1_new != ptr_1);
	REQUIRE(ptr_2_new != ptr_2);

	REQUIRE(*ptr_1_new == 111);
	REQUIRE(*ptr_2_new == 222);
}


TEST_CASE("Size Change")
{
	zakero::MemoryPool memory_pool("Zakero_MemoryPool_Test_SizeChange");

	SECTION("onSizeChange")
	{
		int count = 0;
		memory_pool.sizeOnChange([&count](size_t new_size)
		{
			count++;
		});

		// A resizeable memory pool with 8-bit alignment.
		INFO("init() should not call the onSizeChange() lambda.");
		auto error = memory_pool.init(1
			, true
			, zakero::MemoryPool::Alignment::Bits_8
			);
		REQUIRE(error.value() == 0);
		REQUIRE(count == 0);

		off_t offset = -1;

		INFO("No expansion, onSizeChange() lambda should not execute.");
		offset = memory_pool.alloc(1);
		memory_pool.free(offset);
		REQUIRE(count == 0);

		INFO("onSizeChange() lambda should execute.");
		offset = memory_pool.alloc(3);
		memory_pool.free(offset);
		REQUIRE(count == 1);
		REQUIRE(memory_pool.size() == 3);

		off_t offset_2 = -1;
		count = 0;

		INFO("onSizeChange() lambda should execute on 3rd alloc().");
		// MemoryPool size = 3 bytes
		offset = memory_pool.alloc(1);   // 1st alloc(), 2 bytes avail
		offset_2 = memory_pool.alloc(2); // 2nd alloc(), 0 bytes avail
		memory_pool.free(offset);        //              1 byte  avail
		// Over allocate
		offset = memory_pool.alloc(2);   // 3rd alloc(), 0 bytes avail
		memory_pool.free(offset);
		memory_pool.free(offset_2);
		REQUIRE(count == 1);
		REQUIRE(memory_pool.size() >= 4);
	}
}
