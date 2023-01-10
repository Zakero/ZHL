/******************************************************************************
 * Copyright 2021-2022 Andrew Moore
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#if !defined(Zakero_Ini_h) || defined(ZAKERO_INI_IMPLEMENTATION)
#define Zakero_Ini_h

/**
 * \file
 *
 * \brief Zakero Ini
 *
|                   |                      |                      |                      |                     |                     |                         |
|-------------------|----------------------|----------------------|----------------------|---------------------|---------------------|-------------------------|
| \api{zakero::Ini} | \refdeps{zakero_ini} | \reftldr{zakero_ini} | \refwhat{zakero_ini} | \refwhy{zakero_ini} | \refhow{zakero_ini} | \refversion{zakero_ini} |
 *
 * Information about how to add _Zakero_Ini_ to your project and how to use it
 * can be found here.
 *
 * \pardeps{zakero_ini}
 * - Zakero_Base.h
 * \endpardeps
 *
 * \partldr{zakero_ini}
 * This library provides a way to read and write INI files.
 *
 * To use:
 * 1. Add the implementation to a source code file:
 *    \code
 *    #define ZAKERO_INI_IMPLEMENTATION
 *    #include "Zakero_Ini.h"
 *    \endcode
 * 2. Add the library to where it is used:
 *    \code
 *    #include "Zakero_Ini.h"
 *    \endcode
 * \endpartldr
 *
 * \parwhat{zakero_ini}
 * INI files are a common way of storing data that is easy to read for both 
 * humans and computers. The _Zakero INI_ library can read INI files and tha 
 * data within. It is also possible to store data in the INI object and then 
 * write the data.
 * \endparwhat
 *
 * \parwhy{zakero_ini}
 * Why use the _Zakero INI_ library instead of the numerous other projects that 
 * do the same thing. The _Zakero INI_ is implemented as a single file 
 * solution, making it very easy to add to your projects. And more importantly,
 * the _Zakero INI_ library only depends on the core C++ functionality. No 
 * boost, no hidden dependencies, no surprises (except for a simple C++ 
 * implementation).
 * \endparwhy
 *
 * \parhow{zakero_ini}
 * __Step 0__
 *
 * Your compiler must support at least the C++20 standard.  The location of the 
 * `Zakero_*.h` header files _must_ be in your compiler's include path.
 *
 * __Step 1__
 *
 * The first step is to select which C++ source code file will contain the 
 * _Zakero INI_ implementation.  Once the location has been determined, add the 
 * following to that file:
 *
 * ~~~
 * #define ZAKERO_INI_IMPLEMENTATION
 * #include "Zakero_Ini.h"
 * ~~~
 *
 * The macro \ref ZAKERO_INI_IMPLEMENTATION tells the header file to include 
 * the implementation of the Ini object.
 *
 * In all other files that will use the Ini object, they need to include the 
 * header.
 *
 * ~~~
 * #include "Zakero_Ini.h"
 * ~~~
 *
 * __Step 2__
 *
 * After creating a [Ini](\ref zakero::Ini::Ini) object, it can be used 
 * immediately by storing data in it or reading from a file.
 *
 * This is an example of loading an INI file, changing a value, then saving the 
 * modified INI file.
 *
 * ~~~
 * zakero::Ini ini;
 *
 * zakero::ini::read("/path/to/file.ini", ini);
 * ini["Some Section"]["foo"] = "bar";
 * zakero::ini::write(ini, "/path/to/file.ini");
 * ~~~
 * \endparhow
 *
 * \parversion{zakero_ini}
 * __0.3.0__
 * - INI values can now be treated as CSV data.
 *
 * __0.2.2__
 * - Bug Fix: Sections are added even if empty.
 * - Bug Fix: A blank line was inserted at the top of a written file.
 * - Section names and Property names are now sorted when being written.
 * - Duplicate Sections will be merged, the key/value to keep is the latest.
 *
 * __0.2.1__
 * - The comment character is now configurable.
 * - Error logging can be disabled separate from debugging error checks.
 *
 * __0.2.0__
 * - A complete rewrite based on unordered_map.
 *
 * __0.1.0__
 * - Initial version
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

// C++
#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Zakero_Base.h"


/******************************************************************************
 * Macros
 */

#ifndef ZAKERO_INI_DEBUG_ENABLED
/**
 * \brief Enable debugging.
 *
 * Enabling this macro, by setting the value to `1`, will add many data 
 * validation checks. This addition code is useful for diagonosing problems 
 * both internal to zakero::Ini as well as external.
 */
#define ZAKERO_INI_DEBUG_ENABLED 0
#endif

#ifndef ZAKERO_INI_ERROR_STREAM_ENABLED
/**
 * \brief Enable debug stream.
 *
 * Enabling this macro, by setting the value to `1`, will log error messages to 
 * the stream defined by `ZAKERO_INI_ERROR_STREAM`.
 * This addition logging is useful for diagonosing problems.
 */
#define ZAKERO_INI_ERROR_STREAM_ENABLED 0
#endif

#ifndef ZAKERO_INI_ERROR_STREAM
/**
 * \brief Where to write errors.
 *
 * Set this macro to the _stream_ to use for writing error messages.
 *
 * _Only useful if debugging is enabled._
 */
#define ZAKERO_INI_ERROR_STREAM std::cerr
#endif


/******************************************************************************
 * Objects
 */


/******************************************************************************
 * Forward Declarations
 */


/******************************************************************************
 * Typedefs / Usings
 */

namespace zakero::ini
{
	using Section = std::unordered_map<std::string, std::string>;
	using Ini     = std::unordered_map<std::string, zakero::ini::Section>;
}


/******************************************************************************
 * Structures
 */


/******************************************************************************
 * Functions
 */

namespace zakero::ini
{
	[[]]          std::error_code parse(const std::string_view, zakero::ini::Ini&) noexcept;
	[[]]          std::error_code parse(const std::string_view, const char, zakero::ini::Ini&) noexcept;

	[[]]          std::error_code read(const std::filesystem::path, zakero::ini::Ini&) noexcept;
	[[]]          std::error_code read(const std::filesystem::path, const char, zakero::ini::Ini&) noexcept;

	[[]]          std::error_code write(const zakero::ini::Ini&, std::filesystem::path) noexcept;

	[[]]          void            csvAdd(zakero::ini::Ini&, const std::string_view, const std::string_view, const std::string_view) noexcept;
	[[]]          void            csvRemove(zakero::ini::Ini&, const std::string_view, const std::string_view, const std::string_view) noexcept;
	[[]]          void            csvRemove(zakero::ini::Ini&, const std::string_view, const std::string_view, const size_t) noexcept;
	[[nodiscard]] size_t          csvCount(const zakero::ini::Ini&, const std::string_view, const std::string_view) noexcept;
	[[nodiscard]] bool            csvContains(const zakero::ini::Ini&, const std::string_view, const std::string_view, const std::string_view) noexcept;
	[[nodiscard]] int             csvIndexOf(const zakero::ini::Ini&, const std::string_view, const std::string_view, const std::string_view) noexcept;
	[[nodiscard]] std::string     csvAt(const zakero::ini::Ini&, const std::string_view, const std::string_view, const size_t) noexcept;
	[[]]          void            csvSet(zakero::ini::Ini&, const std::string_view, const std::string_view, const size_t, const std::string_view) noexcept;

	[[nodiscard]] std::string     to_string(const zakero::ini::Ini&) noexcept;
}

std::ostream& operator<<(std::ostream&, const zakero::ini::Ini&) noexcept;
std::ostream& operator<<(std::ostream&, const zakero::ini::Ini*) noexcept;


/******************************************************************************
 * Implementation
 */

// {{{ Implementation

#ifdef ZAKERO_INI_IMPLEMENTATION

namespace zakero::ini
{
// {{{ Macros
// {{{ Macros : Doxygen

#ifdef ZAKERO__DOXYGEN_DEFINE_DOCS

// Only used for generating Doxygen documentation

/**
 * \brief Activate the implementation code.
 *
 * Defining this macro will cause the zakero::Ini implementation to be 
 * included.  This should only be done once, since compiler and/or linker 
 * errors will typically be generated if more than a single implementation is 
 * found.
 *
 * \note It does not matter if the macro is given a value or not, only its 
 * existence is checked.
 */
#define ZAKERO_INI_IMPLEMENTATION

#endif

// }}}

/**
 * \internal
 *
 * \brief _Error_Table_
 *
 * An X-Macro table of error codes. The columns are:
 * -# __ErrorName__<br>
 *    The error name will be accessible as zakero::Ini::_ErrorName_
 *    <br><br>
 * -# __ErrorValue__<br>
 *    The integer value of the error
 *    <br><br>
 * -# __ErrorMessage__<br>
 *    The text that will be used by `std::error_code.message()`
 *
 * Error Codes used internally.
 */
#define ZAKERO__INI_ERROR_DATA \
	X(Error_None              , 0 , "No Error"                            ) \
	X(Error_Invalid_Parameter , 1 , "A provided parameter was not valid." ) \
	X(Error_Cant_Open_File    , 2 , "Unable to open the requested file."  ) \
	X(Error_File_Is_Bad       , 3 , "The file is currupt or invalid."     ) \

/**
 * \brief The INI identifier for error messages.
 */
#define ZAKERO__INI "Ini: "


/**
 * \brief Make generating Error Conditions less verbose.
 *
 * All this Macro Function does is expand into code that creates an 
 * std::error_code.
 *
 * \param err_ The error code
 */
#define ZAKERO__INI_SYSTEM_ERROR(err_) std::error_code(err_, std::system_category());

// }}}
// {{{ Anonymous Namespace

namespace
{
	// {{{ Declaration: IniErrorCategory_

	/**
	 * \brief INI Error Categories.
	 *
	 * This class implements the std::error_category interface to provide 
	 * consistent access to error code values and messages.
	 *
	 * See https://en.cppreference.com/w/cpp/error/error_category for 
	 * details.
	 */
	class IniErrorCategory_
		: public std::error_category
	{
		public:
			constexpr IniErrorCategory_() noexcept
			{
			}

			const char* name() const noexcept override
			{
				return "zakero.ini";
			}

			std::string message(int condition) const noexcept override
			{
				switch(condition)
				{
#define X(name_, val_, mesg_) \
					case val_: return mesg_;
					ZAKERO__INI_ERROR_DATA
#undef X
				}

				return "Unknown error condition";
			}
	} IniErrorCategory;
	
#define X(name_, val_, mesg_) \
const std::error_code name_(val_, IniErrorCategory);
ZAKERO__INI_ERROR_DATA
#undef X

	// }}}
}

// }}}
// {{{ Documentation
// }}}
// {{{ zakero::ini::parse()

/**
 * \brief Parse a string into INI data
 *
 * The raw character data in the provided \p string will be parsed and stored 
 * in the \p ini object.
 *
 * \note Any existing data in the \p ini object may be overwritten by new data.
 *
 * \bug No error checking is done, assumes properly formatted data.
 *
 * \return An error code
 */
std::error_code parse(const std::string_view string ///< The string to parse
	, Ini&                               ini    ///< The parsed INI data
	) noexcept
{
	return parse(string, '#', ini);
}


/**
 * \brief Parse a string into INI data
 *
 * The raw character data in the provided \p string will be parsed and stored 
 * in the \p ini object.
 *
 * This method provides a way to set a \p comment character. As comments are 
 * _not_ a part of the INI standard, this parser will treat any line that 
 * contains the \p comment character all the way to the end of the line as a 
 * comment and ignore it.  Recomended comment characters are:
 * - '#' (Bash Style)
 * - ';' (DOS Batch Style)
 *
 * \note Any existing data in the \p ini object may be overwritten by new data.
 *
 * \bug No error checking is done, assumes properly formatted data.
 *
 * \return An error code
 */
std::error_code parse(const std::string_view string  ///< The string to parse
	, const char                         comment ///< The comment character
	, Ini&                               ini     ///< The parsed INI data
	) noexcept
{
	std::string section = "";

	auto iter = string.begin();
	while(iter != string.end())
	{
		while(iter != string.end() && isspace(*iter))
		{
			++iter;
		}

		if(iter == string.end())
		{
			break;
		}

		// Comment
		if(*iter == comment)
		{
			while(iter != string.end() && *iter != '\n')
			{
				++iter;
			}
		}
		// Section
		else if(*iter == '[')
		{
			++iter;
			auto start = iter;

			while(iter != string.end()
				&& *iter != ']'
				&& *iter != '\n'
				)
			{
				++iter;
			}

			if(*iter == ']')
			{
				section = std::string(start, iter);
				
				if(ini.contains(section) == false)
				{
					ini[section] = {};
				}

				++iter;
			}
		}
		// Property
		else
		{
			auto start = iter;
			std::string name;
			std::string value;

			while(iter != string.end()
				&& *iter != '='
				&& *iter != '\n'
				)
			{
				++iter;
			}

			if(*iter == '=')
			{
				auto temp = iter;
				--temp;
				while(isspace(*temp))
				{
					--temp;
				}
				++temp;

				name = std::string(start, temp);

				++iter;

				while(iter != string.end()
					&& isspace(*iter)
					&& *iter != '\n'
					)
				{
					++iter;
				}

				start = iter;

				while(iter != string.end() && *iter != '\n')
				{
					++iter;
				}

				while(isspace(*iter))
				{
					--iter;
				}

				++iter;

				if(start == iter)
				{
					value = "";
				}
				else
				{
					value = std::string(start, iter);
				}

				ini[section][name] = value;
			}
		}
	}

	return Error_None;
}

// }}} zakero::ini::parse()
// {{{ zakero::ini::read()

/**
 * \brief Read data from a file.
 *
 * INI file data can be loaded into this object by using this method.  The file 
 * will overwrite any existing data.
 *
 * \examplecode
 * zakero::Ini prefs;
 *
 * zakero::ini::parse(defaults, prefs);
 * zakero::ini::read("/path/to/some_file.ini", prefs);
 * \endexamplecode
 *
 * \return An error code.
 */
std::error_code read(const std::filesystem::path path ///< The file path
	, zakero::ini::Ini&                      ini  ///< Where to store the data
	) noexcept
{
	return read(path, '\0', ini);
}


/**
 * \brief Read data from a file.
 *
 * INI file data can be loaded into this object by using this method.  The file 
 * will overwrite any existing data.
 *
 * This method provides a way to set a \p comment character. As comments are 
 * _not_ a part of the INI standard, this parser will treat any line that 
 * contains the \p comment character all the way to the end of the line as a 
 * comment and ignore it.  Recomended comment characters are:
 * - '#' (Bash Style)
 * - ';' (DOS Batch Script Style)
 *
 * \examplecode
 * zakero::Ini prefs;
 *
 * zakero::ini::parse(defaults, prefs);
 * zakero::ini::read("/path/to/some_file.ini", '#', prefs);
 * \endexamplecode
 *
 * \return An error code.
 */
std::error_code read(const std::filesystem::path path    ///< The file path
	, const char                             comment ///< The comment character
	, zakero::ini::Ini&                      ini     ///< Where to store the data
	) noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(path.empty())
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: 'path' can not be empty"
			<< '\n'
			;
		#endif

		return Error_Invalid_Parameter;
	}

	if(std::filesystem::exists(path) == false)
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: 'path' does not exist: "
			<< path
			<< '\n'
			;
		#endif

		return Error_Invalid_Parameter;
	}

	if(std::filesystem::is_regular_file(path) == false)
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: 'path' is not a file: "
			<< path
			<< '\n'
			;
		#endif

		return Error_Invalid_Parameter;
	}

	std::filesystem::perms perms = std::filesystem::status(path).permissions();

	if((bool(perms & std::filesystem::perms::owner_read) == false)
		&& (bool(perms & std::filesystem::perms::group_read) == false)
		&& (bool(perms & std::filesystem::perms::others_read) == false)
		)
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: 'path' is not readable: "
			<< path
			<< '\n'
			;
		#endif

		return Error_Invalid_Parameter;
	}
#endif // }}}

	errno = 0;
	FILE* file_handle = fopen(path.c_str(), "rb");

	if(file_handle == nullptr)
	{
		auto error = errno;

		if(error != 0)
		{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
			#if ZAKERO_INI_ERROR_STREAM_ENABLED
			ZAKERO_INI_ERROR_STREAM
				<< "Error: "
				<< path
				<< ": "
				<< strerror(error)
				<< '\n'
				;
			#endif
#endif // }}}
			return ZAKERO__INI_SYSTEM_ERROR(error);
		}

		return Error_Cant_Open_File;
	}

	if(fseek(file_handle, 0, SEEK_END) != 0)
	{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM
			<< "Error: "
			<< path
			<< ": Can't find end of file_handle"
			<< '\n'
			;
		#endif
#endif // }}}
		fclose(file_handle);

		return Error_File_Is_Bad;
	}

	errno = 0;
	auto file_tell = ftell(file_handle);
	
	if(file_tell == -1)
	{
		auto error = errno;
		if(error != 0)
		{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
			#if ZAKERO_INI_ERROR_STREAM_ENABLED
			ZAKERO_INI_ERROR_STREAM
				<< "Error: "
				<< path
				<< ": "
				<< strerror(error)
				<< '\n'
				;
			#endif
#endif // }}}
			return ZAKERO__INI_SYSTEM_ERROR(error);
		}

		fclose(file_handle);

		return Error_File_Is_Bad;
	}

	size_t file_size = file_tell;

	rewind(file_handle);

	std::vector<char> data;
	data.resize(file_size);

	size_t bytes = fread(data.data(), 1, file_size, file_handle);

	fclose(file_handle);

	if(bytes != file_size)
	{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM
			<< "Error: "
			<< path
			<< ": Can't read data"
			<< '\n'
			;
		#endif
#endif // }}}
		data.clear();

		return Error_File_Is_Bad;
	}

	std::error_code error = parse(std::string_view(data.begin(), data.end())
		, comment
		, ini
		);

	return error;
}

// }}} zakero::ini::read()
// {{{ zakero::ini::write()

/**
 * \brief Write data to a file.
 *
 * Use this method to write the \p ini data in this object to the file 
 * specified by \p path.
 *
 * \examplecode
 * Ini config;
 *
 * zakero::ini::read("/path/to/config.ini", config);
 * zakero::ini::write(config, "/path/to/backup.ini");
 * \endexamplecode
 */
std::error_code write(const Ini& ini  ///< The data to write
	, std::filesystem::path  path ///< The file to be written
	) noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(path.empty())
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: path can not be empty"
			<< '\n'
			;
		#endif
	}

	std::filesystem::perms perms = std::filesystem::status(path).permissions();

	if((bool(perms & std::filesystem::perms::owner_write) == false)
		&& (bool(perms & std::filesystem::perms::group_write) == false)
		&& (bool(perms & std::filesystem::perms::others_write) == false)
		)
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: path is not writable"
			<< '\n'
			;
		#endif
	}
#endif // }}}

	std::ofstream stream(path);

	stream << to_string(ini);

	return Error_None;
}

// }}} zakero::ini::write()
// {{{ zakero::ini::csv

/**
 * \brief Add a value to CSV formatted data.
 *
 * The provided \p value will be added to the value of the \p key.
 *
 * \examplecode
 * zakero::ini::Ini ini = {};
 *
 * zakero::ini::csvAdd(ini, "section", "key", "foo");
 * // [section]
 * // key=foo
 *
 * zakero::ini::csvAdd(ini, "section", "key", "bar");
 * // [section]
 * // key=foo,bar
 * \endexamplecode
 */
void csvAdd(zakero::ini::Ini& ini        ///< The INI data
	, const std::string_view section ///< The Section Name
	, const std::string_view key     ///< The Key
	, const std::string_view value   ///< The Value
	) noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(ini.contains(std::string(section)) == false)
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: ini does not contain section"
			<< '\n'
			;
		#endif
	}

	if(ini.at(std::string(section)).contains(std::string(key)) == false)
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: The ini section does not contain key"
			<< '\n'
			;
		#endif
	}
#endif // }}}

	/**
	 * \internal
	 * While just appending the value to the CSV would be faster, the goal 
	 * here was to keep the same "code pattern" as the other csv*() 
	 * functions.
	 */

	const std::string val(value);
	std::string& csv = ini[std::string(section)][std::string(key)];
	std::vector<std::string> vector = zakero::split(csv, ",");

	vector.push_back(val);
	csv = zakero::join(vector, ",");
}


/**
 * \brief Remove a value from CSV formatted data.
 *
 * Removes the requested \p value from the CSV formatted data.
 * If the \p value does not exist, then nothing will happen.
 *
 * \examplecode
 * // ini contains:
 * // [section]
 * // key=foo,bar
 *
 * zakero::ini::csvRemove(ini, "section", "key", "foo");
 * // [section]
 * // key=bar
 * \endexamplecode
 */
void csvRemove(zakero::ini::Ini& ini     ///< The INI data
	, const std::string_view section ///< The Section Name
	, const std::string_view key     ///< The Key
	, const std::string_view value   ///< The Value
	) noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(ini.contains(std::string(section)) == false)
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: ini does not contain section"
			<< '\n'
			;
		#endif
	}

	if(ini.at(std::string(section)).contains(std::string(key)) == false)
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: The ini section does not contain key"
			<< '\n'
			;
		#endif
	}
#endif // }}}

	const std::string val(value);
	std::string& csv = ini[std::string(section)][std::string(key)];
	std::vector<std::string> vector = zakero::split(csv, ",");

	if(zakero::vectorContains(vector, val) == false)
	{
		return;
	}

	zakero::vectorErase(vector, val);
	csv = zakero::join(vector, ",");
}


/**
 * \brief Remove a value from CSV formatted data.
 *
 * Removes a value from the CSV formatted data at the specified \p index.
 *
 * \examplecode
 * // ini contains:
 * // [section]
 * // key=foo,bar,xyzzy
 *
 * zakero::ini::csvRemove(ini, "section", "key", 1);
 * // [section]
 * // key=foo,xyzzy
 * \endexamplecode
 */
void csvRemove(zakero::ini::Ini& ini     ///< The INI data
	, const std::string_view section ///< The Section Name
	, const std::string_view key     ///< The Key
	, const size_t index             ///< The index of the Value
	) noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(ini.contains(std::string(section)) == false)
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: ini does not contain section"
			<< '\n'
			;
		#endif
	}

	if(ini.at(std::string(section)).contains(std::string(key)) == false)
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: The ini section does not contain key"
			<< '\n'
			;
		#endif
	}
#endif // }}}

	std::string& csv = ini[std::string(section)][std::string(key)];

	std::vector<std::string> vector = zakero::split(csv, ",");

	vector.erase(vector.begin() + index);

	csv = zakero::join(vector, ",");
}


/**
 * \brief Count the number of values in CSV formatted data.
 *
 * The number of items in the CSV formatted data will be returned.
 *
 * \return The number of items.
 *
 * \examplecode
 * // ini contains:
 * // [section]
 * // key=foo,bar,xyzzy
 *
 * size_t count = zakero::ini::csvCount(ini, "section", "key");
 * // count == 3
 * \endexamplecode
 */
size_t csvCount(const zakero::ini::Ini& ini ///< The INI data
	, const std::string_view section    ///< The Section Name
	, const std::string_view key        ///< The Key
	) noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(ini.contains(std::string(section)) == false)
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: ini does not contain section"
			<< '\n'
			;
		#endif
	}

	if(ini.at(std::string(section)).contains(std::string(key)) == false)
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: The ini section does not contain key"
			<< '\n'
			;
		#endif
	}
#endif // }}}

	const std::string& csv = ini
		.at(std::string(section))
		.at(std::string(key));

	if(csv.empty())
	{
		return 0;
	}
	
	size_t count = 1;

	for(size_t i = 0; i < csv.size(); i++)
	{
		if(csv[i] == ',')
		{
			count++;
		}
	}

	return count;
}


/**
 * \brief Check if the CSV formatted data contains a value.
 *
 * The CSV data located at the \p section and \p key will be checked if it 
 * contains the specified \p value.
 *
 * \retval true  The value was found
 * \retval false The value was not found
 *
 * \examplecode
 * // ini contains:
 * // [section]
 * // key=foo,bar,xyzzy
 *
 * bool found = zakero::ini::csvContains(ini, "section", "key", "foo");
 * // found == true
 * \endexamplecode
 */
bool csvContains(const zakero::ini::Ini& ini ///< The INI data
	, const std::string_view section     ///< The Section Name
	, const std::string_view key         ///< The Key
	, const std::string_view value       ///< The Value
	) noexcept
{
	// ???
	// vector v = split(csv, ",")
	// bool b = contains(v, value)
	// return b
	// ???

	if(ini.contains(std::string(section)) == false)
	{
		return false;
	}

	if(ini.at(std::string(section)).contains(std::string(key)) == false)
	{
		return false;
	}

	const std::string_view csv = ini
		.at(std::string(section))
		.at(std::string(key));

	if(csv.size() < value.size())
	{
		return false;
	}

	if(csv.starts_with(value))
	{
		if(csv.size() == value.size())
		{
			return true;
		}

		if(csv[value.size()] == ',')
		{
			return true;
		}
	}
	
	if(csv.ends_with(value))
	{
		if(csv[csv.size() - value.size() - 1] == ',')
		{
			return true;
		}
	}

	size_t location = 0;
	while(true)
	{
		location = csv.find(value, location + value.size());
		if(location == csv.npos)
		{
			return false;
		}

		if((csv[location - 1] == ',')
			&& (csv[location + value.size()] == ',')
			)
		{
			return true;
		}
	}

	return false;
}


/**
 * \brief Get the index of a value in CSV formatted data.
 *
 * The CSV data located at \p section and \p key will be searched for the 
 * requested \p value. If the value is found, the index of the value is 
 * returned.
 *
 * \return The index of the value or `-1` if the value was not found.
 *
 * \examplecode
 * // ini contains:
 * // [section]
 * // key=foo,bar,xyzzy
 *
 * int index = zakero::ini::csvContains(ini, "section", "key", "xyzzy");
 * // index == 2
 * \endexamplecode
 */
int csvIndexOf(const zakero::ini::Ini& ini ///< The INI data
	, const std::string_view section   ///< The Section Name
	, const std::string_view key       ///< The Key
	, const std::string_view value     ///< The Value
	) noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(ini.contains(std::string(section)) == false)
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: ini does not contain section"
			<< '\n'
			;
		#endif
	}

	if(ini.at(std::string(section)).contains(std::string(key)) == false)
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: The ini section does not contain key"
			<< '\n'
			;
		#endif
	}
#endif // }}}

	const std::string_view csv = ini
		.at(std::string(section))
		.at(std::string(key));
	
	std::vector<std::string> vector = zakero::split(csv, ",");

	int index = 0;

	for(const std::string& csv_value : vector)
	{
		if(csv_value == value)
		{
			return index;
		}

		index++;
	}

	return -1;
}


/**
 * \brief Get a Value from CSV formatted data.
 *
 * The value located at the specified \p index will be returned.
 *
 * \return The value.
 *
 * \examplecode
 * // ini contains:
 * // [section]
 * // key=foo,bar,xyzzy
 *
 * std::string str = zakero::ini::csvAt(ini, "section", "key", 0);
 * // str == "foo"
 * \endexamplecode
 */
std::string csvAt(const zakero::ini::Ini& ini ///< The INI data
	, const std::string_view section      ///< The Section Name
	, const std::string_view key          ///< The Key
	, const size_t           index        ///< The index
	) noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(ini.contains(std::string(section)) == false)
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: ini does not contain section"
			<< '\n'
			;
		#endif
	}

	if(ini.at(std::string(section)).contains(std::string(key)) == false)
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: The ini section does not contain key"
			<< '\n'
			;
		#endif
	}
#endif // }}}

	const std::string& csv = ini.at(std::string(section)).at(std::string(key));

	const std::vector<std::string> vector = zakero::split(csv, ",");

	return vector.at(index);
}


/**
 * \brief Set a value at a location in CSV formatted data.
 *
 * The data at the provided \p index will be replaced with the specified \p 
 * value.
 *
 * \examplecode
 * // ini contains:
 * // [section]
 * // key=foo,bar,xyzzy
 *
 * zakero::ini::csvSet(ini, "section", "key", 2, "UUDDLRLRBA");
 * // [section]
 * // key=foo,bar,UUDDLRLRBA
 * \endexamplecode
 */
void csvSet(zakero::ini::Ini& ini        ///< The INI data
	, const std::string_view section ///< The Section Name
	, const std::string_view key     ///< The Key
	, const size_t           index   ///< The index
	, const std::string_view value   ///< The new Value
	) noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(ini.contains(std::string(section)) == false)
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: ini does not contain section"
			<< '\n'
			;
		#endif
	}

	if(ini.at(std::string(section)).contains(std::string(key)) == false)
	{
		#if ZAKERO_INI_ERROR_STREAM_ENABLED
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: The ini section does not contain key"
			<< '\n'
			;
		#endif
	}
#endif // }}}

	std::string& csv = ini[std::string(section)][std::string(key)];

	std::vector<std::string> vector = zakero::split(csv, ",");

	vector[index] = std::string(value);

	csv = zakero::join(vector, ",");
}


// }}}
// {{{ zakero::ini::to_string()

/**
 * \brief Convert into a string.
 *
 * The provided \p ini object will be converted into a propertly formatted 
 * string.
 *
 * \return A string.
 */
std::string to_string(const Ini& ini ///< The INI data
	) noexcept
{
	std::stringstream buffer         = {};
	bool              add_blank_line = false;

	if(ini.contains(""))
	{
		const Section& global_properties = ini.at("");

		if(global_properties.empty() == false)
		{
			for(const auto& property_iter : global_properties)
			{
				buffer	<< property_iter.first
					<< "="
					<< property_iter.second
					<< std::endl;
			}

			add_blank_line = true;
		}
	}

	std::vector<std::string> section_list = {};

	for(const auto& section_iter : ini)
	{
		const std::string& section_name = section_iter.first;

		if(section_name == "")
		{
			// An empty key is a global property
			// which has already been done.
			continue;
		}

		section_list.push_back(section_name);
	}

	std::sort(section_list.begin(), section_list.end());

	for(std::string& section_name : section_list)
	{
		if(add_blank_line)
		{
			buffer << std::endl;
		}

		buffer	<< "["
			<< section_name
			<< "]"
			<< std::endl;

		std::vector<std::string> property_list = {};
		const Section& section = ini.at(section_name);

		for(const auto& property_iter : section)
		{
			const std::string& property_name = property_iter.first;

			property_list.push_back(property_name);
		}

		std::sort(property_list.begin(), property_list.end());

		for(std::string& property_name : property_list)
		{
			buffer	<< property_name
				<< "="
				<< section.at(property_name)
				<< std::endl;
		}

		add_blank_line = true;
	}

	return buffer.str();
}

// }}} zakero::ini::to_string()
}

// {{{ Operators

/**
 * \brief Write INI data to a stream.
 *
 * This method will write the INI data to the specified stream.
 *
 * \examplecode
 * Ini ini;
 *
 * zakero::ini::read("/path/to/config.ini", ini);
 *
 * std::cout << ini;
 * \endexamplecode
 */
std::ostream& operator<<(std::ostream& stream
	, const zakero::ini::Ini&      ini
	) noexcept
{
	stream << zakero::ini::to_string(ini);

	return stream;
}


/**
 * \brief Write INI data to a stream.
 *
 * This method will write the INI data to the specified stream.
 *
 * \examplecode
 * Ini* ini = new Ini();
 *
 * zakero::ini::read("/path/to/config.ini", *ini);
 *
 * std::cout << ini;
 * \endexamplecode
 */
std::ostream& operator<<(std::ostream& stream
	, const zakero::ini::Ini*      ini
	) noexcept
{
	stream << zakero::ini::to_string(*ini);

	return stream;
}

// }}} Operators

#endif // ZAKERO_INI_IMPLEMENTATION

// }}}

#endif // Zakero_Ini_h
