/******************************************************************************
 * Copyright 2021-2022 Andrew Moore
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, value. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef zakero_Network_h
#define zakero_Network_h

/**
 * \file
 *
 * \brief Zakero Network
 *
|     |                              |                              |                              |                             |                             |                                 |
|-----|------------------------------|------------------------------|------------------------------|-----------------------------|-----------------------------|---------------------------------|
| API | \refdeps{zakero_network} | \reftldr{zakero_network} | \refwhat{zakero_network} | \refwhy{zakero_network} | \refhow{zakero_network} | \refversion{zakero_network} |
 * 
 * The _Zakero_Network_ will DO WHAT?
 *
 *
 * \pardeps{zakero_network}
 * - None
 * \endpardeps
 *
 *
 * \partldr{zakero_network}
 * This library makes it very easy to DO WHAT?
 *
 * To use:
 * 1. Add the implementation to a source code file:
 *    \code
 *    #define ZAKERO_NETWORK_IMPLEMENTATION
 *    #include "Zakero_Network.h"
 *    \endcode
 * 2. Add the library to where it is used:
 *    \code
 *    #include "Zakero_Network.h"
 *    \endcode
 * \endpartldr
 *
 *
 * \parwhat{zakero_network}
 * The _Zakero_Network_ library provides a way to DO WHAT?
 * \endparwhat
 *
 *
 * \parwhy{zakero_network}
 * There are many libraries available to do the same thing, however the 
 * _Zakero_Network_ library offers the following:
 * 
 * __Benefits__
 * - ???
 *
 * __Draw Backs__
 * - ???
 *
 * Instead of attempting to interface with other libraries or provide a dynamic 
 * templated interface, basic C++ types are used. It is expected that this 
 * approach will make using the library with other code bases easier and less 
 * volatile.
 * \endparwhy
 *
 *
 * \parhow{zakero_network}
 * __Step 0__
 *
 * Your compiler must support at least the C++20 standard. The location of the 
 * `Zakero_Network.h` header files _must_ be in your compiler's include 
 * path.
 *
 * __Step 1__
 *
 * The first step is to select which C++ source code file will contain the 
 * _Zakero Network_ implementation. Once the location has been determined, 
 * add the following to that file:
 *
 * ~~~
 * #define ZAKERO_NETWORK_IMPLEMENTATION
 * #include "Zakero_Network.h"
 * ~~~
 *
 * The macro \ref ZAKERO_NETWORK_IMPLEMENTATION tells the header file to 
 * include the implementation of the Network.
 *
 * In all other files that will use the Network, they need to include the 
 * header.
 *
 * ~~~
 * #include "Zakero_Network.h"
 * ~~~
 *
 * __Step 2__
 *
 * ADD SIMPLE EXAMPLE
 *
 * \endparhow
 *
 *
 * \parversion{zakero_network}
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
#include <limits>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

// POSIX
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

// Linux


/******************************************************************************
 * Macros
 */

// {{{ Macros

/**
 * \internal
 *
 * \brief Error_Table
 *
 * An X-Macro table of error codes. The columns are:
 * -# __ErrorName__<br>
 *    The error name will be accessible as zakero::Network::_ErrorName_
 *    <br><br>
 * -# __ErrorValue__<br>
 *    The integer value of the error and will be available as `int` values in 
 *    the zakero::Network class.
 *    <br><br>
 * -# __ErrorMessage__<br>
 *    The text that will be used by `std::error_code.message()`
 */
#define ZAKERO_NETWORK__ERROR_DATA \
	X(Error_None                    ,  0 , "No Error"                                                                                        ) \
	X(Error_Unknown                 ,  1 , "An unknown error has occurred"                                                                   ) \
	X(Error_IPv4_Invalid_Notation   ,  2 , "The IPv4 numbers-and-dots notation was not valid"                                                ) \
	X(Error_Address_Family          ,  3 , "The specified network host does not have any network addresses in the requested address family." ) \
	X(Error_Try_Again_Later         ,  4 , "The name server returned a temporary failure indication. Try again later."                       ) \
	X(Error_Bad_Hints               ,  5 , "The provided hint flags were invalid."                                                           ) \
	X(Error_Failure                 ,  6 , "The name server returned a permanent failure indication."                                        ) \
	X(Error_Family                  ,  7 , "The requested address family is not supported."                                                  ) \
	X(Error_Out_Of_Memory           ,  8 , "Out of memory."                                                                                  ) \
	X(Error_No_Data                 ,  9 , "The  specified  network  host  exists, but does not have any network addresses defined."         ) \
	X(Error_No_Name                 , 10 , "The provided name is not known."                                                                 ) \
	X(Error_Invalid_Service         , 11 , "The requested service is not available for the requested socket type."                           ) \
	X(Error_Invalid_Socket_Type     , 12 , "The requested socket type is not supported."                                                     ) \
	X(Error_Invalid_IP              , 13 , "The provided IP object is not valid."                                                            ) \
	X(Error_Permission_Denied       , 14 , "The address or port is protected."                                                               ) \
	X(Error_Port_Busy               , 15 , "The requested port is being used."                                                               ) \
	X(Error_Invalid_Socket_FD       , 16 , "The socket file descriptor is not valid."                                                        ) \
	X(Error_Socket_Busy             , 17 , "The socket is already bound."                                                                    ) \
	X(Error_Invalid_Socket          , 18 , "The socket is invalid."                                                                          ) \
	X(Error_Invalid_Address         , 19 , "The requested address is not valid."                                                             ) \
	X(Error_Bad_Pointer             , 20 , "An internal pointer has become corrupted."                                                       ) \
	X(Error_Too_Many_Links          , 21 , "Too many links were encountered resolving an address."                                           ) \
	X(Error_Address_Too_Long        , 22 , "The address is too long."                                                                        ) \
	X(Error_Invalid_Socket_Path     , 23 , "Part of the socket path does not exist."                                                         ) \
	X(Error_Invalid_Socket_Dir      , 24 , "Part of the socket path is not a directory."                                                     ) \
	X(Error_Socket_Read_Only        , 25 , "The socket inode is read-only."                                                                  ) \
	X(Error_Not_Supported           , 26 , "The request operation is not supported."                                                         ) \
	X(Error_No_Connection_Available , 27 , "The request operation is not supported."                                                         ) \
	X(Error_Connection_Aborted      , 28 , "A connection has been aborted."                                                                  ) \
	X(Error_Address_Not_Writable    , 29 , "Data is now in a writable part of the user address space."                                       ) \
	X(Error_Interrupted             , 30 , "Interrupted before a connection arrived."                                                        ) \
	X(Error_Not_Listening           , 31 , "The socket is not listening for connections."                                                    ) \
	X(Error_No_More_Process_FD      , 32 , "No more file descriptors are available for the process."                                         ) \
	X(Error_No_More_System_FD       , 33 , "No more file descriptors are available for the system."                                         ) \
	X(Error_Protocol                , 34 , "Protocol error."                                                                                ) \
// }}}


/******************************************************************************
 * Objects
 */

namespace zakero::network
{
	// {{{ Error

	class ErrorCategory_
		: public std::error_category
	{
		public:
			constexpr ErrorCategory_() noexcept {};

			[[nodiscard]] const char* name() const noexcept final override;
			[[nodiscard]] std::string message(int condition) const noexcept final override;
	};

	extern ErrorCategory_ ErrorCategory;

#define X(name_, val_, mesg_) \
	const std::error_code name_(val_, ErrorCategory);
	ZAKERO_NETWORK__ERROR_DATA
#undef X

	// }}}
	// {{{ IP

	class IP
	{
		public:
			virtual ~IP() noexcept {};
			[[nodiscard]] virtual struct in_addr address() const noexcept = 0;
			[[nodiscard]] virtual IP*            copy() const noexcept = 0;
			[[nodiscard]] virtual int            family() const noexcept = 0;
			[[nodiscard]] virtual std::string    string() const noexcept = 0;
			[[nodiscard]] virtual int            version() const noexcept = 0;
	};

	// {{{ IP v4

	class IPv4
		: public IP
	{
		public:
			[[nodiscard]] static IP*                   create(const std::string) noexcept;
			[[nodiscard]] static IP*                   create(const std::string, std::error_code&) noexcept;
			[[nodiscard]] static std::set<std::string> hostname(const std::string) noexcept;
			[[nodiscard]] static std::set<std::string> hostname(const std::string, std::error_code&) noexcept;

			[[nodiscard]] struct in_addr address() const noexcept final override;
			[[nodiscard]] IP*            copy() const noexcept final override;
			[[nodiscard]] int            family() const noexcept final override;
			[[nodiscard]] std::string    string() const noexcept final override;
			[[nodiscard]] int            version() const noexcept final override;

		private:
			mutable std::string string_ = {};
			in_addr             addr_   = { 0 };

			[[nodiscard]] static IP* create(in_addr) noexcept;
	};

	// }}}
	// {{{ IP v6

	class IPv6
	{
		IPv6() noexcept;
	};

	// }}}
	// }}}
	// {{{ TCP
// {{{ TCP (Private)

	class TCP_
	{
		public:
			virtual ~TCP_() noexcept;

			[[nodiscard]] const IP&   ip() const noexcept;
			[[nodiscard]] uint16_t    port() const noexcept;
			[[nodiscard]] int         socket() const noexcept;
			[[nodiscard]] std::string string() const noexcept;

		protected:
			IP*                ip_;                       // Server
			uint16_t           port_;                     // Server
			int                type_       = SOCK_STREAM; // Server
			int                protocol_   = 0;           // Server
			struct sockaddr_in addr_       = { 0 };       // Server
			int                socket_     = -1;          // Server
			int                recv_flags_ = 0;
			int                send_flags_ = 0;

			TCP_(IP*, uint16_t) noexcept;
	};

// }}}
// {{{ TCP (Base)

			class TCPServer;
	class TCP
		: public TCP_
	{
		public:
			virtual ~TCP() noexcept;

			[[]]          std::vector<uint8_t> read(const size_t) const noexcept;

			[[]]          ssize_t              write(std::string_view) const noexcept;
			[[]]          ssize_t              write(std::vector<char8_t>) const noexcept;
			[[]]          ssize_t              write(std::vector<int8_t>) const noexcept;
			[[]]          ssize_t              write(std::vector<uint8_t>) const noexcept;

		protected:
			friend TCPServer;
			TCP(IP*, uint16_t) noexcept;
	};

// }}}
	// {{{ TCP Client

	class TCPClient
		: public TCP
	{
		public:
			virtual ~TCPClient() noexcept;

			[[nodiscard]] static TCPClient* create(IP*&, const uint16_t) noexcept;
			[[nodiscard]] static TCPClient* create(IP*&, const uint16_t, std::error_code&) noexcept;
			[[nodiscard]] static TCPClient* create(const IP&, const uint16_t) noexcept;
			[[nodiscard]] static TCPClient* create(const IP&, const uint16_t, std::error_code&) noexcept;

			[[]]          bool              connect() noexcept;

		private:
			TCPClient(IP*, uint16_t) noexcept;
	};

	// }}}
	// {{{ TCP Server

	class TCPServer
		: public TCP_
	{
		public:
			virtual ~TCPServer() noexcept;

			[[nodiscard]] static TCPServer* create(IP*&, const uint16_t) noexcept;
			[[nodiscard]] static TCPServer* create(IP*&, const uint16_t, std::error_code&) noexcept;
			[[nodiscard]] static TCPServer* create(const IP&, const uint16_t) noexcept;
			[[nodiscard]] static TCPServer* create(const IP&, const uint16_t, std::error_code&) noexcept;

			[[nodiscard]] TCP*              waitForConnection() noexcept;
			[[nodiscard]] TCP*              waitForConnection(std::error_code&) noexcept;

		private:
			TCPServer(IP*, uint16_t) noexcept;

			[[nodiscard]] std::error_code   init() noexcept;
	};

	// }}}
	// }}}
	// {{{ UDP

	class UDP
	{
	};

	// }}}
	// {{{ OpenSSL

	class OpenSSL
	{
	};

	// }}}
} // zakero::network

// {{{ Operators
// }}}
// {{{ Implementation

#ifdef ZAKERO_NETWORK_IMPLEMENTATION

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST
#include <thread>
#endif

// {{{ Macros
// {{{ Macros : Doxygen

#ifdef ZAKERO__DOXYGEN_DEFINE_DOCS

// Only used for generating Doxygen documentation

/**
 * \brief Activate the implementation code.
 *
 * Defining this macro will cause the zakero::Network implementation to be 
 * included. This should only be done once, since compiler and/or linker errors 
 * will typically be generated if more than a single implementation is found.
 *
 * \note It does not matter if the macro is given a value or not, only its 
 * existence is checked.
 */
#define ZAKERO_NETWORK_IMPLEMENTATION

/**
 * \def X(name_, val_, mesg_)
 *
 * \brief Generate Code.
 */

#endif

// }}}
// }}}

namespace zakero::network
{
// {{{ Documentation

/**
 * \class ErrorCategory_
 *
 * \brief Network Error Categories.
 */

// }}}
// {{{ Anonymous Namespace

namespace
{
#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
#ifdef ZAKERO_NETWORK_TEST_IP
	const std::string Test_IP = ZAKERO_NETWORK_TEST_IP;
#else
	const std::string Test_IP = "140.82.112.3"; // github.com
#endif
#endif // }}}

	std::error_code accept_error_code(const int error
		) noexcept
	{
		switch(error)
		{
			// Posix
			case EAGAIN:       return Error_No_Connection_Available;
			case EBADF:        return Error_Invalid_Socket_FD;
			case ECONNABORTED: return Error_Connection_Aborted;
			case EFAULT:       return Error_Address_Not_Writable;
			case EINTR:        return Error_Interrupted;
			case EINVAL:       return Error_Not_Listening;
			case EMFILE:       return Error_No_More_Process_FD;
			case ENFILE:       return Error_No_More_System_FD;
			case ENOBUFS:      return Error_Out_Of_Memory;
			case ENOMEM:       return Error_Out_Of_Memory;
			case ENOTSOCK:     return Error_Invalid_Socket;
			case EOPNOTSUPP:   return Error_Not_Supported;
			case EPROTO:       return Error_Protocol;
			case EPERM:        return Error_Permission_Denied;
			default:
				return std::error_code(error, std::system_category());
		}

		return Error_Unknown;
	}


	std::error_code bind_error_code(const int error
		) noexcept
	{
		switch(error)
		{
			case EACCES:         return Error_Permission_Denied;
			case EADDRINUSE:     return Error_Port_Busy;
			case EBADF:          return Error_Invalid_Socket_FD;
			case EINVAL:         return Error_Socket_Busy;
			case ENOTSOCK:       return Error_Invalid_Socket;
			case EADDRNOTAVAIL:  return Error_Invalid_Address;
			case EFAULT:         return Error_Bad_Pointer;
			case ELOOP:          return Error_Too_Many_Links;
			case ENAMETOOLONG:   return Error_Address_Too_Long;
			case ENOENT:         return Error_Invalid_Socket_Path;
			case ENOMEM:         return Error_Out_Of_Memory;
			case ENOTDIR:        return Error_Invalid_Socket_Dir;
			case EROFS:          return Error_Socket_Read_Only;
			default:
				return std::error_code(error, std::system_category());
		}

		return Error_Unknown;
	}


	std::error_code eai_error_code(const int eai
		, const int err
		) noexcept
	{
		switch(eai)
		{
			case EAI_ADDRFAMILY: return Error_Address_Family;
			case EAI_AGAIN:      return Error_Try_Again_Later;
			case EAI_BADFLAGS:   return Error_Bad_Hints;
			case EAI_FAIL:       return Error_Failure;
			case EAI_FAMILY:     return Error_Family;
			case EAI_MEMORY:     return Error_Out_Of_Memory;
			case EAI_NODATA:     return Error_No_Data;
			case EAI_NONAME:     return Error_No_Name;
			case EAI_SERVICE:    return Error_Invalid_Service;
			case EAI_SOCKTYPE:   return Error_Invalid_Socket_Type;
			case EAI_SYSTEM:
				return std::error_code(err, std::system_category());
		}

		return Error_Unknown;
	}


	std::error_code listen_error_code(const int error
		) noexcept
	{
		switch(error)
		{
			case EADDRINUSE:     return Error_Port_Busy;
			case EBADF:          return Error_Invalid_Socket_FD;
			case ENOTSOCK:       return Error_Invalid_Socket;
			default:
				return std::error_code(error, std::system_category());
		}

		return Error_Unknown;
	}
}

// }}}
// {{{ Error

/**
 * \class ErrorCategary_
 *
 * \brief Error categories.
 *
 * This class holds all the error categories for the error codes. The data in 
 * this class is built from the ZAKERO_NETWORK__ERROR_DATA macro.
 */


/**
 * \fn ErrorCategory_::ErrorCategory_()
 *
 * \brief Constructor
 */


/**
 * \brief The name of the error category.
 * 
 * \return A C-Style string.
 */
const char* ErrorCategory_::name() const noexcept
{
	return "zakero::network";
}


/**
 * \brief A description message.
 *
 * \return The message.
 */
std::string ErrorCategory_::message(int condition ///< The error code.
	) const noexcept
{
	switch(condition)
	{
#define X(name_, val_, mesg_) \
		case val_: return mesg_;
		ZAKERO_NETWORK__ERROR_DATA
#undef X
	}

	return "Unknown error condition";
}


/**
 * \brief A single instance.
 *
 * This one instance will be used by all error codes.
 */
ErrorCategory_ ErrorCategory;

// }}}
// {{{ IP v4

IP* IPv4::create(const std::string address
	) noexcept
{
	std::error_code error;

	return create(address, error);
}


IP* IPv4::create(const std::string address
	, std::error_code& error
	) noexcept
{
	in_addr addr = { 0 };

	int retval = inet_aton(address.data(), &addr);
	if(retval == 0)
	{
		error = Error_IPv4_Invalid_Notation;
		return nullptr;
	}

	error = Error_None;
	IP* ip = create(addr);

	return ip;
}


IP* IPv4::create(in_addr addr
	) noexcept
{
	IPv4* ip = new IPv4();
	ip->addr_ = addr;

	return ip;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("ipv4/create")
{
	IP*             ip    = nullptr;
	std::error_code error = {};

	SUBCASE("IPv4 Invalid")
	{
		ip = IPv4::create("", error);
		CHECK(ip    == nullptr);
		CHECK(error == Error_IPv4_Invalid_Notation);
		delete ip;

		ip = IPv4::create("aaa.bbb.ccc.ddd", error);
		CHECK(ip    == nullptr);
		CHECK(error == Error_IPv4_Invalid_Notation);
		delete ip;

		ip = IPv4::create("333.444.555.666", error);
		CHECK(ip    == nullptr);
		CHECK(error == Error_IPv4_Invalid_Notation);
		delete ip;
	}

	SUBCASE("IPv4 Valid")
	{
		ip = IPv4::create("0", error);
		CHECK(ip    != nullptr);
		CHECK(error == Error_None);
		delete ip;

		ip = IPv4::create("0x7f000001", error);
		CHECK(ip    != nullptr);
		CHECK(error == Error_None);
		delete ip;

		ip = IPv4::create("0.0.0.0", error);
		CHECK(ip    != nullptr);
		CHECK(error == Error_None);
		delete ip;

		ip = IPv4::create("000.000.000.000", error);
		CHECK(ip    != nullptr);
		CHECK(error == Error_None);
		delete ip;

		ip = IPv4::create("127.0.0.1", error);
		CHECK(ip    != nullptr);
		CHECK(error == Error_None);
		delete ip;

		ip = IPv4::create("127.000.000.001", error);
		CHECK(ip    != nullptr);
		CHECK(error == Error_None);
		delete ip;

		ip = IPv4::create("255.255.255.255", error);
		CHECK(ip    != nullptr);
		CHECK(error == Error_None);
		delete ip;
	}
}
#endif // }}}


std::set<std::string> IPv4::hostname(const std::string hostname
	) noexcept
{
	std::error_code error;

	return IPv4::hostname(hostname, error);
}


std::set<std::string> IPv4::hostname(const std::string hostname
	, std::error_code& error
	) noexcept
{
	struct addrinfo hints =
	{	.ai_flags     = AI_ADDRCONFIG
	,	.ai_family    = AF_INET
	,	.ai_socktype  = 0
	,	.ai_protocol  = 0
	,	.ai_addrlen   = 0
	,	.ai_addr      = nullptr
	,	.ai_canonname = nullptr
	,	.ai_next      = nullptr
	};

	struct addrinfo* result = nullptr;

	errno = 0;
	int retval = getaddrinfo(hostname.c_str(), nullptr, &hints, &result);

	if(retval != 0)
	{
		error = eai_error_code(retval, errno);
		return {};
	}

	std::set<std::string> set = {};

	for(struct addrinfo* r = result; r != nullptr; r = r->ai_next)
	{
		struct sockaddr_in* addr = (struct sockaddr_in*)r->ai_addr;
		set.insert(inet_ntoa(addr->sin_addr));
	}

	freeaddrinfo(result);

	error = Error_None;

	return set;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("ipv4/hostname")
{
	std::set<std::string> set   = {};
	std::error_code       error = {};

	SUBCASE("Hostname Invalid")
	{
		set = IPv4::hostname("2607:f8b0:4000:81a::200e", error); // IPv6 address
		CHECK(set.empty() == true);
		CHECK(error       == Error_Address_Family);

		set = IPv4::hostname("", error);
		CHECK(set.empty() == true);
		CHECK(error       == Error_No_Name);

		set = IPv4::hostname("does.not.exist", error);
		CHECK(set.empty() == true);
		CHECK(error       == Error_No_Name);
	}

	SUBCASE("Hostname Valid")
	{
		auto contains_127_0_0_1 = [](const std::set<std::string>& set) -> bool
		{
			for(const std::string& s : set)
			{
				if(s == "127.0.0.1")
				{
					return true;
				}
			}

			return false;
		};

		set = IPv4::hostname("localhost", error);
		CHECK(set.empty()             == false);
		CHECK(error                   == Error_None);
		CHECK(contains_127_0_0_1(set) == true);

		set = IPv4::hostname("127.0.0.1", error);
		CHECK(set.empty() == false);
		CHECK(error       == Error_None);
		CHECK(contains_127_0_0_1(set) == true);
	}
}
#endif // }}}


struct in_addr IPv4::address(
	) const noexcept
{
	return addr_;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("ipv4/address")
{
	const char*    ip_addr = "127.0.0.1";
	struct in_addr addr    = { 0 };

	inet_aton(ip_addr, &addr);

	IP* ip = IPv4::create(ip_addr);

	CHECK(inet_lnaof(addr) == inet_lnaof(ip->address()));
	CHECK(inet_netof(addr) == inet_netof(ip->address()));

	delete ip;
}
#endif // }}}


IP* IPv4::copy(
	) const noexcept
{
	IP* ip = create(this->addr_);

	return ip;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("ipv4/copy")
{
	IP* ip      = IPv4::create("127.0.0.1");
	IP* ip_copy = ip->copy();

	CHECK(ip            != ip_copy);
	CHECK(ip->string()  == ip_copy->string());
	CHECK(ip->version() == ip_copy->version());

	delete ip;
	delete ip_copy;
}
#endif // }}}


int IPv4::family(
	) const noexcept
{
	return AF_INET;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("ipv4/family")
{
	IP* ip = IPv4::create("127.0.0.1");
	CHECK(ip->family() == AF_INET);

	delete ip;
}
#endif // }}}


std::string IPv4::string(
	) const noexcept
{
	if(string_.empty())
	{
		string_ = std::string(inet_ntoa(addr_));
	}

	return string_;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("ipv4/string")
{
	IP* ip = nullptr;

	SUBCASE("IPv4 Localhost 1")
	{
		ip = IPv4::create("127.0.0.1");
		CHECK(ip->string() == "127.0.0.1");
	}

	SUBCASE("IPv4 Localhost 2")
	{
		ip = IPv4::create("127.000.000.001");
		CHECK(ip->string() == "127.0.0.1");
	}

	delete ip;
}
#endif // }}}


int IPv4::version(
	) const noexcept
{
	return 4;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("ipv4/version")
{
	IP* ip = IPv4::create("127.0.0.1");
	CHECK(ip->version() == 4);
	delete ip;
}
#endif // }}}

// }}}
// {{{ IP v6


// }}}
// {{{ TCP
// {{{ TCP (Private)

TCP_::TCP_(IP* ip
	, uint16_t port
	) noexcept
	: ip_(ip)
	, port_(port)
{
}


TCP_::~TCP_(
	) noexcept
{
	if(socket_ >= 0)
	{
		close(socket_);
	}

	delete ip_;

	ip_         = nullptr;
	port_       = 0;
	type_       = SOCK_STREAM;
	protocol_   = 0;
	addr_       = { 0 };
	socket_     = -1;
	recv_flags_ = { 0 };
	send_flags_ = { 0 };
}


/**
 * \brief Access the IP object.
 */
const IP& TCP_::ip(
	) const noexcept
{
	return *ip_;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("tcp/ip")
{
	SUBCASE("IPv4")
	{
		IP* ip = IPv4::create("127.0.0.1");
		uint16_t port = 65535;

		TCPClient* tcp = TCPClient::create(ip, port);
		CHECK(tcp->ip().version() == 4);
	}
}
#endif // }}}


uint16_t TCP_::port(
	) const noexcept
{
	return port_;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("tcp/port")
{
	IP* ip = IPv4::create("127.0.0.1");
	uint16_t port = 65535;

	TCPClient* tcp = TCPClient::create(ip, port);
	CHECK(tcp->port() == port);

	delete tcp;
}
#endif // }}}


int TCP_::socket(
	) const noexcept
{
	return socket_;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("tcp/socket")
{
	IP* ip = IPv4::create("127.0.0.1");
	uint16_t port = 65535;

	SUBCASE("IPv4 Not Connected")
	{
		TCPClient* tcp = TCPClient::create(ip, port);
		CHECK(tcp->socket() == -1);
	}
}
#endif // }}}

std::string TCP_::string(
	) const noexcept
{
	std::string s = ip_->string() + ":" + std::to_string(port_);

	return s;
}

// }}}
// {{{ TCP (Base)

TCP::TCP(IP* ip
	, uint16_t port
	) noexcept
	: TCP_(ip, port)
{
}


TCP::~TCP(
	) noexcept
{
}


std::vector<uint8_t> TCP::read(const size_t max_bytes
	) const noexcept
{
	std::vector<uint8_t> data(max_bytes, 0);

	ssize_t bytes = recv(socket_, (void*)data.data(), max_bytes, recv_flags_);

	if(bytes < 0)
	{
		return {};
	}

	data.resize(bytes);

	return data;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("tcp/read")
{
	IP* ip = IPv4::create(Test_IP);
	uint16_t port = 80;

	TCPClient* tcp = TCPClient::create(ip, port);

	tcp->connect();
	tcp->write("GET / HTTP/1.1\r\n\r\n");

	std::vector<uint8_t> data = tcp->read(256);

	CHECK(data.empty() == false);

	/*
	std::string string = {};
	for(uint8_t ch : data)
	{
		string.push_back(ch);
	}
	std::cout << "Message:" << string << "\n";
	*/

	delete tcp;
}
#endif // }}}


ssize_t TCP::write(std::string_view data
	) const noexcept
{
	ssize_t bytes = send(socket_, (void*)data.data(), data.size(), send_flags_);

	if(bytes < 0)
	{
		return bytes;
	}

	return bytes;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("tcp/write/string_view")
{
	IP* ip = IPv4::create(Test_IP);
	uint16_t port = 80;

	TCPClient* tcp = TCPClient::create(ip, port);

	tcp->connect();

	std::string_view message = "GET / HTTP/1.1\r\n\r\n";
	ssize_t bytes = tcp->write(message);

	CHECK(bytes > 0);

	delete tcp;
}
#endif // }}}


ssize_t TCP::write(std::vector<char8_t> data
	) const noexcept
{
	ssize_t bytes = send(socket_, (void*)data.data(), data.size(), send_flags_);

	if(bytes < 0)
	{
		return bytes;
	}

	return bytes;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("tcp/write/vector/char8_t")
{
	IP* ip = IPv4::create(Test_IP);
	uint16_t port = 80;

	TCPClient* tcp = TCPClient::create(ip, port);

	tcp->connect();

	std::string_view message = "GET / HTTP/1.1\r\n\r\n";
	std::vector<char8_t> data(std::begin(message), std::end(message));
	ssize_t bytes = tcp->write(data);

	CHECK(bytes > 0);

	delete tcp;
}
#endif // }}}


ssize_t TCP::write(std::vector<int8_t> data
	) const noexcept
{
	ssize_t bytes = send(socket_, (void*)data.data(), data.size(), send_flags_);

	if(bytes < 0)
	{
		return bytes;
	}

	return bytes;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("tcp/write/vector/int8_t")
{
	IP* ip = IPv4::create(Test_IP);
	uint16_t port = 80;

	TCPClient* tcp = TCPClient::create(ip, port);

	tcp->connect();

	std::string_view message = "GET / HTTP/1.1\r\n\r\n";
	std::vector<int8_t> data(std::begin(message), std::end(message));
	ssize_t bytes = tcp->write(data);

	CHECK(bytes > 0);

	delete tcp;
}
#endif // }}}


ssize_t TCP::write(std::vector<uint8_t> data
	) const noexcept
{
	ssize_t bytes = send(socket_, (void*)data.data(), data.size(), send_flags_);

	if(bytes < 0)
	{
		return bytes;
	}

	return bytes;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("tcp/write/vector/uint8_t")
{
	IP* ip = IPv4::create(Test_IP);
	uint16_t port = 80;

	TCPClient* tcp = TCPClient::create(ip, port);

	tcp->connect();

	std::string_view message = "GET / HTTP/1.1\r\n\r\n";
	std::vector<uint8_t> data(std::begin(message), std::end(message));
	ssize_t bytes = tcp->write(data);

	CHECK(bytes > 0);

	delete tcp;
}
#endif // }}}

// }}}
// {{{ TCP Client

TCPClient::TCPClient(IP* ip
	, uint16_t port
	) noexcept
	: TCP(ip, port)
{
}


TCPClient::~TCPClient(
	) noexcept
{
}


TCPClient* TCPClient::create(IP*& ip
	, const uint16_t port
	) noexcept
{
	std::error_code error;

	return TCPClient::create(ip, port, error);
}


TCPClient* TCPClient::create(IP*& ip
	, const uint16_t   port
	, std::error_code& error
	) noexcept
{
	if(ip == nullptr)
	{
		error = Error_Invalid_IP;
		return nullptr;
	}

	TCPClient* tcp = TCPClient::create(*ip, port, error);

	ip = nullptr;

	return tcp;
}


TCPClient* TCPClient::create(const IP& ip
	, const uint16_t port
	) noexcept
{
	std::error_code error;

	return TCPClient::create(ip, port, error);
}


TCPClient* TCPClient::create(const IP& ip
	, const uint16_t   port
	, std::error_code& error
	) noexcept
{
	TCPClient* tcp = new TCPClient(ip.copy(), port);

	return tcp;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("tcp/client/create")
{
	uint16_t port = 65535;

	SUBCASE("Invalid IP")
	{
		IP* ip = nullptr;
		TCPClient* tcp = TCPClient::create(ip, port);
		CHECK(tcp == nullptr);
		delete tcp;
	}

	SUBCASE("Valid IP")
	{
		IP* ip = IPv4::create("127.0.0.1");

		TCPClient* tcp = TCPClient::create(ip, port);
		CHECK(tcp != nullptr);

		delete tcp;
	}
}
#endif // }}}


bool TCPClient::connect(
	) noexcept
{
	if(socket_ < 0)
	{
		// TODO : Add error handling
		socket_ = ::socket(ip_->family(), type_, protocol_);

		if(socket_ < 0)
		{
			return false;
		}
	}

	addr_.sin_family = ip_->family();
	addr_.sin_port   = htons(port_);
	addr_.sin_addr   = ip_->address();

	// TODO : Add error handling

	int retval = ::connect(socket_, (struct sockaddr*)&addr_, sizeof(addr_));

	if(retval < 0)
	{
		return false;
	}

	return true;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
#if 1 // Disable if no external internet access
TEST_CASE("tcp/connect")
{
	SUBCASE("IPv4")
	{
		IP* ip = IPv4::create(Test_IP);
		uint16_t port = 80;

		TCPClient* tcp = TCPClient::create(ip, port);
		CHECK(tcp->connect() == true);

		delete tcp;
	}
}
#endif
#endif // }}}

// }}}
// {{{ TCP Server

TCPServer::TCPServer(IP* ip
	, uint16_t port
	) noexcept
	: TCP_(ip, port)
{
}


TCPServer::~TCPServer(
	) noexcept
{
}


TCPServer* TCPServer::create(IP*& ip
	, const uint16_t port
	) noexcept
{
	std::error_code error;

	return TCPServer::create(ip, port, error);
}


TCPServer* TCPServer::create(IP*& ip
	, const uint16_t   port
	, std::error_code& error
	) noexcept
{
	if(ip == nullptr)
	{
		error = Error_Invalid_IP;
		return nullptr;
	}

	TCPServer* tcp = TCPServer::create(*ip, port, error);

	ip = nullptr;

	return tcp;
}


TCPServer* TCPServer::create(const IP& ip
	, const uint16_t port
	) noexcept
{
	std::error_code error;

	return TCPServer::create(ip, port, error);
}


TCPServer* TCPServer::create(const IP& ip
	, const uint16_t   port
	, std::error_code& error
	) noexcept
{
	TCPServer* tcp = new TCPServer(ip.copy(), port);
	error = tcp->init();

	if(error != Error_None)
	{
		delete tcp;
		tcp = nullptr;
	}

	return tcp;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("tcp/server/create")
{
	const uint16_t  port  = 65535;
	std::error_code error = {};

	SUBCASE("IP Invalid")
	{
		IP* ip = nullptr;
		TCPServer* tcp = TCPServer::create(ip, port, error);
		CHECK(tcp   == nullptr);
		CHECK(error == Error_Invalid_IP);
	}

	SUBCASE("IP Take-Ownership")
	{
		IP* ip = IPv4::create("127.0.0.1");

		TCPServer* tcp = TCPServer::create(ip, port, error);
		CHECK(tcp   != nullptr);
		CHECK(ip    == nullptr);
		CHECK(error == Error_None);
		delete tcp;
	}

	SUBCASE("IP Make Copy")
	{
		IP* ip = IPv4::create("127.0.0.1");

		TCPServer* tcp = TCPServer::create(*ip, port, error);
		CHECK(tcp   != nullptr);
		CHECK(ip    != nullptr);
		CHECK(error == Error_None);
		delete tcp;

		delete ip;
	}

	SUBCASE("IP Adddress Invalid")
	{
		IP*      ip   = IPv4::create("10.10.10.10");
		uint16_t port = 9999;

		TCPServer* tcp = TCPServer::create(ip, port, error);
		CHECK(tcp   == nullptr);
		CHECK(errno == EADDRNOTAVAIL);
		CHECK(error == Error_Invalid_Address);
		delete tcp;
	}

	SUBCASE("Port Invalid")
	{
		IP*      ip   = IPv4::create("127.0.0.1");
		uint16_t port = 1;

		TCPServer* tcp = TCPServer::create(ip, port, error);
		CHECK(tcp   == nullptr);
		CHECK(errno == EACCES);
		CHECK(error == Error_Permission_Denied);
		delete tcp;
	}

	SUBCASE("TCPServer Valid")
	{
		IP*      ip   = IPv4::create("127.0.0.1");
		uint16_t port = 9999;

		TCPServer* tcp = TCPServer::create(ip, port, error);
		CHECK(error == Error_None);
		delete tcp;
	}
}
#endif // }}}


std::error_code TCPServer::init(
	) noexcept
{
	if(socket_ >= 0)
	{
		close(socket_);
	}

	errno = 0;
	socket_ = ::socket(ip_->family(), type_, protocol_);

	if(socket_ < 0)
	{
		return std::error_code(errno, std::system_category());
	}

	addr_.sin_family = ip_->family();
	addr_.sin_port   = htons(port_);
	addr_.sin_addr   = ip_->address();

	int retval = 0;

	errno = 0;
	retval = ::bind(socket_, (struct sockaddr*)&addr_, sizeof(addr_));
	if(retval < 0)
	{
		retval = errno;

		close(socket_);
		socket_ = -1;

		errno = retval;
		return bind_error_code(retval);
	}

	errno = 0;
	retval = listen(socket_, 3); // "3" should be a variable
	if(retval < 0)
	{
		retval = errno;

		close(socket_);
		socket_ = -1;

		errno = retval;
		return listen_error_code(retval);
	}

	return Error_None;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
// Tested via TCPServer::create()
#endif // }}}


TCP* TCPServer::waitForConnection(
	) noexcept
{
	std::error_code error;

	return this->waitForConnection(error);
}


TCP* TCPServer::waitForConnection(std::error_code& error
	) noexcept
{
	if(socket_ < 0)
	{
		error = init();

		if(error)
		{
			return nullptr;
		}
	}

	struct sockaddr_in client_socket;
	size_t socklen = sizeof(client_socket);
	int retval = 0;
	errno = 0;
	retval = accept(socket_
		, (struct sockaddr*)&client_socket
		, (socklen_t*)&socklen);

	if(retval < 0)
	{
		error = accept_error_code(errno);
		return nullptr;
	}

	IP* client_ip = IPv4::create(inet_ntoa(client_socket.sin_addr));
	if(client_ip == nullptr)
	{
		error = Error_Unknown;
		return nullptr;
	}

	error = Error_None;

	TCP* client = new TCP(client_ip, client_socket.sin_port);

	return client;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("tcp/server/waitforconnection")
{
	bool made_connection = false;
	bool server_ready    = false;

	auto thread = std::jthread([&]()
	{
		IP*             ip           = IPv4::create("0.0.0.0");
		uint16_t        port         = 9999;
		std::error_code server_error = {};

		TCPServer* server = TCPServer::create(ip, port);
		server_ready = true;
		TCP* client = server->waitForConnection(server_error);
		if(server_error)
		{
			FAIL("TCPServer Error: ", server_error.message());
		}

		if(client != nullptr)
		{
			made_connection = true;
		}

		delete client;
		delete server;
	});

	IP* ip = IPv4::create("127.0.0.1");
	uint16_t port = 9999;

	TCPClient* client = TCPClient::create(ip, port);

	do
	{
		sleep(1);
	} while(server_ready == false);
	client->connect();

	thread.join();

	delete client;

	CHECK(made_connection == true);
}
#endif // }}}

// }}}
// }}}
// {{{ UDP


// }}}
// {{{ OpenSSL


// }}}
} // zakero::network

// {{{ Operators
// }}}

#endif // ZAKERO_NETWORK_IMPLEMENTATION

// }}}

#endif // zakero_Network_h
