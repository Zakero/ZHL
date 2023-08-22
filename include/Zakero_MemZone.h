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
	X(Error_None                  , 0 , "No Error"                                                     ) \

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
{	Zakero_MemZone_Defrag_Level_0 = 0 // No automatic defrag
,	Zakero_MemZone_Defrag_Level_1 = 1 // Defrag when memory is free'ed
,	Zakero_MemZone_Defrag_Level_2 = 2 // and when memory is released
,	Zakero_MemZone_Defrag_Level_3 = 3 // and when memory is acquired
,	Zakero_MemZone_Defrag_Level_4 = 4 // and when memory is allocated
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

enum Zakero_MemZone_Mode
{	Zakero_MemZone_Mode_FD
,	Zakero_MemZone_Mode_Alloc
};


struct Zakero_MemZone_BlockHeader
{
	uint64_t                    id;
	uint64_t                    flag;
	Zakero_MemZone_BlockHeader* next;
	Zakero_MemZone_BlockHeader* prev;
	size_t                      size;
};

struct Zakero_MemZone_Block
{
	struct Zakero_MemZone_BlockHeader header;
	uint8_t                           data[];
};

struct Zakero_MemZone
{
	uint8_t* memory = nullptr;
	int      fd     = -1;
};


int Zakero_MemZone_Init(Zakero_MemZone& //memzone
	, Zakero_MemZone_Mode //mode
	)
{
	return Zakero_MemZone_Error_None;
}

/*
int Zakero_MemZone_Allocate(Zakero_MemZone& memzone
	, size_t    size
	, uint64_t& id
	)
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
size_t Zakero_MemZone_Available_Largest(Zakero_MemZone& memzone
	)
size_t Zakero_MemZone_Available_Total(Zakero_MemZone& memzone
	)
size_t Zakero_MemZone_Used_Largest(Zakero_MemZone& memzone
	)
size_t Zakero_MemZone_Used_Total(Zakero_MemZone& memzone
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

// }}}

// {{{ Implementation : C
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
