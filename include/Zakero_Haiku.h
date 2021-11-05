/******************************************************************************
 * Copyright 2020 Andrew Moore
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef zakero_Haiku_h
#define zakero_Haiku_h

/**
 * \file
 *
 * \brief Zakero Haiku
 *
|     |                        |                        |                        |                       |                       |                           |
|-----|------------------------|------------------------|------------------------|-----------------------|-----------------------|---------------------------|
| API | \refdeps{zakero_haiku} | \reftldr{zakero_haiku} | \refwhat{zakero_haiku} | \refwhy{zakero_haiku} | \refhow{zakero_haiku} | \refversion{zakero_haiku} |
 * 
 *
 * A collection of helper functions that may be useful in your projects.
 *
 * In the future, macros and templates may be added.
 *
 *
 * \pardeps{zakero_haiku}
 * - None
 * \endpardeps
 *
 *
 * \partldr{zakero_haiku}
 * This library provides helper functions to help in creating and debugging 
 * Haiku applications and libraries.
 *
 * To use:
 * 1. Add the implementation to a source code file:
 *    \code
 *    #define ZAKERO_HAIKU_IMPLEMENTATION
 *    #include "Zakero_Haiku.h"
 *    \endcode
 * 2. Add the library to where it is used:
 *    \code
 *    #include "Zakero_Haiku.h"
 *    \endcode
 * \endpartldr
 *
 *
 * \parwhat{zakero_haiku}
 * Seriously, it is just a collection of helper functions.
 * \endparwhat
 *
 *
 * \parwhy{zakero_haiku}
 * ...to make life easier...
 * \endparwhy
 *
 *
 * \parhow{zakero_haiku}
 * __Step 0__
 *
 * Your compiler must support at least the C++20 standard. The location of the 
 * `Zakero_Haiku.h` header files _must_ be in your compiler's include path.
 *
 * __Step 1__
 *
 * The first step is to select which C++ source code file will contain the 
 * _Zakero Haiku_ implementation. Once the location has been determined, 
 * add the following to that file:
 *
 * ~~~
 * #define ZAKERO_HAIKU_IMPLEMENTATION
 * #include "Zakero_Haiku.h"
 * ~~~
 *
 * The macro \ref ZAKERO_HAIKU_IMPLEMENTATION tells the header file to 
 * include the implementation of the Haiku.
 *
 * In all other files that will use the Haiku, they only need to include the 
 * header.
 *
 * ~~~
 * #include "Zakero_Haiku.h"
 * ~~~
 * \endparhow
 *
 * \parversion{zakero_base}
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

#if defined( __HAIKU__ )

/******************************************************************************
 * Includes
 */

// C++
#include <string>

// POSIX

// Haiku
#include <Point.h>


/******************************************************************************
 * Functions
 */

namespace zakero::haiku
{
	// {{{ Utilities

	std::string to_string(const status_t) noexcept;
	std::string to_string(const BPoint&) noexcept;

	// }}} Utilities
} // zakero::haiku

// {{{ Macros
// }}}
// {{{ Implementation

#ifdef ZAKERO_HAIKU_IMPLEMENTATION

// Haiku
#include <support/Errors.h>

namespace zakero::haiku
{
// {{{ Utilities::to_string

/**
 * \brief Convert a status code into a string.
 *
 * Converts the provided status code \p value into a string.
 *
 * \return A string
 */
std::string to_string(const status_t value ///< The value
	) noexcept
{
	switch(value)
	{
		// --- POSIX Error Codes --- //

		//case E2BIG:                               return "E2BIG";         // Use the Haiku variant
		case ECHILD:                              return "ECHILD";
		case EDEADLK:                             return "EDEADLK";
		//case EFBIG:                               return "EFBIG";         // Use the Haiku variant
		case EMLINK:                              return "EMLINK";
		case ENFILE:                              return "ENFILE";
		//case ENODEV:                              return "ENODEV";        // Use the Haiku variant
		case ENOLCK:                              return "ENOLCK";
		case ENOSYS:                              return "ENOSYS";
		case ENOTTY:                              return "ENOTTY";
		case ENXIO:                               return "ENXIO";
		case ESPIPE:                              return "ESPIPE";
		case ESRCH:                               return "ESRCH";
		case EFPOS:                               return "EFPOS";
		case ESIGPARM:                            return "ESIGPARM";
		case EDOM:                                return "EDOM";
		//case ERANGE:                              return "ERANGE";        // Use the Haiku variant
		case EPROTOTYPE:                          return "EPROTOTYPE";
		case EPROTONOSUPPORT:                     return "EPROTONOSUPPORT";
		case EPFNOSUPPORT:                        return "EPFNOSUPPORT";
		case EAFNOSUPPORT:                        return "EAFNOSUPPORT";
		case EADDRINUSE:                          return "EADDRINUSE";
		case EADDRNOTAVAIL:                       return "EADDRNOTAVAIL";
		case ENETDOWN:                            return "ENETDOWN";
		case ENETUNREACH:                         return "ENETUNREACH";
		case ENETRESET:                           return "ENETRESET";
		case ECONNABORTED:                        return "ECONNABORTED";
		case ECONNRESET:                          return "ECONNRESET";
		case EISCONN:                             return "EISCONN";
		case ENOTCONN:                            return "ENOTCONN";
		case ESHUTDOWN:                           return "ESHUTDOWN";
		case ECONNREFUSED:                        return "ECONNREFUSED";
		case EHOSTUNREACH:                        return "EHOSTUNREACH";
		case ENOPROTOOPT:                         return "ENOPROTOOPT";
		case ENOBUFS:                             return "ENOBUFS";
		case EINPROGRESS:                         return "EINPROGRESS";
		case EALREADY:                            return "EALREADY";
		case EILSEQ:                              return "EILSEQ";
		case ENOMSG:                              return "ENOMSG";
		case ESTALE:                              return "ESTALE";
		//case EOVERFLOW:                           return "EOVERFLOW";     // Use the Haiku variant
		case EMSGSIZE:                            return "EMSGSIZE";
		//case EOPNOTSUPP:                          return "EOPNOTSUPP";    // Use the Haiku variant
		case ENOTSOCK:                            return "ENOTSOCK";
		case EHOSTDOWN:                           return "EHOSTDOWN";
		case EBADMSG:                             return "EBADMSG";
		case ECANCELED:                           return "ECANCELED";
		case EDESTADDRREQ:                        return "EDESTADDRREQ";
		case EDQUOT:                              return "EDQUOT";
		case EIDRM:                               return "EIDRM";
		case EMULTIHOP:                           return "EMULTIHOP";
		case ENODATA:                             return "ENODATA";
		case ENOLINK:                             return "ENOLINK";
		case ENOSR:                               return "ENOSR";
		case ENOSTR:                              return "ENOSTR";
		case ENOTSUP:                             return "ENOTSUP";
		case EPROTO:                              return "EPROTO";
		case ETIME:                               return "ETIME";
		case ETXTBSY:                             return "ETXTBSY";
		case ENOATTR:                             return "ENOATTR";
		//case ENOMEM:                              return "ENOMEM";        // Use the Haiku variant
		//case EACCES:                              return "EACCES";        // Use the Haiku variant
		//case EINTR:                               return "EINTR";         // Use the Haiku variant
		//case EIO:                                 return "EIO";           // Use the Haiku variant
		//case EBUSY:                               return "EBUSY";         // Use the Haiku variant
		//case EFAULT:                              return "EFAULT";        // Use the Haiku variant
		//case ETIMEDOUT:                           return "ETIMEDOUT";     // Use the Haiku variant
		//case EAGAIN:                              return "EAGAIN";        // Use the Haiku variant
		//case EWOULDBLOCK:                         return "EWOULDBLOCK";   // Use the Haiku variant
		//case EBADF:                               return "EBADF";         // Use the Haiku variant
		//case EEXIST:                              return "EEXIST";        // Use the Haiku variant
		//case EINVAL:                              return "EINVAL";        // Use the Haiku variant
		//case ENAMETOOLONG:                        return "ENAMETOOLONG";  // Use the Haiku variant
		//case ENOENT:                              return "ENOENT";        // Use the Haiku variant
		//case EPERM:                               return "EPERM";         // Use the Haiku variant
		//case ENOTDIR:                             return "ENOTDIR";       // Use the Haiku variant
		//case EISDIR:                              return "EISDIR";        // Use the Haiku variant
		//case ENOTEMPTY:                           return "ENOTEMPTY";     // Use the Haiku variant
		//case ENOSPC:                              return "ENOSPC";        // Use the Haiku variant
		//case EROFS:                               return "EROFS";         // Use the Haiku variant
		//case EMFILE:                              return "EMFILE";        // Use the Haiku variant
		//case EXDEV:                               return "EXDEV";         // Use the Haiku variant
		//case ELOOP:                               return "ELOOP";         // Use the Haiku variant
		//case ENOEXEC:                             return "ENOEXEC";       // Use the Haiku variant
		//case EPIPE:                               return "EPIPE";         // Use the Haiku variant

		// --- Haiku Error Codes --- //

		case B_ALREADY_RUNNING:                   return "B_ALREADY_RUNNING";
		case B_AMBIGUOUS_APP_LAUNCH:              return "B_AMBIGUOUS_APP_LAUNCH";
		case B_BAD_ADDRESS:                       return "B_BAD_ADDRESS";
		case B_BAD_DATA:                          return "B_BAD_DATA";
		case B_BAD_HANDLER:                       return "B_BAD_HANDLER";
		case B_BAD_IMAGE_ID:                      return "B_BAD_IMAGE_ID";
		case B_BAD_INDEX:                         return "B_BAD_INDEX";
		case B_BAD_MIME_SNIFFER_RULE:             return "B_BAD_MIME_SNIFFER_RULE";
		case B_BAD_PORT_ID:                       return "B_BAD_PORT_ID";
		case B_BAD_REPLY:                         return "B_BAD_REPLY";
		case B_BAD_SCRIPT_SYNTAX:                 return "B_BAD_SCRIPT_SYNTAX";
		case B_BAD_SEM_ID:                        return "B_BAD_SEM_ID";
		case B_BAD_SUBSCRIBER:                    return "B_BAD_SUBSCRIBER";
		case B_BAD_TEAM_ID:                       return "B_BAD_TEAM_ID";
		case B_BAD_THREAD_ID:                     return "B_BAD_THREAD_ID";
		case B_BAD_THREAD_STATE:                  return "B_BAD_THREAD_STATE";
		case B_BAD_TYPE:                          return "B_BAD_TYPE";
		case B_BAD_VALUE:                         return "B_BAD_VALUE";
		case B_BUFFER_NOT_AVAILABLE:              return "B_BUFFER_NOT_AVAILABLE";
		case B_BUFFER_OVERFLOW:                   return "B_BUFFER_OVERFLOW";
		case B_BUSTED_PIPE:                       return "B_BUSTED_PIPE";
		case B_BUSY:                              return "B_BUSY";
		case B_CANCELED:                          return "B_CANCELED";
		case B_CROSS_DEVICE_LINK:                 return "B_CROSS_DEVICE_LINK";
		case B_DEBUGGER_ALREADY_INSTALLED:        return "B_DEBUGGER_ALREADY_INSTALLED";
		case B_DEVICE_FULL:                       return "B_DEVICE_FULL";
		case B_DEVICE_NOT_FOUND:                  return "B_DEVICE_NOT_FOUND";
		case B_DEV_BAD_DRIVE_NUM:                 return "B_DEV_BAD_DRIVE_NUM";
		case B_DEV_BAD_PID:                       return "B_DEV_BAD_PID";
		case B_DEV_CONFIGURATION_ERROR:           return "B_DEV_CONFIGURATION_ERROR";
		case B_DEV_CRC_ERROR:                     return "B_DEV_CRC_ERROR";
		case B_DEV_DATA_OVERRUN:                  return "B_DEV_DATA_OVERRUN";
		case B_DEV_DATA_UNDERRUN:                 return "B_DEV_DATA_UNDERRUN";
		case B_DEV_DISABLED_BY_USER:              return "B_DEV_DISABLED_BY_USER";
		case B_DEV_DOOR_OPEN:                     return "B_DEV_DOOR_OPEN";
		case B_DEV_FIFO_OVERRUN:                  return "B_DEV_FIFO_OVERRUN";
		case B_DEV_FIFO_UNDERRUN:                 return "B_DEV_FIFO_UNDERRUN";
		case B_DEV_FORMAT_ERROR:                  return "B_DEV_FORMAT_ERROR";
		case B_DEV_ID_ERROR:                      return "B_DEV_ID_ERROR";
		case B_DEV_INVALID_IOCTL:                 return "B_DEV_INVALID_IOCTL";
		case B_DEV_INVALID_PIPE:                  return "B_DEV_INVALID_PIPE";
		case B_DEV_MEDIA_CHANGED:                 return "B_DEV_MEDIA_CHANGED";
		case B_DEV_MEDIA_CHANGE_REQUESTED:        return "B_DEV_MEDIA_CHANGE_REQUESTED";
		case B_DEV_MULTIPLE_ERRORS:               return "B_DEV_MULTIPLE_ERRORS";
		case B_DEV_NOT_READY:                     return "B_DEV_NOT_READY";
		case B_DEV_NO_MEDIA:                      return "B_DEV_NO_MEDIA";
		case B_DEV_NO_MEMORY:                     return "B_DEV_NO_MEMORY";
		case B_DEV_PENDING:                       return "B_DEV_PENDING";
		case B_DEV_READ_ERROR:                    return "B_DEV_READ_ERROR";
		case B_DEV_RECALIBRATE_ERROR:             return "B_DEV_RECALIBRATE_ERROR";
		case B_DEV_RESOURCE_CONFLICT:             return "B_DEV_RESOURCE_CONFLICT";
		case B_DEV_SEEK_ERROR:                    return "B_DEV_SEEK_ERROR";
		case B_DEV_STALLED:                       return "B_DEV_STALLED";
		case B_DEV_TIMEOUT:                       return "B_DEV_TIMEOUT";
		case B_DEV_TOO_LATE:                      return "B_DEV_TOO_LATE";
		case B_DEV_UNEXPECTED_PID:                return "B_DEV_UNEXPECTED_PID";
		case B_DEV_UNREADABLE:                    return "B_DEV_UNREADABLE";
		case B_DEV_WRITE_ERROR:                   return "B_DEV_WRITE_ERROR";
		case B_DIRECTORY_NOT_EMPTY:               return "B_DIRECTORY_NOT_EMPTY";
		case B_DONT_DO_THAT:                      return "B_DONT_DO_THAT";
		case B_DUPLICATE_REPLY:                   return "B_DUPLICATE_REPLY";
		case B_ENTRY_NOT_FOUND:                   return "B_ENTRY_NOT_FOUND";
		case B_ERROR:                             return "B_ERROR";
		case B_FILE_ERROR:                        return "B_FILE_ERROR";
		case B_FILE_EXISTS:                       return "B_FILE_EXISTS";
		//case B_FILE_NOT_FOUND:                    return "B_FILE_NOT_FOUND"; // Deprecated
		case B_FILE_TOO_LARGE:                    return "B_FILE_TOO_LARGE";
		case B_ILLEGAL_DATA:                      return "B_ILLEGAL_DATA";
		case B_INTERRUPTED:                       return "B_INTERRUPTED";
		case B_IO_ERROR:                          return "B_IO_ERROR";
		case B_IS_A_DIRECTORY:                    return "B_IS_A_DIRECTORY";
		case B_LAST_BUFFER_ERROR:                 return "B_LAST_BUFFER_ERROR";
		case B_LAUNCH_FAILED:                     return "B_LAUNCH_FAILED";
		case B_LAUNCH_FAILED_APP_IN_TRASH:        return "B_LAUNCH_FAILED_APP_IN_TRASH";
		case B_LAUNCH_FAILED_APP_NOT_FOUND:       return "B_LAUNCH_FAILED_APP_NOT_FOUND";
		case B_LAUNCH_FAILED_EXECUTABLE:          return "B_LAUNCH_FAILED_EXECUTABLE";
		case B_LAUNCH_FAILED_FILES_APP_NOT_FOUND: return "B_LAUNCH_FAILED_FILES_APP_NOT_FOUND";
		case B_LAUNCH_FAILED_NO_PREFERRED_APP:    return "B_LAUNCH_FAILED_NO_PREFERRED_APP";
		case B_LAUNCH_FAILED_NO_RESOLVE_LINK:     return "B_LAUNCH_FAILED_NO_RESOLVE_LINK";
		case B_LEGACY_EXECUTABLE:                 return "B_LEGACY_EXECUTABLE";
		case B_LINK_LIMIT:                        return "B_LINK_LIMIT";
		case B_MAIL_ACCESS_ERROR:                 return "B_MAIL_ACCESS_ERROR";
		case B_MAIL_INVALID_MAIL:                 return "B_MAIL_INVALID_MAIL";
		case B_MAIL_NO_DAEMON:                    return "B_MAIL_NO_DAEMON";
		case B_MAIL_NO_RECIPIENT:                 return "B_MAIL_NO_RECIPIENT";
		case B_MAIL_UNKNOWN_FIELD:                return "B_MAIL_UNKNOWN_FIELD";
		case B_MAIL_UNKNOWN_HOST:                 return "B_MAIL_UNKNOWN_HOST";
		case B_MAIL_UNKNOWN_USER:                 return "B_MAIL_UNKNOWN_USER";
		case B_MAIL_WRONG_PASSWORD:               return "B_MAIL_WRONG_PASSWORD";
		case B_MEDIA_ADDON_DISABLED:              return "B_MEDIA_ADDON_DISABLED";
		case B_MEDIA_ADDON_FAILED:                return "B_MEDIA_ADDON_FAILED";
		case B_MEDIA_ADDON_RESTRICTED:            return "B_MEDIA_ADDON_RESTRICTED";
		case B_MEDIA_ALREADY_CONNECTED:           return "B_MEDIA_ALREADY_CONNECTED";
		case B_MEDIA_APP_ALREADY_REGISTERED:      return "B_MEDIA_APP_ALREADY_REGISTERED";
		case B_MEDIA_APP_NOT_REGISTERED:          return "B_MEDIA_APP_NOT_REGISTERED";
		case B_MEDIA_BAD_BUFFER:                  return "B_MEDIA_BAD_BUFFER";
		case B_MEDIA_BAD_CLIP_FORMAT:             return "B_MEDIA_BAD_CLIP_FORMAT";
		case B_MEDIA_BAD_DESTINATION:             return "B_MEDIA_BAD_DESTINATION";
		case B_MEDIA_BAD_FORMAT:                  return "B_MEDIA_BAD_FORMAT";
		case B_MEDIA_BAD_NODE:                    return "B_MEDIA_BAD_NODE";
		case B_MEDIA_BAD_SOURCE:                  return "B_MEDIA_BAD_SOURCE";
		case B_MEDIA_BUFFERS_NOT_RECLAIMED:       return "B_MEDIA_BUFFERS_NOT_RECLAIMED";
		case B_MEDIA_BUFFER_ALREADY_EXISTS:       return "B_MEDIA_BUFFER_ALREADY_EXISTS";
		case B_MEDIA_CANNOT_CHANGE_RUN_MODE:      return "B_MEDIA_CANNOT_CHANGE_RUN_MODE";
		case B_MEDIA_CANNOT_RECLAIM_BUFFERS:      return "B_MEDIA_CANNOT_RECLAIM_BUFFERS";
		case B_MEDIA_CANNOT_SEEK:                 return "B_MEDIA_CANNOT_SEEK";
		case B_MEDIA_CHANGE_IN_PROGRESS:          return "B_MEDIA_CHANGE_IN_PROGRESS";
		case B_MEDIA_DUPLICATE_FORMAT:            return "B_MEDIA_DUPLICATE_FORMAT";
		case B_MEDIA_NODE_ALREADY_EXISTS:         return "B_MEDIA_NODE_ALREADY_EXISTS";
		case B_MEDIA_NODE_BUSY:                   return "B_MEDIA_NODE_BUSY";
		case B_MEDIA_NOT_CONNECTED:               return "B_MEDIA_NOT_CONNECTED";
		case B_MEDIA_NO_HANDLER:                  return "B_MEDIA_NO_HANDLER";
		case B_MEDIA_REALTIME_DISABLED:           return "B_MEDIA_REALTIME_DISABLED";
		case B_MEDIA_REALTIME_UNAVAILABLE:        return "B_MEDIA_REALTIME_UNAVAILABLE";
		case B_MEDIA_STALE_CHANGE_COUNT:          return "B_MEDIA_STALE_CHANGE_COUNT";
		case B_MEDIA_SYSTEM_FAILURE:              return "B_MEDIA_SYSTEM_FAILURE";
		case B_MEDIA_TIME_SOURCE_BUSY:            return "B_MEDIA_TIME_SOURCE_BUSY";
		case B_MEDIA_TIME_SOURCE_STOPPED:         return "B_MEDIA_TIME_SOURCE_STOPPED";
		case B_MEDIA_TOO_MANY_BUFFERS:            return "B_MEDIA_TOO_MANY_BUFFERS";
		case B_MEDIA_TOO_MANY_NODES:              return "B_MEDIA_TOO_MANY_NODES";
		case B_MESSAGE_TO_SELF:                   return "B_MESSAGE_TO_SELF";
		case B_MISMATCHED_VALUES:                 return "B_MISMATCHED_VALUES";
		case B_MISSING_LIBRARY:                   return "B_MISSING_LIBRARY";
		case B_MISSING_SYMBOL:                    return "B_MISSING_SYMBOL";
		case B_NAME_IN_USE:                       return "B_NAME_IN_USE";
		case B_NAME_NOT_FOUND:                    return "B_NAME_NOT_FOUND";
		case B_NAME_TOO_LONG:                     return "B_NAME_TOO_LONG";
		case B_NOT_ALLOWED:                       return "B_NOT_ALLOWED";
		case B_NOT_AN_EXECUTABLE:                 return "B_NOT_AN_EXECUTABLE";
		case B_NOT_A_DIRECTORY:                   return "B_NOT_A_DIRECTORY";
		case B_NOT_A_MESSAGE:                     return "B_NOT_A_MESSAGE";
		case B_NOT_INITIALIZED:                   return "B_NOT_INITIALIZED";
		case B_NOT_SUPPORTED:                     return "B_NOT_SUPPORTED";
		case B_NO_MEMORY:                         return "B_NO_MEMORY";
		case B_NO_MORE_FDS:                       return "B_NO_MORE_FDS";
		case B_NO_MORE_PORTS:                     return "B_NO_MORE_PORTS";
		case B_NO_MORE_SEMS:                      return "B_NO_MORE_SEMS";
		case B_NO_MORE_TEAMS:                     return "B_NO_MORE_TEAMS";
		case B_NO_MORE_THREADS:                   return "B_NO_MORE_THREADS";
		case B_NO_PRINT_SERVER:                   return "B_NO_PRINT_SERVER";
		case B_NO_TRANSLATOR:                     return "B_NO_TRANSLATOR";
		case B_OK:                                return "B_OK";
		case B_PARTIAL_READ:                      return "B_PARTIAL_READ";
		case B_PARTIAL_WRITE:                     return "B_PARTIAL_WRITE";
		case B_PARTITION_TOO_SMALL:               return "B_PARTITION_TOO_SMALL";
		case B_PERMISSION_DENIED:                 return "B_PERMISSION_DENIED";
		case B_POSIX_ENOMEM:                      return "B_POSIX_ENOMEM";
		case B_READ_ONLY_DEVICE:                  return "B_READ_ONLY_DEVICE";
		case B_RESOURCE_NOT_FOUND:                return "B_RESOURCE_NOT_FOUND";
		case B_RESOURCE_UNAVAILABLE:              return "B_RESOURCE_UNAVAILABLE";
		case B_RESULT_NOT_REPRESENTABLE:          return "B_RESULT_NOT_REPRESENTABLE";
		case B_SERVER_NOT_FOUND:                  return "B_SERVER_NOT_FOUND";
		case B_SHUTDOWN_CANCELLED:                return "B_SHUTDOWN_CANCELLED";
		case B_SHUTTING_DOWN:                     return "B_SHUTTING_DOWN";
		case B_STREAM_NOT_FOUND:                  return "B_STREAM_NOT_FOUND";
		case B_SUBSCRIBER_NOT_ENTERED:            return "B_SUBSCRIBER_NOT_ENTERED";
		case B_TIMED_OUT:                         return "B_TIMED_OUT";
		case B_TOO_MANY_ARGS:                     return "B_TOO_MANY_ARGS";
		case B_TRANSLATION_BASE_ERROR:            return "B_TRANSLATION_BASE_ERROR";
		case B_UNKNOWN_EXECUTABLE:                return "B_UNKNOWN_EXECUTABLE";
		case B_UNKNOWN_MIME_TYPE:                 return "B_UNKNOWN_MIME_TYPE";
		case B_UNSUPPORTED:                       return "B_UNSUPPORTED";
		case B_WOULD_BLOCK:                       return "B_WOULD_BLOCK";
	}

	return {};
}


/**
 * \brief Convert a Point into a string.
 *
 * \return A string.
 */
std::string to_string(const BPoint& point ///< The point to convert
	) noexcept
{
	std::string str = std::string()
		+ "{ 'x': " + std::to_string(point.x)
		+ ", 'y': " + std::to_string(point.y)
		+ " }";
	
	return str;
}

// }}} Utilities::to_string
}

#endif // ZAKERO_HAIKU_IMPLEMENTATION

// }}}

#endif // defined( __HAIKU__ )
#endif // zakero_Haiku_h
