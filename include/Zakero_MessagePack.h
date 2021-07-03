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
|                           |                              |                              |                              |                             |                             |                                 |
|---------------------------|------------------------------|------------------------------|------------------------------|-----------------------------|-----------------------------|---------------------------------|
| \api{zakero::MessagePack} | \refdeps{zakero_messagepack} | \reftldr{zakero_messagepack} | \refwhat{zakero_messagepack} | \refwhy{zakero_messagepack} | \refhow{zakero_messagepack} | \refversion{zakero_messagepack} |
 * 
 * The _Zakero_MessagePack_ will serialize and deserialize data using the 
 * MessagePack specification.
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
 *
 * __Draw Backs__
 * - Memory Usage: Serialization makes a copy of the contents
 * - No C++ iterators, must use traditional C-Style loops
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
 * After creating an instance of [MessagePack](\ref zakero::MessagePack), it 
 * needs to be populated with data. There are two options to do this:
 *
 * _Add data manually_
 * ~~~
 * zakero::MessagePack message_pack;
 *
 * size_t text;
 * message_pack.append(uint64_t(42));
 * message_pack.append(true);
 * message_pack.append("Hello, World!", &text);
 * ~~~
 *
 * _Deserialize data_
 * ~~~
 * std::vector<uint8_t> data = load_data();
 *
 * zakero::MessagePack message_pack;
 * message_pack.deserialize(data);
 * ~~~
 *
 * The data in the MessagePack can be modified:
 * ~~~
 * message_pack.object(text).string = "Good Bye!";
 * ~~~
 *
 * Then the MessagePack can be (re)serialized:
 * ~~~
 * data = MessagePack::serialize(message_pack.);
 *
 * save_data(data);
 * ~~~
 * \endparhow
 *
 *
 * \parversion{zakero_messagepack}
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
 */


/******************************************************************************
 * Includes
 */

// C++
#include <cstring>
#include <string>
#include <limits>
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


/**
 * \internal
 *
 * \brief _Object_Types_
 *
 * An X-Macro of supported Object data-types.
 * -# __Name__: The name of the Object Type
 * -# __Type__: The C++ data-type
 */
#define ZAKERO_MESSAGEPACK__OBJECT_TYPE \
	/* Name      Type                      */ \
	X(Bool     , bool                       ) \
	X(Int64_t  , int64_t                    ) \
	X(Uint64_t , uint64_t                   ) \
	X(Float    , float                      ) \
	X(Double   , double                     ) \
	X(String   , std::string                ) \
	X(Binary   , std::vector<uint8_t>       ) \
	X(Array    , zakero::messagepack::Array ) \

// }}}


namespace zakero::messagepack
{
		// {{{ Declaration

		struct Object;

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
				      size_t               append(const Object&) noexcept;
				      size_t               append(Object&) noexcept;
				      size_t               appendNull() noexcept;

			[[nodiscard]] inline Object&       object(const size_t index) noexcept { return object_vector[index]; }
			[[nodiscard]] inline const Object& object(const size_t index) const noexcept { return object_vector[index]; }

				      inline void          clear() noexcept { return object_vector.clear(); };
			[[nodiscard]] inline size_t        size() const noexcept { return object_vector.size(); };

			[[nodiscard]] std::vector<uint8_t> serialize() const noexcept;

			std::vector<Object> object_vector = {};
		};

		struct Object
		{
			std::variant<std::monostate
#define X(name_,type_) \
				, type_ \

				ZAKERO_MESSAGEPACK__OBJECT_TYPE
#undef X
				> value;

			template<typename T> [[nodiscard]] T&                          as() noexcept { return std::get<T>(value); };
			template<typename T> [[nodiscard]] const T&                    as() const noexcept { return std::get<T>(value); };
			                     [[nodiscard]] messagepack::Array&         asArray() noexcept { return std::get<messagepack::Array>(value); };
			                     [[nodiscard]] const messagepack::Array&   asArray() const noexcept { return std::get<messagepack::Array>(value); };
			                     [[nodiscard]] std::vector<uint8_t>&       asBinary() noexcept { return std::get<std::vector<uint8_t>>(value); };
			                     [[nodiscard]] const std::vector<uint8_t>& asBinary() const noexcept { return std::get<std::vector<uint8_t>>(value); };
			                     [[nodiscard]] const std::string&          asString() const noexcept { return std::get<std::string>(value); };

			template<typename T> [[nodiscard]] constexpr bool              is() const noexcept { return std::holds_alternative<T>(value); };
			                     [[nodiscard]] constexpr bool              isArray() const noexcept { return std::holds_alternative<messagepack::Array>(value); };
			                     [[nodiscard]] constexpr bool              isBinary() const noexcept { return std::holds_alternative<std::vector<uint8_t>>(value); };
			                     [[nodiscard]] constexpr bool              isNull() const noexcept { return std::holds_alternative<std::monostate>(value); };
			                     [[nodiscard]] constexpr bool              isString() const noexcept { return std::holds_alternative<std::string>(value); };

			                     [[nodiscard]] std::vector<uint8_t>        serialize() const noexcept;
		};

		// }}}
		// {{{ Utilities

		[[nodiscard]] Object      deserialize(const std::vector<uint8_t>&) noexcept;
		[[nodiscard]] Object      deserialize(const std::vector<uint8_t>&, size_t&) noexcept;
		[[nodiscard]] std::string to_string(const messagepack::Array&) noexcept;
		[[nodiscard]] std::string to_string(const messagepack::Object&) noexcept;

		// }}}
}

// {{{ Operators

std::ostream& operator<<(std::ostream&, const zakero::messagepack::Array&) noexcept;
std::ostream& operator<<(std::ostream&, const zakero::messagepack::Object&) noexcept;

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
	X(fixmap        , 0x80      , 0b00000000  , "fixmap"          ) \
	X(Fixed_Array   , 0x90      , 0b11110000  , "fixarray"        ) \
	X(Fixed_Str     , 0xa0      , 0b11100000  , "fixstr"          ) \
	X(Nill          , 0xc0      , 0b11111111  , "nill"            ) \
	X(never_used    , 0xc1      , 0b00000000  , "(never used)"    ) \
	X(False         , 0xc2      , 0b11111111  , "false"           ) \
	X(True          , 0xc3      , 0b11111111  , "true"            ) \
	X(Bin8          , 0xc4      , 0b11111111  , "bin 8"           ) \
	X(Bin16         , 0xc5      , 0b11111111  , "bin 16"          ) \
	X(Bin32         , 0xc6      , 0b11111111  , "bin 32"          ) \
	X(ext8          , 0xc7      , 0b00000000  , "ext 8"           ) \
	X(ext16         , 0xc8      , 0b00000000  , "ext 16"          ) \
	X(ext32         , 0xc9      , 0b00000000  , "ext 32"          ) \
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
	X(fixext1       , 0xd4      , 0b00000000  , "fixext 1"        ) \
	X(fixext2       , 0xd5      , 0b00000000  , "fixext 2"        ) \
	X(fixext4       , 0xd6      , 0b00000000  , "fixext 4"        ) \
	X(fixext8       , 0xd7      , 0b00000000  , "fixext 8"        ) \
	X(fixext16      , 0xd8      , 0b00000000  , "fixext 16"       ) \
	X(Str8          , 0xd9      , 0b11111111  , "str 8"           ) \
	X(Str16         , 0xda      , 0b11111111  , "str 16"          ) \
	X(Str32         , 0xdb      , 0b11111111  , "str 32"          ) \
	X(Array16       , 0xdc      , 0b11111111  , "array 16"        ) \
	X(Array32       , 0xdd      , 0b11111111  , "array 32"        ) \
	X(map16         , 0xde      , 0b00000000  , "map 16"          ) \
	X(map32         , 0xdf      , 0b00000000  , "map 32"          ) \
	X(Fixed_Int_Neg , 0xe0      , 0b11100000  , "negative fixint" ) \

// }}}
// {{{ Documentation

/**
 * \class zakero::MessagePack
 *
 * \brief Object and Data Serialization.
 *
 * This class implements the 
 * [MessagePack](https://github.com/msgpack/msgpack/blob/master/spec.md) 
 * specification. Usage is simple:
 *
 * __To Serialize:__
 *
 * -# Create a MessagePack instance
 * -# Add data
 * -# Serialize the data into a std::vector
 *
 * __To Deserialize:__
 * -# Create a MessagePack instance
 * -# Pass a std::vector to be deserialized
 * -# Access the data objects
 */

/**
 * \enum zakero::MessagePack::DataType
 *
 * \brief The supported C++ data-types.
 */

// }}}

namespace zakero
{
namespace messagepack
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
	 */
	std::map<uint8_t, std::string> Format_Name =
	{
	#define X(format_, id_, mask_, name_) \
		{ uint8_t(id_ & mask_), std::string(name_) },

		ZAKERO_MESSAGEPACK__FORMAT_TYPE
	#undef X
	};


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
	 * \brief Object Type names.
	 */
	const std::string Object_Type_Name[] =
	{	"Null"
		/**
		 * \brief Convert ZAKERO_MESSAGEPACK__FORMAT_TYPE into code.
		 */
#define X(name_, type_) \
	,	#name_ \

		ZAKERO_MESSAGEPACK__OBJECT_TYPE
#undef X
	};


	/**
	 * \brief In-Place Byte Conversion.
	 */
	union
	{
		uint64_t uint64;
		uint32_t uint32;
		uint16_t uint16;
		int64_t  int64;
		int32_t  int32;
		int16_t  int16;
		float    float32;
		double   float64;
		uint8_t  uint8[8];
	} Convert;


	/**
	 * \name Endian Conversion
	 * \{
	 */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	uint8_t& Convert_Byte0 = Convert.uint8[7];
	uint8_t& Convert_Byte1 = Convert.uint8[6];
	uint8_t& Convert_Byte2 = Convert.uint8[5];
	uint8_t& Convert_Byte3 = Convert.uint8[4];
	uint8_t& Convert_Byte4 = Convert.uint8[3];
	uint8_t& Convert_Byte5 = Convert.uint8[2];
	uint8_t& Convert_Byte6 = Convert.uint8[1];
	uint8_t& Convert_Byte7 = Convert.uint8[0];
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	uint8_t& Convert_Byte0 = Convert.uint8[0];
	uint8_t& Convert_Byte1 = Convert.uint8[1];
	uint8_t& Convert_Byte2 = Convert.uint8[2];
	uint8_t& Convert_Byte3 = Convert.uint8[3];
	uint8_t& Convert_Byte4 = Convert.uint8[4];
	uint8_t& Convert_Byte5 = Convert.uint8[5];
	uint8_t& Convert_Byte6 = Convert.uint8[6];
	uint8_t& Convert_Byte7 = Convert.uint8[7];
#endif
	/**
	 * \}
	 */


	using VectorObject = std::vector<messagepack::Object>;

	inline size_t append_(const bool value  ///< The value to add
		, VectorObject&          vector ///< The vector
		) noexcept
	{
		const size_t index = vector.size();

		vector.emplace_back(value);

		return index;
	}


	inline size_t append_(const int64_t value  ///< The value to add
		, VectorObject&             vector ///< The vector
		) noexcept
	{
		const size_t index = vector.size();

		vector.emplace_back(value);

		return index;
	}


	inline size_t append_(const uint64_t value  ///< The value to add
		, VectorObject&              vector ///< The vector
		) noexcept
	{
		const size_t index = vector.size();

		vector.emplace_back(value);

		return index;
	}


	inline size_t append_(const float value  ///< The value to add
		, VectorObject&           vector ///< The vector
		) noexcept
	{
		const size_t index = vector.size();

		vector.emplace_back(value);

		return index;
	}


	inline size_t append_(const double value  ///< The value to add
		, VectorObject&            vector ///< The vector
		) noexcept
	{
		const size_t index = vector.size();

		vector.emplace_back(value);

		return index;
	}


	inline size_t append_(const std::string_view value  ///< The value to add
		, VectorObject&                      vector ///< The vector
		) noexcept
	{
		const size_t index = vector.size();

		vector.emplace_back(std::string(value));

		return index;
	}


	inline size_t append_(const std::vector<uint8_t>& value  ///< The value to add
		, VectorObject&                           vector ///< The vector
		) noexcept
	{
		const size_t index = vector.size();

		vector.emplace_back(value);

		return index;
	}


	inline size_t append_(std::vector<uint8_t>&& value  ///< The value to add
		, VectorObject&                      vector ///< The vector
		) noexcept
	{
		const size_t index = vector.size();

		vector.emplace_back(std::move(value));

		return index;
	}


	inline size_t append_(const uint8_t* value  ///< The value to add
		, const size_t               size   ///< The value count
		, VectorObject&              vector ///< The vector
		) noexcept
	{
		const size_t index = vector.size();

		std::vector<uint8_t> data(size);
		memcpy(data.data(), value, size);

		vector.emplace_back(std::move(data));

		return index;
	}


	inline size_t appendArray_(VectorObject& vector ///< The vector
		) noexcept
	{
		const size_t index = vector.size();

		vector.emplace_back(messagepack::Array{});

		return index;
	}


	inline size_t appendNull_(VectorObject& vector ///< The vector
		) noexcept
	{
		const size_t index = vector.size();

		vector.emplace_back();

		return index;
	}


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
			/**
			 * \todo Add `vector.reserve()` support. May need to 
			 * add `Object::size()`.
			 */
			const messagepack::Array& array = object.asArray();
			const size_t              array_size = array.size();

			if(array_size <= 15)
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
				
			for(size_t i = 0; i < array_size; i++)
			{
				serialize_(array.object(i), vector);
			}
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


#if 0 // REMOVE ???
	void deserialize_(const std::vector<uint8_t>& data
		, size_t&                             index
		, std::vector<messagepack::Object>&   object_vector
		) noexcept
	{
		const uint8_t byte = data[index];

		if(byte == (uint8_t)Format::Nill)
		{
			appendNull_(object_vector);
		}
		else if(byte == (uint8_t)Format::False)
		{
			append_(false, object_vector);
		}
		else if(byte == (uint8_t)Format::True)
		{
			append_(true, object_vector);
		}
		else if((byte & (uint8_t)Fixed_Int_Pos_Mask) == (uint8_t)Format::Fixed_Int_Pos)
		{
			const int64_t value = byte & (uint8_t)Fixed_Int_Pos_Value;
			append_(value, object_vector);
		}
		else if((byte & (uint8_t)Fixed_Int_Neg_Mask) == (uint8_t)Format::Fixed_Int_Neg)
		{
			const int64_t value = (int8_t)(byte & Fixed_Int_Neg_Value) - 32;
			append_(value, object_vector);
		}
		else if(byte == (uint8_t)Format::Int8)
		{
			const int64_t value = (int8_t)data[++index];
			append_(value, object_vector);
		}
		else if(byte == (uint8_t)Format::Int16)
		{
			Convert_Byte1 = data[++index];
			Convert_Byte0 = data[++index];

			const int64_t value = Convert.int16;
			append_(value, object_vector);
		}
		else if(byte == (uint8_t)Format::Int32)
		{
			Convert_Byte3 = data[++index];
			Convert_Byte2 = data[++index];
			Convert_Byte1 = data[++index];
			Convert_Byte0 = data[++index];

			const int64_t value = Convert.int32;
			append_(value, object_vector);
		}
		else if(byte == (uint8_t)Format::Int64)
		{
			Convert_Byte7 = data[++index];
			Convert_Byte6 = data[++index];
			Convert_Byte5 = data[++index];
			Convert_Byte4 = data[++index];
			Convert_Byte3 = data[++index];
			Convert_Byte2 = data[++index];
			Convert_Byte1 = data[++index];
			Convert_Byte0 = data[++index];

			const int64_t value = Convert.int64;
			append_(value, object_vector);
		}
		else if(byte == (uint8_t)Format::Uint8)
		{
			const uint64_t value = data[++index];
			append_(value, object_vector);
		}
		else if(byte == (uint8_t)Format::Uint16)
		{
			Convert_Byte1 = data[++index];
			Convert_Byte0 = data[++index];

			const uint64_t value = Convert.uint16;
			append_(value, object_vector);
		}
		else if(byte == (uint8_t)Format::Uint32)
		{
			Convert_Byte3 = data[++index];
			Convert_Byte2 = data[++index];
			Convert_Byte1 = data[++index];
			Convert_Byte0 = data[++index];

			const uint64_t value = Convert.uint32;
			append_(value, object_vector);
		}
		else if(byte == (uint8_t)Format::Uint64)
		{
			Convert_Byte7 = data[++index];
			Convert_Byte6 = data[++index];
			Convert_Byte5 = data[++index];
			Convert_Byte4 = data[++index];
			Convert_Byte3 = data[++index];
			Convert_Byte2 = data[++index];
			Convert_Byte1 = data[++index];
			Convert_Byte0 = data[++index];

			const uint64_t value = Convert.uint64;
			append_(value, object_vector);
		}
		else if(byte == (uint8_t)Format::Float32)
		{
			Convert_Byte3 = data[++index];
			Convert_Byte2 = data[++index];
			Convert_Byte1 = data[++index];
			Convert_Byte0 = data[++index];

			const float value = Convert.float32;
			append_(value, object_vector);
		}
		else if(byte == (uint8_t)Format::Float64)
		{
			Convert_Byte7 = data[++index];
			Convert_Byte6 = data[++index];
			Convert_Byte5 = data[++index];
			Convert_Byte4 = data[++index];
			Convert_Byte3 = data[++index];
			Convert_Byte2 = data[++index];
			Convert_Byte1 = data[++index];
			Convert_Byte0 = data[++index];

			const double value = Convert.float64;
			append_(value, object_vector);
		}
		else if((byte & (uint8_t)Fixed_Str_Mask) == (uint8_t)Format::Fixed_Str)
		{
			const size_t length = byte & Fixed_Str_Value;

			if(length == 0)
			{
				append_(std::string_view(""), object_vector);
			}
			else
			{
				std::string_view str((char*)&data[++index], length);
				append_(str, object_vector);

				index += length - 1;
			}
		}
		else if(byte == (uint8_t)Format::Str8)
		{
			const size_t length = data[++index];

			std::string_view str((char*)&data[++index], length);
			append_(str, object_vector);

			index += length - 1;
		}
		else if(byte == (uint8_t)Format::Str16)
		{
			Convert_Byte1 = data[++index];
			Convert_Byte0 = data[++index];

			const size_t length = Convert.uint16;
			const std::string_view str((char*)&data[++index], length);
			append_(str, object_vector);

			index += length - 1;
		}
		else if(byte == (uint8_t)Format::Str32)
		{
			Convert_Byte3 = data[++index];
			Convert_Byte2 = data[++index];
			Convert_Byte1 = data[++index];
			Convert_Byte0 = data[++index];

			const size_t length = Convert.uint32;
			const std::string_view str((char*)&data[++index], length);
			append_(str, object_vector);

			index += length - 1;
		}
		else if(byte == (uint8_t)Format::Bin8)
		{
			const size_t length = data[++index];

			std::vector<uint8_t> vector(length);
			memcpy((void*)vector.data(), (void*)&data[++index], length);
			append_(std::move(vector), object_vector);

			index += length - 1;
		}
		else if(byte == (uint8_t)Format::Bin16)
		{
			Convert_Byte1 = data[++index];
			Convert_Byte0 = data[++index];

			const size_t length = Convert.uint16;

			std::vector<uint8_t> vector(length);
			memcpy((void*)vector.data(), (void*)&data[++index], length);
			append_(std::move(vector), object_vector);

			index += length - 1;
		}
		else if(byte == (uint8_t)Format::Bin32)
		{
			Convert_Byte3 = data[++index];
			Convert_Byte2 = data[++index];
			Convert_Byte1 = data[++index];
			Convert_Byte0 = data[++index];

			const size_t length = Convert.uint32;

			std::vector<uint8_t> vector(length);
			memcpy((void*)vector.data(), (void*)&data[++index], length);
			append_(std::move(vector), object_vector);

			index += length - 1;
		}
/**
 * \todo Re-enable
		else if((byte & (uint8_t)Fixed_Array_Mask) == (uint8_t)Format::Fixed_Array)
		{
			const size_t array_index = appendArray_(object_vector);
			const size_t count = byte & (uint8_t)Fixed_Array_Value;

			object_vector[array_index]
				.asArray()
				.deserialize(data, ++index, count)
				;
			index--;
		}
		else if(byte == (uint8_t)Format::Array16)
		{
			Convert_Byte1 = data[++index];
			Convert_Byte0 = data[++index];

			const size_t array_index = appendArray_(object_vector);
			const size_t count = Convert.uint16;

			object_vector[array_index]
				.asArray()
				.deserialize(data, ++index, count)
				;
			index--;
		}
		else if(byte == (uint8_t)Format::Array32)
		{
			Convert_Byte3 = data[++index];
			Convert_Byte2 = data[++index];
			Convert_Byte1 = data[++index];
			Convert_Byte0 = data[++index];

			const size_t array_index = appendArray_(object_vector);
			const size_t count = Convert.uint32;

			object_vector[array_index]
				.asArray()
				.deserialize(data, ++index, count)
				;
			index--;
		}
*/

		index++;
	}
#endif
}

// }}}
// {{{ Array

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

	object_vector.emplace_back(value);

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: Bool")
{
	Array array;

	size_t index = array.append(true);

	CHECK(index == 0);
	CHECK(array.size() == 1);

	index = array.append(false);

	CHECK(index == 1);
	CHECK(array.size() == 2);

	// Check serialized data

	std::vector<uint8_t> data = array.serialize();
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

	object_vector.emplace_back(value);

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: int64_t")
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

	std::vector<uint8_t> data = array.serialize();

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

	object_vector.emplace_back(value);

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: uint64_t")
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

	std::vector<uint8_t> data = array.serialize();

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

	object_vector.emplace_back(value);

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: float")
{
	const float f32_min = std::numeric_limits<float>::min();
	const float f32_max = std::numeric_limits<float>::max();
	size_t count = 0;

	Array array;
	array.append(f32_min); count++;
	array.append(f32_max); count++;

	CHECK(array.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = array.serialize();

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

	object_vector.emplace_back(value);

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: double")
{
	const double f64_min = std::numeric_limits<double>::min();
	const double f64_max = std::numeric_limits<double>::max();
	size_t count = 0;

	Array array;
	array.append(f64_min); count++;
	array.append(f64_max); count++;

	CHECK(array.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = array.serialize();

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

	object_vector.emplace_back(std::string(value));

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: string")
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

	std::vector<uint8_t> data = array.serialize();

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

	object_vector.emplace_back(value);

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: binary (copy)")
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

	std::vector<uint8_t> data = array.serialize();

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

	object_vector.emplace_back(std::move(value));

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: binary (move)")
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

	std::vector<uint8_t> data = array.serialize();

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
 * The \p value will be appended to the contents of this Array.
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
size_t Array::append(const Array& array
	) noexcept
{
	size_t index = object_vector.size();
	object_vector.emplace_back(Array{});

	Array& sub_array = object_vector[index].asArray();
	sub_array.object_vector = array.object_vector;

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: Array (copy)")
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

	std::vector<uint8_t> data = array.serialize();

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
 * The \p value will be appended to the contents of this Array.
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
size_t Array::append(Array& array
	) noexcept
{
	size_t index = object_vector.size();
	object_vector.emplace_back(Array{});

	Array& sub_array = object_vector[index].asArray();
	sub_array.object_vector = std::move(array.object_vector);

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: Array (move)")
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

	std::vector<uint8_t> data = array.serialize();

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
size_t Array::append(const Object& object
	) noexcept
{
	const size_t index = object_vector.size();

	object_vector.push_back(object);

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: Object (copy)")
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

	std::vector<uint8_t> data = array.serialize();

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
size_t Array::append(Object& object
	) noexcept
{
	const size_t index = object_vector.size();

	object_vector.push_back(std::move(object));
	object = {};

	return index;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: Object (move)")
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

	std::vector<uint8_t> data = array.serialize();

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
TEST_CASE("Append: Null")
{
	const size_t count = std::numeric_limits<uint16_t>::max() + 1;
	Array array;

	for(size_t i = 0; i < count; i++)
	{
		array.append(Object{true});
	}

	CHECK(array.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = array.serialize();

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
 * \fn zakero::messagepack::Array::object(const size_t) const
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
std::vector<uint8_t> Array::serialize() const noexcept
{
	std::vector<uint8_t> vector;

	serialize_(*this, vector);

	return vector;
}


// No tests needed, functionality is tested else where.


// }}} Array
// {{{ Object

/**
 * \struct Object
 *
 * \brief A Data Object.
 *
 * This structure contains the basic data-types supported by MessagePack.
 */

/**
 * \var Object::Type
 *
 * \brief The data-type.
 *
 * For the primitive data-types, there is a one-to-one correlation to the 
 * anonymous union. If the \p type is DataType::Bool, then the `bool_` value 
 * should be used.
 */
std::vector<uint8_t> Object::serialize() const noexcept
{
	std::vector<uint8_t> vector;

	if(this->isNull())
	{
		vector.push_back((uint8_t)Format::Nill);
	}
	else if(this->is<bool>())
	{
		const int64_t value = this->as<bool>();

		vector.push_back(value
			? (uint8_t)Format::True
			: (uint8_t)Format::False
			);
	}
	else if(this->is<int64_t>())
	{
		const int64_t value = this->as<int64_t>();

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
	else if(this->is<uint64_t>())
	{
		const uint64_t value = this->as<uint64_t>();

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
	else if(this->is<float>())
	{
		float value = this->as<float>();

		vector.reserve(vector.size() + 5);

		vector.push_back((uint8_t)Format::Float32);

		Convert.float32 = value;
		vector.push_back(Convert_Byte3);
		vector.push_back(Convert_Byte2);
		vector.push_back(Convert_Byte1);
		vector.push_back(Convert_Byte0);
	}
	else if(this->is<double>())
	{
		double value = this->as<double>();

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
	else if(this->isString())
	{
		const std::string& value = this->asString();

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
	else if(this->isBinary())
	{
		const std::vector<uint8_t>& value = this->asBinary();

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
	else if(this->isArray())
	{
		const messagepack::Array& array = this->asArray();

		serialize_(array, vector);
	}

	return vector;
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{

TEST_CASE("Object: Nill")
{
	Object object = {};
	CHECK(object.isNull());

	std::vector<uint8_t> data = object.serialize();
	CHECK(data.size() == 1);

	size_t index = 0;
	CHECK(data[index] == (uint8_t)Format::Nill);

	object = deserialize(data);
	CHECK(object.isNull());
}


TEST_CASE("Object: Bool")
{
	Object               object;
	std::vector<uint8_t> data;
	size_t               index = 0;

	object = {true};
	CHECK(object.is<bool>());

	data = object.serialize();
	CHECK(data.size() == 1);

	index = 0;
	CHECK(data[index] == (uint8_t)Format::True);

	object = deserialize(data);
	CHECK(object.is<bool>());
	CHECK(object.as<bool>() == true);

	// --------------------------------------------

	object = {false};
	CHECK(object.is<bool>());

	data = object.serialize();
	CHECK(data.size() == 1);
	index = 0;
	CHECK(data[index] == (uint8_t)Format::False);

	object = deserialize(data);
	CHECK(object.is<bool>());
	CHECK(object.as<bool>() == false);
}


TEST_CASE("Object: Fixed_Int")
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

		data = object.serialize();
		CHECK(data.size() == 1);

		index = 0;
		CHECK((data[index] & Fixed_Int_Pos_Mask) == (uint8_t)Format::Fixed_Int_Pos);
		CHECK((data[index] & Fixed_Int_Pos_Value) == fixint_zero);

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

		data = object.serialize();
		CHECK(data.size() == 1);

		index = 0;
		CHECK((data[index] & Fixed_Int_Pos_Mask) == (uint8_t)Format::Fixed_Int_Pos);
		CHECK((data[index] & Fixed_Int_Pos_Value) == fixint_max);

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

		data = object.serialize();
		CHECK(data.size() == 1);

		index = 0;
		CHECK((data[index] & Fixed_Int_Neg_Mask) == (uint8_t)Format::Fixed_Int_Neg);
		CHECK(((data[index] & Fixed_Int_Neg_Value) - 32) == fixint_min);

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

		data = object.serialize();
		CHECK(data.size() == 1);

		index = 0;
		CHECK((data[index] & Fixed_Int_Pos_Mask) == (uint8_t)Format::Fixed_Int_Pos);
		CHECK((data[index] & Fixed_Int_Pos_Value) == fixint_p24);

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

		data = object.serialize();
		CHECK(data.size() == 1);

		index = 0;
		CHECK((data[index] & Fixed_Int_Neg_Mask) == (uint8_t)Format::Fixed_Int_Neg);
		CHECK(((data[index] & Fixed_Int_Neg_Value) - 32) == fixint_n24);

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == fixint_n24);
	}
}


TEST_CASE("Object: Int8")
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

		data = object.serialize();
		CHECK(data.size() == 2);

		index = 0;
		CHECK(data[index] == (uint8_t)Format::Int8);
		index++;
		CHECK((int8_t)data[index] == i8_min);

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

		data = object.serialize();
		CHECK(data.size() == 2);

		index = 0;
		CHECK(data[index] == (uint8_t)Format::Int8);
		index++;
		CHECK((int8_t)data[index] == i8_min);

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == i8_min);
	}
}


TEST_CASE("Object: Int16")
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

		data = object.serialize();
		CHECK(data.size() == 3);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int16);

		Convert.uint64 = 0;
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int16 == i16_min);

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

		data = object.serialize();
		CHECK(data.size() == 3);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int16);

		Convert.uint64 = 0;
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int16 == i16_min);

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

		data = object.serialize();
		CHECK(data.size() == 3);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int16);

		Convert.uint64 = 0;
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int16 == i16_max);

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

		data = object.serialize();
		CHECK(data.size() == 3);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int16);

		Convert.uint64 = 0;
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int16 == i16_max);

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == i16_max);
	}
}


TEST_CASE("Object: Int32")
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

		data = object.serialize();
		CHECK(data.size() == 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int32);

		Convert.uint64 = 0;
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int32 == i32_min);

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

		data = object.serialize();
		CHECK(data.size() == 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int32);

		Convert.uint64 = 0;
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int32 == i32_min);

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

		data = object.serialize();
		CHECK(data.size() == 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int32);

		Convert.uint64 = 0;
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int32 == i32_max);

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

		data = object.serialize();
		CHECK(data.size() == 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Int32);

		Convert.uint64 = 0;
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.int32 == i32_max);

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == i32_max);
	}
}


TEST_CASE("Object: Int64")
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

		data = object.serialize();
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

		data = object.serialize();
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

		data = object.serialize();
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

		data = object.serialize();
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

		object = deserialize(data);
		CHECK(object.is<int64_t>());
		CHECK(object.as<int64_t>() == i64_max);
	}
}


TEST_CASE("Object: Uint8")
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

		data = object.serialize();
		CHECK(data.size() == 2);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Uint8);
		CHECK(data[index++] == u8_min);

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

		data = object.serialize();
		CHECK(data.size() == 2);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Uint8);
		CHECK(data[index++] == u8_max);

		object = deserialize(data);
		CHECK(object.is<uint64_t>());
		CHECK(object.as<uint64_t>() == u8_max);
	}
}


TEST_CASE("Object: Uint16")
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

		data = object.serialize();
		CHECK(data.size() == 3);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Uint16);
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.uint16 == u16_min);

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

		data = object.serialize();
		CHECK(data.size() == 3);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Uint16);
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.uint16 == u16_max);

		object = deserialize(data);
		CHECK(object.is<uint64_t>());
		CHECK(object.as<uint64_t>() == u16_max);
	}
}


TEST_CASE("Object: Uint32")
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

		data = object.serialize();
		CHECK(data.size() == 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Uint32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.uint32 == u32_min);

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

		data = object.serialize();
		CHECK(data.size() == 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Uint32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.uint32 == u32_max);

		object = deserialize(data);
		CHECK(object.is<uint64_t>());
		CHECK(object.as<uint64_t>() == u32_max);
	}
}


TEST_CASE("Object: Uint64")
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

		data = object.serialize();
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

		data = object.serialize();
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

		object = deserialize(data);
		CHECK(object.is<uint64_t>());
		CHECK(object.as<uint64_t>() == u64_max);
	}
}


TEST_CASE("Object: Float32")
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

		data = object.serialize();
		CHECK(data.size() == 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Float32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.float32 == f32_zero);

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

		data = object.serialize();
		CHECK(data.size() == 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Float32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.float32 == f32_min);

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

		data = object.serialize();
		CHECK(data.size() == 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Float32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		CHECK(Convert.float32 == f32_max);

		object = deserialize(data);
		CHECK(object.is<float>());
		CHECK(object.as<float>() == f32_max);
	}
}


TEST_CASE("Object: Float64")
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

		data = object.serialize();
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

		data = object.serialize();
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

		data = object.serialize();
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

		object = deserialize(data);
		CHECK(object.is<double>());
		CHECK(object.as<double>() == f64_max);
	}
}


TEST_CASE("Object: Fixed_Str")
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

		data = object.serialize();
		CHECK(data.size() == 1);

		index = 0;
		CHECK((data[index] & Fixed_Str_Mask) == (uint8_t)Format::Fixed_Str);
		str_len = data[index] & Fixed_Str_Value;
		CHECK(str_len == string.size());

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

		data = object.serialize();
		CHECK(data.size() == string.size() + 1);

		index = 0;
		CHECK((data[index] & Fixed_Str_Mask) == (uint8_t)Format::Fixed_Str);
		str_len = data[index] & Fixed_Str_Value;
		CHECK(str_len == string.size());

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

		data = object.serialize();
		CHECK(data.size() == string.size() + 1);

		index = 0;
		CHECK((data[index] & Fixed_Str_Mask) == (uint8_t)Format::Fixed_Str);
		str_len = data[index] & Fixed_Str_Value;
		CHECK(str_len == string.size());

		object = deserialize(data);
		CHECK(object.is<std::string>());
		CHECK(object.as<std::string>() == string);
	}
}


TEST_CASE("Object: Str8")
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

		data = object.serialize();
		CHECK(data.size() == string.size() + 2);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Str8);
		str_len = data[index++];
		CHECK(str_len == string.size());

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

		data = object.serialize();
		CHECK(data.size() == (string.size() + 2));

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Str8);
		str_len = data[index++];
		CHECK(str_len == string.size());

		object = deserialize(data);
		CHECK(object.is<std::string>());
		CHECK(object.as<std::string>() == string);
	}
}


TEST_CASE("Object: Str16")
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

		data = object.serialize();
		CHECK(data.size() == string.size() + 3);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Str16);
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		str_len = Convert.uint16;
		CHECK(str_len == string.size());

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

		data = object.serialize();
		CHECK(data.size() == (string.size() + 3));

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Str16);
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		str_len = Convert.uint16;
		CHECK(str_len == string.size());

		object = deserialize(data);
		CHECK(object.is<std::string>());
		CHECK(object.as<std::string>() == string);
	}
}


TEST_CASE("Object: Str32")
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

		data = object.serialize();
		CHECK(data.size() == string.size() + 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Str32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		str_len = Convert.uint32;
		CHECK(str_len == string.size());

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

		data = object.serialize();
		CHECK(data.size() == (string.size() + 3));

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Str32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		str_len = Convert.uint32;
		CHECK(str_len == string.size());

		object = deserialize(data);
		CHECK(object.is<std::string>());
		CHECK(object.as<std::string>() == string);
		 */
	}
}


TEST_CASE("Object: Bin8")
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

		data = object.serialize();
		CHECK(data.size() == bin.size() + 2);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Bin8);
		bin_len = data[index++];
		CHECK(bin_len == bin.size());

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

		data = object.serialize();
		CHECK(data.size() == bin.size() + 2);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Bin8);
		bin_len = data[index++];
		CHECK(bin_len == bin.size());

		object = deserialize(data);
		CHECK(object.isBinary());

		const std::vector<uint8_t>& vector = object.asBinary();
		for(size_t i = 0; i < bin_len; i++)
		{
			CHECK(vector[i] == bin[i]);
		}
	}
}


TEST_CASE("Object: Bin16")
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

		data = object.serialize();
		CHECK(data.size() == bin.size() + 3);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Bin16);
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		bin_len = Convert.uint16;
		CHECK(bin_len == bin.size());

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

		data = object.serialize();
		CHECK(data.size() == bin.size() + 3);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Bin16);
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		bin_len = Convert.uint16;
		CHECK(bin_len == bin.size());

		object = deserialize(data);
		CHECK(object.isBinary());

		const std::vector<uint8_t>& vector = object.asBinary();
		for(size_t i = 0; i < bin_len; i++)
		{
			CHECK(vector[i] == bin[i]);
		}
	}
}


TEST_CASE("Object: Bin32")
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

		data = object.serialize();
		CHECK(data.size() == bin.size() + 5);

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Bin32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		bin_len = Convert.uint32;
		CHECK(bin_len == bin.size());

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

		data = object.serialize();
		CHECK(data.size() == (bin.size() + 5));

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Bin32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		bin_len = Convert.uint32;
		CHECK(bin_len == bin.size());

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


TEST_CASE("Object: Fixed_Array")
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

		data = object.serialize();
		CHECK(data.size() == 1);

		index = 0;
		CHECK((data[index] & Fixed_Array_Mask) == (uint8_t)Format::Fixed_Array);
		array_len = data[index] & Fixed_Array_Value;
		CHECK(array_len == 0);

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

		data = object.serialize();
		CHECK(data.size() == (array_len + 1));

		index = 0;
		CHECK((data[index] & Fixed_Array_Mask) == (uint8_t)Format::Fixed_Array);
		array_len = data[index] & Fixed_Array_Value;
		CHECK(array_len == 15);

		object = deserialize(data);
		CHECK(object.isArray());
		CHECK(object.asArray().size() == 15);

		for(size_t i = 0; i < 15; i++)
		{
			CHECK(object.asArray().object(i).isNull());
		}
	}
}


TEST_CASE("Object: Array16")
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

		data = object.serialize();
		CHECK(data.size() == (array_len + 3));

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Array16);
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		array_len = Convert.uint16;
		CHECK(array_len == 16);

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

		data = object.serialize();
		CHECK(data.size() == (array_len + 3));

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Array16);
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		array_len = Convert.uint16;
		CHECK(array_len == std::numeric_limits<uint16_t>::max());

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


TEST_CASE("Object: Array32")
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

		data = object.serialize();
		CHECK(data.size() == (array_len + 5));

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Array32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		array_len = Convert.uint32;
		CHECK(array_len == (std::numeric_limits<uint16_t>::max() + 1));

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

		data = object.serialize();
		CHECK(data.size() == (array_len + 5));

		index = 0;
		CHECK(data[index++] == (uint8_t)Format::Array32);
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		array_len = Convert.uint32;
		CHECK(array_len == std::numeric_limits<uint32_t>::max());

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


// }}} MessagePack::Object
// {{{ Utilities

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
 * \todo Add an index parameter so that last parsed data location will be 
 * available.
 *
 * \todo Add error codes.
 *       - Error_None
 *       - Error_Bad_Format_Type
 *       - Error_Incomplete
 *
 * \todo Can the contents of the for-loop be rewritten as a switch-statement?
 */
Object deserialize(const std::vector<uint8_t>& data ///< The packed data
	) noexcept
{
	size_t index = 0;

	return deserialize(data, index);
}


Object deserialize(const std::vector<uint8_t>& data  ///< The packed data
	, size_t&                              index ///< The starting index
	) noexcept
{
	const uint8_t byte = data[index++];

	if(byte == (uint8_t)Format::Nill)
	{
		return Object{};
	}

	if(byte == (uint8_t)Format::False)
	{
		return Object{false};
	}

	if(byte == (uint8_t)Format::True)
	{
		return Object{true};
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
	
	if(byte == (uint8_t)Format::Int8)
	{
		const int64_t value = (int8_t)data[index++];

		return Object{value};
	}
	
	if(byte == (uint8_t)Format::Int16)
	{
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];

		const int64_t value = Convert.int16;

		return Object{value};
	}

	if(byte == (uint8_t)Format::Int32)
	{
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];

		const int64_t value = Convert.int32;

		return Object{value};
	}
	
	if(byte == (uint8_t)Format::Int64)
	{
		Convert_Byte7 = data[index++];
		Convert_Byte6 = data[index++];
		Convert_Byte5 = data[index++];
		Convert_Byte4 = data[index++];
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];

		const int64_t value = Convert.int64;

		return Object{value};
	}
	
	if(byte == (uint8_t)Format::Uint8)
	{
		const uint64_t value = data[index++];

		return Object{value};
	}
	
	if(byte == (uint8_t)Format::Uint16)
	{
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];

		const uint64_t value = Convert.uint16;

		return Object{value};
	}
	
	if(byte == (uint8_t)Format::Uint32)
	{
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];

		const uint64_t value = Convert.uint32;

		return Object{value};
	}

	if(byte == (uint8_t)Format::Uint64)
	{
		Convert_Byte7 = data[index++];
		Convert_Byte6 = data[index++];
		Convert_Byte5 = data[index++];
		Convert_Byte4 = data[index++];
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];

		const uint64_t value = Convert.uint64;

		return Object{value};
	}
	
	if(byte == (uint8_t)Format::Float32)
	{
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];

		const float value = Convert.float32;

		return Object{value};
	}
	
	if(byte == (uint8_t)Format::Float64)
	{
		Convert_Byte7 = data[index++];
		Convert_Byte6 = data[index++];
		Convert_Byte5 = data[index++];
		Convert_Byte4 = data[index++];
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];

		const double value = Convert.float64;

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
	
	if(byte == (uint8_t)Format::Str8)
	{
		const size_t length = data[index++];

		std::string_view str((char*)&data[index], length);

		index += length;

		return Object{std::string(str)};
	}
	
	if(byte == (uint8_t)Format::Str16)
	{
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		const size_t length = Convert.uint16;

		const std::string_view str((char*)&data[index], length);

		index += length;

		return Object{std::string(str)};
	}
	
	if(byte == (uint8_t)Format::Str32)
	{
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		const size_t length = Convert.uint32;

		const std::string_view str((char*)&data[index], length);

		index += length;

		return Object{std::string(str)};
	}
	
	if(byte == (uint8_t)Format::Bin8)
	{
		const size_t length = data[index++];

		std::vector<uint8_t> vector(length);
		memcpy((void*)vector.data(), (void*)&data[index], length);

		index += length;

		return Object{std::move(vector)};
	}
	
	if(byte == (uint8_t)Format::Bin16)
	{
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		const size_t length = Convert.uint16;

		std::vector<uint8_t> vector(length);
		memcpy((void*)vector.data(), (void*)&data[index], length);

		index += length;

		return Object{std::move(vector)};
	}
	
	if(byte == (uint8_t)Format::Bin32)
	{
		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		const size_t length = Convert.uint32;

		std::vector<uint8_t> vector(length);
		memcpy((void*)vector.data(), (void*)&data[index], length);

		index += length;

		return Object{std::move(vector)};
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
	
	if(byte == (uint8_t)Format::Array16)
	{
		Object object = {Array{}};

		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		const size_t count = Convert.uint16;

		for(size_t i = 0; i < count; i++)
		{
			object.asArray().append(deserialize(data, index));
		}

		return object;
	}
	
	if(byte == (uint8_t)Format::Array32)
	{
		Object object = {Array{}};

		Convert_Byte3 = data[index++];
		Convert_Byte2 = data[index++];
		Convert_Byte1 = data[index++];
		Convert_Byte0 = data[index++];
		const size_t count = Convert.uint32;

		for(size_t i = 0; i < count; i++)
		{
			object.asArray().append(deserialize(data, index));
		}

		return object;
	}

	return {};
}


/**
 * \brief Convert to string.
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
 * \brief Convert to string.
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
		s += "'type': 'zakero::messagepack::Array', 'value': ["
			;

		std::string prefix = " ";

		const zakero::messagepack::Array& array = object.asArray();
		for(size_t i = 0; i < array.size(); i++)
		{
			s += prefix + zakero::messagepack::to_string(array.object(i));

			if(i == 0)
			{
				prefix = ", ";
			}
		}

		s += " ]";
	}

	s += " }";

	return s;
}

// }}}
} // namespace zakero::messagepack
} // namespace zakero

// {{{ Operators

/**
 * \brief OStream operator.
 *
 * The \p point object will be written to the request \p stream as a JSON 
 * formatted string.
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
 * The \p point object will be written to the request \p stream as a JSON 
 * formatted string.
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


// }}}

#endif // ZAKERO_MESSAGEPACK_IMPLEMENTATION

// }}}

#endif // zakero_MessagePack_h
