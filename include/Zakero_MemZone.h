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
	X(Error_None                       ,  0 , "No Error"                                             ) \
	X(Error_Init_Failure_Name          ,  1 , "Failed to initialize the MemZone name"                ) \
	X(Error_Init_Failure_FD            ,  2 , "Failed to initialize the MemZone FD"                  ) \
	X(Error_Init_Failure_RAM           ,  3 , "Failed to initialize the MemZone RAM"                 ) \
	X(Error_Invalid_Parameter_Name     ,  4 , "The 'name' parameter is not valid"                    ) \
	X(Error_Invalid_Parameter_Size     ,  5 , "The 'size' parameter is not valid"                    ) \
	X(Error_Invalid_Parameter_Mode     ,  6 , "The 'mode' parameter is not valid"                    ) \
	X(Error_Invalid_Parameter_Expand   ,  7 , "The 'expand' parameter is not valid"                  ) \
	X(Error_Invalid_Parameter_Defrag   ,  8 , "The 'defrag' parameter is not valid"                  ) \
	X(Error_Invalid_Parameter_Id       ,  9 , "The 'id' parameter is not valid"                      ) \
	X(Error_Already_Initialized        , 10 , "MemZone has already been initialized"                 ) \
	X(Error_Not_Initialized            , 11 , "MemZone has not been initialized"                     ) \
	X(Error_Not_Enough_Memory          , 12 , "Not enough memory is availalbe"                       ) \
	X(Error_Not_Enough_Memory_Expand   , 13 , "Not enough memory is availalbe and expanding failed"  ) \
	X(Error_Not_Enough_Memory_Defrag   , 14 , "Not enough memory is availalbe and defragging failed" ) \
	X(Error_Destroyed_Allocated_Memory , 15 , "MemZone was destroyed with Allocated memory"          ) \
	X(Error_Destroyed_Acquired_Memory  , 16 , "MemZone was destroyed with Acquired memory"           ) \
	X(Error_Id_Is_Acquired             , 17 , "Operation can not be done on an acquired ID"          ) \
	X(Error_Id_Is_Not_Acquired         , 18 , "The ID has not been aquired"                          ) \

#define ZAKERO_BYTE(val_)     (val_)
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

enum Zakero_MemZone_Mode
{	Zakero_MemZone_Mode_FD  = 0x0001'0000'0000'0000
,	Zakero_MemZone_Mode_RAM = 0x0002'0000'0000'0000
,	Zakero_MemZone_Mode_SHM = 0x0003'0000'0000'0000
};

enum Zakero_MemZone_Defrag_Event
{	Zakero_MemZone_Defrag_Disable     = 0x0000'0000'0000'0000
,	Zakero_MemZone_Defrag_On_Allocate = 0x0000'0000'0000'0001
,	Zakero_MemZone_Defrag_On_Free     = 0x0000'0000'0000'0002
,	Zakero_MemZone_Defrag_On_Acquire  = 0x0000'0000'0000'0004
,	Zakero_MemZone_Defrag_On_Release  = 0x0000'0000'0000'0008
,	Zakero_MemZone_Defrag_On_Resize   = 0x0000'0000'0000'0018
};


#define zakero_memzone_block_data_(block_) \
	((uint8_t*)((uint64_t)block_ + sizeof(Zakero_MemZone_Block)))

struct Zakero_MemZone
{
	uint8_t* memory  = nullptr;
	size_t   size    = 0;
	uint64_t next_id = 0;
	uint64_t flag    = 0;

	//char*  name    = nullptr;
	//int    fd      = -1;
};


[[]]          int         Zakero_MemZone_Init(Zakero_MemZone&, const Zakero_MemZone_Mode, const size_t) noexcept;
[[]]          int         Zakero_MemZone_Destroy(Zakero_MemZone&) noexcept;
[[]]          void        Zakero_MemZone_DefragNow(Zakero_MemZone&) noexcept;
[[]]          void        Zakero_MemZone_DefragSet(Zakero_MemZone&, uint64_t) noexcept;
[[]]          void        Zakero_MemZone_ExpandDisable(Zakero_MemZone&) noexcept;
[[]]          void        Zakero_MemZone_ExpandEnable(Zakero_MemZone&) noexcept;
[[]]          void        Zakero_MemZone_ZeroFillDisable(Zakero_MemZone&) noexcept;
[[]]          void        Zakero_MemZone_ZeroFillEnable(Zakero_MemZone&) noexcept;
[[]]          int         Zakero_MemZone_Allocate(Zakero_MemZone&, size_t, uint64_t&) noexcept;
[[]]          int         Zakero_MemZone_Resize(Zakero_MemZone&, uint64_t, size_t) noexcept;
[[]]          int         Zakero_MemZone_Free(Zakero_MemZone&, uint64_t id) noexcept;
[[nodiscard]] void*       Zakero_MemZone_Acquire(Zakero_MemZone&, uint64_t id) noexcept;
[[]]          int         Zakero_MemZone_Release(Zakero_MemZone&, uint64_t id) noexcept;
[[nodiscard]] size_t      Zakero_MemZone_Available_Largest(Zakero_MemZone&) noexcept;
[[nodiscard]] size_t      Zakero_MemZone_Available_Total(Zakero_MemZone&) noexcept;
[[nodiscard]] size_t      Zakero_MemZone_Used_Largest(Zakero_MemZone&) noexcept;
[[nodiscard]] size_t      Zakero_MemZone_Used_Total(Zakero_MemZone&) noexcept;
[[nodiscard]] size_t      Zakero_MemZone_Size_Of(Zakero_MemZone&, uint64_t) noexcept;
[[nodiscard]] const char* Zakero_MemZone_Error_Message(int) noexcept;

/*

int Zakero_MemZone_Init_From(Zakero_MemZone& //memzone
	, const Zakero_MemZone& //memzone
	) noexcept;

int Zakero_MemZone_Init_From_FD(Zakero_MemZone& //memzone
	, const int //fd
	) noexcept;

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
// {{{ Private Namespace -

namespace
{
	/* {{enum(name=Zakero_MemZone_Expand
	 *   , member =
	 *     [ { name  = Zakero_MemZone_Expand_Disable
	 *       , brief = The MemZone is not allowed to expand.
	 *       }
	 *     , { name  = Zakero_MemZone_Expand_Auto
	 *       , brief = The MemZone will automatically expand as needed.
	 *       }
	 *     ]
	 *   )
	 * }}
	 */
	enum Zakero_MemZone_Expand
	{	Zakero_MemZone_Expand_Disable = 0x0000'0000'0000'0000
	,	Zakero_MemZone_Expand_Enable  = 0x0000'0001'0000'0000
	};

	enum Zakero_MemZone_ZeroFill
	{	Zakero_MemZone_ZeroFill_Disable = 0x0000'0000'0000'0000
	,	Zakero_MemZone_ZeroFill_Enable  = 0x0000'0000'0001'0000
	};

	constexpr uint64_t Zakero_MemZone_Defrag_Mask_   = 0x0000'0000'0000'000f;
	constexpr uint64_t Zakero_MemZone_ZeroFill_Mask_ = 0x0000'0000'ffff'0000;
	constexpr uint64_t Zakero_MemZone_Expand_Mask_   = 0x0000'ffff'0000'0000;
	constexpr uint64_t Zakero_MemZone_Mode_Mask_     = 0xffff'0000'0000'0000;

	// TODO: Can this be removed?
	enum Zakero_MemZone_Block_Find
	{	Zakero_MemZone_Block_Find_Forward
	,	Zakero_MemZone_Block_Find_Backward
	};

	enum Zakero_MemZone_Block_State
	{	Zakero_MemZone_Block_State_Allocated = (1 << 0)
	,	Zakero_MemZone_Block_State_Acquired  = (1 << 1)
	,	Zakero_MemZone_Block_State_ZeroFill  = (1 << 2)
	,	Zakero_MemZone_Block_State_Last      = (1 << 3)
	};

	struct Zakero_MemZone_Block
	{
		uint64_t id;
		uint64_t flag;
		uint64_t size;
		uint64_t prev;
	};
	constexpr uint64_t Size_Min_ = sizeof(Zakero_MemZone_Block) + sizeof(uint64_t);

}

// }}}
// {{{ Implementation : C -
// {{{ round_to_64bit() -

[[nodiscard]] static inline size_t round_to_64bit(size_t size
	) noexcept
{
	return ((size + 7) & ~0x07);
}

// }}}
// {{{ block_state_acquired_() -

[[nodiscard]] static inline bool block_state_acquired_(const Zakero_MemZone_Block* block
	) noexcept
{
	return (bool)(block->flag & Zakero_MemZone_Block_State_Acquired);
}

// }}}
// {{{ block_state_acquired_set_() -

static inline void block_state_acquired_set_(Zakero_MemZone_Block* block
	, bool value
	) noexcept
{
	if(value == true)
	{
		block->flag |= Zakero_MemZone_Block_State_Acquired;
		return;
	}

	block->flag &= (~Zakero_MemZone_Block_State_Acquired);
}

// }}}
// {{{ block_state_allocated_() -

[[nodiscard]] static inline bool block_state_allocated_(const Zakero_MemZone_Block* block
	) noexcept
{
	return (bool)(block->flag & Zakero_MemZone_Block_State_Allocated);
}

// }}}
// {{{ block_state_allocated_set_() -

static inline void block_state_allocated_set_(Zakero_MemZone_Block* block
	, bool value
	) noexcept
{
	if(value == true)
	{
		block->flag |= Zakero_MemZone_Block_State_Allocated;
		return;
	}

	block->flag &= (~Zakero_MemZone_Block_State_Allocated);
}

// }}}
// {{{ block_state_free_() -

[[nodiscard]] static inline bool block_state_free_(const Zakero_MemZone_Block* block
	) noexcept
{
	return (bool)((block->flag & Zakero_MemZone_Block_State_Allocated) == 0);
}

// }}}
// {{{ block_state_last_() -

[[nodiscard]] static inline bool block_state_last_(const Zakero_MemZone_Block* block
	) noexcept
{
	return (bool)(block->flag & Zakero_MemZone_Block_State_Last);
}

// }}}
// {{{ block_state_last_set_() -

static inline void block_state_last_set_(Zakero_MemZone_Block* block
	, bool value
	) noexcept
{
	if(value == true)
	{
		block->flag |= Zakero_MemZone_Block_State_Last;
		return;
	}

	block->flag &= (~Zakero_MemZone_Block_State_Last);
}

// }}}
// {{{ block_state_zerofill_() -

[[nodiscard]] static inline bool block_state_zerofill_(const Zakero_MemZone_Block* block
	) noexcept
{
	return (bool)(block->flag & Zakero_MemZone_Block_State_ZeroFill);
}

// }}}
// {{{ block_state_zerofill_set_() -

static inline void block_state_zerofill_set_(Zakero_MemZone_Block* block
	, bool value
	) noexcept
{
	if(value == true)
	{
		block->flag |= Zakero_MemZone_Block_State_ZeroFill;
		return;
	}

	block->flag &= (~Zakero_MemZone_Block_State_ZeroFill);
}

// }}}
// {{{ block_next_() -

[[nodiscard]] static inline Zakero_MemZone_Block* block_next_(const Zakero_MemZone_Block* block
	) noexcept
{
	Zakero_MemZone_Block* block_next = (Zakero_MemZone_Block*)
		(	(((uint64_t)block) + sizeof(Zakero_MemZone_Block) + block->size)
		*	(uint64_t)(block_state_last_(block) == false)
		);

	return block_next;
}

// }}}
// {{{ block_next_ignore_last_() -

[[nodiscard]] static inline Zakero_MemZone_Block* block_next_ignore_last_(const Zakero_MemZone_Block* block
	) noexcept
{
	Zakero_MemZone_Block* block_next = (Zakero_MemZone_Block*)
		(	(((uint64_t)block) + sizeof(Zakero_MemZone_Block) + block->size)
		);

	return block_next;
}

// }}}
// {{{ block_prev_() -

[[nodiscard]] static inline Zakero_MemZone_Block* block_prev_(const Zakero_MemZone_Block* block
	) noexcept
{
	Zakero_MemZone_Block* block_prev = (Zakero_MemZone_Block*)
		(	(((uint64_t)block) - block->prev)
		*	(uint64_t)(block->prev != 0)
		);

	return block_prev;
}

// }}}
// {{{ block_prev_set_() -

static inline void block_prev_set_(Zakero_MemZone_Block* block
	, const Zakero_MemZone_Block* block_prev
	) noexcept
{
	if(block_prev == nullptr)
	{
		block->prev = 0;
		return;
	}

	block->prev = (uint64_t)block - (uint64_t)block_prev;
}

// }}}
// {{{ block_zerofill_() -

static void block_zerofill_(Zakero_MemZone_Block* block
	) noexcept
{
	memset(zakero_memzone_block_data_(block)
		, 0
		, block->size
		);
}

// }}}
// {{{ block_init_() -

static inline void block_init_(Zakero_MemZone_Block* block
	, uint64_t              size
	, Zakero_MemZone_Block* block_prev
	) noexcept
{
	block->id   = 0;
	block->flag = 0;
	block->size = size;

	block_prev_set_(block, block_prev);
}

// }}}
// {{{ block_find_active_() -

[[nodiscard]] static Zakero_MemZone_Block* block_find_active_(Zakero_MemZone_Block* block
	, Zakero_MemZone_Block_Find direction
	) noexcept
{
	if(direction == Zakero_MemZone_Block_Find_Forward)
	{
		while(block != nullptr)
		{
			if(block_state_acquired_(block) == true)
			{
				return block;
			}

			block = block_next_(block);
		}

		return block;
	}
	else if(direction == Zakero_MemZone_Block_Find_Backward)
	{
		while(block != nullptr)
		{
			if(block_state_acquired_(block) == true)
			{
				return block;
			}

			block = block_prev_(block);
		}

		return block;
	}

	return nullptr;
}

// }}}
// {{{ block_find_free_() -

[[nodiscard]] static Zakero_MemZone_Block* block_find_free_(Zakero_MemZone_Block* block
	, size_t                    size
	, Zakero_MemZone_Block_Find direction
	) noexcept
{
	if(direction == Zakero_MemZone_Block_Find_Forward)
	{
		while(block != nullptr)
		{
			if(block_state_free_(block) == true)
			{
				if(block->size >= size)
				{
					return block;
				}
			}

			block = block_next_(block);
		}

		return block;
	}
	else if(direction == Zakero_MemZone_Block_Find_Backward)
	{
		while(block != nullptr)
		{
			if(block_state_free_(block) == true)
			{
				if(block->size >= size)
				{
					return block;
				}
			}

			block = block_prev_(block);
		}

		return block;
	}

	return nullptr;
}

// }}}
// {{{ block_find_id_() -

[[nodiscard]] static Zakero_MemZone_Block* block_find_id_(Zakero_MemZone_Block* block
	, uint64_t                  id
	, Zakero_MemZone_Block_Find direction
	) noexcept
{
	if(direction == Zakero_MemZone_Block_Find_Forward)
	{
		while(block != nullptr)
		{
			if((block->id == id)
				&& (block_state_allocated_(block) == true)
				)
			{
				return block;
			}

			block = block_next_(block);
		}

		return block;
	}
	else if(direction == Zakero_MemZone_Block_Find_Backward)
	{
		while(block != nullptr)
		{
			if((block->id == id)
				&& (block_state_allocated_(block) == true)
				)
			{
				return block;
			}

			block = block_prev_(block);
		}

		return block;
	}

	return nullptr;
}

// }}}
// {{{ block_find_last_() -

[[nodiscard]] static Zakero_MemZone_Block* block_find_last_(Zakero_MemZone_Block* block
	) noexcept
{
	while(block_state_last_(block) == false)
	{
		block = block_next_(block);
	}

	return block;
}

// }}}
// {{{ block_find_last_allocated_() -

[[nodiscard]] static Zakero_MemZone_Block* block_find_last_allocated_(Zakero_MemZone_Block* block
	) noexcept
{
	Zakero_MemZone_Block* block_stop = block;

	block = block_find_last_(block);

	while(block_state_allocated_(block) == false)
	{
		block = block_prev_(block);

		if(block == nullptr)
		{
			return nullptr;
		}

		if(block <= block_stop)
		{
			return nullptr;
		}
	}

	return block;
}

// }}}
// {{{ block_merge_with_next_() -

static void block_merge_with_next_(Zakero_MemZone_Block* block
	) noexcept
{
	Zakero_MemZone_Block* block_next = block_next_(block);

	if(block_state_last_(block_next) == true)
	{
		block_state_last_set_(block, true);
	}
	else
	{
		block_prev_set_(block_next_(block_next), block);
	}

	block->size += block_next->size + sizeof(Zakero_MemZone_Block);
	block_init_(block_next, 0, nullptr);
}

// }}}
// {{{ block_merge_free_() -

static Zakero_MemZone_Block* block_merge_free_(Zakero_MemZone_Block* block
	) noexcept
{
	if((block_state_last_(block) == false)
		&& (block_state_free_(block_next_(block)) == true)
		)
	{
		block_merge_with_next_(block);
	}

	Zakero_MemZone_Block* block_prev = block_prev_(block);
	if((block_prev != nullptr)
		&& (block_state_free_(block_prev) == true)
		)
	{
		block = block_prev;
		block_merge_with_next_(block);
	}

	return block;
}

// }}}
// {{{ block_move_() -

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
		memset((void*)
			(	(	(uint64_t)block_dst
					+ sizeof(Zakero_MemZone_Block)
				) + block_src->size
			)
			, 0
			, block_dst->size - block_src->size
			);
	}

	block_src->id = 0;
	block_state_allocated_set_(block_src, false);

	if(block_state_zerofill_(block_src) == true)
	{
		block_zerofill_(block_src);
		block_state_zerofill_set_(block_src, false);
	}

	block_merge_free_(block_src);
}

// }}}
// {{{ block_split_() -

static Zakero_MemZone_Block* block_split_(Zakero_MemZone_Block* block
	, size_t size
	) noexcept
{
	size_t block_next_size = block->size - (sizeof(Zakero_MemZone_Block) + size);
	block->size = size;

	Zakero_MemZone_Block* block_next = block_next_ignore_last_(block);
	block_init_(block_next
		, block_next_size
		, block
		);

	if(block_state_last_(block) == false)
	{
		block_prev_set_(block_next_(block_next), block_next);
	}
	else // block_state_last_(block) == true
	{
		block_state_last_set_(block     , false);
		block_state_last_set_(block_next, true);
	}

	return block_next;
}

// }}}
// {{{ block_swap_free_with_next_() -

[[nodiscard]] static Zakero_MemZone_Block* block_swap_free_with_next_(Zakero_MemZone_Block* block_left
	) noexcept
{
	Zakero_MemZone_Block* block_right = block_next_(block_left);

	Zakero_MemZone_Block block_left_temp = *block_right;
	block_left_temp.prev = block_left->prev;
		
	Zakero_MemZone_Block block_right_temp = *block_left;
		
	memmove(zakero_memzone_block_data_(block_left)
		, zakero_memzone_block_data_(block_right)
		, block_right->size
		);

	*block_left = block_left_temp;

	block_right = block_next_(block_left);
	*block_right = block_right_temp;

	if(block_state_last_(block_left) == true)
	{
		block_state_last_set_(block_left , false);
		block_state_last_set_(block_right, true);
	}

	if(block_state_zerofill_(block_right) == true)
	{
		block_zerofill_(block_right);
		block_state_zerofill_set_(block_right, false);
	}

	return block_right;
}

// }}}
// {{{ block_dump_() -
#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

[[maybe_unused]]
static void block_dump_(const Zakero_MemZone_Block* block
	) noexcept
{
	printf("---------------------------------------\n");
	printf("Block: %lu %016lx <- %016lx -> %016lx\n"
		, block->id
		, (uint64_t)block_prev_(block)
		, (uint64_t)block
		, (uint64_t)block_next_(block)
		);
	printf("Block Flag  : %016lx\n", block->flag);
	printf("Block Data  : %016lx (%lu)\n", (uint64_t)zakero_memzone_block_data_(block), block->size);
	printf("Block End   : %016lx\n", (uint64_t)(zakero_memzone_block_data_(block)) + block->size);
}

#endif // }}}
// }}}
// {{{ memzone_block_first_() -

[[nodiscard]] static inline Zakero_MemZone_Block* memzone_block_first_(Zakero_MemZone& memzone
	) noexcept
{
	Zakero_MemZone_Block* block = (Zakero_MemZone_Block*)memzone.memory;

	return block;
}

// }}}
// {{{ memzone_block_last_() -

[[nodiscard]] static Zakero_MemZone_Block* memzone_block_last_(Zakero_MemZone& memzone
	) noexcept
{
	Zakero_MemZone_Block* block = memzone_block_first_(memzone);

	while(block_state_last_(block) == false)
	{
		block = block_next_(block);
	}

	return block;
}

// }}}
// {{{ memzone_defrag_pass_() -

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

[[nodiscard]] static Zakero_MemZone_Block* memzone_defrag_pass_(Zakero_MemZone_Block* block
	) noexcept
{
	Zakero_MemZone_Block* block_free = block_find_free_(block, 0, Zakero_MemZone_Block_Find_Forward);
	if((block_free == nullptr)
		|| (block_state_last_(block_free) == true)
		)
	{
		return nullptr;
	}

	Zakero_MemZone_Block* block_temp    = block_find_last_allocated_(block_free);
	Zakero_MemZone_Block* block_to_move = nullptr;

	while((block_temp != nullptr)
		&& (block_temp > block_free)
		)
	{
		if(block_state_allocated_(block_temp) == true)
		{
			if(block_free->size >= block_temp->size)
			{
				if((block_to_move == nullptr)
					|| (block_temp->size > block_to_move->size)
					)
				{
					block_to_move = block_temp;
				}
			}
		}

		block_temp = block_prev_(block_temp);
	}

	if(block_to_move == nullptr)
	{
		block_free = block_swap_free_with_next_(block_free); // TODO: Rename block_free_swap_with_next_()
		block_free = block_merge_free_(block_free);

		return block_free;
	}

	if(block_free->size < (block_to_move->size + sizeof(Zakero_MemZone_Block)))
	{
		block_move_(block_to_move, block_free); // TODO: Rename block_free_swap_()
		block_free = block_to_move;

		return block_free;
	}

	block_temp = block_free;
	block_free = block_split_(block_temp, block_to_move->size);
	block_move_(block_to_move, block_temp);

	return block_free;
}

// }}}
// {{{ memzone_defrag_() -

static void memzone_defrag_(Zakero_MemZone& memzone
	, uint64_t defrag_passes
	) noexcept
{
	Zakero_MemZone_Block* block = memzone_block_first_(memzone);

	if(defrag_passes == 0)
	{
		do
		{
			block = memzone_defrag_pass_(block);
		} while(block != nullptr);
	}
	else
	{
		do
		{
			block = memzone_defrag_pass_(block);
			defrag_passes--;
		} while(block != nullptr && defrag_passes > 0);
	}
}

// }}}
// {{{ memzone_defrag_is_enabled_() -

[[nodiscard]] static inline bool memzone_defrag_is_enabled_(const Zakero_MemZone& memzone
	) noexcept
{
	return (bool)(memzone.flag & Zakero_MemZone_Defrag_Mask_);
}

// }}}
// {{{ memzone_defrag_on_allocate_() -

[[nodiscard]] static inline bool memzone_defrag_on_allocate_(const Zakero_MemZone& memzone
	) noexcept
{
	return (bool)(memzone.flag & Zakero_MemZone_Defrag_On_Allocate);
}

// }}}
// {{{ memzone_defrag_on_free_() -

[[nodiscard]] static inline bool memzone_defrag_on_free_(const Zakero_MemZone& memzone
	) noexcept
{
	return (bool)(memzone.flag & Zakero_MemZone_Defrag_On_Free);
}

// }}}
// {{{ memzone_defrag_on_resize_() -

[[nodiscard]] static inline bool memzone_defrag_on_resize_(const Zakero_MemZone& memzone
	) noexcept
{
	return (bool)(memzone.flag & Zakero_MemZone_Defrag_On_Resize);
}

// }}}
// {{{ memzone_defrag_on_acquire_() -

[[nodiscard]] static inline bool memzone_defrag_on_acquire_(const Zakero_MemZone& memzone
	) noexcept
{
	return (bool)(memzone.flag & Zakero_MemZone_Defrag_On_Acquire);
}

// }}}
// {{{ memzone_defrag_on_release_() -

[[nodiscard]] static inline bool memzone_defrag_on_release_(const Zakero_MemZone& memzone
	) noexcept
{
	return (bool)(memzone.flag & Zakero_MemZone_Defrag_On_Release);
}

// }}}
// {{{ memzone_expand_ram_() -

[[nodiscard]] static Zakero_MemZone_Block* memzone_expand_ram_(Zakero_MemZone& memzone
	, size_t size
	) noexcept
{
	size_t memzone_size = memzone.size + size;
	bool   append       = false;

	Zakero_MemZone_Block* block = memzone_block_last_(memzone);
	if(block_state_free_(block) == true)
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
		Zakero_MemZone_Block* block_prev = block;

		block = block_next_ignore_last_(block);
		block_init_(block, size, block_prev);

		block_state_last_set_(block_prev, false);
		block_state_last_set_(block     , true);
	}

	memset(zakero_memzone_block_data_(block), 1, block->size);

	return block;
}

// }}}
// {{{ memzone_expand_fd_() -

[[nodiscard]] static Zakero_MemZone_Block* memzone_expand_fd_(Zakero_MemZone& //memzone
	, size_t //size
	) noexcept
{
	fprintf(stderr, "%s: Not Implemented\n", __PRETTY_FUNCTION__);
	return nullptr;
}

// }}}
// {{{ memzone_expand_shm_() -

[[nodiscard]] static Zakero_MemZone_Block* memzone_expand_shm_(Zakero_MemZone& //memzone
	, size_t //size
	) noexcept
{
	fprintf(stderr, "%s: Not Implemented\n", __PRETTY_FUNCTION__);
	return nullptr;
}

// }}}
// {{{ memzone_expand_() -

[[nodiscard]] static inline Zakero_MemZone_Block* memzone_expand_(Zakero_MemZone& memzone
	, const size_t size
	) noexcept
{
	Zakero_MemZone_Block* block = memzone_block_first_(memzone);
	block = block_find_active_(block, Zakero_MemZone_Block_Find_Forward);

	if(block != nullptr)
	{
		// A block is in use, can not expand.
		return nullptr;
	}

	switch(memzone.flag & Zakero_MemZone_Mode_Mask_)
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
// {{{ memzone_expand_is_enabled_() -

[[nodiscard]] static inline bool memzone_expand_is_enabled_(const Zakero_MemZone& memzone
	) noexcept
{
	return (bool)(memzone.flag & Zakero_MemZone_Expand_Mask_);
}

// }}}
// {{{ memzone_expand_is_enabled_() -

[[nodiscard]] static inline bool memzone_zerofill_is_enabled_(const Zakero_MemZone& memzone
	) noexcept
{
	return (bool)(memzone.flag & Zakero_MemZone_ZeroFill_Mask_);
}

// }}}
// {{{ memzone_next_id_() -

[[nodiscard]] static uint64_t memzone_next_id_(Zakero_MemZone& memzone
	) noexcept
{
	uint64_t id = memzone.next_id;

	memzone.next_id++;

	return id;
}

// }}}
// {{{ memzone_block_shrink_() -

static void memzone_block_shrink_(Zakero_MemZone& memzone
	, Zakero_MemZone_Block* block
	, size_t                size
	, Zakero_MemZone_Block* block_free
	) noexcept
{
	uint64_t size_delta = block->size - size;

	if(size_delta >= Size_Min_)
	{
		block_free = block_split_(block, size_delta);
		block_merge_free_(block_free);

		return;
	}

	if(block_free == nullptr)
	{
		return;
	}

	size_delta = block_free->size - size;
	if(size_delta < Size_Min_)
	{
		block_free = memzone_block_first_(memzone);
		block_free = block_find_free_(block_free
			, (size + sizeof(Zakero_MemZone_Block))
			, Zakero_MemZone_Block_Find_Forward
			);

		if(block_free == nullptr)
		{
			return;
		}
	}

	block_free = block_split_(block_free, size);
	block_move_(block, block_free);
}

// }}}
// {{{ memzone_block_grow_() -

static void memzone_block_grow_(Zakero_MemZone& memzone
	, Zakero_MemZone_Block* block
	, size_t                size
	, Zakero_MemZone_Block* block_free
	) noexcept
{
	if(block_free == nullptr)
	{
		uint64_t size_delta = size - block->size;
		block_free = block_next_(block);
		if(block_free != nullptr
			&& block_state_free_(block_free) == true
			&& block_free->size >= size_delta
			)
		{
			block_free = block_split_(block_free, size_delta);
			block->size = size + sizeof(Zakero_MemZone_Block);

			return;
		}

		if(memzone_expand_is_enabled_(memzone) == true)
		{
			block_free = memzone_expand_(memzone, size);
		}
	}

	if(block_free == nullptr)
	{
		return;
	}

	if(block_free->size > (size + sizeof(Zakero_MemZone_Block)))
	{
		block_free = block_split_(block, size);
	}

	block_move_(block, block_free);
}

// }}}
// {{{ memzone_destroy_fd_() -

#ifdef __linux__

static void memzone_destroy_fd_(Zakero_MemZone& memzone
	) noexcept
{
}

#elif __HAIKU__

static void memzone_destroy_fd_(Zakero_MemZone& memzone
	) noexcept
{
}

#else
#	error "memzone_destroy_fd_()" has not been implemented yet!
#endif

// }}}
// {{{ memzone_destroy_ram_() -

#if defined __linux__ \
	|| __HAIKU__

static void memzone_destroy_ram_(Zakero_MemZone& memzone
	) noexcept
{
	memset(memzone.memory, 0, memzone.size);
	free(memzone.memory);
}

#else
#	error "memzone_destroy_ram_()" has not been implemented yet!
#endif

// }}}
// {{{ memzone_destroy_shm_() -

#ifdef __linux__

static void memzone_destroy_shm_(Zakero_MemZone& memzone
	) noexcept
{
}

#elif __HAIKU__

static void memzone_destroy_shm_(Zakero_MemZone& memzone
	) noexcept
{
}

#else
#	error "memzone_destroy_shm_()" has not been implemented yet!
#endif

// }}}
// {{{ memzone_init_fd_() -

#ifdef __linux__

static void memzone_init_fd_(Zakero_MemZone& memzone
	) noexcept
{
	//memzone.fd = memfd_create(memzone.name, 0);
}

#elif __HAIKU__

static void memzone_init_fd_(Zakero_MemZone& memzone
	) noexcept
{
}

#else
#	error "memzone_init_fd_()" has not been implemented yet!
#endif

// }}}
// {{{ memzone_init_ram_() -

#if defined __linux__ \
	|| defined __HAIKU__

static void memzone_init_ram_(Zakero_MemZone& memzone
	) noexcept
{
	memzone.memory = (uint8_t*)calloc(memzone.size, sizeof(uint8_t));
}

#else
#	error "memzone_init_ram_()" has not been implemented yet!
#endif

// }}}
// {{{ defrag_is_valid_() -

[[nodiscard]] static bool defrag_is_valid_(const uint64_t defrag
	) noexcept
{
	if((defrag & (~Zakero_MemZone_Defrag_Mask_)) == 0)
	{
		return true;
	}

	return false;
}

// }}}
// {{{ mode_is_valid_() -

#ifdef __linux__

[[nodiscard]] static bool mode_is_valid_(const Zakero_MemZone_Mode mode
	) noexcept
{
	switch(mode)
	{
		case Zakero_MemZone_Mode_FD:
			return true;

		case Zakero_MemZone_Mode_RAM:
			return true;

		case Zakero_MemZone_Mode_SHM:
			return true;
	};

	return false;
}

#elif __HAIKU__

[[nodiscard]] static bool mode_is_valid_(const Zakero_MemZone_Mode mode
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
#	error "mode_is_valid_()" has not been implemented yet!
#endif

// }}}

// {{{ Zakero_MemZone_Init() -

/* {{function(name = Zakero_MemZone_Init
 *   , param =
 *     [ { Zakero_MemZone& , memzone , The data.                 }
 *     , { const size_t    , mode    , The type of memory pool.  }
 *     , { uint64_t        , defrag  , When to defrag memory.    }
 *     ]
 *   , return = { int, An error code or 0 on success. }
 *   , attr   = [ noexcept ]
 *   , brief  = Prepare the Zakero_MemZone data for use.
 *   )
 *   Before anything can be done with Zakero_MemZone data, it must be 
 *   initialized. Initializing Zakero_MemZone data will create a memory pool 
 *   resource that can be used to store and retrieve arbitrary data.
 *
 *   The size of the memory pool will be rounded up so that it will align on a 
 *   64-bit boundary.
 *
 *   The defrag'ing of memory is event based, which allows each operation the 
 *   chance to defrag part of the pool. This reduces overhead to reduce the 
 *   penalty imposed by examining and defragmenting the entire memory pool. See 
 *   {{link(target=[Zakero_MemZone_DefragNow]}} for more details.
 *
 *   If the Zakero_MemZone was successfully initialized, the return value will 
 *   be {{link(target=[Zakero_MemZone_Error_None])}}, aka '0'. Otherwise the 
 *   appropriate error code will be returned.
 * }}
 */
int Zakero_MemZone_Init(Zakero_MemZone& memzone
	, Zakero_MemZone_Mode        mode
	, const size_t               size
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED // {{{
	if(memzone.memory != nullptr)
	{
		return Zakero_MemZone_Error_Already_Initialized;
	}

	if(size == 0)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'size' must be greater than 0");
		return Zakero_MemZone_Error_Invalid_Parameter_Size;
	}

	if(mode_is_valid_(mode) == false)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'mode' has unsupported value");
		return Zakero_MemZone_Error_Invalid_Parameter_Mode;
	}
#endif // }}}

	const size_t block_size = round_to_64bit(size);

	memzone.size    = sizeof(Zakero_MemZone_Block) + block_size;
	memzone.next_id = 1;
	memzone.flag    = 0
		| Zakero_MemZone_Mode_RAM
		;

	switch(memzone.flag & Zakero_MemZone_Mode_Mask_)
	{
		case Zakero_MemZone_Mode_RAM:
			memzone_init_ram_(memzone);
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

			memzone_init_fd_(memzone);
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

	block_init_(block, block_size, nullptr);
	block_state_last_set_(block, true);

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
			);

		CHECK(error == Zakero_MemZone_Error_Invalid_Parameter_Mode);
	} // }}}
	SUBCASE("Invalid Mode: Zakero_MemZone_Mode_SHM") // {{{
	{
		error = Zakero_MemZone_Init(memzone
			, Zakero_MemZone_Mode_SHM
			, ZAKERO_MEGABYTE(1)
			);

		CHECK(error == Zakero_MemZone_Error_Invalid_Parameter_Mode);
	} // }}}
#endif // }}}
#if __linux__ // {{{ Invalid Mode
	SUBCASE("Invalid Mode: Zakero_MemZone_Mode (fake)") // {{{
	{
		error = Zakero_MemZone_Init(memzone
			, (Zakero_MemZone_Mode)0xffff'ffff'ffff'ffff
			, ZAKERO_MEGABYTE(1)
			);

		CHECK(error == Zakero_MemZone_Error_Invalid_Parameter_Mode);
	} // }}}
#endif // }}}
	SUBCASE("Invalid Size: 0") // {{{
	{
		error = Zakero_MemZone_Init(memzone
			, Zakero_MemZone_Mode_RAM
			, 0 // Size
			);

		CHECK(error == Zakero_MemZone_Error_Invalid_Parameter_Size);
	} // }}}
	SUBCASE("64-bit Rounding: 1 -> 8") // {{{
	{
		error = Zakero_MemZone_Init(memzone
			, Zakero_MemZone_Mode_RAM
			, 1 // Size
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
			);

		CHECK(error == Zakero_MemZone_Error_None);

		error = Zakero_MemZone_Init(memzone
			, Zakero_MemZone_Mode_RAM
			, ZAKERO_MEGABYTE(1)
			);

		CHECK(error == Zakero_MemZone_Error_Already_Initialized);

		Zakero_MemZone_Destroy(memzone);
	} // }}}
	SUBCASE("Initialized") // {{{
	{
		error = Zakero_MemZone_Init(memzone
			, Zakero_MemZone_Mode_RAM
			, ZAKERO_KILOBYTE(1)
			);

		CHECK(error == Zakero_MemZone_Error_None);

		Zakero_MemZone_Block* block = memzone_block_first_(memzone);

		CHECK(block                         != nullptr);
		CHECK(block_next_(block)            == nullptr);
		CHECK(block_state_acquired_(block)  == false);
		CHECK(block_state_allocated_(block) == false);
		CHECK(block_state_free_(block)      == true);
		CHECK(block_state_last_(block)      == true);

		Zakero_MemZone_Destroy(memzone);
	} // }}}
} // }}}
TEST_CASE("/c/init/fd/") // {{{
{
	/*
	if(mode_is_valid_(Zakero_MemZone_Mode_FD) == false)
	{
		return;
	}

	//const char*    name    = "MemZone_c_init_fd";
	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_FD
		, ZAKERO_MEGABYTE(1)
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
#if defined(__HAIKU__) \
	|| defined(__linux__)

	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(memzone
		 ,Zakero_MemZone_Mode_RAM
		, ZAKERO_KILOBYTE(1)
		);

	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(memzone.memory != nullptr);

	Zakero_MemZone_Destroy(memzone);
#endif
} // }}}
TEST_CASE("/c/init/shm/") // {{{
{
#if 0 //defined(__linux__)
	if(mode_is_valid_(Zakero_MemZone_Mode_SHM) == false)
	{
		return;
	}
#endif
} // }}}

#endif // }}}

// }}}
// {{{ Zakero_MemZone_Destroy() -

/* {{function(name = Zakero_MemZone_Init
 *   , param =
 *     [ { Zakero_MemZone& , memzone , The data. }
 *     ]
 *   , return = { int, An error code or 0 on success. }
 *   , attr   = [ noexcept ]
 *   , brief  = Destroy the Zakero_MemZone data.
 *   )
 *   When the Zakero_MemZone data is no longer needed, this method will destroy 
 *   and release all used resources.
 *
 *   If the Zakero_MemZone is backed by RAM, then the memory will be 
 *   zero-filled then free'ed.
 * }}
 */
int Zakero_MemZone_Destroy(Zakero_MemZone& memzone
	) noexcept
{
	int error = Zakero_MemZone_Error_None;

#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED // {{{

	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
#		ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{
		return Zakero_MemZone_Error_Not_Initialized;
#		endif // }}}
	}

	bool has_acquired = false;
	bool has_allocated = false;
	Zakero_MemZone_Block* block = memzone_block_first_(memzone);
	do
	{
		if(block_state_acquired_(block) == true)
		{
			has_acquired = true;
			ZAKERO_MEMZONE_LOG_ERROR("Id %lu is still acquired", block->id);
		}
		else if(block_state_allocated_(block) == true)
		{
			has_allocated = true;
			ZAKERO_MEMZONE_LOG_ERROR("Id %lu is still allocated", block->id);
		}

		block = block_next_(block);
	} while(block != nullptr);

	if(has_acquired == true)
	{
		error = Zakero_MemZone_Error_Destroyed_Acquired_Memory;
	}
	else if(has_allocated == true)
	{
		error = Zakero_MemZone_Error_Destroyed_Allocated_Memory;
	}

#endif // }}}

	switch(memzone.flag & Zakero_MemZone_Mode_Mask_)
	{
		case Zakero_MemZone_Mode_FD:
			memzone_destroy_fd_(memzone);
			break;
		case Zakero_MemZone_Mode_RAM:
			memzone_destroy_ram_(memzone);
			break;
		case Zakero_MemZone_Mode_SHM:
			memzone_destroy_shm_(memzone);
			break;
	}

	memzone.memory  = nullptr;
	memzone.size    = 0;
	memzone.next_id = 0;
	memzone.flag    = 0;

	return error;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/destroy/") // {{{
{
	SUBCASE("Destroy Uninitialized") // {{{
	{
		Zakero_MemZone memzone = {};
		int            error   = 0;

		error = Zakero_MemZone_Destroy(memzone);
		CHECK(error == Zakero_MemZone_Error_Not_Initialized);
	} // }}}
	SUBCASE("Destroy With Allocated Memory") // {{{
	{
		Zakero_MemZone memzone = {};
		int            error   = 0;

		error = Zakero_MemZone_Init(memzone
			, Zakero_MemZone_Mode_RAM
			, ZAKERO_KILOBYTE(1)
			);

		CHECK(error == Zakero_MemZone_Error_None);

		uint64_t id = 0;
		Zakero_MemZone_Allocate(memzone, ZAKERO_BYTE(128), id);
		CHECK(id != 0);

		error = Zakero_MemZone_Destroy(memzone);
		CHECK(error == Zakero_MemZone_Error_Destroyed_Allocated_Memory);
	} // }}}
	SUBCASE("Destroy With Acquired Memory") // {{{
	{
		Zakero_MemZone memzone = {};
		int            error   = 0;

		error = Zakero_MemZone_Init(memzone
			, Zakero_MemZone_Mode_RAM
			, ZAKERO_KILOBYTE(1)
			);

		CHECK(error == Zakero_MemZone_Error_None);

		uint64_t id = 0;
		Zakero_MemZone_Allocate(memzone, ZAKERO_BYTE(128), id);
		CHECK(id != 0);

		void* ptr = Zakero_MemZone_Acquire(memzone, id);
		CHECK(ptr != nullptr);

		error = Zakero_MemZone_Destroy(memzone);
		CHECK(error == Zakero_MemZone_Error_Destroyed_Acquired_Memory);
	} // }}}
} // }}}
TEST_CASE("/c/destroy/ram/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		);

	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(memzone.memory != nullptr);

	Zakero_MemZone_Destroy(memzone);

	CHECK(memzone.memory  == nullptr);
	CHECK(memzone.size    == 0);
	CHECK(memzone.next_id == 0);
	CHECK(memzone.flag    == 0);
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
// {{{ Zakero_MemZone_DefragNow() ---------TEST-

void Zakero_MemZone_DefragNow(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif

	memzone_defrag_(memzone, 0);
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/defrag/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		);

	CHECK(error == Zakero_MemZone_Error_None);

	Zakero_MemZone_Destroy(memzone);
} // }}}
// TEST_CASE("/c/allocate/defrag-trigger")
// TEST_CASE("/c/allocate/defrag-any")
// TEST_CASE("/c/allocate/defrag-none")

#endif // }}}

// }}}
// {{{ Zakero_MemZone_DefragSet() ---------TEST-

/* {{function(name = Zakero_MemZone_DefragNow
 *   , param =
 *     [ { Zakero_MemZone& , memzone , The data.              }
 *     , { uint64_t        , defrag  , When to defrag memory. }
 *     ]
 *   , attr   = [ noexcept ]
 *   , brief  = Determine when to automatically defrag.
 *   )
 *   The automatic defrag'ing of memory is event based, which allows each 
 *   operation the chance to defrag part of the pool. This reduces overhead to 
 *   reduce the penalty imposed by examining and defragmenting the entire 
 *   memory pool. See {{link(target=[Zakero_MemZone_DefragNow]}} for more 
 *   details.
 * }}
 */
void Zakero_MemZone_DefragSet(Zakero_MemZone& memzone
	, uint64_t defrag
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED // {{{
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}

	if(defrag_is_valid_(defrag) == false)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'defrag' has unsupported value");
	}
#endif // }}}

	memzone.flag |= defrag;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/defragset/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		);

	CHECK(error == Zakero_MemZone_Error_None);

	Zakero_MemZone_Destroy(memzone);
} // }}}
// TEST_CASE("/c/allocate/defrag-trigger")
// TEST_CASE("/c/allocate/defrag-any")
// TEST_CASE("/c/allocate/defrag-none")

#endif // }}}

// }}}
// {{{ Zakero_MemZone_ExpandDisable() -----TEST-

/* {{function(name = Zakero_MemZone_ExpandDisable
 *   , param =
 *     [ { Zakero_MemZone& , memzone , The data. }
 *     ]
 *   , attr = [ noexcept ]
 *   , brief = Do not allow the memory pool to expand.
 *   )
 *   Use this function to keep the MemZone at a fixed size.
 *
 *   {{bold This is the default.}}
 * }}
 */
void Zakero_MemZone_ExpandDisable(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif

	memzone.flag &= (~Zakero_MemZone_Expand_Enable);
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/expand/disable") // {{{
{
	Zakero_MemZone memzone = {};

	SUBCASE("Uninitialized") // {{{
	{
		Zakero_MemZone_ExpandDisable(memzone);
	} // }}}

	int error = 0;
	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_KILOBYTE(1)
		);

	CHECK(error == Zakero_MemZone_Error_None);

	uint64_t id = 0;
	// ExpandDisable is the default
	error = Zakero_MemZone_Allocate(memzone, ZAKERO_KILOBYTE(2), id);
	CHECK(error == Zakero_MemZone_Error_Not_Enough_Memory);

	Zakero_MemZone_ExpandDisable(memzone);
	error = Zakero_MemZone_Allocate(memzone, ZAKERO_KILOBYTE(2), id);
	CHECK(error == Zakero_MemZone_Error_Not_Enough_Memory);

	Zakero_MemZone_ExpandEnable(memzone);
	Zakero_MemZone_ExpandDisable(memzone);
	error = Zakero_MemZone_Allocate(memzone, ZAKERO_KILOBYTE(2), id);
	CHECK(error == Zakero_MemZone_Error_Not_Enough_Memory);

	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// {{{ Zakero_MemZone_ExpandEnable() ------TEST-

/* {{function(name = Zakero_MemZone_ExpandEnable
 *   , parm =
 *     [ { Zakero_MemZone& , memzone , The data. }
 *     ]
 *   , attr = [ noexcept ]
 *   , brief = Allow the memory pool to automatially expand.
 *   )
 *   After calling this function, the MemZone memory pool will automatically 
 *   grow to accomodate new allocations if possible.
 */
void Zakero_MemZone_ExpandEnable(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif

	memzone.flag |= Zakero_MemZone_Expand_Enable;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/expand/enable") // {{{
{
	Zakero_MemZone memzone = {};

	SUBCASE("Uninitialized") // {{{
	{
		Zakero_MemZone_ExpandEnable(memzone);
	} // }}}

	int error = 0;
	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_KILOBYTE(1)
		);

	CHECK(error == Zakero_MemZone_Error_None);

	// ExpandDisable is the default
	uint64_t id = 0;
	error = Zakero_MemZone_Allocate(memzone, ZAKERO_KILOBYTE(2), id);
	CHECK(error == Zakero_MemZone_Error_Not_Enough_Memory);

	Zakero_MemZone_ExpandEnable(memzone);
	error = Zakero_MemZone_Allocate(memzone, ZAKERO_KILOBYTE(2), id);
	CHECK(error == Zakero_MemZone_Error_None);

	Zakero_MemZone_Free(memzone, id);
	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// {{{ Zakero_MemZone_ZeroFillDisable() -

/* {{function(name = Zakero_MemZone_ZeroFillDisable
 *   , param =
 *     [ { Zakero_MemZone& , memzone , The data. }
 *     ]
 *   , attr = [ noexcept ]
 *   , brief = Do not allow the memory pool to expand.
 *   )
 *   Use this function to keep the MemZone at a fixed size.
 *
 *   {{bold This is the default.}}
 * }}
 */
void Zakero_MemZone_ZeroFillDisable(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED // {{{
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif // }}}

	memzone.flag &= (~Zakero_MemZone_ZeroFill_Enable);
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/zerofill/disable") // {{{
{
	Zakero_MemZone memzone = {};

	SUBCASE("Uninitialized") // {{{
	{
		Zakero_MemZone_ZeroFillDisable(memzone);
	} // }}}

	int error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_BYTE(64)
		);
	CHECK(error == Zakero_MemZone_Error_None);

	//----------------------------------------
	// This is the default, but disable anyway
	Zakero_MemZone_ZeroFillDisable(memzone);
	//----------------------------------------

	uint64_t id = 0;
	error = Zakero_MemZone_Allocate(memzone, ZAKERO_BYTE(64), id);
	CHECK(error == Zakero_MemZone_Error_None);

	constexpr uint8_t VALUE = 0x11;
	void* ptr = Zakero_MemZone_Acquire(memzone, id);
	memset(ptr, VALUE, Zakero_MemZone_Size_Of(memzone, id));
	Zakero_MemZone_Release(memzone, id);
	Zakero_MemZone_Free(memzone, id);

	error = Zakero_MemZone_Allocate(memzone, ZAKERO_BYTE(64), id);
	CHECK(error == Zakero_MemZone_Error_None);
	ptr = Zakero_MemZone_Acquire(memzone, id);
	CHECK(ptr != nullptr);
	uint8_t value = ((uint8_t*)ptr)[0];
	CHECK(value == VALUE);

	Zakero_MemZone_Release(memzone, id);
	Zakero_MemZone_Free(memzone, id);
	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// {{{ Zakero_MemZone_ZeroFillEnable() -

/* {{function(name = Zakero_MemZone_ZeroFillEnable
 *   , parm =
 *     [ { Zakero_MemZone& , memzone , The data. }
 *     ]
 *   , attr = [ noexcept ]
 *   , brief = Allow the memory pool to automatially expand.
 *   )
 *   After calling this function, the MemZone memory pool will automatically 
 *   grow to accomodate new allocations if possible.
 */
void Zakero_MemZone_ZeroFillEnable(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED // {{{
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif // }}}

	memzone.flag |= Zakero_MemZone_ZeroFill_Enable;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/zerofill/enable") // {{{
{
	Zakero_MemZone memzone = {};

	SUBCASE("Uninitialized") // {{{
	{
		Zakero_MemZone_ZeroFillEnable(memzone);
	} // }}}

	int error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_KILOBYTE(1)
		);
	CHECK(error == Zakero_MemZone_Error_None);

	//----------------------------------------
	Zakero_MemZone_ZeroFillEnable(memzone);
	//----------------------------------------

	uint64_t id = 0;
	error = Zakero_MemZone_Allocate(memzone, ZAKERO_BYTE(64), id);
	CHECK(error == Zakero_MemZone_Error_None);

	constexpr uint8_t VALUE = 0x11;
	void* ptr = Zakero_MemZone_Acquire(memzone, id);
	memset(ptr, VALUE, Zakero_MemZone_Size_Of(memzone, id));
	Zakero_MemZone_Release(memzone, id);
	Zakero_MemZone_Free(memzone, id);

	error = Zakero_MemZone_Allocate(memzone, ZAKERO_BYTE(64), id);
	CHECK(error == Zakero_MemZone_Error_None);
	ptr = Zakero_MemZone_Acquire(memzone, id);
	CHECK(ptr != nullptr);
	uint8_t value = ((uint8_t*)ptr)[0];
	CHECK(value == 0x00);

	Zakero_MemZone_Release(memzone, id);
	Zakero_MemZone_Free(memzone, id);
	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// {{{ Zakero_MemZone_Allocate() -

/* {{function(name = Zakero_MemZone_Allocate
 *   , param =
 *     [ { Zakero_MemZone& , memzone , The data.                             }
 *     , { const size_t    , memzone , The byte size of the memory section.  }
 *     , { uint64_t&       , id      , The ID of the memory section.         }
 *     ]
 *   , return = { int, An error code or 0 on success. }
 *   , attr   = [ noexcept ]
 *   , brief  = Allocate a section of the Zakero_MemZone data for use.
 *   )
 *   Use this function to reserve a section of the Zakero_MemZone data for 
 *   future use. The "id" of the data will be provided as the means of 
 *   identifing the reserved section of data.
 * }}
 */
int Zakero_MemZone_Allocate(Zakero_MemZone& memzone
	, size_t    size
	, uint64_t& id
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED // {{{
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
#		ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{
		return Zakero_MemZone_Error_Not_Initialized;
#		endif // }}}
	}

#endif // }}}

	const size_t block_size = round_to_64bit(size == 0 ? 1 : size);

	int error_code = Zakero_MemZone_Error_Not_Enough_Memory;

	Zakero_MemZone_Block* block = memzone_block_first_(memzone);
	block = block_find_free_(block, block_size, Zakero_MemZone_Block_Find_Forward);

	if((block == nullptr)
		&& memzone_defrag_is_enabled_(memzone)
		)
	{
		memzone_defrag_(memzone, 0);

		block = memzone_block_first_(memzone);
		block = block_find_free_(block, block_size, Zakero_MemZone_Block_Find_Forward);

		if(block == nullptr)
		{
			error_code = Zakero_MemZone_Error_Not_Enough_Memory_Defrag;
		}
	}

	if((block == nullptr)
		&& memzone_expand_is_enabled_(memzone)
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

	if((block->size - block_size) >= Size_Min_)
	{
		block_split_(block, block_size);
	}

	id        = memzone_next_id_(memzone);
	block->id = id;
	block_state_allocated_set_(block, true);

	if(memzone_defrag_on_allocate_(memzone) == true)
	{
		memzone_defrag_(memzone, 1);
	}

	return Zakero_MemZone_Error_None;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/allocate/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;

	SUBCASE("Uninitialized") // {{{
	{
		uint64_t id = 0;
		error = Zakero_MemZone_Allocate(memzone, 0, id);
		CHECK(error == Zakero_MemZone_Error_Not_Initialized);
	} // }}}

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_KILOBYTE(1)
		);
	CHECK(error == Zakero_MemZone_Error_None);

	SUBCASE("Not Enough Memory") // {{{
	{
		uint64_t id = 0;
		error = Zakero_MemZone_Allocate(memzone
			, ZAKERO_KILOBYTE(2)
			, id
			);

		CHECK(error == Zakero_MemZone_Error_Not_Enough_Memory);
	} // }}}
	SUBCASE("Just Enough Memory") // {{{
	{
		uint64_t id = 0;
		error = Zakero_MemZone_Allocate(memzone
			, ZAKERO_KILOBYTE(1)
			, id
			);
		CHECK(error == Zakero_MemZone_Error_None);
		CHECK(id    != 0);

		error = Zakero_MemZone_Free(memzone, id);
		CHECK(error == Zakero_MemZone_Error_None);
	} // }}}
	SUBCASE("Enough Memory x2") // {{{
	{
		uint64_t id_1 = 0;
		uint64_t id_2 = 0;
		error = Zakero_MemZone_Allocate(memzone
			, 128 // Bytes
			, id_1
			);
		CHECK(error == Zakero_MemZone_Error_None);
		CHECK(id_1  != 0);

		error = Zakero_MemZone_Allocate(memzone
			, 128 // Bytes
			, id_2
			);
		CHECK(error == Zakero_MemZone_Error_None);
		CHECK(id_2  != 0);
		CHECK(id_1  != id_2);

		error = Zakero_MemZone_Free(memzone, id_1);
		CHECK(error == Zakero_MemZone_Error_None);

		error = Zakero_MemZone_Free(memzone, id_2);
		CHECK(error == Zakero_MemZone_Error_None);
	} // }}}

	Zakero_MemZone_Destroy(memzone);
} // }}}
TEST_CASE("/c/allocate/defrag/") // {{{
{
	Zakero_MemZone memzone = {};

	int error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, (64 * 4) + (sizeof(Zakero_MemZone_Block) * 2)
		);
	CHECK(error == Zakero_MemZone_Error_None);

	uint64_t id_1 = 0;
	error = Zakero_MemZone_Allocate(memzone
		, 64
		, id_1
		);
	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(id_1  != 0);
	memset(Zakero_MemZone_Acquire(memzone, id_1), 0x11, 64);
	Zakero_MemZone_Release(memzone, id_1);

	uint64_t id_3 = 0;
	error = Zakero_MemZone_Allocate(memzone
		, 64
		, id_3
		);
	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(id_3  != 0);
	memset(Zakero_MemZone_Acquire(memzone, id_3), 0x33, 64);
	Zakero_MemZone_Release(memzone, id_3);

	uint64_t id_2 = 0;
	error = Zakero_MemZone_Allocate(memzone
		, 64
		, id_2
		);
	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(id_2  != 0);
	memset(Zakero_MemZone_Acquire(memzone, id_2), 0x22, 64);
	Zakero_MemZone_Release(memzone, id_2);

	error = Zakero_MemZone_Free(memzone, id_3);
	CHECK(error == Zakero_MemZone_Error_None);

	error = Zakero_MemZone_Allocate(memzone
		, (64 * 2)
		, id_3
		);
	CHECK(error == Zakero_MemZone_Error_Not_Enough_Memory);

	// -----------------------------------
	Zakero_MemZone_DefragNow(memzone);
	// -----------------------------------

	error = Zakero_MemZone_Allocate(memzone
		, (64 * 2)
		, id_3
		);
	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(id_3  != 0);
	memset(Zakero_MemZone_Acquire(memzone, id_3), 0x33, (64 * 2));

	Zakero_MemZone_Release(memzone, id_1);
	Zakero_MemZone_Free(memzone, id_1);

	Zakero_MemZone_Release(memzone, id_2);
	Zakero_MemZone_Free(memzone, id_2);

	Zakero_MemZone_Release(memzone, id_3);
	Zakero_MemZone_Free(memzone, id_3);

	Zakero_MemZone_Destroy(memzone);
} // }}}
TEST_CASE("/c/allocate/expand/") // {{{
{
	Zakero_MemZone memzone = {};

	int error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_KILOBYTE(1)
		);
	CHECK(error == Zakero_MemZone_Error_None);

	uint64_t id_1 = 0;
	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_KILOBYTE(1)
		, id_1
		);
	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(id_1  != 0);

	uint64_t id_2 = 0;
	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_KILOBYTE(1)
		, id_2
		);
	CHECK(error == Zakero_MemZone_Error_Not_Enough_Memory);
	CHECK(id_2  == 0);

	// -----------------------------------
	Zakero_MemZone_ExpandEnable(memzone);
	// -----------------------------------

	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_KILOBYTE(1)
		, id_2
		);
	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(id_2  != 0);

	CHECK(id_1  != id_2);
	CHECK(Zakero_MemZone_Used_Total(memzone) > ZAKERO_KILOBYTE(1));

	error = Zakero_MemZone_Free(memzone, id_1);
	CHECK(error == Zakero_MemZone_Error_None);

	error = Zakero_MemZone_Free(memzone, id_2);
	CHECK(error == Zakero_MemZone_Error_None);

	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// {{{ Zakero_MemZone_Resize() ------------TEST-

int Zakero_MemZone_Resize(Zakero_MemZone& memzone
	, uint64_t id
	, size_t   size
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif

	size = round_to_64bit(size);
	
	Zakero_MemZone_Block* block = memzone_block_first_(memzone);
	block = block_find_id_(block, id, Zakero_MemZone_Block_Find_Forward);

	if(block_state_acquired_(block) == true)
	{
		return Zakero_MemZone_Error_Id_Is_Acquired;
	}

	if(block->size == size)
	{
		return Zakero_MemZone_Error_None;
	}

	Zakero_MemZone_Block* block_free = memzone_block_first_(memzone);
	block_free = block_find_free_(block, size, Zakero_MemZone_Block_Find_Forward);

	if(block_free != nullptr
		&& block_free->size == size
		)
	{
		block_move_(block, block_free);
	}
	else if(block->size > size) // Shrink
	{
		memzone_block_shrink_(memzone, block, size, block_free);
	}
	else // Grow
	{
		memzone_block_grow_(memzone, block, size, block_free);

	}

	if(memzone_defrag_on_resize_(memzone) == true)
	{
		memzone_defrag_(memzone, 1);
	}

	return Zakero_MemZone_Error_None;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/resize/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_MEGABYTE(1)
		);

	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(memzone.memory != nullptr);

	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// {{{ Zakero_MemZone_Free() ------ DEFRAG TEST-

/* {{function(nam = Zakero_MemZone_Free
 *   , param =
 *     [ { Zakero_MemZone& , memzone , The data.       }
 *     , { uint64_t        , id      , The ID to free. }
 *     ]
 *   , attr = [ noexcept ]
 *   , brief = Free allocated memory.
 *   )
 *   When allocated memory is no longer needed, use this function to free it.
 * }}
 */
int Zakero_MemZone_Free(Zakero_MemZone& memzone
	, uint64_t id
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED // {{{
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
#		ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{
		return Zakero_MemZone_Error_Not_Initialized;
#		endif // }}}
	}
#endif // }}}

	Zakero_MemZone_Block* block = memzone_block_first_(memzone);
	block = block_find_id_(block, id, Zakero_MemZone_Block_Find_Forward);

#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED // {{{
	if(block == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'id(%lu)' doses not exist.", id);
#		ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{
		return Zakero_MemZone_Error_Invalid_Parameter_Id;
#		endif // }}}
	}

	if(block_state_acquired_(block) == true)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Id %lu is still acquired", block->id);
#		ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{
		return Zakero_MemZone_Error_Id_Is_Acquired;
#		endif // }}}
	}
#endif // }}}

	if(memzone_zerofill_is_enabled_(memzone) == true)
	{
		block_zerofill_(block);
	}

	block->id   = 0;
	block_state_allocated_set_(block, false);
	block_merge_free_(block);

	if(memzone_defrag_on_free_(memzone) == true)
	{
		memzone_defrag_(memzone, 1);
	}

	return Zakero_MemZone_Error_None;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/free/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;
	uint64_t       id      = 0;

	SUBCASE("Uninitialized") // {{{
	{
		error = Zakero_MemZone_Free(memzone, id);
		CHECK(error == Zakero_MemZone_Error_Not_Initialized);
	} // }}}

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_BYTE(128)
		);
	CHECK(error == Zakero_MemZone_Error_None);

	SUBCASE("Does Not Exist: Never Created") // {{{
	{
		error = Zakero_MemZone_Free(memzone, id);
		CHECK(error == Zakero_MemZone_Error_Invalid_Parameter_Id);
	} // }}}

	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_BYTE(128)
		, id
		);
	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(id    != 0);

	SUBCASE("Happy Path") // {{{
	{
		error = Zakero_MemZone_Free(memzone, id);
		CHECK(error == Zakero_MemZone_Error_None);
	} // }}}
	SUBCASE("Is Acquired") // {{{
	{
		(void)Zakero_MemZone_Acquire(memzone, id);

		error = Zakero_MemZone_Free(memzone, id);
		CHECK(error == Zakero_MemZone_Error_Id_Is_Acquired);
	} // }}}
	SUBCASE("Free'ed 2x") // {{{
	{
		error = Zakero_MemZone_Free(memzone, id);
		error = Zakero_MemZone_Free(memzone, id);
		CHECK(error == Zakero_MemZone_Error_Invalid_Parameter_Id);
	} // }}}

	Zakero_MemZone_Destroy(memzone);
} // }}}
// TEST_CASE("/c/allocate/invalid-id")
// TEST_CASE("/c/allocate/acquired")
// TEST_CASE("/c/allocate/defrag-trigger")
// TEST_CASE("/c/allocate/defrag-any")
// TEST_CASE("/c/allocate/defrag-none")

#endif // }}}

// }}}
// {{{ Zakero_MemZone_Acquire() -

/* {{function(name = Zakero_MemZone_Acquire
 *   , param =
 *     [ { Zakero_MemZone& , memzone , The data.      }
 *     , { uint64_t        , id      , The memory ID. }
 *     ]
 *   , attr  = [ noexcept ]
 *   , brief = Get a pointer to the allocated memory.
 *   )
 *   This function will provide a pointer to the memory that has been allocated 
 *   and associated with an ID. When a pointer to the memory has been acquired, 
 *   that memory will be "lock" in position. Any memory that is "locked" will 
 *   not be moved by Zakero_MemZone, meaning that defragging operations will 
 *   not touch that memory and any attempts to expand the size of 
 *   Zakero_MemZone will automatically fail (the entire memory pool may be 
 *   moved).
 *
 *   When the memory no longer needs to be used, but still kept for future 
 *   access, {{link(target=[Zakero_MemZone_Release]) Release}} the ID so that 
 *   Zakero_MemZone can move the memory to another location if needed.
 * }}
 */
void* Zakero_MemZone_Acquire(Zakero_MemZone& memzone
	, uint64_t id
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED // {{{
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
	}
#endif // }}}

	if(memzone_defrag_on_acquire_(memzone) == true)
	{
		memzone_defrag_(memzone, 1);
	}

	Zakero_MemZone_Block* block = memzone_block_first_(memzone);
	block = block_find_id_(block, id, Zakero_MemZone_Block_Find_Forward);

#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED // {{{
	if(block == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'id(%lu)' doses not exist.", id);
		return nullptr;
	}
#endif // }}}

	block_state_acquired_set_(block, true);
	void* ptr = (void*)zakero_memzone_block_data_(block);

	return ptr;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/acquire/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;
	uint64_t       id      = 0;
	void*          ptr     = nullptr;

	SUBCASE("Uninitialized") // {{{
	{
		ptr = Zakero_MemZone_Acquire(memzone, id);
		CHECK(ptr == nullptr);
	} // }}}

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_BYTE(128)
		);
	CHECK(error == Zakero_MemZone_Error_None);

	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_BYTE(64)
		, id
		);
	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(id    != 0);

	ptr = Zakero_MemZone_Acquire(memzone, id);
	CHECK(ptr != nullptr);

	ptr = Zakero_MemZone_Acquire(memzone, id + 1);
	CHECK(ptr == nullptr);

	Zakero_MemZone_Release(memzone, id);
	Zakero_MemZone_Free(memzone, id);
	Zakero_MemZone_Destroy(memzone);
} // }}}
TEST_CASE("/c/acquire/defrag/") // {{{
{
	Zakero_MemZone memzone  = {};
	int            error    = 0;
	uint64_t       id_1     = 0;
	uint64_t       id_2     = 0;
	void*          p_before = nullptr;
	void*          p_after  = nullptr;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_BYTE(128) + sizeof(Zakero_MemZone_Block)
		);
	CHECK(error == Zakero_MemZone_Error_None);

	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_BYTE(64)
		, id_1
		);
	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(id_1  != 0);
	p_before = Zakero_MemZone_Acquire(memzone, id_1);
	memset(p_before, 0x11, Zakero_MemZone_Size_Of(memzone, id_1));
	Zakero_MemZone_Release(memzone, id_1);

	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_BYTE(64)
		, id_2
		);
	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(id_2  != 0);
	p_before = Zakero_MemZone_Acquire(memzone, id_2);
	memset(p_before, 0x22, Zakero_MemZone_Size_Of(memzone, id_2));
	Zakero_MemZone_Release(memzone, id_2);

	Zakero_MemZone_Free(memzone, id_1);

	//----------------------------------------
	Zakero_MemZone_DefragSet(memzone
		, Zakero_MemZone_Defrag_On_Acquire
		);
	//----------------------------------------

	p_after = Zakero_MemZone_Acquire(memzone, id_2);
	CHECK(p_before != p_after);

	Zakero_MemZone_Release(memzone, id_2);
	Zakero_MemZone_Free(memzone, id_2);
	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// {{{ Zakero_MemZone_Release() -

/* {{function(name = Zakero_MemZone_Release
 *   , param =
 *     [ { Zakero_MemZone& , memzone , The data.      }
 *     , { uint64_t        , id      , The memory ID. }
 *     ]
 *   , attr  = [ noexcept ]
 *   , brief = Release the pointer to allocated memory.
 *   )
 *   After a pointer has been acquired to allocated memory, use this function 
 *   to release the pointer. The memory will also be "unlocked", allowing 
 *   Zakero_MemZone to relocate the memory if needed.
 * }}
 */
int Zakero_MemZone_Release(Zakero_MemZone& memzone
	, uint64_t id
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED // {{{
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
#		ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{
		return Zakero_MemZone_Error_Not_Initialized;
#		endif // }}}
	}
#endif // }}}

	Zakero_MemZone_Block* block = memzone_block_first_(memzone);
	block = block_find_id_(block, id, Zakero_MemZone_Block_Find_Forward);

#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED // {{{
	if(block == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'id(%lu)' doses not exist.", id);
#		ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{
		return Zakero_MemZone_Error_Invalid_Parameter_Id;
#		endif // }}}
	}

	if(block_state_acquired_(block) == false)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'id(%lu)' has not been acquired.", id);
#		ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{
		return Zakero_MemZone_Error_Id_Is_Not_Acquired;
#		endif // }}}
	}
#endif // }}}

	block_state_acquired_set_(block, false);

	if(memzone_defrag_on_release_(memzone) == true)
	{
		memzone_defrag_(memzone, 1);
	}

	return Zakero_MemZone_Error_None;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/release/") // {{{
{
	Zakero_MemZone memzone = {};
	int            error   = 0;
	uint64_t       id      = 0;

	SUBCASE("Uninitialized") // {{{
	{
		error = Zakero_MemZone_Release(memzone, id);
		CHECK(error == Zakero_MemZone_Error_Not_Initialized);
	} // }}}

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_BYTE(128)
		);
	CHECK(error == Zakero_MemZone_Error_None);

	SUBCASE("Does Not Exist: Never Created") // {{{
	{
		error = Zakero_MemZone_Release(memzone, id);
		CHECK(error == Zakero_MemZone_Error_Invalid_Parameter_Id);
	} // }}}

	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_BYTE(64)
		, id
		);
	CHECK(error == Zakero_MemZone_Error_None);

	SUBCASE("Not Acquired") // {{{
	{
		error = Zakero_MemZone_Release(memzone, id);
		CHECK(error == Zakero_MemZone_Error_Id_Is_Not_Acquired);
	} // }}}
	SUBCASE("Happy Path") // {{{
	{
		(void)Zakero_MemZone_Acquire(memzone, id);

		error = Zakero_MemZone_Release(memzone, id);
		CHECK(error == Zakero_MemZone_Error_None);
	} // }}}
	SUBCASE("Released 2x") // {{{
	{
		(void)Zakero_MemZone_Acquire(memzone, id);

		error = Zakero_MemZone_Release(memzone, id);
		error = Zakero_MemZone_Release(memzone, id);
		CHECK(error == Zakero_MemZone_Error_Id_Is_Not_Acquired);
	} // }}}

	Zakero_MemZone_Free(memzone, id);
	Zakero_MemZone_Destroy(memzone);
} // }}}
TEST_CASE("/c/release/defrag/") // {{{
{
	Zakero_MemZone memzone  = {};
	int            error    = 0;
	uint64_t       id_1     = 0;
	uint64_t       id_2     = 0;
	void*          p_before = nullptr;
	void*          p_after  = nullptr;

	error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_BYTE(128) + sizeof(Zakero_MemZone_Block)
		);
	CHECK(error == Zakero_MemZone_Error_None);

	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_BYTE(64)
		, id_1
		);
	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(id_1  != 0);
	p_before = Zakero_MemZone_Acquire(memzone, id_1);
	memset(p_before, 0x11, Zakero_MemZone_Size_Of(memzone, id_1));
	Zakero_MemZone_Release(memzone, id_1);

	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_BYTE(64)
		, id_2
		);
	CHECK(error == Zakero_MemZone_Error_None);
	CHECK(id_2  != 0);
	p_before = Zakero_MemZone_Acquire(memzone, id_2);
	memset(p_before, 0x22, Zakero_MemZone_Size_Of(memzone, id_2));
	Zakero_MemZone_Release(memzone, id_2);

	Zakero_MemZone_Free(memzone, id_1);

	//----------------------------------------
	Zakero_MemZone_DefragSet(memzone
		, Zakero_MemZone_Defrag_On_Release
		);
	//----------------------------------------

	p_after = Zakero_MemZone_Acquire(memzone, id_2);
	CHECK(p_before == p_after);

	Zakero_MemZone_Release(memzone, id_2);

	p_after = Zakero_MemZone_Acquire(memzone, id_2);
	CHECK(p_before != p_after);

	Zakero_MemZone_Release(memzone, id_2);
	Zakero_MemZone_Free(memzone, id_2);
	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// {{{ Zakero_MemZone_Available_Largest() -

/* {{function(name = Zakero_MemZone_Available_Largest
 *   , param =
 *     [ { Zakero_MemZone& , memzone , The data. }
 *     ]
 *   , attr   = [ noexcept ]
 *   , brief  = Get the size of the largest contiguous block of memory.
 *   , return = The size of the memory.
 *   )
 * }}
 */
size_t Zakero_MemZone_Available_Largest(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED // {{{
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
#		ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{
		return 0;
#		endif // }}}
	}
#endif // }}}

	Zakero_MemZone_Block* block  = memzone_block_first_(memzone);
	size_t                retval = 0;

	do
	{
		if(block_state_free_(block) == true)
		{
			retval = std::max(retval, block->size);
		}

		block = block_next_(block);
	} while(block != nullptr);

	return retval;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/availalbe/largest/") // {{{
{
	Zakero_MemZone memzone = {};

	SUBCASE("Uninitialized") // {{{
	{
		CHECK(Zakero_MemZone_Available_Largest(memzone) == 0);
	} // }}}

	int error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_KILOBYTE(1)
		);
	CHECK(error == Zakero_MemZone_Error_None);

	CHECK(Zakero_MemZone_Available_Largest(memzone) == ZAKERO_KILOBYTE(1));

	uint64_t id_1 = 0;
	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_BYTE(64)
		, id_1
		);
	CHECK(error == Zakero_MemZone_Error_None);

	uint64_t id_2 = 0;
	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_BYTE(200)
		, id_2
		);
	CHECK(error == Zakero_MemZone_Error_None);

	uint64_t id_3 = 0;
	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_BYTE(256)
		, id_3
		);
	CHECK(error == Zakero_MemZone_Error_None);

	uint64_t id_4 = 0;
	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_BYTE(200)
		, id_4
		);
	CHECK(error == Zakero_MemZone_Error_None);

	uint64_t id_5 = 0;
	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_BYTE(128)
		, id_5
		);
	CHECK(error == Zakero_MemZone_Error_None);

	error = Zakero_MemZone_Free(memzone, id_1);
	CHECK(error == Zakero_MemZone_Error_None);

	error = Zakero_MemZone_Free(memzone, id_3);
	CHECK(error == Zakero_MemZone_Error_None);

	error = Zakero_MemZone_Free(memzone, id_5);
	CHECK(error == Zakero_MemZone_Error_None);

	CHECK(Zakero_MemZone_Available_Largest(memzone) == ZAKERO_BYTE(256));

	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// {{{ Zakero_MemZone_Available_Total() -

/* {{function(name = Zakero_MemZone_Available_Total
 *   , param =
 *     [ { Zakero_MemZone& , memzone , The data. }
 *     ]
 *   , attr   = [ noexcept ]
 *   , brief  = Get the current size of the memory at ID.
 *   , return = The total amount of memory available for allocation.
 *   )
 * }}
 */
size_t Zakero_MemZone_Available_Total(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED // {{{
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
#		ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{
		return 0;
#		endif // }}}
	}
#endif // }}}

	Zakero_MemZone_Block* block  = memzone_block_first_(memzone);
	size_t                retval = 0;

	do
	{
		if(block_state_free_(block) == true)
		{
			retval += block->size;
		}

		block = block_next_(block);
	} while(block != nullptr);

	return retval;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/availalbe/total/") // {{{
{
	constexpr size_t TEST_SIZE = ZAKERO_BYTE(64);
	constexpr size_t OVERHEAD  = sizeof(Zakero_MemZone_Block);
	Zakero_MemZone   memzone   = {};

	SUBCASE("Uninitialized") // {{{
	{
		CHECK(Zakero_MemZone_Available_Total(memzone) == 0);
	} // }}}

	int error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_KILOBYTE(1)
		);
	CHECK(error == Zakero_MemZone_Error_None);

	CHECK(Zakero_MemZone_Available_Total(memzone) == ZAKERO_KILOBYTE(1));

	size_t expected_size = ZAKERO_KILOBYTE(1);

	uint64_t id_1 = 0;
	error = Zakero_MemZone_Allocate(memzone
		, TEST_SIZE
		, id_1
		);
	CHECK(error == Zakero_MemZone_Error_None);
	expected_size -= (TEST_SIZE + OVERHEAD);

	uint64_t id_2 = 0;
	error = Zakero_MemZone_Allocate(memzone
		, TEST_SIZE
		, id_2
		);
	CHECK(error == Zakero_MemZone_Error_None);
	expected_size -= (TEST_SIZE + OVERHEAD);

	CHECK(Zakero_MemZone_Available_Total(memzone) == expected_size);


	Zakero_MemZone_Free(memzone, id_1);
	expected_size += TEST_SIZE; // OVERHEAD is still used by the free block

	CHECK(Zakero_MemZone_Available_Total(memzone) == expected_size);

	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// {{{ Zakero_MemZone_Used_Largest() -

/* {{function(name = Zakero_MemZone_Used_Largest
 *   , param =
 *     [ { Zakero_MemZone& , memzone , The data. }
 *     ]
 *   , attr   = [ noexcept ]
 *   , brief  = Get the current size of the memory at ID.
 *   , return = The single largest amount of memory in use.
 *   )
 * }}
 */
size_t Zakero_MemZone_Used_Largest(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED // {{{
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
#		ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{
		return 0;
#		endif // }}}
	}
#endif // }}}

	Zakero_MemZone_Block* block  = memzone_block_first_(memzone);
	size_t                retval = 0;

	do
	{
		if(block_state_allocated_(block) == true)
		{
			retval = std::max(retval, block->size);
		}

		block = block_next_(block);
	} while(block != nullptr);

	return retval;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/used/largest/") // {{{
{
	Zakero_MemZone memzone = {};

	SUBCASE("Uninitialized") // {{{
	{
		CHECK(Zakero_MemZone_Used_Largest(memzone) == 0);
	} // }}}

	int error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, ZAKERO_KILOBYTE(1)
		);
	CHECK(error == Zakero_MemZone_Error_None);

	CHECK(Zakero_MemZone_Used_Largest(memzone) == 0);

	uint64_t id_1 = 0;
	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_BYTE(64)
		, id_1
		);
	CHECK(error == Zakero_MemZone_Error_None);

	uint64_t id_2 = 0;
	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_BYTE(256)
		, id_2
		);
	CHECK(error == Zakero_MemZone_Error_None);

	uint64_t id_3 = 0;
	error = Zakero_MemZone_Allocate(memzone
		, ZAKERO_BYTE(128)
		, id_3
		);
	CHECK(error == Zakero_MemZone_Error_None);

	CHECK(Zakero_MemZone_Used_Largest(memzone) == ZAKERO_BYTE(256));

	Zakero_MemZone_Free(memzone, id_1);
	Zakero_MemZone_Free(memzone, id_2);
	Zakero_MemZone_Free(memzone, id_3);
	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// {{{ Zakero_MemZone_Used_Total() -

/* {{function(name = Zakero_MemZone_Used_Total
 *   , param =
 *     [ { Zakero_MemZone& , memzone , The data. }
 *     ]
 *   , attr   = [ noexcept ]
 *   , brief  = Get the current size of the memory at ID.
 *   , return = The total size of all memory that is in use.
 *   )
 * }}
 */
size_t Zakero_MemZone_Used_Total(Zakero_MemZone& memzone
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED // {{{
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
#		ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{
		return 0;
#		endif // }}}
	}
#endif // }}}

	Zakero_MemZone_Block* block  = memzone_block_first_(memzone);
	size_t                retval = 0;

	do
	{
		retval += sizeof(Zakero_MemZone_Block);

		if(block_state_allocated_(block) == true)
		{
			retval += block->size;
		}

		block = block_next_(block);
	} while(block != nullptr);

	return retval;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/used/total/") // {{{
{
	constexpr size_t TEST_SIZE  = ZAKERO_BYTE(64);
	constexpr size_t TEST_COUNT = 1;
	constexpr size_t OVERHEAD   = sizeof(Zakero_MemZone_Block);
	Zakero_MemZone   memzone    = {};

	SUBCASE("Uninitialized") // {{{
	{
		CHECK(Zakero_MemZone_Used_Total(memzone) == 0);
	} // }}}

	int error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, (TEST_SIZE + OVERHEAD) * TEST_COUNT * 2
		);
	CHECK(error == Zakero_MemZone_Error_None);

	SUBCASE("Does Not Exist: Never Created") // {{{
	{
		CHECK(Zakero_MemZone_Size_Of(memzone, 0) == 0);
	} // }}}

	CHECK(Zakero_MemZone_Used_Total(memzone) == OVERHEAD); // of free block

	uint64_t id[TEST_COUNT];

	for(size_t i = 0; i < TEST_COUNT; i++)
	{
		Zakero_MemZone_Allocate(memzone
			, TEST_SIZE
			, id[i]
			);
	}

	size_t expected_total = 0;
	// The number of allocated memory blocks including header
	expected_total += (TEST_SIZE + OVERHEAD) * TEST_COUNT;
	// The header of the free memory block
	expected_total += OVERHEAD;

	CHECK(Zakero_MemZone_Used_Total(memzone) == expected_total);

	for(size_t i = 0; i < TEST_COUNT; i++)
	{
		Zakero_MemZone_Free(memzone, id[i]);
	}
	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// {{{ Zakero_MemZone_Size_Of() -

/* {{function(name = Zakero_MemZone_Size_Of
 *   , param =
 *     [ { Zakero_MemZone& , memzone , The data.      }
 *     , { uint64_t        , id      , The memory ID. }
 *     ]
 *   , attr   = [ noexcept ]
 *   , brief  = Get the current size of the memory at ID.
 *   , return = The size of the memory at ID in bytes.
 *   )
 * }}
 */
size_t Zakero_MemZone_Size_Of(Zakero_MemZone& memzone
	, uint64_t id
	) noexcept
{
#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED // {{{
	if(memzone.memory == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'memzone' has not been initialized.");
#		ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{
		return 0;
#		endif // }}}
	}
#endif // }}}

	Zakero_MemZone_Block* block = memzone_block_first_(memzone);
	block = block_find_id_(block, id, Zakero_MemZone_Block_Find_Forward);

#if ZAKERO_MEMZONE_VALIDATE_IS_ENABLED // {{{
	if(block == nullptr)
	{
		ZAKERO_MEMZONE_LOG_ERROR("Parameter 'id(%lu)' doses not exist.", id);
#		ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{
		return 0;
#		endif // }}}
	}
#endif // }}}

	return block->size;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/size-of/") // {{{
{
	constexpr size_t TEST_SIZE = ZAKERO_BYTE(64);
	Zakero_MemZone   memzone   = {};
	uint64_t         id        = 0;

	SUBCASE("Uninitialized") // {{{
	{
		CHECK(Zakero_MemZone_Size_Of(memzone, id) == 0);
	} // }}}

	int error = Zakero_MemZone_Init(memzone
		, Zakero_MemZone_Mode_RAM
		, TEST_SIZE
		);
	CHECK(error == Zakero_MemZone_Error_None);

	SUBCASE("Does Not Exist: Never Created") // {{{
	{
		CHECK(Zakero_MemZone_Size_Of(memzone, id) == 0);
	} // }}}

	error = Zakero_MemZone_Allocate(memzone
		, TEST_SIZE
		, id
		);
	CHECK(error == Zakero_MemZone_Error_None);

	CHECK(Zakero_MemZone_Size_Of(memzone, id) == TEST_SIZE);

	Zakero_MemZone_Free(memzone, id);
	Zakero_MemZone_Destroy(memzone);
} // }}}

#endif // }}}

// }}}
// {{{ Zakero_MemZone_Error_Message() -

/* {{function(name = Zakero_MemZone_Message
 *   , param =
 *     [ { int , error_code , The error code. }
 *     ]
 *   , attr  = [ noexcept ]
 *   , brief = Convert an error code into a string.
 *   )
 *   Use this function to get a breif description of the error_code.
 * }}
 */
const char* Zakero_MemZone_Error_Message(int error_code
	) noexcept
{
	switch(error_code)
	{
		default:
#define X(error_name_, error_code_, error_message_) \
		case error_code_: return error_message_;
ZAKERO_MEMZONE__ERROR_DATA
#undef X
	}

	return nullptr;
}

#ifdef ZAKERO_MEMZONE_IMPLEMENTATION_TEST // {{{

TEST_CASE("/c/error-message/") // {{{
{
#	define X(error_name_, error_code_, error_message_) \
	CHECK(Zakero_MemZone_Error_Message(error_code_) == error_message_);
	ZAKERO_MEMZONE__ERROR_DATA
#	undef X
	CHECK(Zakero_MemZone_Error_Message(-1) == Zakero_MemZone_Error_Message(Zakero_MemZone_Error_None));
} // }}}

#endif // }}}

// }}}
// }}}
// {{{ Implementation : C++ -
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
