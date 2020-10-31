/******************************************************************************
 * Copyright 2020 Andrew Moore
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef zakero_Xenium_h
#define zakero_Xenium_h

/**
 * \file
 *
 * \brief Zakero Xenium
 *
|                      |                         |                         |                         |                        |                        |                            |
|----------------------|-------------------------|-------------------------|-------------------------|------------------------|------------------------|----------------------------|
| \api{zakero::Xenium} | \refdeps{zakero_xenium} | \reftldr{zakero_xenium} | \refwhat{zakero_xenium} | \refwhy{zakero_xenium} | \refhow{zakero_xenium} | \refversion{zakero_xenium} |
 *
 * Making the X11/XCB windowing system easy to use.
 *
 * \pardeps{zakero_xenium}
 * - Zakero_Base.h
 * - Zakero_MemoryPool.h
 * \endpardeps
 *
 * \partldr{zakero_xenium}
 * This library makes it very easy to connect to an X11 server and display 
 * windows.  The developer must write pixel data directly into the window.  No 
 * UI or graphics functionality is provided.
 *
 * To use:
 * 1. Add the implementation to a source code file:
 *    \code
 *    #define ZAKERO_MEMORYPOOL_IMPLEMENTATION
 *    #define ZAKERO_XENIUM_IMPLEMENTATION
 *    #include "Zakero_Xenium.h"
 *    \endcode
 * 2. Add the library to where it is used:
 *    \code
 *    #include "Zakero_Xenium.h"
 *    \endcode
 * 3. Link to the X11/XCB libraries and pthread when building:
 *    \code
 *    -lpthread -lwayland-client
 *    \endcode
 * \endpartldr
 *
 * \parwhat{zakero_xenium}
 * X11 is a Windowing System and is used in many of the Unix-based operating 
 * systems such as Linux and BSD.
 *
 * \endparwhat
 *
 * \parwhy{zakero_xenium}
 * \endparwhy
 *
 * \parhow{zakero_xenium}
 * __Step 0__
 *
 * Your compiler must support at least the C++20 standard.  The location of the 
 * `Zakero_*.h` header files _must_ be in your compiler's include path.
 *
 * __Step 1__
 *
 * The first step is to select which C++ source code file will contain the 
 * _Zakero Xenium_ implementation.  Once the location has been determined, add 
 * the following to that file:
 *
 * ~~~
 * #define ZAKERO_MEMORYPOOL_IMPLEMENTATION
 * #define ZAKERO_XENIUM_IMPLEMENTATION
 * #include "Zakero_Xenium.h"
 * ~~~
 *
 * The macro \ref ZAKERO_XENIUM_IMPLEMENTATION tells the header file to include 
 * the implementation of the Xenium.
 *
 * Notice that the _Zakero MemoryPool_ implementation, 
 * ZAKERO_MEMORYPOLL_IMPLEMENTATION, is included before the `Zakero_Yetain.h` 
 * header.  _Zakero Xenium_ depends on the memory pool for shared memory with 
 * the X11 server.  If the _Zakero MemoryPool_ implementation was added else 
 * where, this define can be omitted.
 *
 * In all other files that will use Xenium, they only need to include the 
 * header.
 *
 * ~~~
 * #include "Zakero_Xenium.h"
 * ~~~
 *
 * __Step 2__
 *
 * Below is a simple program that will conncet to the X11 Server and display a 
 * window.
 *
 * ~~~
 * #define ZAKERO_MEMORYPOOL_IMPLEMENTATION
 * #define ZAKERO_XENIUM_IMPLEMENTATION
 * #include "Zakero_Xenium.h"
 *
 * int main()
 * {
 * 	std::error_code error;
 * 
 * 	zakero::Xenium* xenium = zakero::Xenium::connect(error);
 * 	if(error)
 * 	{
 * 		std::cout << "Error: " << error << '\n';
 * 		return 1;
 * 	}
 * 
 * 	zakero::Xenium::SizePixel window_size = { 640, 480 };
 * 	auto* window = xenium->windowCreate(window_size, error);
 * 	if(error)
 * 	{
 * 		std::cout << "Error: " << error << '\n';
 * 		return 1;
 * 	}
 * 
 * 	window->setTitle("Xenium");
 * 
 * 	bool time_to_die = false;
 * 	window->onCloseRequest([&]()
 * 	{
 * 		time_to_die = true;
 * 	});
 * 
 * 	while(time_to_die == false)
 * 	{
 * 		std::this_thread::yield();
 * 	}
 * 
 * 	delete window;
 * 	delete xenium;
 * 
 * 	return 0;
 * }
 * ~~~
 *
 * The following commands will build and run the example program.
 * ~~~
 * > g++ -std=c++20 -lpthread -lxcb -o example example.cpp
 * > ./example
 * ~~~
 * \endparhow
 *
 * \parversion{zakero_xenium}
 * __v0.1.0__
 * - The start
 *
 * \endparversion
 */

/******************************************************************************
 * Includes
 */

#include <iostream>
#include <thread>

// POSIX
#include <poll.h>

// Linux
#include <linux/input-event-codes.h>

// X11/XCB
#include <xcb/xcb.h>
#include <xcb/randr.h>

// Zakero
#include "Zakero_Base.h"
#include "Zakero_MemoryPool.h"


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
#define ZAKERO_XENIUM__ERROR_DATA \
	X(Error_None                                   ,  0 , "No Error"                                                         ) \
	X(Error_Unknown                                ,  1 , "An unknown error has occurred"                                    ) \
	X(Error_Connection_Failed                      ,  2 , "Failed due to socket, pipe, or other stream errors"               ) \
	X(Error_Extension_Not_Supported                ,  3 , "The requested XCB extension is not supported"                     ) \
	X(Error_Not_Enough_Memory                      ,  4 , "Insufficient memory"                                              ) \
	X(Error_Request_Too_Long                       ,  5 , "The request was longer than what is excepted by the X11 server"   ) \
	X(Error_Invalid_Display_Name                   ,  6 , "An error occured while parsing the X11 display name"              ) \
	X(Error_Invalid_Screen                         ,  7 , "The X11 server does not have a screen matching the display"       ) \
	X(Error_RandR_Not_Available                    ,  8 , "XCB RandR extenstion is not available"                            ) \
	X(Error_RandR_Version_Too_Old                  ,  9 , "XCB RandR version is too old"                                     ) \
	X(Error_RandR_Screen_Resources_Not_Found       , 10 , "XCB RandR could not locate any screen resources"                  ) \
	X(Error_RandR_Invalid_CRTC_Id                  , 11 , "XCB RandR CRTC ID is not valid"                                   ) \
	X(Error_RandR_Invalid_Output_Id                , 12 , "XCB RandR Output ID is not valid"                                 ) \
	X(Error_RandR_Output_Info_Not_Found            , 13 , "XCB RandR Output Information was not found"                       ) \
	X(Error_RandR_CRTC_Info_Not_Found              , 14 , "XCB RandR CRTC Information was not found"                         ) \
	X(Error_RandR_Output_Info_Is_Incomplete        , 15 , "XCB RandR Output Information does not have enough data"           ) \
	X(Error_Minimum_Size_Greater_Than_Maximum_Size , 92 , "The minimum window size is larger than the maximum window size."  ) \
	X(Error_Window_Size_Too_Small                  , 99 , "The window size was too small."                                   ) \

	/* --- To Be Deleted --- */
	/*
	*/

// }}}

/**
 * \cond Doxygen_Ignore
 */

/**
 * \endcond
 */

namespace zakero
{
	// {{{ Declaration

	class Xenium
	{
		public:
#define X(name_, val_, mesg_) \
			static constexpr int name_ = val_;
			ZAKERO_XENIUM__ERROR_DATA
#undef X

			virtual ~Xenium() noexcept;

			// {{{ Type : Key

			enum struct KeyState
			{	Released = 0
			,	Pressed  = 1
			,	Repeat   = 2
			};

			struct Key
			{
				uint32_t time;
				uint32_t code;
				KeyState state;
			};

			static constexpr uint32_t KeyModifier_Shift    = 0x00000001;
			static constexpr uint32_t KeyModifier_CapsLock = 0x00000002;
			static constexpr uint32_t KeyModifier_Control  = 0x00000004;
			static constexpr uint32_t KeyModifier_Alt      = 0x00000008;
			static constexpr uint32_t KeyModifier_NumLock  = 0x00000010;
			static constexpr uint32_t KeyModifier_Meta     = 0x00000040;

			struct KeyModifier
			{
				uint32_t pressed = 0;
				uint32_t latched = 0;
				uint32_t locked  = 0;
				uint32_t group   = 0;
			};

			// }}}
			// {{{ Type : Point

			struct PointMm
			{
				uint32_t time;
				float    x;
				float    y;

				friend bool operator==(Xenium::PointMm&, Xenium::PointMm&) noexcept;
			};

			struct PointPercent
			{
				uint32_t time;
				float    x;
				float    y;

				friend bool operator==(Xenium::PointPercent&, Xenium::PointPercent&) noexcept;
			};

			struct PointPixel
			{
				uint32_t time;
				int32_t  x;
				int32_t  y;

				friend bool operator==(Xenium::PointPixel&, Xenium::PointPixel&) noexcept;
			};

			// }}}
			// {{{ Type : Pointer Axis

			enum struct PointerAxisSource
			{	Unknown
			,	Continuous
			,	Finger
			,	Wheel
			,	Wheel_Tilt
			};

			enum struct PointerAxisType
			{	Unknown
			,	Horizontal
			,	Vertical
			};

			struct PointerAxis
			{
				uint32_t                  time;
				int32_t                   steps;
				float                     distance;
				Xenium::PointerAxisSource source;
				Xenium::PointerAxisType   type;
			};

			// }}}
			// {{{ Type : Pointer Button

			enum struct PointerButtonState
			{	Released = 0
			,	Pressed  = 1
			};

			struct PointerButton
			{
				uint32_t                   code;
				Xenium::PointerButtonState state;
			};

			// }}}
			// {{{ Type : Size

			struct SizeMm
			{
				float width;
				float height;

				friend bool operator==(Xenium::SizeMm&, Xenium::SizeMm&) noexcept;
			};

			struct SizePercent
			{
				float width;
				float height;

				friend bool operator==(Xenium::SizePercent&, Xenium::SizePercent&) noexcept;
			};

			struct SizePixel
			{
				int32_t width;
				int32_t height;

				friend bool operator==(Xenium::SizePixel&, Xenium::SizePixel&) noexcept;
			};

			// }}}
			// {{{ Connection

			static Xenium* connect() noexcept;
			static Xenium* connect(const std::string&) noexcept;
			static Xenium* connect(std::error_code&) noexcept;
			static Xenium* connect(const std::string&, std::error_code&) noexcept;

			// }}}
			// {{{ Cursor

			// }}}
			// {{{ Keyboard

			int32_t keyRepeatDelay() const noexcept;
			int32_t keyRepeatRate() const noexcept;

			// }}}
			// {{{ Output : X11/XCB

			struct Output
			{
				std::string name                     = "";
				//std::string make                     = ""; // Not Available?
				//std::string model                    = ""; // Not Available?
				int32_t     x                        = 0;
				int32_t     y                        = 0;
				int32_t     width                    = 0;
				int32_t     height                   = 0;
				int64_t     physical_width_mm        = 0;
				int64_t     physical_height_mm       = 0;
				int32_t     subpixel                 = 0;
				//int32_t     refresh_mHz              = 0; // Not Available?
				//int32_t     scale_factor             = 0; // Not Available?
				int32_t     transform                = 0; // Not Available?
				float       pixels_per_mm_horizontal = 0.0;
				float       pixels_per_mm_vertical   = 0.0;
			};

			// -------------------------------------------------- //

			using OutputId = uint32_t;

			using LambdaOutputId = std::function<void(const Xenium::OutputId)>;

			using VectorOutputId = std::vector<OutputId>;

			// -------------------------------------------------- //

			Xenium::Output         output(const Xenium::OutputId) const noexcept;
			Xenium::VectorOutputId outputVector() const noexcept;
			static std::string     outputSubpixelName(int32_t) noexcept;
			//static std::string     outputTransformName(int32_t) noexcept; // Not Available?

			Xenium::PointMm        outputConvertToMm(const Xenium::OutputId, const Xenium::PointPixel&) const noexcept;
			Xenium::PointPercent   outputConvertToPercent(const Xenium::OutputId, const Xenium::PointPixel&) const noexcept;
			Xenium::PointPixel     outputConvertToPixel(const Xenium::OutputId, const Xenium::PointMm&) const noexcept;
			Xenium::PointPixel     outputConvertToPixel(const Xenium::OutputId, const Xenium::PointPercent&) const noexcept;

			Xenium::SizeMm         outputConvertToMm(const Xenium::OutputId, const Xenium::SizePixel&) const noexcept;
			Xenium::SizePercent    outputConvertToPercent(const Xenium::OutputId, const Xenium::SizePixel&) const noexcept;
			Xenium::SizePixel      outputConvertToPixel(const Xenium::OutputId, const Xenium::SizeMm&) const noexcept;
			Xenium::SizePixel      outputConvertToPixel(const Xenium::OutputId, const Xenium::SizePercent&) const noexcept;

			void                   outputOnAdd(Xenium::LambdaOutputId) noexcept;
			void                   outputOnChange(Xenium::LambdaOutputId) noexcept;
			void                   outputOnRemove(Xenium::LambdaOutputId) noexcept;

			// }}}
			// {{{ Window

			enum struct WindowDecorations
			{	Client_Side
			,	Server_Side
			};

			enum struct WindowMode
			{	Normal
			,	Fullscreen
			,	Maximized
			};

			// -------------------------------------------------- //

			using Lambda                  = std::function<void()>;
			using LambdaKey               = std::function<void(const Xenium::Key&, const Xenium::KeyModifier&)>;
			using LambdaAxis              = std::function<void(const Xenium::PointerAxis&, const Xenium::KeyModifier&)>;
			using LambdaButtonMm          = std::function<void(const Xenium::PointerButton&, const Xenium::PointMm&, const Xenium::KeyModifier&)>;
			using LambdaButtonPercent     = std::function<void(const Xenium::PointerButton&, const Xenium::PointPercent&, const Xenium::KeyModifier&)>;
			using LambdaButtonPixel       = std::function<void(const Xenium::PointerButton&, const Xenium::PointPixel&, const Xenium::KeyModifier&)>;
			using LambdaPointMm           = std::function<void(const Xenium::PointMm&, const Xenium::KeyModifier&)>;
			using LambdaPointPercent      = std::function<void(const Xenium::PointPercent&, const Xenium::KeyModifier&)>;
			using LambdaPointPixel        = std::function<void(const Xenium::PointPixel&, const Xenium::KeyModifier&)>;
			using LambdaBool              = std::function<void(bool)>;
			using LambdaWindowDecorations = std::function<void(Xenium::WindowDecorations)>;
			using LambdaWindowMode        = std::function<void(Xenium::WindowMode)>;
			using LambdaSizeMm            = std::function<void(const Xenium::SizeMm&)>;
			using LambdaSizePercent       = std::function<void(const Xenium::SizePercent&)>;
			using LambdaSizePixel         = std::function<void(const Xenium::SizePixel&)>;

			// -------------------------------------------------- //

			class Window
			{
				public:
					Window();
					virtual ~Window();

					// {{{ Configuration

					void                 setClass(const std::string&) noexcept;
					void                 setTitle(const std::string&) noexcept;
					std::error_code      setDecorations(const Xenium::WindowDecorations) noexcept;
					std::error_code      setSize(const Xenium::SizeMm&) noexcept;
					std::error_code      setSize(const Xenium::SizePercent&) noexcept;
					std::error_code      setSize(const Xenium::SizePixel&) noexcept;
					std::error_code      setSizeMinMax(const Xenium::SizeMm&, const Xenium::SizeMm&) noexcept;
					std::error_code      setSizeMinMax(const Xenium::SizePercent&, const Xenium::SizePercent&) noexcept;
					std::error_code      setSizeMinMax(const Xenium::SizePixel&, const Xenium::SizePixel&) noexcept;

					uint8_t              bytesPerPixel() const noexcept;

					// }}}
					// {{{ Window Mode

					Xenium::WindowMode   windowMode() noexcept;
					bool                 windowModeIs(const Xenium::WindowMode) noexcept;
					void                 windowModeSet(const Xenium::WindowMode) noexcept;
					void                 windowModeOnChange(Xenium::LambdaWindowMode) noexcept;

					void                 minimize() noexcept;

					// }}}
					// {{{ Rendering

					std::error_code      imageNext(uint8_t*&, Xenium::SizePixel&) noexcept;
					void                 imagePresent() noexcept;
					uint32_t             time() const noexcept;

					// }}}
					// {{{ Conversion

					Xenium::PointMm      convertToMm(const Xenium::PointPixel&) const noexcept;
					Xenium::PointPercent convertToPercent(const Xenium::PointPixel&) const noexcept;
					Xenium::PointPixel   convertToPixel(const Xenium::PointMm&) const noexcept;
					Xenium::PointPixel   convertToPixel(const Xenium::PointPercent&) const noexcept;

					Xenium::SizeMm       convertToMm(const Xenium::SizePixel&) const noexcept;
					Xenium::SizePercent  convertToPercent(const Xenium::SizePixel&) const noexcept;
					Xenium::SizePixel    convertToPixel(const Xenium::SizeMm&) const noexcept;
					Xenium::SizePixel    convertToPixel(const Xenium::SizePercent&) const noexcept;

					// }}}
					// {{{ Cursor

					// }}}
					// {{{ Keyboard

					void                 keyboardOnEnter(Xenium::Lambda) noexcept;
					void                 keyboardOnLeave(Xenium::Lambda) noexcept;
					void                 keyboardOnKey(Xenium::LambdaKey) noexcept;

					// }}}
					// {{{ Pointer

					void                 pointerOnEnter(Xenium::LambdaPointMm) noexcept;
					void                 pointerOnEnter(Xenium::LambdaPointPercent) noexcept;
					void                 pointerOnEnter(Xenium::LambdaPointPixel) noexcept;
					void                 pointerOnLeave(Xenium::Lambda) noexcept;
					void                 pointerOnMotion(Xenium::LambdaPointMm) noexcept;
					void                 pointerOnMotion(Xenium::LambdaPointPercent) noexcept;
					void                 pointerOnMotion(Xenium::LambdaPointPixel) noexcept;
					void                 pointerOnButton(Xenium::LambdaButtonMm) noexcept;
					void                 pointerOnButton(Xenium::LambdaButtonPercent) noexcept;
					void                 pointerOnButton(Xenium::LambdaButtonPixel) noexcept;
					void                 pointerOnAxis(Xenium::LambdaAxis) noexcept;
					void                 pointerOnAxisSource(Xenium::Lambda) noexcept;
					void                 pointerOnAxisStop(Xenium::Lambda) noexcept;
					void                 pointerOnAxisDiscrete(Xenium::Lambda) noexcept;

					// }}}
					// {{{ Events

					void                 onCloseRequest(Xenium::Lambda) noexcept;
					void                 onDecorationsChange(Xenium::LambdaWindowDecorations) noexcept;
					void                 onFocusChange(Xenium::LambdaBool) noexcept;
					void                 onSizeChange(Xenium::LambdaSizeMm) noexcept;
					void                 onSizeChange(Xenium::LambdaSizePercent) noexcept;
					void                 onSizeChange(Xenium::LambdaSizePixel) noexcept;

					// }}}

				private:
					Window(const Window&) = delete;
					Window& operator=(const Window&) = delete;
			};

			// -------------------------------------------------- //

			Xenium::Window* windowCreate(const Xenium::SizeMm&, std::error_code&) noexcept;
			Xenium::Window* windowCreate(const Xenium::SizePercent&, std::error_code&) noexcept;
			Xenium::Window* windowCreate(const Xenium::SizePixel&, std::error_code&) noexcept;

			// }}}

		private:
			bool               magic_compiler_happies;

			static constexpr uint32_t Size_Max = (uint32_t)std::numeric_limits<int32_t>::max();

			Xenium() noexcept;

			// {{{ Type

			// }}}
			// {{{ Connection

			void disconnect() noexcept;

			// }}}
			// {{{ Initialization

			std::error_code init(xcb_connection_t*, int) noexcept;

			// }}}
			// {{{ Cursor

			// }}}
			// {{{ Event Loop

			std::jthread      event_loop;
			std::atomic<bool> event_loop_is_running;

			// -------------------------------------------------- //

			void        eventLoopStart() noexcept;
			static void eventLoop(std::stop_token, Xenium*) noexcept;

			// }}}
			// {{{ Output

			using Map_OutputId_Output = std::unordered_map<Xenium::OutputId, Xenium::Output>;

			struct OutputData
			{
				mutable std::mutex  mutex     = {};
				Map_OutputId_Output map       = {};
				LambdaOutputId      on_add    = {};
				LambdaOutputId      on_change = {};
				LambdaOutputId      on_remove = {};
			};

			// -------------------------------------------------- //

			OutputData output_data = {};

			// -------------------------------------------------- //

			std::error_code outputInit() noexcept;
			std::error_code outputAdd(xcb_randr_crtc_t, xcb_randr_output_t) noexcept;
			void            outputAdd(const xcb_randr_get_crtc_info_reply_t*, const xcb_randr_get_output_info_reply_t*) noexcept;

			// }}}
			// {{{ XCB

			xcb_connection_t*  connection = nullptr;
			const xcb_setup_t* setup      = nullptr;
			xcb_screen_t*      screen     = nullptr;

			// }}}
			// {{{ XCB : RandR

			int randr_error_base          = 0;
			int randr_event_base          = 0;
			int randr_query_version_major = 0;
			int randr_query_version_minor = 0;

			// -------------------------------------------------- //

			std::error_code randrInit() noexcept;

			void randrEvent(const xcb_randr_crtc_change_t*) noexcept;
			void randrEvent(const xcb_randr_output_change_t*) noexcept;
			void randrEvent(const xcb_randr_notify_event_t*) noexcept;
			void randrEvent(const xcb_randr_screen_change_notify_event_t*) noexcept;

			// }}}
			// {{{ Window

			// }}}
			// {{{ Utility

			std::pair<float, float>     convertPixelToMm(const Xenium::Output&, int32_t, int32_t) const noexcept;
			std::pair<float, float>     convertPixelToPercent(const Xenium::Output&, int32_t, int32_t) const noexcept;
			std::pair<int32_t, int32_t> convertMmToPixel(const Xenium::Output&, float, float) const noexcept;
			std::pair<int32_t, int32_t> convertPercentToPixel(const Xenium::Output&, float, float) const noexcept;

			// }}}

			Xenium(const Xenium&) = delete;
			Xenium& operator=(const Xenium&) = delete;
	}; // class Xenium

	// }}}
	// {{{ Convenience

	std::string to_string(const std::error_code&) noexcept;
	std::string to_string(const Xenium::KeyModifier&) noexcept;
	std::string to_string(const Xenium::KeyState) noexcept;
	std::string to_string(const Xenium::Output&) noexcept;
	std::string to_string(const Xenium::PointerAxisSource) noexcept;
	std::string to_string(const Xenium::PointerAxisType) noexcept;
	std::string to_string(const Xenium::PointerButtonState) noexcept;
	std::string to_string(const Xenium::WindowMode) noexcept;

	// }}}
}

// {{{ Implementation

#ifdef ZAKERO_XENIUM_IMPLEMENTATION

// {{{ Macros

/******************************************************************************
 * Macros
 */

// {{{ Macros : Doxygen

#ifdef ZAKERO__DOXYGEN_DEFINE_DOCS

// Only used for generating Doxygen documentation

/**
 * \brief Activate the implementation code.
 *
 * Defining this macro will cause the _Zakero Xenium_ implementation to be 
 * included.  This should only be done once, since compiler and/or linker 
 * errors will typically be generated if more than a single implementation is 
 * found.
 *
 * \note It does not matter if the macro is given a value or not, only its 
 * existence is checked.
 */
#define ZAKERO_XENIUM_IMPLEMENTATION

/**
 * \brief Enable debugging output
 *
 * Enabled debugging messages to be written.
 *
 * \internal
 * These messages are written by using `ZAKERO_XENIUM__DEBUG` macros.
 */
#define ZAKERO_XENIUM_ENABLE_DEBUG

/**
 * \brief Add extra error checking.
 *
 * Before a method executes, it should validate the argument values that it 
 * receives.  This is useful while developing code but adds overhead.  If the 
 * calling code can guarantee that all argument data is valid, this macro can 
 * be enabled to reduce the validation checks and overhead.
 */
#define ZAKERO_XENIUM_ENABLE_SAFE_MODE

#endif // ZAKERO__DOXYGEN_DEFINE_DOCS

// }}}

/**
 * \internal
 * 
 * \def ZAKERO_XENIUM__DEBUG_DISABLED
 *
 * \brief A disabled flag
 *
 * The inverse of ZAKERO_XENIUM_DEBUG_ENABLED and is used to enable or disable 
 * the debugging messages.
 *
 * If `ZAKERO_XENIUM_ENABLE_DEBUG` is `true` then this flag is `false`.
 * If `ZAKERO_XENIUM_ENABLE_DEBUG` is `false` then this flag is `true`.
 *
 * The purpose of this is to make other macros easier to read.
 */
#ifdef ZAKERO_XENIUM_ENABLE_DEBUG
#define ZAKERO_XENIUM__DEBUG_DISABLED false
#else
#define ZAKERO_XENIUM__DEBUG_DISABLED true
#endif


/**
 * \def ZAKERO_XENIUM_ENABLE_DEBUG_STREAM
 *
 * \brief The stream to use for debugging output.
 *
 * This macro holds the stream that will be written to for the debug messages.  
 * If this macro is undefined at compile time, then `std::cerr` will be used.
 *
 * \parcode
 * #define ZAKERO_XENIUM_ENABLE_DEBUG_STREAM MyLogger::errorLogStream()
 * \endparcode
 */
#ifndef ZAKERO_XENIUM_ENABLE_DEBUG_STREAM
#define ZAKERO_XENIUM_ENABLE_DEBUG_STREAM std::cerr
#endif


/**
 * \internal
 *
 * \brief Write debugging message.
 *
 * These macros are available to help make debugging easier.
 *
 * If the `ZAKERO_XENIUM_ENABLE_DEBUG` macro is enabled, this macro will write 
 * to the stream defined by ZAKERO_XENIUM_ENABLE_DEBUG_STREAM.
 *
 * \parcode
 * ZAKERO_XENIUM__DEBUG << "Insert debugging message here.\n";
 * \endparcode
 *
 * \todo Use std::source_location instead of macros once that feature is no 
 * longer experimental.
 */
#define ZAKERO_XENIUM__DEBUG                   \
	if(ZAKERO_XENIUM__DEBUG_DISABLED) {}   \
	else ZAKERO_XENIUM_ENABLE_DEBUG_STREAM \
		<< __FILE__"("                 \
		<< std::to_string(__LINE__)    \
		<< ") "                        \
		<< __PRETTY_FUNCTION__         \
		<< " "

/**
 * \internal
 *
 * \brief Write debugging message.
 *
 * Does the samething as `ZAKERO_XENIUM__DEBUG` but is specialized to work with 
 * std::ostream compatible objects.  Casting may be required to get the desired 
 * output.
 *
 * \parcode
 * uint64_t foo = 42;
 * ZAKERO_XENIUM__DEBUG_VAR(foo);
 * \endparcode
 *
 * \param var_ The variable to write.
 */
#define ZAKERO_XENIUM__DEBUG_VAR(var_)   \
	ZAKERO_XENIUM__DEBUG             \
		<< #var_ << ": " << var_ \
		<< "\n";

/**
 * \internal
 *
 * \brief Write debugging message.
 *
 * Does the samething as `ZAKERO_XENIUM__DEBUG` but is specialized to work with 
 * boolean values.
 *
 * \parcode
 * bool foo = true
 * ZAKERO_XENIUM__DEBUG_BOOL(foo);
 * \endparcode
 *
 * \param var_ The variable to write.
 */
#define ZAKERO_XENIUM__DEBUG_BOOL(var_)                    \
	ZAKERO_XENIUM__DEBUG                               \
		<< #var_ << ": " << std::boolalpha << var_ \
		<< "\n";

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
#define ZAKERO_XENIUM__ERROR(err_) std::error_code(err_, XeniumErrorCategory)


/**
 * \internal
 *
 * \brief Convert the enum to a string.
 */
#define ZAKERO_XENIUM__OUTPUT_SUBPIXEL \
	X(XCB_RENDER_SUB_PIXEL_UNKNOWN        , "Unkown Geometry" ) \
	X(XCB_RENDER_SUB_PIXEL_HORIZONTAL_RGB , "Horizontal RGB"  ) \
	X(XCB_RENDER_SUB_PIXEL_HORIZONTAL_BGR , "Horizontal BGR"  ) \
	X(XCB_RENDER_SUB_PIXEL_VERTICAL_RGB   , "Vertical RGB"    ) \
	X(XCB_RENDER_SUB_PIXEL_VERTICAL_BGR   , "Vertical BGR"    ) \
	X(XCB_RENDER_SUB_PIXEL_NONE           , "No Geometry"     ) \


// }}}

#include <sstream>

namespace zakero
{
// {{{ Anonymous Namespace

namespace
{
	// {{{ Cursor Names (Not used yet)
	/*
	 * \brief Common cursor names
	 *
	 * These were found in:
	 * - gdkcursor-wayland.c
	 * - /usr/share/icons/whiteglass/cursors
	const std::array<const char*, 131> common_cursor_names =
	{	"X_cursor"
	,	"alias"
	,	"all-scroll"
	,	"arrow"
	,	"base_arrow_down"
	,	"base_arrow_up"
	,	"bd_double_arrow"
	,	"boat"
	,	"bottom_left_corner"
	,	"bottom_right_corner"
	,	"bottom_side"
	,	"bottom_tee"
	,	"cell"
	,	"center_ptr"
	,	"circle"
	,	"closedhand"
	,	"col-resize"
	,	"color-picker"
	,	"context-menu"
	,	"copy"
	,	"cross"
	,	"cross_reverse"
	,	"crossed_circle"
	,	"crosshair"
	,	"default"
	,	"dnd-copy"
	,	"dnd-link"
	,	"dnd-move"
	,	"dnd-no-drop"
	,	"dnd-none"
	,	"dot"
	,	"dot_box_mask"
	,	"double_arrow"
	,	"down-arrow"
	,	"draft"
	,	"draft_large"
	,	"draft_small"
	,	"draped_box"
	,	"e-resize"
	,	"ew-resize"
	,	"exchange"
	,	"fd_double_arrow"
	,	"fleur"
	,	"forbidden"
	,	"grab"
	,	"grabbing"
	,	"gumby"
	,	"h_double_arrow"
	,	"half-busy"
	,	"hand"
	,	"hand1"
	,	"hand2"
	,	"help"
	,	"ibeam"
	,	"left-arrow"
	,	"left_ptr"
	,	"left_ptr_help"
	,	"left_ptr_watch"
	,	"left_side"
	,	"left_tee"
	,	"link"
	,	"ll_angle"
	,	"lr_angle"
	,	"move"
	,	"n-resize"
	,	"ne-resize"
	,	"nesw-resize"
	,	"no-drop"
	,	"not-allowed"
	,	"ns-resize"
	,	"nw-resize"
	,	"nwse-resize"
	,	"openhand"
	,	"pencil"
	,	"pirate"
	,	"plus"
	,	"pointer"
	,	"pointing_hand"
	,	"progress"
	,	"question_arrow"
	,	"right-arrow"
	,	"right_ptr"
	,	"right_side"
	,	"right_tee"
	,	"row-resize"
	,	"s-resize"
	,	"sailboat"
	,	"sb_down_arrow"
	,	"sb_h_double_arrow"
	,	"sb_left_arrow"
	,	"sb_right_arrow"
	,	"sb_up_arrow"
	,	"sb_v_double_arrow"
	,	"se-resize"
	,	"shuttle"
	,	"size-bdiag"
	,	"size-fdiag"
	,	"size-hor"
	,	"size-ver"
	,	"size_all"
	,	"size_bdiag"
	,	"size_fdiag"
	,	"size_hor"
	,	"size_ver"
	,	"sizing"
	,	"split_h"
	,	"split_v"
	,	"sw-resize"
	,	"target"
	,	"tcross"
	,	"text"
	,	"top_left_arrow"
	,	"top_left_corner"
	,	"top_right_corner"
	,	"top_side"
	,	"top_tee"
	,	"trek"
	,	"ul_angle"
	,	"up-arrow"
	,	"ur_angle"
	,	"v_double_arrow"
	,	"vertical-text"
	,	"w-resize"
	,	"wait"
	,	"watch"
	,	"wayland-cursor"
	,	"whats_this"
	,	"x-cursor"
	,	"xterm"
	,	"zoom-in"
	,	"zoom-out"
	};
	 */
	// }}}

	constexpr uint8_t XCB_KEY_REPEAT = XCB_KEY_PRESS | 0x80;

	/**
	 * \brief Error Categories.
	 *
	 * This class implements the std::error_category interface to provide 
	 * consistent access to error code values and messages.
	 *
	 * See https://en.cppreference.com/w/cpp/error/error_category for 
	 * details.
	 */
	class XeniumErrorCategory_
		: public std::error_category
	{
		public:
			constexpr XeniumErrorCategory_() noexcept
			{
			}

			const char* name() const noexcept override
			{
				return "zakero.Xenium";
			}

			std::string message(int condition) const override
			{
				switch(condition)
				{
#define X(name_, val_, mesg_) \
					case val_: return mesg_;
					ZAKERO_XENIUM__ERROR_DATA
#undef X
				}

				return "Unknown error condition";
			}
	} XeniumErrorCategory;


	/**
	 * \name Lambda's that do nothing.
	 * \{
	 */
	Xenium::Lambda                  Lambda_DoNothing                  = []() noexcept {};
	Xenium::LambdaKey               LambdaKey_DoNothing               = [](const Xenium::Key&, const Xenium::KeyModifier&) noexcept {};
	Xenium::LambdaAxis              LambdaAxis_DoNothing              = [](const Xenium::PointerAxis&, const Xenium::KeyModifier&) noexcept {};
	Xenium::LambdaButtonMm          LambdaButtonMm_DoNothing          = [](const Xenium::PointerButton&, const Xenium::PointMm&, const Xenium::KeyModifier&) noexcept {};
	Xenium::LambdaButtonPercent     LambdaButtonPercent_DoNothing     = [](const Xenium::PointerButton&, const Xenium::PointPercent&, const Xenium::KeyModifier&) noexcept {};
	Xenium::LambdaButtonPixel       LambdaButtonPixel_DoNothing       = [](const Xenium::PointerButton&, const Xenium::PointPixel&, const Xenium::KeyModifier&) noexcept {};
	Xenium::LambdaPointMm           LambdaPointMm_DoNothing           = [](const Xenium::PointMm&, const Xenium::KeyModifier&) noexcept {};
	Xenium::LambdaPointPercent      LambdaPointPercent_DoNothing      = [](const Xenium::PointPercent&, const Xenium::KeyModifier&) noexcept {};
	Xenium::LambdaPointPixel        LambdaPointPixel_DoNothing        = [](const Xenium::PointPixel&, const Xenium::KeyModifier&) noexcept {};
	Xenium::LambdaBool              LambdaBool_DoNothing              = [](const bool) noexcept {};
	Xenium::LambdaOutputId          LambdaOutputId_DoNothing          = [](const Xenium::OutputId) noexcept {};
	Xenium::LambdaWindowDecorations LambdaWindowDecorations_DoNothing = [](const Xenium::WindowDecorations) noexcept {};
	Xenium::LambdaWindowMode        LambdaWindowMode_DoNothing        = [](const Xenium::WindowMode) noexcept {};
	Xenium::LambdaSizeMm            LambdaSizeMm_DoNothing            = [](const Xenium::SizeMm&) noexcept {};
	Xenium::LambdaSizePercent       LambdaSizePercent_DoNothing       = [](const Xenium::SizePercent&) noexcept {};
	Xenium::LambdaSizePixel         LambdaSizePixel_DoNothing         = [](const Xenium::SizePixel&) noexcept {};
	/**
	 * \}
	 */

	
	/**
	 * \brief Convert the XCB error code.
	 *
	 * The provided \p xcb_error_code will be converted into a Xenium-based 
	 * error code.
	 *
	 * \return An error code.
	 */
	std::error_code convertConnectionError(int xcb_error_code ///< The XCB error
		) noexcept
	{
		switch(xcb_error_code)
		{
			case 0:
				return ZAKERO_XENIUM__ERROR(Xenium::Error_None);

			case XCB_CONN_ERROR:
				return ZAKERO_XENIUM__ERROR(Xenium::Error_Connection_Failed);

			case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:
				return ZAKERO_XENIUM__ERROR(Xenium::Error_Extension_Not_Supported);

			case XCB_CONN_CLOSED_MEM_INSUFFICIENT:
				return ZAKERO_XENIUM__ERROR(Xenium::Error_Not_Enough_Memory);

			case XCB_CONN_CLOSED_REQ_LEN_EXCEED:
				return ZAKERO_XENIUM__ERROR(Xenium::Error_Request_Too_Long);

			case XCB_CONN_CLOSED_PARSE_ERR:
				return ZAKERO_XENIUM__ERROR(Xenium::Error_Invalid_Display_Name);

			case XCB_CONN_CLOSED_INVALID_SCREEN:
				return ZAKERO_XENIUM__ERROR(Xenium::Error_Invalid_Screen);

			default:
				return ZAKERO_XENIUM__ERROR(Xenium::Error_Unknown);
		}
	}


	/**
	 * \brief Compare 2 floats
	 *
	 * Compare two floats for equality.  Since floats are not _exact_, this 
	 * function will calculate the difference between them.  For the float 
	 * values to be "equal", the difference must be less than the specified 
	 * \p delta.
	 *
	 * \retval true  The values are equal
	 * \retval false The values are not equal
	 */
	bool equalish(const float a     ///< The first value
		, const float     b     ///< The second value
		, const float     delta ///< The maximum difference
		) noexcept
	{
		return (std::abs(a - b) < delta);
	}


	/*
	std::string to_string(const xcb_screen_t& screen)
	{
		std::ostringstream buf;

		buf
			<< "{ \"root\": "                  << uint32_t(screen.root)
			<< ", \"default_colormap\": "      << uint32_t(screen.default_colormap)
			<< ", \"white_pixel\": "           << uint32_t(screen.white_pixel)
			<< ", \"black_pixel\": "           << uint32_t(screen.black_pixel)
			<< ", \"current_input_masks\": "   << uint32_t(screen.current_input_masks)
			<< ", \"width_in_pixels\": "       << uint32_t(screen.width_in_pixels)
			<< ", \"height_in_pixels\": "      << uint32_t(screen.height_in_pixels)
			<< ", \"width_in_millimeters\": "  << uint32_t(screen.width_in_millimeters)
			<< ", \"height_in_millimeters\": " << uint32_t(screen.height_in_millimeters)
			<< ", \"min_installed_maps\": "    << uint32_t(screen.min_installed_maps)
			<< ", \"max_installed_maps\": "    << uint32_t(screen.max_installed_maps)
			<< ", \"root_visual\": "           << uint32_t(screen.root_visual)
			<< ", \"backing_stores\": "        << uint32_t(screen.backing_stores)
			<< ", \"save_unders\": "           << uint32_t(screen.save_unders)
			<< ", \"root_depth\": "            << uint32_t(screen.root_depth)
			<< ", \"allowed_depths_len\": "    << uint32_t(screen.allowed_depths_len)
			<< " }";
		
		return buf.str();
	}
	*/


	/*
	std::string to_string(const xcb_setup_t& setup)
	{
		std::ostringstream buf;

		buf
			<< "{ \"status\": "                      << uint32_t(setup.status)
			<< ", \"pad0\": "                        << uint32_t(setup.pad0)
			<< ", \"protocol_major_version\": "      << uint32_t(setup.protocol_major_version)
			<< ", \"protocol_minor_version\": "      << uint32_t(setup.protocol_minor_version)
			<< ", \"length\": "                      << uint32_t(setup.length)
			<< ", \"release_number\": "              << uint32_t(setup.release_number)
			<< ", \"resource_id_base\": "            << uint32_t(setup.resource_id_base)
			<< ", \"resource_id_mask\": "            << uint32_t(setup.resource_id_mask)
			<< ", \"motion_buffer_size\": "          << uint32_t(setup.motion_buffer_size)
			<< ", \"vendor_len\": "                  << uint32_t(setup.vendor_len)
			<< ", \"maximum_request_length\": "      << uint32_t(setup.maximum_request_length)
			<< ", \"roots_len\": "                   << uint32_t(setup.roots_len)
			<< ", \"pixmap_formats_len\": "          << uint32_t(setup.pixmap_formats_len)
			<< ", \"image_byte_order\": "            << uint32_t(setup.image_byte_order)
			<< ", \"bitmap_format_bit_order\": "     << uint32_t(setup.bitmap_format_bit_order)
			<< ", \"bitmap_format_scanline_unit\": " << uint32_t(setup.bitmap_format_scanline_unit)
			<< ", \"bitmap_format_scanline_pad\": "  << uint32_t(setup.bitmap_format_scanline_pad)
			<< ", \"min_keycode\": "                 << uint32_t(setup.min_keycode)
			<< ", \"max_keycode\": "                 << uint32_t(setup.max_keycode)
			<< ", \"pad1\": [ "                      << uint32_t(setup.pad1[0])
				<< ", "                          << uint32_t(setup.pad1[1])
				<< ", "                          << uint32_t(setup.pad1[2])
				<< ", "                          << uint32_t(setup.pad1[3])
				<< " ]"
			<< " }";
		
		return buf.str();
	}
	*/


	/**
	 * \brief Validate values.
	 *
	 * Check the \p min value with the \p max value.
	 * - All values must be greater than or equal to 0
	 * - All \p min values must be less than the \p max values
	 *   - Exception: min or max equal to 0
	 *
	 * \return An error_code.
	 *
	 * \todo The check if \p min is > 0 may be unneeded.
	 * \todo Constraints would be nice, but not required.
	 */
	template<class Type ///< A Size* type
		>
	std::error_code validateMinMax(const Type& min ///< The minimum size
		, const Type&                      max ///< The maximum size
		) noexcept
	{
		if((min.width < 0)
			|| (min.height < 0)
			|| (max.width < 0)
			|| (max.height < 0)
			)
		{
			return ZAKERO_XENIUM__ERROR(Xenium::Error_Window_Size_Too_Small);
		}

		if((min.width > 0)
			&& (max.width > 0)
			&& (min.width > max.width)
			)
		{
			return ZAKERO_XENIUM__ERROR(Xenium::Error_Minimum_Size_Greater_Than_Maximum_Size);
		}

		if((min.height > 0)
			&& (max.height > 0)
			&& (min.height > max.height)
			)
		{
			return ZAKERO_XENIUM__ERROR(Xenium::Error_Minimum_Size_Greater_Than_Maximum_Size);
		}

		return ZAKERO_XENIUM__ERROR(Xenium::Error_None);
	};
}

// }}}
// {{{ Documentation

/**
 * \class Xenium
 *
 * \brief A wrapper class for X11/XCB.
 *
 * Refer to Zakero_Xenium.h to learn how to include this library.
 *
 * The intent of this class is to make creating programs that interface 
 * directly with X11 easier.  While there are toolkits out there that do this 
 * (GTK, Qt, SDL, etc.), they also come with a lot of overhead.  This class 
 * will abstract much of X11 and provide a simple event driven interface.
 *
 * As soon as a connection to the X11 server has been established, a thread 
 * will be created to move all the communications to the background.  This 
 * means that there is no "main loop" in the Xenium object to hook into.  The 
 * benefit to this is that Window contents/graphics will not block event 
 * handling.
 *
 * Refer to \refhow{zakero_xenium} for a simple example.
 *
 * \par %Window
 * \parblock
 * Using Xenium to connect to a X11 server is just the first step.  The next 
 * step is to create a Window that can display _something_.  This is done using 
 * one of the %windowCreate() methods.  There are 3 sets of methods:
 * - Millimeters
 *   - windowCreate(const Xenium::SizeMm&, std::error_code&)
 * - Percent
 *   - windowCreate(const Xenium::SizePercent&, std::error_code&)
 * - Pixels
 *   - windowCreate(const Xenium::SizePixel&, std::error_code&)
 *
 * Why three?  The traditional pixels for size has many problems in today's 
 * display technology.  A resolution of 1024x768 may sound consistent in that 
 * you know exactly how many pixels will be used on every display.  However, 
 * 1024x768 may be OK on some displays, it may be too small on 4k laptop 
 * screens.  And what about the 8k displays?
 *
 * The easy answer is to find the maximum resolution and use all of it, or just 
 * half for a windowed app.  Instead of doing that work, why not just use 
 * percentages like 1.00x1.00 or 0.50x0.50 that will work on all display 
 * resolutions?  That is why Xenium has the SizePercent object.
 *
 * What about millimeters?  Using millimeters gives the developer more 
 * confidence that size of an area will be physically consistent across 
 * devices.  To illustrate this consider defining a 300x150 button.  This 
 * button may be easily clickable on most monitors but may be too small on a 4k 
 * display.  Defining a button to be 20mm x 10mm, it will be the same physical 
 * size on all monitors.
 *
 * What's wrong with using [DPI](https://en.wikipedia.org/wiki/Dots_per_inch)?  
 * DPI has become a mess in the computer world and is resolution, not a size.
 * \endparblock
 *
 * \par Thread (not) Safe
 * \parblock
 *
 * The main X11 event loop runs in a dedicated thread.  Because of this, there 
 * are race-conditions where execution uses the same data.  The most likely 
 * thread conflict is the resizing of a surface:
 * - User Thread: Calling one of the window's "size" methods
 * - X11 Thread: Maximizing/Fullscreen/resizing the window
 * .
 * While the above is mostly protected via mutexs' there is still a chance.
 *
 * There are other potential problems such as the User Thread deleting a window 
 * while the X11 Thread is resizing the same window.  To resolve this issue 
 * will require a complete evaluation of data and thread activities.
 *
 * Or take the easy way out and rewrite Xenium to be single-threaded...
 * \endparblock
 *
 * \internal
 *
 * If a struct contains a mutex, that mutex should be locked before interacting 
 * with the contents of the struct.
 */

/**
 * \enum Xenium::KeyState
 *
 * \brief %Keyboard key state
 *
 * When using a keyboard, a key will emit one of these states.
 */
/* Disabled because Doxygen does not support "enum classes"
 *
 * \var Xenium::Released
 * \brief The key was released
 *
 * \var Xenium::Pressed
 * \brief The key was pressed
 *
 * \var Xenium::Repeat
 * \brief The key is being held down
 */

/**
 * \struct Xenium::Key
 *
 * \brief %Key event information.
 *
 * Time is based on the "steady clock" and not system time.
 *
 * \var Xenium::Key::time
 * \brief When the key event happened.
 *
 * \var Xenium::Key::code
 * \brief The key code of the event.
 *
 * \var Xenium::Key::state
 * \brief The state of the key.
 */

/**
 * \var Xenium::KeyModifier_Alt
 * \brief %Key Modifier flag.
 *
 * \var Xenium::KeyModifier_CapsLock
 * \brief %Key Modifier flag.
 *
 * \var Xenium::KeyModifier_Control
 * \brief %Key Modifier flag.
 *
 * \var Xenium::KeyModifier_Meta
 * \brief %Key Modifier flag.
 *
 * \var Xenium::KeyModifier_Shift
 * \brief %Key Modifier flag.
 */

/**
 * \struct Xenium::KeyModifier
 *
 * \brief A collection modifier flags.
 *
 * The meaning of the modifiers are:
 * - __pressed__<br>
 *   This is the easiest to understand type of modifier.  A modifier such as 
 *   "Control" is pressed.
 * - __locked__<br>
 *   The best example of a "locked" modifier is the "Caps Lock" key.
 * - __latched__<br>
 *   This modifer can occur with "Sticky Keys".
 *
 * \var Xenium::KeyModifier::pressed
 * \brief A collection of pressed modifiers.
 *
 * \var Xenium::KeyModifier::latched
 * \brief A collection of latched modifiers.
 *
 * \var Xenium::KeyModifier::locked
 * \brief A collection of locked modifiers.
 *
 * \var Xenium::KeyModifier::group
 * \brief The keyboard layout
 */

/**
 * \struct Xenium::PointMm
 *
 * \brief A location that uses millimeters.
 *
 * \var Xenium::PointMm::time
 * \brief Where in time the point is (if > 0).
 *
 * \var Xenium::PointMm::x
 * \brief Where in the X-Axis the point is.
 *
 * \var Xenium::PointMm::y
 * \brief Where in the Y-Axis the point is.
 */

/**
 * \struct Xenium::PointPercent
 *
 * \brief A location that uses percentages.
 *
 * The percentage range of values is 0.0...1.0, where 0 is the left (or top) 
 * and 1 is the right (or bottom).
 *
 * \var Xenium::PointPercent::time
 * \brief Where in time the point is (if > 0).
 *
 * \var Xenium::PointPercent::x
 * \brief Where in the X-Axis the point is.
 *
 * \var Xenium::PointPercent::y
 * \brief Where in the Y-Axis the point is.
 */

/**
 * \struct Xenium::PointPixel
 *
 * \brief A location that uses pixels.
 *
 * \var Xenium::PointPixel::time
 * \brief Where in time the point is (if > 0).
 *
 * \var Xenium::PointPixel::x
 * \brief Where in the X-Axis the point is.
 *
 * \var Xenium::PointPixel::y
 * \brief Where in the Y-Axis the point is.
 */

/**
 * \enum Xenium::PointerAxisSource
 *
 * \brief Where the axis information came from.
 *
 * \var Xenium::Unknown
 * \brief Unknown
 *
 * \var Xenium::Continuous
 * \brief Continuous
 *
 * \var Xenium::Finger
 * \brief Finger
 *
 * \var Xenium::Wheel
 * \brief Wheel
 *
 * \var Xenium::Wheel_Tilt
 * \brief Wheel Tilt
 */

/**
 * \enum Xenium::PointerAxisType
 *
 * \brief The direction of the axis movement.
 *
 * \var Xenium::Unknown
 * \brief Unknown
 *
 * \var Xenium::Horizontal
 * \brief Horizontal
 *
 * \var Xenium::Vertical
 * \brief Vertical
 */

/**
 * \struct Xenium::PointerAxis
 *
 * \brief Information about an Axis event.
 *
 * \var Xenium::PointerAxis::time
 * \brief When the event occurred
 *
 * \var Xenium::PointerAxis::steps
 * \brief The number of rotation steps
 *
 * \var Xenium::PointerAxis::distance
 * \brief The distance traveled
 *
 * \var Xenium::PointerAxis::source
 * \brief The source of the event
 *
 * \var Xenium::PointerAxis::type
 * \brief The type of Axis
 */

/**
 * \enum Xenium::PointerButtonState
 *
 * \brief Mouse button state.
 *
 * \var Xenium::Released
 * \brief Released
 *
 * \var Xenium::Pressed
 * \brief Pressed
 */

/**
 * \struct Xenium::PointerButton
 *
 * \brief Information about a pointer button event.
 *
 * \var Xenium::PointerButton::code
 * \brief The event code
 *
 * \var Xenium::PointerButton::state
 * \brief The button state
 */

/**
 * \struct Xenium::SizeMm
 *
 * \brief Size measured in millimeters.
 *
 * \var SizeMm::width
 * \brief The width.
 *
 * \var SizeMm::height
 * \brief The height.
 */

/**
 * \struct Xenium::SizePercent
 *
 * \brief Size measured as a percentage of the Output (Monitor) resolution.
 *
 * The percentage range of values is 0.0...1.0, where 0 is the left (or top) 
 * and 1 is the right (or bottom).
 *
 * \var SizePercent::width
 * \brief The width.
 *
 * \var SizePercent::height
 * \brief The height.
 */

/**
 * \struct Xenium::SizePixel
 *
 * \brief Size measured in pixels.
 *
 * \var SizePixel::width
 * \brief The width.
 *
 * \var SizePixel::height
 * \brief The height.
 */

// }}}
// {{{ Static Member Initialization
// }}}
// {{{ Constructor / Destructor

/**
 * \brief Constructor.
 *
 * Create a new instance of Xenium.
 */
Xenium::Xenium() noexcept
	: magic_compiler_happies(true)
	, event_loop()
	, event_loop_is_running(false)
{
}


/**
 * \brief Destructor.
 *
 * Deleting the Xenium object will automatically disconnect from the X11 
 * server.
 *
 * Because Xenium is multithreaded, this method will block until all 
 * connections to X11 have been closed.
 */
Xenium::~Xenium() noexcept
{
	if(event_loop_is_running || event_loop.joinable())
	{
		event_loop.request_stop();
		event_loop.join();
	}

	disconnect();
}

// }}}
// {{{ Connection

/**
 * \brief Establish a connection with the X11 server.
 *
 * Establish a connection with the X11 server.  The following values will be 
 * used to determine which X11 server to connect to:
 * -# __DISPLAY__<br>
 *    Use the value of this environment variable.
 * -# `nullptr`<br>
 *    The %X11 defined default.
 *
 * If a connection was successful, then a pointer to a new Xenium instance will 
 * be returned.  This pointer will need to be `delete`d by the caller.  If 
 * there was an error connecting to the X11 server, `nullptr` will be returned.
 *
 * \note A connection to the X11 server must be made before any windows are 
 * created.
 *
 * \return A pointer to a Xenium instance or a `nullptr` if there was an error.
 *
 * \thread_user
 */
Xenium* Xenium::connect() noexcept
{
	std::error_code error; 

	return Xenium::connect("", error);
}


/**
 * \brief Establish a connection with the X11 server.
 *
 * Establish a connection with the X11 server.  The following values will be 
 * used to determine which X11 server to connect to:
 * -# __DISPLAY__<br>
 *    Use the value of this environment variable.
 * -# `nullptr`<br>
 *    The %X11 defined default.
 *
 * If a connection was successful, then a pointer to a new Xenium instance will 
 * be returned.  This pointer will need to be `delete`d by the caller.  If 
 * there was an error connecting to the X11 server, `nullptr` will be returned.
 *
 * \note A connection to the X11 server must be made before any windows are 
 * created.
 *
 * \return A pointer to a Xenium instance or a `nullptr` if there was an error.
 *
 * \thread_user
 */
Xenium* Xenium::connect(const std::string& display ///< The Display Name or ID
	) noexcept
{
	std::error_code error; 

	return Xenium::connect(display, error);
}


/**
 * \brief Establish a connection with the X11 server.
 *
 * Establish a connection with the X11 server.  The following values will be 
 * used to determine which X11 server to connect to:
 * -# __DISPLAY__<br>
 *    Use the value of this environment variable.
 * -# `nullptr`<br>
 *    The %X11 defined default.
 *
 * If a connection was successful, then a pointer to a new Xenium instance will 
 * be returned.  This pointer will need to be `delete`d by the caller.  If 
 * there was an error connecting to the X11 server, `nullptr` will be returned.
 *
 * The provided \p error will be set to Xenium::Error_None or the reason for 
 * the connection failure.  An error code of `Xenium::Error_Connection_Failed` 
 * or `Xenium::Error_Invalid_Display_Name` may indicate that there is no X11 
 * server available.
 *
 * \note A connection to the X11 server must be made before any windows are 
 * created.
 *
 * \return A pointer to a Xenium instance or a `nullptr` if there was an error.
 *
 * \thread_user
 */
Xenium* Xenium::connect(std::error_code& error ///< The error code
	) noexcept
{
	return Xenium::connect("", error);
}

/**
 * \brief Establish a connection with the X11 server.
 *
 * Establish a connection with the X11 server.  The following values will be 
 * used to determine which X11 server to connect to:
 * -# __DISPLAY__<br>
 *    Use the value of this environment variable.
 * -# `nullptr`<br>
 *    The %X11 defined default.
 *
 * If a connection was successful, then a pointer to a new Xenium instance will 
 * be returned.  This pointer will need to be `delete`d by the caller.  If 
 * there was an error connecting to the X11 server, `nullptr` will be returned.
 *
 * The provided \p error will be set to Xenium::Error_None or the reason for 
 * the connection failure.  An error code of `Xenium::Error_Connection_Failed` 
 * or `Xenium::Error_Invalid_Display_Name` may indicate that there is no X11 
 * server available.
 *
 * \note A connection to the X11 server must be made before any windows are 
 * created.
 *
 * \return A pointer to a Xenium instance or a `nullptr` if there was an error.
 *
 * \thread_user
 */
Xenium* Xenium::connect(const std::string& display ///! The Display Name or ID
	, std::error_code&                 error   ///! The error status
	) noexcept
{
	const char* display_name = nullptr;

	if(display.empty() == false)
	{
		display_name = display.c_str();
	}

	// --- Connect To X11 Server --- //
	int screen_number = 0;
	xcb_connection_t* connection = xcb_connect(display_name, &screen_number);
	int xcb_error = xcb_connection_has_error(connection);
	if(xcb_error)
	{
		error = convertConnectionError(xcb_error);

		return nullptr;
	}

	// --- Xenium --- //
	Xenium* xenium = new Xenium();

	error = xenium->init(connection, screen_number);
	if(error)
	{
		ZAKERO_DELETE(xenium);

		return nullptr;
	}

	xenium->eventLoopStart();

	error = ZAKERO_XENIUM__ERROR(Error_None);

	return xenium;
}


/**
 * \brief Disconnect from X11.
 *
 * In addition to disconnecting from X11, all allocated resources will be 
 * destroyed.  Any windows created from this instance will be invalidated and 
 * any further use of those windows will result in undefined behavior.
 *
 * \thread_user
 */
void Xenium::disconnect() noexcept
{
	this->setup = nullptr;

	if(connection)
	{
		xcb_disconnect(this->connection);
		connection = nullptr;
	}

	return;
}

// }}}
// {{{ Initialization

/**
 * \brief Initialize the Xenium object.
 *
 * \return The error code.
 */
std::error_code Xenium::init(xcb_connection_t* connection ///< The XCB Connection
	, int screen_number                               ///< The Screen being used.
	) noexcept
{
	this->connection = connection;

	// --- X11 Server Setup Information --- //
	this->setup = xcb_get_setup(this->connection);

	// --- Find the current screen --- //
	xcb_screen_iterator_t screen_iterator = xcb_setup_roots_iterator(this->setup);

	for(int i = 0; i < screen_number; i++)
	{
		xcb_screen_next(&screen_iterator);
	}

	this->screen = screen_iterator.data;

	// --- Initialize The Internals --- //
	std::error_code error;

	error = randrInit();
	if(error)
	{
		return error;
	}

	error = outputInit();
	if(error)
	{
		return error;
	}

	return ZAKERO_XENIUM__ERROR(Error_None);
}


// }}}
// {{{ Cursor

// }}}
// {{{ Utility

// }}}
// {{{ Event Loop

//#define ZAKERO_XENIUM__ENABLE_THREAD_SCHEDULER

/**
 * \brief Start the event loop.
 *
 * Calling this method will start the Event Loop and block until the Event Loop 
 * exits.
 *
 * \thread_user
 */
void Xenium::eventLoopStart() noexcept
{
	event_loop = std::jthread(&Xenium::eventLoop, this);

	while(event_loop_is_running.load() == false)
	{
		// Wait for the thread to start
		std::this_thread::sleep_for(std::chrono::nanoseconds(42));
	}

	#ifdef ZAKERO_XENIUM__ENABLE_THREAD_SCHEDULER
	int policy = SCHED_FIFO;
	int priority_min = sched_get_priority_min(policy);
	int priority_max = sched_get_priority_max(policy);

	sched_param sched =
	{	.sched_priority = (priority_min + priority_max) / 2
	};

	pthread_setschedparam(event_loop.native_handle(), policy, &sched);
	#endif
}


/**
 * \brief Event processing.
 *
 * The Xenium Event Loop all the messages between the X11 client and server.  
 * Without this communication, programs that use the Xenium object will not be 
 * able to do anything.
 *
 * \thread_xcb
 */
void Xenium::eventLoop(std::stop_token thread_token ///< Used to signal thread termination
	, Xenium*                      xenium       ///< The Xenium instance to use
	) noexcept
{
	const int Randr_Notify_Event =
		xenium->randr_event_base + XCB_RANDR_NOTIFY;

	xcb_generic_event_t* event = nullptr;

	xenium->event_loop_is_running.store(true);

	// Handle events and render window contents
	while(thread_token.stop_requested() == false)
	{
		// Process all events
		while(true)
		{
			event = xcb_poll_for_event(xenium->connection);

			if(event == nullptr)
			{
				break;
			}

			event->response_type &= 0x7f;

			//printf("Event Type: %d\n", event->response_type);
			//printf("Base: %d\n", xenium->randr_event_base);
			//printf("Root: %08x\n", xenium->screen->root);

			if(event->response_type == Randr_Notify_Event)
			{
				xenium->randrEvent(
					(xcb_randr_notify_event_t*)event
					);
			}

			free(event);
		}


		// Update and render windows
		/*
		for(unsigned int i = 0; i < windows_to_close.size(); i++)
		{
			const uint32_t window_id = windows_to_close[i];

			delete XcbWindow::window(window_id);
		}

		windows_to_close.clear();
		*/

		std::this_thread::yield();
	}

	xenium->event_loop_is_running.store(false);
}

// }}}
// {{{ Output

/**
 * \brief Get a copy of the Output information.
 *
 * All the available information about an output device will be provided.  It 
 * is possible that an output device is removed after the data has been 
 * retrieved, so consider the provided data as a "snapshot in time".
 *
 * If the provided \p output_id is not valid, then the returned data will be 
 * empty.
 *
 * \return The Output data.
 *
 * \thread_user
 */
Xenium::Output Xenium::output(const Xenium::OutputId output_id
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.map.contains(output_id) == false)
	{
		ZAKERO_XENIUM__DEBUG
			<< "Invalid output_id: "
			<< std::to_string(output_id)
			;

		return {};
	}

	return output_data.map.at(output_id);
}


/**
 * \brief Get a list of the Output Id's.
 *
 * All the Output IDs will be returned.  It is possible that an output device 
 * is removed after the data has been retrieved, so consider the provided data 
 * as a "snapshot in time".
 *
 * \return The Output Id's.
 *
 * \thread_user
 */
Xenium::VectorOutputId Xenium::outputVector() const noexcept
{
	Xenium::VectorOutputId vector;

	std::lock_guard<std::mutex> lock(output_data.mutex);

	for(const auto& iter : output_data.map)
	{
		vector.push_back(iter.first);
	}

	return vector;
}


/**
 * \brief Get a human readable string.
 *
 * The `Xenium::Output::subpixel` is an XCB enum value and this method will 
 * convert that value into a descriptive name string.  If an invalid value is 
 * passed, then an empty string will be returned.
 *
 * \return The name of the subpixel format.
 *
 * \thread_user
 */
std::string Xenium::outputSubpixelName(int32_t subpixel_format ///< The Subpixel format
	) noexcept
{
	switch(subpixel_format)
	{
#define X(value_, name_) \
		case value_: return name_;
		ZAKERO_XENIUM__OUTPUT_SUBPIXEL
#undef X
		default: return "";
	}
}


/**
 * \brief Convert Pixel to Millimeter.
 *
 * Convert a pixel-based \p point location to millimeters using the provided \p 
 * output_id.
 *
 * \return A point that uses millimeters.
 *
 * \thread_user
 */
Xenium::PointMm Xenium::outputConvertToMm(const Xenium::OutputId output_id ///< The output device for the conversion
	, const Xenium::PointPixel&                              point     ///< The point to convert
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.map.contains(output_id) == false)
	{
		return { point.time, 0, 0 };
	}

	const Xenium::Output& output = output_data.map.at(output_id);

	auto p = convertPixelToMm(output, point.x, point.y);

	return { point.time, p.first, p.second };
}


/**
 * \brief Convert Pixel to a Percentage.
 *
 * Convert a pixel-based \p point location to a percentage using the provided 
 * \p output_id.
 *
 * \return A point that uses a percentage.
 *
 * \thread_user
 */
Xenium::PointPercent Xenium::outputConvertToPercent(const Xenium::OutputId output_id ///< The output device for the conversion
	, const Xenium::PointPixel&                                        point     ///< The point to convert
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.map.contains(output_id) == false)
	{
		return { point.time, 0, 0 };
	}

	const Xenium::Output& output = output_data.map.at(output_id);

	auto p = convertPixelToPercent(output, point.x, point.y);

	return { point.time, p.first, p.second };
}


/**
 * \brief Convert Millimeter to Pixel.
 *
 * Convert a millimeter-based \p point location to pixel using the provided \p 
 * output_id.
 *
 * \return A point that uses pixels.
 *
 * \thread_user
 */
Xenium::PointPixel Xenium::outputConvertToPixel(const Xenium::OutputId output_id ///< The output device for the conversion
	, const Xenium::PointMm&                                       point     ///< The point to convert
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.map.contains(output_id) == false)
	{
		return { point.time, 0, 0 };
	}

	const Xenium::Output& output = output_data.map.at(output_id);

	auto p = convertMmToPixel(output, point.x, point.y);

	return { point.time, p.first, p.second };
}


/**
 * \brief Convert Percentage to Pixel.
 *
 * Convert a percentage-based \p point location to pixel using the provided \p 
 * output_id.
 *
 * \return A point that uses pixels.
 *
 * \thread_user
 */
Xenium::PointPixel Xenium::outputConvertToPixel(const Xenium::OutputId output_id ///< The output device for the conversion
	, const Xenium::PointPercent&                                  point     ///< The point to convert
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.map.contains(output_id) == false)
	{
		return { point.time, 0, 0 };
	}

	const Xenium::Output& output = output_data.map.at(output_id);

	auto p = convertPercentToPixel(output, point.x, point.y);

	return { point.time, p.first, p.second };
}


/**
 * \brief Convert Pixel to Millimeter.
 *
 * Convert a pixel-based \p size to millimeters using the provided \p 
 * output_id.
 *
 * \return A size that uses millimeters.
 *
 * \thread_user
 */
Xenium::SizeMm Xenium::outputConvertToMm(const Xenium::OutputId output_id ///< The output device for the conversion
	, const Xenium::SizePixel&                              size      ///< The size to convert
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.map.contains(output_id) == false)
	{
		return { 0, 0 };
	}

	const Xenium::Output& output = output_data.map.at(output_id);

	auto p = convertPixelToMm(output, size.width, size.height);

	return { p.first, p.second };
}


/**
 * \brief Convert Pixel to a Percentage.
 *
 * Convert a pixel-based \p size to a percentage using the provided \p 
 * output_id.
 *
 * \return A size that uses a percentage.
 *
 * \thread_user
 */
Xenium::SizePercent Xenium::outputConvertToPercent(const Xenium::OutputId output_id ///< The output device for the conversion
	, const Xenium::SizePixel&                                        size      ///< The size to convert
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.map.contains(output_id) == false)
	{
		return { 0, 0 };
	}

	const Xenium::Output& output = output_data.map.at(output_id);

	auto p = convertPixelToPercent(output, size.width, size.height);

	return { p.first, p.second };
}


/**
 * \brief Convert Millimeter to Pixel.
 *
 * Convert a millimeter-based \p size to pixel using the provided \p output_id.
 *
 * \return A size that uses pixels.
 *
 * \thread_user
 */
Xenium::SizePixel Xenium::outputConvertToPixel(const Xenium::OutputId output_id ///< The output device for the conversion
	, const Xenium::SizeMm&                                       size      ///< The size to convert
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.map.contains(output_id) == false)
	{
		return { 0, 0 };
	}

	const Xenium::Output& output = output_data.map.at(output_id);

	auto p = convertMmToPixel(output, size.width, size.height);

	return { p.first, p.second };
}


/**
 * \brief Convert Percentage to Pixel.
 *
 * Convert a percentage-based \p size to pixel using the provided \p output_id.
 *
 * \return A size that uses pixels.
 *
 * \thread_user
 */
Xenium::SizePixel Xenium::outputConvertToPixel(const Xenium::OutputId output_id ///< The output device for the conversion
	, const Xenium::SizePercent&                                  size      ///< The size to convert
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.map.contains(output_id) == false)
	{
		return { 0, 0 };
	}

	const Xenium::Output& output = output_data.map.at(output_id);

	auto p = convertPercentToPixel(output, size.width, size.height);

	return { p.first, p.second };
}


/**
 * \brief Notification of adding an Output device.
 *
 * When a Wayland output device has been added, the \p lambda that was provided 
 * to this method will be called.  To disable these notifications, pass 
 * `nullptr` as the value of \p lambda.
 *
 * \thread_user
 */
void Xenium::outputOnAdd(LambdaOutputId lambda ///< The lambda to call
	) noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(lambda == nullptr)
	{
		output_data.on_add = LambdaOutputId_DoNothing;
	}
	else
	{
		output_data.on_add = lambda;
	}
}


/**
 * \brief Notification that an Output device has changed.
 *
 * When a Wayland output device's configuration has been changed, the \p lambda 
 * that was provided to this method will be called.  To disable these 
 * notifications, pass `nullptr` as the value of \p lambda.
 *
 * \thread_user
 */
void Xenium::outputOnChange(LambdaOutputId lambda ///< The lambda to call
	) noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(lambda == nullptr)
	{
		output_data.on_change = LambdaOutputId_DoNothing;
	}
	else
	{
		output_data.on_change = lambda;
	}
}


/**
 * \brief Notification of removing an Output device.
 *
 * When a Wayland output device has been removed, the \p lambda that was 
 * provided to this method will be called.  To disable these notifications, 
 * pass `nullptr` as the value of \p lambda.
 *
 * \thread_user
 */
void Xenium::outputOnRemove(LambdaOutputId lambda ///< The lambda to call
	) noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(lambda == nullptr)
	{
		output_data.on_remove = LambdaOutputId_DoNothing;
	}
	else
	{
		output_data.on_remove = lambda;
	}
}


/**
 * \brief Initialize the Output objects.
 *
 * Query the XCB RandR interface to get all the currently known output devices 
 * and create an Output object for each device.
 *
 * \return The error code.
 */
std::error_code Xenium::outputInit() noexcept
{
	xcb_generic_error_t* xcb_generic_error = nullptr;

	xcb_randr_get_screen_resources_current_reply_t* screen_resources =
		xcb_randr_get_screen_resources_current_reply(this->connection
			, xcb_randr_get_screen_resources_current(this->connection
				, this->screen->root
				)
			, &xcb_generic_error
			);
	
	if(screen_resources == nullptr)
	{
		return ZAKERO_XENIUM__ERROR(Error_RandR_Screen_Resources_Not_Found);
	}

	xcb_randr_output_t* output_list =
		xcb_randr_get_screen_resources_current_outputs(screen_resources
			);
	
	int output_list_size =
		xcb_randr_get_screen_resources_current_outputs_length(screen_resources
			);

	for(int i = 0; i < output_list_size; i++)
	{
		xcb_randr_get_output_info_reply_t* output_info =
			xcb_randr_get_output_info_reply(this->connection
				, xcb_randr_get_output_info(this->connection
					, output_list[i]
					, screen_resources->config_timestamp
					)
				, &xcb_generic_error
				);

		if(output_info == nullptr
			|| output_info->connection != XCB_RANDR_CONNECTION_CONNECTED
			|| output_info->crtc == XCB_NONE
			|| output_info->mm_width == 0
			|| output_info->mm_height == 0
			)
		{
			// Output Info is not usable
			ZAKERO_FREE(output_info);
			continue;
		}

		xcb_randr_get_crtc_info_reply_t* crtc_info =
			xcb_randr_get_crtc_info_reply(this->connection
				, xcb_randr_get_crtc_info(this->connection
					, output_info->crtc
					, screen_resources->config_timestamp
					)
				, &xcb_generic_error
				);

		if(crtc_info == nullptr)
		{
			ZAKERO_FREE(crtc_info);
			ZAKERO_FREE(output_info);
			continue;
		}

		outputAdd(crtc_info, output_info);

		ZAKERO_FREE(crtc_info);
		ZAKERO_FREE(output_info);
	}

	ZAKERO_FREE(screen_resources);

	return ZAKERO_XENIUM__ERROR(Error_None);
}


/**
 * \brief Add a new Output device
 *
 * Using the provided \p randr_crtc Id and \p randr_output Id, create a new 
 * Outpup object.  If the Output object already exists, it will be replaced 
 * with the new data.
 *
 * \return The error code.
 */
std::error_code Xenium::outputAdd(xcb_randr_crtc_t randr_crtc   ///< The RandR CRTC Id
	, xcb_randr_output_t                       randr_output ///< The RandR Output Id
	) noexcept
{
	if(randr_crtc == XCB_NONE)
	{
		return ZAKERO_XENIUM__ERROR(Error_RandR_Invalid_CRTC_Id);
	}

	if(randr_output == XCB_NONE)
	{
		return ZAKERO_XENIUM__ERROR(Error_RandR_Invalid_Output_Id);
	}

	xcb_generic_error_t* xcb_generic_error = nullptr;

	xcb_randr_get_screen_resources_current_reply_t* screen_resources =
		xcb_randr_get_screen_resources_current_reply(this->connection
			, xcb_randr_get_screen_resources_current(this->connection
				, this->screen->root
				)
			, &xcb_generic_error
			);

	if(screen_resources == nullptr)
	{
		return ZAKERO_XENIUM__ERROR(Error_RandR_Screen_Resources_Not_Found);
	}

	xcb_randr_get_output_info_reply_t* output_info =
		xcb_randr_get_output_info_reply(this->connection
			, xcb_randr_get_output_info(this->connection
				, randr_output
				, screen_resources->config_timestamp
				)
			, &xcb_generic_error
			);

	if(output_info == nullptr)
	{
		ZAKERO_FREE(screen_resources);

		return ZAKERO_XENIUM__ERROR(Error_RandR_Output_Info_Not_Found);
	}

	if(output_info->mm_width == 0
		|| output_info->mm_height == 0
		)
	{
		ZAKERO_FREE(output_info);
		ZAKERO_FREE(screen_resources);

		return ZAKERO_XENIUM__ERROR(Error_RandR_Output_Info_Is_Incomplete);
	}

	xcb_randr_get_crtc_info_reply_t* crtc_info =
		xcb_randr_get_crtc_info_reply(this->connection
			, xcb_randr_get_crtc_info(this->connection
				, randr_crtc
				, screen_resources->config_timestamp
				)
			, &xcb_generic_error
			);

	if(crtc_info == nullptr)
	{
		ZAKERO_FREE(output_info);
		ZAKERO_FREE(screen_resources);

		return ZAKERO_XENIUM__ERROR(Error_RandR_CRTC_Info_Not_Found);
	}

	outputAdd(crtc_info, output_info);

	ZAKERO_FREE(crtc_info);
	ZAKERO_FREE(output_info);
	ZAKERO_FREE(screen_resources);

	return ZAKERO_XENIUM__ERROR(Error_None);
}


/**
 * \brief Add an output device.
 *
 * Using the provided \p crtc_info and \p output_info data, create a new Output 
 * object.
 */
void Xenium::outputAdd(const xcb_randr_get_crtc_info_reply_t* crtc_info   ///< CRTC Information
	, const xcb_randr_get_output_info_reply_t*            output_info ///< Output Information
	) noexcept
{
	const std::string output_name(
		(const char*)xcb_randr_get_output_info_name(output_info),
		xcb_randr_get_output_info_name_length(output_info)
		);

	OutputId output_id = output_info->crtc;

	this->output_data.map[output_id] =
	{	.name                     = output_name
	//,	.make                     = ""
	//,	.model                    = ""
	,	.x                        = crtc_info->x
	,	.y                        = crtc_info->y
	,	.width                    = crtc_info->width
	,	.height                   = crtc_info->height
	,	.physical_width_mm        = output_info->mm_width
	,	.physical_height_mm       = output_info->mm_height
	,	.subpixel                 = output_info->subpixel_order
	//,	.refresh_mHz              = 0 // ????
	//,	.scale_factor             = 0 // ????
	,	.transform                = crtc_info->rotation
	,	.pixels_per_mm_horizontal = (float)crtc_info->width  / output_info->mm_width
	,	.pixels_per_mm_vertical   = (float)crtc_info->height / output_info->mm_height
	};
}


// }}}
// {{{ Window

/**
 * \enum Xenium::WindowDecorations
 *
 * \brief Who is responsible for rendering the decorations.
 *
 * \var Xenium::Client_Side
 * \brief The user app must draw the decorations.
 *
 * \var Xenium::Server_Side
 * \brief The X11 server will draw the decorations.
 */

/**
 * \enum Xenium::WindowMode
 *
 * All the available window modes.
 *
 * \var Xenium::Normal
 * \brief A normal window.
 *
 * \var Xenium::Fullscreen
 * \brief A window that uses the entire screen, no borders.
 *
 * \var Xenium::Maximized
 * \brief A window that uses as much of the screen as possible.
 */


/**
 * \internal
 *
 * \struct WindowData
 *
 * \brief Data to create a Window
 *
 * A container to hold all the data required to create a window.
 */


/**
 * \name Lambdas
 * \{
 */

/**
 * \typedef Xenium::Lambda
 *
 * \brief A Lambda that has no parameters.
 */

/**
 * \typedef Xenium::LambdaKey
 *
 * \brief A Lambda that has parameters: Key and KeyModifier.
 */

/**
 * \typedef Xenium::LambdaAxis
 *
 * \brief A Lambda that has parameters: PointerAxis and KeyModifier.
 */

/**
 * \typedef Xenium::LambdaButtonMm
 *
 * \brief A Lambda that has parameters: PointerButton, PointMm and KeyModifier.
 */

/**
 * \typedef Xenium::LambdaButtonPercent
 *
 * \brief A Lambda that has parameters: PointerButton, PointPercent and 
 * KeyModifier.
 */

/**
 * \typedef Xenium::LambdaButtonPixel
 *
 * \brief A Lambda that has parameters: PointerButton, PointPixel and 
 * KeyModifier.
 */

/**
 * \typedef Xenium::LambdaPointMm
 *
 * \brief A Lambda that has parameters: PointMm and KeyModifier.
 */

/**
 * \typedef Xenium::LambdaPointPercent
 *
 * \brief A Lambda that has parameters: PointPercent and KeyModifier.
 */

/**
 * \typedef Xenium::LambdaPointPixel
 *
 * \brief A Lambda that has parameters: PointPixel and KeyModifier.
 */

/**
 * \typedef Xenium::LambdaBool
 *
 * \brief A Lambda that has a parameter: bool.
 */

/**
 * \typedef Xenium::LambdaWindowDecorations
 *
 * \brief A Lambda that has a parameter: WindowDecorations.
 */

/**
 * \typedef Xenium::LambdaWindowMode
 *
 * \brief A Lambda that has a parameter: WindowMode.
 */

/**
 * \typedef Xenium::LambdaSizeMm
 *
 * \brief A Lambda that has a parameter: SizeMm.
 */

/**
 * \typedef Xenium::LambdaSizePercent
 *
 * \brief A Lambda that has a parameter: SizePercent.
 */

/**
 * \typedef Xenium::LambdaSizePixel
 *
 * \brief A Lambda that has a parameter: SizePixel.
 */

/**
 * \}
 */


/**
 * \brief Create a window.
 *
 * Create a new Window of the specified \p size.  A pointer to the Window will 
 * be returned.  If there was a problem, a `nullptr` will returned.
 *
 * The \p error parameter will be set to Xenium::Error_None on success or to an 
 * appropriate error if there was a problem.
 *
 * \note The size of a window __must__ be greater than `0`.
 *
 * \return A pointer to the new Window or `nullptr` on error.
 */
Xenium::Window* Xenium::windowCreate(const Xenium::SizeMm& size  ///< The window size
	, std::error_code&                                 error ///< The error state
	) noexcept
{
	ZAKERO_UNUSED(size);

	error = ZAKERO_XENIUM__ERROR(Error_None);

	return nullptr;
}


/**
 * \brief Create a window.
 *
 * Create a new Window of the specified \p size.  A pointer to the Window will 
 * be returned.  If there was a problem, a `nullptr` will returned.
 *
 * The \p error parameter will be set to Xenium::Error_None on success or to an 
 * appropriate error if there was a problem.
 *
 * \note The size of a window __must__ be greater than `0`.
 *
 * \return A pointer to the new Window or `nullptr` on error.
 */
Xenium::Window* Xenium::windowCreate(const Xenium::SizePercent& size  ///< The window size
	, std::error_code&                                      error ///< The error state
	) noexcept
{
	ZAKERO_UNUSED(size);

	error = ZAKERO_XENIUM__ERROR(Error_None);

	return nullptr;
}


/**
 * \brief Create a window.
 *
 * Create a new Window of the specified \p size.  A pointer to the Window will 
 * be returned.  If there was a problem, a `nullptr` will returned.
 *
 * The \p error parameter will be set to Xenium::Error_None on success or to an 
 * appropriate error if there was a problem.
 *
 * \note The size of a window __must__ be greater than `0`.
 *
 * \return A pointer to the new Window or `nullptr` on error.
 */
Xenium::Window* Xenium::windowCreate(const Xenium::SizePixel& size  ///< The window size
	, std::error_code&                                    error ///< The error state
	) noexcept
{
	ZAKERO_UNUSED(size);

	error = ZAKERO_XENIUM__ERROR(Error_None);

	return nullptr;
}

// }}}
// {{{ Window : Documentation

/**
 * \class Xenium::Window
 *
 * \brief A %Window
 *
 * The %Window is the real work-horse of Xenium.  This is the object that the 
 * user sees and interacts with.  The %Window API is rather straight forward 
 * and unsurprising, but there are some details to be aware of.
 *
 * \par Decorations
 * \parblock
 * Decorations are the window border, the title, and other things like the 
 * close button.  These items are separate from the window itself in that a 
 * window is just a rectangular area on-screen.
 *
 * Currently, there are two schools of thought, each with pro's and con's:
 * -# __Client-Side Decorations__: The application should be responsible for 
 * rendering its own decorations.
 *    - Pro: The application knows best how to present decorations in a way 
 *    that best fits it User Interface.
 *    - Con: No consistent Look-And-Feel across all applications.
 *    <br><br>
 * -# __Server-Side Decorations__: The Desktop Environment is responsible for 
 * rendering the window decorations.
 *    - Pro: Interacting with windows in consistent across all applications and 
 *    windows are only responsible for there rectangular area on-screen.
 *    - Con: The Desktop Environment's window decorations may visually clash 
 *    with the application's User Interface and/or provide redundant controls 
 *    (like have two ways of closing the application).
 *
 * Because of this conflict, window decorations are still non-standard and vary 
 * in almost every Desktop Environment.
 * \endparblock
 *
 * \par Rendering
 * \parblock
 * Updating the contents of the window is a two-step process.  The first step 
 * is to get an "image" from the window (Xenium::Window::imageNext()).  The 
 * %Window will provide a pointer to it's internal graphics buffer, which can 
 * then be used for rendering.
 *
 * How do you do this rendering?  Well that is up to you, _Zakero %Xenium_ does 
 * not provide any graphics API.  You can write your own graphics library (not 
 * that hard), or use one that can accept a pointer and use it.  For (a bad) 
 * example: Qt5's QImage can be used with a raw data pointer.
 *
 * After writing all the required data to the "image", the second step is to 
 * tell the %Window to present the image (Xenium::Window::imagePresent()).  
 * Then the %Window will tell the X11 server to update the window contents 
 * on-screen.
 *
 * Why the method names "imageNext()" and "imagePresent()"?  That is to match 
 * the same language that the _Vulkan_ API uses.  _Zakero %Xenium_ is not 
 * compatible with _Vulkan_ at this point in time.
 * \endparblock
 *
 * \par Cursors
 * \parblock
 *
 * \endparblock
 *
 * \par Focus
 * \parblock
 *
 * \endparblock
 *
 * \internal
 *
 * The %Window object just binds together data that is in the Xenium object.  
 * Since %Window is a sub-class of Xenium, it has full access to Xenium's 
 * private methods.
 */

// }}}
// {{{ Window : Constructor / Destructor

/**
 * \brief Construct a %Window.
 *
 * Using the data pointed to by \p ptr, create a new %Window instance.
 *
 * __This constructor is not intended to be used.  The correct way to create a 
 * %Window is to use one of these methods:__
 * - Xenium::windowCreate(const Xenium::SizeMm&, std::error_code&)
 * - Xenium::windowCreate(const Xenium::SizeMm&, const wl_shm_format)
 * - Xenium::windowCreate(const Xenium::SizeMm&, const wl_shm_format, std::error_code&)
 * - Xenium::windowCreate(const Xenium::SizePercent&, std::error_code&)
 * - Xenium::windowCreate(const Xenium::SizePercent&, const wl_shm_format)
 * - Xenium::windowCreate(const Xenium::SizePercent&, const wl_shm_format, std::error_code&)
 * - Xenium::windowCreate(const Xenium::SizePixel&, std::error_code&)
 * - Xenium::windowCreate(const Xenium::SizePixel&, const wl_shm_format)
 * - Xenium::windowCreate(const Xenium::SizePixel&, const wl_shm_format, std::error_code&)
 *
 * \internal
 *
 * The \p ptr is the Xenium::WindowData in disguise.  The constructor will use 
 * the WindowData to create the requested window.  If there is an error during 
 * construction, %Window will be in an unknown state and the 
 * Xenium::WindowData.error will be set appropriately.
 *
 * It is expected that the caller will check the WindowData error and delete 
 * the incomplete %Window if needed.
 */
Xenium::Window::Window(
	)
{
}


/**
 * \brief Destroy a %Window.
 */
Xenium::Window::~Window()
{
}

// }}}
// {{{ Window : Cursor

// }}}
// {{{ Window : Settings

/**
 * \brief Change the window class.
 *
 * The \p app_id of a window is a name that is used to group windows which the 
 * Desktop Environment may be able to use.  An example of this grouping would 
 * be give all the windows a \p app_id of the application name.  Another 
 * example would be to give a "file browser" \p app_id to a window that allows 
 * the user to navigate the file system.
 *
 * It is suggested to use a \p app_id that matches the basename of the 
 * application's .desktop file.  For example, "org.freedesktop.FooViewer" where 
 * the .desktop file is "org.freedesktop.FooViewer.desktop".
 *
 * \note See http://standards.freedesktop.org/desktop-entry-spec for more 
 * details.
 */
void Xenium::Window::setClass(const std::string& class_name ///< The class name
	) noexcept
{
	ZAKERO_UNUSED(class_name);
}


/**
 * \brief Change the window title.
 *
 * The window's title can be changed by using this method.  Changing the title 
 * does not change the window's name.
 */
void Xenium::Window::setTitle(const std::string& title ///< The window title
	) noexcept
{
	ZAKERO_UNUSED(title);
}


/**
 * \brief Use the Desktop Environment borders.
 *
 * Using this method will inform the %Wayland Compositor that the window would 
 * like to use the "system borders" of the desktop environment by setting \p 
 * use_system_borders to `true`.  Or by setting \p use_system_borders to 
 * `false' the Compositor expect the window to provide its own title and 
 * borders or just be a borderlass window.
 * 
 * Even if \p use_system_borders is `true`, it may be possible for the Desktop 
 * Environment to hide the title and borders of the window.
 *
 * \note Not all %Wayland Compositors support this functionality.  Those 
 * Compositors that do not support it, expect the window provide its own title 
 * and borders.
 *
 * \return An error code.  If there was no error, then `error_code.value() == 
 * 0`.
 */
std::error_code Xenium::Window::setDecorations(const Xenium::WindowDecorations decorations ///< The requested decorations
	) noexcept
{
	ZAKERO_UNUSED(decorations);

	return ZAKERO_XENIUM__ERROR(Error_None);
}


/**
 * \brief Get the current WindowMode
 *
 * Provides the current WindowMode of the %Window.
 *
 * \return Xenium::WindowMode
 */
Xenium::WindowMode Xenium::Window::windowMode() noexcept
{
	return WindowMode::Normal;
}


/**
 * \brief Check the WindowMode
 *
 * Compare the provided \p window_mode with the current window mode.
 *
 * \retval true  The WindowMode matches
 * \retval false The WindowMode's are different
 */
bool Xenium::Window::windowModeIs(const Xenium::WindowMode window_mode ///< The WindowMode
	) noexcept
{
	ZAKERO_UNUSED(window_mode);

	return false;
}


/**
 * \brief Change the window mode.
 *
 * The current mode of a window can be changed programmatically by using this 
 * method.
 *
 * \see Xenium::WindowMode
 */
void Xenium::Window::windowModeSet(const Xenium::WindowMode window_mode ///< The WindowMode
	) noexcept
{
	ZAKERO_UNUSED(window_mode);

	return;
}


/**
 * \brief Set the window size.
 *
 * The window will be resized to the requested \p size.
 * 
 * Changing the size in this manner ignores the %Window's minimum and maximum 
 * size settings.  This can result in strange behavior when a user then 
 * attempts to manually resize the %Window.
 *
 * \note The size of a window __must__ be greater than `0`.
 *
 * \note This method does __not__ trigger the Resize Event.
 *
 * \return An error code.  If there was no error, then `error_code.value() == 
 * 0`.
 */
std::error_code Xenium::Window::setSize(const Xenium::SizeMm& size ///< The %Window size
	) noexcept
{
	ZAKERO_UNUSED(size);

	return ZAKERO_XENIUM__ERROR(Error_None);
}


/**
 * \brief Set the window size.
 *
 * The window will be resized to the requested \p size.
 * 
 * Changing the size in this manner ignores the %Window's minimum and maximum 
 * size settings.  This can result in strange behavior when a user then 
 * attempts to manually resize the %Window.
 *
 * \note The size of a window __must__ be greater than `0`.
 *
 * \note This method does __not__ trigger the Resize Event.
 *
 * \return An error code.  If there was no error, then `error_code.value() == 
 * 0`.
 */
std::error_code Xenium::Window::setSize(const Xenium::SizePercent& size ///< The %Window size
	) noexcept
{
	ZAKERO_UNUSED(size);

	return ZAKERO_XENIUM__ERROR(Error_None);
}


/**
 * \brief Set the window size.
 *
 * The window will be resized to the requested \p size.
 * 
 * Changing the size in this manner ignores the %Window's minimum and maximum 
 * size settings.  This can result in strange behavior when a user then 
 * attempts to manually resize the %Window.
 *
 * \note The size of a window __must__ be greater than `0`.
 *
 * \note This method does __not__ trigger the Resize Event.
 *
 * \return An error code.  If there was no error, then `error_code.value() == 
 * 0`.
 */
std::error_code Xenium::Window::setSize(const Xenium::SizePixel& size ///< The %Window size
	) noexcept
{
	ZAKERO_UNUSED(size);

	return ZAKERO_XENIUM__ERROR(Error_None);
}


/**
 * \brief Set the minimum window size.
 *
 * The window will be restricted to the provided \p size_min and \p size_max.
 *
 * If either the width or height values in \p size_min or \p size_max are `0`, 
 * then the size restriction for that dimension will be disabled.
 *
 * \return An error code.  If there was no error, then `error_code.value() == 
 * 0`.
 */
std::error_code Xenium::Window::setSizeMinMax(const Xenium::SizeMm& size_min ///< The minimum size
	, const Xenium::SizeMm&                                     size_max ///< The maximum size
	) noexcept
{
	ZAKERO_UNUSED(size_min);
	ZAKERO_UNUSED(size_max);

	return ZAKERO_XENIUM__ERROR(Error_None);
}


/**
 * \brief Set the minimum window size.
 *
 * The window will be restricted to the provided \p size_min and \p size_max.
 *
 * If either the width or height values in \p size_min or \p size_max are `0`, 
 * then the size restriction for that dimension will be disabled.
 *
 * \return An error condition.  If there was no error, then 
 * `error_code.value() == 0`.
 */
std::error_code Xenium::Window::setSizeMinMax(const Xenium::SizePercent& size_min ///< The minimum size
	, const Xenium::SizePercent&                                     size_max ///< The maximum size
	) noexcept
{
	ZAKERO_UNUSED(size_min);
	ZAKERO_UNUSED(size_max);

	return ZAKERO_XENIUM__ERROR(Error_None);
}


/**
 * \brief Set the minimum window size.
 *
 * The window will be restricted to the provided \p size_min and \p size_max.
 *
 * If either the width or height values in \p size_min or \p size_max are `0`, 
 * then the size restriction for that dimension will be disabled.
 *
 * \return An error condition.  If there was no error, then 
 * `error_code.value() == 0`.
 */
std::error_code Xenium::Window::setSizeMinMax(const Xenium::SizePixel& size_min ///< The minimum size
	, const Xenium::SizePixel&                                     size_max ///< The maximum size
	) noexcept
{
	ZAKERO_UNUSED(size_min);
	ZAKERO_UNUSED(size_max);

	return ZAKERO_XENIUM__ERROR(Error_None);
}

// }}}
// {{{ Window : Rendering

/**
 * \brief Get an image buffer.
 *
 * To change the contents of the %Window, the image data must be update.  This 
 * method will provide access to the %Windows image data via the \p image 
 * pointer.  The image data will have the same pixel format that was used when 
 * the %Window was created.  If the source graphic data is in a different pixel 
 * format, it must be translated to the %Window's pixel format.
 *
 * The \p size parameter is the width and height of the image data in pixels.  
 * The total length of the \p image data in bytes is:
 * > `size.width * size.height * Window->bytesPerPixel()`
 *
 * To index into the \p image data the following formula is used:
 * > `image[(size.width * y) + x] = pixel_color`
 * 
 * The contents of the data pointed to by \p image is undefined and may contain 
 * "garbage".
 *
 * \parcode
 * // Pixel Format: ARGB8888 -- 32-bit, 4 bytes per pixel
 * uint32_t* image = nullptr;
 * Xenium::SizePixel size;
 *
 * window->imageNext((uint8_t*)image, size);
 *
 * uint32_t color = 0x00ffffff; // White
 * uint32_t x = 44;
 * uint32_t y = 22;
 *
 * uint32_t pixel = (y * size.width) + x;
 * image[pixel] = color;
 * \endparcode
 *
 * \return An error code.
 */
std::error_code Xenium::Window::imageNext(uint8_t*& image ///< The image data
	, Xenium::SizePixel&                        size  ///< The image size
	) noexcept
{
	ZAKERO_UNUSED(image);
	ZAKERO_UNUSED(size);

	return ZAKERO_XENIUM__ERROR(Error_None);
}


/**
 * \brief Render the image.
 *
 * Once the image data has been updated, this method will schedule the data to 
 * be rendered.
 */
void Xenium::Window::imagePresent() noexcept
{
}


/**
 * \brief When the last frame was rendered.
 *
 * Access the time, in milliseconds, of most recent window update.  The delta 
 * between two window time stamps can be used to determine the 
 * Frames-Per-Second.
 *
 * \note This is not based on wall-time.
 *
 * \return An error code.  If there was no error, then `error_code.value() == 
 * 0`.
 */
uint32_t Xenium::Window::time() const noexcept
{
	return 0;
}

// }}}
// {{{ Window : Misc

/**
 * \brief Get the number of bytes per pixel.
 *
 * The number of bytes required to represent one pixel is provided by this 
 * method.
 *
 * \return The number of bytes per pixel.
 */
uint8_t Xenium::Window::bytesPerPixel() const noexcept
{
	return 0;
}


/**
 * \brief Minimize the window.
 *
 * Using this method will remove the window from view.  The user will have to 
 * use the Desktop Environment to have the window redisplayed, perhaps using 
 * the task viewer.
 */
void Xenium::Window::minimize() noexcept
{
}

// }}}
// {{{ Window : Measurement Conversion

/**
 * \brief Unit conversion.
 *
 * The provided \p point will be converted to millimeters.
 *
 * \return The converted point.
 */
Xenium::PointMm Xenium::Window::convertToMm(const Xenium::PointPixel& point ///< The point to convert
	) const noexcept
{
	ZAKERO_UNUSED(point);

	return {};
}


/**
 * \brief Unit conversion.
 *
 * The provided \p point will be converted to a percentage.
 *
 * \return The converted point.
 */
Xenium::PointPercent Xenium::Window::convertToPercent(const Xenium::PointPixel& point ///< The point to convert
	) const noexcept
{
	ZAKERO_UNUSED(point);

	return {};
}


/**
 * \brief Unit conversion.
 *
 * The provided \p point will be converted to pixels.
 *
 * \return The converted point.
 */
Xenium::PointPixel Xenium::Window::convertToPixel(const Xenium::PointMm& point ///< The point to convert
	) const noexcept
{
	ZAKERO_UNUSED(point);

	return {};
}


/**
 * \brief Unit conversion.
 *
 * The provided \p point will be converted to pixels.
 *
 * \return The converted point.
 */
Xenium::PointPixel Xenium::Window::convertToPixel(const Xenium::PointPercent& point ///< The point to convert
	) const noexcept
{
	ZAKERO_UNUSED(point);

	return {};
}


/**
 * \brief Unit conversion.
 *
 * The provided \p size will be converted to millimeters.
 *
 * \return The converted size.
 */
Xenium::SizeMm Xenium::Window::convertToMm(const Xenium::SizePixel& size ///< The size to convert
	) const noexcept
{
	ZAKERO_UNUSED(size);

	return {};
}


/**
 * \brief Unit conversion.
 *
 * The provided \p size will be converted to a percentage.
 *
 * \return The converted size.
 */
Xenium::SizePercent Xenium::Window::convertToPercent(const Xenium::SizePixel& size ///< The size to convert
	) const noexcept
{
	ZAKERO_UNUSED(size);

	return {};
}


/**
 * \brief Unit conversion.
 *
 * The provided \p size will be converted to pixels.
 *
 * \return The converted size.
 */
Xenium::SizePixel Xenium::Window::convertToPixel(const Xenium::SizeMm& size ///< The size to convert
	) const noexcept
{
	ZAKERO_UNUSED(size);

	return {};
}


/**
 * \brief Unit conversion.
 *
 * The provided \p size will be converted to pixels.
 *
 * \return The converted size.
 */
Xenium::SizePixel Xenium::Window::convertToPixel(const Xenium::SizePercent& size ///< The size to convert
	) const noexcept
{
	ZAKERO_UNUSED(size);

	return {};
}

// }}}
// {{{ Window : Event Handling

/**
 * \brief Respond to "Close Request" events.
 *
 * When a user requests a %Window to be close via the Desktop Environment, the 
 * Desktop Environment __may__ send an event to the window so that the 
 * application can decide how to handle the request.  The provided \p lambda 
 * will be called when the window receives a "Close Request" event.
 *
 * If a lambda has been previously set that needs to be removed, then pass a 
 * `nullptr` as the \p lambda value.
 */
void Xenium::Window::onCloseRequest(Xenium::Lambda lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Decoration Change" events.
 *
 * For the Wayland Compositors that support Server-Side Decorations, the 
 * Desktop Environment can notify a %Window whether or not it should render its 
 * own decorations.  An example of this would be a user requesting for a 
 * %Window to be border-less.  This lambda will be called when that event 
 * happens.
 */
void Xenium::Window::onDecorationsChange(Xenium::LambdaWindowDecorations lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Active" change events.
 *
 * When a window becomes active (gains focus) or becomes inactive (loses 
 * focus), an "Active" event is emitted.  The provided \p lambda will be called 
 * to handle the change of the "Active" status.
 *
 * If a lambda has been previously set that needs to be removed, then pass a 
 * `nullptr` as the \p lambda value.
 *
 * \note Execution of the lambda will block the Xenium object's event handling.  
 * So keep the lambda as small and simple as possible for best performance.
 */
void Xenium::Window::onFocusChange(Xenium::LambdaBool lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Keyboard Enter" events.
 *
 * When a window gains keyboard focus, the provided \p lambda will be called.
 */
void Xenium::Window::keyboardOnEnter(Xenium::Lambda lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Keyboard Leave" events.
 *
 * When a window loses keyboard focus, the provided \p lambda will be called.
 */
void Xenium::Window::keyboardOnLeave(Xenium::Lambda lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Keyboard Key" events.
 *
 * When a Window has _Keyboard Focus_, it will be notified of key press, key 
 * repeat, and key release events.  The provided \p lambda will be called when 
 * these events happen.
 */
void Xenium::Window::keyboardOnKey(Xenium::LambdaKey lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Window Mode" events.
 *
 * The Desktop Environment is able to change the %Window's mode.  When that 
 * event happens, the provided \p lambda will be called.
 */
void Xenium::Window::windowModeOnChange(Xenium::LambdaWindowMode lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Resize" events.
 *
 * "Resize" events are unique to Xenium.  After a window has been resized, the 
 * provided \p lambda will be called.  This event is triggered by the user 
 * manually resizing the %Window.
 *
 * If a lambda has been previously set that needs to be removed, then pass a 
 * `nullptr` as the \p lambda value.
 *
 * \note Execution of the lambda will block the Xenium object's event handling.  
 * So keep the lambda as small and simple as possible for best performance.
 */
void Xenium::Window::onSizeChange(Xenium::LambdaSizeMm lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Resize" events.
 *
 * "Resize" events are unique to Xenium.  After a window has been resized, the 
 * provided \p lambda will be called.  This event is triggered by the user 
 * manually resizing the %Window.
 *
 * If a lambda has been previously set that needs to be removed, then pass a 
 * `nullptr` as the \p lambda value.
 *
 * \note Execution of the lambda will block the Xenium object's event handling.  
 * So keep the lambda as small and simple as possible for best performance.
 */
void Xenium::Window::onSizeChange(Xenium::LambdaSizePercent lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Resize" events.
 *
 * "Resize" events are unique to Xenium.  After a window has been resized, the 
 * provided \p lambda will be called.  This event is triggered by the user 
 * manually resizing the %Window.
 *
 * If a lambda has been previously set that needs to be removed, then pass a 
 * `nullptr` as the \p lambda value.
 *
 * \note Execution of the lambda will block the Xenium object's event handling.  
 * So keep the lambda as small and simple as possible for best performance.
 */
void Xenium::Window::onSizeChange(Xenium::LambdaSizePixel lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Pointer Button" events.
 *
 * When the user clicks on a _mouse button_ or some other pointer device, it 
 * generates a "Pointer Button" event.  The provided \p lambda will be called 
 * when that event occurs.
 */
void Xenium::Window::pointerOnButton(Xenium::LambdaButtonMm lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Pointer Button" events.
 *
 * When the user clicks on a _mouse button_ or some other pointer device, it 
 * generates a "Pointer Button" event.  The provided \p lambda will be called 
 * when that event occurs.
 */
void Xenium::Window::pointerOnButton(Xenium::LambdaButtonPercent lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Pointer Button" events.
 *
 * When the user clicks on a _mouse button_ or some other pointer device, it 
 * generates a "Pointer Button" event.  The provided \p lambda will be called 
 * when that event occurs.
 */
void Xenium::Window::pointerOnButton(Xenium::LambdaButtonPixel lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Pointer Enter" events.
 *
 * When the _mouse_ or some other pointer device enters the %Window, a "Pointer 
 * Enter" event occurs.  The provided \p lambda will be called when that event 
 * happens.
 *
 * This event does not occur when the pointer enters the boarder/decoration 
 * area, but the %Window (content) itself.
 */
void Xenium::Window::pointerOnEnter(Xenium::LambdaPointMm lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Pointer Enter" events.
 *
 * When the _mouse_ or some other pointer device enters the %Window, a "Pointer 
 * Enter" event occurs.  The provided \p lambda will be called when that event 
 * happens.
 *
 * This event does not occur when the pointer enters the boarder/decoration 
 * area, but the %Window (content) itself.
 */
void Xenium::Window::pointerOnEnter(Xenium::LambdaPointPercent lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Pointer Enter" events.
 *
 * When the _mouse_ or some other pointer device enters the %Window, a "Pointer 
 * Enter" event occurs.  The provided \p lambda will be called when that event 
 * happens.
 *
 * This event does not occur when the pointer enters the boarder/decoration 
 * area, but the %Window (content) itself.
 */
void Xenium::Window::pointerOnEnter(Xenium::LambdaPointPixel lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Pointer Leave" events.
 *
 * When the _mouse_ or some other pointer device leave the %Window, a "Pointer 
 * Leave" event occurs.  The provided \p lambda will be called when that event 
 * happens.
 */
void Xenium::Window::pointerOnLeave(Xenium::Lambda lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Pointer Motion" events.
 *
 * When the _mouse_ or some other pointer device moves in the %Window, a 
 * "Pointer Motion" event occurs.  The provided \p lambda will be called when 
 * that event happens.
 */
void Xenium::Window::pointerOnMotion(Xenium::LambdaPointMm lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Pointer Motion" events.
 *
 * When the _mouse_ or some other pointer device moves in the %Window, a 
 * "Pointer Motion" event occurs.  The provided \p lambda will be called when 
 * that event happens.
 */
void Xenium::Window::pointerOnMotion(Xenium::LambdaPointPercent lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Pointer Motion" events.
 *
 * When the _mouse_ or some other pointer device moves in the %Window, a 
 * "Pointer Motion" event occurs.  The provided \p lambda will be called when 
 * that event happens.
 */
void Xenium::Window::pointerOnMotion(Xenium::LambdaPointPixel lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Pointer Axis" events.
 *
 * Some pointer devices have a "wheel" that can be rotated.  The provided \p 
 * lambda will be called when these "Pointer Axis" events happen.
 */
void Xenium::Window::pointerOnAxis(Xenium::LambdaAxis lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Pointer Axis Source" events.
 *
 * The provided \p lambda will be called when the "Pointer Axis Source" events 
 * occur.
 */
void Xenium::Window::pointerOnAxisSource(Xenium::Lambda lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Pointer Axis Stop" events.
 *
 * The provided \p lambda will be called when the "Pointer Axis Stop" events 
 * occur.
 */
void Xenium::Window::pointerOnAxisStop(Xenium::Lambda lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}


/**
 * \brief Respond to "Pointer Axis Discrete" events.
 *
 * The provided \p lambda will be called when the "Pointer Axis Discrete" 
 * events occur.
 */
void Xenium::Window::pointerOnAxisDiscrete(Xenium::Lambda lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}

// }}}
// {{{ Window : Helpers

// }}}
// {{{ XCB : RandR

/**
 * \brief Initialize the XCB RandR interface.
 *
 * After checking that the XCB RandR interface extension is available and the 
 * version is adequate, configure the events that will be used.
 *
 * \return The error code.
 */
std::error_code Xenium::randrInit() noexcept
{
	const xcb_query_extension_reply_t* randr =
		xcb_get_extension_data(this->connection, &xcb_randr_id);
	
	if(randr->present == 0)
	{
		return ZAKERO_XENIUM__ERROR(Error_RandR_Not_Available);
	}

	randr_error_base = randr->first_error;
	randr_event_base = randr->first_event;

	xcb_generic_error_t* xcb_generic_error = nullptr;

	xcb_randr_query_version_reply_t* randr_query_version =
		xcb_randr_query_version_reply(this->connection
			, xcb_randr_query_version(this->connection
				, std::numeric_limits<uint32_t>::max()
				, std::numeric_limits<uint32_t>::max()
				)
			, &xcb_generic_error
			);

	randr_query_version_major = randr_query_version->major_version;
	randr_query_version_minor = randr_query_version->minor_version;

	ZAKERO_FREE(randr_query_version);

	if(randr_query_version_major < 1
		|| (randr_query_version_major == 1
			&& randr_query_version_minor < 1
			)
		)
	{
		return ZAKERO_XENIUM__ERROR(Error_RandR_Version_Too_Old);
	}

	xcb_randr_select_input(this->connection
		, this->screen->root
		, 0
			| XCB_RANDR_NOTIFY_MASK_CRTC_CHANGE
			| XCB_RANDR_NOTIFY_MASK_OUTPUT_CHANGE
			/*  Might be of future use
			| XCB_RANDR_NOTIFY_MASK_SCREEN_CHANGE
			| XCB_RANDR_NOTIFY_MASK_OUTPUT_PROPERTY
			| XCB_RANDR_NOTIFY_MASK_PROVIDER_CHANGE
			| XCB_RANDR_NOTIFY_MASK_PROVIDER_PROPERTY
			| XCB_RANDR_NOTIFY_MASK_RESOURCE_CHANGE
			*/
		);

	return ZAKERO_XENIUM__ERROR(Error_None);
}


/**
 * \brief Handle the XCB RandR CRTC Change event
 */
void Xenium::randrEvent(const xcb_randr_crtc_change_t* event ///< The event
	) noexcept
{
	OutputId output_id = event->crtc;

	{
		std::lock_guard<std::mutex> lock(output_data.mutex);

		if(this->screen->root != event->window
			|| output_data.map.contains(output_id) == false
			)
		{
			return;
		}

		Output& output = output_data.map[output_id];

		if(output.x == event->x
			&& output.y == event->y
			&& output.width == event->width
			&& output.height == event->height
			&& output.transform == event->rotation
			)
		{
			return;
		}

		output.x         = event->x;
		output.y         = event->y;
		output.width     = event->width;
		output.height    = event->height;
		output.transform = event->rotation;

		output.pixels_per_mm_horizontal = (float)event->width  / output.physical_width_mm;
		output.pixels_per_mm_vertical   = (float)event->height / output.physical_height_mm;
	}

	output_data.on_change(output_id);
}


/**
 * \brief Handle the XCB RandR Output Change event
 */
void Xenium::randrEvent(const xcb_randr_output_change_t* event ///< The event
	) noexcept
{
	OutputId output_id  = 0;
	bool call_on_add    = false;
	bool call_on_change = false;
	bool call_on_remove = false;

	{
		std::lock_guard<std::mutex> lock(output_data.mutex);

		if(this->screen->root != event->window)
		{
			return;
		}

		output_id = event->crtc;

		if(output_data.map.contains(output_id))
		{
			Output& output = output_data.map[output_id];

			if(output.subpixel != event->subpixel_order
				|| output.transform != event->rotation
				)
			{
				call_on_change = true;
				output.subpixel  = event->subpixel_order;
				output.transform = event->rotation;
			}

			if(event->connection == XCB_RANDR_CONNECTION_DISCONNECTED)
			{
				call_on_remove = true;
				output_data.map.erase(output_id);
			}
		}
		else if(event->connection == XCB_RANDR_CONNECTION_CONNECTED)
		{
			std::error_code error = outputAdd(event->crtc, event->output);

			if(!error)
			{
				call_on_add = true;
			}
		}
	}

	if(call_on_add)
	{
		output_data.on_add(output_id);
	}

	if(call_on_change)
	{
		output_data.on_change(output_id);
	}

	if(call_on_remove)
	{
		output_data.on_remove(output_id);
	}
}


/**
 * \brief Handle the XCB RandR Notify event
 *
 * The XCB RandR Notify event is actually a wrapper for a more specific event.  
 * After parsing the \p event, pass the "sub-event" to the appropriate handler.
 */
void Xenium::randrEvent(const xcb_randr_notify_event_t* event ///< The event
	) noexcept
{
	switch(event->subCode)
	{
		case XCB_RANDR_NOTIFY_CRTC_CHANGE:
			randrEvent(&event->u.cc);
			break;
		case XCB_RANDR_NOTIFY_OUTPUT_CHANGE:
			randrEvent(&event->u.oc);
			break;
		case XCB_RANDR_NOTIFY_OUTPUT_PROPERTY:
			// Not Used
			break;
		case XCB_RANDR_NOTIFY_PROVIDER_CHANGE:
			// Not Used
			break;
		case XCB_RANDR_NOTIFY_PROVIDER_PROPERTY:
			// Not Used
			break;
		case XCB_RANDR_NOTIFY_RESOURCE_CHANGE:
			// Not Used
			break;
		case XCB_RANDR_NOTIFY_LEASE:
			// Not Used
			break;
		default:
			fprintf(stderr, "Unhandled Sub-Event %d\n", event->subCode);
	}
}


/**
 * \brief Handle the XCB RandR Screen Change Notify event
 */
void Xenium::randrEvent(const xcb_randr_screen_change_notify_event_t* event ///< The event
	) noexcept
{
	printf("Event: Randr_Screen_Change_Notify_Event\n");
	printf("t: %u\n", event->timestamp);
	printf("r: %08x\n",  event->root);
	printf("w: %08x\n",  event->request_window);
	printf("d: %ux%u\n", event->width, event->height);
	printf("m: %ux%u\n", event->mwidth, event->mheight);
}

// }}}
// {{{ Utility

/**
 * \brief Convert Pixel to Millimeter.
 *
 * Using the \p output device, convert the provided pixel values into 
 * millimeter values.
 *
 * \return The converted values.
 *
 * \thread_user
 * \thread_wayland
 */
std::pair<float, float> Xenium::convertPixelToMm(const Xenium::Output& output ///< The output for the conversion
	, int32_t                                                      xw     ///< The x or width value
	, int32_t                                                      yh     ///< The y or height value
	) const noexcept
{
	const float ratio_h = output.pixels_per_mm_horizontal;
	const float ratio_v = output.pixels_per_mm_vertical;

	return
	{	xw / ratio_h
	,	yh / ratio_v
	};
}


/**
 * \brief Convert Pixel to Percentage.
 *
 * Using the \p output device, convert the provided pixel values into 
 * percentage values.
 *
 * \return The converted values.
 *
 * \thread_user
 * \thread_wayland
 */
std::pair<float, float> Xenium::convertPixelToPercent(const Xenium::Output& output ///< The output for the conversion
	, int32_t                                                           xw     ///< The x or width value
	, int32_t                                                           yh     ///< The y or height value
	) const noexcept
{
	return
	{	float(xw) / output.width
	,	float(yh) / output.height
	};
}


/**
 * \brief Convert Millimeter to Pixel.
 *
 * Using the \p output device, convert the provided millimeter values into 
 * pixel values.
 *
 * \return The converted values.
 *
 * \thread_user
 * \thread_wayland
 */
std::pair<int32_t, int32_t> Xenium::convertMmToPixel(const Xenium::Output& output ///< The output for the conversion
	, float                                                            xw     ///< The x or width value
	, float                                                            yh     ///< The y or height value
	) const noexcept
{
	const float ratio_h = output.pixels_per_mm_horizontal;
	const float ratio_v = output.pixels_per_mm_vertical;

	return
	{	int32_t(xw * ratio_h)
	,	int32_t(yh * ratio_v)
	};
}


/**
 * \brief Convert Percentage to Pixel.
 *
 * Using the \p output device, convert the provided percentage values into 
 * pixel values.
 *
 * \return The converted values.
 *
 * \thread_user
 * \thread_wayland
 */
std::pair<int32_t, int32_t> Xenium::convertPercentToPixel(const Xenium::Output& output ///< The output for the conversion
	, float                                                                 xw     ///< The x or width value
	, float                                                                 yh     ///< The y or height value
	) const noexcept
{
	return
	{	int32_t(xw * output.width)
	,	int32_t(yh * output.height)
	};
}

// }}}
// {{{ Convenience

/**
 * \brief Convert a value to a std::string.
 *
 * The \p key_modifier data will be converted into a JSON formatted 
 * std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::KeyModifier& key_modifier ///< The value
	) noexcept
{
	auto mod_to_str = [](std::string& s, uint32_t m)
	{
		s += "[";
		std::string delim = "";

		if(m & Xenium::KeyModifier_Shift)
		{
			s += delim + "\"Shift\"";
			delim = ",";
		}

		if(m & Xenium::KeyModifier_CapsLock)
		{
			s += delim + "\"CapsLock\"";
			delim = ",";
		}

		if(m & Xenium::KeyModifier_Control)
		{
			s += delim + "\"Control\"";
			delim = ",";
		}

		if(m & Xenium::KeyModifier_Alt)
		{
			s += delim + "\"Alt\"";
			delim = ",";
		}

		if(m & Xenium::KeyModifier_Meta)
		{
			s += delim + "\"Meta\"";
		}

		s += "]";
	};

	std::string str = "{ \"pressed\": ";
	mod_to_str(str, key_modifier.pressed);

	str += ", \"latched\": ";
	mod_to_str(str, key_modifier.latched);

	str += ", \"locked\": ";
	mod_to_str(str, key_modifier.locked);

	str += " }";

	return str;
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p key_state will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::KeyState key_state ///< The value
	) noexcept
{
	switch(key_state)
	{
		case Xenium::KeyState::Pressed:  return "Pressed";
		case Xenium::KeyState::Released: return "Released";
		case Xenium::KeyState::Repeat:   return "Repeat";
		default: return "";
	}
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p output data will be converted into a JSON formatted std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::Output& output ///< The value
	) noexcept
{
	return std::string()
		+ "{\tname: \""                     + output.name + "\""
		+ "\n,\tx: "                        + std::to_string(output.x)
		+ "\n,\ty: "                        + std::to_string(output.y)
		+ "\n,\tphysical_width_mm: "        + std::to_string(output.physical_width_mm)
		+ "\n,\tphysical_height_mm: "       + std::to_string(output.physical_height_mm)
		+ "\n,\tsubpixel: "                 + std::to_string(output.subpixel)
		//+ "\n,\tmake: \""                   + output.make + "\""
		//+ "\n,\tmodel: \""                  + output.model + "\""
		//+ "\n,\ttransform: "                + std::to_string(output.transform)
		+ "\n,\twidth: "                    + std::to_string(output.width)
		+ "\n,\theight: "                   + std::to_string(output.height)
		//+ "\n,\trefresh_mHz: "              + std::to_string(output.refresh_mHz)
		//+ "\n,\tscale_factor: "             + std::to_string(output.scale_factor)
		+ "\n,\tpixels_per_mm_horizontal: " + std::to_string(output.pixels_per_mm_horizontal)
		+ "\n,\tpixels_per_mm_vertical: "   + std::to_string(output.pixels_per_mm_vertical)
		+ "\n}";
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p source will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::PointerAxisSource source ///< The value
	) noexcept
{
	switch(source)
	{
		case Xenium::PointerAxisSource::Continuous: return "Continuous";
		case Xenium::PointerAxisSource::Finger:     return "Finger";
		case Xenium::PointerAxisSource::Wheel:      return "Wheel";
		case Xenium::PointerAxisSource::Wheel_Tilt: return "Wheel Tilt";
		case Xenium::PointerAxisSource::Unknown:    [[fallthrough]];
		default:                                    return "";
	}
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p type will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::PointerAxisType type ///< The value
	) noexcept
{
	switch(type)
	{
		case Xenium::PointerAxisType::Horizontal: return "Horizontal";
		case Xenium::PointerAxisType::Vertical:   return "Vertical";
		case Xenium::PointerAxisType::Unknown:    [[fallthrough]];
		default:                                  return "";
	}
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p button_state will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::PointerButtonState button_state ///< The value
	) noexcept
{
	switch(button_state)
	{
		case Xenium::PointerButtonState::Pressed:  return "Pressed";
		case Xenium::PointerButtonState::Released: return "Released";
		default: return "";
	}
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p window_mode value will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::WindowMode window_mode ///< The value
	) noexcept
{
	switch(window_mode)
	{
		case Xenium::WindowMode::Fullscreen: return "Fullscreen";
		case Xenium::WindowMode::Maximized:  return "Maximized";
		case Xenium::WindowMode::Normal:     return "Normal";
		default: return "";
	}
}


/**
 * \brief Compare two Point objects.
 *
 * The X coordinates of both objects are considered to be equal if they are 
 * less than 0.001mm apart.
 *
 * The same applies with the Y coordinates.
 *
 * \note The `time` value is __not__ compared.
 *
 * \retval true  Equal
 * \retval false Not Equal
 */
bool operator==(Xenium::PointMm& lhs ///< Left-Hand side
	, Xenium::PointMm&       rhs ///< Right-Hand side
	) noexcept
{
	return equalish(lhs.x, rhs.x, 0.001)
		&&  equalish(lhs.y, rhs.y, 0.001)
		;
}


/**
 * \brief Compare two Point objects.
 *
 * The X coordinates of both objects are considered to be equal if they are 
 * less than 0.001% apart.
 *
 * The same applies with the Y coordinates.
 *
 * \note The `time` value is __not__ compared.
 *
 * \retval true  Equal
 * \retval false Not Equal
 */
bool operator==(Xenium::PointPercent& lhs ///< Left-Hand side
	, Xenium::PointPercent&       rhs ///< Right-Hand side
	) noexcept
{
	return equalish(lhs.x, rhs.x, 0.00001)
		&&  equalish(lhs.y, rhs.y, 0.00001)
		;
}


/**
 * \brief Compare two Point objects.
 *
 * Compare the X and Y values.
 *
 * \note The `time` value is __not__ compared.
 *
 * \retval true  Equal
 * \retval false Not Equal
 */
bool operator==(Xenium::PointPixel& lhs ///< Left-Hand side
	, Xenium::PointPixel&       rhs ///< Right-Hand side
	) noexcept
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}


/**
 * \brief Compare two Point objects.
 *
 * The width of both objects are considered to be equal if they are less than 
 * 0.001mm difference in length.
 *
 * The same applies with the height.
 *
 * \retval true  Equal
 * \retval false Not Equal
 */
bool operator==(Xenium::SizeMm& lhs ///< Left-Hand side
	, Xenium::SizeMm&       rhs ///< Right-Hand side
	) noexcept
{
	return equalish(lhs.width, rhs.width, 0.001)
		&&  equalish(lhs.height, rhs.height, 0.001)
		;
}


/**
 * \brief Compare two Point objects.
 *
 * The width of both objects are considered to be equal if they are less than 
 * 0.001% difference in length.
 *
 * The same applies with the height.
 *
 * \retval true  Equal
 * \retval false Not Equal
 */
bool operator==(Xenium::SizePercent& lhs ///< Left-Hand side
	, Xenium::SizePercent&       rhs ///< Right-Hand side
	) noexcept
{
	return equalish(lhs.width, rhs.width, 0.00001)
		&&  equalish(lhs.height, rhs.height, 0.00001)
		;
}


/**
 * \brief Compare two Size objects.
 *
 * Compare the width and height values.
 *
 * \retval true  Equal
 * \retval false Not Equal
 */
bool operator==(Xenium::SizePixel& lhs ///< Left-Hand side
	, Xenium::SizePixel&       rhs ///< Right-Hand side
	) noexcept
{
	return (lhs.width == rhs.width) && (lhs.height == rhs.height);
}

// }}}

};

#endif // ZAKERO_XENIUM_IMPLEMENTATION

// }}}

#endif // zakero_Xenium_h