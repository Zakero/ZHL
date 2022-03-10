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
	X(Error_None                , 0 , "No Error"                                ) \
	X(Error_Unknown             , 1 , "An unknown error has occurred"           ) \

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
			[[nodiscard]] virtual IP*            copy()  const noexcept = 0;
			[[nodiscard]] virtual int            family()  const noexcept = 0;
			[[nodiscard]] virtual std::string    string()  const noexcept = 0;
			[[nodiscard]] virtual int            version() const noexcept = 0;
	};

// }}}
// {{{ IP v4

	class IPv4
		: public IP
	{
		public:
			[[nodiscard]] static IPv4* create(const std::string) noexcept;

			[[nodiscard]] struct in_addr address() const noexcept final override;
			[[nodiscard]] IP*            copy() const noexcept final override;
			[[nodiscard]] int            family() const noexcept final override;
			[[nodiscard]] std::string    string() const noexcept final override;
			[[nodiscard]] int            version() const noexcept final override;

		private:
			mutable std::string string_ = {};
			in_addr             addr_   = {};
	};

// }}}
// {{{ IP v6

	class IPv6
	{
		IPv6() noexcept;
	};

// }}}
// {{{ TCP

	class TCP
	{
		public:
			~TCP() noexcept;

			[[nodiscard]] static TCP* create(IP*&, const uint16_t) noexcept;
			[[nodiscard]] static TCP* create(const IP*, const uint16_t) noexcept;

			[[nodiscard]] const IP& ip() const noexcept;
			[[nodiscard]] int       fd() const noexcept;
			[[nodiscard]] uint16_t  port() const noexcept;

			// --- Client --- //
			[[nodiscard]] bool      connect() noexcept;

		private:
			IP*                ip_;
			const uint16_t     port_;
			const int          type_     = SOCK_STREAM;
			const int          protocol_ = 0;
			struct sockaddr_in addr_     = { 0 };
			int                fd_       = -1;

			TCP(IP*, uint16_t) noexcept;
	};

// }}}
// {{{ TCP Client

	class TCPClient
	{
	};

// }}}
// {{{ TCP Server

	class TCPServer
	{
	};

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

IPv4* IPv4::create(const std::string address
	) noexcept
{
	in_addr addr = { 0 };

	int retval = inet_aton(address.data(), &addr);
	if(retval == 0)
	{
		return nullptr;
	}

	IPv4* ip = new IPv4();
	ip->addr_ = addr;

	return ip;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("ipv4/create)")
{
	IPv4* ip = nullptr;

	SUBCASE("IPv4 Invalid")
	{
		ip = IPv4::create("");
		CHECK(ip == nullptr);
		delete ip;

		ip = IPv4::create("aaa.bbb.ccc.ddd");
		CHECK(ip == nullptr);
		delete ip;

		ip = IPv4::create("333.444.555.666");
		CHECK(ip == nullptr);
		delete ip;
	}

	SUBCASE("IPv4 Valid")
	{
		ip = IPv4::create("0.0.0.0");
		CHECK(ip != nullptr);
		delete ip;

		ip = IPv4::create("127.0.0.1");
		CHECK(ip != nullptr);
		delete ip;

		ip = IPv4::create("127.000.000.001");
		CHECK(ip != nullptr);
		delete ip;

		ip = IPv4::create("255.255.255.255");
		CHECK(ip != nullptr);
		delete ip;
	}
}
#endif // }}}


struct in_addr IPv4::address(
	) const noexcept
{
	return addr_;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("ipv4/address)")
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
	IPv4* ip = new IPv4();
	ip->addr_ = this->addr_;

	return ip;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("ipv4/copy)")
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
TEST_CASE("ipv4/family)")
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
TEST_CASE("ipv4/string)")
{
	IPv4* ip = nullptr;

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
TEST_CASE("ipv4/version)")
{
	IPv4* ip = IPv4::create("127.0.0.1");
	CHECK(ip->version() == 4);
	delete ip;
}
#endif // }}}

// }}}
// {{{ IP v6


// }}}
// {{{ TCP
// {{{ TCP : Constructor / Destructor

TCP::TCP(IP* ip
	, uint16_t port
	) noexcept
	: ip_(ip)
	, port_(port)
{
}


TCP::~TCP(
	) noexcept
{
	if(fd_ >= 0)
	{
		close(fd_);
	}

	delete ip_;
	ip_ = nullptr;
}

// }}}

TCP* TCP::create(IP*& ip
	, const uint16_t port
	) noexcept
{
	if(ip == nullptr)
	{
		return nullptr;
	}

	TCP* tcp = new TCP(ip, port);

	ip = nullptr;

	return tcp;
}


TCP* TCP::create(const IP* ip
	, const uint16_t port
	) noexcept
{
	if(ip == nullptr)
	{
		return nullptr;
	}

	TCP* tcp = new TCP(ip->copy(), port);

	return tcp;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("tcp/create)")
{
	uint16_t port = 65535;

	SUBCASE("Invalid IP")
	{
		TCP* tcp = TCP::create(nullptr, port);
		CHECK(tcp == nullptr);
		delete tcp;
	}

	SUBCASE("Valid IP")
	{
		IPv4* ip = IPv4::create("127.0.0.1");

		TCP* tcp = TCP::create(ip, port);
		CHECK(tcp != nullptr);

		delete tcp;
		delete ip;
	}
}
#endif // }}}


bool TCP::connect(
	) noexcept
{
	if(fd_ < 0)
	{
		// TODO : Add error handling
		fd_ = socket(ip_->family(), type_, protocol_);

		if(fd_ < 0)
		{
			return false;
		}
	}

	addr_.sin_family = ip_->family();
	addr_.sin_port   = htons(port_);
	addr_.sin_addr   = ip_->address();

	// TODO : Add error handling

	int retval = ::connect(fd_, (struct sockaddr*)&addr_, sizeof(addr_));

	if(retval < 0)
	{
		return false;
	}

	return true;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
#if 0 // Can't test, must have something to connect to...
TEST_CASE("tcp/connect)")
{
	SUBCASE("IPv4")
	{
		IPv4*    ip   = IPv4::create("127.0.0.1");
		uint16_t port = 80;

		TCP* tcp = TCP::create(ip, port);
		CHECK(tcp->connect_() == true);

		delete tcp;
		delete ip;
	}
}
#endif
#endif // }}}


int TCP::fd(
	) const noexcept
{
	return fd_;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("tcp/fd)")
{
	IPv4*    ip   = IPv4::create("127.0.0.1");
	uint16_t port = 65535;

	SUBCASE("IPv4 Not Connected")
	{
		TCP* tcp = TCP::create(ip, port);
		CHECK(tcp->fd() == -1);
	}
}
#endif // }}}


const IP& TCP::ip(
	) const noexcept
{
	return *ip_;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("tcp/ip)")
{
	SUBCASE("IPv4")
	{
		IPv4* ip = IPv4::create("127.0.0.1");
		uint16_t port = 65535;

		TCP* tcp = TCP::create(ip, port);
		CHECK(tcp->ip().version() == 4);
	}
}
#endif // }}}


uint16_t TCP::port(
	) const noexcept
{
	return port_;
}

#ifdef ZAKERO_NETWORK_IMPLEMENTATION_TEST // {{{
TEST_CASE("tcp/port)")
{
	uint16_t port = 65535;
	IPv4* ip  = IPv4::create("127.0.0.1");
	TCP*  tcp = TCP::create(ip, port);
	CHECK(tcp->port() == port);

	delete tcp;
	delete ip;
}
#endif // }}}

// }}}
// {{{ TCP Server


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
