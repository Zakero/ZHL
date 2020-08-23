/* *******************************************************************
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef zakero_Base_h
#define zakero_Base_h

/**
 * \file
 *
 * \brief Zakero Base
 *
 * Nothing complicated here, just a collection of helper functions, macros, and 
 * templates that may be useful in your projects.
 *
 * Include this header in your source code to use these features.
 *
 * \version 0.9.0
 * - The initial collection
 *
 * \copyright [Mozilla Public License 
 * v2](https://www.mozilla.org/en-US/MPL/2.0/) 
 *
 * \author Andrew "Zakero" Moore
 * - Original Author
 */

// POSIX
#include <vector>


/**
 * \internal
 *
 * \brief Concatenate the two things.
 *
 * An extra layer of C/C++ Preprocessor indirection to enforce proper order of 
 * macro expansion and concatenation.
 *
 * \param thing_1_ Symbol left side
 * \param thing_2_ Symbol right side
 */
#define ZAKERO_CONCAT_(thing_1_, thing_2_) thing_1_ ## thing_2_

/**
 * \brief Concatenate the two things.
 *
 * Use the C/C++ Preprocessor to create a new symbol name.  For example the 
 * symbol _abcxyz_ could be created using _ZAKERO_CONCAT(abc, xyz)_.
 *
 * \parcode
 * int foobar = 1;
 * ZAKERO_CONCAT(foo, bar)++; // foobar == 2
 *
 * int ZAKERO_CONCAT(magic_, 42) = 123;
 * // int magic_42 = 123;
 * \endparcode
 *
 * \param thing_1_ Symbol left side
 * \param thing_2_ Symbol right side
 */
#define ZAKERO_CONCAT(thing_1_, thing_2_) ZAKERO_CONCAT_(thing_1_, thing_2_)

/**
 * \brief Check if a macro has a value.
 *
 * Use this macro function to determine if a macro has a value and is not just 
 * defined.
 *
 * \parcode
 * #define BEER
 *
 * #if ZAKERO_MACRO_HAS_VALUE(BEER)
 * #warning I can has beer!
 * #else
 * #error No beer! // <-- This happens
 * #endif
 * \endparcode
 *
 * \param define_ The defined macro to check.
 */
#define ZAKERO_MACRO_HAS_VALUE(define_) \
	~(~define_ + 0) == 0 && ~(~define_ + 1) == 1

namespace zakero
{
	/**
	 * \brief Conversion Type.
	 *
	 * Sizes in powers of 2.
	 */
	enum class Storage : uint64_t
	{	Byte     = 0x0000'0000'0000'0001 ///< 1 byte
	,	Kilobyte = 0x0000'0000'0000'0400 ///< 1024 bytes
	,	Megabyte = 0x0000'0000'0010'0000 ///< 1024 kilobytes
	,	Gigabyte = 0x0000'0000'4000'0000 ///< 1024 megabytes
	,	Terabyte = 0x0000'0100'0000'0000 ///< 1024 gigabytes
	,	Petabyte = 0x0004'0000'0000'0000 ///< 1024 terabytes
	,	Exabyte  = 0x1000'0000'0000'0000 ///< 1024 petabytes
	};

	/**
	 * \brief Convert storage sizes.
	 *
	 * Conversion from one storage unit to another is handled by this 
	 * method.  These storage units are in powers of 2.
	 *
	 * Converting to a larger size is rounded down and may result in `0` if 
	 * the from \p size is not large enough.
	 *
	 * \par "Example"
	 * \parblock
	 * \code
	 * uint64_t bytes = zakero::convert(uint64_t(16)
	 * 	, zakero::Storage::Gigabyte
	 * 	, zakero::Storage::Byte
	 * 	);
	 *
	 * uint64_t megs = zakero::convert(uint64_t(16)
	 * 	, zakero::Storage::Kilobyte
	 * 	, zakero::Storage::Megabyte
	 * 	);
	 * // megs == 0
	 * \endcode
	 * \endparblock
	 *
	 * \return The converted value.
	 */
	inline uint64_t convert(const uint64_t size ///< The size to convert
		, const zakero::Storage        from ///< The source unit
		, const zakero::Storage        to   ///< The destination unit
		) noexcept
	{
		return size * static_cast<uint64_t>(from) / static_cast<uint64_t>(to);
	}


	/**
	 * \brief Convert storage sizes.
	 *
	 * Conversion from one storage unit to another is handled by this 
	 * method.  These storage units are in powers of 2.
	 *
	 * The difference between this method and zakero::convert(const 
	 * uint64_t, const zakero::Storage, const zakero::Storage) is that 
	 * conversions to a larger unit will be a fraction.
	 *
	 * \par "Example"
	 * \parblock
	 * \code
	 * double bytes = zakero::convert(double(16)
	 * 	, zakero::Storage::Gigabyte
	 * 	, zakero::Storage::Byte
	 * 	);
	 * double megs = zakero::convert(double(16)
	 * 	, zakero::Storage::Kilobyte
	 * 	, zakero::Storage::Megabyte
	 * 	);
	 * \endcode
	 * \endparblock
	 *
	 * \return The converted value.
	 */
	inline double convert(const double size ///< The size to convert
		, const zakero::Storage    from ///< The source unit
		, const zakero::Storage    to   ///< The destination unit
		) noexcept
	{
		return size * static_cast<uint64_t>(from) / static_cast<uint64_t>(to);
	}

	/**
	 * \brief Check the contents of a std::vector.
	 *
	 * A convience method to make searching a vector easier, like 
	 * std::map::contains().
	 *
	 * \par "Example"
	 * \parblock
	 * \code
	 * std::vector<int> v = { 0, 1, 2, 3 };
	 *
	 * if(vectorContains(v, 1))
	 * {
	 * 	// Found it
	 * }
	 * \endcode
	 * \endparblock
	 *
	 * \retval true  The \p value was found.
	 * \retval false The \p value was __not__ found.
	 */
	template <class Type ///< The vector type
		>
	inline bool vectorContains(const std::vector<Type>& vector ///< The vector to search
		, const Type&                               value  ///< The value to look for
		) noexcept
	{
		return (std::find(std::begin(vector), std::end(vector), value) != std::end(vector));
	}

	/**
	 * \brief Check the contents of a std::vector.
	 *
	 * A convience method to make searching a vector easier.  While this 
	 * method does not save that many keystrokes, it does lead to more 
	 * readable code.
	 *
	 * \par "Example"
	 * \parblock
	 * \code
	 * std::vector<int> v = { 0, 1, 2, 3 };
	 *
	 * if(vectorContains(std::begin(v), std::end(v), 1)
	 * {
	 * 	// Found it
	 * }
	 *
	 * // Compare with the "long" form, the
	 * // value that is searhed for gets lost
	 * // very easily.
	 * if(std::find(std::begin(v), std::end(v), 1) != std::end(v))
	 * {
	 * 	// Found it
	 * }
	 * \endcode
	 * \endparblock
	 *
	 * \retval true  The \p value was found.
	 * \retval false The \p value was __not__ found.
	 */
	template <class InputIter ///< The iterator type
		, class Type      ///< The vector type
		>
	inline bool vectorContains(InputIter first ///< Start searching here
		, InputIter                  last  ///< Stop seaching here
		, const Type&                value ///< The value to look for
		) noexcept
	{
		return (std::find(first, last, value) != last);
	}
}

#endif // zakero_Base_h
