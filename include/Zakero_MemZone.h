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
	#define Zakero_MemZone_ ## name_ value_
ZAKERO_MEMZONE__ERROR_DATA
#undef X

enum Zakero_MemZone_Block_Flag
{	Zakero_MemZone_Block_Flag_Allocated     = 0x0000'0000'0000'0001
,	Zakero_MemZone_Block_Flag_In_Use        = 0x0000'0000'0000'0010
,	Zakero_MemZone_Block_Flag_Clear_On_Free = 0x0000'0000'0000'0100
}

enum Zakero_MemZone_Defrag_Level
{	Zakero_MemZone_Defrag_Level_0 = 0 // No automatic defrag
,	Zakero_MemZone_Defrag_Level_1 = 1 // Defrag when memory is free'ed
,	Zakero_MemZone_Defrag_Level_2 = 2 // and when memory is released
,	Zakero_MemZone_Defrag_Level_3 = 3 // and when memory is acquired
,	Zakero_MemZone_Defrag_Level_4 = 4 // and when memory is allocated
}


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
	uint8_t[]                         data;
}


struct Zakero_MemZone
{
	uint8_t* memory = nullptr;
	int      fd     = -1;
}


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
	 * \name Lambda's that do nothing.
	 * \{
	 */
	zakero::MemZone::LambdaSize       LambdaSize_DoNothing       = [](size_t) noexcept {};
	zakero::MemZone::LambdaAddressMap LambdaAddressMap_DoNothing = [](const zakero::MemZone::AddressMap&) noexcept {};
	/**
	 * \}
	 */

	/**
	 * \brief Calculate the byte-aligned size.
	 *
	 * The byte-aligned size will be determined based on the provided \p 
	 * size.
	 *
	 * \return The byte-aligned size.
	 */
	inline off_t calculateActualSize(const off_t  size      ///< The size
		, const zakero::MemZone::Alignment alignment ///< The alignment
		)
	{
		const off_t mod  = static_cast<off_t>(alignment);
		const off_t step = static_cast<off_t>(alignment) + 1;

		return ((size + mod) / step) * step;
	}
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

	/**
	 * \brief Constructor.
	 *
	 * Create a new instance of the MemZone.  The provide file \p name 
	 * is not the name of a file on the file system.  The file \p name will 
	 * only exist in RAM with an optional backing store in swap in 
	 * available.
	 *
	 * \parcode
	 * zakero::MemZone memory_pool("The name of the MemZone");
	 * \endparcode
	 */
	MemZone::MemZone(const std::string& name ///< The file name
		) noexcept
		: memory(nullptr)
		, name(name)
		, mutex()
		, segment()
		, size_on_change(LambdaSize_DoNothing)
		, on_remap(LambdaAddressMap_DoNothing)
		, pool_size(0)
		, file_descriptor(-1)
		, alignment(zakero::MemZone::Alignment::Bits_64)
		, pool_can_expand(false)
	{
	}


	/**
	 * \brief Destructor
	 *
	 * Release all allocated resources.
	 */
	MemZone::~MemZone() noexcept
	{
		on_remap = LambdaAddressMap_DoNothing;
		segment.clear();
		name.clear();

		if(memory != nullptr)
		{
			#ifdef ZAKERO_MEMZONE_ZERO_ON_FREE
			memset(memory, '\0', pool_size);
			#endif

			munmap(memory, pool_size);
			memory = nullptr;
		}

		close(file_descriptor);
		file_descriptor = -1;

		pool_size = 0;
	}


	/**
	 * \brief Initialize the MemZone.
	 *
	 * The MemZone must be initialized before it can be used.  At a 
	 * minimum, the size of the MemZone must be specified in bytes and 
	 * the size must be greater than 0.
	 *
	 * The maximum allowable size is MemZone::Size_Max which represents 
	 * the largest offset value supported by the MemZone.  Your hardware 
	 * configuration and/or operating system may lower this limit.
	 *
	 * Setting the \p expandable flag to `true` will allow the MemZone 
	 * to grow to a larger size.  If an allocation request is made that is 
	 * larger that the largest available contiguous space available, then 
	 * the Memory Pool will expand just enough to accommodate the request.
	 *
	 * The Byte Boundary of all the allocations for this MemZone object 
	 * is specified by the \p alignment.
	 *
	 * \note The size of the MemZone will never shrink.
	 *
	 * \parcode
	 * zakero::MemZone rgba_textures("Active Texture Cache");
	 *
	 * #define KILOBYETS(size_) ((size_) * 1024)
	 * #define MEGABYTES(size_) (KILOBYTES(size_) * 1024)
	 * rgba_textures.init(MEGABYTES(32)
	 * 	, false // Restricted environment, no expanding
	 * 	, zakero::MemZone::Alignment::Bits_32 // RGBA == 32 bits
	 * 	);
	 * \endparcode
	 *
	 * \return An error condition.  If there was no error, then the value 
	 * of the error condition will be `0`.
	 */
	std::error_code MemZone::init(const size_t size       ///< The initial size in bytes
		, const bool                          expandable ///< Allow the MemZone to expand
		, const MemZone::Alignment         alignment  ///< The Byte Alignment
		) noexcept
	{
		//ZAKERO_PROFILER_DURATION("MemZone", "init");

		if(this->file_descriptor != -1)
		{
			return ZAKERO_MEMZONE__ERROR(Error_Already_Initialized);
		}

		if(size <= 0)
		{
			return ZAKERO_MEMZONE__ERROR(Error_Size_Too_Small);
		}

		if(size > MemZone::Size_Max)
		{
			return ZAKERO_MEMZONE__ERROR(Error_Size_Too_Large);
		}

		size_t pool_size = calculateActualSize(size, alignment);

		#ifdef __linux__
		int fd = memfd_create(name.c_str(), 0);
		#else
		#error Need more code...
		#endif

		if(fd == -1)
		{
			return ZAKERO_MEMZONE__ERROR(Error_Failed_To_Create_File);
		}

		if(ftruncate(fd, pool_size) == -1)
		{
			return ZAKERO_MEMZONE__ERROR(Error_Failed_To_Resize_File);
		}

		uint8_t* memory = (uint8_t*)mmap(nullptr
			, pool_size
			, PROT_READ | PROT_WRITE
			, MAP_SHARED | MAP_NORESERVE
			, fd
			, 0
			);

		if(memory == MAP_FAILED)
		{
			close(fd);

			return ZAKERO_MEMZONE__ERROR(Error_Failed_To_Map_File);
		}

		this->segment.push_back(
		{	.offset = 0
		,	.size   = (off_t)pool_size
		,	.in_use = false
		});

		this->pool_can_expand = expandable;
		this->alignment       = alignment;
		this->pool_size       = pool_size;
		this->memory          = memory;
		this->file_descriptor = fd;

		return ZAKERO_MEMZONE__ERROR(Error_None);
	}


	/**
	 * \brief The backing file descriptor.
	 *
	 * If something needs to be able to map the same region of memory as 
	 * this MemZone, then this method will provide the file descriptor 
	 * to do it.
	 *
	 * The entire MemZone will be accessible from the file descriptor.
	 *
	 * \parcode
	 * // Get read access to the shared memory
	 * uint8_t* mem_reader = (uint8_t*)mmap(nullptr
	 * 	, memory_pool.size()
	 * 	, PROT_READ
	 * 	, MAP_SHARED | MAP_NORESERVE
	 * 	, memory_pool.fd()
	 * 	);
	 * \endparcode
	 *
	 * \see MemZone::size()
	 *
	 * \return The file descriptor.
	 */
	int MemZone::fd() const noexcept
	{
		return file_descriptor;
	}


	/**
	 * \brief The size of the memory pool.
	 *
	 * The current size of the MemZone, in bytes, will be returned.
	 *
	 * \return The size of the memory pool.
	 */
	size_t MemZone::size() const noexcept
	{
		return pool_size;
	}


	/**
	 * \brief Set the Size Event callback.
	 *
	 * If the MemZone was configured to be able to dynamically expand as 
	 * needed (see init()), then the provided \p lambda will be called when 
	 * the memory pool changes size.  Using the sizeOnChange() method 
	 * before calling init() will not cause the lambda to be executed when 
	 * init() is called.
	 *
	 * The \p lambda will receive the new size, in bytes, of the memory 
	 * pool.
	 *
	 * \parcode
	 * zakero::MemZone memory_pool("Size Matters");
	 *
	 * memory_pool.init(1, true);
	 *
	 * memory_pool.sizeOnChange([](size_t new_size)
	 * {
	 * 	std::cout << "Size: " << size_t << "\n";
	 * });
	 *
	 * off_t uno = memory_pool.alloc(256);
	 * off_t dos = memory_pool.alloc(512);
	 * \endparcode
	 *
	 * \note The MemZone will be in a "locked state" so any call from 
	 * the lambda to a non-const MemZone method will block indefinitely.
	 */
	void MemZone::sizeOnChange(MemZone::LambdaSize lambda ///< The callback
		) noexcept
	{
		if(lambda == nullptr)
		{
			size_on_change = LambdaSize_DoNothing;
		}
		else
		{
			size_on_change = lambda;
		}
	}


	/**
	 * \brief Allocate memory from the pool.
	 *
	 * The requested \p size (in bytes) will be allocated from the memory 
	 * pool.  If the memory could not be allocated, then `-1` will be 
	 * returned.
	 *
	 * \parcode
	 * zakero::MemZone memory_pool("foo");
	 * memory_pool.init(1024);
	 *
	 * off_t data_offset = memory_pool.alloc(128);
	 * \endparcode
	 *
	 * \note The contents of the memory is undefined.
	 *
	 * \return The offset of the block of memory.
	 */
	off_t MemZone::alloc(const size_t size ///< The size in bytes
		) noexcept
	{
		std::error_code error;

		return alloc(size, error);
	}


	/**
	 * \brief Allocate memory from the pool.
	 *
	 * The requested \p size (in bytes) will be allocated from the memory 
	 * pool.  If the memory could not be allocated, then `-1` will be 
	 * returned and the reason will be stored in \p error.
	 *
	 * \parcode
	 * zakero::MemZone memory_pool("foo");
	 * memory_pool.init(1024);
	 *
	 * std::error_code error;
	 * off_t data_offset = memory_pool.alloc(512, error);
	 *
	 * if(error.value() != 0)
	 * {
	 * 	std::cerr << "Error: " << error.message() << "\n";
	 * }
	 * \endparcode
	 *
	 * \note The contents of the memory is undefined.
	 *
	 * \return The offset of the block of memory.
	 */
	off_t MemZone::alloc(const size_t size  ///< The size in bytes
		, std::error_code&           error ///< The error
		) noexcept
	{
		//ZAKERO_PROFILER_DURATION("MemZone", "alloc");

		// Validate Input

		if(size <= 0)
		{
			error = ZAKERO_MEMZONE__ERROR(Error_Size_Too_Small);
			return -1;
		}

		if(size > MemZone::Size_Max)
		{
			error = ZAKERO_MEMZONE__ERROR(Error_Size_Too_Large);
			return -1;
		}

		const size_t segment_size = calculateActualSize(size, alignment);

		// Allocate from the pool

		std::lock_guard<std::mutex> lock(mutex);

		size_t index = 0;

		if(segmentFindBestFit(segment_size, index) == false)
		{
			if(pool_can_expand == false)
			{
				error = ZAKERO_MEMZONE__ERROR(Error_Out_Of_Memory);
				return -1;
			}

			if(expandToFit(segment_size, index) == false)
			{
				error = ZAKERO_MEMZONE__ERROR(Error_Failed_To_Resize_File);
				return -1;
			}
		}

		error = ZAKERO_MEMZONE__ERROR(Error_None);

		segment[index].in_use = true;

		if((size_t)segment[index].size == segment_size)
		{
			return segment[index].offset;
		}

		segmentSplit(index, segment_size);

		return segment[index].offset;
	}


	/**
	 * \brief Allocate memory from the pool.
	 *
	 * The requested \p size (in bytes) will be allocated from the memory 
	 * pool.  If the memory could not be allocated, then `-1` will be 
	 * returned.
	 *
	 * The every byte of the allocated memory will be set to \p value.
	 *
	 * \parcode
	 * zakero::MemZone memory_pool("foo");
	 * memory_pool.init(1024);
	 *
	 * off_t data_offset = memory_pool.alloc(512, uint8_t(0xa5));
	 * \endparcode
	 *
	 * \return The offset of the block of memory.
	 */
	off_t MemZone::alloc(size_t size  ///< The size in bytes
		, uint8_t              value ///< The fill value
		) noexcept
	{
		std::error_code error;

		return alloc(size, value, error);
	}


	/**
	 * \brief Allocate memory from the pool.
	 *
	 * The requested \p size (in bytes) will be allocated from the memory 
	 * pool.  If the memory could not be allocated, then `-1` will be 
	 * returned and the reason will be stored in \p error.
	 *
	 * The every byte of the allocated memory will be set to \p value.
	 *
	 * \parcode
	 * zakero::MemZone memory_pool("foo");
	 * memory_pool.init(1024);
	 *
	 * std::error_code error;
	 * off_t data_offset = memory_pool.alloc(512, uint8_t(0xa5), error);
	 *
	 * if(error.value() != 0)
	 * {
	 * 	std::cerr << "Error: " << error.message() << "\n";
	 * }
	 * \endparcode
	 *
	 * \return The offset of the block of memory.
	 */
	off_t MemZone::alloc(size_t size  ///< The size in bytes
		, uint8_t              value ///< The fill value
		, std::error_code&     error ///< The error
		) noexcept
	{
		off_t offset = alloc(size, error);

		if(offset < 0)
		{
			return offset;
		}

		uint8_t* ptr = addressOf(offset);

		memset(ptr, value, size);

		return offset;
	}


	/**
	 * \brief Allocate memory from the pool.
	 *
	 * The requested \p size (in bytes) will be allocated from the memory 
	 * pool.  If the memory could not be allocated, then `-1` will be 
	 * returned.
	 *
	 * Every 32-bit in the allocated memory will be set to \p value.  Any 
	 * bytes that leftover will be undefined.  For example, 10 byte 
	 * allocation with a \p value of 0xaaaa5555.  The last 2 bytes will be 
	 * undefined.  Memory Contents: `aaaa5555aaaa5555??`
	 *
	 * \parcode
	 * zakero::MemZone memory_pool("foo");
	 * memory_pool.init(1024);
	 *
	 * off_t data_offset = memory_pool.alloc(512, uint32_t(0xaaaa5555));
	 * \endparcode
	 *
	 * \return The offset of the block of memory.
	 */
	off_t MemZone::alloc(size_t size  ///< The size in bytes
		, uint32_t             value ///< The fill value
		) noexcept
	{
		std::error_code error;

		return alloc(size, value, error);
	}


	/**
	 * \brief Allocate memory from the pool.
	 *
	 * The requested \p size (in bytes) will be allocated from the memory 
	 * pool.  If the memory could not be allocated, then `-1` will be 
	 * returned and the reason will be stored in \p error.
	 *
	 * Every 32-bit in the allocated memory will be set to \p value.  Any 
	 * bytes that leftover will be undefined.  For example, 10 byte 
	 * allocation with a \p value of 0xaaaa5555.  The last 2 bytes will be 
	 * undefined.  Memory Contents: `aaaa5555aaaa5555??`
	 *
	 * \parcode
	 * zakero::MemZone memory_pool("foo");
	 * memory_pool.init(1024);
	 *
	 * std::error_code error;
	 * const uint32_t fill = 0xaaaa5555;
	 * off_t data_offset = memory_pool.alloc(512, fill, error);
	 *
	 * if(error.value() != 0)
	 * {
	 * 	std::cerr << "Error: " << error.message() << "\n";
	 * }
	 * \endparcode
	 *
	 * \return The offset of the block of memory.
	 */
	off_t MemZone::alloc(size_t size  ///< The size in bytes
		, const uint32_t       value ///< The fill value
		, std::error_code&     error ///< The error
		) noexcept
	{
		off_t offset = alloc(size, error);

		if(offset < 0)
		{
			return offset;
		}

		size_t count = size / 4;
		uint32_t* p = (uint32_t*)addressOf(offset);

		for(uint32_t i = 0; i < count; i++)
		{
			p[i] = value;
		}

		return offset;
	}


	/**
	 * \brief Free allocated memory.
	 *
	 * The allocated memory at the provided \p offset will be free'ed.  The 
	 * \p offset will be set to `-1`.
	 *
	 * If the \p offset is not valid, its value will not be changed.
	 *
	 * \parcode
	 * zakero::MemZone memory_pool("A Memory Pool");
	 *
	 * memory_pool.init(128);
	 *
	 * off_t offset = memory_pool.alloc(64);
	 *
	 * // Do stuff
	 *
	 * memory_pool.free(offset);
	 * \endparcode
	 */
	void MemZone::free(off_t& offset ///< The memory to free
		) noexcept
	{
		//ZAKERO_PROFILER_DURATION("MemZone", "free");

		std::lock_guard<std::mutex> lock(mutex);

		size_t index = 0;

		if(segmentFindInUse(offset, index) == false)
		{
			return;
		}

		#ifdef ZAKERO_MEMZONE_ZERO_ON_FREE
		memset(memory + segment[index].offset
			, '\0'
			, segment[index].size
			);
		#endif

		segment[index].in_use = false;

		segmentMergeNext(index);
		segmentMergePrev(index);

		offset = -1;
	}


	/**
	 * \brief Change the size of allocated memory.
	 *
	 * This method is similar to std::realloc(), in that it will resize the 
	 * allocated memory at the given \p offset.  If the resize was 
	 * successful, the new offset will be returned.
	 *
	 * The contents of the memory will be preserved.
	 *
	 * The return value will be `-1` if the allocated memory could not be 
	 * resized.
	 *
	 * \parcode
	 * zakero::MemZone memory_pool("Resizing in the Pool");
	 *
	 * memory_pool.init(128);
	 *
	 * off_t offset = memory_pool.alloc(64);
	 *
	 * // Do stuff
	 * // Oops, need more space
	 *
	 * offset = memory_pool.resize(offset, 96);
	 * \endparcode
	 *
	 * \return The offset of the resized memory location.
	 */
	off_t MemZone::realloc(off_t offset ///< The memory to resize
		, size_t                size   ///< The size in bytes
		) noexcept
	{
		std::error_code error;

		return realloc(offset, size, error);
	}


	/**
	 * \brief Change the size of allocated memory.
	 *
	 * This method is similar to std::realloc(), in that it will resize the 
	 * allocated memory at the given \p offset.  If the resize was 
	 * successful, the new offset will be returned.
	 *
	 * The contents of the memory will be preserved.
	 *
	 * The return value will be `-1` if the allocated memory could not be 
	 * resized and the reason will be stored in \p error.
	 *
	 * \parcode
	 * zakero::MemZone memory_pool("Resizing in the Pool");
	 *
	 * memory_pool.init(128);
	 *
	 * off_t offset = memory_pool.alloc(64);
	 *
	 * // Do stuff
	 * // Oops, need more space
	 *
	 * std::error_code error;
	 * auto new_offset = memory_pool.resize(offset, 96, error);
	 * if(error(bool) == true)
	 * {
	 * 	std::cerr << "Error: " << error.message() << "\n";
	 * }
	 * else
	 * {
	 * 	offset = new_offset;
	 * }
	 * \endparcode
	 *
	 * \return The offset of the resized memory location.
	 */
	off_t MemZone::realloc(off_t offset ///< The memory to resize
		, size_t                size   ///< The size in bytes
		, std::error_code&      error  ///< The error
		) noexcept
	{
		//ZAKERO_PROFILER_DURATION("MemZone", "realloc");

		// Validate Input

		if(size <= 0)
		{
			error = ZAKERO_MEMZONE__ERROR(Error_Size_Too_Small);
			return -1;
		}

		if(size > MemZone::Size_Max)
		{
			error = ZAKERO_MEMZONE__ERROR(Error_Size_Too_Large);
			return -1;
		}

		const size_t segment_size = calculateActualSize(size, alignment);

		std::lock_guard<std::mutex> lock(mutex);

		size_t index_src = 0;

		if(segmentFindInUse(offset, index_src) == false)
		{
			error = ZAKERO_MEMZONE__ERROR(Error_Invalid_Offset);
			return -1;
		}

		Segment& segment_src = segment[index_src];

		// Same size, nothing to do
		if(segment_size == (size_t)segment_src.size)
		{
			error = ZAKERO_MEMZONE__ERROR(Error_None);
			return offset;
		}

		// New size is smaller, shrink segment
		if(segment_size < (size_t)segment_src.size)
		{
			segmentSplit(index_src, segment_size);

			error = ZAKERO_MEMZONE__ERROR(Error_None);
			return offset;
		}

		// Larger, try to expand into the next segment
		if(segmentExpand(index_src, segment_size) == true)
		{
			error = ZAKERO_MEMZONE__ERROR(Error_None);
			return offset;
		}

		// Larger, find a new location
		size_t index_dst = 0;

		if(segmentFindBestFit(segment_size, index_dst) == false)
		{
			if(pool_can_expand == false)
			{
				error = ZAKERO_MEMZONE__ERROR(Error_Out_Of_Memory);
				return -1;
			}

			if(expandToFit(segment_size, index_dst) == false)
			{
				error = ZAKERO_MEMZONE__ERROR(Error_Failed_To_Resize_File);
				return -1;
			}
		}

		index_dst = segmentMove(index_src, segment_size, index_dst);

		offset = segment[index_dst].offset;

		error = ZAKERO_MEMZONE__ERROR(Error_None);
		return offset;
	}


	/**
	 * \brief Convert an offset into a pointer.
	 *
	 * The provided \p offset will be converted into an address that can be 
	 * de-referenced as a normal C-Style pointer.  If the \p offset is not 
	 * valid, then \nullptr will be returned.
	 *
	 * \parcode
	 * off_t offset = memory_pool.alloc(256);
	 * uint8_t* ptr = memory_pool.addressOf(offset);
	 * \endparcode
	 *
	 * \note If the MemZone expands and is relocated, the returned 
	 * pointers will no longer be valid.
	 *
	 * \see onRemap()
	 *
	 * \return An address
	 */
	uint8_t* MemZone::addressOf(off_t offset ///< The offset
		) const noexcept
	{
		//ZAKERO_PROFILER_DURATION("MemZone", "addressOf");

		size_t index = 0;

		if(segmentFindInUse(offset, index) == false)
		{
			return nullptr;
		}

		return memory + offset;
	}


	/**
	 * \brief Set the Remap Event callback.
	 *
	 * There are times when the MemZone will move allocated data.  When 
	 * this happens, the provided \p lambda will be called so that the 
	 * caller will have an opportunity to update their pointers.
	 *
	 * The lambda will receive a map of addresses where the key is the old 
	 * address and the value is the new address.
	 *
	 * If the MemZone was configured to be _not expandable_ (see 
	 * init()), then the MemZone will never have a need to move it's 
	 * region of memory.  Therefore, the \p lambda will never be called and 
	 * the pointers will never become invalid (unless the memory is freed).
	 *
	 * \parcode
	 * uint8_t* secret = nullptr;
	 *
	 * memory_pool.onRemap([&](const zakero::MemZone::AddressMap& map)
	 * {
	 * 	secret = zakero::MemZone::remap(map, secret);
	 * });
	 *
	 * off_t secret_offset = memory_pool.alloc(512);
	 * secret = memory_pool.addressOf(secret_offset);
	 * \endparcode
	 *
	 * \note The MemZone will be in a "locked state" so any call from 
	 * the lambda to a non-const MemZone method will block indefinitely.
	 *
	 * \see remap()
	 */
	void MemZone::onRemap(MemZone::LambdaAddressMap lambda ///< The callback
		) noexcept
	{
		if(lambda == nullptr)
		{
			on_remap = LambdaAddressMap_DoNothing;
		}
		else
		{
			on_remap = lambda;
		}
	}


	/**
	 * \brief Get the new memory address.
	 *
	 * Lookup the provided \p address in the \p addr_map and return the new 
	 * address.  If the \p address was not in the \p addr_map, the value of 
	 * the \p address will be returned.
	 * 
	 * \note This is a convenience method whose only purpose is to improve 
	 * the quality-of-life of the MemZone API.
	 *
	 * \see onRemap()
	 *
	 * \return A memory address.
	 */
	uint8_t* MemZone::remap(const MemZone::AddressMap& addr_map ///< A map of old/new addresses
		, uint8_t*                                       address  ///< An old memory address
		) noexcept
	{
		if(addr_map.contains(address))
		{
			return addr_map.at(address);
		}

		return address;
	}


	/**
	 * \brief Output the internal state.
	 *
	 * The internal state of the MemZone will be converted into a JSON 
	 * formatted string.  The JSON string will contain the following:
	 * - The name of the anonymous file
	 * - The size of the memory pool
	 * - A list of segments and information about each segment
	 * - A layout of memory usage
	 *
	 * The content of the layout is controlled by the \p 
	 * bytes_per_character and \p characters_per_line parameters.
	 *
	 * \parcode
	 * zakero::MemZone memory_pool("Dump It");
	 *
	 * memory_pool(512);
	 *
	 * off_t o1 = memory_pool.alloc(64);
	 * off_t o2 = memory_pool.alloc(64);
	 * off_t o3 = memory_pool.alloc(128);
	 * off_t o4 = memory_pool.alloc(256);
	 * memory_pool.free(o2);
	 *
	 * memory_pool.dump(1, 128);
	 * \endparcode
	 *
	 * \return The JSON formatted string.
	 */
	std::string MemZone::dump(size_t bytes_per_character ///< Used to determine how many allocated bytes are represented by each character in the layout.
		, size_t                    characters_per_line ///< The length of each layout line.
		) const noexcept
	{
		std::lock_guard<std::mutex> lock(mutex);

		std::string str = "{ \"name\": \"" + name + "\"\n"
			+ ", \"pool_size\": " + std::to_string(pool_size) + "\n"
			;

		size_t count = 0;

		str += ", \"Segment\":\n";
		std::string delim = "  [ ";
		for(const auto& seg : segment)
		{
			str += delim;
			str += "{ \"offset\": " + std::to_string(seg.offset)
				+ ", \"size\": "   + std::to_string(seg.size)
				+ ", \"in_use\": " + std::to_string(seg.in_use)
				+ " }\n";

			count++;
			delim = "  , ";
		}
		str += "  ]\n";

		count = 0;

		int character = 'A';

		str += ", \"Layout\":\n  [ \"";
		for(const auto& seg : segment)
		{
			int ch = '.';
			if(seg.in_use)
			{
				ch = character;
			}

			size_t segment_count = seg.size / bytes_per_character;

			while(segment_count > 0)
			{
				if((count + segment_count) >= characters_per_line)
				{
					size_t t = characters_per_line - count;

					str += std::string(t, ch) + "\"\n  , \"";

					count = 0;
					segment_count -= t;
				}
				else
				{
					str += std::string(segment_count, ch);
					
					count += segment_count;
					segment_count = 0;
				}
			}

			if(ch != '.')
			{
				if(character == 'Z')
				{
					character = 'A';
				}
				else
				{
					character++;
				}
			}
		}

		str += "\"\n  ]\n}";

		return str;
	}


	/**
	 * \brief Information about the current internal state.
	 *
	 * \return A collection Segments
	 */
	MemZone::VectorSegment MemZone::segmentList(
		) const noexcept
	{
		std::lock_guard<std::mutex> lock(mutex);

		return segment;
	}


	/**
	 * \brief Increase the size of the memory pool.
	 *
	 * \retval true  The memory pool size was increased.
	 * \retval false The memory pool size was __not__ increased.
	 */
	bool MemZone::expandToFit(size_t size_increase ///< The amount of the increase
		, size_t&                   index         ///< The new index
		) noexcept
	{
		//ZAKERO_PROFILER_DURATION("MemZone", "expandToFit");

		if(pool_can_expand == false)
		{
			return false;
		}

		if(segment.back().in_use == false)
		{
			size_increase -= segment.back().size;
		}

		if(pool_size + size_increase > MemZone::Size_Max)
		{
			return false;
		}

		uint8_t*     old_memory = memory;
		const size_t old_size   = pool_size;
		const size_t new_size   = pool_size + size_increase;

		if(ftruncate(file_descriptor, new_size) == -1)
		{
			return false;
		}

		void* new_memory = mremap(old_memory
			, old_size
			, new_size
			, MREMAP_MAYMOVE
			);

		if(new_memory == MAP_FAILED)
		{
			return false;
		}

		pool_size = new_size;

		index = segment.size();
		if(index > 0
			&& segment[index - 1].in_use == false
			)
		{
			index--;
			segment[index].size += (off_t)size_increase;
		}
		else
		{
			segment.push_back(
			{	.offset = (off_t)old_size
			,	.size   = (off_t)size_increase
			,	.in_use = false
			});
		}

		size_on_change(pool_size);

		if(new_memory != old_memory)
		{
			memory = (uint8_t*)new_memory;

			MemZone::AddressMap addr_map;

			for(const auto& seg : segment)
			{
				if(seg.in_use)
				{
					uint8_t* old_addr = old_memory + seg.offset;
					uint8_t* new_addr = memory + seg.offset;

					addr_map[old_addr] = new_addr;
				}
			}

			on_remap(addr_map);
		}

		return true;
	}


	/**
	 * \brief Increase the size of a segment.
	 *
	 * The size of the specified segment at \p index will be increased by 
	 * using the available space of the next segment.  But only if that 
	 * next segment is not in_use.
	 *
	 * \retval true  The segment was expanded.
	 * \retval false The segment was __not__ expanded.
	 */
	bool MemZone::segmentExpand(const size_t index ///< The segment index
		, const size_t                      size  ///< The new size
		) noexcept
	{
		//ZAKERO_PROFILER_DURATION("MemZone::Segment", "Expand");

		const size_t index_next = index + 1;
		if(index_next >= segment.size())
		{
			return false;
		}

		Segment& segment_next = segment[index_next];

		if(segment_next.in_use == true)
		{
			return false;
		}

		Segment& segment_this = segment[index];

		if(size > size_t(segment_this.size + segment_next.size))
		{
			return false;
		}

		const size_t size_next = size - segment_this.size;

		segment_this.size = size;

		if(size_next == 0)
		{
			segment.erase(std::begin(segment) + index_next);
		}
		else
		{
			segment_next.offset += size_next;
			segment_next.size   -= size_next;
		}

		return true;
	}


	/**
	 * \brief Find a segment.
	 *
	 * Look for a segment that has a size greater than (or equal to) \p 
	 * size and is not in use.
	 *
	 * \return The index of the segment.
	 */
	bool MemZone::segmentFindBestFit(const size_t size ///< The minimum size
		, size_t& index                               ///< The index if found
		) noexcept
	{
		//ZAKERO_PROFILER_DURATION("MemZone::Segment", "FindBestFit");

		for(size_t i = 0; i < segment.size(); i++)
		{
			const Segment& seg = segment[i];

			if(seg.in_use == false && (size_t)seg.size >= size)
			{
				index = i;

				return true;
			}
		}

		return false;
	}


	/**
	 * \brief Find a segment.
	 *
	 * Find the segment that starts with the specified \p offset.
	 *
	 * \return The index of the segment.
	 */
	bool MemZone::segmentFindInUse(const off_t offset ///< The offset to find
		, size_t&                             index  ///< The index if found
		) const noexcept
	{
		//ZAKERO_PROFILER_DURATION("MemZone::Segment", "FindInUse");

		for(size_t i = 0; i < segment.size(); i++)
		{
			const Segment& seg = segment[i];

			if(seg.offset > offset)
			{
				return false;
			}

			if(seg.offset == offset && seg.in_use == true)
			{
				index = i;

				return true;
			}
		}

		return false;
	}


	/**
	 * \brief Combine 2 segments into 1.
	 *
	 * The segment at the specified index will be merged with the next 
	 * segment (index + 1), but only if the following conditions are true:
	 * - (index + 1) is a valid segment index
	 * - The next segment is __not__ in use
	 *
	 * The "next" segment that is after the provided index will be erased.
	 */
	void MemZone::segmentMergeNext(const size_t index ///< The segment to merge
		) noexcept
	{
		//ZAKERO_PROFILER_DURATION("MemZone::Segment", "MergeNext");

		size_t index_next = index + 1;

		if(index_next >= segment.size())
		{
			return;
		}

		Segment& segment_next = segment[index_next];

		if(segment_next.in_use == true)
		{
			return;
		}

		segment[index].size += segment[index_next].size;

		segment.erase(std::begin(segment) + index_next);
	}


	/**
	 * \brief Combine 2 segments into 1.
	 *
	 * The segment at the specified index will be merged with the previous 
	 * segment (index - 1), but only if the following conditions are true:
	 * - (index - 1) is a valid segment index
	 * - The previous segment is __not__ in use
	 *
	 * The "previous" segment that is before the provided index will be 
	 * erased.
	 *
	 * \note The provided \p index will no longer be valid if the merge is 
	 * successful.
	 */
	void MemZone::segmentMergePrev(const size_t index ///< The segment to merge
		) noexcept
	{
		//ZAKERO_PROFILER_DURATION("MemZone::Segment", "MergePrev");

		if(index == 0)
		{
			return;
		}

		const size_t index_prev = index - 1;

		Segment& segment_prev = segment[index_prev];

		if(segment_prev.in_use == true)
		{
			return;
		}

		segment_prev.size += segment[index].size;

		segment.erase(std::begin(segment) + index);
	}


	/**
	 * \brief Move the contents of a segment.
	 *
	 * The contents of the segment at \p src_index will be moved to the \p 
	 * dst_index segment.  The \p dst_size parameter is used to determine 
	 * the resulting size of the \p dst_index segment.
	 *
	 * \note No error checking is done, all provided values are expected to 
	 * be correct and valid.
	 *
	 * It is expected that \p dst_size will be in the range of:
	 *     segment[src_index].size <= dst_size <= segment[dst_index].size
	 *
	 * The segment at \p src_index will be destroyed.
	 *
	 * \return The index of the destination segment.
	 */
	size_t MemZone::segmentMove(const size_t src_index ///< The source segment
		, const size_t                      dst_size  ///< The destination size
		, size_t&                           dst_index ///< The destination segment
		) noexcept
	{
		//ZAKERO_PROFILER_DURATION("MemZone::Segment", "Move");

		Segment&       src_segment = segment[src_index];
		const uint8_t* src_addr    = memory + src_segment.offset;
		const size_t   src_size    = src_segment.size;

		Segment&     dst_segment = segment[dst_index];
		uint8_t*     dst_addr    = memory + dst_segment.offset;
		const size_t dst_offset  = dst_segment.offset;

		dst_segment.in_use = true;

		memcpy(dst_addr, src_addr, src_size);

		#ifdef ZAKERO_MEMZONE_ZERO_ON_FREE
		memset(src_addr, '\0', src_size);
		#endif
		src_segment.in_use = false;

		if(src_index > dst_index)
		{
			segmentMergeNext(src_index);
			segmentMergePrev(src_index);

			segmentSplit(dst_index, dst_size);
		}
		else
		{
			segmentSplit(dst_index, dst_size);

			segmentMergeNext(src_index);
			segmentMergePrev(src_index);

			segmentFindInUse(dst_offset, dst_index);
		}

		return dst_index;
	}


	/**
	 * \brief Split a segment.
	 *
	 * The segment at the provided \p index will be split into two 
	 * segments.  The first segment will
	 * - have the specified \p size in bytes
	 * - inherit the segment's `in_use` status
	 *
	 * The other segment will
	 * - have the remaining space
	 * - be flagged as not `in_use`
	 *
	 * \note No error checking is done.  The provided values are expected 
	 * to be correct and valid.
	 *
	 * \note Splitting a segment that has `in_use == false` is effectively 
	 * an expensive no-op.
	 */
	void MemZone::segmentSplit(size_t index ///< The segment to split
		, size_t                     size  ///< The "new" segment size
		) noexcept
	{
		//ZAKERO_PROFILER_DURATION("MemZone::Segment", "Split");

		Segment& this_segment = segment[index];

		const size_t index_next  = index + 1;
		const off_t  offset_next = this_segment.offset + size;
		const off_t  size_next   = this_segment.size - size;

		this_segment.size = size;

		if(this_segment.in_use)
		{
			uint8_t* addr = memory + offset_next;

			memset(addr, '\0', size_next);
		}

		if(index_next >= segment.size())
		{
			segment.push_back(
			{	.offset = offset_next
			,	.size   = size_next
			,	.in_use = false
			});
		}
		else
		{
			Segment& segment_next = segment[index_next];

			if(segment_next.in_use)
			{
				segment.insert(std::begin(segment) + index_next,
				{	.offset = offset_next
				,	.size   = size_next
				,	.in_use = false
				});
			}
			else // Not in use
			{
				segment_next.offset  = offset_next;
				segment_next.size   += size_next;
			}
		}
	}

// }}}
// {{{ Convenience

	/**
	 * \brief Convert VectorSegment into a JSON formated string.
	 *
	 * \return The JSON formatted string.
	 */
	std::string to_string(const MemZone::VectorSegment& segment ///< The value to convert
		) noexcept
	{
		std::string str = "";

		std::string delim = "[ ";
		for(const auto& seg : segment)
		{
			str += delim;
			str += "{ \"offset\": " + std::to_string(seg.offset)
				+ ", \"size\": "   + std::to_string(seg.size)
				+ ", \"in_use\": " + std::to_string(seg.in_use)
				+ " }\n";

			delim = ", ";
		}
		str += "]\n";

		return str;
	}

// }}}

}
// }}}

#endif // ZAKERO_MEMZONE_IMPLEMENTATION

// }}}

#endif // zakero_MemZone_h
