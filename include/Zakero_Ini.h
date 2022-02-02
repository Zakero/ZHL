/******************************************************************************
 * Copyright 2021-2022 Andrew Moore
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef Zakero_Ini_h
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
 * - None
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
 * \parversion{zakero_memorypool}
 * __0.2.1__
 * - The comment character is now configurable.
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
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>


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
	[[]]          std::error_code read(const std::filesystem::path, zakero::ini::Ini&) noexcept;
	[[]]          std::error_code read(const std::filesystem::path, const char, zakero::ini::Ini&) noexcept;
	[[]]          std::error_code write(const zakero::ini::Ini&, std::filesystem::path) noexcept;

	[[]]          std::error_code parse(const std::string_view, zakero::ini::Ini&) noexcept;
	[[]]          std::error_code parse(const std::string_view, const char, zakero::ini::Ini&) noexcept;
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
#define ZAKERO_MEMORYPOOL_IMPLEMENTATION

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
				return "zakero.Ini";
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
// {{{ zakero::ini::read

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
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: 'path' can not be empty"
			<< '\n'
			;

		return Error_Invalid_Parameter;
	}

	if(std::filesystem::exists(path) == false)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: 'path' does not exist: "
			<< path
			<< '\n'
			;

		return Error_Invalid_Parameter;
	}

	if(std::filesystem::is_regular_file(path) == false)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: 'path' is not a file: "
			<< path
			<< '\n'
			;

		return Error_Invalid_Parameter;
	}

	std::filesystem::perms perms = std::filesystem::status(path).permissions();

	if((bool(perms & std::filesystem::perms::owner_read) == false)
		&& (bool(perms & std::filesystem::perms::group_read) == false)
		&& (bool(perms & std::filesystem::perms::others_read) == false)
		)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: 'path' is not readable: "
			<< path
			<< '\n'
			;

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
			ZAKERO_INI_ERROR_STREAM
				<< "Error: "
				<< path
				<< ": "
				<< strerror(error)
				<< '\n'
				;
#endif // }}}
			return ZAKERO__INI_SYSTEM_ERROR(error);
		}

		return Error_Cant_Open_File;
	}

	if(fseek(file_handle, 0, SEEK_END) != 0)
	{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
		ZAKERO_INI_ERROR_STREAM
			<< "Error: "
			<< path
			<< ": Can't find end of file_handle"
			<< '\n'
			;
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
			ZAKERO_INI_ERROR_STREAM
				<< "Error: "
				<< path
				<< ": "
				<< strerror(error)
				<< '\n'
				;
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
		ZAKERO_INI_ERROR_STREAM
			<< "Error: "
			<< path
			<< ": Can't read data"
			<< '\n'
			;
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

// }}} zakero::ini::read
// {{{ zakero::ini::write

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
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: path can not be empty"
			<< '\n'
			;
	}

	std::filesystem::perms perms = std::filesystem::status(path).permissions();

	if((bool(perms & std::filesystem::perms::owner_write) == false)
		&& (bool(perms & std::filesystem::perms::group_write) == false)
		&& (bool(perms & std::filesystem::perms::others_write) == false)
		)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO__INI
			<< "Invalid Parameter: path is not writable"
			<< '\n'
			;
	}
#endif // }}}

	std::ofstream stream(path);

	stream << to_string(ini);

	return Error_None;
}

// }}} zakero::ini::write
// {{{ zakero::ini::parse

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
				&& *iter != ':'
				&& *iter != '\n'
				)
			{
				++iter;
			}

			if(*iter == '=' || *iter == ':')
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

// }}} zakero::ini::parse
// {{{ Utilities

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
		const std::unordered_map<std::string, std::string>& global_properties = ini.at("");

		if(global_properties.empty() == false)
		{
			add_blank_line = true;
		}
		else
		{
			for(auto& property_iter : global_properties)
			{
				buffer	<< property_iter.first
					<< "="
					<< property_iter.second
					<< std::endl;
			}
		}
	}

	for(auto& section_iter : ini)
	{
		if(section_iter.first == "")
		{
			continue;
		}

		if(add_blank_line)
		{
			buffer << std::endl;
		}

		buffer	<< "["
			<< section_iter.first
			<< "]"
			<< std::endl;

		for(auto& property_iter : section_iter.second)
		{
			buffer	<< property_iter.first
				<< "="
				<< property_iter.second
				<< std::endl;
		}

		add_blank_line = true;
	}

	return buffer.str();
}

// }}} Utilities
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
