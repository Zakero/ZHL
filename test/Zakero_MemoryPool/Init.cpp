/*
g++ -std=c++20 -DCATCH_CONFIG_MAIN -Wall -Werror -o Init Init.cpp && ./Init
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


TEST_CASE("Init")
{
	MemoryPool memory_pool("Zakero_MemoryPool_Test_Init");

	SECTION("Pre-Init")
	{
		REQUIRE(memory_pool.fd()                            == -1     );
		REQUIRE(memory_pool.size()                          ==  0     );
		REQUIRE(memory_pool.alloc(1)                        == -1     );
		REQUIRE(memory_pool.alloc(1, uint8_t(0xff))         == -1     );
		REQUIRE(memory_pool.alloc(1, uint32_t(0xffff'ffff)) == -1     );
		REQUIRE(memory_pool.realloc( 1, 1)                  == -1     );
		REQUIRE(memory_pool.realloc( 1, 0)                  == -1     );
		REQUIRE(memory_pool.realloc(-1, 1)                  == -1     );
		REQUIRE(memory_pool.addressOf( 0)                   == nullptr);
		REQUIRE(memory_pool.addressOf( 1)                   == nullptr);
		REQUIRE(memory_pool.addressOf(-1)                   == nullptr);
	}

	SECTION("Bad Init")
	{
		std::error_code error;

		error = memory_pool.init(0);
		INFO(error.value() << "\t" << error.message());
		REQUIRE(error.value() == MemoryPool::Error_Size_Too_Small);

		error = memory_pool.init(-1); // -1 converted to unsigned
		INFO(error.value() << "\t" << error.message());
		REQUIRE(error.value() == MemoryPool::Error_Size_Too_Large);

		error = memory_pool.init(MemoryPool::Size_Max + 1);
		INFO(error.value() << "\t" << error.message());
		REQUIRE(error.value() == MemoryPool::Error_Size_Too_Large);

		// Not enough physical memory on for this to pass
		error = memory_pool.init(MemoryPool::Size_Max);
		INFO(error.value() << "\t" << error.message());
		REQUIRE(error.value() == MemoryPool::Error_Failed_To_Resize_File);

		memory_pool.init(pool_size);
		error = memory_pool.init(pool_size);
		INFO(error.value() << "\t" << error.message());
		REQUIRE(error.value() == MemoryPool::Error_Already_Initialized);

		/*
		 * Error Codes 4 and 6 are OS failures.  Can't test those 
		 * without a mocking framework.
		 */
	}

	SECTION("8-Bit")
	{
		auto error = memory_pool.init(pool_size
			, false
			, MemoryPool::Alignment::Bits_8
			);

		REQUIRE(bool(error) == false);
		REQUIRE(memory_pool.fd() >= 0);
		REQUIRE(memory_pool.size() == pool_size);

		constexpr size_t count = pool_size / 1;
		std::array<off_t, count> offset_array;
		for(size_t i = 0; i < count; i++)
		{
			INFO("Allocation #" << std::to_string(i));
			offset_array[i] = memory_pool.alloc(1);
			REQUIRE(offset_array[i] >= 0);
		}

		off_t off = memory_pool.alloc(1);
		REQUIRE(off == -1);
	}

	SECTION("16-Bit")
	{
		constexpr size_t pool_size = 128;

		auto error = memory_pool.init(pool_size
			, false
			, MemoryPool::Alignment::Bits_16
			);

		REQUIRE(bool(error) == false);
		REQUIRE(memory_pool.fd() >= 0);
		REQUIRE(memory_pool.size() == pool_size);

		constexpr size_t count = pool_size / 2;
		std::array<off_t, count> offset_array;
		for(size_t i = 0; i < count; i++)
		{
			INFO("Allocation #" << std::to_string(i));
			offset_array[i] = memory_pool.alloc(1);
			REQUIRE(offset_array[i] >= 0);
		}

		off_t off = memory_pool.alloc(1);
		REQUIRE(off == -1);
	}

	SECTION("32-Bit")
	{
		constexpr size_t pool_size = 128;

		auto error = memory_pool.init(pool_size
			, false
			, MemoryPool::Alignment::Bits_32
			);

		REQUIRE(bool(error) == false);
		REQUIRE(memory_pool.fd() >= 0);
		REQUIRE(memory_pool.size() == pool_size);

		constexpr size_t count = pool_size / 4;
		std::array<off_t, count> offset_array;
		for(size_t i = 0; i < count; i++)
		{
			INFO("Allocation #" << std::to_string(i));
			offset_array[i] = memory_pool.alloc(1);
			REQUIRE(offset_array[i] >= 0);
		}

		off_t off = memory_pool.alloc(1);
		REQUIRE(off == -1);
	}

	SECTION("64-Bit")
	{
		constexpr size_t pool_size = 128;

		auto error = memory_pool.init(pool_size
			, false
			, MemoryPool::Alignment::Bits_64
			);

		REQUIRE(bool(error) == false);
		REQUIRE(memory_pool.fd() >= 0);
		REQUIRE(memory_pool.size() == pool_size);

		constexpr size_t count = pool_size / 8;
		std::array<off_t, count> offset_array;
		for(size_t i = 0; i < count; i++)
		{
			INFO("Allocation #" << std::to_string(i));
			offset_array[i] = memory_pool.alloc(1);
			REQUIRE(offset_array[i] >= 0);
		}

		off_t off = memory_pool.alloc(1);
		REQUIRE(off == -1);
	}
}
