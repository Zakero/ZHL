/*
g++ -std=c++20 -DCATCH_CONFIG_MAIN -Wall -Werror -o Allocation Allocation.cpp && ./Allocation
 */

#ifdef CATCH_CONFIG_MAIN
#define ZAKERO_MEMORYPOOL_IMPLEMENTATION
#endif

#include "../../include/Zakero_MemoryPool.h"

#include "../catch.hpp"

using namespace zakero;

namespace
{
}


TEST_CASE("Fixed Size MemoryPool")
{
	MemoryPool memory_pool("Zakero_MemoryPool_Test_Allocation");

	SECTION("Bad Alloc")
	{
		auto retval = memory_pool.init(128
			, false
			, MemoryPool::Alignment::Bits_8
			);
		REQUIRE(bool(retval) == false);

		std::error_condition error;
		off_t offset = -1;

		INFO("Allocation size of 0 not allowed.");
		offset = memory_pool.alloc(0, error);
		REQUIRE(offset == -1);
		REQUIRE(error.value() == MemoryPool::Error_Size_Too_Small);

		INFO("Allocation size > MemoryPool::Size_Max");
		offset = memory_pool.alloc(MemoryPool::Size_Max + 1, error);
		REQUIRE(offset == -1);
		REQUIRE(error.value() == MemoryPool::Error_Size_Too_Large);

		INFO("Allocation size > MemoryPool size");
		offset = memory_pool.alloc(memory_pool.size() + 1, error);
		REQUIRE(offset == -1);
		REQUIRE(error.value() == MemoryPool::Error_Out_Of_Memory);

		INFO("Allocat too much");
		offset = memory_pool.alloc(memory_pool.size(), error);
		REQUIRE(offset >= 0);
		REQUIRE(error.value() == MemoryPool::Error_None);

		auto off_err = memory_pool.alloc(memory_pool.size(), error);
		REQUIRE(off_err == -1);
		REQUIRE(error.value() == MemoryPool::Error_Out_Of_Memory);
	}

	SECTION("Good Alloc")
	{
		auto retval = memory_pool.init(128
			, false
			, MemoryPool::Alignment::Bits_8
			);
		REQUIRE(bool(retval) == false);

		std::error_condition error;
		off_t offset_1 = -1;
		off_t offset_2 = -1;
		size_t size = memory_pool.size() / 2;

		offset_1 = memory_pool.alloc(size, error);
		REQUIRE(offset_1 >= 0);
		REQUIRE(error.value() == MemoryPool::Error_None);

		offset_2 = memory_pool.alloc(size, error);
		REQUIRE(offset_2 >= 0);
		REQUIRE(error.value() == MemoryPool::Error_None);

		uint8_t* ptr_1 = memory_pool.addressOf(offset_1);
		REQUIRE(ptr_1 != nullptr);

		uint8_t* ptr_2 = memory_pool.addressOf(offset_2);
		REQUIRE(ptr_2 != nullptr);

		for(size_t i = 0; i < size; i++)
		{
			ptr_1[i] = 0x00;
			ptr_2[i] = 0xff;
		}

		INFO("Check for no memory over-lap");
		for(size_t i = 0; i < size; i++)
		{
			REQUIRE(ptr_1[i] == 0x00);
			REQUIRE(ptr_2[i] == 0xff);
		}
	}

	SECTION("8-Bit Fill")
	{
		auto retval = memory_pool.init(128
			, false
			, MemoryPool::Alignment::Bits_8
			);
		REQUIRE(bool(retval) == false);

		std::error_condition error;
		off_t offset_1 = -1;
		off_t offset_2 = -1;
		size_t size = memory_pool.size() / 2;

		offset_1 = memory_pool.alloc(size, uint8_t(0x00), error);
		REQUIRE(offset_1 >= 0);
		REQUIRE(error.value() == MemoryPool::Error_None);

		offset_2 = memory_pool.alloc(size, uint8_t(0xff), error);
		REQUIRE(offset_2 >= 0);
		REQUIRE(error.value() == MemoryPool::Error_None);

		uint8_t* ptr_1 = memory_pool.addressOf(offset_1);
		REQUIRE(ptr_1 != nullptr);

		uint8_t* ptr_2 = memory_pool.addressOf(offset_2);
		REQUIRE(ptr_2 != nullptr);

		INFO("Check for memory");
		for(size_t i = 0; i < size; i++)
		{
			REQUIRE(ptr_1[i] == 0x00);
			REQUIRE(ptr_2[i] == 0xff);
		}
	}

	SECTION("32-Bit Fill")
	{
		auto retval = memory_pool.init(128
			, false
			, MemoryPool::Alignment::Bits_32
			);
		REQUIRE(bool(retval) == false);

		std::error_condition error;
		off_t offset_1 = -1;
		off_t offset_2 = -1;
		size_t size = memory_pool.size() / (sizeof(uint32_t) * 2);

		offset_1 = memory_pool.alloc(size, uint32_t(0x0000'0000), error);
		REQUIRE(offset_1 >= 0);
		REQUIRE(error.value() == MemoryPool::Error_None);

		offset_2 = memory_pool.alloc(size, uint32_t(0xffff'ffff), error);
		REQUIRE(offset_2 >= 0);
		REQUIRE(error.value() == MemoryPool::Error_None);

		uint32_t* ptr_1 = (uint32_t*)memory_pool.addressOf(offset_1);
		REQUIRE(ptr_1 != nullptr);

		uint32_t* ptr_2 = (uint32_t*)memory_pool.addressOf(offset_2);
		REQUIRE(ptr_2 != nullptr);

		INFO("Check memory values");
		size_t count = size / sizeof(uint32_t);
		for(size_t i = 0; i < count; i++)
		{
			REQUIRE(ptr_1[i] == 0x0000'0000);
			REQUIRE(ptr_2[i] == 0xffff'ffff);
		}
	}
}


TEST_CASE("Dynamic Sized MemoryPool")
{
	MemoryPool memory_pool("Zakero_MemoryPool_Test_Allocation");

	SECTION("Bad Alloc")
	{
		auto retval = memory_pool.init(128
			, true
			, MemoryPool::Alignment::Bits_8
			);
		REQUIRE(bool(retval) == false);

		std::error_condition error;
		off_t offset = -1;

		INFO("Allocation size of 0 not allowed.");
		offset = memory_pool.alloc(0, error);
		REQUIRE(offset == -1);
		REQUIRE(error.value() == MemoryPool::Error_Size_Too_Small);

		INFO("Allocation size > MemoryPool::Size_Max");
		offset = memory_pool.alloc(MemoryPool::Size_Max + 1, error);
		REQUIRE(offset == -1);
		REQUIRE(error.value() == MemoryPool::Error_Size_Too_Large);
	}

	SECTION("Good Alloc")
	{
		auto retval = memory_pool.init(128
			, true
			, MemoryPool::Alignment::Bits_8
			);
		REQUIRE(bool(retval) == false);

		std::error_condition error;
		off_t offset_1 = -1;
		off_t offset_2 = -1;
		size_t size = memory_pool.size();

		offset_1 = memory_pool.alloc(size, error);
		REQUIRE(offset_1 >= 0);
		REQUIRE(error.value() == MemoryPool::Error_None);

		offset_2 = memory_pool.alloc(size, error);
		REQUIRE(offset_2 >= 0);
		REQUIRE(error.value() == MemoryPool::Error_None);

		uint8_t* ptr_1 = memory_pool.addressOf(offset_1);
		REQUIRE(ptr_1 != nullptr);

		uint8_t* ptr_2 = memory_pool.addressOf(offset_2);
		REQUIRE(ptr_2 != nullptr);

		for(size_t i = 0; i < size; i++)
		{
			ptr_1[i] = 0x00;
			ptr_2[i] = 0xff;
		}

		INFO("Check for no memory over-lap");
		for(size_t i = 0; i < size; i++)
		{
			REQUIRE(ptr_1[i] == 0x00);
			REQUIRE(ptr_2[i] == 0xff);
		}
	}

	SECTION("8-Bit Fill")
	{
		auto retval = memory_pool.init(128
			, true
			, MemoryPool::Alignment::Bits_8
			);
		REQUIRE(bool(retval) == false);

		std::error_condition error;
		off_t offset_1 = -1;
		off_t offset_2 = -1;
		size_t size = memory_pool.size();

		offset_1 = memory_pool.alloc(size, uint8_t(0x00), error);
		REQUIRE(offset_1 >= 0);
		REQUIRE(error.value() == MemoryPool::Error_None);

		offset_2 = memory_pool.alloc(size, uint8_t(0xff), error);
		REQUIRE(offset_2 >= 0);
		REQUIRE(error.value() == MemoryPool::Error_None);

		uint8_t* ptr_1 = memory_pool.addressOf(offset_1);
		REQUIRE(ptr_1 != nullptr);

		uint8_t* ptr_2 = memory_pool.addressOf(offset_2);
		REQUIRE(ptr_2 != nullptr);

		INFO("Check for memory");
		for(size_t i = 0; i < size; i++)
		{
			REQUIRE(ptr_1[i] == 0x00);
			REQUIRE(ptr_2[i] == 0xff);
		}
	}

	SECTION("32-Bit Fill")
	{
		auto retval = memory_pool.init(128
			, true
			, MemoryPool::Alignment::Bits_32
			);
		REQUIRE(bool(retval) == false);

		std::error_condition error;
		off_t offset_1 = -1;
		off_t offset_2 = -1;
		size_t size = memory_pool.size();

		offset_1 = memory_pool.alloc(size, uint32_t(0x0000'0000), error);
		REQUIRE(offset_1 >= 0);
		REQUIRE(error.value() == MemoryPool::Error_None);

		offset_2 = memory_pool.alloc(size, uint32_t(0xffff'ffff), error);
		REQUIRE(offset_2 >= 0);
		REQUIRE(error.value() == MemoryPool::Error_None);

		uint32_t* ptr_1 = (uint32_t*)memory_pool.addressOf(offset_1);
		REQUIRE(ptr_1 != nullptr);

		uint32_t* ptr_2 = (uint32_t*)memory_pool.addressOf(offset_2);
		REQUIRE(ptr_2 != nullptr);

		INFO("Check memory values");
		size_t count = size / sizeof(uint32_t);
		for(size_t i = 0; i < count; i++)
		{
			REQUIRE(ptr_1[i] == 0x0000'0000);
			REQUIRE(ptr_2[i] == 0xffff'ffff);
		}
	}
}


TEST_CASE("Free")
{
	MemoryPool memory_pool("Zakero_MemoryPool_Test_Allocation");

	SECTION("Bad Free")
	{
		off_t offset_32 = 0;

		memory_pool.free(offset_32);

		auto retval = memory_pool.init(64
			, false
			, MemoryPool::Alignment::Bits_8
			);
		REQUIRE(bool(retval) == false);

		offset_32 = memory_pool.alloc(64);
		memory_pool.free(offset_32);
		offset_32 = memory_pool.alloc(64);

		REQUIRE(offset_32 != -1);
		memory_pool.free(offset_32);

		off_t offset_1 = -1;
		offset_1  = memory_pool.alloc(32);
		offset_32 = memory_pool.alloc(32);
		REQUIRE(offset_32 != -1);

		memory_pool.free(offset_1);
		off_t offset_16 = -1;
		offset_1  = memory_pool.alloc(16);
		offset_16 = memory_pool.alloc(16);

		memory_pool.free(offset_1);
		off_t offset_8 = -1;
		offset_1 = memory_pool.alloc(8);
		offset_8 = memory_pool.alloc(8);

		memory_pool.free(offset_1);
		off_t offset_4 = -1;
		offset_1 = memory_pool.alloc(4);
		offset_4 = memory_pool.alloc(4);

		memory_pool.free(offset_1);
		off_t offset_2 = -1;
		offset_1 = memory_pool.alloc(2);
		offset_2 = memory_pool.alloc(2);

		memory_pool.free(offset_1);
		off_t offset = -1;
		offset   = memory_pool.alloc(1);
		offset_1 = memory_pool.alloc(1);

		memory_pool.free(offset   );
		memory_pool.free(offset_1 );
		memory_pool.free(offset_2 );
		memory_pool.free(offset_4 );
		memory_pool.free(offset_8 );
		memory_pool.free(offset_16);
		memory_pool.free(offset_32);
	}
}


TEST_CASE("Resize")
{
	MemoryPool memory_pool("Zakero_MemoryPool_Test_Allocation");

	SECTION("Bad Resize")
	{
		std::error_condition error;

		off_t retval = -1;
		(void)retval;

		retval = memory_pool.realloc(-1, 1, error);
		REQUIRE(error.value() == MemoryPool::Error_Invalid_Offset);

		memory_pool.init(128
			, false
			, MemoryPool::Alignment::Bits_8
			);

		off_t offset =  0;
		retval = memory_pool.realloc(offset, 1, error);
		REQUIRE(error.value() == MemoryPool::Error_Invalid_Offset);

		offset = memory_pool.alloc(128);
		REQUIRE(offset >= 0);

		retval = memory_pool.realloc(offset, 0, error);
		REQUIRE(error.value() == MemoryPool::Error_Size_Too_Small);

		retval = memory_pool.realloc(offset, MemoryPool::Size_Max + 1, error);
		REQUIRE(error.value() == MemoryPool::Error_Size_Too_Large);

		retval = memory_pool.realloc(offset, 256, error);
		REQUIRE(error.value() == MemoryPool::Error_Out_Of_Memory);
	}

	SECTION("Fixed size pool")
	{
		memory_pool.init(128
			, false
			, MemoryPool::Alignment::Bits_8
			);

		std::error_condition error;
		off_t offset_1 = -1;
		off_t offset_2 = -1;

		offset_1 = memory_pool.alloc(32);
		REQUIRE(offset_1 >= 0);

		offset_2 = memory_pool.alloc(32);
		REQUIRE(offset_2 >= 0);

		offset_1 = memory_pool.realloc(offset_1, 64, error);
		REQUIRE(offset_1 >= 0);
		REQUIRE(error.value() == 0);
	}

	SECTION("Dynamic sized pool")
	{
		memory_pool.init(128
			, true
			, MemoryPool::Alignment::Bits_8
			);

		std::error_condition error;
		off_t offset_1 = -1;
		off_t offset_2 = -1;

		offset_1 = memory_pool.alloc(32);
		REQUIRE(offset_1 >= 0);

		offset_2 = memory_pool.alloc(32);
		REQUIRE(offset_2 >= 0);

		offset_1 = memory_pool.realloc(offset_1, 128, error);
		REQUIRE(offset_1 >= 0);
		REQUIRE(error.value() == 0);

		offset_2 = memory_pool.realloc(offset_2, 256, error);
		REQUIRE(offset_2 >= 0);
		REQUIRE(error.value() == 0);

		memory_pool.free(offset_2);

		offset_1 = memory_pool.realloc(offset_1, 64, error);
		REQUIRE(offset_1 >= 0);
		REQUIRE(error.value() == 0);

		offset_2 = memory_pool.alloc(64);
		REQUIRE(offset_2 >= 0);
		REQUIRE(error.value() == 0);

		offset_2 = memory_pool.realloc(offset_2, 128, error);
		REQUIRE(offset_2 >= 0);
		REQUIRE(error.value() == 0);
	}
}
