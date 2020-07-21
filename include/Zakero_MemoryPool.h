/* *******************************************************************
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * \file
 *
 * \brief Zakero MemoryPool
 *
 * See zakero::MemoryPool for the API documentation.
 * 
 * \par TL;DR:
 *
 * This library will provide a memory pool for your application.
 * To use:
 * 1. Add the implementation to a source code file:
 *    ~~~
 *    #define ZAKERO_MEMORYPOOL_IMPLEMENTATION
 *    #include "path/to/Zakero_MemoryPool.h"
 *    ~~~
 * 2. Add the library to where it is used:
 *    ~~~
 *    #include "path/to/Zakero_MemoryPool.h"
 *    ~~~
 *
 * \par What Is It?
 * \parblock
 * The _Zakero MemoryPool_ library will create and manage a region of memory.
 * From this pool of memory, sections of memory can be allocated and freed.
 * When allocated, the memory is identified by an offset into the region of 
 * memory instead of a pointer.  Programs are expected to be "good citizens" by 
 * using the offset and not writing outside of their allocated area.
 *
 * The region of memory is anchored to an anonymous file descriptor.  The 
 * benefit of using the file description is that the Operating System can remap 
 * the file to a larger area as needed.  And since all allocated memory uses an 
 * offset, no pointers end up pointing to a bad location.
 * \endparblock
 *
 * \par Why Use It?
 * \parblock
 * As with many things, there are benefits and draw backs to using a memory 
 * pool.  For the MemoryPool object they are:
 * 
 * __Benefits__
 * - SPEED!!! Much faster allocations than `new` or `malloc`
 * - Data focused, allocations are based on size not object-type
 * - The entire memory pool can be easily shared across process-space
 * - Can automatically grow as needed [optional feature]
 * 
 * __Draw Backs__
 * - Requires extra work to convert offsets to pointers
 * - If the memory pool expands, pointers can be invalidated
 * - Memory fragmentation has more of an impact
 * - No bounds checking in memory writes
 *
 * To put things into perspective, allocating memory is a very expensive 
 * operation.  Using the MemoryPool means this operation only needs to happen 
 * once.  Allocating memory from the MemoryPool just needs to scan the memory 
 * region that can hold the request size.  Even for extremely large memory 
 * pools, this is a very fast operation.  Requiring the size (in bytes) to be 
 * allocated from the MemoryPool contributes to the speed of the allocation.
 *
 * Since the MemoryPool uses a Unix File Descriptor for the memory region, only 
 * that Unix File Descriptor must be shared between processes to access the 
 * entire MemoryPool data.
 *
 * As a result of using a Unix File Descriptor, allocating memory returns an 
 * offset into the MemoryPool data.  When the memory region expands in the 
 * MemoryPool, the location of the "file" may change.  Using offsets into the 
 * data negates the problem of pointing to invalid memory.  Unfortunately, 
 * programming uses pointers instead of offsets.  MemoryPool does provide a way 
 * to be notified when the memory region moves so the pointers an application 
 * is using can be updated.
 *
 * Unless C++'s [Placement 
 * New](https://en.cppreference.com/w/cpp/language/new#Placement_new) is being 
 * used extensively, the developer must be fully aware of where they are in 
 * their area of the memory region so that other data is not over written.  
 * _Placement New will break when the memory region moves._
 *
 * Memory fragmentation already happens in most applications.  The impact of 
 * the fragmentation is not felt due to huge amounts of memory in today's 
 * computers.  However, for small amounts of memory fragmentation becomes a 
 * larger issue.  Over the course of an application's life-time, it may 
 * allocate and free memory many times.  At some point in time, if a large 
 * block of memory is requested, that allocation may fail because a contiguous 
 * region of memory is not available.  This is the problem that plagues memory 
 * pools.
 *
 * If the benefits out-weigh the draw backs for your application, then the 
 * MemoryPool is what you need.
 * 
 * \note This implementation is limited to 32-bit file sizes.
 * \endparblock
 *
 * \par How To Use It?
 * \parblock
 * __Step 0__
 *
 * Your compiler must support at least the C++20 standard.
 *
 * __Step 1__
 *
 * The first step is to select which C++ source code file will contain the 
 * _Zakero MemoryPool_ implementation.  Once the location has been determined, 
 * add the following to that file:
 *
 * ~~~
 * #define ZAKERO_MEMORYPOOL_IMPLEMENTATION
 * #include "path/to/Zakero_MemoryPool.h"
 * ~~~
 *
 * The macro __ZAKERO_MEMORYPOOL_IMPLEMENTATION__ tells the header file to 
 * include the implementation of the MemoryPool.
 *
 * In all other files that will use the MemoryPool, they need to include the 
 * header.
 *
 * ~~~
 * #include "path/to/Zakero_MemoryPool.h"
 * ~~~
 *
 * __Step 2__
 *
 * After creating a [MemoryPool](\ref zakero::MemoryPool::MemoryPool), it must 
 * be [initialized](\ref zakero::MemoryPool::init()) before it can be used.  
 * Once that is done, you can freely [allocate](\ref 
 * zakero::MemoryPool::alloc()) and [free](\ref zakero::MemoryPool::free()) 
 * memory from the MemoryPool.
 *
 * This is an example of creating two std::array's that are backed by the 
 * MemoryPool.
 *
 * ~~~
 * zakero::MemoryPool memory_pool("Array_Data");
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
 * \endparblock
 *
 * \version 0.8.0
 * - Allocate and manage memory pool
 * - Automatically expand the memory pool as needed
 * - Share the region of memory using the file descriptor
 *
 * \copyright [Mozilla Public License 
 * v2](https://www.mozilla.org/en-US/MPL/2.0/) 
 *
 * \author Andrew "Zakero" Moore
 * 	- Original Author
 *
 * \todo Add protections so that memory sizes above off_t are not allowed.
 * \todo Add support for huge file sizes (64-bit / huge table fs)
 *       - Maybe toggled via a macro flag
 * \todo Be able to defrag the memory pool
 * \todo Pass a lambda to the resize() method so that how the memory is moved 
 * can be controlled.  For example, if the memory was holding a texture, the 
 * texture should be "clipped" or "zero'ed" space added around the existing 
 * data.
 * \todo Be able to initialize a MemoryPool with a Unix File Descriptor and the 
 * file size.
 */

#ifndef zakero_MemoryPool_h
#define zakero_MemoryPool_h

#include <cstring>
#include <functional>
#include <map>
#include <mutex>
#include <sys/mman.h>
#include <system_error>
#include <unistd.h>

// {{{ Error Codes

/**
 * \brief Error Codes used internally.
 */
#define ZAKERO_MEMORYPOOL__ERROR_CODES \
	X(Error_None                 ,  0 , "No Error"                                                     ) \
	X(Error_Already_Initialized  ,  1 , "The Memory Pool has already been initialized."                ) \
	X(Error_Size_Too_Small       ,  2 , "Invalid Size: Must be greater than 0."                        ) \
	X(Error_Size_Too_Large       ,  3 , "Invalid Size: Must be less than zakero::MemoryPool::Size_Max" ) \
	X(Error_Failed_To_Create_File,  4 , "Unable to create file."                                       ) \
	X(Error_Failed_To_Resize_File,  5 , "Unable to resize file."                                       ) \
	X(Error_Failed_To_Map_File   ,  6 , "Unable to memory map the file."                               ) \
	X(Error_Out_Of_Memory        ,  7 , "Not enough contiguous memory."                                ) \
	X(Error_Invalid_Offset       ,  8 , "The offset is not valid."                                     ) \

// }}}
// {{{ Declaration

namespace zakero
{
	class MemoryPool
	{
		public:
#define X(name_, val_, mesg_) \
			static constexpr int name_ = val_;
			ZAKERO_MEMORYPOOL__ERROR_CODES
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

			using AddressMap = std::map<uint8_t*, uint8_t*>;

			using LambdaSize       = std::function<void(size_t)>;
			using LambdaAddressMap = std::function<void(const MemoryPool::AddressMap&)>;

			MemoryPool(const std::string&) noexcept;
			~MemoryPool() noexcept;

			              std::error_condition init(size_t, const bool = false, const MemoryPool::Alignment = MemoryPool::Alignment::Bits_64) noexcept;
			[[nodiscard]] int                  fd() const noexcept;
			[[nodiscard]] size_t               size() const noexcept;
			              void                 sizeOnChange(MemoryPool::LambdaSize) noexcept;

			[[nodiscard]] off_t                alloc(size_t) noexcept;
			[[nodiscard]] off_t                alloc(size_t, std::error_condition&) noexcept;
			[[nodiscard]] off_t                alloc(size_t, uint8_t) noexcept;
			[[nodiscard]] off_t                alloc(size_t, uint8_t, std::error_condition&) noexcept;
			[[nodiscard]] off_t                alloc(size_t, uint32_t) noexcept;
			[[nodiscard]] off_t                alloc(size_t, uint32_t, std::error_condition&) noexcept;
			              void                 free(off_t&) noexcept;
			[[nodiscard]] off_t                resize(off_t, size_t) noexcept;
			[[nodiscard]] off_t                resize(off_t, size_t, std::error_condition&) noexcept;

			[[nodiscard]] uint8_t*             addressOf(off_t) const noexcept;
			              void                 onRemap(MemoryPool::LambdaAddressMap) noexcept;
			[[nodiscard]] static uint8_t*      remap(const MemoryPool::AddressMap&, uint8_t*) noexcept;

			[[nodiscard]] std::string          dump(size_t, size_t) const noexcept;

		private:
			[[nodiscard]] off_t  actualSize(off_t) noexcept;
			[[nodiscard]] bool   expandBy(size_t) noexcept;
                                      
			              void   segmentDestroy(size_t) noexcept;
			[[nodiscard]] bool   segmentExpand(size_t, size_t) noexcept;
			[[nodiscard]] size_t segmentFind(uint8_t*) noexcept;
			[[nodiscard]] size_t segmentFind(off_t) noexcept;
			[[nodiscard]] size_t segmentFindAvail(size_t) noexcept;
			[[nodiscard]] size_t segmentFindInUse(off_t) const noexcept;
			              void   segmentMerge(size_t) noexcept;
			[[nodiscard]] size_t segmentMove(size_t, size_t, size_t) noexcept;
			              void   segmentSplit(size_t, size_t) noexcept;

			// ------------------------------------------------- //

			struct Segment
			{
				off_t offset;
				off_t size;
				bool  in_use;
			};

			using VectorSegment = std::vector<Segment>;

			uint8_t*                     memory;
			std::string                  name;
			mutable std::mutex           mutex;
			MemoryPool::VectorSegment    segment;
			MemoryPool::LambdaSize       size_on_change;
			MemoryPool::LambdaAddressMap on_remap;
			size_t                       pool_size;
			int                          file_descriptor;
			MemoryPool::Alignment        alignment;
			bool                         is_dynamic;
	};
};

// }}}
// {{{ Implementation

#if defined (ZAKERO_MEMORYPOOL_IMPLEMENTATION)

// {{{ Defines

#define ZAKERO_MEMORYPOOL__ERROR(err_) std::error_condition(err_, MemoryPoolErrorCategory);

// }}}

namespace
{
	class MemoryPoolErrorCategory_
		: public std::error_category
	{
		public:
			constexpr MemoryPoolErrorCategory_() noexcept
			{
			}

			const char* name() const noexcept override
			{
				return "zakero.MemoryPool";
			}

			std::string message(int condition) const override
			{
				switch(condition)
				{
#define X(name_, val_, mesg_) \
					case val_: return mesg_;
					ZAKERO_MEMORYPOOL__ERROR_CODES
#undef X
				}

				return "Unknown error condition";
			}
	} MemoryPoolErrorCategory;

	zakero::MemoryPool::LambdaSize       LambdaSize_DoNothing       = [](size_t){};
	zakero::MemoryPool::LambdaAddressMap LambdaAddressMap_DoNothing = [](const zakero::MemoryPool::AddressMap&){};
}

namespace zakero
{
	/**
	 * \class zakero::MemoryPool
	 *
	 * \brief A pool of memory.
	 *
	 * Refer to Zakero_MemoryPool.h to learn how to include this library.
	 *
	 * This object will create a region of memory and provide an interface 
	 * to allocate from that memory.
	 */

	/**
	 * \enum MemoryPool::Alignment
	 *
	 * \brief The Byte-Alignment of the MemoryPool.
	 *
	 * When allocating memory from the MemoryPool, this enum determines 
	 * which byte-boundary will be used.
	 */

	/**
	 * \typedef MemoryPool::AddressMap
	 *
	 * \brief A mapping of old addresses to new addresses.
	 */

	/**
	 * \typedef MemoryPool::LambdaSize
	 *
	 * \brief A lambda that receives a `size_t` argument.
	 */

	/**
	 * \typedef MemoryPool::LambdaAddressMap
	 *
	 * \brief A lambda that receives a MemoryPool::AddressMap.
	 */

	/**
	 * \struct MemoryPool::Segment
	 *
	 * \brief Data that defines a segment.
	 */

	/**
	 * \typedef MemoryPool::VectorSegment
	 *
	 * \brief A convenience data type.
	 */

	/**
	 * \brief Constructor.
	 *
	 * Create a new instance of the MemoryPool.  The provide file \p name 
	 * is not the name of a file on the file system.  The file \p name will 
	 * only exist in RAM with an optional backing store in swap in 
	 * available.
	 *
	 * \example
	 * zakero::MemoryPool memory_pool("The name of the MemoryPool");
	 * \endexample
	 */
	MemoryPool::MemoryPool(const std::string& name ///< The file name
		) noexcept
		: memory(nullptr)
		, name(name)
		, mutex()
		, segment()
		, size_on_change(LambdaSize_DoNothing)
		, on_remap(LambdaAddressMap_DoNothing)
		, pool_size(0)
		, file_descriptor(-1)
		, alignment(zakero::MemoryPool::Alignment::Bits_64)
		, is_dynamic(false)
	{
	}


	/**
	 * \brief Destructor
	 *
	 * Release all allocated resources.
	 */
	MemoryPool::~MemoryPool()
	{
		on_remap = LambdaAddressMap_DoNothing;
		segment.clear();
		name.clear();

		if(memory != nullptr)
		{
			#if defined (ZAKERO_MEMORYPOOL_ZERO_ON_FREE)
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
	 * \brief Initialize the MemoryPool.
	 *
	 * The MemoryPool must be initialized before it can be used.  At a 
	 * minimum, the size of the MemoryPool must be specified in bytes.
	 *
	 * The maximum allowable size is MemoryPool::Size_Max which represents 
	 * the largest offset value supported by the MemoryPool.  Your hardware 
	 * configuration and/or operating system may lower this limit.
	 *
	 * Setting the \p expandable flag to `true` will allow the MemoryPool 
	 * to grow to a larger size.  If an allocation request is made that is 
	 * larger that the largest available contiguous space available, then 
	 * the Memory Pool will expand just enough to accommodate the request.
	 *
	 * The Byte Boundary of all the allocations for this MemoryPool object 
	 * is specified by the \p alignment.
	 *
	 * \note The size of the MemoryPool will never shrink.
	 *
	 * \example
	 * zakero::MemoryPool rgba_textures("Active Texture Cache");
	 *
	 * #define KILOBYETS(size_) ((size_) * 1024)
	 * #define MEGABYTES(size_) (KILOBYTES(size_) * 1024)
	 * rgba_textures.init(MEGABYTES(32)
	 * 	, false // Restricted environment, no expanding
	 * 	, zakero::MemoryPool::Alignment::Bits_32 // RGBA == 32 bits
	 * 	);
	 * \endexample
	 *
	 * \return An error condition.  If there was no error, then the value 
	 * of the error condition will be `0`.
	 */
	std::error_condition MemoryPool::init(size_t size       ///< The initial size in bytes
		, const bool                         expandable ///< Allow the MemoryPool to expand
		, const MemoryPool::Alignment        alignment  ///< The Byte Alignment
		) noexcept
	{
		if(file_descriptor != -1)
		{
			return ZAKERO_MEMORYPOOL__ERROR(Error_Already_Initialized);
		}

		if(size <= 0)
		{
			return ZAKERO_MEMORYPOOL__ERROR(Error_Size_Too_Small);
		}

		if(size > MemoryPool::Size_Max)
		{
			return ZAKERO_MEMORYPOOL__ERROR(Error_Size_Too_Large);
		}

		this->is_dynamic = expandable;
		this->alignment  = alignment;

		size = actualSize(size);

#if defined (__linux__)
		file_descriptor = memfd_create(name.c_str(), 0);
#else
#error Need morecode...
#endif

		if(file_descriptor == -1)
		{
			return ZAKERO_MEMORYPOOL__ERROR(Error_Failed_To_Create_File);
		}

		if(ftruncate(file_descriptor, size) == -1)
		{
			return ZAKERO_MEMORYPOOL__ERROR(Error_Failed_To_Resize_File);
		}

		memory = (uint8_t*)mmap(nullptr
			, size
			, PROT_READ | PROT_WRITE
			, MAP_SHARED | MAP_NORESERVE
			, file_descriptor
			, 0
			);

		if(memory == MAP_FAILED)
		{
			return ZAKERO_MEMORYPOOL__ERROR(Error_Failed_To_Map_File);
		}

		pool_size = size;

		segment.push_back(
		{	.offset = 0
		,	.size   = (off_t)size
		,	.in_use = false
		});

		return ZAKERO_MEMORYPOOL__ERROR(Error_None);
	}


	/**
	 * \brief The backing file descriptor.
	 *
	 * If something needs to be able to map the same region of memory as 
	 * this MemoryPool, then this method will provide the file descriptor 
	 * to do it.
	 *
	 * The entire MemoryPool will be accessible from the file descriptor.
	 *
	 * \example
	 * // Get read access to the shared memory
	 * uint8_t* mem_reader = (uint8_t*)mmap(nullptr
	 * 	, memory_pool.size()
	 * 	, PROT_READ
	 * 	, MAP_SHARED | MAP_NORESERVE
	 * 	, memory_pool.fd()
	 * 	);
	 * \endexample
	 *
	 * \see MemoryPool::size()
	 *
	 * \return The file descriptor.
	 */
	int MemoryPool::fd() const noexcept
	{
		return file_descriptor;
	}


	/**
	 * \brief The size of the memory pool.
	 *
	 * The current size of the MemoryPool, in bytes, will be returned.
	 *
	 * \return The size of the memory pool.
	 */
	size_t MemoryPool::size() const noexcept
	{
		return pool_size;
	}


	/**
	 * \brief Set the Size Event callback.
	 *
	 * If the MemoryPool was configured to be able to dynamically expand as 
	 * needed (see init()), then the provided \p lambda will be called when 
	 * the memory pool changes size.  Using the sizeOnChange() method 
	 * before calling init() will not cause the lambda to be executed when 
	 * init() is called.
	 *
	 * The \p lambda will receive the new size, in bytes, of the memory 
	 * pool.
	 *
	 * \example
	 * zakero::MemoryPool memory_pool("Size Matters");
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
	 * \endexample
	 *
	 * \note The MemoryPool will be in a "locked state" so any call from 
	 * the lambda to a non-const MemoryPool method will block indefinitely.
	 */
	void MemoryPool::sizeOnChange(MemoryPool::LambdaSize lambda ///< The callback
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
	 * \example
	 * zakero::MemoryPool memory_pool("foo");
	 * memory_pool.init(1024);
	 *
	 * off_t data_offset = memory_pool.alloc(128);
	 * \endexample
	 *
	 * \note The contents of the memory is undefined.
	 *
	 * \return The offset of the block of memory.
	 */
	off_t MemoryPool::alloc(size_t size ///< The size in bytes
		) noexcept
	{
		std::error_condition error;

		return alloc(size, error);
	}


	/**
	 * \brief Allocate memory from the pool.
	 *
	 * The requested \p size (in bytes) will be allocated from the memory 
	 * pool.  If the memory could not be allocated, then `-1` will be 
	 * returned and the reason will be stored in \p error.
	 *
	 * \example
	 * zakero::MemoryPool memory_pool("foo");
	 * memory_pool.init(1024);
	 *
	 * std::error_condition error;
	 * off_t data_offset = memory_pool.alloc(512, error);
	 *
	 * if(error.value() != 0)
	 * {
	 * 	std::cerr << "Error: " << error.message() << "\n";
	 * }
	 * \endexample
	 *
	 * \note The contents of the memory is undefined.
	 *
	 * \return The offset of the block of memory.
	 */
	off_t MemoryPool::alloc(size_t  size  ///< The size in bytes
		, std::error_condition& error ///< The error
		) noexcept
	{
		if(size <= 0)
		{
			error = ZAKERO_MEMORYPOOL__ERROR(Error_Size_Too_Small);
			return -1;
		}

		if(size > MemoryPool::Size_Max)
		{
			error = ZAKERO_MEMORYPOOL__ERROR(Error_Size_Too_Large);
			return -1;
		}

		std::lock_guard<std::mutex> lock(mutex);

		size = actualSize(size);

		size_t index = segmentFindAvail(size);

		if(index >= segment.size())
		{
			if(expandBy(size) == false)
			{
				error = ZAKERO_MEMORYPOOL__ERROR(Error_Out_Of_Memory);
				return -1;
			}

			index = segmentFindAvail(size);
		}

		error = ZAKERO_MEMORYPOOL__ERROR(Error_None);

		segment[index].in_use = true;

		if((size_t)segment[index].size == size)
		{
			return segment[index].offset;
		}

		segmentSplit(index, size);

		return segment[index].offset;
	}


	/**
	 * \brief Allocate memory from the pool.
	 *
	 * The requested \p size (in bytes) will be allocated from the memory 
	 * pool.  If the memory could not be allocated, then `-1` will be 
	 * returned.
	 *
	 * \example
	 * zakero::MemoryPool memory_pool("foo");
	 * memory_pool.init(1024);
	 *
	 * off_t data_offset = memory_pool.alloc(512, uint8_t(0xa5));
	 * \endexample
	 *
	 * \return The offset of the block of memory.
	 */
	off_t MemoryPool::alloc(size_t size  ///< The size in bytes
		, uint8_t              value ///< The fill value
		) noexcept
	{
		std::error_condition error;

		return alloc(size, value, error);
	}


	/**
	 * \brief Allocate memory from the pool.
	 *
	 * The requested \p size (in bytes) will be allocated from the memory 
	 * pool.  If the memory could not be allocated, then `-1` will be 
	 * returned and the reason will be stored in \p error.
	 *
	 * \example
	 * zakero::MemoryPool memory_pool("foo");
	 * memory_pool.init(1024);
	 *
	 * std::error_condition error;
	 * off_t data_offset = memory_pool.alloc(512, uint8_t(0xa5), error);
	 *
	 * if(error.value() != 0)
	 * {
	 * 	std::cerr << "Error: " << error.message() << "\n";
	 * }
	 * \endexample
	 *
	 * \return The offset of the block of memory.
	 */
	off_t MemoryPool::alloc(size_t  size  ///< The size in bytes
		, uint8_t               value ///< The fill value
		, std::error_condition& error ///< The error
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
	 * \example
	 * zakero::MemoryPool memory_pool("foo");
	 * memory_pool.init(1024);
	 *
	 * off_t data_offset = memory_pool.alloc(512, uint32_t(0xa5));
	 * \endexample
	 *
	 * \return The offset of the block of memory.
	 */
	off_t MemoryPool::alloc(size_t size  ///< The size in bytes
		, uint32_t             value ///< The fill value
		) noexcept
	{
		std::error_condition error;

		return alloc(size, value, error);
	}


	/**
	 * \brief Allocate memory from the pool.
	 *
	 * The requested \p size (in bytes) will be allocated from the memory 
	 * pool.  If the memory could not be allocated, then `-1` will be 
	 * returned and the reason will be stored in \p error.
	 *
	 * \example
	 * zakero::MemoryPool memory_pool("foo");
	 * memory_pool.init(1024);
	 *
	 * std::error_condition error;
	 * off_t data_offset = memory_pool.alloc(512, uint32_t(0xa5), error);
	 *
	 * if(error.value() != 0)
	 * {
	 * 	std::cerr << "Error: " << error.message() << "\n";
	 * }
	 * \endexample
	 *
	 * \return The offset of the block of memory.
	 */
	off_t MemoryPool::alloc(size_t  size  ///< The size in bytes
		, uint32_t              value ///< The fill value
		, std::error_condition& error ///< The error
		) noexcept
	{
		off_t offset = alloc(size);

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
	 * \example
	 * zakero::MemoryPool memory_pool("A Memory Pool");
	 *
	 * memory_pool.init(128);
	 *
	 * off_t offset = memory_pool.alloc(64);
	 *
	 * // Do stuff with offset
	 *
	 * memory_pool.free(offset);
	 * \endexample
	 */
	void MemoryPool::free(off_t& offset ///< The memory to free
		) noexcept
	{
		std::lock_guard<std::mutex> lock(mutex);

		size_t index = segmentFindInUse(offset);

		if(index >= segment.size())
		{
			return;
		}

		segmentDestroy(index);

		offset = -1;
	}


	/**
	 * \brief Change the size of allocated memory.
	 *
	 * This method is similar to realloc(), in that it will resize the 
	 * allocated memory at the given \p offset.  If the resize was 
	 * successful, the new offset will be returned.
	 *
	 * The contents of the memory will be preserved.
	 *
	 * The return value will be `-1` if the allocated memory could not be 
	 * resized.
	 *
	 * \example
	 * zakero::MemoryPool memory_pool("Resizing in the Pool");
	 *
	 * memory_pool.init(128);
	 *
	 * off_t offset = memory_pool.alloc(64);
	 *
	 * // Do stuff with offset
	 * // Oops, need more space
	 *
	 * offset = memory_pool.resize(offset, 96);
	 * \endexample
	 *
	 * \return The offset of the resized memory location.
	 */
	off_t MemoryPool::resize(off_t offset ///< The memory to resize
		, size_t               size   ///< The size in bytes
		) noexcept
	{
		std::error_condition error;

		return resize(offset, size, error);
	}


	/**
	 * \brief Change the size of allocated memory.
	 *
	 * This method is similar to realloc(), in that it will resize the 
	 * allocated memory at the given \p offset.  If the resize was 
	 * successful, the new offset will be returned.
	 *
	 * The contents of the memory will be preserved.
	 *
	 * The return value will be `-1` if the allocated memory could not be 
	 * resized and the reason will be stored in \p error.
	 *
	 * \example
	 * zakero::MemoryPool memory_pool("Resizing in the Pool");
	 *
	 * memory_pool.init(128);
	 *
	 * off_t offset = memory_pool.alloc(64);
	 *
	 * // Do stuff with offset
	 * // Oops, need more space
	 *
	 * std::error_codition error;
	 * auto new_offset = memory_pool.resize(offset, 96, error);
	 * if(error(bool) == true)
	 * {
	 * 	std::cerr << "Error: " << error.message() << "\n";
	 * }
	 * else
	 * {
	 * 	offset = new_offset;
	 * }
	 * \endexample
	 *
	 * \return The offset of the resized memory location.
	 */
	off_t MemoryPool::resize(off_t  offset ///< The memory to resize
		, size_t                size   ///< The size in bytes
		, std::error_condition& error  ///< The error
		) noexcept
	{
		if(size <= 0)
		{
			error = ZAKERO_MEMORYPOOL__ERROR(Error_Size_Too_Small);
			return -1;
		}

		if(size > MemoryPool::Size_Max)
		{
			error = ZAKERO_MEMORYPOOL__ERROR(Error_Size_Too_Large);
			return -1;
		}

		std::lock_guard<std::mutex> lock(mutex);

		const size_t index_src = segmentFindInUse(offset);

		if(index_src >= segment.size())
		{
			error = ZAKERO_MEMORYPOOL__ERROR(Error_Invalid_Offset);
			return -1;
		}

		size = actualSize(size);

		// Same size, nothing to do
		if(size == (size_t)segment[index_src].size)
		{
			error = ZAKERO_MEMORYPOOL__ERROR(Error_None);
			return offset;
		}

		// New size is smaller, shrink segment
		if(size < (size_t)segment[index_src].size)
		{
			segmentSplit(index_src, size);

			error = ZAKERO_MEMORYPOOL__ERROR(Error_None);
			return offset;
		}

		// Larger, try to expand into the next segment
		if(segmentExpand(index_src, size) == true)
		{
			error = ZAKERO_MEMORYPOOL__ERROR(Error_None);
			return offset;
		}

		// Larger, find a new location
		size_t index_dst = segmentFindAvail(size);

		if(index_dst >= segment.size())
		{
			if(expandBy(size) == false)
			{
				error = ZAKERO_MEMORYPOOL__ERROR(Error_Out_Of_Memory);
				return -1;
			}

			index_dst = segmentFindAvail(size);
		}

		index_dst = segmentMove(index_src, index_dst, size);

		offset = segment[index_dst].offset;

		error = ZAKERO_MEMORYPOOL__ERROR(Error_None);
		return offset;
	}


	/**
	 * \brief Convert an offset into a pointer.
	 *
	 * The provided \p offset will be converted into an address that can be 
	 * de-referenced as a normal C-Style pointer.  If the \p offset is not 
	 * valid, then `nullptr` will be returned.
	 *
	 * \example
	 * off_t offset = memory_pool.alloc(256);
	 * uint8_t* ptr = memory_pool.addressOf(offset);
	 * \endexample
	 *
	 * \note If the MemoryPool expands and is relocated, the returned 
	 * pointers will no longer be valid.
	 *
	 * \see onRemap()
	 *
	 * \return An address
	 */
	uint8_t* MemoryPool::addressOf(off_t offset ///< The offset
		) const noexcept
	{
		if(segmentFindInUse(offset) >= segment.size())
		{
			return nullptr;
		}

		return memory + offset;
	}


	/**
	 * \brief Set the Remap Event callback.
	 *
	 * There are times when the MemoryPool will move allocated data.  When 
	 * this happens, the provided \p lambda will be called so that the 
	 * caller will have an opportunity to update their pointers.
	 *
	 * The lambda will receive a map of addresses where the key is the old 
	 * address and the value is the new address.
	 *
	 * If the MemoryPool was configured to be _not expandable_ (see 
	 * init()), then the MemoryPool will never have a need to move it's 
	 * region of memory.  Therefore, the \p lambda will never be called and 
	 * the pointers will never become invalid (unless the memory is freed).
	 *
	 * \example
	 * uint8_t* secret = nullptr;
	 *
	 * memory_pool.onRemap([&](const zakero::MemoryPool::AddressMap& map)
	 * {
	 * 	secret = zakero::MemoryPool::remap(map, secret);
	 * });
	 *
	 * off_t secret_offset = memory_pool.alloc(512);
	 * secret = memory_pool.addressOf(secret_offset);
	 * \endexample
	 *
	 * \note The MemoryPool will be in a "locked state" so any call from 
	 * the lambda to a non-const MemoryPool method will block indefinitely.
	 *
	 * \see remap()
	 */
	void MemoryPool::onRemap(MemoryPool::LambdaAddressMap lambda ///< The callback
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
	 * the quality-of-life of the MemoryPool API.
	 *
	 * \see onRemap()
	 *
	 * \return A memory address.
	 */
	uint8_t* MemoryPool::remap(const MemoryPool::AddressMap& addr_map ///< A map of old/new addresses
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
	 * The internal state of the MemoryPool will be converted into a JSON 
	 * formatted string.  The JSON string will contain the following:
	 * - The name of the anonymous file
	 * - The size of the memory pool
	 * - A list of segments and information about each segment
	 * - A layout of memory usage
	 *
	 * The content of the layout is controlled by the \p 
	 * bytes_per_character and \p characters_per_line parameters.
	 *
	 * \example
	 * zakero::MemoryPool memory_pool("Dump It");
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
	 * \endexample
	 *
	 * \return The JSON formatted string.
	 */
	std::string MemoryPool::dump(size_t bytes_per_character ///< Used to determine how many allocated bytes are represented by each character in the layout.
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
	 * \brief Calculate the byte-aligned size.
	 *
	 * The byte-aligned size will be determined based on the provided \p 
	 * size.
	 *
	 * \return The byte-aligned size.
	 */
	inline off_t MemoryPool::actualSize(off_t size ///< The initial size
		) noexcept
	{
		const off_t mod  = static_cast<off_t>(alignment);
		const off_t step = static_cast<off_t>(alignment) + 1;

		return ((size + mod) / step) * step;
	}

	/**
	 * \brief Increase the size of the memory pool.
	 *
	 * \retval true  The memory pool size was increased.
	 * \retval false The memory pool size was __not__ increased.
	 */
	bool MemoryPool::expandBy(size_t size_increase ///< The amount of the increase
		) noexcept
	{
		if(is_dynamic == false)
		{
			return false;
		}

		if(segment.back().in_use == false)
		{
			size_increase -= segment.back().size;
		}

		size_increase = actualSize(size_increase);

		if(pool_size + size_increase > MemoryPool::Size_Max)
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

		const size_t index = segment.size();

		segment.push_back(
		{	.offset = (off_t)old_size
		,	.size   = (off_t)size_increase
		,	.in_use = false
		});

		segmentMerge(index);

		size_on_change(pool_size);

		if(new_memory != old_memory)
		{
			memory = (uint8_t*)new_memory;

			MemoryPool::AddressMap addr_map;

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
	 * \brief Destroy a segment.
	 */
	void MemoryPool::segmentDestroy(size_t index ///< The segment to destroy
		) noexcept
	{
		segment[index].in_use = false;

		#if defined (ZAKERO_MEMORYPOOL_ZERO_ON_FREE)
		memset(memory + segment[index].offset
			, '\0'
			, segment[index].size
			);
		#endif

		segmentMerge(index);
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
	bool MemoryPool::segmentExpand(size_t index ///< The segment index
		, size_t                      size  ///< The new size
		) noexcept
	{
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

		if(size > size_t(segment[index].size + segment_next.size))
		{
			return false;
		}

		const size_t size_delta = size - segment[index].size;

		if(size_delta == 0)
		{
			const auto iter = std::begin(segment);

			segment.erase(iter + index_next);
		}
		else
		{
			segment_next.offset += size_delta;
			segment_next.size   -= size_delta;
		}

		segment[index].size = size;

		return true;
	}


	/**
	 * \brief Find a segment.
	 *
	 * Find the segment that starts with the specified \p address.
	 *
	 * \return The index of the segment.
	 */
	size_t MemoryPool::segmentFind(uint8_t* addr ///< The address to find
		) noexcept
	{
		const size_t offset = addr - memory;

		return segmentFind(offset);
	}


	/**
	 * \brief Find a segment.
	 *
	 * Find the segment that starts with the specified \p offset.
	 *
	 * \return The index of the segment.
	 */
	size_t MemoryPool::segmentFind(off_t offset ///< The offset to find
		) noexcept
	{
		for(size_t index = 0; index < segment.size(); index++)
		{
			if(segment[index].offset == offset)
			{
				return index;
			}
		}

		return ~0;
	}


	/**
	 * \brief Find a segment.
	 *
	 * Look for a segment that has a size greater than (or equal to) \p 
	 * size and is not in use.
	 *
	 * \return The index of the segment.
	 */
	size_t MemoryPool::segmentFindAvail(size_t size ///< The minimum size
		) noexcept
	{
		for(size_t index = 0; index < segment.size(); index++)
		{
			if(segment[index].in_use == false
				&& (size_t)segment[index].size >= size
				)
			{
				return index;
			}
		}

		return ~0;
	}


	/**
	 * \brief Find a segment.
	 *
	 * Find the segment that starts with the specified \p offset.
	 *
	 * \return The index of the segment.
	 */
	size_t MemoryPool::segmentFindInUse(off_t offset ///< The offset to find
		) const noexcept
	{
		for(size_t index = 0; index < segment.size(); index++)
		{
			if(segment[index].offset == offset
				&& segment[index].in_use == true
				)
			{
				return index;
			}
		}

		return ~0;
	}


	/**
	 * \brief Merge unused segments.
	 *
	 * The unused segments surrounding the segment at the provided \p index 
	 * will be merged into a single segment.
	 *
	 * \note No error checking is done,  It segment at \p index is expected 
	 * to be not `in_use`.
	 */
	void MemoryPool::segmentMerge(size_t index ///< The segment to merge
		) noexcept
	{
		if(size_t index_next = index + 1;
			index_next < segment.size()
			)
		{
			Segment& segment_next = segment[index_next];

			if(segment_next.in_use == false)
			{
				segment[index].size += segment_next.size;

				auto segment_begin = std::begin(segment);
				segment.erase(segment_begin + index_next);
			}
		}

		if(index > 0)
		{
			const size_t index_prev = index - 1;

			Segment& segment_prev = segment[index_prev];

			if(segment_prev.in_use == false)
			{
				segment_prev.size += segment[index].size;

				auto segment_begin = std::begin(segment);
				segment.erase(segment_begin + index);
			}
		}
	}


	/**
	 * \brief Move the contents of a segment.
	 *
	 * The contents of the segment at \p index_src will be moved to the \p 
	 * index_dst segment.  The \p size parameter is used to determine the 
	 * resulting size of the \p index_dst segment.
	 *
	 * \note No error checking is done, all provided values are expected to 
	 * be correct and valid.
	 *
	 * It is expected that \p size will be in the range of:
	 *     segment[index_src].size <= size <= segment[index_dst].size
	 *
	 * The segment at \p index_src will be destroyed.
	 *
	 * \return The index of the destination segment.
	 */
	size_t MemoryPool::segmentMove(size_t index_src ///< The source segment
		, size_t                      index_dst ///< The destination segment
		, size_t                      size      ///< The destination size
		) noexcept
	{
		Segment& segment_src = segment[index_src];
		uint8_t* addr_src    = memory + segment_src.offset;
		size_t   size_src    = segment_src.size;

		Segment& segment_dst = segment[index_dst];
		uint8_t* addr_dst    = memory + segment_dst.offset;
		size_t   offset_dst  = segment_dst.offset;

		segment_dst.in_use = true;

		memcpy(addr_dst, addr_src, size_src);

		if(index_src > index_dst)
		{
			segmentDestroy(index_src);
			segmentSplit(index_dst, size);
		}
		else
		{
			segmentSplit(index_dst, size);
			segmentDestroy(index_src);

			index_dst = segmentFind(offset_dst);
		}

		return index_dst;
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
	void MemoryPool::segmentSplit(size_t index ///< The segment to split
		, size_t                     size  ///< The "new" segment size
		) noexcept
	{
		const size_t index_next = index + 1;

		if(index_next >= segment.size())
		{
			segment.push_back(
			{	.offset = off_t(segment[index].offset + size)
			,	.size   = off_t(segment[index].size - size)
			,	.in_use = false
			});
		}
		else
		{
			segment.insert(std::begin(segment) + index_next,
			{	.offset = off_t(segment[index].offset + size)
			,	.size   = off_t(segment[index].size - size)
			,	.in_use = false
			});
		}

		segment[index].size = size;

		if(segment[index].in_use)
		{
			Segment& segment_next = segment[index_next];

			uint8_t* addr = memory + segment_next.offset;

			memset(addr, '\0', segment_next.size);
		}

		segmentMerge(index_next);
	}
}

#endif

// }}}

#endif // zakero_MemoryPool_h
