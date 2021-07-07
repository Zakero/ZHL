/******************************************************************************
 * Copyright 2021 Andrew Moore
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, value. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef zakero_MessagePack_h
#define zakero_MessagePack_h

/**
 * \file
 *
 * \brief Zakero MessagePack
 *
|     |                              |                              |                              |                             |                             |                                 |
|-----|------------------------------|------------------------------|------------------------------|-----------------------------|-----------------------------|---------------------------------|
| API | \refdeps{zakero_messagepack} | \reftldr{zakero_messagepack} | \refwhat{zakero_messagepack} | \refwhy{zakero_messagepack} | \refhow{zakero_messagepack} | \refversion{zakero_messagepack} |
 * 
 * The _Zakero_MessagePack_ will serialize and deserialize data using the 
 * [MessagePack](https://github.com/msgpack/msgpack/blob/master/spec.md) 
 * specification.
 *
 *
 * \pardeps{zakero_messagepack}
 * - None
 * \endpardeps
 *
 *
 * \partldr{zakero_messagepack}
 * This library makes it very easy to use the MessagePack data format.
 *
 * To use:
 * 1. Add the implementation to a source code file:
 *    \code
 *    #define ZAKERO_MESSAGEPACK_IMPLEMENTATION
 *    #include "Zakero_MessagePack.h"
 *    \endcode
 * 2. Add the library to where it is used:
 *    \code
 *    #include "Zakero_MessagePack.h"
 *    \endcode
 * \endpartldr
 *
 *
 * \parwhat{zakero_messagepack}
 * The _Zakero_MessagePack_ library provides a way to serialize data for 
 * storage or transport over a network. Deserialization is also available so 
 * that the data may be accessed.  The MessagePack specification provides a 
 * format that allows many different types of data to be packed with very 
 * little overhead.
 * \endparwhat
 *
 *
 * \parwhy{zakero_messagepack}
 * There are many libraries available to do the same thing, however the 
 * _Zakero_MessagePack_ library offers the following:
 * 
 * __Benefits__
 * - Single Header Library
 * - Small compile size
 * - Easy to add data
 * - Access to existing data
 * - Modify existing data
 * - Uses native C++ types
 *
 * __Draw Backs__
 * - Memory Usage: Serialization makes a copy of the contents
 *
 * Instead of attempting to interface with other libraries or provide a dynamic 
 * interface, basic C++ types are used. It is expected that this approach will 
 * make using the library with other code bases easier and less 
 * volatile.
 * \endparwhy
 *
 *
 * \parhow{zakero_messagepack}
 * __Step 0__
 *
 * Your compiler must support at least the C++20 standard.  The location of the 
 * `Zakero_*.h` header files _must_ be in your compiler's include path.
 *
 * __Step 1__
 *
 * The first step is to select which C++ source code file will contain the 
 * _Zakero MessagePack_ implementation.  Once the location has been determined, 
 * add the following to that file:
 *
 * ~~~
 * #define ZAKERO_MESSAGEPACK_IMPLEMENTATION
 * #include "Zakero_MessagePack.h"
 * ~~~
 *
 * The macro \ref ZAKERO_MESSAGEPACK_IMPLEMENTATION tells the header file to 
 * include the implementation of the MessagePack.
 *
 * In all other files that will use the MessagePack, they need to include the 
 * header.
 *
 * ~~~
 * #include "Zakero_MessagePack.h"
 * ~~~
 *
 * __Step 2__
 *
 * Create MessagePack [Objects](\ref zakero::messagepack::Object) to store 
 * data.  Use containers to store more objects. Then serialize the data.  
 * MessagePack [Objects](\ref zakero::messagepack::Object)s can also be created 
 * by deserializing data.
 *
 * _Add data manually_
 * ~~~
 * zakero::messagepack::Array array;
 *
 * zakero::messagepack::Object the_answer = Object{uint64_t(42)};
 * array.append(the_answer);
 * array.append(Object{true});
 * array.append({std::string("Hello, World!")});
 * ~~~
 *
 * _Deserialize data_
 * ~~~
 * std::vector<uint8_t> data = load_data();
 *
 * zakero::messagepack::Object object = zakero::messagepack::deserialize(data);
 * ~~~
 *
 * The data in the MessagePack can be modified:
 * ~~~
 * if(object.isArray())
 * {
 * 	zakero::messagepack::Array& array = object.asArray();
 * 	array.object(2) = {std::string("Good Bye!")};
 * }
 * ~~~
 *
 * Then the MessagePack can be (re)serialized:
 * ~~~
 * data = zakero::messagepack::serialize(object);
 * ~~~
 * \endparhow
 *
 *
 * \parversion{zakero_messagepack}
 * __v0.3.0__
 * - Added support for Ext
 * - Added support for Maps
 * - Added support for the Timestamp extension
 * - Complete rewrite
 *
 * __v0.2.0__
 * - Added support for Arrays
 *
 * __v0.1.0__
 * - The initial implementation
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
 * \todo Add error code support.
 */


/******************************************************************************
 * Includes
 */

// C++
#include <cstring>
#include <ctime>
#include <limits>
#include <string>
#include <variant>
#include <vector>

// POSIX
#include <arpa/inet.h>

// Linux


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
 *    The error name will be accessible as zakero::MessagePack::_ErrorName_
 *    <br><br>
 * -# __ErrorValue__<br>
 *    The integer value of the error and will be available as `int` values in 
 *    the zakero::MessagePack class.
 *    <br><br>
 * -# __ErrorMessage__<br>
 *    The text that will be used by `std::error_code.message()`
 */
#define ZAKERO_MESSAGEPACK__ERROR_DATA \
	X(Error_None    ,  0 , "No Error"                                                        ) \
	X(Error_Unknown ,  1 , "An unknown error has occurred"                                   ) \

// }}}


/******************************************************************************
 * Objects
 */

namespace zakero::messagepack
{
		struct Array;
		struct Ext;
		struct Map;
		struct Object;

		// {{{ Array

		struct Array
		{
				      size_t               append(const bool) noexcept;
				      size_t               append(const int64_t) noexcept;
				      size_t               append(const uint64_t) noexcept;
				      size_t               append(const float) noexcept;
				      size_t               append(const double) noexcept;
				      size_t               append(const std::string_view) noexcept;
				      size_t               append(const std::vector<uint8_t>&) noexcept;
				      size_t               append(std::vector<uint8_t>&) noexcept;
				      size_t               append(const Array&) noexcept;
				      size_t               append(Array&) noexcept;
				      size_t               append(const Ext&) noexcept;
				      size_t               append(Ext&) noexcept;
				      size_t               append(const Map&) noexcept;
				      size_t               append(Map&) noexcept;
				      size_t               append(const Object&) noexcept;
				      size_t               append(Object&) noexcept;
				      size_t               appendNull() noexcept;

			[[nodiscard]] inline Object&       object(const size_t index) noexcept { return object_vector[index]; }
			[[nodiscard]] inline const Object& object(const size_t index) const noexcept { return object_vector[index]; }

				      inline void          clear() noexcept { return object_vector.clear(); };
			[[nodiscard]] inline size_t        size() const noexcept { return object_vector.size(); };

			std::vector<Object> object_vector = {};
		};

		// }}} Array
		// {{{ Ext

		struct Ext
		{
			std::vector<uint8_t> data = {};
			int8_t               type = 0;
		};

		// }}} Ext
		// {{{ Map

		struct Map
		{
			              size_t               set(Object&, Object&) noexcept;
			              size_t               set(const Object&, const Object&) noexcept;
			[[nodiscard]] inline const Object& keyAt(const size_t index) const noexcept { return object_key[index]; };
			[[nodiscard]] inline Object&       keyAt(const size_t index) noexcept { return object_key[index]; };
			[[nodiscard]] bool                 keyExists(const Object&) const noexcept;
			[[nodiscard]] size_t               keyIndex(const Object&) const noexcept;
			[[nodiscard]] inline const Object& valueAt(const size_t index) const noexcept { return object_value[index]; };
			[[nodiscard]] inline Object&       valueAt(const size_t index) noexcept { return object_value[index]; };
			[[nodiscard]] Object&              valueOf(Object&) noexcept;
			[[nodiscard]] const Object&        valueOf(const Object&) const noexcept;

			              void                 erase(const Object&) noexcept;
			              inline void          clear() noexcept { object_key.clear(); object_value.clear(); };
			[[nodiscard]] inline size_t        size() const noexcept { return object_key.size(); };

			std::vector<Object> object_key   = {};
			std::vector<Object> object_value = {};
		};

		// }}} Map
		// {{{ Object

		struct Object
		{
			std::variant<std::monostate
				, bool
				, int64_t
				, uint64_t
				, float
				, double
				, std::string
				, std::vector<uint8_t>
				, zakero::messagepack::Array
				, zakero::messagepack::Ext
				, zakero::messagepack::Map
				> value;

			template<typename T> [[nodiscard]] T&                          as() noexcept { return std::get<T>(value); };
			template<typename T> [[nodiscard]] const T&                    as() const noexcept { return std::get<T>(value); };
			                     [[nodiscard]] messagepack::Array&         asArray() noexcept { return std::get<messagepack::Array>(value); };
			                     [[nodiscard]] const messagepack::Array&   asArray() const noexcept { return std::get<messagepack::Array>(value); };
			                     [[nodiscard]] messagepack::Ext&           asExt() noexcept { return std::get<messagepack::Ext>(value); };
			                     [[nodiscard]] const messagepack::Ext&     asExt() const noexcept { return std::get<messagepack::Ext>(value); };
			                     [[nodiscard]] messagepack::Map&           asMap() noexcept { return std::get<messagepack::Map>(value); };
			                     [[nodiscard]] const messagepack::Map&     asMap() const noexcept { return std::get<messagepack::Map>(value); };
			                     [[nodiscard]] std::vector<uint8_t>&       asBinary() noexcept { return std::get<std::vector<uint8_t>>(value); };
			                     [[nodiscard]] const std::vector<uint8_t>& asBinary() const noexcept { return std::get<std::vector<uint8_t>>(value); };
			                     [[nodiscard]] const std::string&          asString() const noexcept { return std::get<std::string>(value); };

			template<typename T> [[nodiscard]] constexpr bool              is() const noexcept { return std::holds_alternative<T>(value); };
			                     [[nodiscard]] constexpr bool              isArray() const noexcept { return std::holds_alternative<messagepack::Array>(value); };
			                     [[nodiscard]] constexpr bool              isBinary() const noexcept { return std::holds_alternative<std::vector<uint8_t>>(value); };
			                     [[nodiscard]] constexpr bool              isExt() const noexcept { return std::holds_alternative<messagepack::Ext>(value); };
			                     [[nodiscard]] constexpr bool              isMap() const noexcept { return std::holds_alternative<messagepack::Map>(value); };
			                     [[nodiscard]] constexpr bool              isNull() const noexcept { return std::holds_alternative<std::monostate>(value); };
			                     [[nodiscard]] constexpr bool              isString() const noexcept { return std::holds_alternative<std::string>(value); };
		};

		// }}} Object
		// {{{ Extensions

		[[nodiscard]] bool            extensionTimestampCheck(const Object&) noexcept;
		[[nodiscard]] struct timespec extensionTimestampConvert(const Object&) noexcept;
		[[nodiscard]] Object          extensionTimestampConvert(const struct timespec&) noexcept;

		// }}} Extensions
		// {{{ Utilities

		[[nodiscard]] Object               deserialize(const std::vector<uint8_t>&) noexcept;
		[[nodiscard]] Object               deserialize(const std::vector<uint8_t>&, size_t&) noexcept;
		[[nodiscard]] std::vector<uint8_t> serialize(const messagepack::Array&) noexcept;
		[[nodiscard]] std::vector<uint8_t> serialize(const messagepack::Ext&) noexcept;
		[[nodiscard]] std::vector<uint8_t> serialize(const messagepack::Map&) noexcept;
		[[nodiscard]] std::vector<uint8_t> serialize(const messagepack::Object&) noexcept;
		[[nodiscard]] std::string          to_string(const messagepack::Array&) noexcept;
		[[nodiscard]] std::string          to_string(const messagepack::Ext&) noexcept;
		[[nodiscard]] std::string          to_string(const messagepack::Map&) noexcept;
		[[nodiscard]] std::string          to_string(const messagepack::Object&) noexcept;

		// }}} Utilities
} // zakero::messagepack

// {{{ Operators

std::ostream& operator<<(std::ostream&, const zakero::messagepack::Array&) noexcept;
std::ostream& operator<<(std::ostream&, const zakero::messagepack::Ext&) noexcept;
std::ostream& operator<<(std::ostream&, const zakero::messagepack::Map&) noexcept;
std::ostream& operator<<(std::ostream&, const zakero::messagepack::Object&) noexcept;

bool operator==(const zakero::messagepack::Object& lhs, const zakero::messagepack::Object& rhs) noexcept;

// }}}
// {{{ Implementation

#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION

// {{{ Macros
// {{{ Macros : Doxygen

#ifdef ZAKERO__DOXYGEN_DEFINE_DOCS

// Only used for generating Doxygen documentation

/**
 * \brief Activate the implementation code.
 *
 * Defining this macro will cause the zakero::MessagePack implementation to be 
 * included.  This should only be done once, since compiler and/or linker 
 * errors will typically be generated if more than a single implementation is 
 * found.
 *
 * \note It does not matter if the macro is given a value or not, only its 
 * existence is checked.
 */
#define ZAKERO_MESSAGEPACK_IMPLEMENTATION

#endif

// }}}

/**
 * \internal
 *
 * \brief Format Data
 *
 * This X-Macro table contains:
 * - The internal format type name
 * - The type id
 * - A type id mask (the complement is used to get the value)
 * - The type spec name
 */
#define ZAKERO_MESSAGEPACK__FORMAT_TYPE \
	/* Format Name    Format Id   Format Mask   Format Name      */ \
	X(Fixed_Int_Pos , 0x00      , 0b10000000  , "positive fixint" ) \
	X(Fixed_Map     , 0x80      , 0b11110000  , "fixmap"          ) \
	X(Fixed_Array   , 0x90      , 0b11110000  , "fixarray"        ) \
	X(Fixed_Str     , 0xa0      , 0b11100000  , "fixstr"          ) \
	X(Nill          , 0xc0      , 0b11111111  , "nill"            ) \
	X(never_used    , 0xc1      , 0b00000000  , "(never used)"    ) \
	X(False         , 0xc2      , 0b11111111  , "false"           ) \
	X(True          , 0xc3      , 0b11111111  , "true"            ) \
	X(Bin8          , 0xc4      , 0b11111111  , "bin 8"           ) \
	X(Bin16         , 0xc5      , 0b11111111  , "bin 16"          ) \
	X(Bin32         , 0xc6      , 0b11111111  , "bin 32"          ) \
	X(Ext8          , 0xc7      , 0b11111111  , "ext 8"           ) \
	X(Ext16         , 0xc8      , 0b11111111  , "ext 16"          ) \
	X(Ext32         , 0xc9      , 0b11111111  , "ext 32"          ) \
	X(Float32       , 0xca      , 0b11111111  , "float 32"        ) \
	X(Float64       , 0xcb      , 0b11111111  , "float 64"        ) \
	X(Uint8         , 0xcc      , 0b11111111  , "uint 8"          ) \
	X(Uint16        , 0xcd      , 0b11111111  , "uint 16"         ) \
	X(Uint32        , 0xce      , 0b11111111  , "uint 32"         ) \
	X(Uint64        , 0xcf      , 0b11111111  , "uint 64"         ) \
	X(Int8          , 0xd0      , 0b11111111  , "int 8"           ) \
	X(Int16         , 0xd1      , 0b11111111  , "int 16"          ) \
	X(Int32         , 0xd2      , 0b11111111  , "int 32"          ) \
	X(Int64         , 0xd3      , 0b11111111  , "int 64"          ) \
	X(Fixed_Ext1    , 0xd4      , 0b11111111  , "fixext 1"        ) \
	X(Fixed_Ext2    , 0xd5      , 0b11111111  , "fixext 2"        ) \
	X(Fixed_Ext4    , 0xd6      , 0b11111111  , "fixext 4"        ) \
	X(Fixed_Ext8    , 0xd7      , 0b11111111  , "fixext 8"        ) \
	X(Fixed_Ext16   , 0xd8      , 0b11111111  , "fixext 16"       ) \
	X(Str8          , 0xd9      , 0b11111111  , "str 8"           ) \
	X(Str16         , 0xda      , 0b11111111  , "str 16"          ) \
	X(Str32         , 0xdb      , 0b11111111  , "str 32"          ) \
	X(Array16       , 0xdc      , 0b11111111  , "array 16"        ) \
	X(Array32       , 0xdd      , 0b11111111  , "array 32"        ) \
	X(Map16         , 0xde      , 0b11111111  , "map 16"          ) \
	X(Map32         , 0xdf      , 0b11111111  , "map 32"          ) \
	X(Fixed_Int_Neg , 0xe0      , 0b11100000  , "negative fixint" ) \

// }}}
// {{{ Documentation


// }}}

namespace zakero::messagepack
{
// {{{ Anonymous Namespace

namespace
{
	/**
	 * \brief MessagePack Format IDs.
	 */
	enum class Format : uint8_t
	{
#define X(type_, id_, mask_, text_) \
		type_ = id_, \

		ZAKERO_MESSAGEPACK__FORMAT_TYPE
#undef X
	};


	/**
	 * \brief Format ID names.
	 *
	 * Useful for debugging.
	std::map<uint8_t, std::string> Format_Name =
	{
	#define X(format_, id_, mask_, name_) \
		{ uint8_t(id_ & mask_), std::string(name_) },

		ZAKERO_MESSAGEPACK__FORMAT_TYPE
	#undef X
	};
	 */


	/**
	 * \name Format ID Masks
	 * \{
	 */
#define X(type_, id_, mask_, text_) \
	constexpr uint8_t type_ ## _Mask  = mask_;  \

	ZAKERO_MESSAGEPACK__FORMAT_TYPE
#undef X
	/**
	 * \}
	 */


	/**
	 * \name Format Value Masks
	 * \{
	 */
#define X(type_, id_, mask_, text_) \
	constexpr uint8_t type_ ## _Value = (uint8_t)~mask_;\

	ZAKERO_MESSAGEPACK__FORMAT_TYPE
#undef X
	/**
	 * \}
	 */


	/**
	 * \brief In-Place Byte Conversion.
	 */
	union
	{
		uint64_t uint64;
		uint32_t uint32;
		uint16_t uint16;
		uint8_t  uint8;
		int64_t  int64;
		int32_t  int32;
		int16_t  int16;
		int8_t   int8;
		float    float32;
		double   float64;
		uint8_t  uint8_[8];
	} Convert;


	/**
	 * \name Endian Conversion
	 * \{
	 */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	uint8_t& Convert_Byte0 = Convert.uint8_[7];
	uint8_t& Convert_Byte1 = Convert.uint8_[6];
	uint8_t& Convert_Byte2 = Convert.uint8_[5];
	uint8_t& Convert_Byte3 = Convert.uint8_[4];
	uint8_t& Convert_Byte4 = Convert.uint8_[3];
	uint8_t& Convert_Byte5 = Convert.uint8_[2];
	uint8_t& Convert_Byte6 = Convert.uint8_[1];
	uint8_t& Convert_Byte7 = Convert.uint8_[0];
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	uint8_t& Convert_Byte0 = Convert.uint8_[0];
	uint8_t& Convert_Byte1 = Convert.uint8_[1];
	uint8_t& Convert_Byte2 = Convert.uint8_[2];
	uint8_t& Convert_Byte3 = Convert.uint8_[3];
	uint8_t& Convert_Byte4 = Convert.uint8_[4];
	uint8_t& Convert_Byte5 = Convert.uint8_[5];
	uint8_t& Convert_Byte6 = Convert.uint8_[6];
	uint8_t& Convert_Byte7 = Convert.uint8_[7];
#endif
	/**
	 * \}
	 */


	void serialize_(const messagepack::Array&, std::vector<uint8_t>&) noexcept;
	void serialize_(const messagepack::Ext&, std::vector<uint8_t>&) noexcept;
	void serialize_(const messagepack::Map&, std::vector<uint8_t>&) noexcept;

	void serialize_(const messagepack::Object& object
		, std::vector<uint8_t>&            vector
		) noexcept
	{
		if(object.isNull())
		{
			vector.push_back((uint8_t)Format::Nill);
		}
		else if(object.is<bool>())
		{
			vector.push_back(object.as<bool>()
				? (uint8_t)Format::True
				: (uint8_t)Format::False
				);
		}
		else if(object.is<int64_t>())
		{
			const int64_t value = object.as<int64_t>();

			if(value < 0)
			{
				if(value >= -32)
				{
					vector.push_back((uint8_t)Format::Fixed_Int_Neg
						| (uint8_t)(value & Fixed_Int_Neg_Value)
						);
				}
				else if(value >= std::numeric_limits<int8_t>::min())
				{
					vector.reserve(vector.size() + 2);

					vector.push_back((uint8_t)Format::Int8);
					vector.push_back((int8_t)value);
				}
				else if(value >= std::numeric_limits<int16_t>::min())
				{
					vector.reserve(vector.size() + 3);

					vector.push_back((uint8_t)Format::Int16);

					Convert.int16 = (int16_t)value;
					vector.push_back(Convert_Byte1);
					vector.push_back(Convert_Byte0);
				}
				else if(value >= std::numeric_limits<int32_t>::min())
				{
					vector.reserve(vector.size() + 5);

					vector.push_back((uint8_t)Format::Int32);

					Convert.int32 = (int32_t)value;
					vector.push_back(Convert_Byte3);
					vector.push_back(Convert_Byte2);
					vector.push_back(Convert_Byte1);
					vector.push_back(Convert_Byte0);
				}
				else if(value >= std::numeric_limits<int64_t>::min())
				{
					vector.reserve(vector.size() + 9);

					vector.push_back((uint8_t)Format::Int64);

					Convert.int64 = (int64_t)value;
					vector.push_back(Convert_Byte7);
					vector.push_back(Convert_Byte6);
					vector.push_back(Convert_Byte5);
					vector.push_back(Convert_Byte4);
					vector.push_back(Convert_Byte3);
					vector.push_back(Convert_Byte2);
					vector.push_back(Convert_Byte1);
					vector.push_back(Convert_Byte0);
				}
			}
			else
			{
				if(value <= std::numeric_limits<int8_t>::max())
				{
					vector.push_back((int8_t)value);
				}
				else if(value <= std::numeric_limits<int16_t>::max())
				{
					vector.reserve(vector.size() + 3);

					vector.push_back((uint8_t)Format::Int16);

					Convert.int16 = (int16_t)value;
					vector.push_back(Convert_Byte1);
					vector.push_back(Convert_Byte0);
				}
				else if(value <= std::numeric_limits<int32_t>::max())
				{
					vector.reserve(vector.size() + 5);

					vector.push_back((uint8_t)Format::Int32);

					Convert.int32 = (int32_t)value;
					vector.push_back(Convert_Byte3);
					vector.push_back(Convert_Byte2);
					vector.push_back(Convert_Byte1);
					vector.push_back(Convert_Byte0);
				}
				else if(value <= std::numeric_limits<int64_t>::max())
				{
					vector.reserve(vector.size() + 9);

					vector.push_back((uint8_t)Format::Int64);

					Convert.int64 = (int64_t)value;
					vector.push_back(Convert_Byte7);
					vector.push_back(Convert_Byte6);
					vector.push_back(Convert_Byte5);
					vector.push_back(Convert_Byte4);
					vector.push_back(Convert_Byte3);
					vector.push_back(Convert_Byte2);
					vector.push_back(Convert_Byte1);
					vector.push_back(Convert_Byte0);
				}
			}
		}
		else if(object.is<uint64_t>())
		{
			const uint64_t value = object.as<uint64_t>();

			if(value <= std::numeric_limits<uint8_t>::max())
			{
				vector.reserve(vector.size() + 2);

				vector.push_back((uint8_t)Format::Uint8);
				vector.push_back((uint8_t)value);
			}
			else if(value <= std::numeric_limits<uint16_t>::max())
			{
				vector.reserve(vector.size() + 3);

				vector.push_back((uint8_t)Format::Uint16);

				Convert.uint16 = (uint16_t)value;
				vector.push_back(Convert_Byte1);
				vector.push_back(Convert_Byte0);
			}
			else if(value <= std::numeric_limits<uint32_t>::max())
			{
				vector.reserve(vector.size() + 5);

				vector.push_back((uint8_t)Format::Uint32);

				Convert.uint32 = (uint32_t)value;
				vector.push_back(Convert_Byte3);
				vector.push_back(Convert_Byte2);
				vector.push_back(Convert_Byte1);
				vector.push_back(Convert_Byte0);
			}
			else
			{
				vector.reserve(vector.size() + 9);

				vector.push_back((uint8_t)Format::Uint64);

				Convert.uint64 = value;
				vector.push_back(Convert_Byte7);
				vector.push_back(Convert_Byte6);
				vector.push_back(Convert_Byte5);
				vector.push_back(Convert_Byte4);
				vector.push_back(Convert_Byte3);
				vector.push_back(Convert_Byte2);
				vector.push_back(Convert_Byte1);
				vector.push_back(Convert_Byte0);
			}
		}
		else if(object.is<float>())
		{
			float value = object.as<float>();

			vector.reserve(vector.size() + 5);

			vector.push_back((uint8_t)Format::Float32);

			Convert.float32 = value;
			vector.push_back(Convert_Byte3);
			vector.push_back(Convert_Byte2);
			vector.push_back(Convert_Byte1);
			vector.push_back(Convert_Byte0);
		}
		else if(object.is<double>())
		{
			double value = object.as<double>();

			vector.push_back((uint8_t)Format::Float64);

			Convert.float64 = value;
			vector.push_back(Convert_Byte7);
			vector.push_back(Convert_Byte6);
			vector.push_back(Convert_Byte5);
			vector.push_back(Convert_Byte4);
			vector.push_back(Convert_Byte3);
			vector.push_back(Convert_Byte2);
			vector.push_back(Convert_Byte1);
			vector.push_back(Convert_Byte0);
		}
		else if(object.isString())
		{
			const std::string& value = object.asString();

			const size_t string_length = value.size();

			if(string_length <= 31)
			{
				vector.reserve(string_length + 1);

				vector.push_back((uint8_t)Format::Fixed_Str
					| (uint8_t)string_length
					);

				for(const auto& c : value)
				{
					vector.push_back((uint8_t)c);
				}
			}
			else if(string_length <= std::numeric_limits<uint8_t>::max())
			{
				vector.reserve(string_length + 2);

				vector.push_back((uint8_t)Format::Str8);
				vector.push_back((uint8_t)string_length);

				for(const auto& c : value)
				{
					vector.push_back((uint8_t)c);
				}
			}
			else if(string_length <= std::numeric_limits<uint16_t>::max())
			{
				vector.reserve(string_length + 3);

				vector.push_back((uint8_t)Format::Str16);

				Convert.uint16 = (uint16_t)string_length;
				vector.push_back(Convert_Byte1);
				vector.push_back(Convert_Byte0);

				for(const auto& c : value)
				{
					vector.push_back((uint8_t)c);
				}
			}
			else if(string_length <= std::numeric_limits<uint32_t>::max())
			{
				vector.reserve(string_length + 5);

				vector.push_back((uint8_t)Format::Str32);

				Convert.uint32 = (uint32_t)string_length;
				vector.push_back(Convert_Byte3);
				vector.push_back(Convert_Byte2);
				vector.push_back(Convert_Byte1);
				vector.push_back(Convert_Byte0);

				for(const auto& c : value)
				{
					vector.push_back((uint8_t)c);
				}
			}
		}
		else if(object.isBinary())
		{
			const std::vector<uint8_t>& value = object.asBinary();

			const size_t vector_length = value.size();

			if(vector_length <= std::numeric_limits<uint8_t>::max())
			{
				vector.reserve(vector_length + 2);

				vector.push_back((uint8_t)Format::Bin8);
				vector.push_back((uint8_t)vector_length);

				vector.insert(vector.end()
					, value.begin()
					, value.end()
					);
			}
			else if(vector_length <= std::numeric_limits<uint16_t>::max())
			{
				vector.reserve(vector_length + 3);

				vector.push_back((uint8_t)Format::Bin16);

				Convert.uint16 = (uint16_t)vector_length;
				vector.push_back(Convert_Byte1);
				vector.push_back(Convert_Byte0);

				vector.insert(vector.end()
					, value.begin()
					, value.end()
					);
			}
			else if(vector_length <= std::numeric_limits<uint32_t>::max())
			{
				vector.reserve(vector_length + 5);

				vector.push_back((uint8_t)Format::Bin32);

				Convert.uint32 = (uint32_t)vector_length;
				vector.push_back(Convert_Byte3);
				vector.push_back(Convert_Byte2);
				vector.push_back(Convert_Byte1);
				vector.push_back(Convert_Byte0);

				vector.insert(vector.end()
					, value.begin()
					, value.end()
					);
			}
		}
		else if(object.isArray())
		{
			const messagepack::Array& array = object.asArray();

			serialize_(array, vector);
		}
		else if(object.isExt())
		{
			const messagepack::Ext& ext = object.asExt();

			serialize_(ext, vector);
		}
		else if(object.isMap())
		{
			const messagepack::Map& map = object.asMap();

			serialize_(map, vector);
		}
	}


	void serialize_(const messagepack::Array& array
		, std::vector<uint8_t>&           vector
		) noexcept
	{
		const size_t array_size = array.size();

		if(array_size < 16)
		{
			vector.push_back((uint8_t)Format::Fixed_Array
				| (uint8_t)array_size
				);
		}
		else if(array_size <= std::numeric_limits<uint16_t>::max())
		{
			vector.push_back((uint8_t)Format::Array16);

			Convert.uint16 = (uint16_t)array_size;
			vector.push_back(Convert_Byte1);
			vector.push_back(Convert_Byte0);
		}
		else if(array_size <= std::numeric_limits<uint32_t>::max())
		{
			vector.push_back((uint8_t)Format::Array32);

			Convert.uint32 = (uint32_t)array_size;
			vector.push_back(Convert_Byte3);
			vector.push_back(Convert_Byte2);
			vector.push_back(Convert_Byte1);
			vector.push_back(Convert_Byte0);
		}
		else
		{
			/**
			 * \todo ERROR
			 */
		}
			
		/**
		 * \todo Add `vector.reserve()` support. May need to 
		 * add `Object::size()`.
		 */
		for(const messagepack::Object& object : array.object_vector)
		{
			serialize_(object, vector);
		}
	}


	void serialize_(const messagepack::Ext& ext
		, std::vector<uint8_t>&         vector
		) noexcept
	{
		const size_t data_size = ext.data.size();

		if(data_size == 1)
		{
			vector.push_back((uint8_t)Format::Fixed_Ext1);
		}
		else if(data_size == 2)
		{
			vector.push_back((uint8_t)Format::Fixed_Ext2);
		}
		else if(data_size == 4)
		{
			vector.push_back((uint8_t)Format::Fixed_Ext4);
		}
		else if(data_size == 8)
		{
			vector.push_back((uint8_t)Format::Fixed_Ext8);
		}
		else if(data_size == 16)
		{
			vector.push_back((uint8_t)Format::Fixed_Ext16);
		}
		else if(data_size == 0
			|| data_size <= std::numeric_limits<uint8_t>::max()
			)
		{
			vector.push_back((uint8_t)Format::Ext8);
			vector.push_back((uint8_t)data_size);
		}
		else if(data_size <= std::numeric_limits<uint16_t>::max())
		{
			vector.push_back((uint8_t)Format::Ext16);

			Convert.uint64 = data_size;
			vector.push_back(Convert_Byte1);
			vector.push_back(Convert_Byte0);
		}
		else if(data_size <= std::numeric_limits<uint32_t>::max())
		{
			vector.push_back((uint8_t)Format::Ext32);

			Convert.uint64 = data_size;
			vector.push_back(Convert_Byte3);
			vector.push_back(Convert_Byte2);
			vector.push_back(Convert_Byte1);
			vector.push_back(Convert_Byte0);
		}

		Convert.uint64 = 0;
		Convert.int8   = ext.type;
		vector.push_back(Convert.uint8);

		if(data_size == 0)
		{
			vector.reserve(vector.size() + 1);
			vector.push_back(0);
		}
		else
		{
			size_t index = vector.size();
			vector.resize(vector.size() + data_size);
			memcpy((void*)(vector.data() + index), (void*)ext.data.data(), data_size);
		}
	}


	void serialize_(const messagepack::Map& map
		, std::vector<uint8_t>&         vector
		) noexcept
	{
		const size_t map_size = map.size();

		if(map_size < 16)
		{
			vector.push_back((uint8_t)Format::Fixed_Map
				| (uint8_t)map_size
				);
		}
		else if(map_size <= std::numeric_limits<uint16_t>::max())
		{
			vector.push_back((uint8_t)Format::Map16);

			Convert.uint16 = (uint16_t)map_size;
			vector.push_back(Convert_Byte1);
			vector.push_back(Convert_Byte0);
		}
		else if(map_size <= std::numeric_limits<uint32_t>::max())
		{
			vector.push_back((uint8_t)Format::Map32);

			Convert.uint32 = (uint32_t)map_size;
			vector.push_back(Convert_Byte3);
			vector.push_back(Convert_Byte2);
			vector.push_back(Convert_Byte1);
			vector.push_back(Convert_Byte0);
		}
		else
		{
			/**
			 * \todo ERROR
			 */
		}
			
		/**
		 * \todo Add `vector.reserve()` support. May need to 
		 * add `Object::size()`.
		 */
		for(size_t i = 0; i < map.object_key.size(); i++)
		{
			serialize_(map.object_key[i]  , vector);
			serialize_(map.object_value[i], vector);
		}
	}
}

// }}}
// {{{ Array

/**
 * \struct Array
 *
 * \brief An array of Objects.
 *
 * The role of this structure is to store a collection of Objects in a 
 * resizable array. The underling `object_vector` can be accessed directly or 
 * use the helper methods to increase code readability.
 *
 * Objects can be tested to find out if they are Arrays by using 
 * Object::isArray() and converted into an Array with Object::asArray().  An 
 * Array can not be converted into an Object. However, an Object can be 
 * constructed using an Array and the utility methods can operate on Arrays as 
 * if they were Objects.
 */


/**
 * \var zakero::messagepack::Array::object_vector
 *
 * \brief Store the Object data.
 *
 * The `object_vector` is used to store all the Qbjects in the Array.
 */


/**
 * \brief Append a boolean value.
 *
 * The \p value will be appended to the contents of the Array.  
 *
 * \parcode
 * zakero::messagepack::Array array;
 * array.append(true);
 * \endparcode
 *
 * \return The index location of where the \p value was stored.
 */
size_t Array::append(const bool value ///< The value to add
	) noexcept
{
	const size_t index = object_vector.size();

	object_vector.emplace_back(Object{value});

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("array/append/bool")
{
	Array array;

	size_t index = array.append(true);

	CHECK(index == 0);
	CHECK(array.size() == 1);

	index = array.append(false);

	CHECK(index == 1);
	CHECK(array.size() == 2);

	// Check serialized data

	std::vector<uint8_t> data = serialize(array);
	CHECK(data.size() == 3);

	index = 0;
	CHECK((data[index] & Fixed_Array_Mask) == (uint8_t)Format::Fixed_Array);
	CHECK((data[index++] & Fixed_Array_Value) == 2);
	CHECK(data[index++] == (uint8_t)Format::True);
	CHECK(data[index++] == (uint8_t)Format::False);

	// Check deserialized data

	Object object = deserialize(data);

	CHECK(object.isArray());
	Array& test = object.asArray();
	CHECK(test.size() == 2);

	{
		const messagepack::Object& object = array.object(0);
		CHECK(object.is<bool>());
		CHECK(object.as<bool>() == true);
	}

	{
		const messagepack::Object& object = array.object(1);
		CHECK(object.is<bool>());
		CHECK(object.as<bool>() == false);
	}
}
#endif // }}}


/**
 * \brief Append a signed integer value.
 *
 * The \p value will be appended to the contents of the Array.
 *
 * \parcode
 * zakero::messagepack::Array array;
 * array.append(int64_t(0));
 * \endparcode
 *
 * \return The index location of where the \p value was stored.
 */
size_t Array::append(const int64_t value ///< The value to add
	) noexcept
{
	const size_t index = object_vector.size();

	object_vector.emplace_back(Object{value});

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("array/append/int64_t")
{
	const int64_t i8_min = -31;
	const int64_t i8_max = 127;
	const int64_t i16_min = std::numeric_limits<int16_t>::min();
	const int64_t i16_max = std::numeric_limits<int16_t>::max();
	const int64_t i32_min = std::numeric_limits<int32_t>::min();
	const int64_t i32_max = std::numeric_limits<int32_t>::max();
	const int64_t i64_min = std::numeric_limits<int64_t>::min();
	const int64_t i64_max = std::numeric_limits<int64_t>::max();
	size_t count = 0;

	Array array;
	array.append(i8_min);  count++;
	array.append(i8_max);  count++;
	array.append(i16_min); count++;
	array.append(i16_max); count++;
	array.append(i32_min); count++;
	array.append(i32_max); count++;
	array.append(i64_min); count++;
	array.append(i64_max); count++;

	CHECK(array.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = serialize(array);

	// Check deserialized data

	Object object = deserialize(data);

	CHECK(object.isArray());
	Array& test = object.asArray();
	CHECK(test.size() == count);

	size_t index = 0;
	CHECK(test.object(index).is<int64_t>());
	CHECK(test.object(index).as<int64_t>() == i8_min);

	index++;
	CHECK(test.object(index).is<int64_t>());
	CHECK(test.object(index).as<int64_t>() == i8_max);

	index++;
	CHECK(test.object(index).is<int64_t>());
	CHECK(test.object(index).as<int64_t>() == i16_min);

	index++;
	CHECK(test.object(index).is<int64_t>());
	CHECK(test.object(index).as<int64_t>() == i16_max);

	index++;
	CHECK(test.object(index).is<int64_t>());
	CHECK(test.object(index).as<int64_t>() == i32_min);

	index++;
	CHECK(test.object(index).is<int64_t>());
	CHECK(test.object(index).as<int64_t>() == i32_max);

	index++;
	CHECK(test.object(index).is<int64_t>());
	CHECK(test.object(index).as<int64_t>() == i64_min);

	index++;
	CHECK(test.object(index).is<int64_t>());
	CHECK(test.object(index).as<int64_t>() == i64_max);
}
#endif // }}}


/**
 * \brief Append an unsigned integer value.
 *
 * The \p value will be appended to the contents of the Array.
 *
 * \parcode
 * zakero::messagepack::Array array;
 * array.append(uint64_t(0));
 * \endparcode
 *
 * \return The index location of where the \p value was stored.
 */
size_t Array::append(const uint64_t value ///< The value to add
	) noexcept
{
	const size_t index = object_vector.size();

	object_vector.emplace_back(Object{value});

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("array/append/uint64_t")
{
	const uint64_t u8_min = -31;
	const uint64_t u8_max = 127;
	const uint64_t u16_min = std::numeric_limits<uint16_t>::min();
	const uint64_t u16_max = std::numeric_limits<uint16_t>::max();
	const uint64_t u32_min = std::numeric_limits<uint32_t>::min();
	const uint64_t u32_max = std::numeric_limits<uint32_t>::max();
	const uint64_t u64_min = std::numeric_limits<uint64_t>::min();
	const uint64_t u64_max = std::numeric_limits<uint64_t>::max();
	size_t count = 0;

	Array array;
	array.append(u8_min);  count++;
	array.append(u8_max);  count++;
	array.append(u16_min); count++;
	array.append(u16_max); count++;
	array.append(u32_min); count++;
	array.append(u32_max); count++;
	array.append(u64_min); count++;
	array.append(u64_max); count++;

	CHECK(array.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = serialize(array);

	// Check deserialized data

	Object object = deserialize(data);

	CHECK(object.isArray());
	Array& test = object.asArray();
	CHECK(test.size() == count);

	size_t index = 0;
	CHECK(test.object(index).is<uint64_t>());
	CHECK(test.object(index).as<uint64_t>() == u8_min);

	index++;
	CHECK(test.object(index).is<uint64_t>());
	CHECK(test.object(index).as<uint64_t>() == u8_max);

	index++;
	CHECK(test.object(index).is<uint64_t>());
	CHECK(test.object(index).as<uint64_t>() == u16_min);

	index++;
	CHECK(test.object(index).is<uint64_t>());
	CHECK(test.object(index).as<uint64_t>() == u16_max);

	index++;
	CHECK(test.object(index).is<uint64_t>());
	CHECK(test.object(index).as<uint64_t>() == u32_min);

	index++;
	CHECK(test.object(index).is<uint64_t>());
	CHECK(test.object(index).as<uint64_t>() == u32_max);

	index++;
	CHECK(test.object(index).is<uint64_t>());
	CHECK(test.object(index).as<uint64_t>() == u64_min);

	index++;
	CHECK(test.object(index).is<uint64_t>());
	CHECK(test.object(index).as<uint64_t>() == u64_max);
}
#endif // }}}


/**
 * \brief Append a 32-bit floating point value.
 *
 * The \p value will be appended to the contents of the Array.
 *
 * \parcode
 * zakero::messagepack::Array array;
 * array.append(float(4.2));
 * \endparcode
 *
 * \return The index location of where the \p value was stored.
 */
size_t Array::append(const float value ///< The value to add
	) noexcept
{
	const size_t index = object_vector.size();

	object_vector.emplace_back(Object{value});

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("array/append/float")
{
	const float f32_min = std::numeric_limits<float>::min();
	const float f32_max = std::numeric_limits<float>::max();
	size_t count = 0;

	Array array;
	array.append(f32_min); count++;
	array.append(f32_max); count++;

	CHECK(array.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = serialize(array);

	// Check deserialized data

	Object object = deserialize(data);

	CHECK(object.isArray());
	Array& test = object.asArray();
	CHECK(test.size() == count);

	size_t index = 0;
	CHECK(test.object(index).is<float>());
	CHECK(test.object(index).as<float>() == f32_min);

	index++;
	CHECK(test.object(index).is<float>());
	CHECK(test.object(index).as<float>() == f32_max);
}
#endif // }}}


/**
 * \brief Append a 64-bit floating point value.
 *
 * The \p value will be appended to the contents of the Array.
 *
 * \parcode
 * zakero::messagepack::Array array;
 * array.append(double(0.42));
 * \endparcode
 *
 * \return The index location of where the \p value was stored.
 */
size_t Array::append(const double value ///< The value to add
	) noexcept
{
	const size_t index = object_vector.size();

	object_vector.emplace_back(Object{value});

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("array/append/double")
{
	const double f64_min = std::numeric_limits<double>::min();
	const double f64_max = std::numeric_limits<double>::max();
	size_t count = 0;

	Array array;
	array.append(f64_min); count++;
	array.append(f64_max); count++;

	CHECK(array.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = serialize(array);

	// Check deserialized data

	Object object = deserialize(data);

	CHECK(object.isArray());
	Array& test = object.asArray();
	CHECK(test.size() == count);

	size_t index = 0;
	CHECK(test.object(index).is<double>());
	CHECK(test.object(index).as<double>() == f64_min);

	index++;
	CHECK(test.object(index).is<double>());
	CHECK(test.object(index).as<double>() == f64_max);
}
#endif // }}}


/**
 * \brief Append a string.
 *
 * The \p value will be appended to the contents of the Array.
 *
 * \parcode
 * zakero::messagepack::Array array;
 * array.append("Hello, World!");
 * \endparcode
 *
 * \return The index location of where the \p value was stored.
 */
size_t Array::append(const std::string_view value ///< The value to add
	) noexcept
{
	const size_t index = object_vector.size();

	object_vector.emplace_back(Object{std::string(value)});

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("array/append/string")
{
	const std::string str_0;
	const std::string str_f (31, '_');
	const std::string str_8 (32, 'X');
	const std::string str_16(std::numeric_limits<uint8_t>::max() + 1 , '*');
	const std::string str_32(std::numeric_limits<uint16_t>::max() + 1, '|');
	size_t count = 0;

	Array array;
	array.append(str_0);  count++;
	array.append(str_f);  count++;
	array.append(str_8);  count++;
	array.append(str_16); count++;
	array.append(str_32); count++;

	CHECK(array.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = serialize(array);

	// Check deserialized data

	Object object = deserialize(data);

	CHECK(object.isArray());
	Array& test = object.asArray();
	CHECK(test.size() == count);

	size_t index = 0;
	CHECK(test.object(index).is<std::string>());
	CHECK(test.object(index).as<std::string>() == str_0);

	index++;
	CHECK(test.object(index).is<std::string>());
	CHECK(test.object(index).as<std::string>() == str_f);

	index++;
	CHECK(test.object(index).is<std::string>());
	CHECK(test.object(index).as<std::string>() == str_8);

	index++;
	CHECK(test.object(index).is<std::string>());
	CHECK(test.object(index).as<std::string>() == str_16);

	index++;
	CHECK(test.object(index).is<std::string>());
	CHECK(test.object(index).as<std::string>() == str_32);
}
#endif // }}}


/**
 * \brief Append a vector of binary data.
 *
 * The \p value will be appended to the contents of the Array.
 *
 * \parcode
 * zakero::messagepack::Array array;
 * const std::vector<uint8_t> data = { 0xde, 0xad, 0xca, 0xfe };
 * array.append(data);
 * \endparcode
 *
 * \return The index location of where the \p value was stored.
 */
size_t Array::append(const std::vector<uint8_t>& value ///< The value to add
	) noexcept
{
	const size_t index = object_vector.size();

	object_vector.emplace_back(Object{value});

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("array/append/binary (copy)")
{
	const std::vector<uint8_t> bin_0;
	const std::vector<uint8_t> bin_8 (32, 'X');
	const std::vector<uint8_t> bin_16(std::numeric_limits<uint8_t>::max() + 1 , '-');
	const std::vector<uint8_t> bin_32(std::numeric_limits<uint16_t>::max() + 1, '|');
	size_t count = 0;

	Array array;
	array.append(bin_0);  count++;
	array.append(bin_8);  count++;
	array.append(bin_16); count++;
	array.append(bin_32); count++;

	CHECK(array.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = serialize(array);

	// Check deserialized data

	Object object = deserialize(data);

	CHECK(object.isArray());
	Array& test = object.asArray();
	CHECK(test.size() == count);

	size_t index = 0;
	CHECK(test.object(index).is<std::vector<uint8_t>>());
	CHECK(test.object(index).as<std::vector<uint8_t>>() == bin_0);

	index++;
	CHECK(test.object(index).is<std::vector<uint8_t>>());
	CHECK(test.object(index).as<std::vector<uint8_t>>() == bin_8);

	index++;
	CHECK(test.object(index).is<std::vector<uint8_t>>());
	CHECK(test.object(index).as<std::vector<uint8_t>>() == bin_16);

	index++;
	CHECK(test.object(index).is<std::vector<uint8_t>>());
	CHECK(test.object(index).as<std::vector<uint8_t>>() == bin_32);
}
#endif // }}}


/**
 * \brief Append a vector of binary data.
 *
 * The \p value will be appended to the contents of the Array.
 *
 * \parcode
 * zakero::messagepack::Array array;
 * std::vector<uint8_t> data = { 0xde, 0xad, 0xca, 0xfe };
 * array.append(data);
 * \endparcode
 *
 * \return The index location of where the \p value was stored.
 */
size_t Array::append(std::vector<uint8_t>& value ///< The value to add
	) noexcept
{
	const size_t index = object_vector.size();

	object_vector.emplace_back(Object{std::move(value)});

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("array/append/binary (move)")
{
	const std::vector<uint8_t> bin_0;
	const std::vector<uint8_t> bin_8 (32, 'X');
	const std::vector<uint8_t> bin_16(std::numeric_limits<uint8_t>::max() + 1 , '-');
	const std::vector<uint8_t> bin_32(std::numeric_limits<uint16_t>::max() + 1, '|');
	size_t count = 0;

	std::vector<uint8_t> tmp_0(bin_0);
	std::vector<uint8_t> tmp_8(bin_8);
	std::vector<uint8_t> tmp_16(bin_16);
	std::vector<uint8_t> tmp_32(bin_32);

	Array array;
	array.append(tmp_0);  count++;
	array.append(tmp_8);  count++;
	array.append(tmp_16); count++;
	array.append(tmp_32); count++;

	CHECK(array.size() == count);
	CHECK(tmp_0.empty());
	CHECK(tmp_8.empty());
	CHECK(tmp_16.empty());
	CHECK(tmp_32.empty());

	// Check serialized data

	std::vector<uint8_t> data = serialize(array);

	// Check deserialized data

	Object object = deserialize(data);

	CHECK(object.isArray());
	Array& test = object.asArray();
	CHECK(test.size() == count);

	size_t index = 0;
	CHECK(test.object(index).is<std::vector<uint8_t>>());
	CHECK(test.object(index).as<std::vector<uint8_t>>() == bin_0);

	index++;
	CHECK(test.object(index).is<std::vector<uint8_t>>());
	CHECK(test.object(index).as<std::vector<uint8_t>>() == bin_8);

	index++;
	CHECK(test.object(index).is<std::vector<uint8_t>>());
	CHECK(test.object(index).as<std::vector<uint8_t>>() == bin_16);

	index++;
	CHECK(test.object(index).is<std::vector<uint8_t>>());
	CHECK(test.object(index).as<std::vector<uint8_t>>() == bin_32);
}
#endif // }}}


/**
 * \brief Append an Array.
 *
 * The \p array will be appended to the contents of this Array.
 *
 * \parcode
 * zakero::messagepack::Array sub_array;
 * sub_array.append(false);
 * sub_array.append(0x42);
 *
 * zakero::messagepack::Array array;
 * array.append(sub_array);
 * \endparcode
 *
 * \return The index location of where the \p value was stored.
 */
size_t Array::append(const Array& array ///< The Array to add
	) noexcept
{
	size_t index = object_vector.size();
	object_vector.emplace_back(Object{Array{}});

	Array& sub_array = object_vector[index].asArray();
	sub_array.object_vector = array.object_vector;

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("array/append/array (copy)")
{
	messagepack::Array sub_0;
	messagepack::Array sub_1;
		sub_1.appendNull();
	messagepack::Array sub_2;
		sub_2.append(true);
		sub_2.append(false);
	messagepack::Array sub_3;
		sub_3.append(std::string_view("Hello"));
		sub_3.append(std::string_view("World"));
	size_t count = 0;

	const messagepack::Array tmp_0 = sub_0;
	const messagepack::Array tmp_1 = sub_1;
	const messagepack::Array tmp_2 = sub_2;
	const messagepack::Array tmp_3 = sub_3;

	Array array;
	array.append(tmp_0); count++;
	array.append(tmp_1); count++;
	array.append(tmp_2); count++;
	array.append(tmp_3); count++;

	CHECK(array.size() == count);
	CHECK(tmp_0.size() == 0);
	CHECK(tmp_1.size() == 1);
	CHECK(tmp_2.size() == 2);
	CHECK(tmp_3.size() == 2);

	// Check serialized data

	std::vector<uint8_t> data = serialize(array);

	// Check deserialized data

	Object object = deserialize(data);

	CHECK(object.isArray());
	Array& test = object.asArray();
	CHECK(test.size() == count);

	size_t index = 0;
	CHECK(test.object(index).isArray());
	CHECK(test.object(index).asArray().size() == 0);

	index++;
	CHECK(test.object(index).isArray());
	CHECK(test.object(index).asArray().size() == 1);
	CHECK(test.object(index).asArray().object(0).isNull());

	index++;
	CHECK(test.object(index).isArray());
	CHECK(test.object(index).asArray().size() == 2);
	CHECK(test.object(index).asArray().object(0).is<bool>());
	CHECK(test.object(index).asArray().object(0).as<bool>() == true);
	CHECK(test.object(index).asArray().object(1).is<bool>());
	CHECK(test.object(index).asArray().object(1).as<bool>() == false);

	index++;
	CHECK(test.object(index).isArray());
	CHECK(test.object(index).asArray().size() == 2);
	CHECK(test.object(index).asArray().object(0).is<std::string>());
	CHECK(test.object(index).asArray().object(0).as<std::string>() == "Hello");
	CHECK(test.object(index).asArray().object(1).is<std::string>());
	CHECK(test.object(index).asArray().object(1).as<std::string>() == "World");
}
#endif // }}}


/**
 * \brief Append an Array.
 *
 * The \p array will be appended to the contents of this Array.
 *
 * \parcode
 * zakero::messagepack::Array sub_array;
 * sub_array.append(false);
 * sub_array.append(0x42);
 *
 * zakero::messagepack::Array array;
 * array.append(sub_array);
 * \endparcode
 *
 * \return The index location of where the \p value was stored.
 */
size_t Array::append(Array& array ///< The Array to add
	) noexcept
{
	size_t index = object_vector.size();
	object_vector.emplace_back(Object{Array{}});

	Array& sub_array = object_vector[index].asArray();
	sub_array.object_vector = std::move(array.object_vector);

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("array/append/array (move)")
{
	messagepack::Array sub_0;
	messagepack::Array sub_1;
		sub_1.appendNull();
	messagepack::Array sub_2;
		sub_2.append(true);
		sub_2.append(false);
	messagepack::Array sub_3;
		sub_3.append(std::string_view("Hello"));
		sub_3.append(std::string_view("World"));
	size_t count = 0;

	messagepack::Array tmp_0 = sub_0;
	messagepack::Array tmp_1 = sub_1;
	messagepack::Array tmp_2 = sub_2;
	messagepack::Array tmp_3 = sub_3;

	Array array;
	array.append(tmp_0); count++;
	array.append(tmp_1); count++;
	array.append(tmp_2); count++;
	array.append(tmp_3); count++;

	CHECK(array.size() == count);
	CHECK(tmp_0.size() == 0);
	CHECK(tmp_1.size() == 0);
	CHECK(tmp_2.size() == 0);
	CHECK(tmp_3.size() == 0);

	// Check serialized data

	std::vector<uint8_t> data = serialize(array);

	// Check deserialized data

	Object object = deserialize(data);

	CHECK(object.isArray());
	Array& test = object.asArray();
	CHECK(test.size() == count);

	size_t index = 0;
	CHECK(test.object(index).isArray());
	CHECK(test.object(index).asArray().size() == 0);

	index++;
	CHECK(test.object(index).isArray());
	CHECK(test.object(index).asArray().size() == 1);
	CHECK(test.object(index).asArray().object(0).isNull());

	index++;
	CHECK(test.object(index).isArray());
	CHECK(test.object(index).asArray().size() == 2);
	CHECK(test.object(index).asArray().object(0).is<bool>());
	CHECK(test.object(index).asArray().object(0).as<bool>() == true);
	CHECK(test.object(index).asArray().object(1).is<bool>());
	CHECK(test.object(index).asArray().object(1).as<bool>() == false);

	index++;
	CHECK(test.object(index).isArray());
	CHECK(test.object(index).asArray().size() == 2);
	CHECK(test.object(index).asArray().object(0).is<std::string>());
	CHECK(test.object(index).asArray().object(0).as<std::string>() == "Hello");
	CHECK(test.object(index).asArray().object(1).is<std::string>());
	CHECK(test.object(index).asArray().object(1).as<std::string>() == "World");
}
#endif // }}}


/**
 * \brief Append an extension.
 *
 * The \p value will be appended to the contents of this Array.
 *
 * \parcode
 * zakero::messagepack::Ext ext;
 * ext.type = 42;
 * ext.data = std::vector<uint8_t>(42, '*');
 *
 * zakero::messagepack::Array array;
 * array.append(ext);
 * \endparcode
 *
 * \return The index location of where the \p value was stored.
 */
size_t Array::append(const Ext& ext ///< The Ext to add
	) noexcept
{
	size_t index = object_vector.size();
	object_vector.emplace_back(Object{ext});

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("array/append/ext (copy)")
{
	const uint8_t chr_16 = '-';
	const uint8_t chr_32 = '|';

	Ext ext_0;
	ext_0.type = 0;
	ext_0.data = {};

	const Ext ext_16 =
	{	.data = std::vector<uint8_t>(16, chr_16)
	,	.type = 16
	};

	const Ext ext_32 =
	{	.data = std::vector<uint8_t>(32, chr_32)
	,	.type = 32
	};

	size_t count = 0;
	Array array;
	array.append(ext_0);  count++;
	array.append(ext_16); count++;
	array.append(ext_32); count++;

	CHECK(array.size() == count);
	CHECK(ext_16.data.size() == 16);
	CHECK(ext_32.data.size() == 32);

	// Check serialized data

	std::vector<uint8_t> data = serialize(array);

	// Check deserialized data

	Object object = deserialize(data);

	CHECK(object.isArray());
	Array& test = object.asArray();
	CHECK(test.size() == count);

	size_t index = 0;
	CHECK(test.object(index).isExt());
	CHECK(test.object(index).asExt().type == 0);
	CHECK(test.object(index).asExt().data.size() == 0);

	index++;
	CHECK(test.object(index).isExt());
	CHECK(test.object(index).asExt().type == 16);
	CHECK(test.object(index).asExt().data.size() == 16);
	for(size_t i = 0; i < test.object(index).asExt().data.size(); i++)
	{
		CHECK(test.object(index).asExt().data[i] == chr_16);
	}

	index++;
	CHECK(test.object(index).isExt());
	CHECK(test.object(index).asExt().type == 32);
	CHECK(test.object(index).asExt().data.size() == 32);
	for(size_t i = 0; i < test.object(index).asExt().data.size(); i++)
	{
		CHECK(test.object(index).asExt().data[i] == chr_32);
	}
}
#endif // }}}


/**
 * \brief Append an extension.
 *
 * The \p value will be appended to the contents of this Array.
 *
 * \parcode
 * zakero::messagepack::Ext ext;
 * ext.type = 42;
 * ext.data = std::vector<uint8_t>(42, '*');
 *
 * zakero::messagepack::Array array;
 * array.append(ext);
 * \endparcode
 *
 * \return The index location of where the \p value was stored.
 */
size_t Array::append(Ext& ext ///< The Ext to add
	) noexcept
{
	size_t index = object_vector.size();
	object_vector.push_back(Object{std::move(ext)});

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("array/append/ext (move)")
{
	const uint8_t chr_16 = '-';
	const uint8_t chr_32 = '|';

	Ext ext_0;
	ext_0.type = 0;
	ext_0.data = {};

	Ext ext_16;
	ext_16.type = 16;
	ext_16.data = std::vector<uint8_t>(16, chr_16);

	Ext ext_32;
	ext_32.type = 32;
	ext_32.data = std::vector<uint8_t>(32, chr_32);

	size_t count = 0;
	Array array;
	array.append(ext_0);  count++;
	array.append(ext_16); count++;
	array.append(ext_32); count++;

	CHECK(array.size() == count);
	CHECK(ext_16.data.size() == 0);
	CHECK(ext_32.data.size() == 0);

	// Check serialized data

	std::vector<uint8_t> data = serialize(array);

	// Check deserialized data

	Object object = deserialize(data);

	CHECK(object.isArray());
	Array& test = object.asArray();
	CHECK(test.size() == count);

	size_t index = 0;
	CHECK(test.object(index).isExt());
	CHECK(test.object(index).asExt().type == 0);
	CHECK(test.object(index).asExt().data.size() == 0);

	index++;
	CHECK(test.object(index).isExt());
	CHECK(test.object(index).asExt().type == 16);
	CHECK(test.object(index).asExt().data.size() == 16);
	for(size_t i = 0; i < test.object(index).asExt().data.size(); i++)
	{
		CHECK(test.object(index).asExt().data[i] == chr_16);
	}

	index++;
	CHECK(test.object(index).isExt());
	CHECK(test.object(index).asExt().type == 32);
	CHECK(test.object(index).asExt().data.size() == 32);
	for(size_t i = 0; i < test.object(index).asExt().data.size(); i++)
	{
		CHECK(test.object(index).asExt().data[i] == chr_32);
	}
}
#endif // }}}


/**
 * \brief Append a Map.
 *
 * The \p value will be appended to the contents of this Array.
 *
 * \parcode
 * zakero::messagepack::Map map;
 * map.set(Object{42}, Object{std::string("foo")});
 *
 * zakero::messagepack::Array array;
 * array.append(map);
 * \endparcode
 *
 * \return The index location of where the \p value was stored.
 */
size_t Array::append(const Map& map ///< The Map to add
	) noexcept
{
	size_t index = object_vector.size();
	object_vector.emplace_back(Object{map});

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("array/append/map (copy)")
{
	const Object key_1 = Object{true};
	const Object key_2 = Object{int64_t(0)};

	const std::string str("Hello, World!");
	const std::vector<uint8_t> bin(21, '@');

	const Object val_1 = Object{str};
	const Object val_2 = Object{bin};

	Map map_1;
	map_1.set(key_1, val_1);
	map_1.set(key_2, val_2);

	Map map_2;
	map_2.set(val_1, key_1);
	map_2.set(val_2, key_2);

	size_t count = 0;
	Array array;
	array.append((const Map)map_1); count++;
	array.append((const Map)map_2); count++;

	CHECK(array.size() == count);
	CHECK(map_1.size() == 2);
	CHECK(map_2.size() == 2);

	// Check serialized data

	std::vector<uint8_t> data = serialize(array);

	// Check deserialized data

	Object object = deserialize(data);

	CHECK(object.isArray());
	Array& test = object.asArray();
	CHECK(test.size() == count);

	size_t index = 0;
	CHECK(test.object(index).isMap());
	CHECK(test.object(index).asMap().size() == 2);
	CHECK(test.object(index).asMap().keyExists(key_1) == true);
	CHECK(test.object(index).asMap().valueOf(key_1) == val_1);
	CHECK(test.object(index).asMap().keyExists(key_2) == true);
	CHECK(test.object(index).asMap().valueOf(key_2) == val_2);

	index++;
	CHECK(test.object(index).isMap());
	CHECK(test.object(index).asMap().size() == 2);
	CHECK(test.object(index).asMap().keyExists(val_1) == true);
	CHECK(test.object(index).asMap().valueOf(val_1) == key_1);
	CHECK(test.object(index).asMap().keyExists(val_2) == true);
	CHECK(test.object(index).asMap().valueOf(val_2) == key_2);
}
#endif // }}}


/**
 * \brief Append a Map.
 *
 * The \p value will be appended to the contents of this Array.
 *
 * \parcode
 * zakero::messagepack::Map map;
 * map.set(Object{42}, Object{std::string("foo")});
 *
 * zakero::messagepack::Array array;
 * array.append(map);
 * \endparcode
 *
 * \return The index location of where the \p value was stored.
 */
size_t Array::append(Map& map ///< The Map to add
	) noexcept
{
	size_t index = object_vector.size();
	object_vector.push_back(Object{std::move(map)});

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("array/append/map (move)")
{
	const Object key_1 = Object{true};
	const Object key_2 = Object{int64_t(0)};

	const std::string str("Hello, World!");
	const std::vector<uint8_t> bin(21, '@');

	const Object val_1 = Object{str};
	const Object val_2 = Object{bin};

	Map map_1;
	map_1.set(key_1, val_1);
	map_1.set(key_2, val_2);

	Map map_2;
	map_2.set(val_1, key_1);
	map_2.set(val_2, key_2);

	size_t count = 0;
	Array array;
	array.append(map_1); count++;
	array.append(map_2); count++;

	CHECK(array.size() == count);
	CHECK(map_1.size() == 0);
	CHECK(map_2.size() == 0);

	// Check serialized data

	std::vector<uint8_t> data = serialize(array);

	// Check deserialized data

	Object object = deserialize(data);

	CHECK(object.isArray());
	Array& test = object.asArray();
	CHECK(test.size() == count);

	size_t index = 0;
	CHECK(test.object(index).isMap());
	CHECK(test.object(index).asMap().size() == 2);
	CHECK(test.object(index).asMap().keyExists(key_1) == true);
	CHECK(test.object(index).asMap().valueOf(key_1) == val_1);
	CHECK(test.object(index).asMap().keyExists(key_2) == true);
	CHECK(test.object(index).asMap().valueOf(key_2) == val_2);

	index++;
	CHECK(test.object(index).isMap());
	CHECK(test.object(index).asMap().size() == 2);
	CHECK(test.object(index).asMap().keyExists(val_1) == true);
	CHECK(test.object(index).asMap().valueOf(val_1) == key_1);
	CHECK(test.object(index).asMap().keyExists(val_2) == true);
	CHECK(test.object(index).asMap().valueOf(val_2) == key_2);
}
#endif // }}}


/**
 * \brief Append an Object.
 *
 * The \p value will be appended to the contents of this Array.
 *
 * \parcode
 * const zakero::messagepack::Object object{int8_t(42)};
 * zakero::messagepack::Array array;
 * array.append(object);
 * \endparcode
 *
 * \return The index location of where the \p value was stored.
 */
size_t Array::append(const Object& object ///< The object to add
	) noexcept
{
	const size_t index = object_vector.size();

	object_vector.push_back(object);

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("array/append/object (copy)")
{
	messagepack::Object obj_0 = Object{true};
	messagepack::Object obj_1 = Object{(uint64_t)42};
	messagepack::Object obj_2 = Object{std::string("foo")};

	const messagepack::Object tmp_0 = obj_0;
	const messagepack::Object tmp_1 = obj_1;
	const messagepack::Object tmp_2 = obj_2;
	size_t count = 0;

	Array array;
	array.append(tmp_0); count++;
	array.append(tmp_1); count++;
	array.append(tmp_2); count++;

	CHECK(array.size() == count);
	CHECK(tmp_0.isNull() == false);
	CHECK(tmp_1.isNull() == false);
	CHECK(tmp_2.isNull() == false);

	// Check serialized data

	std::vector<uint8_t> data = serialize(array);

	// Check deserialized data

	Object object = deserialize(data);

	CHECK(object.isArray());
	Array& test = object.asArray();
	CHECK(test.size() == count);

	size_t index = 0;
	CHECK(test.object(index).is<bool>());
	CHECK(test.object(index).as<bool>() == true);

	index++;
	CHECK(test.object(index).is<uint64_t>());
	CHECK(test.object(index).as<uint64_t>() == 42);

	index++;
	CHECK(test.object(index).is<std::string>());
	CHECK(test.object(index).as<std::string>() == "foo");
}
#endif // }}}


/**
 * \brief Append an Object.
 *
 * The \p value will be appended to the contents of this Array.
 *
 * \parcode
 * zakero::messagepack::Array array;
 * array.append( Object{ int8_t(42) } );
 * \endparcode
 *
 * \return The index location of where the \p value was stored.
 */
size_t Array::append(Object& object ///< The object to add
	) noexcept
{
	const size_t index = object_vector.size();

	object_vector.push_back(std::move(object));
	object = {};

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("array/append/object (move)")
{
	messagepack::Object obj_0 = Object{true};
	messagepack::Object obj_1 = Object{(uint64_t)42};
	messagepack::Object obj_2 = Object{std::string("foo")};

	messagepack::Object tmp_0 = obj_0;
	messagepack::Object tmp_1 = obj_1;
	messagepack::Object tmp_2 = obj_2;
	size_t count = 0;

	Array array;
	array.append(tmp_0); count++;
	array.append(tmp_1); count++;
	array.append(tmp_2); count++;

	CHECK(array.size() == count);
	CHECK(tmp_0.isNull() == true);
	CHECK(tmp_1.isNull() == true);
	CHECK(tmp_2.isNull() == true);

	// Check serialized data

	std::vector<uint8_t> data = serialize(array);

	// Check deserialized data

	Object object = deserialize(data);

	CHECK(object.isArray());
	Array& test = object.asArray();
	CHECK(test.size() == count);

	size_t index = 0;
	CHECK(test.object(index).is<bool>());
	CHECK(test.object(index).as<bool>() == true);

	index++;
	CHECK(test.object(index).is<uint64_t>());
	CHECK(test.object(index).as<uint64_t>() == 42);

	index++;
	CHECK(test.object(index).is<std::string>());
	CHECK(test.object(index).as<std::string>() == "foo");
}
#endif // }}}


/**
 * \brief Append a "Null" value.
 *
 * A "Null" value will be appended to the contents of this Array.
 *
 * \parcode
 * zakero::messagepack::Array array;
 * array.appendNull();
 * \endparcode
 *
 * \return The index location of where the value was stored.
 */
size_t Array::appendNull() noexcept
{
	const size_t index = object_vector.size();

	object_vector.emplace_back();

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("array/append/null")
{
	const size_t count = std::numeric_limits<uint16_t>::max() + 1;
	Array array;

	for(size_t i = 0; i < count; i++)
	{
		array.append(Object{true});
	}

	CHECK(array.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = serialize(array);

	// Check deserialized data

	Object object = deserialize(data);

	CHECK(object.isArray());
	Array& test = object.asArray();
	CHECK(test.size() == count);

	for(size_t i = 0; i < count; i++)
	{
		CHECK(test.object(i).is<bool>());
		CHECK(test.object(i).as<bool>() == true);
	}
}
#endif // }}}


/**
 * \fn zakero::messagepack::Array::object(const size_t)
 *
 * \brief Access a data object.
 *
 * After data has been added to the Array, that data can still be access by 
 * using its index value. The data object's type will be the C++ datatype, not 
 * the MessagePack format type.
 *
 * The returned Object can be modified as needed.
 *
 * \parcode
 * zakero::messagepack::Array array;
 * size_t index_foo = array.append(int64_t(0));
 * size_t index_bar = array.append(int64_t(0));
 *
 * int64_t val = rand();
 * array.object(index_foo) = val;
 * if(val & 1)
 * {
 *      // Change to string
 * 	array.object(index_bar) = "That's odd...";
 * }
 * else
 * {
 *      // Change to boolean
 * 	array.object(index_bar) = true;
 * }
 * \endparcode
 *
 * \return The data object.
 *
 * \param  index  The index of the data object.
 */


// No tests needed, functionality is tested else where.


/**
 * \fn zakero::messagepack::Array::object(const size_t) const
 *
 * \brief Access a data object.
 *
 * After data has been added to the Array, that data can still be access by 
 * using its index value. The data object's type will be the C++ datatype, not 
 * the MessagePack format type.
 *
 * The returned Object is Read-Only.
 *
 * \parcode
 * zakero::messagepack::Array array;
 * size_t index_foo = array.append(int64_t(0));
 * size_t index_bar = array.append(int64_t(0));
 *
 * int64_t val = rand();
 * if(array.object(index_foo) > val)
 * {
 * 	std::cout << "Foo Bigger\n";
 * }
 *
 * if(array.object(index_bar) > val)
 * {
 * 	std::cout << "Bar Bigger\n";
 * }
 * \endparcode
 *
 * \return The data object.
 *
 * \param  index  The index of the data object.
 */


// No tests needed, functionality is tested else where.


/**
 * \fn zakero::messagepack::Array::clear()
 *
 * \brief Remove all data from the Array.
 *
 * Remove all data from the Array.
 */


// No tests needed, a pass-thru to the std::vector<Object>.


/**
 * \fn zakero::messagepack::Array::size()
 *
 * \brief Get the size of the Array.
 *
 * \return The Array size.
 */


// No tests needed, a pass-thru to the std::vector<Object>.

// }}} Array
// {{{ Ext

/**
 * \struct Ext
 *
 * \brief Extension Data.
 *
 * The MessagePack specification defines a structure to hold new data-types and 
 * this structure implements that feature. To add a new data-type, set the 
 * Ext::type to a positive value and then fill the Ext::data with the 
 * information to be stored.
 *
 * \note Negative Ext::type value are reserved for use by the MessagePack 
 * specification.
 *
 * As an example, to add a GIF data-type, define the type value then fill the 
 * Ext::data with the GIF.
 * 
 * \parcode
 * #define TYPE_GIF    2
 * #define INDEX_NAME  0
 * #define INDEX_FILE  1
 * #define INDEX_IMAGE 2
 *
 * zakero::messagepack::Array array = {};
 * array.append({std::string_view("Super-Sonic Scooter")});
 * array.append({std::string_view("super_sonic_scooter.gif")});
 *
 * zakero::messagepack::Ext gif = {};
 * gif.type = TYPE_GIF;
 * gif.data = loadGif("super_sonic_scooter.gif");
 * array.append(gif);
 *
 * auto data = zakero::messagepack::serialize(array);
 *
 * // ...somewhere later...
 *
 * zakero::messagepack::Object object = zakero::messagepack::deserialize(data);
 * zakero::messagepack::Array image = object.asArray();
 *
 * std::string image_name = image.object(INDEX_NAME).asString();
 * std::string image_file = image.object(INDEX_FILE).asString();
 *
 * zakero::messagepack::Ext raw_data = image.object(INDEX_IMAGE).asExt();
 * Image image;
 * if(raw_data.type == TYPE_GIF)
 * {
 * 	image = parseGif(raw_data.data);
 * }
 * \endparcode
 */

/**
 * \var Ext::data
 *
 * \brief Extension binary data.
 */

/**
 * \var Ext::type
 *
 * \brief A unique identifier for the extension.
 */

// }}} Ext
// {{{ Map

/**
 * \struct Map
 *
 * \brief A Key/Value collection of Objects.
 *
 * Using the Map structure allows any MessagePack Object to be used as a "key" 
 * as well as a "value" in a map. The underling `object_key` and `object_value` 
 * can be accessed directly. However for the Map, it is recommended to only use 
 * the helper methods.
 *
 * Objects can be tested to find out if they are Map by using Object::isMap() 
 * and converted into a Map with Object::asMap(). A Map can not be converted 
 * into an Object. However, an Object can be constructed using a Map and the 
 * utility methods can operate on Maps as if they were Objects.
 */

/**
 * \brief Set a key/value pair.
 *
 * The provided \p key / \p value pair will be added to the Map. If the \p key 
 * already exists, its value will be replaced with \p value.
 *
 * \parcode
 * zakero::messagepack::Map map;
 *
 * map.set({std::string("Error Code")}   , {uint64_t(42)});
 * map.set({std::string("Error Message")}, {std::string("All the errors!")});
 * \endparcode
 *
 * \return The index of the \p key / \p value pair.
 */
size_t Map::set(const Object& key   ///< The key
	, const Object&       value ///< The value
	) noexcept
{
	for(size_t i = 0; i < object_key.size(); i++)
	{
		if(object_key[i] == key)
		{
			object_value[i] = value;

			return i;
		}
	}

	const size_t index = object_key.size();

	object_key.emplace_back(key);
	object_value.emplace_back(value);

	return index;
}

#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("map/set (copy)")
{
	const Object key_null  = {};
	const Object key_zero  = {int64_t(0)};
	const Object val_null  = {};
	const Object val_zero  = {int64_t(0)};

	Map map;

	SUBCASE("Same Key, Same Value")
	{
		map.set(key_null, val_null);
		CHECK(map.size() == 1);
		const Object& obj_1 = map.valueOf(key_null);
		CHECK(obj_1  == val_null);
		CHECK(&obj_1 != &val_null);

		map.set(key_null, val_null);
		CHECK(map.size() == 1);
		const Object& obj_2 = map.valueOf(key_null);
		CHECK(obj_2  == val_null);
		CHECK(&obj_2 != &val_null);
	}

	SUBCASE("Same Key, Differnt Value")
	{
		map.set(key_null, val_null);
		CHECK(map.size() == 1);
		const Object& obj_1 = map.valueOf(key_null);
		CHECK(obj_1 == val_null);

		map.set(key_null, val_zero);
		CHECK(map.size() == 1);
		const Object& obj_2 = map.valueOf(key_null);
		CHECK(obj_2 == val_zero);
	}

	SUBCASE("Different Key, Same Value")
	{
		map.set(key_null, val_null);
		map.set(key_zero, val_null);
		CHECK(map.size() == 2);

		const Object& obj_1 = map.valueOf(key_null);
		CHECK(obj_1 == val_null);

		const Object& obj_2 = map.valueOf(key_zero);
		CHECK(obj_2 == val_null);
	}

	SUBCASE("Different Key, Different Value")
	{
		map.set(key_null, val_null);
		map.set(key_zero, val_zero);
		CHECK(map.size() == 2);

		const Object& obj_1 = map.valueOf(key_null);
		CHECK(obj_1 == val_null);

		const Object& obj_2 = map.valueOf(key_zero);
		CHECK(obj_2 == val_zero);
	}
}
#endif // }}}


/**
 * \brief Set a key/value pair.
 *
 * The provided \p key / \p value pair will be added to the Map. If the \p key 
 * already exists, its value will be replaced with \p value.
 *
 * \parcode
 * zakero::messagepack::Map map;
 *
 * map.set({std::string("Error Code")}   , {uint64_t(42)});
 * map.set({std::string("Error Message")}, {std::string("All the errors!")});
 * \endparcode
 *
 * \return The index of the \p key / \p value pair.
 *
 * \todo Storing the key/value pairs in vectors is very slow. Look into using 
 * std::hash<std::variant> to create a std::map().
 */
size_t Map::set(Object& key   ///< The key
	, Object&       value ///< The value
	) noexcept
{
	for(size_t i = 0; i < object_key.size(); i++)
	{
		if(object_key[i] == key)
		{
			object_value[i] = std::move(value);

			return i;
		}
	}

	const size_t index = object_key.size();

	object_key.emplace_back(key);
	object_value.emplace_back(std::move(value));

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("map/set")
{
	Object key_true = Object{true};
	Object key_zero = Object{int64_t(0)};
	Object val_true = Object{true};
	Object val_zero = Object{int64_t(0)};

	Map map;

	SUBCASE("Same Key, Same Value")
	{
		map.set(key_true, val_true);
		CHECK(map.size() == 1);

		Object& obj_1 = map.valueOf(key_true);
		CHECK(obj_1 == val_true);
		obj_1 = {val_zero};
		CHECK(map.valueOf(key_true) == val_zero);

		map.set(key_true, val_zero);
		CHECK(map.size() == 1);

		Object& obj_2 = map.valueOf(key_true);
		CHECK(obj_2 == val_zero);
	}

	SUBCASE("Same Key, Differnt Value")
	{
		map.set(key_true, val_true);
		CHECK(map.size() == 1);

		CHECK(map.valueOf(key_true) == val_true);

		map.set(key_true, val_zero);
		CHECK(map.size() == 1);
		CHECK(map.valueOf(key_true) == val_zero);
	}

	SUBCASE("Different Key, Same Value")
	{
		map.set(key_true, val_true);
		map.set(key_zero, val_true);
		CHECK(map.size() == 2);
		CHECK(map.valueOf(key_true) == val_true);
		CHECK(map.valueOf(key_zero) == val_true);
	}

	SUBCASE("Different Key, Different Value")
	{
		map.set(key_true, val_true);
		map.set(key_zero, val_zero);
		CHECK(map.size() == 2);
		CHECK(map.valueOf(key_true) == val_true);
		CHECK(map.valueOf(key_zero) == val_zero);
	}
}
#endif // }}}


/**
 * \brief Erase a key/value pair.
 *
 * If the specified \p key exists, the \p key and matching value will be 
 * removed frame the Map.
 *
 * \parcode
 * zakero::messagepack::Map map;
 *
 * // What is true?
 * map.set({true}, {uint64_t(123456)});
 *
 * // Truth can not be defined...
 * map.erase({true});
 * \endparcode
 */
void Map::erase(const Object& key ///< The key
	) noexcept
{
	auto key_iter = object_key.begin();
	auto val_iter = object_value.begin();

	while(key_iter != object_key.end())
	{
		if(*key_iter == key)
		{
			object_key.erase(key_iter);
			object_value.erase(val_iter);

			break;
		}

		key_iter++;
		val_iter++;
	}
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("map/erase")
{
	Object key_nope = Object{};
	Object key_true = Object{true};
	Object key_zero = Object{int64_t(0)};
	Object val_null = Object{};

	Map map;

	map.erase(key_nope); // Nothing should happen

	map.set(key_true, val_null);
	map.set(key_zero, val_null);
	CHECK(map.keyExists(key_true) == true);
	CHECK(map.keyExists(key_zero) == true);

	map.erase(key_nope); // Nothing should happen
	CHECK(map.size() == 2);

	map.erase(key_true);
	CHECK(map.size() == 1);
	CHECK(map.keyExists(key_true) == false);
	CHECK(map.keyExists(key_zero) == true);

	map.erase(key_true);
	CHECK(map.size() == 1);
	CHECK(map.keyExists(key_true) == false);
	CHECK(map.keyExists(key_zero) == true);

	map.erase(key_zero);
	CHECK(map.size() == 0);
	CHECK(map.keyExists(key_true) == false);
	CHECK(map.keyExists(key_zero) == false);

	map.erase(key_zero);
	CHECK(map.size() == 0);
	CHECK(map.keyExists(key_true) == false);
	CHECK(map.keyExists(key_zero) == false);
}
#endif // }}}


/**
 * \brief Check if a key exists.
 *
 * Search the Map and return \true if the \p key exists. If the \p key was not 
 * found, return \false.
 *
 * \parcode
 * zakero::messagepack::Map map;
 *
 * map.set(Object{}, Object{});
 * if(map.keyExists(Object{}))
 * {
 * 	// In the Key of Null
 * }
 * \endparcode
 *
 * \retval true  The key exists.
 * \retval false The key does not exist.
 */
bool Map::keyExists(const Object& key ///< The key Object
	) const noexcept
{
	for(const auto& key_ : object_key)
	{
		if(key_ == key)
		{
			return true;
		}
	}

	return false;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("map/keyexists")
{
	Object key_true = Object{true};
	Object key_zero = Object{int64_t(0)};
	Object val_null = Object{};

	Map map;

	CHECK(map.size() == 0);
	CHECK(map.keyExists(key_true) == false);
	CHECK(map.keyExists(key_zero) == false);

	map.set(key_true, val_null);
	CHECK(map.keyExists(key_true) == true);
	CHECK(map.keyExists(key_zero) == false);

	map.set(key_zero, val_null);
	CHECK(map.keyExists(key_true) == true);
	CHECK(map.keyExists(key_zero) == true);
}
#endif // }}}


/**
 * \brief Get the index of a key/value pair.
 *
 * Use this method to get the index of a \p key. The benefits of using an index 
 * is to avoid constantly searching the Map. Also, using the index is the only 
 * way of changing the \p key.
 *
 * \note If any key/value pair is inserted or removed, all indexes after 
 * changed location are no longer valid.
 *
 * \parcode
 * zakero::messagepack::Map map;
 *
 * map.set(Object{}, Object{});
 * size_t index = map.keyIndex(Object{});
 * map.keyAt(index)   = Object{true);
 * map.valueAt(index) = Object{std::string("true"));
 * \endparcode
 *
 * \return The index
 */
size_t Map::keyIndex(const Object& key ///< The key
	) const noexcept
{
	size_t index = 0;

	for(const auto& key_ : object_key)
	{
		if(key_ == key)
		{
			return index;
		}

		index++;
	}

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("map/keyindex")
{
	Object key_nope = Object{};
	Object key_true = Object{true};
	Object key_zero = Object{int64_t(0)};
	Object val_null = Object{};

	Map map;

	CHECK(map.size() == 0);
	CHECK(map.keyIndex(key_true) >= map.size());

	// Assumes key/values are added sequetially

	map.set(key_true, val_null);
	CHECK(map.keyIndex(key_true) == 0);

	map.set(key_zero, val_null);
	CHECK(map.keyIndex(key_zero) == 1);

	CHECK(map.keyIndex(key_nope) >= map.size());
}
#endif // }}}


/**
 * \brief Get the value of a key.
 *
 * The value associated with the provided \p key will be returned. If the \p 
 * key does not exist, then a reference to the \p key will be returned.
 *
 * \parcode
 * zakero::messagepack::Map map;
 *
 * map.set(Object{uint64_t(42)}, Object{std::string("The Answer"});
 *
 * const zakero::messagepack::Object key = Object{true};
 * const zakero::messagepack::Object& object = map.valueOf({true});
 *
 * if(&object == &key)
 * {
 * 	// key not found
 * }
 * \endparcode
 *
 * \return The value.
 */
const Object& Map::valueOf(const Object& key ///< The key
	) const noexcept
{
	for(size_t i = 0; i < object_key.size(); i++)
	{
		if(object_key[i] == key)
		{
			return object_value[i];
		}
	}

	return key;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("map/valueof (const)")
{
	Map map;

	SUBCASE("Exists")
	{
		const Object key_null   = {};
		const Object key_bool   = {true};
		const Object key_int64  = {int64_t(0)};
		const Object key_uint64 = {uint64_t(0)};
		const Object key_float  = {float(0)};
		const Object key_double = {double(0)};
		const Object key_string = {std::string("_")};
		const Object key_binary = {std::vector<uint8_t>(1, 'X')};

		const Object value_0 = {uint64_t(0)};
		const Object value_1 = {uint64_t(1)};
		const Object value_2 = {uint64_t(2)};
		const Object value_3 = {uint64_t(3)};
		const Object value_4 = {uint64_t(4)};
		const Object value_5 = {uint64_t(5)};
		const Object value_6 = {uint64_t(6)};
		const Object value_7 = {uint64_t(7)};

		map.object_key =
		{	key_null
		,	key_bool
		,	key_int64
		,	key_uint64
		,	key_float
		,	key_double
		,	key_string
		,	key_binary
		};

		map.object_value =
		{	value_0
		,	value_1
		,	value_2
		,	value_3
		,	value_4
		,	value_5
		,	value_6
		,	value_7
		};

		CHECK(map.valueOf(key_null)   == value_0);
		CHECK(map.valueOf(key_bool)   == value_1);
		CHECK(map.valueOf(key_int64)  == value_2);
		CHECK(map.valueOf(key_uint64) == value_3);
		CHECK(map.valueOf(key_float)  == value_4);
		CHECK(map.valueOf(key_double) == value_5);
		CHECK(map.valueOf(key_string) == value_6);
		CHECK(map.valueOf(key_binary) == value_7);
	}

	SUBCASE("Not Exists")
	{
		const Object  bad_key = {};
		const Object& bad_val = map.valueOf(bad_key);
		CHECK(&bad_key == &bad_val);
	}
}
#endif // }}}


/**
 * \brief Get the value of a key.
 *
 * The value associated with the provided \p key will be returned. If the \p 
 * key does not exist, then a reference to the \p key will be returned.  
 * Checking if the key exists (keyExists()) is more reliable than using this 
 * error handling.
 *
 * \parcode
 * zakero::messagepack::Map map;
 *
 * map.set(Object{uint64_t(42)}, Object{std::string("The Answer"});
 * zakero::messagepack::Object& thing = map.valueOf({uint64_t(42)});
 *
 * zakero::messagepack::Object key = Object{true};
 * zakero::messagepack::Object& object = map.valueOf(key);
 *
 * if(&object == &key)
 * {
 * 	// key not found
 * }
 * \endparcode
 *
 * \return The value.
 */
Object& Map::valueOf(Object& key ///< The key
	) noexcept
{
	for(size_t i = 0; i < object_key.size(); i++)
	{
		if(object_key[i] == key)
		{
			return object_value[i];
		}
	}

	return key;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("map/valueof")
{
	Map map;

	SUBCASE("Exists")
	{
		Object key_null   = {};
		Object key_bool   = {true};
		Object key_int64  = {int64_t(0)};
		Object key_uint64 = {uint64_t(0)};
		Object key_float  = {float(0)};
		Object key_double = {double(0)};
		Object key_string = {std::string("_")};
		Object key_binary = {std::vector<uint8_t>(1, 'X')};

		Object value_0 = {uint64_t(0)};
		Object value_1 = {uint64_t(1)};
		Object value_2 = {uint64_t(2)};
		Object value_3 = {uint64_t(3)};
		Object value_4 = {uint64_t(4)};
		Object value_5 = {uint64_t(5)};
		Object value_6 = {uint64_t(6)};
		Object value_7 = {uint64_t(7)};

		map.object_key =
		{	key_null
		,	key_bool
		,	key_int64
		,	key_uint64
		,	key_float
		,	key_double
		,	key_string
		,	key_binary
		};

		map.object_value =
		{	value_0
		,	value_1
		,	value_2
		,	value_3
		,	value_4
		,	value_5
		,	value_6
		,	value_7
		};

		CHECK(map.valueOf(key_null)   == value_0);
		CHECK(map.valueOf(key_bool)   == value_1);
		CHECK(map.valueOf(key_int64)  == value_2);
		CHECK(map.valueOf(key_uint64) == value_3);
		CHECK(map.valueOf(key_float)  == value_4);
		CHECK(map.valueOf(key_double) == value_5);
		CHECK(map.valueOf(key_string) == value_6);
		CHECK(map.valueOf(key_binary) == value_7);

		map.valueOf(key_null) = {false};
		CHECK(map.valueOf(key_null) == Object{false});

	}

	SUBCASE("Not Exists")
	{
		Object  bad_key = {};
		Object& bad_val = map.valueOf(bad_key);
		CHECK(&bad_key == &bad_val);
	}
}
#endif // }}}


/**
 * \fn zakero::messagepack::Map::clear()
 *
 * \brief Remove the contents of the Map.
 *
 * All of the key/value pairs will be removed.
 */


/**
 * \fn zakero::messagepack::Map::keyAt(const size_t)
 *
 * \brief Access a _key_ Object.
 *
 * The _key_ at the requested index location will be returned.
 *
 * \return An Object
 *
 * \param index The index of the key.
 */


/**
 * \fn zakero::messagepack::Map::keyAt(const size_t) const
 *
 * \brief Access a _key_ Object.
 *
 * The _key_ at the requested index location will be returned.
 *
 * \return An Object
 *
 * \param index The index of the key.
 */


/**
 * \fn zakero::messagepack::Map::size() const
 *
 * \brief Get the size of the Map.
 *
 * \return The number of key/value pairs.
 */


/**
 * \fn zakero::messagepack::Map::valueAt(const size_t)
 *
 * \brief Get a _value_ from the Map.
 *
 * The Object at the specified index will be returned.
 *
 * \return The Object.
 *
 * \param index The index location.
 */


/**
 * \fn zakero::messagepack::Map::valueAt(const size_t) const
 *
 * \brief Get a _value_ from the Map.
 *
 * The Object at the specified index will be returned.
 *
 * \return The Object.
 *
 * \param index The index location.
 */

// }}} Map
// {{{ Object

/**
 * \struct Object
 *
 * \brief A Data Object.
 *
 * The role of this object is to store all the data-types in the MessagePack 
 * specification. This is accomplish by using the `std::variant` and a 
 * collection of helper methods to reduce the verbosity of templates.
 *
 * \note The `std::monostate` is used to represent `null`.
 *
 * Once an Object has been set to a type, it is an error to cast the object to 
 * any other type.
 *
 * \parcode
 * zakero::messagepack::Object object;
 *
 * object = {true}; // Object is now a boolean
 * int64_t value = object.as<int64_t>(); // Error: Object is not a int64_t
 *
 * // Reusing the same object...
 * object = {int64_t(42)}; // Object is now a int64_t
 * value = object.as<int64_t>(); // Ok: Object is a int64_t
 * \endparcode
 */


/**
 * \fn zakero::messagepack::Object::as()
 *
 * \brief Convert to type `T`.
 *
 * The Object will be converted so that it will be treated __as__ the requested 
 * \p T type.
 *
 * \parcode
 * zakero::messagepack::Object object = {true};
 *
 * bool b = object.as<bool>();
 * \endparcode
 *
 * \return The request data-type.
 *
 * \tparam T The data-type to convert to.
 */


/**
 * \fn zakero::messagepack::Object::as() const
 *
 * \brief Convert to type `T`.
 *
 * The Object will be converted so that it will be treated __as__ the requested 
 * \p T type.
 *
 * \parcode
 * zakero::messagepack::Object object = {true};
 *
 * bool b = object.as<bool>();
 * \endparcode
 *
 * \return The request data-type.
 *
 * \tparam T The data-type to convert to.
 */


/**
 * \fn zakero::messagepack::Object::asArray()
 *
 * \brief Convert to an Array.
 *
 * The same as: `object.as<zakero::messagepack::Array>()`
 *
 * \return A reference to an Array.
 *
 * \see as()
 */


/**
 * \fn zakero::messagepack::Object::asArray() const
 *
 * \brief Convert to an Array.
 *
 * The same as: `object.as<zakero::messagepack::Array>()`
 *
 * \return A reference to an Array.
 *
 * \see as()
 */


/**
 * \fn zakero::messagepack::Object::asBinary()
 *
 * \brief Convert to a std::vector<uint8_t>.
 *
 * The same as: `object.as<std::vector<uint8_t>>()`
 *
 * \return A reference to a std::vector<uint8_t>.
 *
 * \see as()
 */


/**
 * \fn zakero::messagepack::Object::asBinary() const
 *
 * \brief Convert to a std::vector<uint8_t>.
 *
 * The same as: `object.as<std::vector<uint8_t>>()`
 *
 * \return A reference to a std::vector<uint8_t>.
 *
 * \see as()
 */


/**
 * \fn zakero::messagepack::Object::asExt()
 *
 * \brief Convert to an Ext.
 *
 * The same as: `object.as<zakero::messagepack::Ext>()`
 *
 * \return A reference to an Ext.
 *
 * \see as()
 */


/**
 * \fn zakero::messagepack::Object::asExt() const
 *
 * \brief Convert to an Ext.
 *
 * The same as: `object.as<zakero::messagepack::Ext>()`
 *
 * \return A reference to an Ext.
 *
 * \see as()
 */


/**
 * \fn zakero::messagepack::Object::asMap()
 *
 * \brief Convert to a Map.
 *
 * The same as: `object.as<zakero::messagepack::Map>()`
 *
 * \return A reference to a Map.
 *
 * \see as()
 */


/**
 * \fn zakero::messagepack::Object::asMap() const
 *
 * \brief Convert to a Map.
 *
 * The same as: `object.as<zakero::messagepack::Map>()`
 *
 * \return A reference to a Map.
 *
 * \see as()
 */


/**
 * \fn zakero::messagepack::Object::asString() const
 *
 * \brief Convert to a std::string.
 *
 * The same as: `object.as<std::string>()`
 *
 * \return A reference to a std::string.
 *
 * \see as()
 */


/**
 * \fn zakero::messagepack::Object::is() const
 *
 * \brief Is Object of type `T`.
 *
 * The Object will be checked to see if it is of type `T`.
 *
 * \parcode
 * zakero::messagepack::Object object = {int64_t(123)};
 *
 * bool is_int = object.is<int64_t>();
 * \endparcode
 *
 * \retval true  The Object is of type `T`
 * \retval false The Object is not of type `T`
 *
 * \tparam T The data-type to check.
 */


/**
 * \fn zakero::messagepack::Object::isArray()
 *
 * \brief Is Object an Array?
 *
 * The same as: `object.is<zakero::messagepack::Array>()`
 *
 * \retval true  The Object is an Array
 * \retval false The Object is not an Array
 *
 * \see is()
 */


/**
 * \fn zakero::messagepack::Object::isBinary()
 *
 * \brief Is Object binary data?
 *
 * The same as: `object.is<std::vector<uint8_t>>()`
 *
 * \retval true  The Object is a std::vector<uint8_t>
 * \retval false The Object is not a std::vector<uint8_t>
 *
 * \see is()
 */


/**
 * \fn zakero::messagepack::Object::isExt()
 *
 * \brief Is Object an Ext?
 *
 * The same as: `object.is<zakero::messagepack::Ext>()`
 *
 * \retval true  The Object is an Ext
 * \retval false The Object is not an Ext
 *
 * \see is()
 */


/**
 * \fn zakero::messagepack::Object::isMap()
 *
 * \brief Is Object a Map?
 *
 * The same as: `object.is<zakero::messagepack::Map>()`
 *
 * \retval true  The Object is a Map
 * \retval false The Object is not a Map
 *
 * \see is()
 */


/**
 * \fn zakero::messagepack::Object::isNull()
 *
 * \brief Does the Object represent a `null`?
 *
 * The same as: `object.is<std::monostate>()`
 *
 * \retval true  The Object is a std::monostate
 * \retval false The Object is not a std::monostate
 *
 * \see is()
 */


/**
 * \fn zakero::messagepack::Object::isString()
 *
 * \brief Is Object a std::string?
 *
 * The same as: `object.is<std::string>()`
 *
 * \retval true  The Object is a std::string
 * \retval false The Object is not a std::string
 *
 * \see is()
 */

// }}} Object
// {{{ Extensions
// {{{ Extensions: Timestamp

/**
 * \brief Timestamp Extension Check.
 *
 * Use this method to determine if the \p object is a MessagePack Timestamp 
 * Extension.
 *
 * \parcode
 * std::vector<uint8_t> data = getSerializedData();
 * zakero::messagepack::Object obj = zakero::messagepack::deserialize(data);
 *
 * if(zakero::messagepack::extensionTimestampCheck(obj) == false)
 * {
 * 	return ERROR_INVALID_TIMESTAMP;
 * }
 * \endparcode
 *
 * \retval true  The \p object is a Timestamp extension.
 * \retval false The \p object is not a Timestamp extension.
 */
bool extensionTimestampCheck(const Object& object ///< The Ext to check.
	) noexcept
{
	if(object.isExt() == true)
	{
		const Ext& ext = object.asExt();

		if(ext.type == -1)
		{
			if(ext.data.size() == 4
				|| ext.data.size() == 8
				|| ext.data.size() == 12
				)
			{
				return true;
			}
		}
	}

	return false;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("extension/timestamp/check")
{
	Object object = {Ext{}};
	Ext& ext      = object.asExt();

	CHECK(extensionTimestampCheck(object) == false);

	// --- Bad Ext.type value --- //

	ext.data = std::vector<uint8_t>();
	CHECK(extensionTimestampCheck(object) == false);

	ext.data = std::vector<uint8_t>(1, 0);
	CHECK(extensionTimestampCheck(object) == false);

	ext.data = std::vector<uint8_t>(4, 0);
	CHECK(extensionTimestampCheck(object) == false);

	ext.data = std::vector<uint8_t>(8, 0);
	CHECK(extensionTimestampCheck(object) == false);

	ext.data = std::vector<uint8_t>(12, 0);
	CHECK(extensionTimestampCheck(object) == false);

	// --- Good Ext.type value --- //

	ext.type = -1;
	ext.data = std::vector<uint8_t>();
	CHECK(extensionTimestampCheck(object) == false);

	ext.data = std::vector<uint8_t>();
	CHECK(extensionTimestampCheck(object) == false);

	ext.data = std::vector<uint8_t>(1, 0);
	CHECK(extensionTimestampCheck(object) == false);

	ext.data = std::vector<uint8_t>(4, 0);
	CHECK(extensionTimestampCheck(object) == true);

	ext.data = std::vector<uint8_t>(8, 0);
	CHECK(extensionTimestampCheck(object) == true);

	ext.data = std::vector<uint8_t>(12, 0);
	CHECK(extensionTimestampCheck(object) == true);
}
#endif // }}}


/**
 * \brief Convert MessagePack Timestamp extension into a `struct timespec`.
 *
 * Conversion from Object to `struct timespec` is performed by this method. If 
 * the provided \p object is not a valid Timestamp extension then an empty 
 * `struct timespec` will be returned.
 *
 * \parcode
 * std::vector<uint8_t> data = getSerializedData();
 * zakero::messagepack::Object obj = zakero::messagepack::deserialize(data);
 * struct timespec ts = zakero::messagepack::extensionTimestampConvert(obj);
 *
 * char buf[128];
 * std::strftime(buf, sizeof(buf), "%D %T", std::gmtime(&ts.tv_sec));
 * std::printf("Time: %s.%09ld UTC\n", buf, ts.tv_nsec);
 * \endparcode
 *
 * \return A `struct timespec`.
 */
struct timespec extensionTimestampConvert(const Object& object ///< The Ext data.
	) noexcept
{
	if(object.isExt() == false)
	{
		return {};
	}

	const Ext& ext = object.asExt();

	if(ext.type != -1)
	{
		return {};
	}

	switch(ext.data.size())
	{
		case 4:
		{
			Convert.uint64 = 0;
			Convert_Byte3 = ext.data[0];
			Convert_Byte2 = ext.data[1];
			Convert_Byte1 = ext.data[2];
			Convert_Byte0 = ext.data[3];

			timespec ts =
			{	.tv_sec  = Convert.uint32
			,	.tv_nsec = 0
			};

			return ts;
		}

		case 8:
		{
			Convert.uint64 = 0;
			Convert_Byte3 = ext.data[0];
			Convert_Byte2 = ext.data[1];
			Convert_Byte1 = ext.data[2];
			Convert_Byte0 = ext.data[3];
			const uint32_t nsec = Convert.uint32 >> 2;

			Convert.uint64 = 0;
			Convert_Byte4 = ext.data[3];
			Convert_Byte3 = ext.data[4];
			Convert_Byte2 = ext.data[5];
			Convert_Byte1 = ext.data[6];
			Convert_Byte0 = ext.data[7];
			const int64_t sec = Convert.int64 & 0x0000'0003'ffff'ffff;

			timespec ts =
			{	.tv_sec  = sec
			,	.tv_nsec = nsec
			};

			return ts;
		}

		case 12:
		{
			Convert.uint64 = 0;
			Convert_Byte3 = ext.data[0];
			Convert_Byte2 = ext.data[1];
			Convert_Byte1 = ext.data[2];
			Convert_Byte0 = ext.data[3];
			const uint32_t nsec = Convert.uint32;

			Convert.uint64 = 0;
			Convert_Byte7 = ext.data[ 4];
			Convert_Byte6 = ext.data[ 5];
			Convert_Byte5 = ext.data[ 6];
			Convert_Byte4 = ext.data[ 7];
			Convert_Byte3 = ext.data[ 8];
			Convert_Byte2 = ext.data[ 9];
			Convert_Byte1 = ext.data[10];
			Convert_Byte0 = ext.data[11];
			const int64_t sec = Convert.int64;

			timespec ts =
			{	.tv_sec  = sec
			,	.tv_nsec = nsec
			};

			return ts;
		}
	}

	return {};
}


// Test is below


/**
 * \brief Convert a `struct timespec` into a MessagePack Timestamp extension.
 *
 * Conversion from a `struct timespec` into the MessagePack Timestamp extension 
 * is handled by this method.
 *
 * \parcode
 * std::timespec ts;
 * std::timespec_get(&ts, TIME_UTC);
 *
 * zakero::messagepack::Object object =
 * 	zakero::messagepack::extensionTimestampConvert(ts);
 *
 * std::vector<uint8_t> data = zakero::messagepack::serialize(object);
 * \endparcode
 *
 * \return A MessagePack Object.
 */
Object extensionTimestampConvert(const struct timespec& ts ///< The time data
	) noexcept
{
	Object object = {Ext{}};
	Ext&   ext    = object.asExt();

	if((ts.tv_sec >> 34) == 0)
	{
		Convert.uint64 = (ts.tv_nsec << 34) | ts.tv_sec;
		if((Convert.uint64 & 0xffff'ffff'0000'0000) == 0)
		{
			ext.type = -1;
			ext.data.resize(4);

			size_t index = 0;
			ext.data[index++] = Convert_Byte3;
			ext.data[index++] = Convert_Byte2;
			ext.data[index++] = Convert_Byte1;
			ext.data[index++] = Convert_Byte0;

			return object;
		}

		ext.type = -1;
		ext.data.resize(8);

		size_t index = 0;
		ext.data[index++] = Convert_Byte7;
		ext.data[index++] = Convert_Byte6;
		ext.data[index++] = Convert_Byte5;
		ext.data[index++] = Convert_Byte4;
		ext.data[index++] = Convert_Byte3;
		ext.data[index++] = Convert_Byte2;
		ext.data[index++] = Convert_Byte1;
		ext.data[index++] = Convert_Byte0;

		return object;
	}

	ext.type = -1;
	ext.data.resize(12);

	size_t index = 0;
	Convert.uint64 = ts.tv_nsec;
	ext.data[index++] = Convert_Byte3;
	ext.data[index++] = Convert_Byte2;
	ext.data[index++] = Convert_Byte1;
	ext.data[index++] = Convert_Byte0;

	Convert.uint64 = ts.tv_sec;
	ext.data[index++] = Convert_Byte7;
	ext.data[index++] = Convert_Byte6;
	ext.data[index++] = Convert_Byte5;
	ext.data[index++] = Convert_Byte4;
	ext.data[index++] = Convert_Byte3;
	ext.data[index++] = Convert_Byte2;
	ext.data[index++] = Convert_Byte1;
	ext.data[index++] = Convert_Byte0;

	return object;
}

#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("extension/timestamp/convert/32bit")
{
	SUBCASE("min")
	{
		const uint64_t sec  = 0;
		const uint32_t nsec = 0;

		struct timespec time =
		{	.tv_sec  = sec
		,	.tv_nsec = nsec
		};

		Object object = extensionTimestampConvert(time);
		{
			Ext& ext = object.asExt();
			CHECK(ext.type        == -1);
			CHECK(ext.data.size() == 4);
		}

		std::vector<uint8_t> data = serialize(object);
		CHECK(data.size() == 6);
		size_t index = 0;
		CHECK(data[index++] == (uint8_t)Format::Fixed_Ext4);
		CHECK(data[index++] == (uint8_t)-1);
		CHECK(data[index++] == 0);
		CHECK(data[index++] == 0);
		CHECK(data[index++] == 0);
		CHECK(data[index++] == 0);

		object = deserialize(data);
		CHECK(object.isExt() == true);

		Ext& ext = object.asExt();
		CHECK(ext.type        == -1);
		CHECK(ext.data.size() == 4);

		time = extensionTimestampConvert(object);
		CHECK(time.tv_sec  == sec);
		CHECK(time.tv_nsec == nsec);
	}

	SUBCASE("max")
	{
		const uint64_t sec  = 0x0000'0000'ffff'ffff;
		const uint32_t nsec = 0;

		struct timespec time =
		{	.tv_sec  = sec
		,	.tv_nsec = nsec
		};

		Object object = extensionTimestampConvert(time);
		{
			Ext& ext = object.asExt();
			CHECK(ext.type        == -1);
			CHECK(ext.data.size() == 4);
		}

		std::vector<uint8_t> data = serialize(object);
		CHECK(data.size() == 6);
		size_t index = 0;
		CHECK(data[index++] == (uint8_t)Format::Fixed_Ext4);
		CHECK(data[index++] == (uint8_t)-1);
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);

		object = deserialize(data);
		CHECK(object.isExt() == true);

		Ext& ext = object.asExt();
		CHECK(ext.type        == -1);
		CHECK(ext.data.size() == 4);

		time = extensionTimestampConvert(object);
		CHECK(time.tv_sec  == sec);
		CHECK(time.tv_nsec == nsec);
	}

	SUBCASE("pattern")
	{
		const uint64_t sec  = 0x12345678;
		const uint32_t nsec = 0;

		struct timespec time =
		{	.tv_sec  = sec
		,	.tv_nsec = nsec
		};

		Object object = extensionTimestampConvert(time);
		{
			Ext& ext = object.asExt();
			CHECK(ext.type        == -1);
			CHECK(ext.data.size() == 4);
		}

		std::vector<uint8_t> data = serialize(object);
		CHECK(data.size() == 6);
		size_t index = 0;
		CHECK(data[index++] == (uint8_t)Format::Fixed_Ext4);
		CHECK(data[index++] == (uint8_t)-1);
		CHECK(data[index++] == 0x12);
		CHECK(data[index++] == 0x34);
		CHECK(data[index++] == 0x56);
		CHECK(data[index++] == 0x78);

		object = deserialize(data);
		CHECK(object.isExt() == true);

		Ext& ext = object.asExt();
		CHECK(ext.type        == -1);
		CHECK(ext.data.size() == 4);

		time = extensionTimestampConvert(object);
		CHECK(time.tv_sec  == sec);
		CHECK(time.tv_nsec == nsec);
	}
}

TEST_CASE("extension/timestamp/convert/64bit")
{
	SUBCASE("min")
	{
		const uint64_t sec  = 0x0000'0002'0000'0000;
		const uint32_t nsec = 0;

		struct timespec time =
		{	.tv_sec  = sec
		,	.tv_nsec = nsec
		};

		Object object = extensionTimestampConvert(time);
		{
			Ext& ext = object.asExt();
			CHECK(ext.type        == -1);
			CHECK(ext.data.size() == 8);
		}

		std::vector<uint8_t> data = serialize(object);
		CHECK(data.size() == 10);
		size_t index = 0;
		CHECK(data[index++] == (uint8_t)Format::Fixed_Ext8);
		CHECK(data[index++] == (uint8_t)-1);
		CHECK(data[index++] == 0x00);
		CHECK(data[index++] == 0x00);
		CHECK(data[index++] == 0x00);
		CHECK(data[index++] == 0x02);
		CHECK(data[index++] == 0x00);
		CHECK(data[index++] == 0x00);
		CHECK(data[index++] == 0x00);
		CHECK(data[index++] == 0x00);

		object = deserialize(data);
		CHECK(object.isExt() == true);

		Ext& ext = object.asExt();
		CHECK(ext.type        == -1);
		CHECK(ext.data.size() == 8);

		time = extensionTimestampConvert(object);
		CHECK(time.tv_sec  == sec);
		CHECK(time.tv_nsec == nsec);
	}

	SUBCASE("max")
	{
		const uint64_t sec  = 0x0000'0003'ffff'ffff;
		const uint32_t nsec = 0x3fff'ffff;

		struct timespec time =
		{	.tv_sec  = sec
		,	.tv_nsec = nsec
		};

		Object object = extensionTimestampConvert(time);
		{
			Ext& ext = object.asExt();
			CHECK(ext.type        == -1);
			CHECK(ext.data.size() == 8);
		}

		std::vector<uint8_t> data = serialize(object);
		CHECK(data.size() == 10);
		size_t index = 0;
		CHECK(data[index++] == (uint8_t)Format::Fixed_Ext8);
		CHECK(data[index++] == (uint8_t)-1);
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);

		object = deserialize(data);
		CHECK(object.isExt() == true);

		Ext& ext = object.asExt();
		CHECK(ext.type        == -1);
		CHECK(ext.data.size() == 8);

		time = extensionTimestampConvert(object);
		CHECK(time.tv_sec  == sec);
		CHECK(time.tv_nsec == nsec);
	}

	SUBCASE("pattern")
	{
		const uint64_t sec  = 0x0000'0001'2345'6789;
		const uint32_t nsec = 0x0fed'cba9;
		// 0x0fed'cba9
		// 0b0000'1111'1110'1101'1100'1011'1010'1001
		//                                      << 2
		// 0b0011'1111'1011'0111'0010'1110'1010'0100
		// 0x3fb7'2ea4

		struct timespec time =
		{	.tv_sec  = sec
		,	.tv_nsec = nsec
		};

		Object object = extensionTimestampConvert(time);
		{
			Ext& ext = object.asExt();
			CHECK(ext.type        == -1);
			CHECK(ext.data.size() == 8);
		}

		std::vector<uint8_t> data = serialize(object);
		CHECK(data.size() == 10);
		size_t index = 0;
		CHECK(data[index++] == (uint8_t)Format::Fixed_Ext8);
		CHECK(data[index++] == (uint8_t)-1);
		CHECK(data[index++] == 0x3f);
		CHECK(data[index++] == 0xb7);
		CHECK(data[index++] == 0x2e);
		CHECK(data[index++] == 0xa5); // 0xa4 | 0x01
		CHECK(data[index++] == 0x23);
		CHECK(data[index++] == 0x45);
		CHECK(data[index++] == 0x67);
		CHECK(data[index++] == 0x89);

		object = deserialize(data);
		CHECK(object.isExt() == true);

		Ext& ext = object.asExt();
		CHECK(ext.type        == -1);
		CHECK(ext.data.size() == 8);

		time = extensionTimestampConvert(object);
		CHECK(time.tv_sec  == sec);
		CHECK(time.tv_nsec == nsec);
	}
}

TEST_CASE("extension/timestamp/convert/96bit")
{
	SUBCASE("min")
	{
		const int64_t  sec  = 0x0000'0004'0000'0000;
		const uint64_t nsec = 0x0000'0000;

		struct timespec time =
		{	.tv_sec  = sec
		,	.tv_nsec = nsec
		};

		Object object = extensionTimestampConvert(time);
		{
			Ext& ext = object.asExt();
			CHECK(ext.type        == -1);
			CHECK(ext.data.size() == 12);
		}

		std::vector<uint8_t> data = serialize(object);
		CHECK(data.size() == 15);
		size_t index = 0;
		CHECK(data[index++] == (uint8_t)Format::Ext8);
		CHECK(data[index++] == 12);
		CHECK(data[index++] == (uint8_t)-1);
		// nsec
		CHECK(data[index++] == 0x00);
		CHECK(data[index++] == 0x00);
		CHECK(data[index++] == 0x00);
		CHECK(data[index++] == 0x00);
		// sec
		CHECK(data[index++] == 0x00);
		CHECK(data[index++] == 0x00);
		CHECK(data[index++] == 0x00);
		CHECK(data[index++] == 0x04);
		CHECK(data[index++] == 0x00);
		CHECK(data[index++] == 0x00);
		CHECK(data[index++] == 0x00);
		CHECK(data[index++] == 0x00);

		object = deserialize(data);
		CHECK(object.isExt() == true);

		Ext& ext = object.asExt();
		CHECK(ext.type        == -1);
		CHECK(ext.data.size() == 12);

		time = extensionTimestampConvert(object);
		CHECK(time.tv_sec  == sec);
		CHECK(time.tv_nsec == nsec);
	}

	SUBCASE("max")
	{
		const int64_t  sec  = 0x7fff'ffff'ffff'ffff;
		const uint64_t nsec = 0xffff'ffff;

		struct timespec time =
		{	.tv_sec  = sec
		,	.tv_nsec = nsec
		};

		Object object = extensionTimestampConvert(time);
		{
			Ext& ext = object.asExt();
			CHECK(ext.type        == -1);
			CHECK(ext.data.size() == 12);
		}

		std::vector<uint8_t> data = serialize(object);
		CHECK(data.size() == 15);
		size_t index = 0;
		CHECK(data[index++] == (uint8_t)Format::Ext8);
		CHECK(data[index++] == 12);
		CHECK(data[index++] == (uint8_t)-1);
		// nsec
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);
		// sec
		CHECK(data[index++] == 0x7f);
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);
		CHECK(data[index++] == 0xff);

		object = deserialize(data);
		CHECK(object.isExt() == true);

		Ext& ext = object.asExt();
		CHECK(ext.type        == -1);
		CHECK(ext.data.size() == 12);

		time = extensionTimestampConvert(object);
		CHECK(time.tv_sec  == sec);
		CHECK(time.tv_nsec == nsec);
	}

	SUBCASE("pattern")
	{
		const int64_t  sec  = 0x1234'5678'9abc'def0;
		const uint64_t nsec = 0xfedc'ba98;

		struct timespec time =
		{	.tv_sec  = sec
		,	.tv_nsec = nsec
		};

		Object object = extensionTimestampConvert(time);
		{
			Ext& ext = object.asExt();
			CHECK(ext.type        == -1);
			CHECK(ext.data.size() == 12);
		}

		std::vector<uint8_t> data = serialize(object);
		CHECK(data.size() == 15);
		size_t index = 0;
		CHECK(data[index++] == (uint8_t)Format::Ext8);
		CHECK(data[index++] == 12);
		CHECK(data[index++] == (uint8_t)-1);
		// nsec
		CHECK(data[index++] == 0xfe);
		CHECK(data[index++] == 0xdc);
		CHECK(data[index++] == 0xba);
		CHECK(data[index++] == 0x98);
		// sec
		CHECK(data[index++] == 0x12);
		CHECK(data[index++] == 0x34);
		CHECK(data[index++] == 0x56);
		CHECK(data[index++] == 0x78);
		CHECK(data[index++] == 0x9a);
		CHECK(data[index++] == 0xbc);
		CHECK(data[index++] == 0xde);
		CHECK(data[index++] == 0xf0);

		object = deserialize(data);
		CHECK(object.isExt() == true);

		Ext& ext = object.asExt();
		CHECK(ext.type        == -1);
		CHECK(ext.data.size() == 12);

		time = extensionTimestampConvert(object);
		CHECK(time.tv_sec  == sec);
		CHECK(time.tv_nsec == nsec);
	}
}
#endif // }}}
// }}} Extensions: Timestamp
// }}} Extensions
// {{{ Utilities
// {{{ Utilities::deserialize

/**
 * \brief Deserialize MessagePack data.
 *
 * The packed vector of \p data will be converted into an object that can be 
 * queried and used.
 *
 * \parcode
 * std::vector<uint8_t> command_result = get_reply(command_id);
 *
 * zakero::messagepack::Object object;
 * object = zakero::messagepack::deserialize(command_result);
 * if(object.isArray() == false)
 * {
 * 	writeError(ERROR_INVALID_COMMAND_RESULT);
 * 	return;
 * }
 *
 * zakero::messagepack::Array& array = object.asArray();
 *
 * constexpr size_t error_index = 1;
 * constexpr size_t error_code_index = 2;
 * if(array(error_index).boolean == true)
 * {
 * 	writeError(array(error_code_index).int64_);
 * }
 * \endparcode
 *
 * \todo Add error codes.
 *       - Error_None
 *       - Error_Bad_Format_Type
 *       - Error_Incomplete
 */
Object deserialize(const std::vector<uint8_t>& data ///< The packed data
	) noexcept
{
	size_t index = 0;

	return deserialize(data, index);
}


/**
 * \brief Deserialize MessagePack data.
 *
 * The packed vector of \p data will be converted into an object that can be 
 * queried and used.
 *
 * \parcode
 * std::vector<uint8_t> command_result = get_reply(command_id);
 *
 * zakero::messagepack::Object object;
 * size_t index;
 * object = zakero::messagepack::deserialize(command_result, index);
 * if(object.isArray() == false)
 * {
 *	writeError(ERROR_INVALID_COMMAND_RESULT);
 *
 * 	// index points to the end of the "object"
 * 	crashAnalysis(&command_result[index]);
 *
 * 	return;
 * }
 *
 * zakero::messagepack::Array& array = object.asArray();
 *
 * constexpr size_t error_index = 1;
 * constexpr size_t error_code_index = 2;
 * if(array(error_index).boolean == true)
 * {
 * 	writeError(array(error_code_index).int64_);
 * }
 * \endparcode
 *
 * \todo Add error codes.
 *       - Error_None
 *       - Error_Bad_Format_Type
 *       - Error_Incomplete
 */
Object deserialize(const std::vector<uint8_t>& data  ///< The packed data
	, size_t&                              index ///< The starting index
	) noexcept
{
	const uint8_t byte = data[index++];

	switch((Format)byte)
	{
		case Format::Nill:
			return Object{};

		case Format::False:
			return Object{false};

		case Format::True:
			return Object{true};

		case Format::Int8:
			Convert.int64 = (int8_t)data[index++];
			return Object{Convert.int64};
	
		case Format::Int16:
			Convert.uint64 = 0;
			Convert_Byte1 = data[index++];
			Convert_Byte0 = data[index++];
			return Object{Convert.int16};

		case Format::Int32:
			Convert.uint64 = 0;
			Convert_Byte3 = data[index++];
			Convert_Byte2 = data[index++];
			Convert_Byte1 = data[index++];
			Convert_Byte0 = data[index++];
			return Object{Convert.int32};
	
		case Format::Int64:
			Convert_Byte7 = data[index++];
			Convert_Byte6 = data[index++];
			Convert_Byte5 = data[index++];
			Convert_Byte4 = data[index++];
			Convert_Byte3 = data[index++];
			Convert_Byte2 = data[index++];
			Convert_Byte1 = data[index++];
			Convert_Byte0 = data[index++];
			return Object{Convert.int64};
	
		case Format::Uint8:
			Convert.uint64 = data[index++];
			return Object{Convert.uint64};
	
		case Format::Uint16:
			Convert.uint64 = 0;
			Convert_Byte1 = data[index++];
			Convert_Byte0 = data[index++];
			return Object{Convert.uint64};
	
		case Format::Uint32:
			Convert.uint64 = 0;
			Convert_Byte3 = data[index++];
			Convert_Byte2 = data[index++];
			Convert_Byte1 = data[index++];
			Convert_Byte0 = data[index++];
			return Object{Convert.uint64};

		case Format::Uint64:
			Convert_Byte7 = data[index++];
			Convert_Byte6 = data[index++];
			Convert_Byte5 = data[index++];
			Convert_Byte4 = data[index++];
			Convert_Byte3 = data[index++];
			Convert_Byte2 = data[index++];
			Convert_Byte1 = data[index++];
			Convert_Byte0 = data[index++];
			return Object{Convert.uint64};
	
		case Format::Float32:
			Convert.uint64 = 0;
			Convert_Byte3 = data[index++];
			Convert_Byte2 = data[index++];
			Convert_Byte1 = data[index++];
			Convert_Byte0 = data[index++];
			return Object{Convert.float32};
	
		case Format::Float64:
			Convert_Byte7 = data[index++];
			Convert_Byte6 = data[index++];
			Convert_Byte5 = data[index++];
			Convert_Byte4 = data[index++];
			Convert_Byte3 = data[index++];
			Convert_Byte2 = data[index++];
			Convert_Byte1 = data[index++];
			Convert_Byte0 = data[index++];
			return Object{Convert.float64};
	
		case Format::Str8:
		{
			const size_t length = data[index++];

			std::string_view str((char*)&data[index], length);

			index += length;

			return Object{std::string(str)};
		}

		case Format::Str16:
		{
			Convert.uint64 = 0;
			Convert_Byte1 = data[index++];
			Convert_Byte0 = data[index++];
			const size_t length = Convert.uint64;

			const std::string_view str((char*)&data[index], length);

			index += length;

			return Object{std::string(str)};
		}
	
		case Format::Str32:
		{
			Convert.uint64 = 0;
			Convert_Byte3 = data[index++];
			Convert_Byte2 = data[index++];
			Convert_Byte1 = data[index++];
			Convert_Byte0 = data[index++];
			const size_t length = Convert.uint64;

			const std::string_view str((char*)&data[index], length);

			index += length;

			return Object{std::string(str)};
		}
	
		case Format::Bin8:
		{
			const size_t length = data[index++];

			std::vector<uint8_t> vector(length);
			memcpy((void*)vector.data(), (void*)&data[index], length);

			index += length;

			return Object{std::move(vector)};
		}
	
		case Format::Bin16:
		{
			Convert.uint64 = 0;
			Convert_Byte1 = data[index++];
			Convert_Byte0 = data[index++];
			const size_t length = Convert.uint64;

			std::vector<uint8_t> vector(length);
			memcpy((void*)vector.data(), (void*)&data[index], length);

			index += length;

			return Object{std::move(vector)};
		}
	
		case Format::Bin32:
		{
			Convert.uint64 = 0;
			Convert_Byte3 = data[index++];
			Convert_Byte2 = data[index++];
			Convert_Byte1 = data[index++];
			Convert_Byte0 = data[index++];
			const size_t length = Convert.uint64;

			std::vector<uint8_t> vector(length);
			memcpy((void*)vector.data(), (void*)&data[index], length);

			index += length;

			return Object{std::move(vector)};
		}

		case Format::Array16:
		{
			Object object = {Array{}};

			Convert.uint64 = 0;
			Convert_Byte1 = data[index++];
			Convert_Byte0 = data[index++];
			const size_t count = Convert.uint64;

			for(size_t i = 0; i < count; i++)
			{
				object.asArray().append(deserialize(data, index));
			}

			return object;
		}
	
		case Format::Array32:
		{
			Object object = {Array{}};

			Convert.uint64 = 0;
			Convert_Byte3 = data[index++];
			Convert_Byte2 = data[index++];
			Convert_Byte1 = data[index++];
			Convert_Byte0 = data[index++];
			const size_t count = Convert.uint64;

			for(size_t i = 0; i < count; i++)
			{
				object.asArray().append(deserialize(data, index));
			}

			return object;
		}

		case Format::Map16:
		{
			Object object = {Map{}};

			Convert.uint64 = 0;
			Convert_Byte1 = data[index++];
			Convert_Byte0 = data[index++];
			const size_t count = Convert.uint64;

			for(size_t i = 0; i < count; i++)
			{
				Object key = deserialize(data, index);
				Object val = deserialize(data, index);

				object.asMap().set(std::move(key), std::move(val));
			}

			return object;
		}

		case Format::Map32:
		{
			Object object = {Map{}};

			Convert.uint64 = 0;
			Convert_Byte3 = data[index++];
			Convert_Byte2 = data[index++];
			Convert_Byte1 = data[index++];
			Convert_Byte0 = data[index++];
			const size_t count = Convert.uint64;

			for(size_t i = 0; i < count; i++)
			{
				Object key = deserialize(data, index);
				Object val = deserialize(data, index);

				object.asMap().set(std::move(key), std::move(val));
			}

			return object;
		}

		case Format::Fixed_Ext1:
		{
			Object object = {Ext{}};
			Ext& ext = object.asExt();

			Convert.uint64 = 0;
			Convert.uint8  = data[index++];
			ext.type = Convert.int8;

			ext.data.push_back(data[index++]);

			return object;
		}

		case Format::Fixed_Ext2:
		{
			Object object = {Ext{}};
			Ext& ext = object.asExt();

			Convert.uint64 = 0;
			Convert.uint8  = data[index++];
			ext.type = Convert.int8;

			ext.data.push_back(data[index++]);
			ext.data.push_back(data[index++]);

			return object;
		}

		case Format::Fixed_Ext4:
		{
			Object object = {Ext{}};
			Ext& ext = object.asExt();

			Convert.uint64 = 0;
			Convert.uint8  = data[index++];
			ext.type = Convert.int8;

			ext.data.push_back(data[index++]);
			ext.data.push_back(data[index++]);
			ext.data.push_back(data[index++]);
			ext.data.push_back(data[index++]);

			return object;
		}

		case Format::Fixed_Ext8:
		{
			Object object = {Ext{}};
			Ext& ext = object.asExt();

			Convert.uint64 = 0;
			Convert.uint8  = data[index++];
			ext.type = Convert.int8;

			const size_t data_size = 8;
			ext.data.reserve(data_size);

			for(size_t i = 0; i < data_size; i++)
			{
				ext.data.push_back(data[index++]);
			}

			return object;
		}

		case Format::Fixed_Ext16:
		{
			Object object = {Ext{}};
			Ext& ext = object.asExt();

			Convert.uint64 = 0;
			Convert.uint8  = data[index++];
			ext.type = Convert.int8;

			const size_t data_size = 16;
			ext.data.reserve(data_size);

			for(size_t i = 0; i < data_size; i++)
			{
				ext.data.push_back(data[index++]);
			}

			return object;
		}

		case Format::Ext8:
		{
			Object object = {Ext{}};
			Ext& ext = object.asExt();

			const size_t data_size = data[index++];

			Convert.uint64 = 0;
			Convert.uint8  = data[index++];
			ext.type = Convert.int8;

			ext.data.resize(data_size);
			memcpy((void*)ext.data.data(), (void*)&data[index], data_size);

			// If data_size == zero, skip the place-holder byte.
			// If data_size >  zero, move to next byte.
			index += data_size + 1;

			return object;
		}

		case Format::Ext16:
		{
			Object object = {Ext{}};
			Ext& ext = object.asExt();

			Convert.uint64 = 0;
			Convert_Byte1 = data[index++];
			Convert_Byte0 = data[index++];
			const size_t data_size = Convert.uint16;

			Convert.uint64 = 0;
			Convert.uint8  = data[index++];
			ext.type = Convert.int8;

			ext.data.resize(data_size);
			memcpy((void*)ext.data.data(), (void*)&data[index], data_size);

			index += data_size + 1;

			return object;
		}

		case Format::Ext32:
		{
			Object object = {Ext{}};
			Ext& ext = object.asExt();

			Convert.uint64 = 0;
			Convert_Byte3 = data[index++];
			Convert_Byte2 = data[index++];
			Convert_Byte1 = data[index++];
			Convert_Byte0 = data[index++];
			const size_t data_size = Convert.uint32;

			Convert.uint64 = 0;
			Convert.uint8  = data[index++];
			ext.type = Convert.int8;

			ext.data.resize(data_size);
			memcpy((void*)ext.data.data(), (void*)&data[index], data_size);

			index += data_size + 1;

			return object;
		}

		case Format::Fixed_Int_Pos:
		case Format::Fixed_Int_Neg:
		case Format::Fixed_Array:
		case Format::Fixed_Str:
		case Format::Fixed_Map:
			// Handled outside of this swith() statement
			break;

		case Format::never_used:
			break;
	}

	if((byte & (uint8_t)Fixed_Int_Pos_Mask) == (uint8_t)Format::Fixed_Int_Pos)
	{
		const int64_t value = byte & (uint8_t)Fixed_Int_Pos_Value;

		return Object{value};
	}

	if((byte & (uint8_t)Fixed_Int_Neg_Mask) == (uint8_t)Format::Fixed_Int_Neg)
	{
		const int64_t value = (int8_t)(byte & Fixed_Int_Neg_Value) - 32;

		return Object{value};
	}
	
	if((byte & (uint8_t)Fixed_Str_Mask) == (uint8_t)Format::Fixed_Str)
	{
		const size_t length = byte & Fixed_Str_Value;

		if(length == 0)
		{
			return Object{""};
		}
		else
		{
			std::string_view str((char*)&data[index], length);

			index += length;

			return Object{std::string(str)};
		}
	}
	
	if((byte & (uint8_t)Fixed_Array_Mask) == (uint8_t)Format::Fixed_Array)
	{
		Object object = {Array{}};

		const size_t count = byte & (uint8_t)Fixed_Array_Value;

		for(size_t i = 0; i < count; i++)
		{
			object.asArray().append(deserialize(data, index));
		}

		return object;
	}
	
	if((byte & (uint8_t)Fixed_Map_Mask) == (uint8_t)Format::Fixed_Map)
	{
		Object object = {Map{}};

		const size_t count = byte & (uint8_t)Fixed_Map_Value;

		for(size_t i = 0; i < count; i++)
		{
			Object key = deserialize(data, index);
			Object val = deserialize(data, index);

			object.asMap().set(std::move(key), std::move(val));
		}

		return object;
	}
	
	return {};
}

// }}} Utilities::deserialize
// {{{ Utilities::serialize

/**
 * \brief Serialize Array data.
 *
 * The contents of the Array will be packed into the returned std::vector.
 *
 * \parcode
 * zakero::messagepack::Array array;
 * array.append(command_id);
 * array.append(true);
 * array.append(error_code);
 *
 * std::vector<uint8_t> result = array.serialize();
 *
 * reply(host_ip, result);
 * \endparcode
 *
 * \return The packed data.
 */
std::vector<uint8_t> serialize(const Array& array
	) noexcept
{
	std::vector<uint8_t> vector;

	serialize_(array, vector);

	return vector;
}


/**
 * \brief Serialize Ext data.
 *
 * The contents of the Ext will be packed into the returned std::vector.
 *
 * \parcode
 * zakero::messagepack::Ext ext;
 * ext.type = 42;
 * ext.data = std::vector<uint8_t>(16, '_');
 *
 * std::vector<uint8_t> result = zakero::messagepack::serialize(ext);
 * \endparcode
 *
 * \return The packed data.
 */
std::vector<uint8_t> serialize(const Ext& ext
	) noexcept
{
	std::vector<uint8_t> vector;

	serialize_(ext, vector);

	return vector;
}

#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("serialize/ext (fixed_ext1)")
{
	Ext ext;
	std::vector<uint8_t> data;
	const size_t data_len = 1;
	const int8_t type = 42;

	ext.type = type;
	ext.data = std::vector<uint8_t>(data_len, '_');

	data = serialize(ext);
	CHECK(data.size() == 3);
	size_t index = 0;
	CHECK(data[index++] == (uint8_t)Format::Fixed_Ext1);
	CHECK(data[index++] == type);
	CHECK(data[index++] == '_');

	Object object = deserialize(data);
	CHECK(object.isExt() == true);

	CHECK(object.asExt().type        == type);
	CHECK(object.asExt().data.size() == 1);
	CHECK(object.asExt().data[0] == '_');
}

TEST_CASE("serialize/ext (fixed_ext2)")
{
	Ext ext;
	std::vector<uint8_t> data;
	const size_t data_len = 2;
	const int8_t type = 42;

	ext.type = type;
	ext.data = std::vector<uint8_t>(data_len, '_');

	data = serialize(ext);
	CHECK(data.size() == 4);
	size_t index = 0;
	CHECK(data[index++] == (uint8_t)Format::Fixed_Ext2);
	CHECK(data[index++] == type);

	Object object = deserialize(data);
	CHECK(object.isExt() == true);

	CHECK(object.asExt().type        == type);
	CHECK(object.asExt().data.size() == data_len);

	for(size_t i = 0; i < data_len; i++)
	{
		CHECK(object.asExt().data[i] == '_');
	}
}

TEST_CASE("serialize/ext (fixed_ext4)")
{
	Ext ext;
	std::vector<uint8_t> data;
	const size_t data_len = 4;
	const int8_t type = 42;

	ext.type = type;
	ext.data = std::vector<uint8_t>(data_len, '_');

	data = serialize(ext);
	CHECK(data.size() == 6);
	size_t index = 0;
	CHECK(data[index++] == (uint8_t)Format::Fixed_Ext4);
	CHECK(data[index++] == type);

	Object object = deserialize(data);
	CHECK(object.isExt() == true);

	CHECK(object.asExt().type        == type);
	CHECK(object.asExt().data.size() == data_len);

	for(size_t i = 0; i < data_len; i++)
	{
		CHECK(object.asExt().data[i] == '_');
	}
}

TEST_CASE("serialize/ext (fixed_ext8)")
{
	Ext ext;
	std::vector<uint8_t> data;
	const size_t data_len = 8;
	const int8_t type = 42;

	ext.type = type;
	ext.data = std::vector<uint8_t>(data_len, '_');

	data = serialize(ext);
	CHECK(data.size() == 10);
	size_t index = 0;
	CHECK(data[index++] == (uint8_t)Format::Fixed_Ext8);
	CHECK(data[index++] == type);

	Object object = deserialize(data);
	CHECK(object.isExt() == true);

	CHECK(object.asExt().type        == type);
	CHECK(object.asExt().data.size() == data_len);

	for(size_t i = 0; i < data_len; i++)
	{
		CHECK(object.asExt().data[i] == '_');
	}
}

TEST_CASE("serialize/ext (fixed_ext16)")
{
	Ext ext;
	std::vector<uint8_t> data;
	const size_t data_len = 16;
	const int8_t type = 42;

	ext.type = type;
	ext.data = std::vector<uint8_t>(data_len, '_');

	data = serialize(ext);
	CHECK(data.size() == 18);
	size_t index = 0;
	CHECK(data[index++] == (uint8_t)Format::Fixed_Ext16);
	CHECK(data[index++] == type);

	Object object = deserialize(data);
	CHECK(object.isExt() == true);

	CHECK(object.asExt().type        == type);
	CHECK(object.asExt().data.size() == data_len);

	for(size_t i = 0; i < data_len; i++)
	{
		CHECK(object.asExt().data[i] == '_');
	}
}

TEST_CASE("serialize/ext (ext8)")
{
	std::vector<uint8_t> data;
	const int8_t type = 42;

	Ext ext;
	ext.type = type;

	SUBCASE("length=0")
	{
		const size_t data_len = 0;
		ext.data = std::vector<uint8_t>(data_len, '_');

		data = serialize(ext);
		CHECK(data.size() == 4);
		size_t index = 0;
		CHECK(data[index++] == (uint8_t)Format::Ext8);
		CHECK(data[index++] == data_len);
		CHECK(data[index++] == type);

		Object object = deserialize(data);
		CHECK(object.isExt() == true);

		CHECK(object.asExt().type        == type);
		CHECK(object.asExt().data.size() == data_len);

		for(size_t i = 0; i < data_len; i++)
		{
			CHECK(object.asExt().data[i] == '_');
		}
	}

	SUBCASE("length=5")
	{
		const size_t data_len = 5;
		ext.data = std::vector<uint8_t>(data_len, '_');

		data = serialize(ext);
		CHECK(data.size() == 8);
		size_t index = 0;
		CHECK(data[index++] == (uint8_t)Format::Ext8);
		CHECK(data[index++] == data_len);
		CHECK(data[index++] == type);

		Object object = deserialize(data);
		CHECK(object.isExt() == true);

		CHECK(object.asExt().type        == type);
		CHECK(object.asExt().data.size() == data_len);

		for(size_t i = 0; i < data_len; i++)
		{
			CHECK(object.asExt().data[i] == '_');
		}
	}

	SUBCASE("length=max")
	{
		const size_t data_len = std::numeric_limits<uint8_t>::max();
		ext.data = std::vector<uint8_t>(data_len, '_');

		data = serialize(ext);
		CHECK(data.size() == 258);
		size_t index = 0;
		CHECK(data[index++] == (uint8_t)Format::Ext8);
		CHECK(data[index++] == data_len);
		CHECK(data[index++] == type);

		Object object = deserialize(data);
		CHECK(object.isExt() == true);

		CHECK(object.asExt().type        == type);
		CHECK(object.asExt().data.size() == data_len);

		for(size_t i = 0; i < data_len; i++)
		{
			CHECK(object.asExt().data[i] == '_');
		}
	}
}

TEST_CASE("serialize/ext (ext16)")
{
	std::vector<uint8_t> data;
	const int8_t type = 42;

	Ext ext;
	ext.type = type;

	SUBCASE("length=min")
	{
		const size_t data_len = std::numeric_limits<uint8_t>::max() + (size_t)1;
		ext.data = std::vector<uint8_t>(data_len, '_');

		data = serialize(ext);
		CHECK(data.size() == (data_len + 4));
		size_t index = 0;
		CHECK(data[index++] == (uint8_t)Format::Ext16);
		Convert.uint64 = 0;
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.uint16 == data_len);
		CHECK(data[index++] == type);

		Object object = deserialize(data);
		CHECK(object.isExt() == true);

		CHECK(object.asExt().type        == type);
		CHECK(object.asExt().data.size() == data_len);

		for(size_t i = 0; i < data_len; i++)
		{
			CHECK(object.asExt().data[i] == '_');
		}
	}

	SUBCASE("length=max")
	{
		const size_t data_len = std::numeric_limits<uint16_t>::max();
		ext.data = std::vector<uint8_t>(data_len, '_');

		data = serialize(ext);
		CHECK(data.size() == (data_len + 4));
		size_t index = 0;
		CHECK(data[index++] == (uint8_t)Format::Ext16);
		Convert.uint64 = 0;
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.uint16 == data_len);
		CHECK(data[index++] == type);

		Object object = deserialize(data);
		CHECK(object.isExt() == true);

		CHECK(object.asExt().type        == type);
		CHECK(object.asExt().data.size() == data_len);

		for(size_t i = 0; i < data_len; i++)
		{
			CHECK(object.asExt().data[i] == '_');
		}
	}
}

TEST_CASE("serialize/ext (ext32)")
{
	std::vector<uint8_t> data;
	const int8_t type = 42;

	Ext ext;
	ext.type = type;

	SUBCASE("length=min")
	{
		const size_t data_len = std::numeric_limits<uint16_t>::max() + (size_t)1;
		ext.data = std::vector<uint8_t>(data_len, '_');

		data = serialize(ext);
		CHECK(data.size() == (data_len + 6));
		size_t index = 0;
		CHECK(data[index++] == (uint8_t)Format::Ext32);
		Convert.uint64 = 0;
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.uint32 == data_len);
		CHECK(data[index++] == type);

		Object object = deserialize(data);
		CHECK(object.isExt() == true);

		CHECK(object.asExt().type        == type);
		CHECK(object.asExt().data.size() == data_len);

		for(size_t i = 0; i < data_len; i++)
		{
			CHECK(object.asExt().data[i] == '_');
		}
	}

#if 0 // Uses too much RAM and takes too long.
	SUBCASE("length=max")
	{
		const size_t data_len = std::numeric_limits<uint32_t>::max();
		ext.data = std::vector<uint8_t>(data_len, '_');

		data = serialize(ext);
		CHECK(data.size() == (data_len + 6));
		size_t index = 0;
		CHECK(data[index++] == (uint8_t)Format::Ext32);
		Convert.uint64 = 0;
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.uint32 == data_len);
		CHECK(data[index++] == type);

		Object object = deserialize(data);
		CHECK(object.isExt() == true);

		CHECK(object.asExt().type        == type);
		CHECK(object.asExt().data.size() == data_len);

		for(size_t i = 0; i < data_len; i++)
		{
			CHECK(object.asExt().data[i] == '_');
		}
	}
#endif
}
#endif // }}}


/**
 * \brief Serialize Map data.
 *
 * The contents of the Map will be packed into the returned std::vector.
 *
 * \parcode
 * zakero::messagepack::Map map;
 * map.set(Object{uint64_t(42)}, Object{true});
 *
 * std::vector<uint8_t> result = zakero::messagepack::serialize(map);
 *
 * reply(host_ip, result);
 * \endparcode
 *
 * \return The packed data.
 */
std::vector<uint8_t> serialize(const Map& map
	) noexcept
{
	std::vector<uint8_t> vector;

	serialize_(map, vector);

	return vector;
}

#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("serialize/map (fixed_map)")
{
	Map map;
	std::vector<uint8_t> data;

	SUBCASE("empty")
	{
		data = serialize(map);
		CHECK(data.size() == 1);
		CHECK((data[0] & Fixed_Map_Mask) == (uint8_t)Format::Fixed_Map);
		CHECK((data[0] & Fixed_Map_Value) == 0);

		Object object = deserialize(data);
		CHECK(object.isMap() == true);
		CHECK(object.asMap().size() == 0);
	}

	SUBCASE("max")
	{
		size_t max = 15;

		for(size_t i = 0; i < max; i++)
		{
			// i          ==> 1 byte
			// str(0-9)   ==> 2 bytes
			// str(10-14) ==> 3 bytes
			map.set(Object{int64_t(i)}, Object{std::to_string(i)});
		}

		data = serialize(map);
		CHECK(data.size() == 51);
		CHECK((data[0] & Fixed_Map_Mask) == (uint8_t)Format::Fixed_Map);
		CHECK((data[0] & Fixed_Map_Value) == max);

		Object object = deserialize(data);
		CHECK(object.isMap() == true);
		CHECK(object.asMap().size() == max);

		for(size_t i = 0; i < max; i++)
		{
			Object key = Object{int64_t(i)};
			CHECK(map.keyExists(key) == true);
			
			Object value = map.valueOf(key);
			CHECK(value.asString() == std::to_string(i));
		}
	}
}


TEST_CASE("serialize/map (map16)")
{
	Map map;
	std::vector<uint8_t> data;

	SUBCASE("min")
	{
		const size_t min = 16;

		for(size_t i = 0; i < min; i++)
		{
			map.set(Object{int64_t(i)}, Object{std::to_string(i)});
		}

		data = serialize(map);
		CHECK(data.size() == 57);
		CHECK(data[0] == (uint8_t)Format::Map16);
		Convert.uint64 = 0;
		Convert_Byte1 = data[1];
		Convert_Byte0 = data[2];
		CHECK(Convert.uint16 == min);

		Object object = deserialize(data);
		CHECK(object.isMap() == true);
		CHECK(object.asMap().size() == min);

		for(size_t i = 0; i < min; i++)
		{
			Object key = Object{int64_t(i)};
			CHECK(map.keyExists(key) == true);
			
			Object value = map.valueOf(key);
			CHECK(value.asString() == std::to_string(i));
		}
	}

#if 0 // Takes a long time
	SUBCASE("max")
	{
		const size_t max = std::numeric_limits<uint16_t>::max();
		map.object_key.reserve(max);
		map.object_value.reserve(max);

		printf("Generating %lu Map entries\n", max);
		for(size_t i = 0; i < max; i++)
		{
			Object key = {int64_t(i)};
			Object val = {std::to_string(i)};
			map.set(std::move(key), std::move(val));

			if(i % 1000 == 0) { printf("%lu/%lu\n", i, max); }
		}

		data = serialize(map);
		CHECK(data.size() == 643986);
		CHECK(data[0] == (uint8_t)Format::Map16);
		Convert.uint64 = 0;
		Convert_Byte1 = data[1];
		Convert_Byte0 = data[2];
		CHECK(Convert.uint16 == max);

		Object object = deserialize(data);
		CHECK(object.isMap() == true);
		CHECK(object.asMap().size() == max);

		for(size_t i = 0; i < max; i++)
		{
			Object key = Object{int64_t(i)};
			CHECK(map.keyExists(key) == true);
			
			Object value = map.valueOf(key);
			CHECK(value.asString() == std::to_string(i));
		}
	}
#endif
}


TEST_CASE("serialize/map (map32)")
{
	Map map;
	std::vector<uint8_t> data;

#if 0 // Takes a long time
	SUBCASE("min")
	{
		const size_t min = std::numeric_limits<uint16_t>::max() + 1;
		map.object_key.reserve(min);
		map.object_value.reserve(min);

		printf("Generating %lu Map entries\n", min);
		for(size_t i = 0; i < min; i++)
		{
			Object key = {int64_t(i)};
			Object val = {std::to_string(i)};
			map.set(std::move(key), std::move(val));

			if(i % 1000 == 0) { printf("%lu/%lu\n", i, min); }
		}

		data = serialize(map);
		CHECK(data.size() == 643999);
		CHECK(data[0] == (uint8_t)Format::Map32);
		Convert.uint64 = 0;
		Convert_Byte3 = data[1];
		Convert_Byte2 = data[2];
		Convert_Byte1 = data[3];
		Convert_Byte0 = data[4];
		CHECK(Convert.uint32 == min);

		Object object = deserialize(data);
		CHECK(object.isMap() == true);
		CHECK(object.asMap().size() == min);

		for(size_t i = 0; i < min; i++)
		{
			Object key = Object{int64_t(i)};
			CHECK(map.keyExists(key) == true);
			
			Object value = map.valueOf(key);
			CHECK(value.asString() == std::to_string(i));
		}
	}
#endif

#if 0 // Will use too much memory and take to long
	SUBCASE("max")
	{
		const size_t max = std::numeric_limits<uint32_t>::max();
		map.object_key.reserve(max);
		map.object_value.reserve(max);

		printf("Generating %lu Map entries\n", max);
		for(size_t i = 0; i < max; i++)
		{
			Object key = {int64_t(i)};
			Object val = {std::to_string(i)};
			map.set(std::move(key), std::move(val));

			if(i % 1000 == 0) { printf("%lu/%lu\n", i, max); }
		}

		data = serialize(map);
		CHECK(data.size() == 0); // Unknown
		CHECK(data[0] == (uint8_t)Format::Map32);
		Convert.uint64 = 0;
		Convert_Byte3 = data[1];
		Convert_Byte2 = data[2];
		Convert_Byte1 = data[3];
		Convert_Byte0 = data[4];
		CHECK(Convert.uint32 == max);

		Object object = deserialize(data);
		CHECK(object.isMap() == true);
		CHECK(object.asMap().size() == max);

		for(size_t i = 0; i < max; i++)
		{
			Object key = Object{int64_t(i)};
			CHECK(map.keyExists(key) == true);
			
			Object value = map.valueOf(key);
			CHECK(value.asString() == std::to_string(i));
		}
	}
#endif
}
#endif // }}}


/**
 * \brief Serialize Object data.
 *
 * The contents of the Object will be packed into the returned std::vector.
 *
 * \parcode
 * zakero::messagepack::Object object = { true };
 *
 * std::vector<uint8_t> result = zakero::messagepack::serialize(object);
 *
 * reply(host_ip, result);
 * \endparcode
 *
 * \return The packed data.
 */
std::vector<uint8_t> serialize(const Object& object
	) noexcept
{
	std::vector<uint8_t> vector;

	serialize_(object, vector);

	return vector;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{

TEST_CASE("serialize/object/nill")
{
	Object object = {};
	CHECK(object.isNull());

	// Check serialized data

	std::vector<uint8_t> data = serialize(object);
	CHECK(data.size() == 1);

	// Check deserialized data

	size_t index = 0;
	CHECK(data[index] == (uint8_t)Format::Nill);

	object = deserialize(data);
	CHECK(object.isNull());
}


TEST_CASE("serialize/object/bool")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index = 0;

	SUBCASE("True")
	{
		object = {true};
		CHECK(object.is<bool>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 1);

		index = 0;
		CHECK(data[index] == (uint8_t)Format::True);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<bool>());
		CHECK(object.as<bool>() == true);
	}

	SUBCASE("False")
	{
		object = {false};
		CHECK(object.is<bool>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 1);
		index = 0;
		CHECK(data[index] == (uint8_t)Format::False);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<bool>());
		CHECK(object.as<bool>() == false);
	}
}


TEST_CASE("serialize/object/fixed_int")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;

	// -------------------------------------------------------------------

	SUBCASE("Zero")
	{
		const int64_t fixint_zero = 0;

		object = {fixint_zero};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 1);

		index = 0;
		CHECK((data[index] & Fixed_Int_Pos_Mask) == (uint8_t)Format::Fixed_Int_Pos);
		CHECK((data[index] & Fixed_Int_Pos_Value) == fixint_zero);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == fixint_zero);
	}

	// -------------------------------------------------------------------

	SUBCASE("Max")
	{
		const int64_t fixint_max = 127;

		object = {fixint_max};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 1);

		index = 0;
		CHECK((data[index] & Fixed_Int_Pos_Mask) == (uint8_t)Format::Fixed_Int_Pos);
		CHECK((data[index] & Fixed_Int_Pos_Value) == fixint_max);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == fixint_max);
	}

	// -------------------------------------------------------------------

	SUBCASE("Min")
	{
		const int64_t fixint_min = -32;

		object = {fixint_min};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 1);

		index = 0;
		CHECK((data[index] & Fixed_Int_Neg_Mask) == (uint8_t)Format::Fixed_Int_Neg);
		CHECK(((data[index] & Fixed_Int_Neg_Value) - 32) == fixint_min);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == fixint_min);
	}

	// -------------------------------------------------------------------

	SUBCASE("Value: 24")
	{
		const int64_t fixint_p24 = 24;

		object = {fixint_p24};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 1);

		index = 0;
		CHECK((data[index] & Fixed_Int_Pos_Mask) == (uint8_t)Format::Fixed_Int_Pos);
		CHECK((data[index] & Fixed_Int_Pos_Value) == fixint_p24);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == fixint_p24);
	}

	// -------------------------------------------------------------------

	SUBCASE("Value: -24")
	{
		const int64_t fixint_n24 = -24;

		object = {fixint_n24};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 1);

		index = 0;
		CHECK((data[index] & Fixed_Int_Neg_Mask) == (uint8_t)Format::Fixed_Int_Neg);
		CHECK(((data[index] & Fixed_Int_Neg_Value) - 32) == fixint_n24);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == fixint_n24);
	}
}


TEST_CASE("serialize/object/int8")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;

	// Don't test the maximum Int8 value 127
	// because that is handled by Fixed_Int_Pos

	// -------------------------------------------------------------------

	SUBCASE("Min")
	{
		const int64_t i8_min = std::numeric_limits<int8_t>::min();

		object = {i8_min};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 2);

		index = 0;
		CHECK(data[index] == (uint8_t)Format::Int8);
		index++;
		CHECK((int8_t)data[index] == i8_min);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == i8_min);
	}

	// -------------------------------------------------------------------

	SUBCASE("Value: -33")
	{
		const int64_t i8_min = -33;

		object = {i8_min};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 2);

		index = 0;
		CHECK(data[index] == (uint8_t)Format::Int8);
		index++;
		CHECK((int8_t)data[index] == i8_min);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == i8_min);
	}
}


TEST_CASE("serialize/object/int16")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;

	// -------------------------------------------------------------------

	SUBCASE("Min16")
	{
		const int64_t i16_min = std::numeric_limits<int16_t>::min();

		object = {i16_min};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 3);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int16);

		Convert.uint64 = 0;
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int16 == i16_min);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == i16_min);
	}

	// -------------------------------------------------------------------

	SUBCASE("Min8-1")
	{
		const int64_t i16_min = std::numeric_limits<int8_t>::min() - (int64_t)1;

		object = {i16_min};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 3);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int16);

		Convert.uint64 = 0;
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int16 == i16_min);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == i16_min);
	}

	// -------------------------------------------------------------------

	SUBCASE("Max16")
	{
		const int64_t i16_max = std::numeric_limits<int16_t>::max();

		object = {i16_max};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 3);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int16);

		Convert.uint64 = 0;
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int16 == i16_max);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == i16_max);
	}

	// -------------------------------------------------------------------

	SUBCASE("Max8+1")
	{
		const int64_t i16_max = std::numeric_limits<int16_t>::max();

		object = {i16_max};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 3);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int16);

		Convert.uint64 = 0;
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int16 == i16_max);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == i16_max);
	}
}


TEST_CASE("serialize/object/int32")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;

	// -------------------------------------------------------------------

	SUBCASE("Min32")
	{
		const int64_t i32_min = std::numeric_limits<int32_t>::min();

		object = {i32_min};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int32);

		Convert.uint64 = 0;
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int32 == i32_min);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == i32_min);
	}

	// -------------------------------------------------------------------

	SUBCASE("Min16-1")
	{
		const int64_t i32_min = std::numeric_limits<int16_t>::min() - (int64_t)1;

		object = {i32_min};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int32);

		Convert.uint64 = 0;
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int32 == i32_min);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == i32_min);
	}

	// -------------------------------------------------------------------

	SUBCASE("Max32")
	{
		const int64_t i32_max = std::numeric_limits<int32_t>::max();

		object = {i32_max};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int32);

		Convert.uint64 = 0;
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int32 == i32_max);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == i32_max);
	}

	// -------------------------------------------------------------------

	SUBCASE("Max16+1")
	{
		const int64_t i32_max = std::numeric_limits<int16_t>::max() + 1;

		object = {i32_max};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int32);

		Convert.uint64 = 0;
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int32 == i32_max);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == i32_max);
	}
}


TEST_CASE("serialize/object/int64")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;

	// -------------------------------------------------------------------

	SUBCASE("Min64")
	{
		const int64_t i64_min = std::numeric_limits<int64_t>::min();

		object = {i64_min};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 9);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int64);

		Convert.uint64 = 0;
		Convert_Byte7 = data[index++];
		Convert_Byte6 = data[index++];
		Convert_Byte5 = data[index++];
		Convert_Byte4 = data[index++];
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int64 == i64_min);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == i64_min);
	}

	// -------------------------------------------------------------------

	SUBCASE("Min32-1")
	{
		const int64_t i64_min = std::numeric_limits<int32_t>::min() - (int64_t)1;

		object = {i64_min};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 9);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int64);

		Convert.uint64 = 0;
		Convert_Byte7 = data[index++];
		Convert_Byte6 = data[index++];
		Convert_Byte5 = data[index++];
		Convert_Byte4 = data[index++];
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int64 == i64_min);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == i64_min);
	}

	// -------------------------------------------------------------------

	SUBCASE("Max64")
	{
		const int64_t i64_max = std::numeric_limits<int64_t>::max();

		object = {i64_max};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 9);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int64);

		Convert.uint64 = 0;
		Convert_Byte7 = data[index++];
		Convert_Byte6 = data[index++];
		Convert_Byte5 = data[index++];
		Convert_Byte4 = data[index++];
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int64 == i64_max);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == i64_max);
	}

	// -------------------------------------------------------------------

	SUBCASE("Max32+1")
	{
		const int64_t i64_max = std::numeric_limits<int32_t>::max() + (int64_t)1;

		object = {i64_max};
		CHECK(object.is<int64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 9);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int64);

		Convert.uint64 = 0;
		Convert_Byte7 = data[index++];
		Convert_Byte6 = data[index++];
		Convert_Byte5 = data[index++];
		Convert_Byte4 = data[index++];
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int64 == i64_max);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == i64_max);
	}
}


TEST_CASE("serialize/object/uint8")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;

	// -------------------------------------------------------------------

	SUBCASE("Min")
	{
		const uint64_t u8_min = 0;

		object = {u8_min};
		CHECK(object.is<uint64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 2);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Uint8);
		CHECK(data[index++] == u8_min);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<uint64_t>());
		CHECK(object.as<uint64_t>() == u8_min);
	}

	// -------------------------------------------------------------------

	SUBCASE("Max")
	{
		const uint64_t u8_max = std::numeric_limits<uint8_t>::max();

		object = {u8_max};
		CHECK(object.is<uint64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 2);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Uint8);
		CHECK(data[index++] == u8_max);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<uint64_t>());
		CHECK(object.as<uint64_t>() == u8_max);
	}
}


TEST_CASE("serialize/object/uint16")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;

	// -------------------------------------------------------------------

	SUBCASE("Min")
	{
		const uint64_t u16_min = std::numeric_limits<uint8_t>::max() + (uint64_t)1;

		object = {u16_min};
		CHECK(object.is<uint64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 3);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Uint16);
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.uint16 == u16_min);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<uint64_t>());
		CHECK(object.as<uint64_t>() == u16_min);
	}

	// -------------------------------------------------------------------

	SUBCASE("Max")
	{
		const uint64_t u16_max = std::numeric_limits<uint16_t>::max();

		object = {u16_max};
		CHECK(object.is<uint64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 3);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Uint16);
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.uint16 == u16_max);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<uint64_t>());
		CHECK(object.as<uint64_t>() == u16_max);
	}
}


TEST_CASE("serialize/object/uint32")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;

	// -------------------------------------------------------------------

	SUBCASE("Min")
	{
		const uint64_t u32_min = std::numeric_limits<uint16_t>::max() + (uint64_t)1;

		object = {u32_min};
		CHECK(object.is<uint64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Uint32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.uint32 == u32_min);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<uint64_t>());
		CHECK(object.as<uint64_t>() == u32_min);
	}

	// -------------------------------------------------------------------

	SUBCASE("Max")
	{
		const uint64_t u32_max = std::numeric_limits<uint32_t>::max();

		object = {u32_max};
		CHECK(object.is<uint64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Uint32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.uint32 == u32_max);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<uint64_t>());
		CHECK(object.as<uint64_t>() == u32_max);
	}
}


TEST_CASE("serialize/object/uint64")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;

	// -------------------------------------------------------------------

	SUBCASE("Min")
	{
		const uint64_t u64_min = std::numeric_limits<uint32_t>::max() + (uint64_t)1;

		object = {u64_min};
		CHECK(object.is<uint64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 9);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Uint64);
		Convert_Byte7 = data[index++];
		Convert_Byte6 = data[index++];
		Convert_Byte5 = data[index++];
		Convert_Byte4 = data[index++];
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.uint64 == u64_min);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<uint64_t>());
		CHECK(object.as<uint64_t>() == u64_min);
	}

	// -------------------------------------------------------------------

	SUBCASE("Max")
	{
		const uint64_t u64_max = std::numeric_limits<uint64_t>::max();

		object = {u64_max};
		CHECK(object.is<uint64_t>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 9);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Uint64);
		Convert_Byte7 = data[index++];
		Convert_Byte6 = data[index++];
		Convert_Byte5 = data[index++];
		Convert_Byte4 = data[index++];
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.uint64 == u64_max);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<uint64_t>());
		CHECK(object.as<uint64_t>() == u64_max);
	}
}


TEST_CASE("serialize/object/float32")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;

	// -------------------------------------------------------------------

	SUBCASE("Zero")
	{
		const float f32_zero = 0;

		object = {f32_zero};
		CHECK(object.is<float>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Float32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.float32 == f32_zero);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<float>());
		CHECK(object.as<float>() == f32_zero);
	}

	// -------------------------------------------------------------------

	SUBCASE("Min")
	{
		const float f32_min = std::numeric_limits<float>::min();

		object = {f32_min};
		CHECK(object.is<float>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Float32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.float32 == f32_min);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<float>());
		CHECK(object.as<float>() == f32_min);
	}

	// -------------------------------------------------------------------

	SUBCASE("Max")
	{
		const float f32_max = std::numeric_limits<float>::max();

		object = {f32_max};
		CHECK(object.is<float>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Float32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.float32 == f32_max);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<float>());
		CHECK(object.as<float>() == f32_max);
	}
}


TEST_CASE("serialize/object/float64")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;

	// -------------------------------------------------------------------

	SUBCASE("Zero")
	{
		const double f64_zero = 0;

		object = {f64_zero};
		CHECK(object.is<double>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 9);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Float64);
		Convert_Byte7 = data[index++];
		Convert_Byte6 = data[index++];
		Convert_Byte5 = data[index++];
		Convert_Byte4 = data[index++];
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.float64 == f64_zero);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<double>());
		CHECK(object.as<double>() == f64_zero);
	}

	// -------------------------------------------------------------------

	SUBCASE("Min")
	{
		const double f64_min = std::numeric_limits<double>::min();

		object = {f64_min};
		CHECK(object.is<double>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 9);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Float64);
		Convert_Byte7 = data[index++];
		Convert_Byte6 = data[index++];
		Convert_Byte5 = data[index++];
		Convert_Byte4 = data[index++];
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.float64 == f64_min);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<double>());
		CHECK(object.as<double>() == f64_min);
	}

	// -------------------------------------------------------------------

	SUBCASE("Max")
	{
		const double f64_max = std::numeric_limits<double>::max();

		object = {f64_max};
		CHECK(object.is<double>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 9);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Float64);
		Convert_Byte7 = data[index++];
		Convert_Byte6 = data[index++];
		Convert_Byte5 = data[index++];
		Convert_Byte4 = data[index++];
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.float64 == f64_max);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<double>());
		CHECK(object.as<double>() == f64_max);
	}
}


TEST_CASE("serialize/object/fixed_str")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;
	size_t               str_len;

	// -------------------------------------------------------------------

	SUBCASE("Empty")
	{
		const std::string string;

		object = {string};
		CHECK(object.is<std::string>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 1);

		index = 0;
		CHECK((data[index] & Fixed_Str_Mask) == (uint8_t)Format::Fixed_Str);
		str_len = data[index] & Fixed_Str_Value;
		CHECK(str_len == string.size());

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<std::string>());
		CHECK(object.as<std::string>() == string);
	}

	// -------------------------------------------------------------------

	SUBCASE("Len_1")
	{
		const std::string string(1, '_');

		object = {string};
		CHECK(object.is<std::string>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == string.size() + 1);

		index = 0;
		CHECK((data[index] & Fixed_Str_Mask) == (uint8_t)Format::Fixed_Str);
		str_len = data[index] & Fixed_Str_Value;
		CHECK(str_len == string.size());

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<std::string>());
		CHECK(object.as<std::string>() == string);
	}

	// -------------------------------------------------------------------

	SUBCASE("Len_31")
	{
		const std::string string(31, 'X');

		object = {string};
		CHECK(object.is<std::string>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == string.size() + 1);

		index = 0;
		CHECK((data[index] & Fixed_Str_Mask) == (uint8_t)Format::Fixed_Str);
		str_len = data[index] & Fixed_Str_Value;
		CHECK(str_len == string.size());

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<std::string>());
		CHECK(object.as<std::string>() == string);
	}
}


TEST_CASE("serialize/object/str8")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;
	size_t               str_len;

	// -------------------------------------------------------------------

	SUBCASE("Min")
	{
		const std::string string(32, '_');

		object = {string};
		CHECK(object.is<std::string>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == string.size() + 2);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Str8);
		str_len = data[index++];
		CHECK(str_len == string.size());

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<std::string>());
		CHECK(object.as<std::string>() == string);
	}

	// -------------------------------------------------------------------

	SUBCASE("Max")
	{
		const std::string string(std::numeric_limits<uint8_t>::max(), 'X');

		object = {string};
		CHECK(object.is<std::string>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == (string.size() + 2));

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Str8);
		str_len = data[index++];
		CHECK(str_len == string.size());

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<std::string>());
		CHECK(object.as<std::string>() == string);
	}
}


TEST_CASE("serialize/object/str16")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;
	size_t               str_len;

	// -------------------------------------------------------------------

	SUBCASE("Min")
	{
		const std::string string(std::numeric_limits<uint8_t>::max() + 1, '_');

		object = {string};
		CHECK(object.is<std::string>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == string.size() + 3);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Str16);
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		str_len = Convert.uint16;
		CHECK(str_len == string.size());

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<std::string>());
		CHECK(object.as<std::string>() == string);
	}

	// -------------------------------------------------------------------

	SUBCASE("Max")
	{
		const std::string string(std::numeric_limits<uint16_t>::max(), 'X');

		object = {string};
		CHECK(object.is<std::string>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == (string.size() + 3));

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Str16);
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		str_len = Convert.uint16;
		CHECK(str_len == string.size());

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<std::string>());
		CHECK(object.as<std::string>() == string);
	}
}


TEST_CASE("serialize/object/str32")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;
	size_t               str_len;

	// -------------------------------------------------------------------

	SUBCASE("Min")
	{
		const std::string string(std::numeric_limits<uint16_t>::max() + 1, '_');

		object = {string};
		CHECK(object.is<std::string>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == string.size() + 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Str32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		str_len = Convert.uint32;
		CHECK(str_len == string.size());

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<std::string>());
		CHECK(object.as<std::string>() == string);
	}

	// -------------------------------------------------------------------

	SUBCASE("Max")
	{
		/*
		 * This string would be 4GB + overhead from std::string
		 * Serializing would consume another 4GB.
		 * For a total of 8GB to test the maximum Str32 length.
		const std::string string(std::numeric_limits<uint32_t>::max(), 'X');

		object = {string};
		CHECK(object.is<std::string>());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == (string.size() + 3));

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Str32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		str_len = Convert.uint32;
		CHECK(str_len == string.size());

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.is<std::string>());
		CHECK(object.as<std::string>() == string);
		 */
	}
}


TEST_CASE("serialize/object/bin8")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;
	size_t               bin_len;

	// -------------------------------------------------------------------

	SUBCASE("Min")
	{
		const std::vector<uint8_t> bin = {};

		object = {bin};
		CHECK(object.isBinary());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == bin.size() + 2);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Bin8);
		bin_len = data[index++];
		CHECK(bin_len == bin.size());

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.isBinary());

		const std::vector<uint8_t>& vector = object.asBinary();
		for(size_t i = 0; i < bin_len; i++)
		{
			CHECK(vector[i] == bin[i]);
		}
	}

	// -------------------------------------------------------------------

	SUBCASE("Max")
	{
		const std::vector<uint8_t> bin(std::numeric_limits<uint8_t>::max(), 'X');

		object = {bin};
		CHECK(object.isBinary());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == bin.size() + 2);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Bin8);
		bin_len = data[index++];
		CHECK(bin_len == bin.size());

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.isBinary());

		const std::vector<uint8_t>& vector = object.asBinary();
		for(size_t i = 0; i < bin_len; i++)
		{
			CHECK(vector[i] == bin[i]);
		}
	}
}


TEST_CASE("serialize/object/bin16")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;
	size_t               bin_len;

	// -------------------------------------------------------------------

	SUBCASE("Min")
	{
		const std::vector<uint8_t> bin(std::numeric_limits<uint8_t>::max() + 1, '_');

		object = {bin};
		CHECK(object.isBinary());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == bin.size() + 3);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Bin16);
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		bin_len = Convert.uint16;
		CHECK(bin_len == bin.size());

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.isBinary());

		const std::vector<uint8_t>& vector = object.asBinary();
		for(size_t i = 0; i < bin_len; i++)
		{
			CHECK(vector[i] == bin[i]);
		}
	}

	// -------------------------------------------------------------------

	SUBCASE("Max")
	{
		const std::vector<uint8_t> bin(std::numeric_limits<uint16_t>::max(), 'X');

		object = {bin};
		CHECK(object.isBinary());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == bin.size() + 3);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Bin16);
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		bin_len = Convert.uint16;
		CHECK(bin_len == bin.size());

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.isBinary());

		const std::vector<uint8_t>& vector = object.asBinary();
		for(size_t i = 0; i < bin_len; i++)
		{
			CHECK(vector[i] == bin[i]);
		}
	}
}


TEST_CASE("serialize/object/bin32")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;
	size_t               bin_len;

	// -------------------------------------------------------------------

	SUBCASE("Min")
	{
		const std::vector<uint8_t> bin(std::numeric_limits<uint16_t>::max() + 1, '_');

		object = {bin};
		CHECK(object.isBinary());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == bin.size() + 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Bin32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		bin_len = Convert.uint32;
		CHECK(bin_len == bin.size());

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.isBinary());

		const std::vector<uint8_t>& vector = object.asBinary();
		for(size_t i = 0; i < bin_len; i++)
		{
			CHECK(vector[i] == bin[i]);
		}
	}

	// -------------------------------------------------------------------

	SUBCASE("Max")
	{
		/*
		 * This vector would be 4GB + overhead from std::vector
		 * Serializing would consume another 4GB.
		 * For a total of 8GB to test the maximum Str32 length.
		const std::vector<uint8_t> bin(std::numeric_limits<uint32_t>::max(), 'X');

		object = {bin};
		CHECK(object.isBinary());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == (bin.size() + 5));

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Bin32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		bin_len = Convert.uint32;
		CHECK(bin_len == bin.size());

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.isBinary());

		const std::vector<uint8_t>& vector = object.asBinary();
		for(size_t i = 0; i < bin_len; i++)
		{
			CHECK(vector[i] == bin[i]);
		}
		 */
	}
}


TEST_CASE("serialize/object/fixed_array")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;
	size_t               array_len;

	// -------------------------------------------------------------------

	SUBCASE("Empty")
	{
		object = {Array{}};
		CHECK(object.isArray());
		CHECK(object.asArray().size() == 0);

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == 1);

		index = 0;
		CHECK((data[index] & Fixed_Array_Mask) == (uint8_t)Format::Fixed_Array);
		array_len = data[index] & Fixed_Array_Value;
		CHECK(array_len == 0);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.isArray());
		CHECK(object.asArray().size() == 0);
	}

	// -------------------------------------------------------------------

	SUBCASE("Len 15")
	{
		object = {Array{}};
		CHECK(object.isArray());

		for(size_t i = 0; i < 15; i++)
		{
			object.asArray().appendNull();
		}

		array_len = object.asArray().size();
		CHECK(array_len == 15);

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == (array_len + 1));

		index = 0;
		CHECK((data[index] & Fixed_Array_Mask) == (uint8_t)Format::Fixed_Array);
		array_len = data[index] & Fixed_Array_Value;
		CHECK(array_len == 15);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.isArray());
		CHECK(object.asArray().size() == 15);

		for(size_t i = 0; i < 15; i++)
		{
			CHECK(object.asArray().object(i).isNull());
		}
	}
}


TEST_CASE("serialize/object/array16")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;
	size_t               array_len;

	// -------------------------------------------------------------------

	SUBCASE("Min")
	{
		object = {Array{}};
		CHECK(object.isArray());

		for(size_t i = 0; i < 16; i++)
		{
			object.asArray().append(true);
		}

		array_len = object.asArray().size();
		CHECK(array_len == 16);

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == (array_len + 3));

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Array16);
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		array_len = Convert.uint16;
		CHECK(array_len == 16);

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.isArray());
		CHECK(object.asArray().size() == 16);

		for(size_t i = 0; i < 16; i++)
		{
			CHECK(object.asArray().object(i).is<bool>());
			CHECK(object.asArray().object(i).as<bool>() == true);
		}
	}

	// -------------------------------------------------------------------

	SUBCASE("Max")
	{
		object = {Array{}};
		CHECK(object.isArray());

		for(size_t i = 0; i < std::numeric_limits<uint16_t>::max(); i++)
		{
			object.asArray().append(false);
		}

		array_len = object.asArray().size();
		CHECK(array_len == std::numeric_limits<uint16_t>::max());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == (array_len + 3));

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Array16);
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		array_len = Convert.uint16;
		CHECK(array_len == std::numeric_limits<uint16_t>::max());

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.isArray());
		CHECK(array_len == std::numeric_limits<uint16_t>::max());

		for(int i = 0; i < std::numeric_limits<uint16_t>::max(); i++)
		{
			CHECK(object.asArray().object(i).is<bool>());
			CHECK(object.asArray().object(i).as<bool>() == false);
		}
	}
}


TEST_CASE("serialize/object/array32")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index;
	size_t               array_len;

	// -------------------------------------------------------------------

	SUBCASE("Min")
	{
		object = {Array{}};
		CHECK(object.isArray());

		for(size_t i = 0; i < std::numeric_limits<uint16_t>::max() + 1; i++)
		{
			object.asArray().append(true);
		}

		array_len = object.asArray().size();
		CHECK(array_len == (std::numeric_limits<uint16_t>::max() + 1));

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == (array_len + 5));

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Array32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		array_len = Convert.uint32;
		CHECK(array_len == (std::numeric_limits<uint16_t>::max() + 1));

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.isArray());
		CHECK(object.asArray().size() == (std::numeric_limits<uint16_t>::max() + 1));

		for(size_t i = 0; i < std::numeric_limits<uint16_t>::max() + 1; i++)
		{
			CHECK(object.asArray().object(i).is<bool>());
			CHECK(object.asArray().object(i).as<bool>() == true);
		}
	}

	// -------------------------------------------------------------------

	SUBCASE("Max")
	{
		/*
		 * This vector would be 4GB + overhead from std::vector
		 * Serializing would consume another 4GB.
		 * For a total of 8GB to test the maximum Array32 length.
		object = {Array{}};
		CHECK(object.isArray());

		for(size_t i = 0; i < std::numeric_limits<uint32_t>::max(); i++)
		{
			object.asArray().append(false);
		}

		array_len = object.asArray().size();
		CHECK(array_len == std::numeric_limits<uint32_t>::max());

		// Check serialized data

		data = serialize(object);
		CHECK(data.size() == (array_len + 5));

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Array32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		array_len = Convert.uint32;
		CHECK(array_len == std::numeric_limits<uint32_t>::max());

		// Check deserialized data

		object = deserialize(data);
		CHECK(object.isArray());
		CHECK(array_len == std::numeric_limits<uint32_t>::max());

		for(int i = 0; i < std::numeric_limits<uint32_t>::max(); i++)
		{
			CHECK(object.asArray().object(i).is<bool>());
			CHECK(object.asArray().object(i).as<bool>() == false);
		}
		*/
	}
}

#endif // }}}

// }}} Utilities::serialize
// {{{ Utilities::to_string

/**
 * \brief Convert to a JSON formatted string.
 *
 * \return A string.
 */
std::string to_string(const messagepack::Array& array ///< The Array to convert.
	) noexcept
{
	std::string s = "[";

	std::string prefix = " ";

	for(size_t i = 0; i < array.size(); i++)
	{
		s += prefix + to_string(array.object(i));

		if(i == 0)
		{
			prefix = ", ";
		}
	}

	s += "]";

	return s;
}


/**
 * \brief Convert to a JSON formatted string.
 *
 * \return A string.
 */
std::string to_string(const messagepack::Ext& ext ///< The Ext to convert.
	) noexcept
{
	std::string s = "";

	s += "( 'type': " + std::to_string(ext.type);
	s += ", 'data': [";

	std::string prefix = " ";

	for(size_t i = 0; i < ext.data.size(); i++)
	{
		s += prefix + std::to_string(ext.data[i]);

		if(i == 0)
		{
			prefix = ", ";
		}
	}

	s += " ]";
	s += " )";

	return s;
}


/**
 * \brief Convert to a JSON formatted string.
 *
 * \return A string.
 */
std::string to_string(const messagepack::Map& map ///< The Map to convert.
	) noexcept
{
	std::string s = "{";

	std::string prefix = " ";

	for(size_t i = 0; i < map.size(); i++)
	{
		s += prefix
			+ to_string(map.keyAt(i))
			+ ": "
			+ to_string(map.valueAt(i))
			;

		prefix = ", ";
	}

	s += " }";

	return s;
}


/**
 * \brief Convert to a JSON formatted string.
 *
 * \return A string.
 */
std::string to_string(const messagepack::Object& object ///< The Object to convert.
	) noexcept
{
	std::string s = "{ ";

	if(object.isNull())
	{
		s += "'type': 'null'";
	}
	else if(object.is<bool>())
	{
		s += "'type': 'bool', 'value': ";
		if(object.as<bool>() == true)
		{
			s += "true";
		}
		else
		{
			s += "false";
		}
	}
	else if(object.is<int64_t>())
	{
		s += "'type': 'int64_t', 'value': "
			+ std::to_string(object.as<int64_t>())
			;
	}
	else if(object.is<uint64_t>())
	{
		s += "'type': 'uint64_t', 'value': "
			+ std::to_string(object.as<uint64_t>())
			;
	}
	else if(object.is<float>())
	{
		s += "'type': 'float', 'value': "
			+ std::to_string(object.as<float>())
			;
	}
	else if(object.is<double>())
	{
		s += "'type': 'double', 'value': "
			+ std::to_string(object.as<double>())
			;
	}
	else if(object.is<std::string>())
	{
		s += "'type': 'std::string', 'value': '"
			+ object.as<std::string>()
			+ "'"
			;
	}
	else if(object.is<std::vector<uint8_t>>())
	{
		s += "'type': 'std::vector<uint8_t>', 'value': ["
			;

		std::string prefix = " ";

		const std::vector<uint8_t>& data = object.as<std::vector<uint8_t>>();
		for(size_t i = 0; i < data.size(); i++)
		{
			s += prefix + std::to_string(data[i]);

			if(i == 0)
			{
				prefix = ", ";
			}
		}

		s += " ]";
	}
	else if(object.isArray())
	{
		s += "'type': 'zakero::messagepack::Array', 'value': "
			+ zakero::messagepack::to_string(object.asArray())
			;
	}
	else if(object.isExt())
	{
		s += "'type': 'zakero::messagepack::Ext', 'value': "
			+ zakero::messagepack::to_string(object.asExt())
			;
	}
	else if(object.isMap())
	{
		s += "'type': 'zakero::messagepack::Ext', 'value': "
			+ zakero::messagepack::to_string(object.asMap())
			;
	}

	s += " }";

	return s;
}
// }}} Utilities::to_string
// }}} Utilities
} // zakero::messagepack

// {{{ Operators

/**
 * \brief OStream operator.
 *
 * The \p array will be converted into a JSON formatted string and writen to 
 * the \p stream.
 *
 * \return The \p stream object.
 */
std::ostream& operator<<(std::ostream&      stream ///< The stream to use.
	, const zakero::messagepack::Array& array  ///< The data to write.
	) noexcept
{
	stream << zakero::messagepack::to_string(array);
	
	return stream;
}


/**
 * \brief OStream operator.
 *
 * The \p ext will be converted into a JSON formatted string and writen to the 
 * \p stream.
 *
 * \return The \p stream object.
 */
std::ostream& operator<<(std::ostream&    stream ///< The stream to use.
	, const zakero::messagepack::Ext& ext ///< The data to write.
	) noexcept
{
	stream << zakero::messagepack::to_string(ext);
	
	return stream;
}


/**
 * \brief OStream operator.
 *
 * The \p map will be converted into a JSON formatted string and writen to the 
 * \p stream.
 *
 * \return The \p stream object.
 */
std::ostream& operator<<(std::ostream&    stream ///< The stream to use.
	, const zakero::messagepack::Map& map ///< The data to write.
	) noexcept
{
	stream << zakero::messagepack::to_string(map);
	
	return stream;
}


/**
 * \brief OStream operator.
 *
 * The \p object will be converted into a JSON formatted string and writen to 
 * the \p stream.
 *
 * \return The \p stream object.
 */
std::ostream& operator<<(std::ostream&       stream ///< The stream to use.
	, const zakero::messagepack::Object& object ///< The data to write.
	) noexcept
{
	stream << zakero::messagepack::to_string(object);
	
	return stream;
}

/**
 * \brief Compare two Objects for equality.
 *
 * For two Objects to be the _same_, their variant types must be the same 
 * __and__ the contents of the Objects must match.
 *
 * \retval true  The Objects are the same.
 * \retval false The Objects are not the same.
 */
bool operator==(const zakero::messagepack::Object& lhs ///< The Object
	, const zakero::messagepack::Object&       rhs ///< The Object
	) noexcept
{
	if(&lhs == &rhs)
	{
		return true;
	}

	if(lhs.isNull() && rhs.isNull())
	{
		return true;
	}

	if(lhs.is<bool>() && rhs.is<bool>())
	{
		return (lhs.as<bool>() == rhs.as<bool>());
	}

	if(lhs.is<int64_t>() && rhs.is<int64_t>())
	{
		return (lhs.as<int64_t>() == rhs.as<int64_t>());
	}

	if(lhs.is<uint64_t>() && rhs.is<uint64_t>())
	{
		return (lhs.as<uint64_t>() == rhs.as<uint64_t>());
	}

	if(lhs.is<float>() && rhs.is<float>())
	{
		return (lhs.as<float>() == rhs.as<float>());
	}

	if(lhs.is<double>() && rhs.is<double>())
	{
		return (lhs.as<double>() == rhs.as<double>());
	}

	if(lhs.isString() && rhs.isString())
	{
		return (lhs.asString() == rhs.asString());
	}

	if(lhs.isBinary() && rhs.isBinary())
	{
		return (lhs.asBinary() == rhs.asBinary());
	}

	if(lhs.isArray() && rhs.isArray())
	{
		const zakero::messagepack::Array& l_array = lhs.asArray();
		const zakero::messagepack::Array& r_array = rhs.asArray();

		if(l_array.size() != r_array.size())
		{
			return false;
		}

		for(size_t i = 0; i < l_array.size(); i++)
		{
			if(l_array.object(i) != r_array.object(i))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}


// }}}

#endif // ZAKERO_MESSAGEPACK_IMPLEMENTATION

// }}}

#endif // zakero_MessagePack_h
