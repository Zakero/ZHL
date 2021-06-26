/******************************************************************************
 * Copyright 2021 Andrew Moore
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
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
 * An object that serializes and deserializes MessagePack formatted data.
 *
 * Include this header in your source code to use these features.
 *
 * \pardeps{zakero_messagepack}
 * - Zakero_Base.h
 * \endpardeps
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
 * \parversion{zakero_messagepack}
 * __v0.0.0__
 * - The initial implementation
 * \endparversion
 *
 * \copyright [Mozilla Public License 
 * v2](https://www.mozilla.org/en-US/MPL/2.0/) 
 *
 * \author Andrew "Zakero" Moore
 * - Original Author
 */


/******************************************************************************
 * Includes
 */

// C++
#include <string>
#include <vector>

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
 *    The error name will be accessible as zakero::Xenium::_ErrorName_
 *    <br><br>
 * -# __ErrorValue__<br>
 *    The integer value of the error and will be available as `int` values in 
 *    the zakero::Xenium class.
 *    <br><br>
 * -# __ErrorMessage__<br>
 *    The text that will be used by `std::error_code.message()`
 */
#define ZAKERO_MESSAGEPACK__ERROR_DATA \
	X(Error_None    ,  0 , "No Error"                                                        ) \
	X(Error_Unknown ,  1 , "An unknown error has occurred"                                   ) \

#define ZAKERO_MESSAGEPACK__FORMAT_DATA \
	X(positive_fixint , 0x00 , "positive fixint" ) \
	X(fixmap          , 0x80 , "fixmap"          ) \
	X(fixarray        , 0x90 , "fixarray"        ) \
	X(fixstr          , 0xa0 , "fixstr"          ) \
	X(nill            , 0xc0 , "nill"            ) \
	X(never_used      , 0xc1 , "(never used)"    ) \
	X(false_          , 0xc2 , "false"           ) \
	X(true_           , 0xc3 , "true"            ) \
	X(bin8            , 0xc4 , "bin 8"           ) \
	X(bin16           , 0xc5 , "bin 16"          ) \
	X(bin32           , 0xc6 , "bin 32"          ) \
	X(ext8            , 0xc7 , "ext 8"           ) \
	X(ext16           , 0xc8 , "ext 16"          ) \
	X(ext32           , 0xc9 , "ext 32"          ) \
	X(float32         , 0xca , "float 32"        ) \
	X(float64         , 0xcb , "float 64"        ) \
	X(uint8           , 0xcc , "uint 8"          ) \
	X(uint16          , 0xcd , "uint 16"         ) \
	X(uint32          , 0xce , "uint 32"         ) \
	X(uint64          , 0xcf , "uint 64"         ) \
	X(int8            , 0xd0 , "int 8"           ) \
	X(int16           , 0xd1 , "int 16"          ) \
	X(int32           , 0xd2 , "int 32"          ) \
	X(int64           , 0xd3 , "int 64"          ) \
	X(fixext1         , 0xd4 , "fixext 1"        ) \
	X(fixext2         , 0xd5 , "fixext 2"        ) \
	X(fixext4         , 0xd6 , "fixext 4"        ) \
	X(fixext8         , 0xd7 , "fixext 8"        ) \
	X(fixext16        , 0xd8 , "fixext 16"       ) \
	X(str8            , 0xd9 , "str 8"           ) \
	X(str16           , 0xda , "str 16"          ) \
	X(str32           , 0xdb , "str 32"          ) \
	X(array16         , 0xdc , "array 16"        ) \
	X(array32         , 0xdd , "array 32"        ) \
	X(map16           , 0xde , "map 16"          ) \
	X(map32           , 0xdf , "map 32"          ) \
	X(negative_fixint , 0xe0 , "negative fixint" ) \

// }}}


namespace zakero
{
	// {{{ Declaration

	class MessagePack
	{
		public:
			enum class Type : uint8_t
			{
#define X(name_, val_, text_) \
				name_ = val_,
				ZAKERO_MESSAGEPACK__FORMAT_DATA
#undef X
			};

			struct Array
			{
			};

			struct Map
			{
			};

			struct Object
			{
			};

			void                               appendArray(size_t* = nullptr);
			void                               appendMap(size_t* = nullptr);
			void                               appendNull(size_t* = nullptr);
			void                               appendValue(bool, size_t* = nullptr);
			void                               appendValue(double, size_t* = nullptr);
			void                               appendValue(int64_t, size_t* = nullptr);
			void                               appendValue(std::string_view, size_t* = nullptr);
			void                               appendValue(std::vector<uint8_t>&, size_t* = nullptr);
			void                               appendValue(uint8_t*, size_t, size_t* = nullptr);
			void                               appendTimeStamp(uint64_t);
			void                               appendExtension(int8_t, std::vector<uint8_t>&, size_t* = nullptr);
			void                               appendExtension(int8_t, uint8_t*, size_t, size_t* = nullptr);
			[[nodiscard]] size_t               size();

			[[nodiscard]] const Array&         array(size_t index);
			[[nodiscard]] const Map&           map(size_t index);
			[[nodiscard]] const Object&        object(size_t index);
			[[nodiscard]] const int8_t         type(size_t index);

			void                               clear();

			void                               deserialize(std::vector<uint8_t>&);

			[[nodiscard]] std::vector<uint8_t> serialize();
			[[nodiscard]] size_t               serializeSize();

		private:
	}; // class MessagePack

	// }}}
}

// {{{ Implementation

#ifdef ZAKERO_XENIUM_IMPLEMENTATION



#endif // ZAKERO_XENIUM_IMPLEMENTATION

// }}}

#endif // zakero_MessagePack_h
