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
	X(Error_None                   , 0 , "No Error"                              ) \
	X(Error_Init_Failure_Name      , 1 , "Failed to initialize the MemZone name" ) \
	X(Error_Init_Failure_FD        , 2 , "Failed to initialize the MemZone FD"   ) \
	X(Error_Init_Failure_RAM       , 3 , "Failed to initialize the MemZone RAM"  ) \
	X(Error_Invalid_Parameter_Name , 4 , "The 'name' parameter is not valid"     ) \
	X(Error_Invalid_Parameter_Size , 5 , "The 'size' parameter is not valid"     ) \
	X(Error_Invalid_Parameter_Mode , 6 , "The 'mode' parameter is not valid"     ) \
	X(Error_Already_Initialized    , 7 , "MemZone has already been initialized"  ) \
	X(Error_Not_Initialized        , 8 , "MemZone has not been initialized"      ) \
	X(Error_Not_Enough_Memory      , 9 , "Not enough memory is availalbe"        ) \

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

enum Zakero_MemZone_Block_Flag
{	Zakero_MemZone_Block_Flag_Allocated     = 0x0000'0000'0000'0001
,	Zakero_MemZone_Block_Flag_In_Use        = 0x0000'0000'0000'0010
,	Zakero_MemZone_Block_Flag_Clear_On_Free = 0x0000'0000'0000'0100
};

enum Zakero_MemZone_Defrag_Level
{	Zakero_MemZone_Defrag_On_Allocate = (1 << 0)
,	Zakero_MemZone_Defrag_On_Free     = (1 << 1)
,	Zakero_MemZone_Defrag_On_Acquire  = (1 << 2)
,	Zakero_MemZone_Defrag_On_Release  = (1 << 3)
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
{	Zakero_MemZone_Expand_None
,	Zakero_MemZone_Expand_Auto
};

enum Zakero_MemZone_Mode : uint8_t
{	Zakero_MemZone_Mode_FD
,	Zakero_MemZone_Mode_RAM
,	Zakero_MemZone_Mode_SHM
};


// Block can contain upto 8 bytes
// - This is not accounted for!
struct Zakero_MemZone_Block
{
	uint64_t              id;
	uint64_t              flag;
	Zakero_MemZone_Block* next;
	Zakero_MemZone_Block* prev;
	size_t                size;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
	uint8_t               data[];
#pragma GCC diagnostic pop
};

struct Zakero_MemZone
{
	uint8_t*            memory = nullptr;
	size_t              size   = 0;
	//char*               name   = nullptr;
	//int                 fd     = -1;
	Zakero_MemZone_Mode mode   = Zakero_MemZone_Mode_RAM;
	uint8_t             defrag = 0;
};


[[]]          int    Zakero_MemZone_Init(const Zakero_MemZone_Mode, const size_t, Zakero_MemZone&) noexcept;
[[]]          void   Zakero_MemZone_Destroy(Zakero_MemZone&) noexcept;
[[nodiscard]] int    Zakero_MemZone_Allocate(Zakero_MemZone&, size_t, uint64_t&) noexcept;
[[nodiscard]] size_t Zakero_MemZone_Available_Largest(Zakero_MemZone&) noexcept;
[[nodiscard]] size_t Zakero_MemZone_Available_Total(Zakero_MemZone&) noexcept;
[[nodiscard]] size_t Zakero_MemZone_Used_Largest(Zakero_MemZone&) noexcept;
[[nodiscard]] size_t Zakero_MemZone_Used_Total(Zakero_MemZone&) noexcept;

/*
int Zakero_MemZone_Init_From(Zakero_MemZone& //memzone
	, const Zakero_MemZone& //memzone
	) noexcept;

int Zakero_MemZone_Init_From_FD(Zakero_MemZone& //memzone
	, const int //fd
	) noexcept;

int Zakero_MemZone_Free(Zakero_MemZone& memzone
	, uint64_t id
	)
void* Zakero_MemZone_Acquire(Zakero_MemZone& memzone
	, uint64_t id
	)
void Zakero_MemZone_Release(Zakero_MemZone& memzone
	, uint64_t id
	)
size_t Zakero_MemZone_Size_Of(Zakero_MemZone& memzone
	, uint64_t id
	)
size_t Zakero_MemZone_Realloc(Zakero_MemZone& memzone
	, uint64_t id
	, size_t size
	)
uint64_t Zakero_MemZone_Defrag(Zakero_MemZone& memzone
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

constexpr size_t zakero_memzone_block_sizeof_ = sizeof(Zakero_MemZone_Block);

// {{{ Implementation : C : block_id_next_() -

uint64_t block_id_next_(Zakero_MemZone_Block* block
	) noexcept
{
	uint64_t retval = 0;

	do
	{
		if(retval <= block->id)
		{
			retval = block->id + 1;
		}

		block = block->next;
	} while(block != nullptr);

	return retval;
}

// }}}
// {{{ Implementation : C : block_is_allocated_() -

inline bool block_is_allocated_(const Zakero_MemZone_Block& block
	) noexcept
{
	return (bool)(block.flag & Zakero_MemZone_Block_Flag_Allocated);
}

// }}}
// {{{ Implementation : C : block_is_in_use_() -

inline bool block_is_in_use(const Zakero_MemZone_Block& block
	) noexcept
{
	return (bool)(block.flag & Zakero_MemZone_Block_Flag_In_Use);
}

// }}}
// {{{ Implementation : C : block_find_free_() -

Zakero_MemZone_Block* block_find_free_(Zakero_MemZone_Block* block
	, size_t size
	) noexcept
{
	while(block != nullptr)
	{
		if(block_is_allocated_(*block) == false)
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

// }}}
// {{{ Implementation : C : block_split_() -

void block_split_(Zakero_MemZone_Block* block
	, size_t size
	) noexcept
{
	Zakero_MemZone_Block* new_block = nullptr;

	if(size < 8)
	{
		new_block = block + zakero_memzone_block_sizeof_;
		new_block->size = block->size - zakero_memzone_block_sizeof_;
	}
	else
	{
		new_block = block + (size - 8);
		new_block->size = block->size - (size - 8);
	}

	new_block->id = 0;
	new_block->flag = 0;
	new_block->next = block->next;
	new_block->prev = block;

	block->next = new_block;
}

// }}}
// {{{ Implementation : C : defrag_() -

void defrag_(Zakero_MemZone& //memzone
	) noexcept
{
	printf("%s : Not Implemented\n", __PRETTY_FUNCTION__);
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
	memzone.fd = memfd_create(memzone.name, 0);
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

int Zakero_MemZone_Init(const Zakero_MemZone_Mode mode
	, const size_t    size
	, Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory != nullptr)
	{
		return Zakero_MemZone_Error_Already_Initialized;
	}

	if(size < zakero_memzone_block_sizeof_)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'size' must be greater than %lu", zakero_memzone_block_sizeof_);
		return Zakero_MemZone_Error_Invalid_Parameter_Size;
	}

	if(mode_is_supported_(mode) == false)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'mode' has unsupported value");
		return Zakero_MemZone_Error_Invalid_Parameter_Mode;
	}
#endif

	memzone.size = size;

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

	Zakero_MemZone_Block* block = (Zakero_MemZone_Block*)memzone.memory;
	block->id   = 0;
	block->flag = 0;
	block->next = nullptr;
	block->prev = nullptr;
	block->size = memzone.size - zakero_memzone_block_sizeof_ + 8;

	return Zakero_MemZone_Error_None;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/init/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;

#if 0 // Name only applies to FD based memory
	SUBCASE("Invalid Name: Null") // {{{
	{
		error = Zakero_MemZone_Init(nullptr
			, Zakero_MemZone_Mode_FD
			, ZAKERO_MEGABYTE(1)
			, memzone
			);

		CHECK(error == Zakero_MemZone_Error_Invalid_Parameter_Name);
	} // }}}
	SUBCASE("Invalid Name: Empty") // {{{
	{
		error = Zakero_MemZone_Init(""
			, Zakero_MemZone_Mode_RAM
			, ZAKERO_MEGABYTE(1)
			, memzone
			);

		CHECK(error == Zakero_MemZone_Error_Invalid_Parameter_Name);
	} // }}}
#endif
	SUBCASE("Invalid Size: 0") // {{{
	{
		error = Zakero_MemZone_Init(Zakero_MemZone_Mode_RAM
			, 0 // Size
			, memzone
			);

		CHECK(error == Zakero_MemZone_Error_Invalid_Parameter_Size);
	} // }}}

	SUBCASE("Minimum Size") // {{{
	{
		error = Zakero_MemZone_Init(Zakero_MemZone_Mode_RAM
			, 40 // Size
			, memzone
			);

		CHECK(error == Zakero_MemZone_Error_None);
		CHECK(Zakero_MemZone_Available_Largest(memzone) == 8);
		CHECK(Zakero_MemZone_Available_Total(memzone)   == 8);
		CHECK(Zakero_MemZone_Used_Largest(memzone)      == 0);
		CHECK(Zakero_MemZone_Used_Total(memzone)        == 0);
	} // }}}
#if __HAIKU__ // {{{ Invalid Mode
	SUBCASE("Invalid Mode: Zakero_MemZone_Mode_FD") // {{{
	{
		error = Zakero_MemZone_Init(Zakero_MemZone_Mode_FD
			, ZAKERO_MEGABYTE(1)
			, memzone
			);

		CHECK(error == Zakero_MemZone_Error_Invalid_Parameter_Mode);
	} // }}}
	SUBCASE("Invalid Mode: Zakero_MemZone_Mode_SHM") // {{{
	{
		error = Zakero_MemZone_Init(Zakero_MemZone_Mode_SHM
			, ZAKERO_MEGABYTE(1)
			, memzone
			);

		CHECK(error == Zakero_MemZone_Error_Invalid_Parameter_Mode);
	} // }}}
#endif // }}}
	SUBCASE("Already Initialized") // {{{
	{
		error = Zakero_MemZone_Init(Zakero_MemZone_Mode_RAM
			, ZAKERO_MEGABYTE(1)
			, memzone
			);

		CHECK(error == Zakero_MemZone_Error_None);

		error = Zakero_MemZone_Init(Zakero_MemZone_Mode_RAM
			, ZAKERO_MEGABYTE(1)
			, memzone
			);

		CHECK(error == Zakero_MemZone_Error_Already_Initialized);

		Zakero_MemZone_Destroy(memzone);
	} // }}}
} // }}}
TEST_CASE("/c/init/fd/") // {{{
{
#if __linux__
	//const char*    name    = "MemZone_c_init_fd";
	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(Zakero_MemZone_Mode_FD
		, ZAKERO_MEGABYTE(1)
		, memzone
		);

	CHECK(error == Zakero_MemZone_Error_None);
	//CHECK(strcmp(memzone.name, name) == 0);
	CHECK(memzone.fd != -1);
	//CHECK(memzone.memory != nullptr);

	SUBCASE("Already initialized")
	{
	}
#endif
} // }}}
TEST_CASE("/c/init/ram/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		, memzone
		);

	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(memzone.memory != nullptr);

	Zakero_MemZone_Destroy(memzone);
} // }}}
TEST_CASE("/c/init/shm/") // {{{
{
} // }}}

#endif // }}}

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

	memzone.memory = nullptr;
	//memzone.name   = nullptr;
	memzone.size   = 0;
	//memzone.fd     = -1;
	memzone.mode   = Zakero_MemZone_Mode_RAM;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/destroy/fd/") // {{{
{
#if __linux__
	//const char*    name    = "MemZone_c_init_fd";
	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(Zakero_MemZone_Mode_FD
		, ZAKERO_MEGABYTE(1)
		, memzone
		);

	CHECK(error == Zakero_MemZone_Error_None);
	//CHECK(strcmp(memzone.name, name) == 0);
	CHECK(memzone.fd != -1);
	//CHECK(memzone.memory != nullptr);

	SUBCASE("Already initialized")
	{
	}
#endif
} // }}}
TEST_CASE("/c/destroy/ram/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		, memzone
		);

	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(memzone.memory != nullptr);

	Zakero_MemZone_Destroy(memzone);

	CHECK(memzone.memory == nullptr);
	//CHECK(memzone.name   == nullptr);
	CHECK(memzone.size   == 0);
	//CHECK(memzone.fd     == -1);
	CHECK(memzone.mode   == Zakero_MemZone_Mode_RAM);
} // }}}
TEST_CASE("/c/destroy/shm/") // {{{
{
} // }}}

#endif // }}}

int Zakero_MemZone_Allocate(Zakero_MemZone& memzone
	, size_t    size
	, uint64_t& id
	) noexcept
{
	bool did_defrag = false;

	Zakero_MemZone_Block* block = (Zakero_MemZone_Block*)memzone.memory;

	block = block_find_free_(block, size);
	if(block == nullptr)
	{
		if(memzone.defrag != 0)
		{
			defrag_(memzone);
			block = (Zakero_MemZone_Block*)memzone.memory;
			block = block_find_free_(block, size);

			did_defrag = true;
		}
	}

	if(block == nullptr)
	{
		return Zakero_MemZone_Error_Not_Enough_Memory;
	}

	if((block->size - size) > zakero_memzone_block_sizeof_)
	{
		block_split_(block, size);
	}

	block->id = block_id_next_((Zakero_MemZone_Block*)memzone.memory);
	id = block->id;

	if((memzone.defrag & Zakero_MemZone_Defrag_On_Allocate)
		&& (did_defrag == false)
		)
	{
		defrag_(memzone);
	}

	return Zakero_MemZone_Error_None;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/allocate/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;
	uint64_t       id      = 0;

	error = Zakero_MemZone_Init(Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		, memzone
		);

	CHECK(error == Zakero_MemZone_Error_None);

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

	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

size_t Zakero_MemZone_Available_Largest(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif

	Zakero_MemZone_Block* block  = (Zakero_MemZone_Block*)memzone.memory;
	size_t                retval = 0;

	do
	{
		if(block_is_allocated_(*block) == false)
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

	error = Zakero_MemZone_Init(Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		, memzone
		);

	CHECK(error == Zakero_MemZone_Error_None);

	size_t available = Zakero_MemZone_Available_Largest(memzone);

	CHECK(available == (ZAKERO_MEGABYTE(1) - zakero_memzone_block_sizeof_ + 8));

	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

size_t Zakero_MemZone_Available_Total(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif

	Zakero_MemZone_Block* block  = (Zakero_MemZone_Block*)memzone.memory;
	size_t                retval = 0;

	do
	{
		if(block_is_allocated_(*block) == false)
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

	error = Zakero_MemZone_Init(Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		, memzone
		);

	CHECK(error == Zakero_MemZone_Error_None);

	size_t available = Zakero_MemZone_Available_Total(memzone);

	CHECK(available == (ZAKERO_MEGABYTE(1) - zakero_memzone_block_sizeof_ + 8));

	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

size_t Zakero_MemZone_Used_Largest(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif

	Zakero_MemZone_Block* block  = (Zakero_MemZone_Block*)memzone.memory;
	size_t                retval = 0;

	do
	{
		if(block_is_allocated_(*block) == true)
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

	error = Zakero_MemZone_Init(Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		, memzone
		);

	CHECK(error == Zakero_MemZone_Error_None);

	size_t used = Zakero_MemZone_Used_Largest(memzone);

	CHECK(used == 0);

	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

size_t Zakero_MemZone_Used_Total(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif

	Zakero_MemZone_Block* block  = (Zakero_MemZone_Block*)memzone.memory;
	size_t                retval = 0;

	do
	{
		if(block_is_allocated_(*block) == true)
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

	error = Zakero_MemZone_Init(Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		, memzone
		);

	CHECK(error == Zakero_MemZone_Error_None);

	size_t used = Zakero_MemZone_Used_Total(memzone);

	CHECK(used == 0);

	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

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
