/******************************************************************************
 * Copyright 2020-2022 Andrew Moore
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef zakero_MemZone_h
#define zakero_MemZone_h

/**
 * \file
 *
 * \brief Zakero MemZone
 *
|                       |                          |                          |                          |                         |                         |                             |
|-----------------------|--------------------------|--------------------------|--------------------------|-------------------------|-------------------------|-----------------------------|
| \api{zakero::MemZone} | \refdeps{zakero_memzone} | \reftldr{zakero_memzone} | \refwhat{zakero_memzone} | \refwhy{zakero_memzone} | \refhow{zakero_memzone} | \refversion{zakero_memzone} |
 *
 * Here, you will find information about and how to add the _Zakero_MemZone_ 
 * to your project.
 *
 *
 * \pardeps{zakero_memzone}
 * - None
 * \endpardeps
 *
 *
 * \partldr{zakero_memzone}
 * This library will provide a memory pool for your application.
 *
 * To use:
 * 1. Add the implementation to a source code file:
 *    \code
 *    #define ZAKERO_MEMZONE_IMPLEMENTATION
 *    #include "Zakero_MemZone.h"
 *    \endcode
 * 2. Add the library to where it is used:
 *    \code
 *    #include "Zakero_MemZone.h"
 *    \endcode
 * \endpartldr
 *
 *
 * \parwhat{zakero_memzone}
 * The _Zakero MemZone_ library will create and manage a region of memory.
 * From this pool of memory, sections of memory can be allocated and freed 
 * without the overhead of the malloc family of function or the `new` operator.
 *
 * When memory is allocated a unique identifier is returned. This identifier 
 * must be used to access and use the memory. The reason for using the 
 * identifier is to provide a layer of indirection which allows _MemZone_ to 
 * arrange the allocated memory as needed.
 * 
 * Programs are expected to be "good citizens" by not writing outside of their 
 * allocated area.
 * \endparwhat
 *
 *
 * \parwhy{zakero_memzone}
 * As with many things, there are benefits and draw backs to using a memory 
 * pool. For the MemZone object they are:
 * 
 * __Benefits__
 * - SPEED!!! Much faster allocations than `new` or `malloc`
 * - Data focused, allocations are based on size not object-type
 * - The entire memory pool can be easily shared across process-space
 * - Can automatically grow as needed [optional feature]
 * 
 * __Draw Backs__
 * - Requires extra work to convert identifiers to pointers
 * - No bounds checking in memory writes
 *
 * To put things into perspective, allocating memory is a very expensive 
 * operation. Using the MemZone means this operation only needs to happen once.  
 * Allocating memory from the MemZone is also fast because MemZone tracks the 
 * amount and location of free memory.
 *
 * MemZone can also use a Unix File Descriptor for its internal memory pool.  
 * This allows multiple processes to share the data in the same MemZone.
 *
 * Unless C++'s [Placement 
 * New](https://en.cppreference.com/w/cpp/language/new#Placement_new) is being 
 * used extensively, the developer must be fully aware of where they are in 
 * their area of the memory region so that other data is not over written. 
 *
 * Memory fragmentation already happens in most applications. The impact of the 
 * fragmentation is not felt due to huge amounts of memory in today's 
 * computers. However, for smaller memory pools, fragmentation becomes a larger 
 * issue.  Over the course of an application's life-time, memory allocation and 
 * freeing can occur many times. Evetually, a large block of memory is 
 * requested, that allocation may fail because a contiguous region of memory is 
 * not available. This is a common problem that plagues memory pools.
 *
 * To combat this problem, MemZone has de-fragmentation tools built-in which 
 * can be enabled automatically or called explicitly.
 * \endparwhy
 *
 *
 * \parhow{zakero_memzone}
 * __Step 0__
 *
 * Your compiler must support at least the C++20 standard. The location of the 
 * `Zakero_*.h` header files _must_ be in your compiler's include path.
 *
 * __Step 1__
 *
 * The first step is to select which C++ source code file will contain the 
 * _Zakero MemZone_ implementation. Once the location has been determined, add 
 * the following to that file:
 *
 * ~~~
 * #define ZAKERO_MEMZONE_IMPLEMENTATION
 * #include "Zakero_MemZone.h"
 * ~~~
 *
 * The macro \ref ZAKERO_MEMZONE_IMPLEMENTATION tells the header file to 
 * include the implementation of the MemZone.
 *
 * In all other files that will use the MemZone, they need to include the 
 * header.
 *
 * ~~~
 * #include "Zakero_MemZone.h"
 * ~~~
 *
 * __Step 2__
 *
 * After creating a [MemZone](\ref zakero::MemZone::MemZone), it must be 
 * [initialized](\ref zakero::MemZone::init()) before it can be used. Once that 
 * is done, you can freely [allocate](\ref zakero::MemZone::alloc()) and 
 * [free](\ref zakero::MemZone::free()) memory from the MemZone.
 *
 * This is an example of creating two std::array's that are backed by the 
 * MemZone.
 *
 * ~~~
 * zakero::MemZone memory_pool("Array_Data");
 *
 * constexpr size_t count = 100;
 * size_t array_size = sizeof(int64_t) * count;
 *
 * memory_pool.init(array_size * 2);
 *
 * off_t    offset = memory_pool.alloc(array_size);
 * int64_t* ptr    = memory_pool.addressOf(offset);
 *
 * std::array<int64_t,count>* array_1 = new(ptr) std::array<int64_t,count>();
 *
 * offset = memory_pool.alloc(array_size);
 * ptr    = memory_pool.addressOf(offset);
 *
 * std::array<int64_t,count>* array_2 = new(ptr) std::array<int64_t,count>();
 *
 * // Do stuff with array_1 and array_2
 * ~~~
 * \endparhow
 *
 *
 * \parversion{zakero_memzone}
 * __0.1.0__
 * - The initial version
 * \endparversion
 *
 *
 * \copyright [Mozilla Public License 
 * v2](https://www.mozilla.org/en-US/MPL/2.0/) 
 *
 *
 * \author Andrew "Zakero" Moore
 * - Original Author
 *
 *
 * \todo Add `todo` items
 */


/******************************************************************************
 * Includes
 */

// C++23
#include <cstring>
#include <stdint.h>
#include <system_error>

// POSIX
#include <sys/mman.h>
#include <unistd.h>


/******************************************************************************
 * Macros
 */

// {{{ Macros

/**
 * \internal
 *
 * \brief _Error_Table_
 *
 * An X-Macro table of error codes. The columns are:
 * -# __ErrorName__<br>
 *    The error name will be accessible as zakero::MemZone::_ErrorName_
 *    <br><br>
 * -# __ErrorValue__<br>
 *    The integer value of the error
 *    <br><br>
 * -# __ErrorMessage__<br>
 *    The text that will be used by `std::error_code.message()`
 *
 * Error Codes used internally.
 */
#define ZAKERO_MEMZONE__ERROR_DATA \
	X(Error_None                     ,  0 , "No Error"                                             ) \
	X(Error_Init_Failure_Name        ,  1 , "Failed to initialize the MemZone name"                ) \
	X(Error_Init_Failure_FD          ,  2 , "Failed to initialize the MemZone FD"                  ) \
	X(Error_Init_Failure_RAM         ,  3 , "Failed to initialize the MemZone RAM"                 ) \
	X(Error_Invalid_Parameter_Name   ,  4 , "The 'name' parameter is not valid"                    ) \
	X(Error_Invalid_Parameter_Size   ,  5 , "The 'size' parameter is not valid"                    ) \
	X(Error_Invalid_Parameter_Mode   ,  6 , "The 'mode' parameter is not valid"                    ) \
	X(Error_Invalid_Parameter_Id     ,  7 , "The 'id' parameter is not valid"                      ) \
	X(Error_Already_Initialized      ,  8 , "MemZone has already been initialized"                 ) \
	X(Error_Not_Enough_Memory        ,  9 , "Not enough memory is availalbe"                       ) \
	X(Error_Id_Is_In_Use             , 10 , "Can not free an Id that is in use"                    ) \
	X(Error_Not_Enough_Memory_Expand , 11 , "Not enough memory is availalbe and expanding failed"  ) \
	X(Error_Not_Enough_Memory_Defrag , 12 , "Not enough memory is availalbe and defragging failed" ) \

#define ZAKERO_KILOBYTE(val_) (val_ * 1024)
#define ZAKERO_MEGABYTE(val_) (ZAKERO_KILOBYTE(val_) * 1024)
#define ZAKERO_GIGABYTE(val_) (ZAKERO_MEGABYTE(val_) * 1024)

// }}}
// {{{ Declaration : C

extern "C"
{

#define X(name_, value_, mesg_) \
constexpr int Zakero_MemZone_ ## name_ = value_;
ZAKERO_MEMZONE__ERROR_DATA
#undef X

enum Zakero_MemZone_Block_Find
{	Zakero_MemZone_Block_Find_Forward
,	Zakero_MemZone_Block_Find_Backward
};

enum Zakero_MemZone_Block_Flag
{	Zakero_MemZone_Block_Flag_Allocated     = 0x0000'0000'0000'0001
,	Zakero_MemZone_Block_Flag_In_Use        = 0x0000'0000'0000'0010
,	Zakero_MemZone_Block_Flag_Clear_On_Free = 0x0000'0000'0000'0100
};

enum Zakero_MemZone_Defrag_
{	Zakero_MemZone_Defrag_On_Allocate = (1 << 0)
,	Zakero_MemZone_Defrag_On_Free     = (1 << 1)
,	Zakero_MemZone_Defrag_On_Acquire  = (1 << 2)
,	Zakero_MemZone_Defrag_On_Release  = (1 << 3)
,	Zakero_MemZone_Defrag_On_Any =
	(	Zakero_MemZone_Defrag_On_Allocate
	|	Zakero_MemZone_Defrag_On_Free
	|	Zakero_MemZone_Defrag_On_Acquire
	|	Zakero_MemZone_Defrag_On_Release
	)
};

// {{enum(name=Zakero_MemZone_Expand)
//   {{enum_member(name=Zakero_MemZone_Expand_None, value=0)
//     This instance of MemZone is not allowed to expand.
//   }}
//   {{enum_member(name=Zakero_MemZone_Expand_Auto, value=1)
//     This instance of MemZone will automatically expand if needed.
//     All allocated memory must be in a released state, otherwise
//     expansion will fail.
//   }}
// }}
enum Zakero_MemZone_Expand
{	Zakero_MemZone_Expand_None   = 0
,	Zakero_MemZone_Expand_To_Fit = 1
//,	Zakero_MemZone_Expand_10     = 10 /* 10% extra growth */
//,	Zakero_MemZone_Expand_33     = 3  /* 33% extra growth */
//,	Zakero_MemZone_Expand_50     = 2  /* 50% extra growth */
};

enum Zakero_MemZone_Mode : uint8_t
{	Zakero_MemZone_Mode_RAM = 0
,	Zakero_MemZone_Mode_FD
,	Zakero_MemZone_Mode_SHM
};


// Block can contain upto 8 bytes
// - This is not accounted for!
struct Zakero_MemZone_Block
{
	uint64_t              id;   // 0x00   0
	uint64_t              flag; // 0x08   8
	Zakero_MemZone_Block* next; // 0x10  16
	Zakero_MemZone_Block* prev; // 0x18  24
	uint64_t              size; // 0x20  32
};

#define zakero_memzone_block_data_addr_(block_) \
	((uint64_t)block_ + sizeof(Zakero_MemZone_Block))

#define zakero_memzone_block_data_(block_) \
	((uint8_t*)((uint64_t)block_ + sizeof(Zakero_MemZone_Block)))

struct Zakero_MemZone
{
	uint8_t* memory  = nullptr;
	size_t   size    = 0;
	uint64_t next_id = 0;

	// TODO: vvvv merge into a single uint64_t vvvv
	uint8_t  mode    = Zakero_MemZone_Mode_RAM;
	uint8_t  expand  = Zakero_MemZone_Expand_None;
	uint8_t  defrag  = 0;
	// TODO: ^^^^ merge into a single uint64_t ^^^^

	//char*  name    = nullptr;
	//int    fd      = -1;
};


[[]]          int      Zakero_MemZone_Init(Zakero_MemZone&, const Zakero_MemZone_Mode, const size_t) noexcept;
[[]]          void     Zakero_MemZone_Destroy(Zakero_MemZone&) noexcept;
[[nodiscard]] int      Zakero_MemZone_Allocate(Zakero_MemZone&, size_t, uint64_t&) noexcept;
[[nodiscard]] int      Zakero_MemZone_Free(Zakero_MemZone&, uint64_t id) noexcept;
[[]]          void     Zakero_MemZone_Defrag(Zakero_MemZone&) noexcept;
[[nodiscard]] size_t   Zakero_MemZone_Available_Largest(Zakero_MemZone&) noexcept;
[[nodiscard]] size_t   Zakero_MemZone_Available_Total(Zakero_MemZone&) noexcept;
[[nodiscard]] size_t   Zakero_MemZone_Used_Largest(Zakero_MemZone&) noexcept;
[[nodiscard]] size_t   Zakero_MemZone_Used_Total(Zakero_MemZone&) noexcept;
[[nodiscard]] size_t   Zakero_MemZone_Size_Of(Zakero_MemZone&, uint64_t) noexcept;

/*

int Zakero_MemZone_Init_From(Zakero_MemZone& //memzone
	, const Zakero_MemZone& //memzone
	) noexcept;

int Zakero_MemZone_Init_From_FD(Zakero_MemZone& //memzone
	, const int //fd
	) noexcept;

void* Zakero_MemZone_Acquire(Zakero_MemZone& memzone
	, uint64_t id
	)
void Zakero_MemZone_Release(Zakero_MemZone& memzone
	, uint64_t id
	)
size_t Zakero_MemZone_Realloc(Zakero_MemZone& memzone
	, uint64_t id
	, size_t size
	)
block_clear
block_init
block_move
block_merge
block_split
block_swap
block_find_id
block_find_free
block_next
block_prev
block_size
block_is_free
block_id
block_id_set
block_allocated
block_allocated_set
block_acquired
block_acquired_set
 */

} // extern "C"

// }}}
// {{{ Declaration : C++
namespace zakero
{
	// {{{ Declaration

	class MemZone
	{
		public:
#define X(name_, val_, mesg_) \
			static constexpr int name_ = val_;
			ZAKERO_MEMZONE__ERROR_DATA
#undef X
			static constexpr size_t Size_Max = (size_t)std::numeric_limits<off_t>::max();

			enum class Alignment : uint8_t
			{	Bits_8  = 0
			,	Bits_16 = 1
			,	Bits_32 = 3
			,	Bits_64 = 7
			,	Byte_1 = Bits_8
			,	Byte_2 = Bits_16
			,	Byte_4 = Bits_32
			,	Byte_8 = Bits_64
			};

			MemZone() noexcept;
			~MemZone() noexcept;

		private:

			// -------------------------------------------------- //

			// -------------------------------------------------- //

			MemZone(const MemZone&) = delete;
			MemZone& operator=(const MemZone&) = delete;
	};

	// }}}
	// {{{ Convenience
	// }}}
}
// }}}
// {{{ Implementation

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION

// {{{ Macros
// {{{ Macros : Doxygen

#ifdef ZAKERO__DOXYGEN_DEFINE_DOCS

// Only used for generating Doxygen documentation

/**
 * \brief Activate the implementation code.
 *
 * Defining this macro will cause the zakero::MemZone implementation to be 
 * included.  This should only be done once, since compiler and/or linker 
 * errors will typically be generated if more than a single implementation is 
 * found.
 *
 * \note It does not matter if the macro is given a value or not, only its 
 * existence is checked.
 */
#define ZAKERO_MEMZONE_IMPLEMENTATION

#endif

// }}}

/**
 * \internal
 *
 * \brief Create an error condition.
 *
 * This is just a convenience macro.
 *
 * \param err_ The name of an error from the Error Table
 */
#define ZAKERO_MEMZONE__ERROR(err_) std::error_code(err_, MemZoneErrorCategory);

#ifdef ZAKERO_MEMZONE_DEBUG_ENABLED
	/**
	 * \brief A flag to determine if debugging is disabled or enabled.
	 */
	#define ZAKERO_MEMZONE_DEBUG_IS_ENABLED true
	#define ZAKERO_MEMZONE_LOG_ERROR(fmt_, ...) \
	{ \
		fprintf(stderr, "%s:%u:%s:", __FILE__, __LINE__, __PRETTY_FUNCTION__); \
		fprintf(stderr, fmt_ __VA_OPT__(,) __VA_ARGS__); \
		fprintf(stderr, "\n"); \
		fflush(stderr); \
	}
#else
	/**
	 * \brief A flag to determine if debugging is disabled or enabled.
	 */
	#define ZAKERO_MEMZONE_DEBUG_IS_ENABLED false
	#define ZAKERO_MEMZONE_LOG_ERROR(fmt_, ...) {}
#endif // ZAKERO_MEMZONE_DEBUG_ENABLED

#ifdef ZAKERO_MEMZONE_VALIDATE_ENABLED
	/**
	 * \brief A flag to determine if parameter validation is disabled or 
	 * enabled.
	 */
	#define ZAKERO_MEMZONE_VALIDATE_IS_ENABLED true
#else
	/**
	 * \brief A flag to determine if parameter validation is disabled or 
	 * enabled.
	 */
	#define ZAKERO_MEMZONE_VALIDATE_IS_ENABLED false
#endif // ZAKERO_MEMZONE_VAIDATE_ENABLED

// }}}
// {{{ Implementation : C -

// {{{ Implementation : C : round_to_64bit() -

static inline size_t round_to_64bit(size_t size
	) noexcept
{
	return ((size + 7) & ~0x07);
}

// }}}
// {{{ Implementation : C : block_init_() -

static void block_init_(Zakero_MemZone_Block* block
	, size_t size
	, Zakero_MemZone_Block* prev
	, Zakero_MemZone_Block* next
	) noexcept
{
	block->id   = 0;
	block->flag = 0;
	block->next = next;
	block->prev = prev;
	block->size = size;
}

// }}}
// {{{ Implementation : C : memzone_block_first_() -

static inline Zakero_MemZone_Block* memzone_block_first_(Zakero_MemZone& memzone
	) noexcept
{
	Zakero_MemZone_Block* block = (Zakero_MemZone_Block*)memzone.memory;

	return block;
}

// }}}
// {{{ Implementation : C : memzone_block_last_() -

static Zakero_MemZone_Block* memzone_block_last_(Zakero_MemZone& memzone
	) noexcept
{
	Zakero_MemZone_Block* block = memzone_block_first_(memzone);

	while(block->next != nullptr)
	{
		block = block->next;
	}

	return block;
}

// }}}
// {{{ Implementation : C : block_is_in_use_() -

static inline bool block_is_in_use_(const Zakero_MemZone_Block* block
	) noexcept
{
	return (bool)(block->flag & Zakero_MemZone_Block_Flag_In_Use);
}

// }}}
// {{{ Implementation : C : block_find_in_use_() -

static Zakero_MemZone_Block* block_find_in_use_(Zakero_MemZone_Block* block
	, Zakero_MemZone_Block_Find direction
	) noexcept
{
	if(direction == Zakero_MemZone_Block_Find_Forward)
	{
		while(block != nullptr)
		{
			if(block_is_in_use_(block) == true)
			{
				return block;
			}

			block = block->next;
		}

		return block;
	}
	else if(direction == Zakero_MemZone_Block_Find_Backward)
	{
		while(block != nullptr)
		{
			if(block_is_in_use_(block) == true)
			{
				return block;
			}

			block = block->prev;
		}

		return block;
	}

	return nullptr;
}

// }}}
// {{{ Implementation : C : block_is_free_() -

static inline bool block_is_free_(const Zakero_MemZone_Block* block
	) noexcept
{
	return ((block->flag & Zakero_MemZone_Block_Flag_Allocated) == 0);
}

// }}}
// {{{ Implementation : C : memzone_expand_ram_() -

static Zakero_MemZone_Block* memzone_expand_ram_(Zakero_MemZone& memzone
	, size_t size
	) noexcept
{
	size_t memzone_size = memzone.size + size;
	bool   append       = false;

	Zakero_MemZone_Block* block = memzone_block_last_(memzone);
	if(block_is_free_(block) == true)
	{
		append        = true;
		memzone_size -= block->size;
	}
	else
	{
		memzone_size += sizeof(Zakero_MemZone_Block);
	}

	uint8_t* memory = (uint8_t*)realloc(memzone.memory, memzone_size);
	if(memory == nullptr)
	{
		return nullptr;
	}

	memzone.size = memzone_size;

	if(memzone.memory != memory)
	{
		memzone.memory = memory;
		block = memzone_block_last_(memzone);
	}

	if(append == true)
	{
		block->size = size;
	}
	else
	{
		block->next = (Zakero_MemZone_Block*)
			(zakero_memzone_block_data_addr_(block) + block->size);

		block_init_(block->next, size, block, nullptr);
		block = block->next;
	}

	memset(zakero_memzone_block_data_(block), 1, block->size);

	return block;
}

// }}}
// {{{ Implementation : C : memzone_expand_fd_() -

static Zakero_MemZone_Block* memzone_expand_fd_(Zakero_MemZone& //memzone
	, size_t //size
	) noexcept
{
	fprintf(stderr, "%s: Not Implemented\n", __PRETTY_FUNCTION__);
	return nullptr;
}

// }}}
// {{{ Implementation : C : memzone_expand_shm_() -

static Zakero_MemZone_Block* memzone_expand_shm_(Zakero_MemZone& //memzone
	, size_t //size
	) noexcept
{
	fprintf(stderr, "%s: Not Implemented\n", __PRETTY_FUNCTION__);
	return nullptr;
}

// }}}
// {{{ Implementation : C : memzone_expand_() -

static Zakero_MemZone_Block* memzone_expand_(Zakero_MemZone& memzone
	, const size_t size
	) noexcept
{
	Zakero_MemZone_Block* block = memzone_block_first_(memzone);
	block = block_find_in_use_(block, Zakero_MemZone_Block_Find_Forward);

	if(block != nullptr)
	{
		// A block is in use, can not expand.
		return nullptr;
	}

	switch(memzone.mode)
	{
		case Zakero_MemZone_Mode_RAM:
			block = memzone_expand_ram_(memzone, size);
			break;
		case Zakero_MemZone_Mode_FD:
			block = memzone_expand_fd_(memzone, size);
			break;
		case Zakero_MemZone_Mode_SHM:
			block = memzone_expand_shm_(memzone, size);
			break;
	}

	return block;
}

// }}}
// {{{ Implementation : C : memzone_next_id_() -

static uint64_t memzone_next_id_(Zakero_MemZone& memzone
	) noexcept
{
	uint64_t id = memzone.next_id;

	memzone.next_id++;

	return id;
}

// }}}
// {{{ Implementation : C : block_dump_() -
#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

[[maybe_unused]]
static void block_dump_(const Zakero_MemZone_Block* block
	) noexcept
{
	printf("---------------------------------------\n");
	printf("Block: %lu %016lx <- %016lx -> %016lx\n"
		, block->id
		, (uint64_t)block->prev
		, (uint64_t)block
		, (uint64_t)block->next
		);
	printf("Block Flag  : %016lx\n", block->flag);
	printf("Block Data  : %016lx (%lu)\n", (uint64_t)zakero_memzone_block_data_(block), block->size);
	printf("Block End   : %016lx\n", (uint64_t)(zakero_memzone_block_data_(block)) + block->size);
}

#endif // }}}
// }}}
// {{{ Implementation : C : block_is_allocated_() -

static inline bool block_is_allocated_(const Zakero_MemZone_Block* block
	) noexcept
{
	return (bool)(block->flag & Zakero_MemZone_Block_Flag_Allocated);
}

// }}}
// {{{ Implementation : C : block_is_clear_on_free_() -

static inline bool block_is_clear_on_free_(const Zakero_MemZone_Block* block
	) noexcept
{
	return (bool)(block->flag & Zakero_MemZone_Block_Flag_Clear_On_Free);
}

// }}}
// {{{ Implementation : C : memzone_is_defrag_on_allocate_() -

static inline bool memzone_is_defrag_on_allocate_(const Zakero_MemZone& memzone
	) noexcept
{
	return (bool)(memzone.defrag & Zakero_MemZone_Defrag_On_Allocate);
}

// }}}
// {{{ Implementation : C : block_find_id_() -

static Zakero_MemZone_Block* block_find_id_(Zakero_MemZone_Block* block
	, uint64_t                  id
	, Zakero_MemZone_Block_Find direction
	) noexcept
{
	if(direction == Zakero_MemZone_Block_Find_Forward)
	{
		while(block != nullptr)
		{
			if(block->id == id)
			{
				return block;
			}

			block = block->next;
		}

		return block;
	}
	else if(direction == Zakero_MemZone_Block_Find_Backward)
	{
		while(block != nullptr)
		{
			if(block->id == id)
			{
				return block;
			}

			block = block->prev;
		}

		return block;
	}

	return nullptr;
}

// }}}
// {{{ Implementation : C : block_find_free_() -

static Zakero_MemZone_Block* block_find_free_(Zakero_MemZone_Block* block
	, size_t                    size
	, Zakero_MemZone_Block_Find direction
	) noexcept
{
	if(direction == Zakero_MemZone_Block_Find_Forward)
	{
		while(block != nullptr)
		{
			if(block_is_free_(block) == true)
			{
				if(block->size >= size)
				{
					return block;
				}
			}

			block = block->next;
		}

		return block;
	}
	else if(direction == Zakero_MemZone_Block_Find_Backward)
	{
		while(block != nullptr)
		{
			if(block_is_free_(block) == true)
			{
				if(block->size >= size)
				{
					return block;
				}
			}

			block = block->prev;
		}

		return block;
	}

	return nullptr;
}

// }}}
// {{{ Implementation : C : block_find_last_() -

static Zakero_MemZone_Block* block_find_last_(Zakero_MemZone_Block* block
	) noexcept
{
	while(block->next != nullptr)
	{
		block = block->next;
	}

	return block;
}

// }}}
// {{{ Implementation : C : block_find_last_allocated_() -

static Zakero_MemZone_Block* block_find_last_allocated_(Zakero_MemZone_Block* block
	) noexcept
{
	Zakero_MemZone_Block* block_stop = block;

	block = block_find_last_(block);

	while(block_is_allocated_(block) == false)
	{
		block = block->prev;

		if(block == nullptr)
		{
			return nullptr;
		}

		if(block == block_stop)
		{
			return nullptr;
		}
	}

	return block;
}

// }}}
// {{{ Implementation : C : block_merge_with_next_() -

static void block_merge_with_next_(Zakero_MemZone_Block* block
	) noexcept
{
	Zakero_MemZone_Block* block_next = block->next;

	block->next  = block_next->next;
	if(block_next->next != nullptr)
	{
		block_next->next->prev = block;
	}

	block->size += block_next->size + sizeof(Zakero_MemZone_Block);
}

// }}}
// {{{ Implementation : C : block_merge_free_() -

static Zakero_MemZone_Block* block_merge_free_(Zakero_MemZone_Block* block
	) noexcept
{
	if((block->next != nullptr)
		&& (block_is_free_(block->next) == true)
		)
	{
		block_merge_with_next_(block);
	}

	if((block->prev != nullptr)
		&& (block_is_free_(block->prev) == true)
		)
	{
		block = block->prev;
		block_merge_with_next_(block);
	}

	return block;
}

// }}}
// {{{ Implementation : C : block_move_() -

static void block_move_(Zakero_MemZone_Block* block_src
	, Zakero_MemZone_Block* block_dst
	) noexcept
{
	memcpy(zakero_memzone_block_data_(block_dst)
		, zakero_memzone_block_data_(block_src)
		, block_dst->size
		);

	block_dst->id   = block_src->id;
	block_dst->flag = block_src->flag;

	if(block_dst->size > block_src->size)
	{
		memset((void*)(zakero_memzone_block_data_addr_(block_dst) + block_src->size)
			, 0
			, block_dst->size - block_src->size
			);
	}

	block_src->id   = 0;
	block_src->flag = 0;
	block_merge_free_(block_src);
}

// }}}
// {{{ Implementation : C : block_split_() -

static Zakero_MemZone_Block* block_split_(Zakero_MemZone_Block* block
	, size_t size
	) noexcept
{
	Zakero_MemZone_Block* block_next = nullptr;

	block_next = (Zakero_MemZone_Block*)((uint64_t)block + sizeof(Zakero_MemZone_Block) + size);
	block_next->size = block->size - (sizeof(Zakero_MemZone_Block) + size);

	block->size = size;

	block_next->id   = 0;
	block_next->flag = 0;
	block_next->next = block->next;
	block_next->prev = block;

	if(block_next->next != nullptr)
	{
		block_next->next->prev = block_next;
	}

	block->next = block_next;

	return block_next;
}

// }}}
// {{{ Implementation : C : block_swap_with_next_() -

static Zakero_MemZone_Block* block_swap_with_next_(Zakero_MemZone_Block* block
	) noexcept
{
	Zakero_MemZone_Block* block_next = block->next;
	Zakero_MemZone_Block  block_temp = *block;
		
	*block = *block_next;
	memmove(zakero_memzone_block_data_(block)
		, zakero_memzone_block_data_(block_next)
		, block_next->size
		);

	block_next = (Zakero_MemZone_Block*)((uint64_t)block + sizeof(Zakero_MemZone_Block) + block->size);
	*block_next = block_temp;
	memset(zakero_memzone_block_data_(block_next)
		, 0
		, block_next->size
		);

	block_next->prev = block;
	block_next->next = block->next;

	block->prev = block_temp.prev;
	block->next = block_next;

	return block_next;
}

// }}}
// {{{ Implementation : C : defrag_pass_() -

// find first "free" block
// find last "allocated" block
// find an allocated block that fits in the free block from end
// - if found
//   - split the free block
//   - move allocated block
// - else not found
//   - move next block into free block
// - merge old allocated block
// - return next free block

static Zakero_MemZone_Block* defrag_pass_(Zakero_MemZone_Block* block
	) noexcept
{
	Zakero_MemZone_Block* block_free = nullptr;
	block_free = block_find_free_(block_free, 0, Zakero_MemZone_Block_Find_Forward);

	if(block == nullptr)
	{
		return nullptr;
	}

	Zakero_MemZone_Block* block_temp    = block_find_last_allocated_(block_free);
	Zakero_MemZone_Block* block_to_move = nullptr;

	while(block_temp > block_free)
	{
		if(block_temp->size <= block_free->size)
		{
			if(block_to_move == nullptr
				|| block_to_move->size < block_temp->size
				)
			{
				block_to_move = block_temp;
			}
		}

		block_temp = block_temp->prev;
	}

	if(block_to_move != nullptr)
	{
		block_temp = block_split_(block_free, block_to_move->size);
		block_move_(block_to_move, block_free);
		block_free = block_temp;
	}
	else
	{
		block_free = block_swap_with_next_(block_free);
		block_free = block_merge_free_(block_free);
	}

	return block_free;
}

// }}}
// {{{ Implementation : C : defrag_() -

static void defrag_(Zakero_MemZone& memzone
	, uint64_t defrag_passes
	) noexcept
{
	Zakero_MemZone_Block* block = memzone_block_first_(memzone);

	if(defrag_passes == 0)
	{
		do
		{
			block = defrag_pass_(block);
		} while(block != nullptr);
	}
	else
	{
		do
		{
			block = defrag_pass_(block);
			defrag_passes--;
		} while(block != nullptr && defrag_passes > 0);
	}
}

// }}}
// {{{ Implementation : C : destroy_fd_() -

#ifdef __linux__

static void destroy_fd_(Zakero_MemZone& memzone
	) noexcept
{
}

#elif __HAIKU__

static void destroy_fd_(Zakero_MemZone& memzone
	) noexcept
{
}

#else
#	error "destroy_fd_()" has not been implemented yet!
#endif

// }}}
// {{{ Implementation : C : destroy_ram_() -

#if defined __linux__ \
	|| __HAIKU__

static void destroy_ram_(Zakero_MemZone& memzone
	) noexcept
{
	memset(memzone.memory, 0, memzone.size);
	free(memzone.memory);
}

#else
#	error "destroy_ram_()" has not been implemented yet!
#endif

// }}}
// {{{ Implementation : C : destroy_shm_() -

#ifdef __linux__

static void destroy_shm_(Zakero_MemZone& memzone
	) noexcept
{
}

#elif __HAIKU__

static void destroy_shm_(Zakero_MemZone& memzone
	) noexcept
{
}

#else
#	error "destroy_shm_()" has not been implemented yet!
#endif

// }}}
// {{{ Implementation : C : init_fd_() -

#ifdef __linux__

static void init_fd_(Zakero_MemZone& memzone
	) noexcept
{
	//memzone.fd = memfd_create(memzone.name, 0);
}

#elif __HAIKU__

static void init_fd_(Zakero_MemZone& memzone
	) noexcept
{
}

#else
#	error "init_fd_()" has not been implemented yet!
#endif

// }}}
// {{{ Implementation : C : init_ram_() -

#if defined __linux__ \
	|| defined __HAIKU__

static void init_ram_(Zakero_MemZone& memzone
	) noexcept
{
	memzone.memory = (uint8_t*)calloc(memzone.size, sizeof(uint8_t));
}

#else
#	error "init_ram_()" has not been implemented yet!
#endif

// }}}
// {{{ Implementation : C : mode_is_supported_() -

#ifdef __linux__

static bool mode_is_supported_(const Zakero_MemZone_Mode
	) noexcept
{
	return true;
}

#elif __HAIKU__

//[[maybe_unused]]
static bool mode_is_supported_(const Zakero_MemZone_Mode mode
	) noexcept
{
	switch(mode)
	{
		case Zakero_MemZone_Mode_FD:
			return false;

		case Zakero_MemZone_Mode_RAM:
			return true;

		case Zakero_MemZone_Mode_SHM:
			return false;
	};

	return false;
}

#else
#	error "mode_is_supported_()" has not been implemented yet!
#endif

// }}}

// {{{ Implementation : C : Zakero_MemZone_Init() -

int Zakero_MemZone_Init(Zakero_MemZone& memzone
	, const Zakero_MemZone_Mode mode
	, const size_t              size
	, Zakero_MemZone_Expand     expand
	, uint8_t                   defrag
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory != nullptr)
	{
		return Zakero_MemZone_Error_Already_Initialized;
	}

	if(size == 0)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'size' must be greater than 0");
		return Zakero_MemZone_Error_Invalid_Parameter_Size;
	}

	if(mode_is_supported_(mode) == false)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'mode' has unsupported value");
		return Zakero_MemZone_Error_Invalid_Parameter_Mode;
	}
#endif

	const size_t block_size = round_to_64bit(size);

	memzone.size    = sizeof(Zakero_MemZone_Block) + block_size;
	memzone.next_id = 1;
	memzone.expand  = expand;
	memzone.defrag  = defrag;

	switch(mode)
	{
		case Zakero_MemZone_Mode_RAM:
			init_ram_(memzone);
			if(memzone.memory == nullptr)
			{
				return Zakero_MemZone_Error_Init_Failure_RAM;
			}

			break;

		case Zakero_MemZone_Mode_FD:
			return Zakero_MemZone_Error_Invalid_Parameter_Mode;
			/*
			if(name == nullptr)
			{
				ZAKERO_MEMZONE_LOG_ERROR("Parameter 'name' can not NULL");
				return Zakero_MemZone_Error_Invalid_Parameter_Name;
			}

			if(strlen(name) == 0)
			{
				ZAKERO_MEMZONE_LOG_ERROR("Parameter 'name' can not be empty");
				return Zakero_MemZone_Error_Invalid_Parameter_Name;
			}

			memzone.name = strdup(name);
			if(memzone.name == nullptr)
			{
				ZAKERO_MEMZONE_LOG_ERROR("Failed to allocate memory for the name");
				return Zakero_MemZone_Error_Init_Failure_Name;
			}
			*/

			init_fd_(memzone);
			//if(memzone.fd == -1)
			//{
			//	Zakero_MemZone_Destroy(memzone);
			//	return Zakero_MemZone_Error_Init_Failure_FD;
			//}

			break;

		case Zakero_MemZone_Mode_SHM:
			return Zakero_MemZone_Error_Invalid_Parameter_Mode;
			break;
	}

	Zakero_MemZone_Block* block = memzone_block_first_(memzone);

	block_init_(block
		, block_size
		, nullptr
		, nullptr
		);

	return Zakero_MemZone_Error_None;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/init/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;

#if __HAIKU__ // {{{ Invalid Mode
	SUBCASE("Invalid Mode: Zakero_MemZone_Mode_FD") // {{{
	{
		error = Zakero_MemZone_Init(memzone
			, Zakero_MemZone_Mode_FD
			, ZAKERO_MEGABYTE(1)
			, Zakero_MemZone_Expand_None
			, 0 // Defrag Disabled
			);

		CHECK(error == Zakero_MemZone_Error_Invalid_Parameter_Mode);
	} // }}}
	SUBCASE("Invalid Mode: Zakero_MemZone_Mode_SHM") // {{{
	{
		error = Zakero_MemZone_Init(memzone
			, Zakero_MemZone_Mode_SHM
			, ZAKERO_MEGABYTE(1)
			, Zakero_MemZone_Expand_None
			, 0 // Defrag Disabled
			);

		CHECK(error == Zakero_MemZone_Error_Invalid_Parameter_Mode);
	} // }}}
#endif // }}}
#if __linux__ // {{{ Invalid Mode
	// No Invalid Modes
#endif // }}}
	SUBCASE("Invalid Size: 0") // {{{
	{
		error = Zakero_MemZone_Init(memzone
			, Zakero_MemZone_Mode_RAM
			, 0 // Size
			, Zakero_MemZone_Expand_None
			, 0 // Defrag Disabled
			);

		CHECK(error == Zakero_MemZone_Error_Invalid_Parameter_Size);
	} // }}}
	SUBCASE("64-bit Rounding: 1 -> 8") // {{{
	{
		error = Zakero_MemZone_Init(memzone
			, Zakero_MemZone_Mode_RAM
			, 1 // Size
			, Zakero_MemZone_Expand_None
			, 0 // Defrag Disabled
			);

		CHECK(error == Zakero_MemZone_Error_None);
		CHECK(Zakero_MemZone_Available_Largest(memzone) == 8);
		CHECK(Zakero_MemZone_Available_Total(memzone)   == 8);

		Zakero_MemZone_Destroy(memzone);
	} // }}}
	SUBCASE("64-bit Rounding: 7 -> 8") // {{{
	{
		error = Zakero_MemZone_Init(memzone
			, Zakero_MemZone_Mode_RAM
			, 7 // Size
			, Zakero_MemZone_Expand_None
			, 0 // Defrag Disabled
			);

		CHECK(error == Zakero_MemZone_Error_None);
		CHECK(Zakero_MemZone_Available_Largest(memzone) == 8);
		CHECK(Zakero_MemZone_Available_Total(memzone)   == 8);

		Zakero_MemZone_Destroy(memzone);
	} // }}}
	SUBCASE("64-bit Rounding: 10 -> 16") // {{{
	{
		error = Zakero_MemZone_Init(memzone
			, Zakero_MemZone_Mode_RAM
			, 10 // Size
			, Zakero_MemZone_Expand_None
			, 0 // Defrag Disabled
			);

		CHECK(error == Zakero_MemZone_Error_None);
		CHECK(Zakero_MemZone_Available_Largest(memzone) == 16);
		CHECK(Zakero_MemZone_Available_Total(memzone)   == 16);

		Zakero_MemZone_Destroy(memzone);
	} // }}}
	SUBCASE("Already Initialized") // {{{
	{
		error = Zakero_MemZone_Init(memzone
			, Zakero_MemZone_Mode_RAM
			, ZAKERO_MEGABYTE(1)
			, Zakero_MemZone_Expand_None
			, 0 // Defrag Disabled
			);

		CHECK(error == Zakero_MemZone_Error_None);

		error = Zakero_MemZone_Init(memzone
			, Zakero_MemZone_Mode_RAM
			, ZAKERO_MEGABYTE(1)
			, Zakero_MemZone_Expand_None
			, 0 // Defrag Disabled
			);

		CHECK(error == Zakero_MemZone_Error_Already_Initialized);

		Zakero_MemZone_Destroy(memzone);
	} // }}}
} // }}}
TEST_CASE("/c/init/fd/") // {{{
{
	/*
	if(mode_is_supported_(Zakero_MemZone_Mode_FD) == false)
	{
		return;
	}

	//const char*    name    = "MemZone_c_init_fd";
	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_FD
		, ZAKERO_MEGABYTE(1)
		, Zakero_MemZone_Expand_None
		, 0 // Defrag Disabled
		);

	CHECK(error == Zakero_MemZone_Error_None);
	//CHECK(strcmp(memzone.name, name) == 0);
	//CHECK(memzone.fd != -1);
	//CHECK(memzone.memory != nullptr);

	SUBCASE("Already initialized")
	{
	}
	*/
} // }}}
TEST_CASE("/c/init/ram/") // {{{
{
	if(mode_is_supported_(Zakero_MemZone_Mode_RAM) == false)
	{
		return;
	}

	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(memzone
		 ,Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		, Zakero_MemZone_Expand_None
		, 0 // Defrag Disabled
		);

	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(memzone.memory != nullptr);

	Zakero_MemZone_Destroy(memzone);
} // }}}
TEST_CASE("/c/init/shm/") // {{{
{
	if(mode_is_supported_(Zakero_MemZone_Mode_SHM) == false)
	{
		return;
	}

} // }}}

#endif // }}}

// }}}
// {{{ Implementation : C : Zakero_MemZone_Destroy() -

void Zakero_MemZone_Destroy(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif

	switch(memzone.mode)
	{
		case Zakero_MemZone_Mode_FD:
			destroy_fd_(memzone);
			break;
		case Zakero_MemZone_Mode_RAM:
			destroy_ram_(memzone);
			break;
		case Zakero_MemZone_Mode_SHM:
			destroy_shm_(memzone);
			break;
	}

	memzone.memory  = nullptr;
	memzone.size    = 0;
	memzone.next_id = 0;
	memzone.mode    = 0;
	memzone.expand  = 0;
	memzone.defrag  = 0;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/destroy/ram/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		, Zakero_MemZone_Expand_None
		, 0 // Defrag Disabled
		);

	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(memzone.memory != nullptr);

	Zakero_MemZone_Destroy(memzone);

	CHECK(memzone.memory  == nullptr);
	CHECK(memzone.size    == 0);
	CHECK(memzone.next_id == 0);
	CHECK(memzone.mode    == 0);
	CHECK(memzone.expand  == 0);
	CHECK(memzone.defrag  == 0);
} // }}}
TEST_CASE("/c/destroy/fd/") // {{{
{
#if __linux__
	/*
	//const char*    name    = "MemZone_c_init_fd";
	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_FD
		, ZAKERO_MEGABYTE(1)
		, Zakero_MemZone_Expand_None
		, 0 // Defrag Disabled
		);

	CHECK(error == Zakero_MemZone_Error_None);
	//CHECK(strcmp(memzone.name, name) == 0);
	//CHECK(memzone.fd != -1);
	//CHECK(memzone.memory != nullptr);

	SUBCASE("Already initialized")
	{
	}
	*/
#endif
} // }}}
TEST_CASE("/c/destroy/shm/") // {{{
{
} // }}}

#endif // }}}

// }}}
// {{{ Implementation : C : Zakero_MemZone_Allocate() -

int Zakero_MemZone_Allocate(Zakero_MemZone& memzone
	, const size_t size
	, uint64_t&    id
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}

	if(size < 8)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'size' must be greater than or equal to 8");
		return Zakero_MemZone_Error_Invalid_Parameter_Size;
	}
#endif

	size_t block_size = round_to_64bit(size);
	int    error_code = Zakero_MemZone_Error_Not_Enough_Memory;

	Zakero_MemZone_Block* block = memzone_block_first_(memzone);
	block = block_find_free_(block, block_size, Zakero_MemZone_Block_Find_Forward);

	if((block == nullptr)
		&& (memzone.defrag != 0)
		)
	{
		defrag_(memzone, 0);

		block = memzone_block_first_(memzone);
		block = block_find_free_(block, block_size, Zakero_MemZone_Block_Find_Forward);

		if(block == nullptr)
		{
			error_code = Zakero_MemZone_Error_Not_Enough_Memory_Defrag;
		}
	}

	if((block == nullptr)
		&& (memzone.expand != Zakero_MemZone_Expand_None)
		)
	{
		block = memzone_expand_(memzone, block_size);

		if(block == nullptr)
		{
			error_code = Zakero_MemZone_Error_Not_Enough_Memory_Expand;
		}
	}

	if(block == nullptr)
	{
		return error_code;
	}

	if((block->size - block_size) > sizeof(Zakero_MemZone_Block))
	{
		block_split_(block, block_size);
	}

	id          = memzone_next_id_(memzone);
	block->id   = id;
	block->flag = Zakero_MemZone_Block_Flag_Allocated;

	if(memzone_is_defrag_on_allocate_(memzone) == true)
	{
		defrag_(memzone, 1);
	}

	return Zakero_MemZone_Error_None;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/allocate/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;
	uint64_t       id      = 0;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_KILOBYTE(1)
		, Zakero_MemZone_Expand_None
		, 0 // Defrag Disabled
		);

	CHECK(error == Zakero_MemZone_Error_None);

	SUBCASE("Invalid Size: 0") // {{{
	{
		error = Zakero_MemZone_Allocate(memzone
			, 0
			, id
			);

		CHECK(error == Zakero_MemZone_Error_Invalid_Parameter_Size);
	} // }}}
	SUBCASE("Not Enough Memory") // {{{
	{
		error = Zakero_MemZone_Allocate(memzone
			, ZAKERO_MEGABYTE(2)
			, id
			);

		CHECK(error == Zakero_MemZone_Error_Not_Enough_Memory);
	} // }}}

	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_KILOBYTE(1)
		, id
		);

	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(id    != 0);
	CHECK(ZAKERO_KILOBYTE(1) <= Zakero_MemZone_Size_Of(memzone, id));

	Zakero_MemZone_Destroy(memzone);
} // }}}
TEST_CASE("/c/allocate/expand/to-fit/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_KILOBYTE(1)
		, Zakero_MemZone_Expand_To_Fit
		, 0 // Defrag Disabled
		);

	CHECK(error == Zakero_MemZone_Error_None);

	uint64_t id_1 = 0;
	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_KILOBYTE(2)
		, id_1
		);

	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(id_1  != 0);
	CHECK(ZAKERO_KILOBYTE(2) <= Zakero_MemZone_Used_Total(memzone));
	CHECK(ZAKERO_KILOBYTE(0) <= Zakero_MemZone_Available_Total(memzone));
	CHECK(ZAKERO_KILOBYTE(2) <= Zakero_MemZone_Size_Of(memzone, id_1));

	error = Zakero_MemZone_Free(memzone, id_1);

	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_KILOBYTE(1)
		, id_1
		);

	uint64_t id_2 = 0;
	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_KILOBYTE(1)
		, id_2
		);

	uint64_t id_3 = 0;
	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_KILOBYTE(1)
		, id_3
		);

	CHECK(ZAKERO_KILOBYTE(3) <= Zakero_MemZone_Used_Total(memzone));
	CHECK(ZAKERO_KILOBYTE(0) <= Zakero_MemZone_Available_Total(memzone));
	CHECK(ZAKERO_KILOBYTE(1) <= Zakero_MemZone_Size_Of(memzone, id_1));
	CHECK(ZAKERO_KILOBYTE(1) <= Zakero_MemZone_Size_Of(memzone, id_2));
	CHECK(ZAKERO_KILOBYTE(1) <= Zakero_MemZone_Size_Of(memzone, id_3));

	Zakero_MemZone_Destroy(memzone);
} // }}}
TEST_CASE("/c/allocate/expand/10/") // {{{
{
} // }}}
TEST_CASE("/c/allocate/expand/33/") // {{{
{
} // }}}
TEST_CASE("/c/allocate/expand/50/") // {{{
{
} // }}}
// TEST_CASE("/c/allocate/defrag-trigger")
// TEST_CASE("/c/allocate/defrag-any")
// TEST_CASE("/c/allocate/defrag-none")

#endif // }}}

// }}}
// {{{ Implementation : C : Zakero_MemZone_Free() -

int Zakero_MemZone_Free(Zakero_MemZone& memzone
	, uint64_t id
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif

	Zakero_MemZone_Block* block = memzone_block_first_(memzone);
	block = block_find_id_(block, id, Zakero_MemZone_Block_Find_Forward);
	if(block == nullptr)
	{
		return Zakero_MemZone_Error_Invalid_Parameter_Id;
	}

	if(block_is_in_use_(block) == true)
	{
		return Zakero_MemZone_Error_Id_Is_In_Use;
	}

	if(block_is_clear_on_free_(block) == true)
	{
		memset(zakero_memzone_block_data_(block), 0, block->size);
	}

	block->id   = 0;
	block->flag = 0;
	block_merge_free_(block);

	return Zakero_MemZone_Error_None;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/free/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;
	uint64_t       id      = 0;
	size_t         size    = 0;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		, Zakero_MemZone_Expand_None
		, 0 // Defrag Disabled
		);

	CHECK(error == Zakero_MemZone_Error_None);

	size = Zakero_MemZone_Available_Total(memzone);

	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_KILOBYTE(1)
		, id
		);

	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(id    != 0);
	CHECK(size  != Zakero_MemZone_Available_Total(memzone));

	error = Zakero_MemZone_Free(memzone, id);

	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(size  == Zakero_MemZone_Available_Total(memzone));

	Zakero_MemZone_Destroy(memzone);
} // }}}
// TEST_CASE("/c/allocate/invalid-id")
// TEST_CASE("/c/allocate/defrag-trigger")
// TEST_CASE("/c/allocate/defrag-any")
// TEST_CASE("/c/allocate/defrag-none")

#endif // }}}

// }}}
// {{{ Implementation : C : Zakero_MemZone_Defrag() -

void Zakero_MemZone_Defrag(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif

	defrag_(memzone, 0);
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/defrag/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		, Zakero_MemZone_Expand_None
		, 0 // Defrag Disabled
		);

	CHECK(error == Zakero_MemZone_Error_None);

	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// {{{ Implementation : C : Zakero_MemZone_Available_Largest() -

size_t Zakero_MemZone_Available_Largest(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif

	Zakero_MemZone_Block* block  = memzone_block_first_(memzone);
	size_t                retval = 0;

	do
	{
		if(block_is_free_(block) == true)
		{
			retval = std::max(retval, block->size);
		}

		block = block->next;
	} while(block != nullptr);

	return retval;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/availalbe/largest/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		, Zakero_MemZone_Expand_None
		, 0 // Defrag Disabled
		);

	CHECK(error == Zakero_MemZone_Error_None);

	size_t available = Zakero_MemZone_Available_Largest(memzone);

	CHECK(available == ZAKERO_MEGABYTE(1));

	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// {{{ Implementation : C : Zakero_MemZone_Available_Total() -

size_t Zakero_MemZone_Available_Total(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif

	Zakero_MemZone_Block* block  = memzone_block_first_(memzone);
	size_t                retval = 0;

	do
	{
		if(block_is_free_(block) == true)
		{
			retval += block->size;
		}

		block = block->next;
	} while(block != nullptr);

	return retval;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/availalbe/total/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		, Zakero_MemZone_Expand_None
		, 0 // Defrag Disabled
		);

	CHECK(error == Zakero_MemZone_Error_None);

	size_t available = Zakero_MemZone_Available_Total(memzone);

	CHECK(available == ZAKERO_MEGABYTE(1));

	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// {{{ Implementation : C : Zakero_MemZone_Used_Largest() -

size_t Zakero_MemZone_Used_Largest(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif

	Zakero_MemZone_Block* block  = memzone_block_first_(memzone);
	size_t                retval = 0;

	do
	{
		if(block_is_allocated_(block) == true)
		{
			retval = std::max(retval, block->size);
		}

		block = block->next;
	} while(block != nullptr);

	return retval;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/used/largest/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		, Zakero_MemZone_Expand_None
		, 0 // Defrag Disabled
		);

	CHECK(error == Zakero_MemZone_Error_None);

	size_t used = Zakero_MemZone_Used_Largest(memzone);

	CHECK(used == 0);

	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// {{{ Implementation : C : Zakero_MemZone_Used_Total() -

size_t Zakero_MemZone_Used_Total(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif

	Zakero_MemZone_Block* block  = memzone_block_first_(memzone);
	size_t                retval = 0;

	do
	{
		retval += sizeof(Zakero_MemZone_Block);

		if(block_is_allocated_(block) == true)
		{
			retval += block->size;
		}

		block = block->next;
	} while(block != nullptr);

	return retval;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/used/total/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		, Zakero_MemZone_Expand_None
		, 0 // Defrag Disabled
		);

	CHECK(error == Zakero_MemZone_Error_None);

	size_t used = Zakero_MemZone_Used_Total(memzone);

	CHECK(used == sizeof(Zakero_MemZone_Block));

	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// {{{ Implementation : C : Zakero_MemZone_Size_Of() -

size_t Zakero_MemZone_Size_Of(Zakero_MemZone& memzone
	, uint64_t id
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif

	Zakero_MemZone_Block* block = memzone_block_first_(memzone);
	block = block_find_id_(block, id, Zakero_MemZone_Block_Find_Forward);

	if(block == nullptr)
	{
		return 0;
	}

	return block->size;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/size-of/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;
	uint64_t       id      = 0;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		, Zakero_MemZone_Expand_None
		, 0 // Defrag Disabled
		);

	CHECK(error == Zakero_MemZone_Error_None);

	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_KILOBYTE(1)
		, id
		);

	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(Zakero_MemZone_Size_Of(memzone, id) == ZAKERO_KILOBYTE(1));

	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// }}}
// {{{ Implementation : C++
namespace zakero
{
// {{{ Anonymous Namespace

namespace
{
	/**
	 * \brief Error Categories.
	 *
	 * This class implements the std::error_category interface to provide 
	 * consistent access to error code values and messages.
	 *
	 * See https://en.cppreference.com/w/cpp/error/error_category for 
	 * details.
	 */
	class MemZoneErrorCategory_
		: public std::error_category
	{
		public:
			constexpr MemZoneErrorCategory_() noexcept
			{
			}

			const char* name() const noexcept override
			{
				return "zakero.MemZone";
			}

			std::string message(int condition) const override
			{
				switch(condition)
				{
#define X(name_, val_, mesg_) \
					case val_: return mesg_;
					ZAKERO_MEMZONE__ERROR_DATA
#undef X
				}

				return "Unknown error condition";
			}
	} MemZoneErrorCategory;

	/**
	 * \brief Calculate the byte-aligned size.
	 *
	 * The byte-aligned size will be determined based on the provided \p 
	 * size.
	 *
	 * \return The byte-aligned size.
	inline off_t calculateActualSize(const off_t  size      ///< The size
		, const zakero::MemZone::Alignment alignment ///< The alignment
		)
	{
		const off_t mod  = static_cast<off_t>(alignment);
		const off_t step = static_cast<off_t>(alignment) + 1;

		return ((size + mod) / step) * step;
	}
	 */
}

// }}}
// {{{ Documentation

/**
 * \class zakero::MemZone
 *
 * \brief A pool of memory.
 *
 * Refer to Zakero_MemZone.h to learn how to include this library.
 *
 * This object will create a region of memory and provide an interface to 
 * allocate from that memory.
 *
 * \internal
 *
 * The structure of the zakero::MemZone class is that the public API 
 * validates the input then uses the private methods to do the work.  The 
 * advantages of this approach are:
 * - Invalid data _should_ never be a concern for the code that does the work.
 * - The public API handles the thread locking, freeing the worker code from 
 * that burden.
 * - Each private method focuses on only 1 type of work.  The public API calls 
 * the appropriate workers as needed.
 */

/**
 * \enum MemZone::Alignment
 *
 * \brief The Byte-Alignment of the MemZone.
 *
 * When allocating memory from the MemZone, this enum determines which 
 * byte-boundary will be used.
 */

/**
 * \typedef MemZone::AddressMap
 *
 * \brief A mapping of old addresses to new addresses.
 */

/**
 * \typedef MemZone::LambdaSize
 *
 * \brief A lambda that receives a `size_t` argument.
 */

/**
 * \typedef MemZone::LambdaAddressMap
 *
 * \brief A lambda that receives a MemZone::AddressMap.
 */

/**
 * \struct MemZone::Segment
 *
 * \brief Data that defines a segment.
 *
 * \var MemZone::Segment::offset
 * \brief The offset into the allocated memory pool.
 *
 * \var MemZone::Segment::size
 * \brief The size of the allocated memory chunk.
 *
 * \var MemZone::Segment::in_use
 * \brief A flag used to determine if the memory chunk is in use.
 */

/**
 * \typedef MemZone::VectorSegment
 *
 * \brief A convenience data type.
 */

// }}}
// {{{ MemZone


// }}}
// {{{ Convenience


// }}}

}
// }}}

#endif // ZAKERO_MEMZONE_IMPLEMENTATION

// }}}

#endif // zakero_MemZone_h
