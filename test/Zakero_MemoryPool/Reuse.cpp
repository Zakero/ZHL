/*
g++ -std=c++20 -DCATCH_CONFIG_MAIN -Wall -Werror -o Reuse Reuse.cpp && ./Reuse
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


TEST_CASE("Reuse fixed size memory blocks in the MemoryPool")
{
	MemoryPool memory_pool("Zakero_MemoryPool_Test_Reuse");
	MemoryPool::VectorSegment segment;
	std::error_code           error;
	const size_t              block = 7654321;
	size_t                    total_size = 0;

	SECTION("1st Block Reuse")
	{
		error = memory_pool.init(1
			, true
			, MemoryPool::Alignment::Bits_8
			);
		REQUIRE(bool(error) == false);

		INFO("Allocate 3 Blocks");

		off_t block_1 = memory_pool.alloc(block, error);
		REQUIRE(block_1 >= 0);

		off_t block_2 = memory_pool.alloc(block, error);
		REQUIRE(block_2 >= 0);

		off_t block_3 = memory_pool.alloc(block, error);
		REQUIRE(block_3 >= 0);

		total_size = memory_pool.size();
		REQUIRE(total_size == (block * 3));

		segment = memory_pool.segmentList();
		REQUIRE(segment.size() == 3);
		REQUIRE((size_t)segment[0].size == block);
		REQUIRE((size_t)segment[0].in_use == true);
		REQUIRE((size_t)segment[1].size == block);
		REQUIRE((size_t)segment[1].in_use == true);
		REQUIRE((size_t)segment[2].size == block);
		REQUIRE((size_t)segment[2].in_use == true);

		memory_pool.free(block_1);

		total_size = memory_pool.size();
		REQUIRE(total_size == (block * 3));

		segment = memory_pool.segmentList();
		REQUIRE(segment.size() == 3);
		REQUIRE((size_t)segment[0].size == block);
		REQUIRE((size_t)segment[0].in_use == false);
		REQUIRE((size_t)segment[1].size == block);
		REQUIRE((size_t)segment[1].in_use == true);
		REQUIRE((size_t)segment[2].size == block);
		REQUIRE((size_t)segment[2].in_use == true);

		block_1 = memory_pool.alloc(block, error);

		total_size = memory_pool.size();
		REQUIRE(total_size == (block * 3));

		segment = memory_pool.segmentList();
		REQUIRE(segment.size() == 3);
		REQUIRE((size_t)segment[0].size == block);
		REQUIRE((size_t)segment[0].in_use == true);
		REQUIRE((size_t)segment[1].size == block);
		REQUIRE((size_t)segment[1].in_use == true);
		REQUIRE((size_t)segment[2].size == block);
		REQUIRE((size_t)segment[2].in_use == true);
	}

	SECTION("Second Block Reuse")
	{
		error = memory_pool.init(1
			, true
			, MemoryPool::Alignment::Bits_8
			);
		REQUIRE(bool(error) == false);

		INFO("Allocate 3 Blocks");

		off_t block_1 = memory_pool.alloc(block, error);
		REQUIRE(block_1 >= 0);

		off_t block_2 = memory_pool.alloc(block, error);
		REQUIRE(block_2 >= 0);

		off_t block_3 = memory_pool.alloc(block, error);
		REQUIRE(block_3 >= 0);

		total_size = memory_pool.size();
		REQUIRE(total_size == (block * 3));

		segment = memory_pool.segmentList();
		REQUIRE(segment.size() == 3);
		REQUIRE((size_t)segment[0].size == block);
		REQUIRE((size_t)segment[0].in_use == true);
		REQUIRE((size_t)segment[1].size == block);
		REQUIRE((size_t)segment[1].in_use == true);
		REQUIRE((size_t)segment[2].size == block);
		REQUIRE((size_t)segment[2].in_use == true);

		memory_pool.free(block_2);

		total_size = memory_pool.size();
		REQUIRE(total_size == (block * 3));

		segment = memory_pool.segmentList();
		REQUIRE(segment.size() == 3);
		REQUIRE((size_t)segment[0].size == block);
		REQUIRE((size_t)segment[0].in_use == true);
		REQUIRE((size_t)segment[1].size == block);
		REQUIRE((size_t)segment[1].in_use == false);
		REQUIRE((size_t)segment[2].size == block);
		REQUIRE((size_t)segment[2].in_use == true);

		block_2 = memory_pool.alloc(block, error);

		total_size = memory_pool.size();
		REQUIRE(total_size == (block * 3));

		segment = memory_pool.segmentList();
		REQUIRE(segment.size() == 3);
		REQUIRE((size_t)segment[0].size == block);
		REQUIRE((size_t)segment[0].in_use == true);
		REQUIRE((size_t)segment[1].size == block);
		REQUIRE((size_t)segment[1].in_use == true);
		REQUIRE((size_t)segment[2].size == block);
		REQUIRE((size_t)segment[2].in_use == true);
	}

	SECTION("Third Block Reuse")
	{
		error = memory_pool.init(1
			, true
			, MemoryPool::Alignment::Bits_8
			);
		REQUIRE(bool(error) == false);

		INFO("Allocate 3 Blocks");

		off_t block_1 = memory_pool.alloc(block, error);
		REQUIRE(block_1 >= 0);

		off_t block_2 = memory_pool.alloc(block, error);
		REQUIRE(block_2 >= 0);

		off_t block_3 = memory_pool.alloc(block, error);
		REQUIRE(block_3 >= 0);

		total_size = memory_pool.size();
		REQUIRE(total_size == (block * 3));

		segment = memory_pool.segmentList();
		REQUIRE(segment.size() == 3);
		REQUIRE((size_t)segment[0].size == block);
		REQUIRE((size_t)segment[0].in_use == true);
		REQUIRE((size_t)segment[1].size == block);
		REQUIRE((size_t)segment[1].in_use == true);
		REQUIRE((size_t)segment[2].size == block);
		REQUIRE((size_t)segment[2].in_use == true);

		memory_pool.free(block_3);

		total_size = memory_pool.size();
		REQUIRE(total_size == (block * 3));

		segment = memory_pool.segmentList();
		REQUIRE(segment.size() == 3);
		REQUIRE((size_t)segment[0].size == block);
		REQUIRE((size_t)segment[0].in_use == true);
		REQUIRE((size_t)segment[1].size == block);
		REQUIRE((size_t)segment[1].in_use == true);
		REQUIRE((size_t)segment[2].size == block);
		REQUIRE((size_t)segment[2].in_use == false);

		block_3 = memory_pool.alloc(block, error);

		total_size = memory_pool.size();
		REQUIRE(total_size == (block * 3));

		segment = memory_pool.segmentList();
		REQUIRE(segment.size() == 3);
		REQUIRE((size_t)segment[0].size == block);
		REQUIRE((size_t)segment[0].in_use == true);
		REQUIRE((size_t)segment[1].size == block);
		REQUIRE((size_t)segment[1].in_use == true);
		REQUIRE((size_t)segment[2].size == block);
		REQUIRE((size_t)segment[2].in_use == true);
	}

	SECTION("1st and 2nd Block Reuse")
	{
		error = memory_pool.init(1
			, true
			, MemoryPool::Alignment::Bits_8
			);
		REQUIRE(bool(error) == false);

		INFO("Allocate 3 Blocks");

		off_t block_1 = memory_pool.alloc(block, error);
		REQUIRE(block_1 >= 0);

		off_t block_2 = memory_pool.alloc(block, error);
		REQUIRE(block_2 >= 0);

		off_t block_3 = memory_pool.alloc(block, error);
		REQUIRE(block_3 >= 0);

		total_size = memory_pool.size();
		REQUIRE(total_size == (block * 3));

		segment = memory_pool.segmentList();
		REQUIRE(segment.size() == 3);
		REQUIRE((size_t)segment[0].size == block);
		REQUIRE((size_t)segment[0].in_use == true);
		REQUIRE((size_t)segment[1].size == block);
		REQUIRE((size_t)segment[1].in_use == true);
		REQUIRE((size_t)segment[2].size == block);
		REQUIRE((size_t)segment[2].in_use == true);

		// block_1 and block_2 should merge into 1 free block
		memory_pool.free(block_1);
		memory_pool.free(block_2);

		total_size = memory_pool.size();
		REQUIRE(total_size == (block * 3));

		segment = memory_pool.segmentList();
		REQUIRE(segment.size() == 2);
		REQUIRE((size_t)segment[0].size == (block * 2));
		REQUIRE((size_t)segment[0].in_use == false);
		REQUIRE((size_t)segment[1].size == block);
		REQUIRE((size_t)segment[1].in_use == true);

		block_1 = memory_pool.alloc(block, error);
		block_2 = memory_pool.alloc(block, error);

		total_size = memory_pool.size();
		REQUIRE(total_size == (block * 3));

		segment = memory_pool.segmentList();
		REQUIRE(segment.size() == 3);
		REQUIRE((size_t)segment[0].size == block);
		REQUIRE((size_t)segment[0].in_use == true);
		REQUIRE((size_t)segment[1].size == block);
		REQUIRE((size_t)segment[1].in_use == true);
		REQUIRE((size_t)segment[2].size == block);
		REQUIRE((size_t)segment[2].in_use == true);
	}

	SECTION("1st and 3rd Block Reuse")
	{
		error = memory_pool.init(1
			, true
			, MemoryPool::Alignment::Bits_8
			);
		REQUIRE(bool(error) == false);

		INFO("Allocate 3 Blocks");

		off_t block_1 = memory_pool.alloc(block, error);
		REQUIRE(block_1 >= 0);

		off_t block_2 = memory_pool.alloc(block, error);
		REQUIRE(block_2 >= 0);

		off_t block_3 = memory_pool.alloc(block, error);
		REQUIRE(block_3 >= 0);

		total_size = memory_pool.size();
		REQUIRE(total_size == (block * 3));

		segment = memory_pool.segmentList();
		REQUIRE(segment.size() == 3);
		REQUIRE((size_t)segment[0].size == block);
		REQUIRE((size_t)segment[0].in_use == true);
		REQUIRE((size_t)segment[1].size == block);
		REQUIRE((size_t)segment[1].in_use == true);
		REQUIRE((size_t)segment[2].size == block);
		REQUIRE((size_t)segment[2].in_use == true);

		memory_pool.free(block_1);
		memory_pool.free(block_3);

		total_size = memory_pool.size();
		REQUIRE(total_size == (block * 3));

		segment = memory_pool.segmentList();
		REQUIRE(segment.size() == 3);
		REQUIRE((size_t)segment[0].size == block);
		REQUIRE((size_t)segment[0].in_use == false);
		REQUIRE((size_t)segment[1].size == block);
		REQUIRE((size_t)segment[1].in_use == true);
		REQUIRE((size_t)segment[2].size == block);
		REQUIRE((size_t)segment[2].in_use == false);

		block_1 = memory_pool.alloc(block, error);
		block_3 = memory_pool.alloc(block, error);

		total_size = memory_pool.size();
		REQUIRE(total_size == (block * 3));

		segment = memory_pool.segmentList();
		REQUIRE(segment.size() == 3);
		REQUIRE((size_t)segment[0].size == block);
		REQUIRE((size_t)segment[0].in_use == true);
		REQUIRE((size_t)segment[1].size == block);
		REQUIRE((size_t)segment[1].in_use == true);
		REQUIRE((size_t)segment[2].size == block);
		REQUIRE((size_t)segment[2].in_use == true);
	}

	SECTION("2nd ond 3rd Block Reuse")
	{
		error = memory_pool.init(1
			, true
			, MemoryPool::Alignment::Bits_8
			);
		REQUIRE(bool(error) == false);

		INFO("Allocate 3 Blocks");

		off_t block_1 = memory_pool.alloc(block, error);
		REQUIRE(block_1 >= 0);

		off_t block_2 = memory_pool.alloc(block, error);
		REQUIRE(block_2 >= 0);

		off_t block_3 = memory_pool.alloc(block, error);
		REQUIRE(block_3 >= 0);

		total_size = memory_pool.size();
		REQUIRE(total_size == (block * 3));

		segment = memory_pool.segmentList();
		REQUIRE(segment.size() == 3);
		REQUIRE((size_t)segment[0].size == block);
		REQUIRE((size_t)segment[0].in_use == true);
		REQUIRE((size_t)segment[1].size == block);
		REQUIRE((size_t)segment[1].in_use == true);
		REQUIRE((size_t)segment[2].size == block);
		REQUIRE((size_t)segment[2].in_use == true);

		// block_2 and block_3 should merge into 1 free block
		memory_pool.free(block_2);
		memory_pool.free(block_3);

		total_size = memory_pool.size();
		REQUIRE(total_size == (block * 3));

		segment = memory_pool.segmentList();
		REQUIRE(segment.size() == 2);
		REQUIRE((size_t)segment[0].size == block);
		REQUIRE((size_t)segment[0].in_use == true);
		REQUIRE((size_t)segment[1].size == (block * 2));
		REQUIRE((size_t)segment[1].in_use == false);

		block_2 = memory_pool.alloc(block, error);
		block_3 = memory_pool.alloc(block, error);

		total_size = memory_pool.size();
		REQUIRE(total_size == (block * 3));

		segment = memory_pool.segmentList();
		REQUIRE(segment.size() == 3);
		REQUIRE((size_t)segment[0].size == block);
		REQUIRE((size_t)segment[0].in_use == true);
		REQUIRE((size_t)segment[1].size == block);
		REQUIRE((size_t)segment[1].in_use == true);
		REQUIRE((size_t)segment[2].size == block);
		REQUIRE((size_t)segment[2].in_use == true);
	}
}
