/******************************************************************************
 * Copyright 2018-2021 Andrew Moore
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
 * zakero::Ini ini();
 *
 * ini.load("/path/to/file.ini");
 * ini.propertyMap("Some Section")["foo"] = "bar";
 * ini.save("/path/to/file.ini");
 * ~~~
 * \endparhow
 *
 * \parversion{zakero_memorypool}
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
 *
 * \todo Improve the usability of the API.
 */

/******************************************************************************
 * Defines
 */


/******************************************************************************
 * Includes
 */

// C++
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

// POSIX

// Linux


/******************************************************************************
 * Macros
 */

// {{{ Macros

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
#define ZAKERO_INI__ERROR_DATA \
	X(Error_None              , 0 , "No Error"                            ) \
	X(Error_Invalid_Parameter , 1 , "A provided parameter was not valid." ) \
	X(Error_Cant_Open_File    , 2 , "Unable to open the requested file."  ) \
	X(Error_File_Is_Bad       , 3 , "The file is currupt or invalid."     ) \

// }}}


/******************************************************************************
 * Objects
 */


/******************************************************************************
 * Forward Declarations
 */


/******************************************************************************
 * Typedefs / Usings
 */


/******************************************************************************
 * Structures
 */


/******************************************************************************
 * Classes
 */

namespace zakero
{
	// {{{ Declaration

	static class IniErrorCategory_
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
					ZAKERO_INI__ERROR_DATA
#undef X
				}

				return "Unknown error condition";
			}
	} IniErrorCategory;

	class Ini
	{
		public:
			using SectionVector = std::vector<std::string>;
			using PropertyMap   = std::unordered_map<std::string, std::string>;

#define X(name_, val_, mesg_) \
			static const std::error_code name_;
			ZAKERO_INI__ERROR_DATA
#undef X

			Ini() noexcept;

			// --- Mass Update --- //
			[[]]          void               clear() noexcept;

			// --- I/O Operations --- //
			[[]]          std::error_code    load(const uint8_t*, const size_t) noexcept;
			[[]]          std::error_code    load(const std::filesystem::path) noexcept;
			[[]]          void               save(const std::filesystem::path) const noexcept;
			[[]]          void               save(std::ostream&) const noexcept;

			// --- Section --- //
			[[]]          void               sectionCreate(const std::string_view) noexcept;
			[[]]          void               sectionDestroy(const std::string_view) noexcept;
			[[]]          void               sectionRename(const std::string_view, const std::string_view) noexcept;
			[[nodiscard]] bool               sectionExists(const std::string_view) const noexcept;
			[[nodiscard]] SectionVector      sectionList() const noexcept;
			[[nodiscard]] SectionVector      sectionList(const std::string_view) const noexcept;

			// --- Property --- //
			[[nodiscard]] std::string        property(const std::string_view, const std::string_view) const noexcept;
			[[nodiscard]] std::string        property(const std::string_view, const std::string_view, std::string) const noexcept;
			[[]]          void               propertyCreate(const std::string_view, const std::string_view, const std::string_view) noexcept;
			[[]]          void               propertyDestroy(const std::string_view, const std::string_view) noexcept;
			[[nodiscard]] bool               propertyExists(const std::string_view, const std::string_view) const noexcept;
			[[nodiscard]] PropertyMap&       propertyMap(const std::string_view) noexcept;
			[[nodiscard]] const PropertyMap& propertyMap(const std::string_view) const noexcept;

		protected:

		private:
			using VectorUint8 = std::vector<uint8_t>;

			std::unordered_map<std::string, PropertyMap> data = { };

			std::error_code parse(VectorUint8) noexcept;

			Ini(const Ini&) = delete;
			Ini& operator=(const Ini&) = delete;

			Ini(Ini&&) = delete;
			Ini& operator=(Ini&&) = delete;
	};

	// }}}
}

// {{{ Implementation

#ifdef ZAKERO_INI_IMPLEMENTATION

namespace zakero
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
 * \brief The INI identifier for error messages.
 */
#define ZAKERO_INI_ "Ini(0x" << std::hex << this << std::dec << ") "


/**
 * \brief Make generating Error Conditions less verbose.
 *
 * All this Macro Function does is expand into code that creates an 
 * std::error_code.
 *
 * \param err_ The error code
 */
#define ZAKERO_INI_SYSTEM_ERROR(err_) std::error_code(err_, std::system_category());

// }}}
// {{{ Anonymous Namespace

namespace
{
}

// }}}
// {{{ Documentation

/**
 * \class zakero::IniErrorCategory
 *
 * \brief Error Categories.
 *
 * This class implements the std::error_category interface to provide 
 * consistent access to error code values and messages.
 *
 * See https://en.cppreference.com/w/cpp/error/error_category for 
 * details.
 */

/**
 * \class zakero::Ini
 * 
 * \brief Interact with INI formatted data.
 * 
 * Use this object to read and write INI files.  INI files are a collection of 
 * string key/value pairs, called *properties*, which are grouped into 
 * *sections*.
 * \examplecode
 * [section 1]
 * key=value
 *
 * [section 2]
 * another_key=another_value
 * \endexamplecode
 */

// }}}
// {{{ Ini
// {{{ Ini: Class Variables

#define X(name_, val_, mesg_) \
const std::error_code Ini::name_(val_, IniErrorCategory);
ZAKERO_INI__ERROR_DATA
#undef X

// }}}

/**
 * \brief Constructor.
 * 
 * Create a new instance of Ini.
 */
Ini::Ini() noexcept
{
}


/**
 * \brief Remove all data.
 *
 * All data will be erased from this object.
 *
 * \examplecode
 * Ini ini;
 *
 * ini.propertyCreate("foo", "bar", "42");
 * ini.clear();  // all gone...
 * \endexamplecode
 */
void Ini::clear() noexcept
{
	data.clear();
}


/**
 * \brief Load data from a file.
 *
 * INI file data can be loaded into this object by using this method.  The file 
 * will overwrite any existing data.
 *
 * \examplecode
 * Ini prefs;
 *
 * // Hard-Coded default values, lots of propertyCreate()
 *
 * prefs.load("/path/to/prefs.ini");
 * \endexamplecode
 *
 * \return An error code.
 */
std::error_code Ini::load(const std::filesystem::path path ///< The file to load.
	) noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(path.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: 'path' can not be empty"
			<< '\n'
			;

		return Error_Invalid_Parameter;
	}

	if(std::filesystem::exists(path) == false)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: 'path' does not exist: "
			<< path
			<< '\n'
			;

		return Error_Invalid_Parameter;
	}

	if(std::filesystem::is_regular_file(path) == false)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
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
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: 'path' is not readable: "
			<< path
			<< '\n'
			;

		return Error_Invalid_Parameter;
	}
#endif // }}}

	VectorUint8 data;

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
			return ZAKERO_INI_SYSTEM_ERROR(error);
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
			return ZAKERO_INI_SYSTEM_ERROR(error);
		}

		fclose(file_handle);

		return Error_File_Is_Bad;
	}

	size_t file_size = file_tell;

	rewind(file_handle);

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


	std::error_code error = parse(data);

	return error;
}


/**
 * \brief Load data from a memory address.
 *
 * INI data can be loaded into this object by using this method. Any duplicate 
 * data found at the memory address will overwrite any existing data.
 *
 * \examplecode
 * uint8_t* addr = RawIniData;
 * size_t   size = RawIniDataSize;
 * Ini      prefs;
 *
 * prefs.load(addr, size);
 * \endexamplecode
 *
 * \return An error code.
 */
std::error_code Ini::load(const uint8_t* addr ///< The starting address of the INI data
	, const size_t                   size ///< The size (or length) of the INI data
	) noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(addr == nullptr)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: 'addr' can not be NULL"
			<< '\n'
			;
	}

	if(size == 0)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: 'size' must be greater than 0"
			<< '\n'
			;
	}
#endif // }}}

	VectorUint8 data(addr, addr + size);

	std::error_code error = parse(data);

	return error;
}


/**
 * \brief Save data to a file.
 *
 * Use this method to write the current data in this object to the file 
 * specified by \p path.
 *
 * \examplecode
 * Ini data;
 *
 * data.propertyCreate("foo", "bar", "42")
 *
 * data.save("/path/to/data.ini");
 * \endexamplecode
 */
void Ini::save(const std::filesystem::path path ///< The file to write to.
	) const noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(path.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
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
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: path is not writable"
			<< '\n'
			;
	}
#endif // }}}

	std::ofstream stream(path);

	save(stream);
}


/**
 * \brief Write data to a stream.
 *
 * This method will write the current data to the specified stream.
 *
 * \examplecode
 * Ini data;
 *
 * data.propertyCreate("foo", "bar", "42")
 *
 * data.save(std::cout);
 * // or
 * std::cout << data;
 * \endexamplecode
 */
void Ini::save(std::ostream& stream ///< The stream to use.
	) const noexcept
{
	bool add_blank_line = false;

	if(data.contains(""))
	{
		const PropertyMap& global_properties = data.at("");

		if(global_properties.empty() == false)
		{
			add_blank_line = true;
		}
		else
		{
			for(auto& property_iter : global_properties)
			{
				stream
					<< property_iter.first
					<< "="
					<< property_iter.second
					<< std::endl;
			}
		}
	}

	for(auto& section_iter : data)
	{
		if(section_iter.first == "")
		{
			continue;
		}

		if(add_blank_line)
		{
			stream << std::endl;
		}

		stream << "["
			<< section_iter.first
			<< "]"
			<< std::endl;

		for(auto& property_iter : section_iter.second)
		{
			stream
				<< property_iter.first
				<< "="
				<< property_iter.second
				<< std::endl;
		}

		add_blank_line = true;
	}
}


/**
 * \brief Create a new section.
 *
 * A new INI Section will be created.  This section will not have any 
 * properties.
 *
 * \see propertyCreate(const std::string&, const std::string&, const std::string&)
 *
 * \see propertyCreate()
 *
 * \examplecode
 * Ini ini;
 *
 * ini.sectionCreate("foo");
 * // Now contains:
 * // [foo]
 * \endexamplecode
 */
void Ini::sectionCreate(const std::string_view section ///< The name of the section.
	) noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(section.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: section can not be empty."
			<< '\n'
			;
	}

	if(sectionExists(section) == true)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Section \"" << section << "\" already exists."
			<< '\n'
			;
	}
#endif // }}}

	data[std::string(section)] = {};
}


/**
 * \brief Destroy a section.
 *
 * Use this method to destroy a section as well as all the propeties in that 
 * section.
 *
 * \examplecode
 * Ini ini;
 *
 * ini.propertyCreate("foo", "A", "1");
 * ini.propertyCreate("foo", "B", "2");
 * // Now contains:
 * // [foo]
 * // A=1
 * // B=2
 *
 * ini.sectionDestroy("foo");
 * // Section "foo" is removed as well as "A=1" and "B=2"
 * \endexamplecode
 */
void Ini::sectionDestroy(const std::string_view section ///< The name of the section.
	) noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(section.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: section can not be empty."
			<< '\n'
			;
	}

	if(sectionExists(section) == false)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Section \"" << section << "\" does not exist."
			<< '\n'
			;
	}
#endif // }}}

	std::string section_name(section);

	data[section_name].clear();
	data.erase(section_name);
}


/**
 * \brief Rename a section.
 *
 * To change the name of a section, use this method.
 *
 * \examplecode
 * Ini ini;
 *
 * ini.propertyCreate("foo", "A", "1");
 * ini.propertyCreate("foo", "B", "2");
 * // Now contains:
 * // [foo]
 * // A=1
 * // B=2
 *
 * ini.sectionRename("foo", "bar");
 * // Now contains:
 * // [bar]
 * // A=1
 * // B=2
 * \endexamplecode
 */
void Ini::sectionRename(const std::string_view old_section ///< The section name.
	, const std::string_view               new_section ///< The section name.
	) noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(old_section.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: old_section can not be empty."
			<< '\n'
			;
	}

	if(sectionExists(old_section) == false)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Section \"" << old_section << "\" does not exist."
			<< '\n'
			;
	}

	if(new_section.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: new_section can not be empty."
			<< '\n'
			;
	}

	if(sectionExists(new_section) == true)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Section \"" << new_section << "\" already exists."
			<< '\n'
			;
	}
#endif // }}}

	std::string new_name(new_section);
	std::string old_name(old_section);

	data[new_name] = std::move(data[old_name]);
	data.erase(old_name);
}


/**
 * \brief Check if a section exists.
 *
 * \examplecode
 * Ini ini;
 *
 * if(!ini.sectionExists("foo"))
 * {
 * 	std::cout << "Dude, Where's my Section!\n";
 * }
 * \endexamplecode
 *
 * \retval true  The section does exist.
 * \retval false The section does **not** exist.
 */
bool Ini::sectionExists(const std::string_view name ///< The section name.
	) const noexcept
{
	return data.contains(std::string(name));
}


/**
 * \brief Get a list of all the sections.
 *
 * This method will provide a list of all the sections currently in this 
 * object.
 *
 * \examplecode
 * Ini ini;
 *
 * ini.propertyCreate("foo", "A", "1");
 * ini.propertyCreate("bar", "B", "2");
 *
 * for(const auto& section : ini.sectionList())
 * {
 * 	std::cout << "Section: " << section << '\n';
 * }
 *
 * // Will print (order may differ):
 * // Section: foo
 * // Section: bar
 * \endexamplecode
 *
 * \return A vector of std::string's
 */
Ini::SectionVector Ini::sectionList() const noexcept
{
	SectionVector vector;
	vector.reserve(data.size());

	for(auto& iter : data)
	{
		vector.push_back(iter.first);
	}

	return vector;
}


/**
 * \brief Get a list of all the sections.
 *
 * This method will provide a list of all the sections currently in this 
 * object.  The returned list will be filtered based on the provided \p regex 
 * value.
 *
 * \examplecode
 * Ini ini;
 *
 * ini.propertyCreate("local_foo" , "A", "1");
 * ini.propertyCreate("local_bar" , "A", "2");
 * ini.propertyCreate("remote_foo", "A", "100");
 * ini.propertyCreate("remote_bar", "A", "200");
 *
 * for(const auto& section : ini.sectionList("local_*"))
 * {
 * 	std::cout << "A: " << ini.property(section, "A") << '\n';
 * }
 *
 * // Will print (Order may differ):
 * // A: 1
 * // A: 2
 * \endexamplecode
 *
 * \return A vector of std::string's
 */
Ini::SectionVector Ini::sectionList(const std::string_view regex ///< The regular expression
	) const noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(regex.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: regex can not be empty."
			<< '\n'
			;
	}
#endif // }}}

	SectionVector vector;
	vector.reserve(data.size());

	const std::string regex_str(regex);
	const std::regex  expr(regex_str);

	for(const auto& iter : data)
	{
		if(std::regex_match(iter.first, expr))
		{
			vector.push_back(iter.first);
		}
	}

	return vector;
}


/**
 * \brief Get a value.
 *
 * This method will get the specified value.
 *
 * \examplecode
 * Ini ini;
 *
 * ini.propertyCreate("foo", "A", "1");
 *
 * std::cout << "Value: " << ini.property("foo", "A") << '\n';
 *
 * // Will print:
 * // Value: 1
 * \endexamplecode
 *
 * \return The value.
 */
std::string Ini::property(const std::string_view section ///< The section name
	, const std::string_view                 name    ///< The property name
	) const noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(section.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: section can not be empty."
			<< '\n'
			;
	}

	if(sectionExists(section) == false)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Section \"" << section << "\" does not exist."
			<< '\n'
			;
	}

	if(name.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: name can not be empty."
			<< '\n'
			;
	}

	if(propertyExists(section, name) == false)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Property \"" << name << "\" in Section \""
			<< section << "\" does not exist."
			<< '\n'
			;
	}
#endif // }}}

	return data
		.at(std::string(section))
		.at(std::string(name))
		;
}


/**
 * \brief Get a value.
 *
 * This method will get the specified value and if that value does not exist, 
 * the provided default value will be returned.
 *
 * \examplecode
 * Ini ini;
 *
 * ini.propertyCreate("foo", "A", "1");
 *
 * std::cout << "Value: " << ini.property("foo", "B", "2") << '\n';
 *
 * // Will print:
 * // Value: 2
 * \endexamplecode
 *
 * \return The value.
 */
std::string Ini::property(const std::string_view section ///< The section name
	, const std::string_view                 name    ///< The property name
	, std::string                            value   ///< The property value
	) const noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(section.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: section can not be empty."
			<< '\n'
			;
	}

	if(sectionExists(section) == false)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Section \"" << section << "\" does not exist."
			<< '\n'
			;
	}

	if(name.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: name can not be empty."
			<< '\n'
			;
	}
#endif // }}}

	if(propertyExists(section, name) == false)
	{
		return value;
	}

	return data
		.at(std::string(section))
		.at(std::string(name))
		;
}


/**
 * \brief Create a new property.
 *
 * This method will create a new property in the requested \p section.
 *
 * \examplecode
 * Ini ini;
 *
 * ini.propertyCreate("foo", "A", "1");
 * \endexamplecode
 */
void Ini::propertyCreate(const std::string_view section ///< The section name
	, const std::string_view                name    ///< The property name
	, const std::string_view                value   ///< The property value
	) noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(section.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: section can not be empty."
			<< '\n'
			;
	}

	if(name.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: name can not be empty."
			<< '\n'
			;
	}

	if(value.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: value can not be empty."
			<< '\n'
			;
	}
#endif // }}}

	if(sectionExists(section) == false)
	{
		sectionCreate(section);
	}

	std::string section_name(section);
	std::string property_name(name);

	data[section_name][property_name] = std::string(value);
}


/**
 * \brief Destroy a property.
 *
 * To remove a property from a section, use this method.
 *
 * \examplecode
 * Ini ini;
 *
 * ini.propertyCreate("foo", "A", "1");
 * ini.propertyDestroy("foo", "A");
 * // The "A" property has been deleted
 * // and the "foo" section remains.
 * \endexamplecode
 */
void Ini::propertyDestroy(const std::string_view section ///< The section name
	, const std::string_view                 name    ///< The property name
	) noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(section.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: section can not be empty."
			<< '\n'
			;
	}

	if(sectionExists(section) == false)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Section \"" << section << "\" does not exist."
			<< '\n'
			;
	}

	if(name.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: name can not be empty."
			<< '\n'
			;
	}

	if(propertyExists(section, name) == false)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Property \"" << name << "\" in Section \""
			<< section << "\" does not exist."
			<< '\n'
			;
	}
#endif // }}}

	data[std::string(section)].erase(std::string(name));
}


/**
 * \brief Check if a property exists.
 *
 * \examplecode
 * Ini ini;
 *
 * if(!ini.propertyExists("foo", "A"))
 * {
 * 	std::cout << "You have no property!\n";
 * }
 * \endexamplecode
 *
 * \retval true  The property does exist.
 * \retval false The property does **not** exist.
 */
bool Ini::propertyExists(const std::string_view section ///< The section name
	, const std::string_view                name    ///< The property name
	) const noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(section.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: name can not be empty."
			<< '\n'
			;
	}

	if(name.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: name can not be empty."
			<< '\n'
			;
	}
#endif // }}}

	if(sectionExists(section) == false)
	{
		return false;
	}

	return data
		.at(std::string(section))
		.contains(std::string(name))
		;
}


/**
 * \brief Get the properties.
 *
 * This method will provide direct access to all of the properties in the 
 * specified \p section.
 *
 * \examplecode
 * Ini ini;
 *
 * ini.sectionCreate("foo");
 * auto& property = ini.propertyMap("foo");
 * property["A"] = "1";
 *
 * // Now contains:
 * // [foo]
 * // A=1
 * \endexamplecode
 *
 * \return A key/value pair map of properties.
 */
Ini::PropertyMap& Ini::propertyMap(const std::string_view section ///< The section name.
	) noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(section.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: section can not be empty."
			<< '\n'
			;
	}

	if(sectionExists(section) == false)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Section \"" << section << "\" does not exist."
			<< '\n'
			;
	}
#endif // }}}

	return data[std::string(section)];
}


/**
 * \brief Get the properties.
 *
 * This method will provide direct access to all of the properties in the 
 * specified \p section.
 *
 * \examplecode
 * Ini ini;
 *
 * ini.sectionCreate("foo");
 *
 * const Ini&  read_only = ini;
 * const auto& property  = read_only.propertyMap("foo");
 * // Do something with the property map.
 * \endexamplecode
 *
 * \return A key/value pair map of properties.
 */
const Ini::PropertyMap& Ini::propertyMap(const std::string_view section ///< The section name
	) const noexcept
{
#if ZAKERO_INI_DEBUG_ENABLED // {{{
	if(section.empty())
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Invalid Parameter: section can not be empty."
			<< '\n'
			;
	}

	if(sectionExists(section) == false)
	{
		ZAKERO_INI_ERROR_STREAM << ZAKERO_INI_
			<< "Section \"" << section << "\" does not exist."
			<< '\n'
			;
	}
#endif // }}}

	return data.at(std::string(section));
}


/**
 * \brief Parse raw INI data
 *
 * The raw character data in the provided \p vector will be parsed and stored 
 * in this object.
 *
 * \bug No error checking is done, assumes properly formatted data.
 *
 * \return An error code
 */
std::error_code Ini::parse(VectorUint8 vector ///< The data to parse
	) noexcept
{
	std::string section = "";

	auto iter = vector.begin();
	while(iter != vector.end())
	{
		while(iter != vector.end() && isspace(*iter))
		{
			++iter;
		}

		if(iter == vector.end())
		{
			break;
		}

		// Comment
		if(*iter == ';')
		{
			while(iter != vector.end() && *iter != '\n')
			{
				++iter;
			}
		}
		// Section
		else if(*iter == '[')
		{
			++iter;
			auto start = iter;

			while(iter != vector.end()
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

			while(iter != vector.end()
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

				while(iter != vector.end()
					&& isspace(*iter)
					&& *iter != '\n'
					)
				{
					++iter;
				}

				start = iter;

				while(iter != vector.end() && *iter != '\n')
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

				propertyCreate(section, name, value);
			}
		}
	}

	return Error_None;
}


/**
 * \fn Ini::Ini(const Ini&)
 *
 * \brief Disable Copy Constructor.
 */


/**
 * \fn Ini& Ini::operator=(const Ini&)
 *
 * \brief Disable Copy Constructor.
 *
 * \return A copy.
 */


/**
 * \fn Ini::Ini(Ini&&)
 *
 * \brief Disable Move Constructor.
 */


/**
 * \fn Ini& Ini::operator=(Ini&&)
 *
 * \brief Disable Move Constructor.
 *
 * \return A copy.
 */

// }}}
}

#endif // ZAKERO_INI_IMPLEMENTATION

// }}}

#endif // Zakero_Ini_h
