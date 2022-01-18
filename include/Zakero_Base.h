/******************************************************************************
 * Copyright 2020-2022 Andrew Moore
 * 
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
 *
 * \parversion{zakero_base}
 * __v0.9.5__
 * - Added mapKeys()
 *
 * __v0.9.4__
 * - Added ZAKERO_DISABLE_IMPLICIT_CASTS()
 *
 * __v0.9.3__
 * - Added stob()
 * - Added tolower()
 * - Added to_string(const bool)
 * - Added to_string(std::chrono::nanoseconds nanoseconds)
 *
 * __v0.9.2__
 * - Added to_string(std::error_code)
 * - Added operator<<(std::ostream&, std::error_code)
 * - Added ZAKERO_DELETE()
 * - Added ZAKERO_FREE()
 * - Added ZAKERO_UNUSED()
 * - Added ZAKERO_PID
 * - Added equalish()
 *
 * __v0.9.1__
 * - Added macro ZAKERO_STEADY_TIME_NOW()
 * - Added macro ZAKERO_SYSTEM_TIME_NOW()
 * - Added vectorErase()
 *
 * __v0.9.0__
 * - The initial collection
 * \endparversion
 *
 *
 * \copyright [Mozilla Public License 
 * v2](https://www.mozilla.org/en-US/MPL/2.0/) 
 *
 *
 * \author Andrew "Zakero" Moore
 * - Original Author
 */


/******************************************************************************
 * Includes
 */

#include <chrono>
#include <locale>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>


/******************************************************************************
 * Macros
 */

// {{{ Macros

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
 * \brief Delete memory
 *
 * Using this macro will help catch instances of using a pointer after 
 * delete'ing the new'ed memory.  It does this by setting the value of \p ptr_ 
 * to \null after calling `delete`.
 *
 * \param  ptr_  The value to free.
 *
 * \note The `++p` and `p++` syntax should work:
 * > ZAKERO_DELETE(p++);
 * > ZAKERO_DELETE(p++);
 */
#define ZAKERO_DELETE(ptr_) \
{                                    \
	auto*& var##__LINE__ = ptr_; \
	delete var##__LINE__;        \
	var##__LINE__ = nullptr;     \
}                                    \


/**
 * \brief Free memory
 *
 * Using this macro will help catch instances of using a pointer after free'ing 
 * the memory.  It does this by setting the value of \p ptr_ to \null after 
 * calling free().
 *
 * \param  ptr_  The value to free.
 *
 * \note The `++p` and `p++` syntax should work:
 * > ZAKERO_FREE(p++);
 * > ZAKERO_FREE(p++);
 */
#define ZAKERO_FREE(ptr_) \
{                                    \
	auto*& var##__LINE__ = ptr_; \
	free(var##__LINE__);         \
	var##__LINE__ = nullptr;     \
}                                    \


/**
 * \brief Don't allow implicit parameter conversion.
 *
 * When passing a value to a function's parameter which does not have a 
 * matching type, the compiler will try to inject code to convert the value 
 * into the function's expected type. Usually, this is fine. But in some 
 * instances can lead to very subtle bugs not to mention the possible minor hit 
 * in performance. Placing the function name in this macro will prevent the 
 * compiler from doing this automatic type conversion.
 *
 * \param func_name_ The name of the function.
 */
#define ZAKERO_DISABLE_IMPLICIT_CASTS(func_name_) \
	template <typename... T>                  \
	void func_name_(T...) = delete            \
	;


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


/**
 * \def ZAKERO_PID
 *
 * \brief Get the Process Id.
 *
 * Get the ID of the current process.
 *
 * \parcode
 * std::cout << "My PID is " << std::to_string(ZAKERO_PID) << '\n';
 * \endparcode
 *
 * \return The Process Id (`pid_t`)
 *
 * \note POSIX.1-2008
 */
#ifdef __linux__
#	include <sys/types.h>
#	include <unistd.h>
#	define ZAKERO_PID getpid()
#else
#	define ZAKERO_PID (pid_t)-1
#endif


/**
 * \brief Get the current time.
 *
 * This macro will get the current time count of the 
 * `std::chrono::steady_clock`.
 *
 * \note The `std::chrono` namespace is automatically prepended to the \p 
 * unit_.
 *
 * \parcode
 * auto time_now = ZAKERO_STEADY_TIME_NOW(milliseconds);
 * \endparcode
 *
 * \param unit_ The time unit to get.
 */
#define ZAKERO_STEADY_TIME_NOW(unit_) \
	std::chrono::duration_cast<std::chrono::unit_>(             \
		std::chrono::steady_clock::now().time_since_epoch() \
		).count()                                           \


/**
 * \brief Get the current time.
 *
 * This macro will get the current time count of the 
 * `std::chrono::system_clock`.
 *
 * \note The `std::chrono` namespace is automatically prepended to the \p 
 * unit_.
 *
 * \parcode
 * auto time_now = ZAKERO_SYSTEM_TIME_NOW(milliseconds);
 * \endparcode
 *
 * \param unit_ The time unit to get.
 */
#define ZAKERO_SYSTEM_TIME_NOW(unit_) \
std::chrono::duration_cast<std::chrono::unit_>(             \
        std::chrono::steady_clock::now().time_since_epoch() \
        ).count()                                           \


/**
 * \brief Prevent "unused variable" compilier warnings.
 *
 * \param var_ The "unused variable".
 */
#define ZAKERO_UNUSED(var_) \
	(void)(var_);

// }}}

namespace zakero
{
	/**
	 * \brief Conversion Type.
	 *
	 * Sizes in powers of 2.
	 */
	enum struct Storage : uint64_t
	{	Byte     = 0x0000'0000'0000'0001 ///< 1 byte
	,	Kilobyte = 0x0000'0000'0000'0400 ///< 1024 bytes
	,	Megabyte = 0x0000'0000'0010'0000 ///< 1024 kilobytes
	,	Gigabyte = 0x0000'0000'4000'0000 ///< 1024 megabytes
	,	Terabyte = 0x0000'0100'0000'0000 ///< 1024 gigabytes
	,	Petabyte = 0x0004'0000'0000'0000 ///< 1024 terabytes
	,	Exabyte  = 0x1000'0000'0000'0000 ///< 1024 petabytes
	};


	/*
	 * Doxygen Bug: For some reason doxygen can not parse aliases 
	 * only/specifically in this file and in this namespace.
	 */

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
	[[nodiscard]]
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
	[[nodiscard]]
	inline double convert(const double size ///< The size to convert
		, const zakero::Storage    from ///< The source unit
		, const zakero::Storage    to   ///< The destination unit
		) noexcept
	{
		return size * static_cast<uint64_t>(from) / static_cast<uint64_t>(to);
	}


	/**
	 * \brief Compare two floats
	 *
	 * Compare two floats for some degree of equality.  For the float 
	 * values to be considered "equal", the difference between the values 
	 * must be less than the specified \p delta.
	 *
	 * \retval true  The values are equal
	 * \retval false The values are not equal
	 */
	[[nodiscard]]
	inline bool equalish(const float a     ///< The first value
		, const float            b     ///< The second value
		, const float            delta ///< The maximum difference
		) noexcept
	{
		return (std::abs(a - b) < delta);
	}


	/**
	 * \brief Convert a string to lower case.
	 *
	 * The provided string, \p str, will be converted to lower-case using 
	 * the default locale.
	 *
	 * \return The lower-case string.
	 */
	[[nodiscard]]
	inline std::string tolower(std::string str ///< The string to convert
		) noexcept
	{
		std::locale locale;

		for(auto& c : str)
		{
			c = std::tolower(c, locale);
		}

		return str;
	}


	/**
	 * \brief Get the keys in a map.
	 *
	 * Use this method to create a vector of all the keys in the provided 
	 * \p map.
	 *
	 * \note A copy of each key is made.
	 *
	 * \return A vector of keys.
	 */
	template <class Key   ///< The key type
		, class Value ///< The value type
		>
	[[nodiscard]]
	inline std::vector<Key> mapKeys(const std::map<Key, Value>& map ///< The map
		) noexcept
	{
		std::vector<Key> vector;
		vector.reserve(map.size());

		for(const auto& iter : map)
		{
			vector.push_back(iter.first);
		}

		return vector;
	}


	/**
	 * \brief Get the keys in a map.
	 *
	 * Use this method to create a vector of all the keys in the provided 
	 * \p map.
	 *
	 * \note A copy of each key is made.
	 *
	 * \return A vector of keys.
	 */
	template <class Key   ///< The key type
		, class Value ///< The value type
		>
	[[nodiscard]]
	inline std::vector<Key> mapKeys(const std::unordered_map<Key, Value>& map ///< The map
		) noexcept
	{
		std::vector<Key> vector;
		vector.reserve(map.size());

		for(const auto& iter : map)
		{
			vector.push_back(iter.first);
		}

		return vector;
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
	[[nodiscard]]
	inline bool vectorContains(const std::vector<Type>& vector ///< The vector to search
		, const Type&                               value  ///< The value to look for
		) noexcept
	{
		return (std::find(std::begin(vector), std::end(vector), value) != std::end(vector));
	}


	/**
	 * \brief Check the contents of a std::vector.
	 *
	 * A convenience method to make searching a vector easier.  While this 
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
	 * // value that is searched for gets lost
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
	[[nodiscard]]
	inline bool vectorContains(InputIter first ///< Start searching here
		, InputIter                  last  ///< Stop searching here
		, const Type&                value ///< The value to look for
		) noexcept
	{
		return (std::find(first, last, value) != last);
	}


	/**
	 * \brief Erase the contents of a std::vector.
	 *
	 * A convenience method to make removing content from a vector easier.  
	 * Uses the "Erase/Remove Idiom".
	 *
	 * \par "Example"
	 * \parblock
	 * \code
	 * std::vector<int> v = { 0, 1, 2, 3 };
	 *
	 * vectorErase(v, 2);
	 * // v = { 0, 1, 3 };
	 * \endcode
	 * \endparblock
	 *
	 * \retval true  The \p value was found.
	 * \retval false The \p value was __not__ found.
	 */
	template <class Type ///< The vector type
		>
	inline auto vectorErase(std::vector<Type>& vector ///< The vector to search
		, const Type&                      value  ///< The value to look for
		) noexcept
	{
		return vector.erase(std::remove(std::begin(vector), std::end(vector), value), std::end(vector));
	}


	/**
	 * \brief Erase the contents of a std::vector.
	 *
	 * A convenience method to make removing content from a vector easier.  
	 * The order of the element in the \p vector are __NOT__ preserved. The 
	 * reason is to make the removal of the data at the \p iter location as 
	 * fast as possible.
	 *
	 * \par "Example"
	 * \parblock
	 * \code
	 * std::vector<int> v = { 0, 1, 2, 3 };
	 *
	 * auto iter = std::next(std::begin(v));
	 *
	 * vectorErase(v, iter);
	 * // v = { 0, 3, 2 };
	 * \endcode
	 * \endparblock
	 *
	 * \retval true  The \p value was found.
	 * \retval false The \p value was __not__ found.
	 */
	template <class Type ///< The vector type
		>
	inline auto vectorErase(std::vector<Type>&     vector ///< The vector to search
		, typename std::vector<Type>::iterator iter   ///< The iterator to erase
		) noexcept
	{
		if(vector.empty())
		{
			return vector.end();
		}

		auto last = std::prev(std::end(vector));

		if(last != iter)
		{
			iter_swap(iter, last);
		}

		return vector.erase(last);
	}


	/**
	 * \brief Convert a string into a boolean value.
	 *
	 * \todo Make the list of "true" string values convertible.  
	 * (Compile-Time or Run-Time?)
	 *
	 * This is intended to be a complement to the std::stod() and 
	 * std::stoi() family of functions.  However, the main restriction is 
	 * this method only accepts certain string values to represent a 
	 * boolean `true` value:
	 * - "enable"
	 * - "enabled"
	 * - "true"
	 * - "t"
	 * - "yes"
	 * - "y"
	 * - "1"
	 * If the provided \p str is not one of the above strings, `false` will 
	 * be returned.
	 *
	 * \note The string comparison is __not__ case-sensitive.
	 *
	 * \return The string converted to a boolean value.
	 */
	[[nodiscard]]
	inline bool stob(const std::string_view str ///< The value to convert.
		) noexcept
	{
		static std::vector<std::string> v =
		{	"enable"
		,	"enabled"
		,	"true"
		,	"t"
		,	"yes"
		,	"y"
		,	"1"
		};

		return (vectorContains(v, tolower(std::string(str))));
	}


	/**
	 * \brief Convert a bool into a string.
	 *
	 * Converts a bool value in to either "true" or "false".
	 *
	 * \return A string
	 */
	[[nodiscard]]
	inline std::string to_string(const bool value ///< The value
		) noexcept
	{
		if(value)
		{
			return std::string("true");
		}

		return std::string("false");
	}


	/**
	 * \brief Convert nanoseconds into a string.
	 *
	 * The nanoseconds value will be converted into a string that contains 
	 * "days", "hours", "minutes", "seconds", and (of course) 
	 * "nanoseconds".
	 *
	 * \return A string
	 */
	[[nodiscard]]
	inline std::string to_string(std::chrono::nanoseconds nanoseconds ///< The value
		) noexcept
	{
		typedef std::chrono::duration<int64_t, std::ratio<86400>> duration_days;

		const auto days = std::chrono::duration_cast<duration_days>(nanoseconds);
		nanoseconds -= days;

		const auto hours = std::chrono::duration_cast<std::chrono::hours>(nanoseconds);
		nanoseconds -= hours;

		const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(nanoseconds);
		nanoseconds -= minutes;

		const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(nanoseconds);
		nanoseconds -= seconds;

		return    "{ \"days\": "        + std::to_string((int64_t)days.count())
			+ ", \"hours\": "       + std::to_string((int64_t)hours.count())
			+ ", \"minutes\": "     + std::to_string((int64_t)minutes.count())
			+ ", \"seconds\": "     + std::to_string((int64_t)seconds.count())
			+ ", \"nanoseconds\": " + std::to_string((int64_t)nanoseconds.count())
			+ " }";
	}


	/**
	 * \brief Convert an std::error_code to a std::string.
	 *
	 * The provided \p error will be converted to a string.
	 *
	 * \return A string
	 */
	[[nodiscard]]
	inline std::string to_string(const std::error_code& error ///< The value
		) noexcept
	{
		return std::string()
			+ "{ \"category\": \"" + error.category().name() + "\""
			+ ", \"value\": "      + std::to_string(error.value())
			+ ", \"message\": \""  + error.message() + "\""
			+ " }";
	}
} // namespace zakero


/**
 * \brief Insert std::error_code into an output stream.
 *
 * \return The \p stream.
 */
[[nodiscard]]
inline std::ostream& operator<<(std::ostream& stream ///< The stream to use
	, const std::error_code&              error  ///< The value in insert into the stream
	) noexcept
{
	stream
		<< "{ \"category\": \"" << error.category().name() << "\""
		<< ", \"value\": "      << std::to_string(error.value())
		<< ", \"message\": \""  << error.message() << "\""
		<< " }"
		;

	return stream;
}

#endif // zakero_Base_h
