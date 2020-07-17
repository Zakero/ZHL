/* *******************************************************************
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * \file
 *
 * \brief Zakero Profiler
 *
 * \par TL;DR:
 *
 * This library will generate profiling data while your application is running.  
 * To use:
 * 1. Add the implementation to a source code file:
 *    ~~~
 *    #define ZAKERO_PROFILER_IMPLEMENTATION
 *    #include "path/to/Zakero_Profiler.h"
 *    ~~~
 * 2. Add the profiler to where it is used:
 *    ~~~
 *    #include "path/to/Zakero_Profiler.h"
 *    ~~~
 * 3. Enable the profile at compile time by defining ZAKERO_PROFILER_ENABLE
 *    ~~~
 *    #define ZAKERO_PROFILER_ENABLE
 *    ~~~
 *    or as a compiler flag
 *    ~~~
 *    -DZAKERO_PROFILER_ENABLE
 *    ~~~
 *
 * \par What Is It?
 * \parblock
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
 * \endparblock
 *
 * \par Why Use It?
 * \parblock
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
 * \endparblock
 *
 * \par How To Use It?
 * \parblock
 * __Step 0__
 *
 * Your compiler must support at least the C++20 standard.
 *
 * __Step 1__
 *
 * The first step is to select which C++ source code file will contain the 
 * _Zakero Profiler_ implementation.  Once the location has been determined, 
 * add the following to that file:
 *
 * ~~~
 * #define ZAKERO_PROFILER_IMPLEMENTATION
 * #include "path/to/Zakero_Profiler.h"
 * ~~~
 *
 * The macro __ZAKERO_PROFILER_IMPLEMENTATION__ tells the header file to 
 * include the implementation of the profiler.
 *
 * In all other files that will use the _Zakero Profiler_, they need to include 
 * the header.  In addition to including the header, the profiler must be 
 * enabled by defining the ZAKERO_PROFILER_ENABLE macro.
 *
 * The easiest way to do this is to always have the _Zakero_Profiler_ enabled:
 *
 * ~~~
 * #define ZAKERO_PROFILER_ENABLE
 * #include "path/to/Zakero_Profiler.h"
 * ~~~
 *
 * This, however, is not very realistic.  A better solution is to configure 
 * your build system to define ZAKERO_PROFILER_ENABLE when needed and add the 
 * _Zakero Profiler_ header and macros where desired.
 *
 * ~~~
 * #include "path/to/Zakero_Profiler.h"
 * ~~~
 *
 * And if your "build system" is the command-line, the following will turn on 
 * profiling:
 *
 * ~~~
 * > g++ ... -DZAKERO_PROFILER_ENABLE ...
 * ~~~
 *
 * If the ZAKERO_PROFILER_ENABLE macro is __not__ defined, all the other marcos 
 * in this library will be disabled.  With compiler optimizations turned on, 
 * -O2, the unused profiler code should be removed from the resulting 
 * executable.
 *
 * __Step 2__
 *
 * Next, is to initialize the _Zakero Profiler_.  Somewhere in your code, 
 * before any profiling is done (such as in _main()_) add one of the 
 * initializer macros (ZAKERO_PROFILER_INIT(), 
 * ZAKERO_PROFILER_INIT_METADATA()).
 *
 * The following example will have the _Zakero Profiler_ write to a file and 
 * include information about the application.
 *
 * ~~~
 * zakero::Profiler::MetaData meta_data =
 * {	{ "application", "MyApp"                      }
 * ,	{ "version",     "1.0.0"                      }
 * ,	{ "intent",      "=Locate Allocation Delays=" }
 * };
 * 
 * ZAKERO_PROFILER_INIT_METADATA("MyApp.profiler_json", meta_data);
 * ~~~
 *
 * __Step 3__
 *
 * All the prep work is done, now it is time to generate some profiling data.  
 * To do this use one of these macros:
 * - ZAKERO_PROFILER()
 * - ZAKERO_PROFILER_INSTANT()
 *
 * The ZAKERO_PROFILER macro is best used at the start of a code block.  
 * Remember you do not have to put this macro after every '{', only add the 
 * macros where you need to capture data.
 *
 * For example:
 *
 * ~~~
 * void func()
 * {
 * 	ZAKERO_PROFILER("cache availability", "linear defrag")
 *
 * 	do_stuff();
 *
 * 	if(!done)
 * 	{
 * 		ZAKERO_PROFILER("cache availability", "extra defrag-ing")
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
 * \endparblock
 *
 * \version Future
 * - Add meta data to ZAKERO_PROFILER
 * - Add meta data to ZAKERO_PROFILER_INSTANT
 * - Add support for std::filesystem
 *
 * \version 0.8.0
 * - Profile time duration in C++ code blocks
 * - Add "instant" markers to the timeline
 *
 * \copyright [Mozilla Public License 
 * v2](https://www.mozilla.org/en-US/MPL/2.0/) 
 *
 * \author Andrew "Zakero" Moore
 * 	- Original Author
 */

#ifndef ZAKERO_PROFILER_H
#define ZAKERO_PROFILER_H

#include <chrono>
#include <ctime>
#include <experimental/source_location>
#include <fstream>
#include <iostream>
#include <map>
#include <thread>

#if defined(ZAKERO_PROFILER_ENABLE)

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
 * \par Example
 * \code
 * ZAKERO_PROFILER_INIT(std::clog);
 * // --- or --- //
 * ZAKERO_PROFILER_INIT("profile.log");
 * \endcode
 *
 * \note The _Zakero Profiler_ does not flush after writing data to the output 
 * using the stream operator (<<).  The reason for this is to allow the 
 * operating system to decide the best time to write the data, reducing the 
 * number of I/O requests the profiler makes.  As a result of this, profiling 
 * data will be incomplete if the application does not cleanly exit.
 *
 * \see ZAKERO_PROFILER_INIT_METADATA()
 *
 * \param output_ Where to stream the profile data.
 */
#define ZAKERO_PROFILER_INIT(output_) \
	zakero::Profiler::init(output_, {});

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
 * Any extra data that should appear in the profiler output can be added using 
 * the \p meta_data_.  \p meta_data_ is a map of _%std::string/%std::string_ 
 * key/value pairs.  The "date" key will be automatically added if not already 
 * present.  The following keys will be over written if used:
 * - displayTimeUnit
 * - traceEvents
 *
 * \par Example
 * \code
 * zakero::Profiler::MetaData meta_data =
 * {	{ "application", "MyApp" }
 * ,	{ "version",     "1.2.3" }
 * };
 * ZAKERO_PROFILER_INIT_METADATA("MyApp.profile_json", meta_data);
 * \endcode
 *
 * \note The _Zakero Profiler_ does not flush after writing data to the output 
 * using the stream operator (<<).  The reason for this is to allow the 
 * operating system to decide the best time to write the data, reducing the 
 * number of I/O requests the profiler makes.  As a result of this, profiling 
 * data will be incomplete if the application does not cleanly exit.
 *
 * \see ZAKERO_PROFILER_INIT()
 *
 * \param output_    Where to stream the profile data
 * \param meta_data_ Extra data
 */
#define ZAKERO_PROFILER_INIT_METADATA(output_, meta_data_) \
	zakero::Profiler::init(output_, meta_data_);

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
 * \par Example
 * \code
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
 * \endcode
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
 * \internal
 *
 * \brief Concatenate the two things.
 *
 * An extra layer of C/C++ Preprocessor indirection to enforce proper order of 
 * macro expansion and concatenation.
 *
 * \param thing_1_ Symbol left side
 * \param thing_2_ Symbol right side
 */
#define ZAKERO_CONCAT_(thing_1_, thing_2_) thing_1_ ## thing_2_

/**
 * \internal
 *
 * \brief Concatenate the two things.
 *
 * Use the C/C++ Preprocessor to create a new symbol name.  For example the 
 * symbol _abcxyz_ could be created using _ZAKERO_CONCAT(abc, xyz)_.
 *
 * \par Example
 * \code
 * int foobar = 1;
 * ZAKERO_CONCAT(foo, bar)++; // foobar == 2
 *
 * int ZAKERO_CONCAT(magic_, 42) = 123;
 * // int magic_42 = 123;
 * \endcode
 *
 * \param thing_1_ Symbol left side
 * \param thing_2_ Symbol right side
 */
#define ZAKERO_CONCAT(thing_1_, thing_2_) ZAKERO_CONCAT_(thing_1_, thing_2_)

/**
 * \internal
 *
 * \brief Create a unique variable name.
 *
 * Create a unique variable name using the provided \p name_ and append the 
 * current line number.
 *
 * \note An alternative would be to use __COUNTER__ instead of __LINE__, 
 * however __COUNTER__ is not part of the C++ standard even though it is widely 
 * supported.
 *
 * \param name_ The variable name.
 */
#define ZAKERO_PROFILER_UNIQUE(name_) ZAKERO_CONCAT(name_, __LINE__)

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
 * \note The file name and function name are automatically recorded.
 *
 * \par Example
 * \code
 * void func()
 * {
 *     ZAKERO_PROFILER("busy", "doing stuff")
 *     
 *     // Doing stuff
 *     
 *     for(int i = 0; i < max_i; i++)
 *     {
 *         ZAKERO_PROFILER("busy", "gone loopy")
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
 * \endcode
 *
 * \param category_ The category of the data
 * \param name_     The name of the data
 */
#define ZAKERO_PROFILER(category_, name_)                                    \
	zakero::Profiler::Data ZAKERO_PROFILER_UNIQUE(zakero_profiler_data_) \
	( 'B'                                                                \
	, category_                                                          \
	, name_                                                              \
	, std::experimental::source_location::current()                      \
	);                                                                   \

/**
 * \brief Generate profiler data.
 *
 * This macro will generate an "instant" event.  These events are useful to 
 * mark something in the timeline.
 *
 * The \p category_ and \p name_ serve the same function as in 
 * ZAKERO_PROFILER().
 *
 * \param category_ The category of the data
 * \param name_     The name of the data
 */
#define ZAKERO_PROFILER_INSTANT(category_, name_)                            \
	{                                                                    \
	zakero::Profiler::Data ZAKERO_PROFILER_UNIQUE(zakero_profiler_data_) \
	( 'I'                                                                \
	, category_                                                          \
	, name_                                                              \
	, std::experimental::source_location::current()                      \
	);                                                                   \
	}                                                                    \

#else

#define ZAKERO_PROFILER_INIT(output_)
#define ZAKERO_PROFILER_INIT_METADATA(output_, meta_data_)
#define ZAKERO_PROFILER_ACTIVATE
#define ZAKERO_PROFILER_DEACTIVATE
#define ZAKERO_PROFILER(category_, name_)
#define ZAKERO_PROFILER_INSTANT(category_, name_)

#endif

namespace zakero
{
	class Profiler
	{
		public:
			using MetaData = std::map<std::string, std::string>;

			struct Data
			{
				std::string                        category;
				std::string                        name;
				std::experimental::source_location location;
				std::thread::id                    thread_id;
				pid_t                              process_id;
				char                               phase;

				Data(const char, const std::string&, const std::string&, const std::experimental::source_location&) noexcept;
				~Data() noexcept;
			};

			Profiler() noexcept;
			~Profiler() noexcept;

			static void init(std::ostream&, zakero::Profiler::MetaData) noexcept;
			static void init(std::string, zakero::Profiler::MetaData) noexcept;

			static void activate() noexcept;
			static void deactivate() noexcept;

		private:
			static void report(const zakero::Profiler::Data&) noexcept;

			std::ostream* stream;
			std::ofstream file_output;
			bool          is_active;
	};
}

#if defined (ZAKERO_PROFILER_IMPLEMENTATION)

#ifdef __linux__
#include <sys/types.h>
#include <unistd.h>
#define ZAKERO_PROFILER_PID getpid()
#else
#define ZAKERO_PROFILER_PID -1
#endif

namespace
{
	zakero::Profiler zakero_profiler;

	// std::format performance testing
	//uint64_t count = 0;
	//uint64_t total = 0;
}


namespace zakero
{

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

/**
 * \brief Constructor
 *
 * Get the Profiler ready for initialization.  The Profiler is not considered 
 * to be active until after one of the init() methods have been called.
 */
Profiler::Profiler() noexcept
	: stream(nullptr)
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
	(*stream) << "]}" << std::endl;

	if(file_output.is_open())
	{
		file_output.close();
	}

	// std::format performance testing
	//std::cout << "Avg: " << (total / count) << "\n";
}


/**
 * \brief Initialize the Profiler.
 *
 * An output stream will be created using the provided \p file_name.  The 
 * Profiler will maintain the life-time of the output stream.
 *
 * Any information in the \p meta_data will be immediately written to the 
 * output stream.  See the documentation for the 
 * ZAKERO_PROFILER_INIT_METADATA() macro for information on keys that will be 
 * over-written.
 */
void zakero::Profiler::init(std::string file_name ///< The file name
	, zakero::Profiler::MetaData    meta_data ///< Extra meta data
	) noexcept
{
	if(zakero_profiler.stream != nullptr)
	{
		return;
	}

	zakero_profiler.file_output.open(file_name);

	init(zakero_profiler.file_output, meta_data);
}


/**
 * \brief Initialize the Profiler.
 *
 * The provided \p output_stream will be used when writing the profiling data.
 *
 * Any information in the \p meta_data will be immediately written to the 
 * output stream.  See the documentation for the 
 * ZAKERO_PROFILER_INIT_METADATA() macro for information on keys that will be 
 * over-written.
 */
void zakero::Profiler::init(std::ostream& output_stream ///< The output stream
	, zakero::Profiler::MetaData      meta_data     ///< Extra meta data
	) noexcept
{
	if(zakero_profiler.stream != nullptr)
	{
		return;
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
	// std::format performance testing
	//const auto t1  = std::chrono::steady_clock::now();
	const auto now  = std::chrono::steady_clock::now().time_since_epoch();
	const auto nano = std::chrono::duration_cast<std::chrono::microseconds>(now).count();

	(*zakero_profiler.stream) << ",{\"ph\":\"" << data.phase << "\""
		<< ",\"ts\":"     << nano
		<< ",\"cat\":\""  << data.category << "\""
		<< ",\"name\":\"" << data.name << "\""
		<< ",\"pid\":"    << data.process_id
		<< ",\"tid\":"    << data.thread_id
		<< ",\"args\":"
			<< "{\"file_name\":\""     << data.location.file_name() << "\""
			<< ",\"function_name\":\"" << data.location.function_name() << "\""
			<< "}"
		<< "}\n"
		;
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
 * \class Profiler::Data
 *
 * \brief Profiling Data
 *
 * All data related to a profiling event is stored in this object.
 */

/**
 * \brief Initialize the Profiler Data.
 *
 * Initialize the data using the provided values as well as run-time values.  
 * After initialization, write the collected data.
 *
 * Since this object is a member of Profiler, it has access to Profiler's 
 * private methods.
 */
Profiler::Data::Data(const char                     phase    ///< The phase
	, const std::string&                        category ///< The category
	, const std::string&                        name     ///< The name
	, const std::experimental::source_location& location ///< The location
	) noexcept
	: category(category)
	, name(name)
	, location(location)
	, thread_id(std::this_thread::get_id())
	, process_id(ZAKERO_PROFILER_PID)
	, phase(phase)
{
	if(zakero_profiler.is_active)
	{
		zakero::Profiler::report(*this);
	}
	else
	{
		// If the profiler has been deactivated,
		// change the phase to something that
		// will not have an "end" report.
		this->phase = '\0';
	}
}


/**
 * \internal
 *
 * \brief Destructor
 *
 * Report an "ending" event if needed.
 */
Profiler::Data::~Data() noexcept
{
	if(phase == 'B')
	{
		phase = 'E';
		zakero::Profiler::report(*this);
	}
}

} // zakero

#endif // ZAKERO_PROFILER_IMPLEMENTATION
#endif // ZAKERO_PROFILER_H
