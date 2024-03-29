/******************************************************************************
 * Copyright 2020-2022 Andrew Moore
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef zakero_Profiler_h
#define zakero_Profiler_h

/**
 * \file
 *
 * \brief Zakero Profiler
 *
|                        |                           |                           |                           |                          |                          |                              |
|------------------------|---------------------------|---------------------------|---------------------------|--------------------------|--------------------------|------------------------------|
| \api{zakero::Profiler} | \refdeps{zakero_profiler} | \reftldr{zakero_profiler} | \refwhat{zakero_profiler} | \refwhy{zakero_profiler} | \refhow{zakero_profiler} | \refversion{zakero_profiler} |
 * 
 * An invasive profiler to provide timing data output.
 * 
 * \note "Invasive" means that you must add profiler code to your source code.
 *  
 *
 * \pardeps{zakero_profiler}
 * - Zakero_Base.h
 * \endpardeps
 *
 *
 * \partldr{zakero_profiler}
 * This library will generate profiling data while your application is running.  
 * To use, add the implementation to a source code file:
 * ~~~
 * #define ZAKERO_PROFILER_IMPLEMENTATION
 * #include "Zakero_Profiler.h"
 * ~~~
 *
 * Next, add the library to the source code where it is "always enabled"
 * ~~~
 * #define ZAKERO_PROFILER_ENABLE
 * #include "Zakero_Profiler.h"
 * ~~~
 *
 * Or. enable the profiler at compile time.  Only include the library in the 
 * source code:
 * ~~~
 * #include "Zakero_Profiler.h"
 * ~~~
 * Then use a compiler flag when building:
 * ~~~
 * -DZAKERO_PROFILER_ENABLE
 * ~~~
 *
 * Finally, add the profiler macros to your code.
 * \endpartldr
 *
 *
 * \parwhat{zakero_profiler}
 * Profiling a program helps you determine how much time is being used by 
 * different sections of code.  This information can be used to determine where 
 * code optimizations will have the most pay-off or help locate an area that 
 * does not perform consistently.
 * 
 * __If your program is crashing or has some other catastrophic bug, profiling 
 * will not help you.__
 * 
 * The _Zakero Profiler_ is an "internal" profiler that can be added to your 
 * C++ code.   An "internal" profiler has a few advantages over "external" 
 * profilers, such as "gprof".  These advantages include:
 * - You have full control of what is profiled: Add profiling where it matters.
 * - Profiling as it happens, instead of runtime snapshots which can miss 
 * details.
 * - This implementation has Zero-Cost if not enabled.
 * 
 * The disadvantage of "internal" profilers is code-clutter.  You must litter 
 * your code with profiler stuff which can make code harder to read.  This 
 * implementation is no different, but it does try to keep the mess to a 
 * minimum by using a few macros.
 * \endparwhat
 *
 *
 * \parwhy{zakero_profiler}
 * The first benefit is the generated profiler output is JSON formatted.  There 
 * are many JSON parsers available allowing you to parse and use the data 
 * easily.  Which leads into the main benefit...
 *
 * The main benefit of using the _Zakero Profiler_ is that you can visualize 
 * the profiler data in Google Chrome and Chromium.  No need to write your own 
 * visualization code.
 *
 * And the final reason to use the _Zakero Profiler_ is that it is a "Single 
 * Header Library".  Just include the header file where ever you need or want 
 * to add profiling.
 * \endparwhy
 *
 *
 * \parhow{zakero_profiler}
 * __Step 0__
 *
 * Your compiler must support at least the C++20 standard.  The location of the 
 * `Zakero_*.h` header files _must_ be in your compiler's include path.
 *
 * __Step 1__
 *
 * The first step is to select which C++ source code file will contain the 
 * _Zakero Profiler_ implementation.  Once the location has been determined, 
 * add the following to that file:
 *
 * ~~~
 * #define ZAKERO_PROFILER_IMPLEMENTATION
 * #include "Zakero_Profiler.h"
 * ~~~
 *
 * The macro \ref ZAKERO_PROFILER_IMPLEMENTATION tells the header file to 
 * include the implementation of the profiler.
 *
 * In all other files that will use the _Zakero Profiler_, they need to include 
 * the header.  In addition to including the header, the profiler must be 
 * enabled by defining the \ref ZAKERO_PROFILER_ENABLE macro.
 *
 * The easiest way to do this is to always have the _Zakero_Profiler_ enabled:
 *
 * ~~~
 * #define ZAKERO_PROFILER_ENABLE
 * #include "Zakero_Profiler.h"
 * ~~~
 *
 * This, however, is not very realistic.  A better solution is to configure 
 * your build system to define \ref ZAKERO_PROFILER_ENABLE when needed and add 
 * the _Zakero Profiler_ header and macros where desired.
 *
 * ~~~
 * #include "Zakero_Profiler.h"
 * ~~~
 *
 * And if your "build system" is the command-line, the following will turn on 
 * profiling:
 *
 * ~~~
 * > g++ ... -DZAKERO_PROFILER_ENABLE ...
 * ~~~
 *
 * If the \ref ZAKERO_PROFILER_ENABLE macro is __not__ defined, all the other 
 * marcos in this library will be disabled.  With compiler optimizations turned 
 * on, -O2, the unused profiler code should be removed from the resulting 
 * executable.
 *
 * __Step 2__
 *
 * Next, is to initialize the _Zakero Profiler_.  Somewhere in your code, 
 * before any profiling is done (such as in _main()_) add the initializer macro 
 * ZAKERO_PROFILER_INIT().
 *
 * The following example will have the _Zakero Profiler_ write to a file and 
 * include information about the application.
 *
 * ~~~
 * ZAKERO_PROFILER_INIT("MyApp.profiler_json",
 * {	{ "application", "MyApp"                      }
 * ,	{ "version",     "1.0.0"                      }
 * ,	{ "intent",      "=Locate Allocation Delays=" }
 * });
 * // --- Or --- //
 * std::filesystem::path myapp_profile = logging_path / "issue_42.profile";
 * ZAKERO_PROFILER_INIT(myapp_profile,
 * {	{ "application", "MyApp"                      }
 * ,	{ "version",     "1.0.0"                      }
 * ,	{ "intent",      "=Locate Allocation Delays=" }
 * });
 * ~~~
 *
 * __Step 3__
 *
 * All the prep work is done, now it is time to generate some profiling data.  
 * To do this use one of these macros:
 * - ZAKERO_PROFILER_DURATION()
 * - ZAKERO_PROFILER_INSTANT()
 *
 * The ZAKERO_PROFILER_DURATION() macro is best used at the start of a code 
 * block.  Remember you do not have to put this macro after every '{', only add 
 * the macros where you need to capture data.
 *
 * For example:
 *
 * ~~~
 * void func()
 * {
 * 	ZAKERO_PROFILER_DURATION("cache availability", "linear defrag")
 *
 * 	do_stuff();
 *
 * 	if(!done)
 * 	{
 * 		ZAKERO_PROFILER_DURATION("cache availability", "mo-defrag-in")
 *
 * 		do_more_stuff();
 * 	}
 * }
 * ~~~
 *
 * __Step 4__
 *
 * The program ran and you have profile data.  It's time to look at it.  Fire 
 * up one of the following browsers and...
 *
 * _Google Chrome_
 *
 * > Open a new tab and enter __chrome://tracing__ for the URL.
 *
 * _Chromium_
 *
 * > Open a new tab enter __about:tracing__ for the URL.
 *
 * Next, drag the _Zakero Profiler_ output file into the tab to see your data.
 * \endparhow
 *
 *
 * \parversion{zakero_profiler}
 * __0.9.1__
 * - Improved ZAKERO_PROFILER_INIT() so that the MetaData is now optional.
 * - Improved ZAKERO_PROFILER_INIT() to use std::filesystem instead of strings.
 * - Removed Macro: ZAKERO_PROFILER_INIT_METADATA()
 * - Added ZAKERO_PROFILER_COMPLETE()
 * - Added MetaData support to ZAKERO_PROFILER_DURATION()
 * - Added MetaData support to ZAKERO_PROFILER_INSTANT()
 * - Added error support
 *
 * __0.9.0__
 * - Bug Fix: Use a mutex to prevent multiple threads from writing at the same 
 * time
 * - Bug Fix: Don't write to a _null_ stream in the destructor
 * 
 * __0.8.0__
 * - Updated to use Zakero_Base.h
 * 
 * __0.7.0__
 * - Bug fixes
 * 
 * __0.6.0__
 * - Bug fixes
 * - Macro name changes
 * 
 * __0.5.0__
 * - Profile time duration in C++ code blocks
 * - Add "instant" markers to the timeline
 * \endparversion
 *
 *
 * \copyright [Mozilla Public License 
 * v2](https://www.mozilla.org/en-US/MPL/2.0/) 
 *
 *
 * \author Andrew "Zakero" Moore
 * 	- Original Author
 */


/******************************************************************************
 * Includes
 */

#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>

// C++20
#include <experimental/source_location>

// Zakero
#include "Zakero_Base.h"


/******************************************************************************
 * Macros
 */

// {{{ Macros

#ifdef ZAKERO_PROFILER_ENABLE

/**
 * \brief Initialize the profiler.
 *
 * The profiler __must__ be initialized before any of the other macros are 
 * used.
 *
 * The \p output_ can be a file name or a _std::ostream_ object.  If a 
 * _std::ostream_ object is used, the profiler will use a pointer to that 
 * stream.  This means the provided \p output_ stream must remain valid for the 
 * life of the profiler.
 *
 * However, if a file name is given, then the profiler will maintain the 
 * life-span of the file.  Behavior is undefined (i.e. expect a crash) if the 
 * provided file name is not writable.
 *
 * \parcode
 * ZAKERO_PROFILER_INIT(std::clog);
 * // --- or --- //
 * ZAKERO_PROFILER_INIT("profile.log");
 * \endparcode
 *
 * Any extra data that should appear in the profiler output can be added using 
 * \p meta_data_.  The \p meta_data_ is a map of _%std::string/%std::string_ 
 * key/value pairs.  The "date" key will be automatically added if not already 
 * present.  The following keys will be over written if used:
 * - displayTimeUnit
 * - traceEvents
 *
 * \parcode
 * zakero::Profiler::MetaData meta_data =
 * {	{ "application", "MyApp" }
 * ,	{ "version",     "1.0.0" }
 * };
 * ZAKERO_PROFILER_INIT(std::clog, meta_data);
 * // --- or --- //
 * ZAKERO_PROFILER_INIT("profile.log",
 * {	{ "application", "MyApp" }
 * ,	{ "version",     "1.0.0" }
 * });
 * \endparcode
 *
 * \note The _Zakero Profiler_ does not flush after writing data to the output 
 * using the stream operator (<<).  The reason for this is to allow the 
 * operating system to decide the best time to write the data, reducing the 
 * number of I/O requests the profiler makes.  As a result of this, profiling 
 * data will be incomplete if the application does not cleanly exit.
 *
 * \param output_ Where to stream the profile data.
 * \param meta_data_ Extra data
 */
#define ZAKERO_PROFILER_INIT(output_, meta_data_...) \
	zakero::Profiler::init(output_, ##meta_data_);

/**
 * \brief Activate Profiling
 *
 * This macro will reactivate the generation of profiling data.
 *
 * The use of ZAKERO_PROFILER_ACTIVATE and ZAKERO_PROFILER_DEACTIVATE macros 
 * allows the control of _when_ profiling happens.  Instead of having profiling 
 * data being generated for the entire runtime of an application, an event can 
 * be used to call these macros so that only the profiling data of the area of 
 * interest is generated.
 *
 * If _Zakero Profiler_ has been enabled at compile-time, by defining the 
 * ZAKERO_PROFILER_ENABLE macro, then profiling is automatically active.  To 
 * have the profiler manually/programmatically activated, call 
 * ZAKERO_PROFILER_DEACTIVATE immediately after initialization.
 *
 * \parcode
 * int main()
 * {
 * 	ZAKERO_PROFILER_INIT("profiler.json")
 * 	ZAKERO_PROFILER_DEACTIVATE
 *
 * 	eventLoop();
 *
 * 	return 0;
 * }
 *
 * void onEsc()
 * {
 * 	stopEventLoop();
 * }
 *
 * void onF12()
 * {
 * 	ZAKERO_PROFILER_ACTIVATE
 * }
 *
 * void onShiftF12()
 * {
 * 	ZAKERO_PROFILER_DEACTIVATE
 * }
 * \endparcode
 *
 * \see ZAKERO_PROFILER_DEACTIVATE
 */
#define ZAKERO_PROFILER_ACTIVATE \
	zakero::Profiler::activate();

/**
 * \brief Deactivate Profiling
 *
 * The generation of profiling data can be temporarily stopped with this macro.
 *
 * \see ZAKERO_PROFILER_ACTIVATE
 */
#define ZAKERO_PROFILER_DEACTIVATE \
	zakero::Profiler::deactivate();

/**
 * \brief Generate profiler data.
 *
 * This macro will generate a "complete" profiler event, meaning that the time 
 * when the macro was executed is recorded and the time when current code block 
 * goes out-of-scope is also recorded.  The ZAKERO_PROFILER_DURATION() macro 
 * provides the same information, however this event generates less data 
 * resulting in smaller output files.
 *
 * The \p category_ can be used to group together related \p name_ data.  For 
 * example, profiling a C++ class could use the class name for the category and 
 * the \p name_ would be the part of the class being profiled.
 *
 * The \p meta_data_ arg is optional.  The \p meta_data_ is a map of 
 * _%std::string/%std::string_ key/value pairs.  Any data placed in the \p 
 * meta_data_ map will appear as "args" when being viewed.  The following keys 
 * will be over written if used:
 * - file_name
 * - function_name
 * 
 * \note The file name and function name are automatically recorded and stored 
 * in the MetaData.
 *
 * \parcode
 * void func()
 * {
 *     ZAKERO_PROFILER_COMPLETE("busy", "doing stuff")
 *     
 *     // Doing stuff
 *     
 *     for(int i = 0; i < max_i; i++)
 *     {
 *         ZAKERO_PROFILER_COMPLETE("busy", "gone loopy",
 *         {    { "i", std::to_string(i) }
 *         })
 *         
 *         // Doing more stuff
 *         
 *         if(i % magic)
 *         {
 *             ZAKERO_PROFILER_INSTANT("busy", "shhh, it's magic")
 * 
 *             // Doing magic stuff
 *         }
 *     }
 * }
 * \endparcode
 *
 * \param category_  The category of the data
 * \param name_      The name of the data
 * \param meta_data_ Extra data
 */
#define ZAKERO_PROFILER_COMPLETE(category_, name_, meta_data_...)            \
	zakero::Profiler::Complete ZAKERO_CONCAT(zakero_profiler_, __LINE__) \
	( category_                                                          \
	, name_                                                              \
	, std::experimental::source_location::current()                      \
	, ##meta_data_                                                       \
	);                                                                   \


/**
 * \brief Generate profiler data.
 *
 * This macro will generate a "duration" profiler event, meaning that the time 
 * when the macro was executed is recorded and the time when current code block 
 * goes out-of-scope is also recorded.
 *
 * The \p category_ can be used to group together related \p name_ data.  For 
 * example, profiling a C++ class could use the class name for the category and 
 * the \p name_ would be the part of the class being profiled.
 *
 * The \p meta_data_ arg is optional.  The \p meta_data_ is a map of 
 * _%std::string/%std::string_ key/value pairs.  Any data placed in the \p 
 * meta_data_ map will appear as "args" when being viewed.  The following keys 
 * will be over written if used:
 * - file_name
 * - function_name
 * 
 * \note The file name and function name are automatically recorded and stored 
 * in the MetaData.
 *
 * \parcode
 * void func()
 * {
 *     ZAKERO_PROFILER_DURATION("busy", "doing stuff")
 *     
 *     // Doing stuff
 *     
 *     for(int i = 0; i < max_i; i++)
 *     {
 *         ZAKERO_PROFILER_DURATION("busy", "gone loopy",
 *         {    { "i", std::to_string(i) }
 *         })
 *         
 *         // Doing more stuff
 *         
 *         if(i % magic)
 *         {
 *             ZAKERO_PROFILER_INSTANT("busy", "shhh, it's magic")
 * 
 *             // Doing magic stuff
 *         }
 *     }
 * }
 * \endparcode
 *
 * \param category_  The category of the data
 * \param name_      The name of the data
 * \param meta_data_ Extra data
 */
#define ZAKERO_PROFILER_DURATION(category_, name_, meta_data_...)            \
	zakero::Profiler::Duration ZAKERO_CONCAT(zakero_profiler_, __LINE__) \
	( category_                                                          \
	, name_                                                              \
	, std::experimental::source_location::current()                      \
	, ##meta_data_                                                       \
	);                                                                   \

/**
 * \brief Generate profiler data.
 *
 * This macro will generate an "instant" event.  These events are useful to 
 * mark something in the timeline.
 *
 * The \p category_ and \p name_ serve the same function as in 
 * ZAKERO_PROFILER_DURATION().
 *
 * The \p meta_data_ arg is optional.  The \p meta_data_ is a map of 
 * _%std::string/%std::string_ key/value pairs.  Any data placed in the \p 
 * meta_data_ map will appear as "args" when being viewed.  The following keys 
 * will be over written if used:
 * - file_name
 * - function_name
 * 
 * \param category_  The category of the data
 * \param name_      The name of the data
 * \param meta_data_ Extra data
 */
#define ZAKERO_PROFILER_INSTANT(category_, name_, meta_data_...)            \
{                                                                           \
	zakero::Profiler::Instant ZAKERO_CONCAT(zakero_profiler_, __LINE__) \
	( category_                                                         \
	, name_                                                             \
	, std::experimental::source_location::current()                     \
	, ##meta_data_                                                      \
	);                                                                  \
}                                                                           \

#else

#define ZAKERO_PROFILER_INIT(output_, meta_data_...)
#define ZAKERO_PROFILER_ACTIVATE
#define ZAKERO_PROFILER_DEACTIVATE
#define ZAKERO_PROFILER_COMPLETE(category_, name_, meta_data_...)
#define ZAKERO_PROFILER_DURATION(category_, name_, meta_data_...)
#define ZAKERO_PROFILER_INSTANT(category_, name_, meta_data_...)

#endif

// }}}

namespace zakero
{
	// {{{ Declaration

	class Profiler
	{
		public:
			using MetaData = std::map<std::string, std::string>;

			struct Data
			{
				MetaData                           meta_data;
				std::string                        category;
				std::string                        name;
				std::experimental::source_location location;
				std::chrono::microseconds::rep     duration;
				std::chrono::microseconds::rep     time_stamp;
				std::thread::id                    thread_id;
				pid_t                              process_id;
				char                               phase;

				Data(const char, const std::string&, const std::string&, const std::experimental::source_location&, zakero::Profiler::MetaData = {}) noexcept;
			};

			struct Complete
				: public zakero::Profiler::Data
			{
				bool was_active;

				Complete(const std::string&, const std::string&, const std::experimental::source_location&, zakero::Profiler::MetaData = {}) noexcept;
				~Complete() noexcept;
			};

			struct Duration
				: public zakero::Profiler::Data
			{
				bool was_active;

				Duration(const std::string&, const std::string&, const std::experimental::source_location&, zakero::Profiler::MetaData = {}) noexcept;
				~Duration() noexcept;
			};

			struct Instant
				: public zakero::Profiler::Data
			{
				Instant(const std::string&, const std::string&, const std::experimental::source_location&, zakero::Profiler::MetaData = {}) noexcept;
			};

			Profiler() noexcept;
			~Profiler() noexcept;

			static std::error_code init(std::ostream&, zakero::Profiler::MetaData = {}) noexcept;
			static std::error_code init(const std::filesystem::path, zakero::Profiler::MetaData = {}) noexcept;

			static void activate() noexcept;
			static void deactivate() noexcept;

			static void report(const zakero::Profiler::Data&) noexcept;

		private:
			std::mutex    mutex;
			std::ostream* stream;
			std::ofstream file_output;
			bool          is_active;
	};

	// }}}
}

// {{{ Implementation

#ifdef ZAKERO_PROFILER_IMPLEMENTATION

// {{{ Macros

/**
 * \internal
 *
 * \brief _Error_Table_
 *
 * An X-Macro table of error codes. The columns are:
 * -# __ErrorName__<br>
 * -# __ErrorValue__<br>
 * -# __ErrorMessage__<br>
 */
#define ZAKERO_PROFILER__ERROR_DATA \
	X(Error_None                , 0 , "No Error"                                       ) \
	X(Error_Stream_Already_Open , 1 , "The profiler is already using an output stream" ) \
	X(Error_No_Filename         , 2 , "No filename was provided"                       ) \
	X(Error_Cant_Open_Stream    , 3 , "Unable to open the output stream"               ) \
	X(Error_Bad_Stream          , 4 , "The stream is not in a good state"              ) \

/**
 * \internal
 *
 * \brief Make generating Error Conditions less verbose.
 *
 * All this Macro Function does is expand into code that creates an 
 * std::error_code.
 *
 * \param err_ The error code
 */
#define ZAKERO_PROFILER__ERROR(err_) std::error_code(err_, ProfilerErrorCategory)

// {{{ Macros : Doxygen

#ifdef ZAKERO__DOXYGEN_DEFINE_DOCS

// Only used for generating Doxygen documentation

/**
 * \brief Activate the implementation code.
 *
 * Defining this macro will cause the _Zakero Profiler_ implementation to be 
 * included.  This should only be done once, since compiler and/or linker 
 * errors will typically be generated if more than a single implementation is 
 * found.
 *
 * \note It does not matter if the macro is given a value or not, only its 
 * existence is checked.
 */
#define ZAKERO_PROFILER_IMPLEMENTATION

/**
 * \brief Enable the profiler
 *
 * The _Zakero Profiler_ is a macro based system.  When this macro is defined, 
 * the macros will be replaced with code.  If this macro is __not__ defined, 
 * the macros will be removed at compile time.
 *
 * \note It does not matter if the macro is given a value or not, only its 
 * existence is checked.
 */
#define ZAKERO_PROFILER_ENABLE

#endif // ZAKERO__DOXYGEN_DEFINE_DOCS

// }}}
// }}}

namespace zakero
{
// {{{ Anonymous Namespace

namespace
{
	/**
	 * \brief Error Categories.
	 *
	 * This class implements the std::error_category interface to provide 
	 * consistent access to error code values and messages.
	 *
	 * See https://en.cppreference.com/w/cpp/error/error_category for 
	 * details.
	 */
	class ProfilerErrorCategory_
		: public std::error_category
	{
		public:
			constexpr ProfilerErrorCategory_() noexcept
			{
			}

			const char* name() const noexcept override
			{
				return "zakero.Profiler";
			}

			std::string message(int condition) const override
			{
				switch(condition)
				{
#define X(name_, val_, mesg_) \
					case val_: return mesg_;
					ZAKERO_PROFILER__ERROR_DATA
#undef X
				}

				return "Unknown error condition";
			}
	} ProfilerErrorCategory;

#define X(name_, val_, mesg_) \
	static constexpr int name_ = val_;
	ZAKERO_PROFILER__ERROR_DATA
#undef X

	zakero::Profiler zakero_profiler;

	// std::format performance testing
	//uint64_t count = 0;
	//uint64_t total = 0;
}

// }}}
// {{{ Documentation

/**
 * \internal
 *
 * \class Profiler
 *
 * \brief The Profiler
 *
 * The Profiler provides the interface to the output generator.  It primary 
 * responsibility is to write out the information in Profiler::Data as JSON 
 * formatted data.  The JSON format used by Chromium and Google Chrome is 
 * available in the [Trace Event 
 * Format](https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU) 
 * document.
 * - report(const zakero::Profiler::Data&)
 *
 * In order to be able to write the JSON data, the Profiler must initialize the 
 * output streams.
 * - init(std::ostream&, zakero::Profiler::MetaData)
 * - init(std::string, zakero::Profiler::MetaData)
 *
 * To avoid being lost in generated data, the profiler can be turned on and off 
 * as needed.
 * - activate()
 * - deactivate()
 */

// }}}

/**
 * \brief Constructor
 *
 * Get the Profiler ready for initialization.  The Profiler is not considered 
 * to be active until after one of the init() methods have been called.
 */
Profiler::Profiler() noexcept
	: mutex()
	, stream(nullptr)
	, file_output()
	, is_active(false)
{
}


/**
 * \brief Destructor
 *
 * Close the JSON array and object.
 */
Profiler::~Profiler() noexcept
{
	std::lock_guard<std::mutex> lock(zakero_profiler.mutex);

	if(stream != nullptr)
	{
		(*stream) << "]}" << std::endl;

		if(file_output.is_open())
		{
			file_output.close();
		}
	}

	// std::format performance testing
	//std::cout << "Avg: " << (total / count) << "\n";
}


/**
 * \brief Initialize the Profiler.
 *
 * An output stream will be created using the provided \p path.  The Profiler 
 * will maintain the life-time of the output stream.
 *
 * Any information in the \p meta_data will be immediately written to the 
 * output stream.  See the documentation for the ZAKERO_PROFILER_INIT() macro 
 * for information on keys that will be over-written.
 */
std::error_code zakero::Profiler::init(const std::filesystem::path path      ///< The file path
	, zakero::Profiler::MetaData                               meta_data ///< Extra meta data
	) noexcept
{
	if(zakero_profiler.stream != nullptr)
	{
		return ZAKERO_PROFILER__ERROR(Error_Stream_Already_Open);
	}

	if(path.has_filename() == false)
	{
		return ZAKERO_PROFILER__ERROR(Error_No_Filename);
	}

	zakero_profiler.file_output.open(path);

	if(zakero_profiler.file_output.is_open() == false)
	{
		return ZAKERO_PROFILER__ERROR(Error_Cant_Open_Stream);
	}

	std::error_code error;

	error = init(zakero_profiler.file_output, meta_data);

	return error;
}


/**
 * \brief Initialize the Profiler.
 *
 * The provided \p output_stream will be used when writing the profiling data.
 *
 * Any information in the \p meta_data will be immediately written to the 
 * output stream.  See the documentation for the ZAKERO_PROFILER_INIT() macro 
 * for information on keys that will be over-written.
 */
std::error_code zakero::Profiler::init(std::ostream& output_stream ///< The output stream
	, zakero::Profiler::MetaData                 meta_data     ///< Extra meta data
	) noexcept
{
	std::lock_guard<std::mutex> lock(zakero_profiler.mutex);

	if(zakero_profiler.stream != nullptr)
	{
		return ZAKERO_PROFILER__ERROR(Error_Stream_Already_Open);
	}

	if(output_stream.good() == false)
	{
		return ZAKERO_PROFILER__ERROR(Error_Bad_Stream);
	}

	zakero_profiler.stream = &output_stream;
	activate();

	if(meta_data.contains("date") == false)
	{
		std::time_t time = std::chrono::system_clock::to_time_t(
			std::chrono::system_clock::now()
			);

		char buf[128];
		std::strftime(buf, sizeof(buf), "%F %T", std::localtime(&time));

		meta_data["date"] = buf;
	}

	if(meta_data.contains("traceEvents"))
	{
		meta_data.erase("traceEvents");
	}

	meta_data["displayTimeUnit"] = "ms";

	(*zakero_profiler.stream) << "{";

	for(const auto& iter : meta_data)
	{
		(*zakero_profiler.stream) << "\"" << iter.first << "\":\"" << iter.second << "\",";
	}

	(*zakero_profiler.stream) << "\"traceEvents\":[{}\n";

	return ZAKERO_PROFILER__ERROR(Error_None);
}


/**
 * \brief Activate the output generator.
 *
 * The output generator will be activated and profiling will once again be 
 * written.
 */
void zakero::Profiler::activate() noexcept
{
	zakero_profiler.is_active = true;
}


/**
 * \brief Deactivate the output generator.
 *
 * The output generator will be deactivated.  No data will be written until the 
 * profiler has been re-activated.
 *
 * \note If profiler is deactivated before "duration" profiling end-event has 
 * been written, it will still be written.
 */
void zakero::Profiler::deactivate() noexcept
{
	zakero_profiler.is_active = false;
}


/**
 * \brief Write the data.
 *
 * Write the profiling \p data to the output stream.
 */
void Profiler::report(const zakero::Profiler::Data& data ///< Profiling data
	) noexcept
{
	std::lock_guard<std::mutex> lock(zakero_profiler.mutex);

	// std::format performance testing
	//const auto t1  = std::chrono::steady_clock::now();

	(*zakero_profiler.stream)
		<< ",{\"ph\":\""   << data.phase << "\""
		<<  ",\"ts\":"     << data.time_stamp
		<<  ",\"dur\":"    << data.duration
		<<  ",\"pid\":"    << data.process_id
		<<  ",\"tid\":"    << data.thread_id
		<<  ",\"cat\":\""  << data.category << "\""
		<<  ",\"name\":\"" << data.name << "\""
		<<  ",\"args\":"
			<< "{\"file_name\":\""     << data.location.file_name() << "\""
			<< ",\"function_name\":\"" << data.location.function_name() << "\""
		;
	for(const auto& iter : data.meta_data)
	{
		(*zakero_profiler.stream)
			<< ",\"" << iter.first << "\":\"" << iter.second << "\""
			;
	}

	(*zakero_profiler.stream) << "}}\n";

	/* C++20 std::format ready
	(*zakero_profiler.stream) << std::format(
		",{{\"ph\":\"{}\""
		",\"ts\":{}"
		",\"cat\":\"{}\""
		",\"name\":\"{}\""
		",\"pid\":{}"
		",\"tid\":{}"
		",\"args\": {{\"file_name\":\"{}\",\"function_name\":\"{}\"}}"
		"}}\n"
		, data.phase
		, nano
		, data.category
		, data.name
		, data.process_id
		, data.thread_id
		, data.location.file_name()
		, data.location.function_name()
		);

	// std::format performance testing
	const auto t2  = std::chrono::steady_clock::now();
	const auto delta  = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
	std::cout << (uint64_t)delta << "\n";
	total += delta;
	count++;
	*/
}


/**
 * \internal
 *
 * \struct Profiler::Data
 *
 * \brief Profiling Data
 *
 * All data related to a profiling event is stored in this object.
 */

/**
 * \brief Initialize the Profiler Data.
 *
 * Initialize the data using the provided values as well as run-time values.  
 */
Profiler::Data::Data(const char                     phase     ///< The phase
	, const std::string&                        category  ///< The category
	, const std::string&                        name      ///< The name
	, const std::experimental::source_location& location  ///< The location
	, zakero::Profiler::MetaData                meta_data ///< Extra meta data
	) noexcept
	: meta_data(meta_data)
	, category(category)
	, name(name)
	, location(location)
	, duration(0)
	, time_stamp(ZAKERO_STEADY_TIME_NOW(microseconds))
	, thread_id(std::this_thread::get_id())
	, process_id(ZAKERO_PID)
	, phase(phase)
{
}


/**
 * \internal
 *
 * \struct Profiler::Complete
 *
 * \brief Profiling Complete
 *
 * Adds data a behavior to Profiler::Data
 */

/**
 * \brief Initialize the Profiler Complete.
 *
 * Initialize the data using the provided values as well as run-time values.  
 *
 * Since this object is a member of Profiler, it has access to Profiler's 
 * private methods.
 */
Profiler::Complete::Complete(const std::string&     category  ///< The category
	, const std::string&                        name      ///< The name
	, const std::experimental::source_location& location  ///< The location
	, zakero::Profiler::MetaData                meta_data ///< Extra meta data
	) noexcept
	: zakero::Profiler::Data('X', category, name, location, meta_data)
	, was_active(zakero_profiler.is_active)
{
}


/**
 * \brief Destructor
 *
 * Write the collected data.
 */
Profiler::Complete::~Complete() noexcept
{
	if(was_active || zakero_profiler.is_active)
	{
		duration = ZAKERO_STEADY_TIME_NOW(microseconds) - time_stamp;
		zakero::Profiler::report(*this);
	}
}


/**
 * \internal
 *
 * \struct Profiler::Duration
 *
 * \brief Profiling Duration
 *
 * Adds data a behavior to Profiler::Data
 */

/**
 * \brief Initialize the Profiler Duration.
 *
 * Initialize the data using the provided values as well as run-time values.  
 *
 * Since this object is a member of Profiler, it has access to Profiler's 
 * private methods.
 */
Profiler::Duration::Duration(const std::string&     category  ///< The category
	, const std::string&                        name      ///< The name
	, const std::experimental::source_location& location  ///< The location
	, zakero::Profiler::MetaData                meta_data ///< Extra meta data
	) noexcept
	: zakero::Profiler::Data('B', category, name, location, meta_data)
	, was_active(zakero_profiler.is_active)
{
}


/**
 * \brief Destructor
 *
 * Write the collected data.
 */
Profiler::Duration::~Duration() noexcept
{
	if(was_active || zakero_profiler.is_active)
	{
		zakero::Profiler::report(*this);

		phase = 'E';
		time_stamp = ZAKERO_STEADY_TIME_NOW(microseconds);
		zakero::Profiler::report(*this);
	}
}


/**
 * \internal
 *
 * \struct Profiler::Instant
 *
 * \brief Profiling Instant
 *
 * Adds data a behavior to Profiler::Data
 */

/**
 * \brief Initialize the Profiler Instant.
 *
 * Initialize the data using the provided values as well as run-time values.  
 *
 * Since this object is a member of Profiler, it has access to Profiler's 
 * private methods.
 */
Profiler::Instant::Instant(const std::string&       category  ///< The category
	, const std::string&                        name      ///< The name
	, const std::experimental::source_location& location  ///< The location
	, zakero::Profiler::MetaData                meta_data ///< Extra meta data
	) noexcept
	: zakero::Profiler::Data('i', category, name, location, meta_data)
{
	if(zakero_profiler.is_active)
	{
		zakero::Profiler::report(*this);
	}
}

} // zakero

#endif // ZAKERO_PROFILER_IMPLEMENTATION

// }}}

#endif // zakero_Profiler_h
