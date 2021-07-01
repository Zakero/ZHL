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
	/* Name      Type               */ \
	X(Bool     , bool                ) \
	X(Int64_t  , int64_t             ) \
	X(Uint64_t , uint64_t            ) \
	X(Float    , float               ) \
	X(Double   , double              ) \
	X(String   , std::string         ) \
	X(Binary   , std::vector<uint8_t>) \
	X(Array    , MessagePack::Array  ) \

// }}}


namespace zakero
{
	// {{{ Declaration

	class MessagePack
	{
		public:
			struct Object;

			class Array
			{
				public:
					              size_t        append(const bool) noexcept;
					              size_t        append(const int64_t) noexcept;
					              size_t        append(const uint64_t) noexcept;
					              size_t        append(const float) noexcept;
					              size_t        append(const double) noexcept;
					              size_t        append(const std::string_view) noexcept;
					              size_t        append(const std::vector<uint8_t>&) noexcept;
					              size_t        append(std::vector<uint8_t>&&) noexcept;
					              size_t        append(const uint8_t*, const size_t) noexcept;
					              size_t        append(const Array&) noexcept;
					              size_t        appendArray() noexcept;
					              size_t        appendNull() noexcept;

					[[nodiscard]] Object&       object(const size_t index) noexcept;
					[[nodiscard]] const Object& object(const size_t index) const noexcept;

					              void          clear() noexcept;
					[[nodiscard]] size_t        size() const noexcept;

			                              void          deserialize(const std::vector<uint8_t>&, size_t&, const size_t) noexcept;

				private:
					std::vector<MessagePack::Object> object_vector = {};
			};

			struct Object
			{
				enum class Type
				{
					Null = 0
#define X(name_, type_) \
					, name_ \

					ZAKERO_MESSAGEPACK__OBJECT_TYPE
#undef X
				};

				std::variant<std::monostate
#define X(name_, type_) \
					, type_ \

					ZAKERO_MESSAGEPACK__OBJECT_TYPE
#undef X
					> value;

				template<typename T> [[nodiscard]] T&                          as() noexcept { return std::get<T>(value); };
				template<typename T> [[nodiscard]] const T&                    as() const noexcept { return std::get<T>(value); };
				                     [[nodiscard]] MessagePack::Array&         asArray() noexcept { return std::get<MessagePack::Array>(value); };
				                     [[nodiscard]] const MessagePack::Array&   asArray() const noexcept { return std::get<MessagePack::Array>(value); };
				                     [[nodiscard]] std::vector<uint8_t>&       asBinary() noexcept { return std::get<std::vector<uint8_t>>(value); };
				                     [[nodiscard]] const std::vector<uint8_t>& asBinary() const noexcept { return std::get<std::vector<uint8_t>>(value); };
				                     [[nodiscard]] const std::string&          asString() const noexcept { return std::get<std::string>(value); };

				template<typename T> [[nodiscard]] constexpr bool              is() const noexcept { return std::holds_alternative<T>(value); };
				                     [[nodiscard]] constexpr bool              isArray() const noexcept { return std::holds_alternative<MessagePack::Array>(value); };
				                     [[nodiscard]] constexpr bool              isBinary() const noexcept { return std::holds_alternative<std::vector<uint8_t>>(value); };
				                     [[nodiscard]] constexpr bool              isNull() const noexcept { return std::holds_alternative<std::monostate>(value); };
				                     [[nodiscard]] constexpr bool              isString() const noexcept { return std::holds_alternative<std::string>(value); };
						     
						     [[nodiscard]] constexpr Object::Type      type() const noexcept { return static_cast<MessagePack::Object::Type>(value.index()); };
			};


			MessagePack() = default;

			[[nodiscard]] static std::vector<uint8_t> serialize(const MessagePack&) noexcept;
			              static MessagePack          deserialize(const std::vector<uint8_t>&) noexcept;

			size_t                append(const bool) noexcept;
			size_t                append(const int64_t) noexcept;
			size_t                append(const uint64_t) noexcept;
			size_t                append(const float) noexcept;
			size_t                append(const double) noexcept;
			size_t                append(const std::string_view) noexcept;
			size_t                append(const std::vector<uint8_t>&) noexcept;
			size_t                append(std::vector<uint8_t>&&) noexcept;
			size_t                append(const uint8_t*, const size_t) noexcept;
			size_t                append(const Array&) noexcept;
			size_t                appendArray() noexcept;
			//void                  appendExtension(int8_t, std::vector<uint8_t>&, size_t* = nullptr);
			//void                  appendExtension(int8_t, uint8_t*, size_t, size_t* = nullptr);
			//void                  appendMap(size_t* = nullptr);
			size_t                appendNull() noexcept;
			//void                  appendTimeStamp(uint64_t);

			[[nodiscard]] Object&       object(const size_t) noexcept;
			[[nodiscard]] const Object& object(const size_t) const noexcept;

			void                        clear() noexcept;
			[[nodiscard]] size_t        size() const noexcept;

		private:
			std::vector<MessagePack::Object> object_vector = {};
	}; // class MessagePack

	// }}}
	// {{{ Utilities

	[[nodiscard]] std::string to_string(const MessagePack::Object::Type) noexcept;

	// }}}
}

// {{{ Operators

std::ostream& operator<<(std::ostream&, const zakero::MessagePack::Object::Type) noexcept;

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


	using VectorObject = std::vector<MessagePack::Object>;

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

		vector.emplace_back(MessagePack::Array{});

		return index;
	}


	inline size_t appendNull_(VectorObject& vector ///< The vector
		) noexcept
	{
		const size_t index = vector.size();

		vector.emplace_back();

		return index;
	}


	void serialize_(const MessagePack::Object& object
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
			const MessagePack::Array& array = object.asArray();
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


	void deserialize_(const std::vector<uint8_t>& data
		, size_t&                             index
		, std::vector<MessagePack::Object>&   object_vector
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

		index++;
	}
}

// }}}
// {{{ MessagePack

/**
 * \brief Append a boolean value.
 *
 * The boolean \p value will be appended to the current contents of the 
 * MessagePack data. If \p index is not \nullptr, the index location of the \p 
 * value will be stored in \p index.
 *
 * \parcode
 * zakero::MessagePack message_pack;
 * size_t index;
 * message_pack.append(true, &index);
 * \endparcode
 */
size_t MessagePack::append(const bool value ///< The value to add
	) noexcept
{
	return append_(value, object_vector);
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: Bool")
{
	MessagePack mesg_pack;

	size_t index = mesg_pack.append(true);

	CHECK(index == 0);
	CHECK(mesg_pack.size() == 1);

	index = mesg_pack.append(false);

	CHECK(index == 1);
	CHECK(mesg_pack.size() == 2);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	CHECK(data.size() == 2);
	CHECK(data[0] == (uint8_t)Format::True);
	CHECK(data[1] == (uint8_t)Format::False);

	// Check deserialized data

	mesg_pack.deserialize(data);
	CHECK(mesg_pack.size() == 2);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		CHECK(object.is<bool>() == true);
		CHECK(std::get<bool>(object.value) == true);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		CHECK(object.is<bool>() == true);
		CHECK(std::get<bool>(object.value) == false);
	}
}
#endif // }}}


/**
 * \brief Append a signed integer value.
 *
 * The \p value will be appended to the current contents of the MessagePack 
 * data. If \p index is not \nullptr, the index location of the \p value will 
 * be stored in \p index.
 *
 * \parcode
 * zakero::MessagePack message_pack;
 * size_t index;
 * message_pack.append(int64_t(0), &index);
 * \endparcode
 */
size_t MessagePack::append(const int64_t value ///< The value to add
	) noexcept
{
	return append_(value, object_vector);
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: Fixed_Int")
{
	MessagePack mesg_pack;

	const int64_t fixint_zero = 0;
	const int64_t fixint_max  = 127;
	const int64_t fixint_min  = -32;
	const int64_t fixint_p24  = 24;
	const int64_t fixint_n24  = -24;
	size_t count = 0;

	mesg_pack.append(fixint_zero); count++;
	mesg_pack.append(fixint_max);  count++;
	mesg_pack.append(fixint_min);  count++;
	mesg_pack.append(fixint_p24);  count++;
	mesg_pack.append(fixint_n24);  count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * sizeof(uint8_t));

	CHECK(data.size() == size);
	CHECK((data[0] & Fixed_Int_Pos_Mask) == ((uint8_t)Format::Fixed_Int_Pos));
	CHECK((data[0] & Fixed_Int_Pos_Value) == fixint_zero);
	CHECK((data[1] & Fixed_Int_Pos_Mask) == ((uint8_t)Format::Fixed_Int_Pos));
	CHECK((data[1] & Fixed_Int_Pos_Value) == fixint_max);
	CHECK((data[2] & Fixed_Int_Neg_Mask) == ((uint8_t)Format::Fixed_Int_Neg));
	CHECK(((data[2] & Fixed_Int_Neg_Value) - 32) == fixint_min);
	CHECK((data[3] & Fixed_Int_Pos_Mask) == ((uint8_t)Format::Fixed_Int_Pos));
	CHECK((data[3] & Fixed_Int_Pos_Value) == fixint_p24);
	CHECK((data[4] & Fixed_Int_Neg_Mask) == ((uint8_t)Format::Fixed_Int_Neg));
	CHECK(((data[4] & Fixed_Int_Neg_Value) - 32) == fixint_n24);

	// Check deserialized data

	mesg_pack.deserialize(data);
	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == fixint_zero);
		CHECK(std::get<int64_t>(object.value) == fixint_zero);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == fixint_max);
		CHECK(std::get<int64_t>(object.value) == fixint_max);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(2);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == fixint_min);
		CHECK(std::get<int64_t>(object.value) == fixint_min);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(3);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == fixint_p24);
		CHECK(std::get<int64_t>(object.value) == fixint_p24);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(4);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == fixint_n24);
		CHECK(std::get<int64_t>(object.value) == fixint_n24);
	}
}

TEST_CASE("Append: Int8")
{
	MessagePack mesg_pack;

	// Don't test the maximum Int8 value 127
	// because that is handled by Fixed_Int_Pos

	const int64_t i8_min1 = std::numeric_limits<int8_t>::min();
	const int64_t i8_min2 = -33;
	size_t count = 0;

	mesg_pack.append(i8_min1); count++;
	mesg_pack.append(i8_min2); count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 2 * sizeof(uint8_t));
	CHECK(data.size() == size);

	CHECK(data[0] == ((uint8_t)Format::Int8));
	CHECK((int8_t)data[1] == i8_min1);
	CHECK(data[2] == ((uint8_t)Format::Int8));
	CHECK((int8_t)data[3] == i8_min2);

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == i8_min1);
		CHECK(std::get<int64_t>(object.value) == i8_min1);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == i8_min2);
		CHECK(std::get<int64_t>(object.value) == i8_min2);
	}
}

TEST_CASE("Append: Int16")
{
	MessagePack mesg_pack;

	const int64_t i16_min1 = std::numeric_limits<int16_t>::min();
	const int64_t i16_max1 = std::numeric_limits<int16_t>::max();
	const int64_t i16_min2 = std::numeric_limits<int8_t>::min() - (int64_t)1;
	const int64_t i16_max2 = std::numeric_limits<int8_t>::max() + (int64_t)1;
	size_t count = 0;

	mesg_pack.clear();
	mesg_pack.append(i16_min1); count++;
	mesg_pack.append(i16_max1); count++;
	mesg_pack.append(i16_min2); count++;
	mesg_pack.append(i16_max2); count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 3 * sizeof(uint8_t));
	CHECK(data.size() == size);

	CHECK(data[0] == ((uint8_t)Format::Int16));

	Convert.uint64 = 0;
	Convert_Byte1 = data[1];
	Convert_Byte0 = data[2];
	CHECK(Convert.int16 == i16_min1);

	CHECK(data[3] == ((uint8_t)Format::Int16));
	Convert_Byte1 = data[4];
	Convert_Byte0 = data[5];
	CHECK(Convert.int16 == i16_max1);

	CHECK(data[6] == ((uint8_t)Format::Int16));
	Convert_Byte1 = data[7];
	Convert_Byte0 = data[8];
	CHECK(Convert.int16 == i16_min2);

	CHECK(data[9] == ((uint8_t)Format::Int16));
	Convert_Byte1 = data[10];
	Convert_Byte0 = data[11];
	CHECK(Convert.int16 == i16_max2);

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == i16_min1);
		CHECK(std::get<int64_t>(object.value) == i16_min1);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == i16_max1);
		CHECK(std::get<int64_t>(object.value) == i16_max1);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(2);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == i16_min2);
		CHECK(std::get<int64_t>(object.value) == i16_min2);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(3);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == i16_max2);
		CHECK(std::get<int64_t>(object.value) == i16_max2);
	}
}

TEST_CASE("Append: Int32")
{
	MessagePack mesg_pack;

	const int64_t i32_min1 = std::numeric_limits<int32_t>::min();
	const int64_t i32_max1 = std::numeric_limits<int32_t>::max();
	const int64_t i32_min2 = std::numeric_limits<int16_t>::min() - (int64_t)1;
	const int64_t i32_max2 = std::numeric_limits<int16_t>::max() + (int64_t)1;
	size_t count = 0;

	mesg_pack.clear();
	mesg_pack.append(i32_min1); count++;
	mesg_pack.append(i32_max1); count++;
	mesg_pack.append(i32_min2); count++;
	mesg_pack.append(i32_max2); count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 5 * sizeof(uint8_t));
	CHECK(data.size() == size);

	CHECK(data[0] == ((uint8_t)Format::Int32));

	Convert.uint64 = 0;
	Convert_Byte3 = data[1];
	Convert_Byte2 = data[2];
	Convert_Byte1 = data[3];
	Convert_Byte0 = data[4];
	CHECK(Convert.int32 == i32_min1);

	CHECK(data[5] == ((uint8_t)Format::Int32));
	Convert_Byte3 = data[6];
	Convert_Byte2 = data[7];
	Convert_Byte1 = data[8];
	Convert_Byte0 = data[9];
	CHECK(Convert.int32 == i32_max1);

	CHECK(data[10] == ((uint8_t)Format::Int32));
	Convert_Byte3 = data[11];
	Convert_Byte2 = data[12];
	Convert_Byte1 = data[13];
	Convert_Byte0 = data[14];
	CHECK(Convert.int32 == i32_min2);

	CHECK(data[15] == ((uint8_t)Format::Int32));
	Convert_Byte3 = data[16];
	Convert_Byte2 = data[17];
	Convert_Byte1 = data[18];
	Convert_Byte0 = data[19];
	CHECK(Convert.int32 == i32_max2);

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == i32_min1);
		CHECK(std::get<int64_t>(object.value) == i32_min1);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == i32_max1);
		CHECK(std::get<int64_t>(object.value) == i32_max1);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(2);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == i32_min2);
		CHECK(std::get<int64_t>(object.value) == i32_min2);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(3);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == i32_max2);
		CHECK(std::get<int64_t>(object.value) == i32_max2);
	}
}

TEST_CASE("Append: Int64")
{
	MessagePack mesg_pack;

	const int64_t i64_min1 = std::numeric_limits<int64_t>::min();
	const int64_t i64_max1 = std::numeric_limits<int64_t>::max();
	const int64_t i64_min2 = std::numeric_limits<int32_t>::min() - (int64_t)1;
	const int64_t i64_max2 = std::numeric_limits<int32_t>::max() + (int64_t)1;
	size_t count = 0;

	mesg_pack.clear();
	mesg_pack.append(i64_min1); count++;
	mesg_pack.append(i64_max1); count++;
	mesg_pack.append(i64_min2); count++;
	mesg_pack.append(i64_max2); count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 9 * sizeof(uint8_t));
	CHECK(data.size() == size);

	CHECK(data[0] == ((uint8_t)Format::Int64));

	Convert.uint64 = 0;
	Convert_Byte7 = data[1];
	Convert_Byte6 = data[2];
	Convert_Byte5 = data[3];
	Convert_Byte4 = data[4];
	Convert_Byte3 = data[5];
	Convert_Byte2 = data[6];
	Convert_Byte1 = data[7];
	Convert_Byte0 = data[8];
	CHECK(Convert.int64 == i64_min1);

	CHECK(data[9] == ((uint8_t)Format::Int64));
	Convert_Byte7 = data[10];
	Convert_Byte6 = data[11];
	Convert_Byte5 = data[12];
	Convert_Byte4 = data[13];
	Convert_Byte3 = data[14];
	Convert_Byte2 = data[15];
	Convert_Byte1 = data[16];
	Convert_Byte0 = data[17];
	CHECK(Convert.int64 == i64_max1);

	CHECK(data[18] == ((uint8_t)Format::Int64));
	Convert_Byte7 = data[19];
	Convert_Byte6 = data[20];
	Convert_Byte5 = data[21];
	Convert_Byte4 = data[22];
	Convert_Byte3 = data[23];
	Convert_Byte2 = data[24];
	Convert_Byte1 = data[25];
	Convert_Byte0 = data[26];
	CHECK(Convert.int64 == i64_min2);

	CHECK(data[27] == ((uint8_t)Format::Int64));
	Convert_Byte7 = data[28];
	Convert_Byte6 = data[29];
	Convert_Byte5 = data[30];
	Convert_Byte4 = data[31];
	Convert_Byte3 = data[32];
	Convert_Byte2 = data[33];
	Convert_Byte1 = data[34];
	Convert_Byte0 = data[35];
	CHECK(Convert.int64 == i64_max2);

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == i64_min1);
		CHECK(std::get<int64_t>(object.value) == i64_min1);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == i64_max1);
		CHECK(std::get<int64_t>(object.value) == i64_max1);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(2);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == i64_min2);
		CHECK(std::get<int64_t>(object.value) == i64_min2);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(3);
		//CHECK(object.type   == MessagePack::DataType::Int64);
		//CHECK(object.int64_ == i64_max2);
		CHECK(std::get<int64_t>(object.value) == i64_max2);
	}
}
#endif // }}}


/**
 * \brief Append an unsigned integer value.
 *
 * The \p value will be appended to the current contents of the MessagePack 
 * data. If \p index is not \nullptr, the index location of the \p value will 
 * be stored in \p index.
 *
 * \parcode
 * zakero::MessagePack message_pack;
 * size_t index;
 * message_pack.append(uint64_t(0), &index);
 * \endparcode
 */
size_t MessagePack::append(const uint64_t value ///< The value to add
	) noexcept
{
	return append_(value, object_vector);
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: Uint8")
{
	MessagePack mesg_pack;

	const uint64_t u8_min = 0x00;
	const uint64_t u8_max = std::numeric_limits<uint8_t>::max();
	size_t count = 0;

	mesg_pack.append(u8_min); count++;
	mesg_pack.append(u8_max); count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 2 * sizeof(uint8_t));
	CHECK(data.size() == size);

	CHECK(data[0] == ((uint8_t)Format::Uint8));
	CHECK(data[1] == u8_min);
	CHECK(data[2] == ((uint8_t)Format::Uint8));
	CHECK(data[3] == u8_max);

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type    == MessagePack::DataType::Uint64);
		//CHECK(object.uint64_ == u8_min);
		CHECK(std::get<uint64_t>(object.value) == u8_min);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		//CHECK(object.type    == MessagePack::DataType::Uint64);
		//CHECK(object.uint64_ == u8_max);
		CHECK(std::get<uint64_t>(object.value) == u8_max);
	}
}

TEST_CASE("Append: Uint16")
{
	MessagePack mesg_pack;

	const uint64_t u16_min = std::numeric_limits<uint8_t>::max() + (uint64_t)1;
	const uint64_t u16_max = std::numeric_limits<uint16_t>::max();
	size_t count = 0;

	mesg_pack.append(u16_min); count++;
	mesg_pack.append(u16_max); count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 3 * sizeof(uint8_t));
	CHECK(data.size() == size);

	CHECK(data[0] == ((uint8_t)Format::Uint16));

	Convert_Byte1 = data[1];
	Convert_Byte0 = data[2];
	CHECK(Convert.uint16 == u16_min);

	CHECK(data[3] == ((uint8_t)Format::Uint16));
	Convert_Byte1 = data[4];
	Convert_Byte0 = data[5];
	CHECK(Convert.uint16 == u16_max);

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type    == MessagePack::DataType::Uint64);
		//CHECK(object.uint64_ == u16_min);
		CHECK(std::get<uint64_t>(object.value) == u16_min);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		//CHECK(object.type    == MessagePack::DataType::Uint64);
		//CHECK(object.uint64_ == u16_max);
		CHECK(std::get<uint64_t>(object.value) == u16_max);
	}
}

TEST_CASE("Append: Uint32")
{
	MessagePack mesg_pack;

	const uint64_t u32_min = std::numeric_limits<uint16_t>::max() + (uint64_t)1;
	const uint64_t u32_max = std::numeric_limits<uint32_t>::max();
	size_t count = 0;

	mesg_pack.append(u32_min); count++;
	mesg_pack.append(u32_max); count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 5 * sizeof(uint8_t));
	CHECK(data.size() == size);

	CHECK(data[0] == ((uint8_t)Format::Uint32));

	Convert_Byte3 = data[1];
	Convert_Byte2 = data[2];
	Convert_Byte1 = data[3];
	Convert_Byte0 = data[4];
	CHECK(Convert.uint32 == u32_min);

	CHECK(data[5] == ((uint8_t)Format::Uint32));
	Convert_Byte3 = data[6];
	Convert_Byte2 = data[7];
	Convert_Byte1 = data[8];
	Convert_Byte0 = data[9];
	CHECK(Convert.uint32 == u32_max);

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type    == MessagePack::DataType::Uint64);
		//CHECK(object.uint64_ == u32_min);
		CHECK(std::get<uint64_t>(object.value) == u32_min);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		//CHECK(object.type    == MessagePack::DataType::Uint64);
		//CHECK(object.uint64_ == u32_max);
		CHECK(std::get<uint64_t>(object.value) == u32_max);
	}
}

TEST_CASE("Append: Uint64")
{
	MessagePack mesg_pack;

	const uint64_t u64_min = std::numeric_limits<uint32_t>::max() + (uint64_t)1;
	const uint64_t u64_max = std::numeric_limits<uint64_t>::max();
	size_t count = 0;

	mesg_pack.append(u64_min); count++;
	mesg_pack.append(u64_max); count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 9 * sizeof(uint8_t));
	CHECK(data.size() == size);

	CHECK(data[0] == ((uint8_t)Format::Uint64));

	Convert_Byte7 = data[1];
	Convert_Byte6 = data[2];
	Convert_Byte5 = data[3];
	Convert_Byte4 = data[4];
	Convert_Byte3 = data[5];
	Convert_Byte2 = data[6];
	Convert_Byte1 = data[7];
	Convert_Byte0 = data[8];
	CHECK(Convert.uint64 == u64_min);

	CHECK(data[9] == ((uint8_t)Format::Uint64));
	Convert_Byte7 = data[10];
	Convert_Byte6 = data[11];
	Convert_Byte5 = data[12];
	Convert_Byte4 = data[13];
	Convert_Byte3 = data[14];
	Convert_Byte2 = data[15];
	Convert_Byte1 = data[16];
	Convert_Byte0 = data[17];
	CHECK(Convert.uint64 == u64_max);

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type    == MessagePack::DataType::Uint64);
		//CHECK(object.uint64_ == u64_min);
		CHECK(std::get<uint64_t>(object.value) == u64_min);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		//CHECK(object.type    == MessagePack::DataType::Uint64);
		//CHECK(object.uint64_ == u64_max);
		CHECK(std::get<uint64_t>(object.value) == u64_max);
	}
}
#endif // }}}


/**
 * \brief Append a 32-bit floating point value.
 *
 * The \p value will be appended to the current contents of the MessagePack 
 * data. If \p index is not \nullptr, the index location of the \p value will 
 * be stored in \p index.
 *
 * \parcode
 * zakero::MessagePack message_pack;
 * size_t index;
 * message_pack.append(float(0), &index);
 * \endparcode
 */
size_t MessagePack::append(const float value ///< The value to add
	) noexcept
{
	return append_(value, object_vector);
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: Float32")
{
	MessagePack mesg_pack;

	const float f32_min  = std::numeric_limits<float>::min();
	const float f32_max  = std::numeric_limits<float>::max();
	const float f32_zero = 0.0f;
	size_t count = 0;

	mesg_pack.append(f32_min);  count++;
	mesg_pack.append(f32_max);  count++;
	mesg_pack.append(f32_zero); count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 5 * sizeof(uint8_t));
	CHECK(data.size() == size);

	CHECK(data[0] == ((uint8_t)Format::Float32));
	Convert_Byte3 = data[1];
	Convert_Byte2 = data[2];
	Convert_Byte1 = data[3];
	Convert_Byte0 = data[4];
	CHECK(Convert.float32 == f32_min);

	CHECK(data[5] == ((uint8_t)Format::Float32));
	Convert_Byte3 = data[6];
	Convert_Byte2 = data[7];
	Convert_Byte1 = data[8];
	Convert_Byte0 = data[9];
	CHECK(Convert.float32 == f32_max);

	CHECK(data[10] == ((uint8_t)Format::Float32));
	Convert_Byte3 = data[11];
	Convert_Byte2 = data[12];
	Convert_Byte1 = data[13];
	Convert_Byte0 = data[14];
	CHECK(Convert.float32 == f32_zero);

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type   == MessagePack::DataType::Float);
		//CHECK(object.float_ == f32_min);
		CHECK(std::get<float>(object.value) == f32_min);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		//CHECK(object.type   == MessagePack::DataType::Float);
		//CHECK(object.float_ == f32_max);
		CHECK(std::get<float>(object.value) == f32_max);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(2);
		//CHECK(object.type   == MessagePack::DataType::Float);
		//CHECK(object.float_ == f32_zero);
		CHECK(std::get<float>(object.value) == f32_zero);
	}
}
#endif // }}}


/**
 * \brief Append a 64-bit floating point value.
 *
 * The \p value will be appended to the current contents of the MessagePack 
 * data. If \p index is not \nullptr, the index location of the \p value will 
 * be stored in \p index.
 *
 * \parcode
 * zakero::MessagePack message_pack;
 * size_t index;
 * message_pack.append(double(0), &index);
 * \endparcode
 */
size_t MessagePack::append(const double value ///< The value to add
	) noexcept
{
	return append_(value, object_vector);
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: Float64")
{
	MessagePack mesg_pack;

	const double f64_min  = std::numeric_limits<double>::min();
	const double f64_max  = std::numeric_limits<double>::max();
	const double f64_zero = 0.0f;
	size_t count = 0;

	mesg_pack.append(f64_min);  count++;
	mesg_pack.append(f64_max);  count++;
	mesg_pack.append(f64_zero); count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 9 * sizeof(uint8_t));
	CHECK(data.size() == size);

	CHECK(data[0] == ((uint8_t)Format::Float64));
	Convert_Byte7 = data[1];
	Convert_Byte6 = data[2];
	Convert_Byte5 = data[3];
	Convert_Byte4 = data[4];
	Convert_Byte3 = data[5];
	Convert_Byte2 = data[6];
	Convert_Byte1 = data[7];
	Convert_Byte0 = data[8];
	CHECK(Convert.float64 == f64_min);

	CHECK(data[9] == ((uint8_t)Format::Float64));
	Convert_Byte7 = data[10];
	Convert_Byte6 = data[11];
	Convert_Byte5 = data[12];
	Convert_Byte4 = data[13];
	Convert_Byte3 = data[14];
	Convert_Byte2 = data[15];
	Convert_Byte1 = data[16];
	Convert_Byte0 = data[17];
	CHECK(Convert.float64 == f64_max);

	CHECK(data[18] == ((uint8_t)Format::Float64));
	Convert_Byte7 = data[19];
	Convert_Byte6 = data[20];
	Convert_Byte5 = data[21];
	Convert_Byte4 = data[22];
	Convert_Byte3 = data[23];
	Convert_Byte2 = data[24];
	Convert_Byte1 = data[25];
	Convert_Byte0 = data[26];
	CHECK(Convert.float64 == f64_zero);

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type    == MessagePack::DataType::Double);
		//CHECK(object.double_ == f64_min);
		CHECK(std::get<double>(object.value) == f64_min);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		//CHECK(object.type    == MessagePack::DataType::Double);
		//CHECK(object.double_ == f64_max);
		CHECK(std::get<double>(object.value) == f64_max);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(2);
		//CHECK(object.type    == MessagePack::DataType::Double);
		//CHECK(object.double_ == f64_zero);
		CHECK(std::get<double>(object.value) == f64_zero);
	}
}
#endif // }}}


/**
 * \brief Append a string.
 *
 * The \p value will be appended to the current contents of the MessagePack 
 * data. If \p index is not \nullptr, the index location of the \p value will 
 * be stored in \p index.
 *
 * \parcode
 * zakero::MessagePack message_pack;
 * size_t index;
 * message_pack.append("foo", &index);
 * \endparcode
 */
size_t MessagePack::append(const std::string_view value ///< The value to add
	) noexcept
{
	return append_(value, object_vector);
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: Fixed_Str")
{
	MessagePack mesg_pack;

	const std::string str_empty;
	const std::string str_1("_");
	const std::string str_31(31, 'a');
	size_t count = 0;

	mesg_pack.append(str_empty); count++;
	mesg_pack.append(str_1);     count++;
	mesg_pack.append(str_31);    count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 1 * sizeof(uint8_t))
		+ str_empty.size()
		+ str_1.size()
		+ str_31.size()
		;
	CHECK(data.size() == size);

	size_t str_len = 0;
	size_t index = 0;
	std::string_view str;

	CHECK((data[0] & Fixed_Str_Mask) == ((uint8_t)Format::Fixed_Str));
	str_len = data[0] & Fixed_Str_Value;
	CHECK(str_len == str_empty.size());
	index += str_len + 1;

	CHECK((data[index] & Fixed_Str_Mask) == ((uint8_t)Format::Fixed_Str));
	str_len = data[index] & Fixed_Str_Value;
	CHECK(str_len == str_1.size());
	index++;
	str = std::string_view((char*)&data[index], str_len);
	CHECK(str == str_1);
	index += str_len;

	CHECK((data[index] & Fixed_Str_Mask) == ((uint8_t)Format::Fixed_Str));
	str_len = data[index] & Fixed_Str_Value;
	CHECK(str_len == str_31.size());
	index++;
	str = std::string_view((char*)&data[index], str_len);
	CHECK(str == str_31);

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type   == MessagePack::DataType::String);
		//CHECK(object.string == str_empty);
		CHECK(std::get<std::string>(object.value) == str_empty);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		//CHECK(object.type   == MessagePack::DataType::String);
		//CHECK(object.string == str_1);
		CHECK(std::get<std::string>(object.value) == str_1);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(2);
		//CHECK(object.type   == MessagePack::DataType::String);
		//CHECK(object.string == str_31);
		CHECK(std::get<std::string>(object.value) == str_31);
	}
}

TEST_CASE("Append: Str8")
{
	MessagePack mesg_pack;

	const std::string str_min(32  , '_');
	const std::string str_max(std::numeric_limits<uint8_t>::max(), 'X');
	size_t count = 0;

	mesg_pack.append(str_min); count++;
	mesg_pack.append(str_max); count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 2 * sizeof(uint8_t))
		+ str_min.size()
		+ str_max.size()
		;
	CHECK(data.size() == size);

	size_t str_len = 0;
	size_t index = 0;
	std::string_view str;

	CHECK(data[index] == ((uint8_t)Format::Str8));
	str_len = data[++index];
	CHECK(str_len == str_min.size());
	str = std::string_view((char*)&data[++index], str_len);
	CHECK(str == str_min);
	index += str_len;

	CHECK(data[index] == ((uint8_t)Format::Str8));
	str_len = data[++index];
	CHECK(str_len == str_max.size());
	str = std::string_view((char*)&data[++index], str_len);
	CHECK(str == str_max);
	index += str_len;

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type   == MessagePack::DataType::String);
		//CHECK(object.string == str_min);
		CHECK(std::get<std::string>(object.value) == str_min);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		//CHECK(object.type   == MessagePack::DataType::String);
		//CHECK(object.string == str_max);
		CHECK(std::get<std::string>(object.value) == str_max);
	}
}

TEST_CASE("Append: Str16")
{
	MessagePack mesg_pack;

	const std::string str_min(std::numeric_limits<uint8_t>::max() + 1, '_');
	const std::string str_max(std::numeric_limits<uint16_t>::max()   , 'X');
	size_t count = 0;

	mesg_pack.append(str_min); count++;
	mesg_pack.append(str_max); count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 3 * sizeof(uint8_t))
		+ str_min.size()
		+ str_max.size()
		;
	CHECK(data.size() == size);

	size_t str_len = 0;
	size_t index = 0;
	std::string_view str;

	CHECK(data[index] == ((uint8_t)Format::Str16));
	Convert_Byte1 = data[++index];
	Convert_Byte0 = data[++index];
	str_len = Convert.uint16;
	CHECK(str_len == str_min.size());
	str = std::string_view((char*)&data[++index], str_len);
	CHECK(str == str_min);
	index += str_len;

	CHECK(data[index] == ((uint8_t)Format::Str16));
	Convert_Byte1 = data[++index];
	Convert_Byte0 = data[++index];
	str_len = Convert.uint16;
	CHECK(str_len == str_max.size());
	str = std::string_view((char*)&data[++index], str_len);
	CHECK(str == str_max);
	index += str_len;

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type   == MessagePack::DataType::String);
		//CHECK(object.string == str_min);
		CHECK(std::get<std::string>(object.value) == str_min);
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		//CHECK(object.type   == MessagePack::DataType::String);
		//CHECK(object.string == str_max);
		CHECK(std::get<std::string>(object.value) == str_max);
	}
}

TEST_CASE("Append: Str32")
{
	MessagePack mesg_pack;

	const std::string str_min(std::numeric_limits<uint16_t>::max() + 1, '_');
	// This string size would be 4GB + overhead from std::string
	// Serializing would consume another 4GB.
	// For a total of 8GB to test the maximum Str32 length.
	//const std::string str_max(std::numeric_limits<uint32_t>::max()    , 'X');
	size_t count = 0;

	mesg_pack.append(str_min); count++;
	//mesg_pack.append(str_max); count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 5 * sizeof(uint8_t))
		+ str_min.size()
		//+ str_max.size()
		;
	CHECK(data.size() == size);

	size_t str_len = 0;
	size_t index = 0;
	std::string_view str;

	CHECK(data[index] == ((uint8_t)Format::Str32));
	Convert_Byte3 = data[++index];
	Convert_Byte2 = data[++index];
	Convert_Byte1 = data[++index];
	Convert_Byte0 = data[++index];
	str_len = Convert.uint32;
	CHECK(str_len == str_min.size());
	str = std::string_view((char*)&data[++index], str_len);
	CHECK(str == str_min);
	index += str_len;

	//CHECK(data[index] == ((uint8_t)Format::Str32));
	//Convert_Byte1 = data[++index];
	//Convert_Byte0 = data[++index];
	//str_len = Convert.uint16;
	//CHECK(str_len == str_max.size());
	//str = std::string_view((char*)&data[++index], str_len);
	//CHECK(str == str_max);
	//index += str_len;

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type   == MessagePack::DataType::String);
		//CHECK(object.string == str_min);
		CHECK(std::get<std::string>(object.value) == str_min);
	}

	//{
	//	const MessagePack::Object& object = mesg_pack.object(1);
	//	CHECK(object.type   == MessagePack::DataType::String);
	//	CHECK(object.string == str_max);
	//}
}
#endif // }}}


/**
 * \brief Append a binary data.
 *
 * The \p value will be appended to the current contents of the MessagePack 
 * data. If \p index is not \nullptr, the index location of the \p value will 
 * be stored in \p index.
 *
 * \parcode
 * zakero::MessagePack message_pack;
 * size_t index;
 * std::vector<uint8_t> binary_data(1024, 0);
 * message_pack.append(binary_data, &index);
 * \endparcode
 */
size_t MessagePack::append(const std::vector<uint8_t>& value ///< The value to add
	) noexcept
{
	return append_(value, object_vector);
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: Bin8 (Const Ref)")
{
	MessagePack mesg_pack;

	const std::vector<uint8_t> bin_min = {};
	const std::vector<uint8_t> bin_max(std::numeric_limits<uint8_t>::max(), 'X');
	size_t count = 0;

	mesg_pack.append(bin_min); count++;
	mesg_pack.append(bin_max); count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 2 * sizeof(uint8_t))
		+ bin_min.size()
		+ bin_max.size()
		;
	CHECK(data.size() == size);

	size_t bin_len = 0;
	size_t index = 0;

	CHECK(data[index] == ((uint8_t)Format::Bin8));
	bin_len = data[++index];
	CHECK(bin_len == bin_min.size());
	for(size_t i = 0; i < bin_len; i++)
	{
		CHECK(data[++index] == bin_min[i]);
	}

	CHECK(data[++index] == ((uint8_t)Format::Bin8));
	bin_len = data[++index];
	CHECK(bin_len == bin_max.size());
	for(size_t i = 0; i < bin_len; i++)
	{
		CHECK(data[++index] == bin_max[i]);
	}

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type == MessagePack::DataType::Vector);
		const std::vector<uint8_t>& vector = std::get<std::vector<uint8_t>>(object.value);
		CHECK(vector.size() == bin_min.size());
		for(size_t i = 0; i < bin_min.size(); i++)
		{
			CHECK(vector[i] == bin_min[i]);
		}
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		//CHECK(object.type == MessagePack::DataType::Vector);
		const std::vector<uint8_t>& vector = std::get<std::vector<uint8_t>>(object.value);
		CHECK(vector.size() == bin_max.size());
		for(size_t i = 0; i < bin_max.size(); i++)
		{
			CHECK(vector[i] == bin_max[i]);
		}
	}
}

TEST_CASE("Append: Bin16")
{
	MessagePack mesg_pack;

	const std::vector<uint8_t> bin_min(std::numeric_limits<uint8_t>::max() + 1, '_');
	const std::vector<uint8_t> bin_max(std::numeric_limits<uint16_t>::max()   , 'X');
	size_t count = 0;

	mesg_pack.append(bin_min); count++;
	mesg_pack.append(bin_max); count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 3 * sizeof(uint8_t))
		+ bin_min.size()
		+ bin_max.size()
		;
	CHECK(data.size() == size);

	size_t bin_len = 0;
	size_t index = 0;

	CHECK(data[index] == (uint8_t)Format::Bin16);
	Convert_Byte1 = data[++index];
	Convert_Byte0 = data[++index];
	bin_len = Convert.uint16;
	CHECK(bin_len == bin_min.size());
	for(size_t i = 0; i < bin_len; i++)
	{
		CHECK(data[++index] == bin_min[i]);
	}

	CHECK(data[++index] == ((uint8_t)Format::Bin16));
	Convert_Byte1 = data[++index];
	Convert_Byte0 = data[++index];
	bin_len = Convert.uint16;
	CHECK(bin_len == bin_max.size());
	for(size_t i = 0; i < bin_len; i++)
	{
		CHECK(data[++index] == bin_max[i]);
	}

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type == MessagePack::DataType::Vector);
		const std::vector<uint8_t>& vector = std::get<std::vector<uint8_t>>(object.value);
		CHECK(vector.size() == bin_min.size());
		for(size_t i = 0; i < bin_min.size(); i++)
		{
			CHECK(vector[i] == bin_min[i]);
		}
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		//CHECK(object.type == MessagePack::DataType::Vector);
		const std::vector<uint8_t>& vector = std::get<std::vector<uint8_t>>(object.value);
		CHECK(vector.size() == bin_max.size());
		for(size_t i = 0; i < bin_max.size(); i++)
		{
			CHECK(vector[i] == bin_max[i]);
		}
	}
}

TEST_CASE("Append: Bin32")
{
	MessagePack mesg_pack;

	const std::vector<uint8_t> bin_min(std::numeric_limits<uint16_t>::max() + 1, '_');
	// This vector size would be 4GB + overhead from std::vector
	// Serializing would consume another 4GB.
	// For a total of 8GB to test the maximum Bin32 length.
	//const std::vector<uint8_t> bin_max(std::numeric_limits<uint32_t>::max()    , 'X');
	size_t count = 0;

	mesg_pack.append(bin_min); count++;
	//mesg_pack.append(bin_max); count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 5 * sizeof(uint8_t))
		+ bin_min.size()
		//+ bin_max.size()
		;
	CHECK(data.size() == size);

	size_t bin_len = 0;
	size_t index = 0;

	CHECK(data[index] == (uint8_t)Format::Bin32);
	Convert_Byte3 = data[++index];
	Convert_Byte2 = data[++index];
	Convert_Byte1 = data[++index];
	Convert_Byte0 = data[++index];
	bin_len = Convert.uint32;
	CHECK(bin_len == bin_min.size());
	for(size_t i = 0; i < bin_len; i++)
	{
		CHECK(data[++index] == bin_min[i]);
	}

	//CHECK(data[++index] == ((uint8_t)Format::Bin32));
	//Convert_Byte3 = data[++index];
	//Convert_Byte2 = data[++index];
	//Convert_Byte1 = data[++index];
	//Convert_Byte0 = data[++index];
	//bin_len = Convert.uint32;
	//CHECK(bin_len == bin_max.size());
	//for(size_t i = 0; i < bin_len; i++)
	//{
	//	CHECK(data[++index] == bin_max[i]);
	//}

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type == MessagePack::DataType::Vector);
		const std::vector<uint8_t>& vector = std::get<std::vector<uint8_t>>(object.value);
		CHECK(vector.size() == bin_min.size());
		for(size_t i = 0; i < bin_min.size(); i++)
		{
			CHECK(vector[i] == bin_min[i]);
		}
	}

	//{
	//	const MessagePack::Object& object = mesg_pack.object(1);
	//	CHECK(object.type == MessagePack::DataType::Vector);
	//	CHECK(object.vector.size() == bin_max.size());
	//	for(size_t i = 0; i < bin_max.size(); i++)
	//	{
	//		CHECK(object.vector[i] == bin_max[i]);
	//	}
	//}
}
#endif // }}}


/**
 * \brief Append a binary data.
 *
 * The \p value will be appended to the current contents of the MessagePack 
 * data. If \p index is not \nullptr, the index location of the \p value will 
 * be stored in \p index.
 *
 * \note The contents of \p value will be **moved**.
 *
 * \parcode
 * zakero::MessagePack message_pack;
 * size_t index;
 * std::vector<uint8_t> binary_data(1024, 0);
 * message_pack.append(std::move(binary_data), &index);
 * \endparcode
 */
size_t MessagePack::append(std::vector<uint8_t>&& value ///< The value to add
	) noexcept
{
	return append_(std::move(value), object_vector);
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: Bin8 (Move)")
{
	MessagePack mesg_pack;

	std::vector<uint8_t> bin_min = {};
	std::vector<uint8_t> bin_max(std::numeric_limits<uint8_t>::max(), 'X');
	size_t count = 0;

	std::vector<uint8_t> temp_min = bin_min;
	std::vector<uint8_t> temp_max = bin_max;

	mesg_pack.append(std::move(temp_min)); count++;
	mesg_pack.append(std::move(temp_max)); count++;

	CHECK(temp_min.size() == 0);
	CHECK(temp_max.size() == 0);
	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 2 * sizeof(uint8_t))
		+ bin_min.size()
		+ bin_max.size()
		;
	CHECK(data.size() == size);

	size_t bin_len = 0;
	size_t index = 0;

	CHECK(data[index] == ((uint8_t)Format::Bin8));
	bin_len = data[++index];
	CHECK(bin_len == bin_min.size());
	for(size_t i = 0; i < bin_len; i++)
	{
		CHECK(data[++index] == bin_min[i]);
	}

	CHECK(data[++index] == ((uint8_t)Format::Bin8));
	bin_len = data[++index];
	CHECK(bin_len == bin_max.size());
	for(size_t i = 0; i < bin_len; i++)
	{
		CHECK(data[++index] == bin_max[i]);
	}

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type == MessagePack::DataType::Vector);
		const std::vector<uint8_t>& vector = std::get<std::vector<uint8_t>>(object.value);
		CHECK(vector.size() == bin_min.size());
		for(size_t i = 0; i < bin_min.size(); i++)
		{
			CHECK(vector[i] == bin_min[i]);
		}
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		//CHECK(object.type == MessagePack::DataType::Vector);
		const std::vector<uint8_t>& vector = std::get<std::vector<uint8_t>>(object.value);
		CHECK(vector.size() == bin_max.size());
		for(size_t i = 0; i < bin_max.size(); i++)
		{
			CHECK(vector[i] == bin_max[i]);
		}
	}
}

// All the other tests would be the same as the (Const Ref) test case.
// The only difference would be using the `move operator`.
// Unneeded redundancy...
#endif // }}}


/**
 * \brief Append a binary data.
 *
 * The \p value will be appended to the current contents of the MessagePack 
 * data. If \p index is not \nullptr, the index location of the \p value will 
 * be stored in \p index.
 *
 * \parcode
 * zakero::MessagePack message_pack;
 * size_t index;
 * size_t binary_length = 1024;
 * uint8_t binary_data[binary_length] = { 0 };
 * message_pack.append(binary_data, binary_length, &index);
 * \endparcode
 */
size_t MessagePack::append(const uint8_t* value ///< The value to add
	, const size_t                    size  ///< The value count
	) noexcept
{
	return append_(value, size, object_vector);
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: Bin8 (C-Style)")
{
	MessagePack mesg_pack;

	size_t  bin_min_size = 0;
	uint8_t bin_min[bin_min_size];
	size_t  bin_max_size = std::numeric_limits<uint8_t>::max();
	uint8_t bin_max[bin_max_size];
	memset(bin_max, 'X', bin_max_size);
	size_t count = 0;

	mesg_pack.append(bin_min, bin_min_size); count++;
	mesg_pack.append(bin_max, bin_max_size); count++;

	CHECK(mesg_pack.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	size_t size = (count * 2 * sizeof(uint8_t))
		+ bin_min_size
		+ bin_max_size
		;
	CHECK(data.size() == size);

	size_t bin_len = 0;
	size_t index = 0;

	CHECK(data[index] == ((uint8_t)Format::Bin8));
	bin_len = data[++index];
	CHECK(bin_len == bin_min_size);
	for(size_t i = 0; i < bin_len; i++)
	{
		CHECK(data[++index] == bin_min[i]);
	}

	CHECK(data[++index] == ((uint8_t)Format::Bin8));
	bin_len = data[++index];
	CHECK(bin_len == bin_max_size);
	for(size_t i = 0; i < bin_len; i++)
	{
		CHECK(data[++index] == bin_max[i]);
	}

	// Check deserialized data

	mesg_pack.deserialize(data);

	CHECK(mesg_pack.size() == count);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		//CHECK(object.type == MessagePack::DataType::Vector);
		const std::vector<uint8_t>& vector = std::get<std::vector<uint8_t>>(object.value);
		CHECK(vector.size() == bin_min_size);
		for(size_t i = 0; i < bin_min_size; i++)
		{
			CHECK(vector[i] == bin_min[i]);
		}
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		//CHECK(object.type == MessagePack::DataType::Vector);
		const std::vector<uint8_t>& vector = std::get<std::vector<uint8_t>>(object.value);
		CHECK(vector.size() == bin_max_size);
		for(size_t i = 0; i < bin_max_size; i++)
		{
			CHECK(vector[i] == bin_max[i]);
		}
	}
}

// All the other tests would be the same as the (Const Ref) test case.
// The only difference would be using the `move operator`.
// Unneeded redundancy...
#endif // }}}


size_t MessagePack::append(const Array& array
	) noexcept
{
	size_t index = appendArray_(object_vector);

	object(index).asArray().append(array);

	return index;
}


size_t MessagePack::appendArray() noexcept
{
	return appendArray_(object_vector);
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: Fixed_Array")
{
	MessagePack mesg_pack;

	size_t count = 0;
	size_t index = mesg_pack.appendArray();
	MessagePack::Array& array = mesg_pack.object(index).asArray();
	size_t byte_size = 1 * sizeof(uint8_t); // Fixed_Array Overhead
	array.appendNull();                count++;  byte_size += 1;
	array.append(true);                count++;  byte_size += 1;
	array.append(false);               count++;  byte_size += 1;
	array.append(int64_t(-1));         count++;  byte_size += 1;
	array.append(int64_t(1));          count++;  byte_size += 1;
	array.append(uint64_t(42));        count++;  byte_size += 2;
	array.append(float(4.2));          count++;  byte_size += 5;
	array.append(double(0.42));        count++;  byte_size += 9;
	array.append(std::string("foo"));  count++;  byte_size += 4;
	array.append(array);             count++;  byte_size += 1 + byte_size;

	CHECK(mesg_pack.size() == 1);
	CHECK(array.size() == count);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);
	CHECK(data.size() == byte_size);

	index = 0;
	CHECK((data[index  ] & (uint8_t)Fixed_Array_Mask) == (uint8_t)Format::Fixed_Array);
	CHECK((data[index++] & (uint8_t)Fixed_Array_Value) == (uint8_t)array.size());

	CHECK(data[index++] == (uint8_t)Format::Nill);

	CHECK(data[index++] == (uint8_t)Format::True);

	CHECK(data[index++] == (uint8_t)Format::False);

	CHECK( (data[index  ] & (uint8_t)Fixed_Int_Neg_Mask) == (uint8_t)Format::Fixed_Int_Neg);
	CHECK(((data[index++] & (uint8_t)Fixed_Int_Neg_Value) - 32) == -1);

	CHECK((data[index  ] & (uint8_t)Fixed_Int_Pos_Mask) == (uint8_t)Format::Fixed_Int_Pos);
	CHECK((data[index++] & (uint8_t)Fixed_Int_Pos_Value) == 1);

	CHECK(data[index++] == (uint8_t)Format::Uint8);
	CHECK(data[index++] == (uint8_t)42);

	CHECK(data[index++] == (uint8_t)Format::Float32);
	index += 4;

	CHECK(data[index++] == (uint8_t)Format::Float64);
	index += 8;

	CHECK((data[index] & (uint8_t)Fixed_Str_Mask) == (uint8_t)Format::Fixed_Str);
	size_t len = data[index++] & (uint8_t)Fixed_Str_Value;
	CHECK(len == 3);
	index += len;

	CHECK((data[index] & (uint8_t)Fixed_Array_Mask) == (uint8_t)Format::Fixed_Array);
	CHECK((data[index++] & (uint8_t)Fixed_Array_Value) == count);

	// Check deserialized data

	mesg_pack = MessagePack::deserialize(data);
	CHECK(mesg_pack.size() == 1);

	const MessagePack::Object& object = mesg_pack.object(0);
	CHECK(object.isArray());

	const MessagePack::Array& test = object.asArray();
	CHECK(test.size() == count);

	index = 0;
	CHECK(test.object(index).isNull());

	index++;
	CHECK(test.object(index).is<bool>());
	CHECK(test.object(index).as<bool>() == true);

	index++;
	CHECK(test.object(index).is<bool>());
	CHECK(test.object(index).as<bool>() == false);

	index++;
	CHECK(test.object(index).is<int64_t>());
	CHECK(test.object(index).as<int64_t>() == -1);

	index++;
	CHECK(test.object(index).is<int64_t>());
	CHECK(test.object(index).as<int64_t>() == 1);

	index++;
	CHECK(test.object(index).is<uint64_t>());
	CHECK(test.object(index).as<uint64_t>() == 42);

	index++;
	CHECK(test.object(index).is<float>());
	CHECK(test.object(index).as<float>() == 4.2f);

	index++;
	CHECK(test.object(index).is<double>());
	CHECK(test.object(index).as<double>() == 0.42);

	index++;
	CHECK(test.object(index).isString());
	CHECK(test.object(index).asString() == "foo");

	index++;
	CHECK(test.object(index).isArray());
	CHECK(test.object(index).asArray().size() == test.size());
}

TEST_CASE("Append: Array16")
{
	MessagePack mesg_pack;

	size_t size = 0;

	{
		size_t index = mesg_pack.appendArray();
		MessagePack::Array& array = mesg_pack.object(index).asArray();

		size += 3; // Array16 overhead

		for(size_t i = 0; i < 16; i++)
		{
			array.append(true);
			size++; // Boolean is 1 byte
		}
	}

	{
		size_t index = mesg_pack.appendArray();
		MessagePack::Array& array = mesg_pack.object(index).asArray();

		size += 3; // Array16 overhead

		for(size_t i = 0; i < std::numeric_limits<uint16_t>::max(); i++)
		{
			array.append(false);
			size++; // Boolean is 1 byte
		}
	}

	CHECK(mesg_pack.size() == 2);
	CHECK(mesg_pack.object(0).isArray());
	CHECK(mesg_pack.object(0).asArray().size() == 16);
	CHECK(mesg_pack.object(1).isArray());
	CHECK(mesg_pack.object(1).asArray().size() == std::numeric_limits<uint16_t>::max());

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);
	CHECK(data.size() == size);

	size_t index = 0;

	CHECK(data[index++] == (uint8_t)Format::Array16);
	Convert_Byte1 = data[index++];
	Convert_Byte0 = data[index++];
	CHECK(Convert.uint16 == 16);

	for(size_t count = 0; count < 16; count++)
	{
		CHECK(data[index++] == (uint8_t)Format::True);
	}

	CHECK(data[index++] == (uint8_t)Format::Array16);
	Convert_Byte1 = data[index++];
	Convert_Byte0 = data[index++];
	CHECK(Convert.uint16 == std::numeric_limits<uint16_t>::max());

	for(size_t count = 0; count < std::numeric_limits<uint16_t>::max(); count++)
	{
		CHECK(data[index++] == (uint8_t)Format::False);
	}

	// Check deserialized data

	mesg_pack = MessagePack::deserialize(data);
	CHECK(mesg_pack.size() == 2);

	{
		MessagePack::Array& array = mesg_pack.object(0).asArray();
		CHECK(array.size() == 16);

		for(size_t i = 0; i < array.size(); i++)
		{
			CHECK(array.object(i).is<bool>());
			CHECK(array.object(i).as<bool>() == true);
		}
	}
	{
		MessagePack::Array& array = mesg_pack.object(1).asArray();
		CHECK(array.size() == std::numeric_limits<uint16_t>::max());

		for(size_t i = 0; i < array.size(); i++)
		{
			CHECK(array.object(i).is<bool>());
			CHECK(array.object(i).as<bool>() == false);
		}
	}

}

TEST_CASE("Append: Array32")
{
	MessagePack mesg_pack;

	size_t size = 0;

	size_t index = mesg_pack.appendArray();
	MessagePack::Array& array = mesg_pack.object(index).asArray();

	size += 5; // Array16 overhead

	for(size_t i = 0; i < std::numeric_limits<uint16_t>::max() + 1; i++)
	{
		array.appendNull();
		size++; // Boolean is 1 byte
	}

	CHECK(mesg_pack.size() == 1);
	CHECK(mesg_pack.object(0).isArray());
	CHECK(mesg_pack.object(0).asArray().size() == (std::numeric_limits<uint16_t>::max() + 1));

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);
	CHECK(data.size() == size);

	index = 0;

	CHECK(data[index++] == (uint8_t)Format::Array32);
	Convert_Byte3 = data[index++];
	Convert_Byte2 = data[index++];
	Convert_Byte1 = data[index++];
	Convert_Byte0 = data[index++];
	CHECK(Convert.uint32 == (std::numeric_limits<uint16_t>::max() + 1));

	for(size_t count = 0; count < std::numeric_limits<uint16_t>::max() + 1; count++)
	{
		CHECK(data[index++] == (uint8_t)Format::Nill);
	}

	// Check deserialized data

	mesg_pack = MessagePack::deserialize(data);
	CHECK(mesg_pack.size() == 1);

	MessagePack::Array& test = mesg_pack.object(0).asArray();
	CHECK(test.size() == (std::numeric_limits<uint16_t>::max() + 1));

	for(size_t i = 0; i < test.size(); i++)
	{
		CHECK(test.object(i).isNull());
	}
}
#endif // }}}


/**
 * \brief Append a "null" value.
 *
 * A "null" value will be appended to the current contents of the MessagePack 
 * data. If \p index is not \nullptr, the index location of the "null" value 
 * will be stored in \p index.
 *
 * \parcode
 * zakero::MessagePack message_pack;
 * size_t index;
 * message_pack.appendNull(&index);
 * \endparcode
 */
size_t MessagePack::appendNull() noexcept
{
	return appendNull_(object_vector);
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
TEST_CASE("Append: Nill")
{
	MessagePack mesg_pack;

	size_t index = mesg_pack.appendNull();

	CHECK(index == 0);
	CHECK(mesg_pack.size() == 1);

	index = mesg_pack.appendNull();

	CHECK(index == 1);
	CHECK(mesg_pack.size() == 2);

	// Check serialized data

	std::vector<uint8_t> data = MessagePack::serialize(mesg_pack);

	CHECK(data.size() == 2);
	CHECK(data[0] == (uint8_t)Format::Nill);
	CHECK(data[1] == (uint8_t)Format::Nill);

	// Check deserialized data

	mesg_pack.deserialize(data);
	CHECK(mesg_pack.size() == 2);

	{
		const MessagePack::Object& object = mesg_pack.object(0);
		CHECK(object.isNull());
	}

	{
		const MessagePack::Object& object = mesg_pack.object(1);
		CHECK(object.isNull());
	}
}
#endif // }}}


/**
 * \brief Access a data object.
 *
 * After data has been added to the MessagePack, that data can still be access 
 * by using its index value. The data object's type will be the C++ datatype 
 * and not the MessagePack format type.
 *
 * The data-type of the object can be changed as needed. However, be sure to 
 * __update the `type`__ to match the new value.
 *
 * \parcode
 * zakero::MessagePack message_pack;
 * size_t mp_foo;
 * message_pack.append(int64_t(0), &mp_foo);
 * size_t mp_bar;
 * message_pack.append(int64_t(0), &mp_bar);
 *
 * int64_t val = rand();
 * message_pack.object(mp_foo).int64_ = val;
 * if(val & 1)
 * {
 *      // Change to string
 * 	message_pack.object(mp_bar).type   = zakero::MessagePack::DataType::String
 * 	message_pack.object(mp_bar).string = "That's odd...";
 * }
 * else
 * {
 *      // Change to boolean
 * 	message_pack.object(mp_bar).type  = zakero::MessagePack::DataType::Bool
 * 	message_pack.object(mp_bar).bool_ = true;
 * }
 * \endparcode
 *
 * \return The data object.
 */
MessagePack::Object& MessagePack::object(const size_t index ///< The index of the data object.
	) noexcept
{
	return object_vector[index];
}


const MessagePack::Object& MessagePack::object(const size_t index ///< The index of the data object.
	) const noexcept
{
	return object_vector[index];
}


/**
 * \brief Remove all data from the MessagePack.
 */
void MessagePack::clear() noexcept
{
	object_vector.clear();
}


/**
 * \brief Get the number of data objects.
 *
 * Using this method will provide a count of all the data objects that the 
 * MessagePack contains. Arrays and Maps each count as one data object. So, if 
 * the MessagePack contains Arrays or Maps, the contents of those objects is 
 * __not__ counted.
 *
 * \return The number of data objects.
 */
size_t MessagePack::size() const noexcept
{
	return object_vector.size();
}


#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST // {{{
// Nothing to test
#endif // }}}


/**
 * \brief Deserialize MessagePack data.
 *
 * The packed vector of \p data will be converted into data objects that can be 
 * easily accessed and used.
 *
 * \parcode
 * std::vector<uint8_t> command_result = get_reply(command_id);
 *
 * zakero::MessagePack message_pack;
 * message_pack.deserialize(command_result);
 *
 * constexpr size_t error_index = 1;
 * constexpr size_t error_code_index = 2;
 * if(message_pack.object(error_index).boolean == true)
 * {
 * 	writeError(message_pack.object(error_code_index).int64_);
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
MessagePack MessagePack::deserialize(const std::vector<uint8_t>& data ///< The packed data
	) noexcept
{
	MessagePack message_pack;

	size_t index = 0;
	while(index < data.size())
	{
		deserialize_(data, index, message_pack.object_vector);
	}

	return message_pack;
}


/**
 * \brief Serialize MessagePack data.
 *
 * The contents of the MessagePack will be packed into the returned 
 * std::vector.
 *
 * \parcode
 * zakero::MessagePack message_pack;
 * message_pack.append(command_id);
 * message_pack.append(true);
 * message_pack.append(error_code);
 *
 * std::vector<uint8_t> result = MessagePack::serialize(message_pack);
 *
 * reply(host_ip, result);
 * \endparcode
 *
 * \return The packed data.
 */
std::vector<uint8_t> MessagePack::serialize(const MessagePack& message_pack
	) noexcept
{
	std::vector<uint8_t> vector;

	for(const Object& object : message_pack.object_vector)
	{
		serialize_(object, vector);
	}

	return vector;
}

#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST
// Nothing to test
#endif

// }}} MessagePack
// {{{ MessagePack::Array

size_t MessagePack::Array::append(const bool value ///< The value to add
	) noexcept
{
	return append_(value, object_vector);
}


size_t MessagePack::Array::append(const int64_t value ///< The value to add
	) noexcept
{
	return append_(value, object_vector);
}


size_t MessagePack::Array::append(const uint64_t value ///< The value to add
	) noexcept
{
	return append_(value, object_vector);
}


size_t MessagePack::Array::append(const float value ///< The value to add
	) noexcept
{
	return append_(value, object_vector);
}


size_t MessagePack::Array::append(const double value ///< The value to add
	) noexcept
{
	return append_(value, object_vector);
}


size_t MessagePack::Array::append(const std::string_view value ///< The value to add
	) noexcept
{
	return append_(value, object_vector);
}


size_t MessagePack::Array::append(const std::vector<uint8_t>& value ///< The value to add
	) noexcept
{
	return append_(value, object_vector);
}


size_t MessagePack::Array::append(std::vector<uint8_t>&& value ///< The value to add
	) noexcept
{
	return append_(std::move(value), object_vector);
}


size_t MessagePack::Array::append(const uint8_t* value ///< The value to add
	, const size_t                           size  ///< The value count
	) noexcept
{
	return append_(value, size, object_vector);
}


size_t MessagePack::Array::append(const Array& array
	) noexcept
{
	size_t index = appendArray_(object_vector);

	Array& sub_array = object_vector[index].asArray();
	sub_array.object_vector = array.object_vector;

	return index;
}


size_t MessagePack::Array::appendArray() noexcept
{
	return appendArray_(object_vector);
}


size_t MessagePack::Array::appendNull() noexcept
{
	return appendNull_(object_vector);
}


MessagePack::Object& MessagePack::Array::object(const size_t index ///< The index of the data object.
	) noexcept
{
	return object_vector[index];
}


const MessagePack::Object& MessagePack::Array::object(const size_t index ///< The index of the data object.
	) const noexcept
{
	return object_vector[index];
}


void MessagePack::Array::clear() noexcept
{
	object_vector.clear();
}


size_t MessagePack::Array::size() const noexcept
{
	return object_vector.size();
}


void MessagePack::Array::deserialize(const std::vector<uint8_t>& data
	, size_t&                                                index
	, const size_t                                           object_count
	) noexcept
{
	for(size_t count = 0; count < object_count; count++)
	{
		deserialize_(data, index, object_vector);
	}
}

#ifdef ZAKERO_MESSAGEPACK_IMPLEMENTATION_TEST
// Nothing to test. Uses the same code as MessagePack::append(bool).
#endif

// }}} MessagePack::Array
// {{{ MessagePack::Object

/**
 * \struct Object
 *
 * \brief A Data Object.
 *
 * This structure contains the basic data-types supported by MessagePack.
 */

/**
 * \var MessagePack::Object::type
 *
 * \brief The data-type.
 *
 * For the primitive data-types, there is a one-to-one correlation to the 
 * anonymous union. If the \p type is DataType::Bool, then the `bool_` value 
 * should be used.
 */

/*
 * \var MessagePack::Object::bool_
 *
 * \brief A value.
 *
 * \note This variable is a member of a union.
 */

/*
 * \var MessagePack::Object::int64_
 *
 * \brief A value.
 *
 * \note This variable is a member of a union.
 */

/*
 * \var MessagePack::Object::uint64_
 *
 * \brief A value.
 *
 * \note This variable is a member of a union.
 */

/*
 * \var MessagePack::Object::float_
 *
 * \brief A value.
 *
 * \note This variable is a member of a union.
 */

/*
 * \var MessagePack::Object::double_
 *
 * \brief A value.
 *
 * \note This variable is a member of a union.
 */

/**
 * \var MessagePack::Object::string
 *
 * \brief A string value.
 */

/**
 * \var MessagePack::Object::vector
 *
 * \brief A binary data.
 */

// }}} MessagePack::Object
// {{{ Utilities

/**
 * \brief Convert to string.
 *
 * \return A string.
 */
std::string to_string(const MessagePack::Object::Type type ///< The value to convert.
	) noexcept
{
	return Object_Type_Name[static_cast<int>(type)];
}

// }}}
} // zakero

// {{{ Operators

/**
 * \brief OStream operator.
 *
 * The \p point object will be written to the request \p stream as a JSON 
 * formatted string.
 *
 * \return The \p stream object.
 */
std::ostream& operator<<(std::ostream& stream           ///< The stream to use.
	, const zakero::MessagePack::Object::Type type  ///< The data to write.
	) noexcept
{
	stream << zakero::to_string(type);
	
	return stream;
}


// }}}

#endif // ZAKERO_MESSAGEPACK_IMPLEMENTATION

// }}}

#endif // zakero_MessagePack_h
