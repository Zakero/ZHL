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
 * 	window->titleSet("Xenium");
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

// C++
#include <iostream>
#include <thread>
#include <array>
#include <future>

// POSIX
#include <poll.h>

// Linux
#include <linux/input-event-codes.h>

// X11/XCB
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <xcb/randr.h>

#define explicit explicit_
#include <xcb/xkb.h>
#undef explicit

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
	X(Error_Window_Size_Too_Small                  , 16 , "The window size was too small."                                   ) \
	X(Error_Minimum_Size_Greater_Than_Maximum_Size , 17 , "The minimum window size is larger than the maximum window size."  ) \
	X(Error_Xcb_NETWM_State_Not_Available          , 18 , "The XCB NETWM protocol extention is not supported."               ) \
	X(Error_Xcb_Fullscreen_Not_Available           , 19 , "The XCB Window Manager does not support fullscreen windows."      ) \
	X(Error_Xcb_Maximized_Window_Not_Available     , 20 , "The XCB Window Manager does not support maximized windows."       ) \
	X(Error_Xcb_WM_Delete_Window_Not_Available     , 21 , "The XCB Window Manager does not support the delete protocol."     ) \
	X(Error_Xcb_WM_Protocols_Not_Available         , 22 , "The XCB Window Manager protocols are not available."              ) \
	X(Error_Xcb_Hidden_Not_Available               , 23 , "The XCB Window Manager does not support hiding windows."          ) \
	X(Error_Xcb_Xkb_Not_Available                  , 24 , "The XCB XKB Extiension v1.0 is not available."                    ) \

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

			static constexpr int32_t Window_Size_Minimum = 100;

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

			[[nodiscard]] static Xenium* connect() noexcept;
			[[nodiscard]] static Xenium* connect(const std::string&) noexcept;
			[[nodiscard]] static Xenium* connect(std::error_code&) noexcept;
			[[nodiscard]] static Xenium* connect(const std::string&, std::error_code&) noexcept;

			// }}}
			// {{{ Cursor : TODO

			// }}}
			// {{{ Keyboard

			[[nodiscard]] int32_t keyRepeatDelay() const noexcept;
			[[nodiscard]] int32_t keyRepeatRate() const noexcept;

			// }}}
			// {{{ Output : X11/XCB

			struct Output
			{
				std::string name                     = "";
				int32_t     x                        = 0;
				int32_t     y                        = 0;
				int32_t     width                    = 0;
				int32_t     height                   = 0;
				uint32_t    physical_width_mm        = 0;
				uint32_t    physical_height_mm       = 0;
				int32_t     subpixel                 = 0;
				int32_t     transform                = 0;
				float       pixels_per_mm_horizontal = 0.0;
				float       pixels_per_mm_vertical   = 0.0;
				//std::string make                     = ""; // Not Available?
				//std::string model                    = ""; // Not Available?
				//int32_t     refresh_mHz              = 0; // Not Available?
				//int32_t     scale_factor             = 0; // Not Available?
			};

			// -------------------------------------------------- //

			using OutputId = uint32_t;

			using LambdaOutputId = std::function<void(const Xenium::OutputId)>;

			using VectorOutputId = std::vector<OutputId>;

			// -------------------------------------------------- //

			[[nodiscard]] Xenium::Output         output(const Xenium::OutputId) const noexcept;
			[[nodiscard]] Xenium::VectorOutputId outputVector() const noexcept;
			[[nodiscard]] static std::string     outputSubpixelName(int32_t) noexcept;
			[[nodiscard]] static std::string     outputTransformName(int32_t) noexcept;

			[[nodiscard]] Xenium::PointMm        outputConvertToMm(const Xenium::OutputId, const Xenium::PointPixel&) const noexcept;
			[[nodiscard]] Xenium::PointPercent   outputConvertToPercent(const Xenium::OutputId, const Xenium::PointPixel&) const noexcept;
			[[nodiscard]] Xenium::PointPixel     outputConvertToPixel(const Xenium::OutputId, const Xenium::PointMm&) const noexcept;
			[[nodiscard]] Xenium::PointPixel     outputConvertToPixel(const Xenium::OutputId, const Xenium::PointPercent&) const noexcept;

			[[nodiscard]] Xenium::SizeMm         outputConvertToMm(const Xenium::OutputId, const Xenium::SizePixel&) const noexcept;
			[[nodiscard]] Xenium::SizePercent    outputConvertToPercent(const Xenium::OutputId, const Xenium::SizePixel&) const noexcept;
			[[nodiscard]] Xenium::SizePixel      outputConvertToPixel(const Xenium::OutputId, const Xenium::SizeMm&) const noexcept;
			[[nodiscard]] Xenium::SizePixel      outputConvertToPixel(const Xenium::OutputId, const Xenium::SizePercent&) const noexcept;

			void                                 outputOnAdd(Xenium::LambdaOutputId) noexcept;
			void                                 outputOnChange(Xenium::LambdaOutputId) noexcept;
			void                                 outputOnRemove(Xenium::LambdaOutputId) noexcept;

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

			using WindowId = uint32_t;

			// -------------------------------------------------- //

			class Window
			{
				public:
					Window(Xenium*, void*);
					virtual ~Window();

					// {{{ Configuration

					void classSet(const std::string&) noexcept;
					void titleSet(const std::string&) noexcept;

					// }}}
					// {{{ Decorations

					std::error_code decorationsSet(const Xenium::WindowDecorations) noexcept;
					void            decorationsOnChange(Xenium::LambdaWindowDecorations) noexcept;

					// }}}
					// {{{ Size

					std::error_code sizeSet(const Xenium::SizeMm&) noexcept;
					std::error_code sizeSet(const Xenium::SizePercent&) noexcept;
					std::error_code sizeSet(const Xenium::SizePixel&) noexcept;
					std::error_code sizeSetMinMax(const Xenium::SizeMm&, const Xenium::SizeMm&) noexcept;
					std::error_code sizeSetMinMax(const Xenium::SizePercent&, const Xenium::SizePercent&) noexcept;
					std::error_code sizeSetMinMax(const Xenium::SizePixel&, const Xenium::SizePixel&) noexcept;
					void            sizeOnChange(Xenium::LambdaSizeMm) noexcept;
					void            sizeOnChange(Xenium::LambdaSizePercent) noexcept;
					void            sizeOnChange(Xenium::LambdaSizePixel) noexcept;

					// }}}
					// {{{ Window Mode

					[[nodiscard]] Xenium::WindowMode windowMode() const noexcept;
					bool                             windowModeIs(const Xenium::WindowMode) const noexcept;
					std::error_code                  windowModeSet(const Xenium::WindowMode) noexcept;
					void                             windowModeOnChange(Xenium::LambdaWindowMode) noexcept;

					[[nodiscard]] std::error_code    minimize() noexcept;

					// }}}
					// {{{ Rendering

					std::error_code        imageNext(uint8_t*&, Xenium::SizePixel&) noexcept;
					void                   imagePresent() noexcept;
					[[nodiscard]] uint32_t time() const noexcept;
					[[nodiscard]] uint8_t  bytesPerPixel() const noexcept;

					// }}}
					// {{{ Conversion

					[[nodiscard]] Xenium::PointMm      convertToMm(const Xenium::PointPixel&) const noexcept;
					[[nodiscard]] Xenium::PointPercent convertToPercent(const Xenium::PointPixel&) const noexcept;
					[[nodiscard]] Xenium::PointPixel   convertToPixel(const Xenium::PointMm&) const noexcept;
					[[nodiscard]] Xenium::PointPixel   convertToPixel(const Xenium::PointPercent&) const noexcept;

					[[nodiscard]] Xenium::SizeMm       convertToMm(const Xenium::SizePixel&) const noexcept;
					[[nodiscard]] Xenium::SizePercent  convertToPercent(const Xenium::SizePixel&) const noexcept;
					[[nodiscard]] Xenium::SizePixel    convertToPixel(const Xenium::SizeMm&) const noexcept;
					[[nodiscard]] Xenium::SizePixel    convertToPixel(const Xenium::SizePercent&) const noexcept;

					// }}}
					// {{{ Cursor : TODO

					// }}}
					// {{{ Keyboard

					void keyboardOnEnter(Xenium::Lambda) noexcept;
					void keyboardOnLeave(Xenium::Lambda) noexcept;
					void keyboardOnKey(Xenium::LambdaKey) noexcept;

					// }}}
					// {{{ Pointer

					void pointerOnAxis(Xenium::LambdaAxis) noexcept;
					void pointerOnButton(Xenium::LambdaButtonMm) noexcept;
					void pointerOnButton(Xenium::LambdaButtonPercent) noexcept;
					void pointerOnButton(Xenium::LambdaButtonPixel) noexcept;
					void pointerOnEnter(Xenium::LambdaPointMm) noexcept;
					void pointerOnEnter(Xenium::LambdaPointPercent) noexcept;
					void pointerOnEnter(Xenium::LambdaPointPixel) noexcept;
					void pointerOnLeave(Xenium::Lambda) noexcept;
					void pointerOnMotion(Xenium::LambdaPointMm) noexcept;
					void pointerOnMotion(Xenium::LambdaPointPercent) noexcept;
					void pointerOnMotion(Xenium::LambdaPointPixel) noexcept;

					// Future?
					//void pointerOnAxisSource(Xenium::Lambda) noexcept;
					//void pointerOnAxisStop(Xenium::Lambda) noexcept;
					//void pointerOnAxisDiscrete(Xenium::Lambda) noexcept;

					// }}}
					// {{{ Events

					void onCloseRequest(Xenium::Lambda) noexcept;
					void onFocusChange(Xenium::LambdaBool) noexcept;

					// }}}

				private:
					Xenium*           xenium;
					uint8_t*          frame_buffer;
					Xenium::SizePixel frame_buffer_size;
					Xenium::WindowId  window_id;
					xcb_gcontext_t    gc;
					size_t            frame_buffer_length;
					uint32_t          frame_time;

					Window(const Window&) = delete;
					Window& operator=(const Window&) = delete;
			};

			// -------------------------------------------------- //

			[[nodiscard]] Xenium::Window* windowCreate(const Xenium::SizeMm&, std::error_code&) noexcept;
			[[nodiscard]] Xenium::Window* windowCreate(const Xenium::SizeMm&, const uint32_t, xcb_create_window_value_list_t&, std::error_code&) noexcept;
			[[nodiscard]] Xenium::Window* windowCreate(const Xenium::SizePercent&, std::error_code&) noexcept;
			[[nodiscard]] Xenium::Window* windowCreate(const Xenium::SizePercent&, const uint32_t, xcb_create_window_value_list_t&, std::error_code&) noexcept;
			[[nodiscard]] Xenium::Window* windowCreate(const Xenium::SizePixel&, std::error_code&) noexcept;
			[[nodiscard]] Xenium::Window* windowCreate(const Xenium::SizePixel&, const uint32_t, xcb_create_window_value_list_t&, std::error_code&) noexcept;

			// }}}

		private:
			Xenium() noexcept;

			// {{{ Connection

			void disconnect() noexcept;

			// }}}
			// {{{ Initialization

			[[nodiscard]] std::error_code init(xcb_connection_t*, int) noexcept;

			// }}}
			// {{{ Cursor : TODO

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

			// -------------------------------------------------- //

			LambdaOutputId      output_on_add    = {};
			LambdaOutputId      output_on_change = {};
			LambdaOutputId      output_on_remove = {};
			Map_OutputId_Output output_map       = {};
			mutable std::mutex  output_mutex     = {};

			// -------------------------------------------------- //

			[[nodiscard]] const Xenium::Output& output(const int16_t, const int16_t, OutputId&) noexcept;
			[[nodiscard]] std::error_code       outputInit() noexcept;
			[[nodiscard]] std::error_code       outputAdd(xcb_randr_crtc_t, xcb_randr_output_t) noexcept;
			void                                outputAdd(const xcb_randr_get_crtc_info_reply_t*, const xcb_randr_get_output_info_reply_t*) noexcept;

			// }}}
			// {{{ Window
			
			enum struct SizeUnit
			{	Millimeter
			,	Percent
			,	Pixel
			};

			struct MotifHints
			{
				uint32_t flags;
				uint32_t functions;
				uint32_t decorations;
				int32_t  input_mode;
				uint32_t status;
			};
	
			struct WindowCreateData
			{
				std::promise<void>              barrier;
				std::error_code                 error;
				Xenium::WindowId                window_id;
				Xenium::OutputId                output_id;
				xcb_atom_t                      atom;
				xcb_gcontext_t                  gc;
				Xenium::SizeUnit                size_unit;
				Xenium::SizeMm                  size_mm;
				Xenium::SizePercent             size_percent;
				Xenium::SizePixel               size_pixel;
				uint32_t                        value_mask;
				xcb_create_window_value_list_t& value_list;
			};
	
			struct WindowDestroyData
			{
				std::promise<void> barrier;
				Xenium::WindowId   window_id;
			};

			struct WindowDeleteData
			{
				Xenium::Lambda close_request_lambda;
				xcb_atom_t     atom;
			};

			struct WindowSizeData
			{
				Xenium::SizeMm            mm;
				Xenium::SizeMm            mm_minimum;
				Xenium::SizeMm            mm_maximum;
				Xenium::LambdaSizeMm      mm_lambda;
				Xenium::SizePercent       percent;
				Xenium::SizePercent       percent_minimum;
				Xenium::SizePercent       percent_maximum;
				Xenium::LambdaSizePercent percent_lambda;
				Xenium::SizePixel         pixel;
				Xenium::SizePixel         pixel_minimum;
				Xenium::SizePixel         pixel_maximum;
				Xenium::LambdaSizePixel   pixel_lambda;
				Xenium::SizeUnit          unit;
			};

			struct WindowModeData
			{
				Xenium::WindowMode       window_mode;
				Xenium::LambdaWindowMode lambda;
			};

			struct WindowDecorationsData
			{
				Xenium::WindowDecorations       window_decorations;
				Xenium::LambdaWindowDecorations lambda;
			};

			struct WindowOnButtonData
			{
				Xenium::LambdaButtonMm      lambda_mm;
				Xenium::LambdaButtonPercent lambda_percent;
				Xenium::LambdaButtonPixel   lambda_pixel;
			};

			struct WindowOnEnterData
			{
				Xenium::LambdaPointMm      lambda_mm;
				Xenium::LambdaPointPercent lambda_percent;
				Xenium::LambdaPointPixel   lambda_pixel;
			};

			struct WindowOnMotionData
			{
				Xenium::LambdaPointMm      lambda_mm;
				Xenium::LambdaPointPercent lambda_percent;
				Xenium::LambdaPointPixel   lambda_pixel;
			};

			struct WindowKeyboardData
			{
				Xenium::Lambda on_enter;
				Xenium::Lambda on_leave;
			};

			using WindowDecorationsMap = std::unordered_map<WindowId, WindowDecorationsData>;
			using WindowDeleteMap      = std::unordered_map<WindowId, WindowDeleteData>;
			using WindowFocusMap       = std::unordered_map<WindowId, LambdaBool>;
			using WindowKeyboard       = std::unordered_map<WindowId, WindowKeyboardData>;
			using WindowMap            = std::unordered_map<WindowId, Window*>;
			using WindowModeMap        = std::unordered_map<WindowId, WindowModeData>;
			using WindowOnAxisMap      = std::unordered_map<WindowId, LambdaAxis>;
			using WindowOnButtonMap    = std::unordered_map<WindowId, WindowOnButtonData>;
			using WindowOnEnterMap     = std::unordered_map<WindowId, WindowOnEnterData>;
			using WindowOnKeyMap       = std::unordered_map<WindowId, LambdaKey>;
			using WindowOnLeaveMap     = std::unordered_map<WindowId, Lambda>;
			using WindowOnMotionMap    = std::unordered_map<WindowId, WindowOnMotionData>;
			using WindowOutputMap      = std::unordered_map<WindowId, OutputId>;
			using WindowReadyMap       = std::unordered_map<WindowId, bool>;
			using WindowSizeMap        = std::unordered_map<WindowId, WindowSizeData>;
			using WindowToCreate       = std::vector<Xenium::WindowCreateData*>;
			using WindowToDestroy      = std::vector<Xenium::WindowDestroyData*>;

			// -------------------------------------------------- //

			WindowDecorationsMap window_decorations_map = {};
			WindowDeleteMap      window_delete_map      = {};
			WindowFocusMap       window_focus_map       = {};
			WindowKeyboard       window_keyboard        = {};
			WindowMap            window_map             = {};
			WindowModeMap        window_mode_map        = {};
			WindowOnAxisMap      window_on_axis_map     = {};
			WindowOnButtonMap    window_on_button_map   = {};
			WindowOnEnterMap     window_on_enter_map    = {};
			WindowOnKeyMap       window_on_key_map      = {};
			WindowOnLeaveMap     window_on_leave_map    = {};
			WindowOnMotionMap    window_on_motion_map   = {};
			WindowOutputMap      window_output_map      = {};
			WindowReadyMap       window_ready_map       = {};
			WindowSizeMap        window_size_map        = {};
			WindowToCreate       window_to_create       = {};
			WindowToDestroy      window_to_destroy      = {};

			// -------------------------------------------------- //

			[[nodiscard]] std::error_code windowBorder(const WindowId, const bool) noexcept;
			void                          windowCreate(Xenium::WindowCreateData*) noexcept;
			void                          windowCreateAddToQueue(Xenium::WindowCreateData*) noexcept;
			void                          windowDestroyAddToQueue(Xenium::WindowDestroyData*) noexcept;
			[[nodiscard]] std::error_code windowLocationSet(const WindowId, const PointPixel&) noexcept;
			[[nodiscard]] std::error_code windowMinimize(const Xenium::WindowId) noexcept;
			[[nodiscard]] std::error_code windowModeSet(const Xenium::WindowId, const Xenium::WindowMode, const Xenium::WindowMode) noexcept;
			bool                          windowPropertySet(WindowId, const xcb_atom_t, const xcb_atom_t, xcb_generic_error_t&) noexcept;
			bool                          windowPropertySet(WindowId, const xcb_atom_t, const std::string&, xcb_generic_error_t&) noexcept;
			void                          windowReadySet(const WindowId) noexcept;
			void                          windowReadyWait(const WindowId) noexcept;
			void                          windowResizeTo(const Output&, Xenium::WindowSizeData&, const xcb_configure_notify_event_t*) noexcept;
			std::error_code               windowSizeSet(const WindowId, const SizePixel&) noexcept;
			[[nodiscard]] std::error_code windowSizeSetMinMax(const WindowId, const int32_t, const int32_t, const int32_t, const int32_t) noexcept;
			std::error_code               windowSizeSetMinMax(const Xenium::Output&, const Xenium::WindowId, Xenium::WindowSizeData&) noexcept;
                                
			// }}}
			// {{{ XCB

			xcb_connection_t*  connection = nullptr;
			const xcb_setup_t* setup      = nullptr;
			xcb_screen_t*      screen     = nullptr;

			// -------------------------------------------------- //

			void xcbEvent(const xcb_button_press_event_t*) noexcept;
			void xcbEvent(const xcb_client_message_event_t*) noexcept;
			void xcbEvent(const xcb_configure_notify_event_t*) noexcept;
			void xcbEvent(const xcb_enter_notify_event_t*) noexcept;
			void xcbEvent(const xcb_expose_event_t*) noexcept;
			void xcbEvent(const xcb_focus_in_event_t*) noexcept;
			void xcbEvent(const xcb_gravity_notify_event_t*) noexcept;
			void xcbEvent(const xcb_key_press_event_t*) noexcept;
			void xcbEvent(const xcb_map_notify_event_t*) noexcept;
			void xcbEvent(const xcb_motion_notify_event_t*) noexcept;
			void xcbEvent(const xcb_property_notify_event_t*) noexcept;
			void xcbEvent(const xcb_reparent_notify_event_t*) noexcept;
			void xcbEvent(const xcb_unmap_notify_event_t*) noexcept;

			void xcbWindowCreate(Xenium::WindowCreateData*) noexcept;
			void xcbWindowDestroy(Xenium::WindowDestroyData*) noexcept;
			void xcbWindowInit(Xenium::WindowCreateData*) noexcept;

			// }}}
			// {{{ XCB : Atom

			xcb_atom_t atom_wm_change_state             = XCB_ATOM_NONE;
			xcb_atom_t atom_wm_delete_window            = XCB_ATOM_NONE;
			xcb_atom_t atom_wm_protocols                = XCB_ATOM_NONE;
			xcb_atom_t atom_net_frame_extents           = XCB_ATOM_NONE;
			xcb_atom_t atom_net_wm_state                = XCB_ATOM_NONE;
			xcb_atom_t atom_net_wm_state_fullscreen     = XCB_ATOM_NONE;
			xcb_atom_t atom_net_wm_state_hidden         = XCB_ATOM_NONE;
			xcb_atom_t atom_net_wm_state_maximized_horz = XCB_ATOM_NONE;
			xcb_atom_t atom_net_wm_state_maximized_vert = XCB_ATOM_NONE;

			// -------------------------------------------------- //

			[[nodiscard]] std::error_code          atomInit() noexcept;
			[[nodiscard]] xcb_atom_t               atomCreateDeleteWindow(const WindowId, xcb_generic_error_t&) noexcept;
			[[nodiscard]] std::string              atomName(const xcb_atom_t) noexcept;
			[[nodiscard]] std::vector<xcb_atom_t>  atomValueAtom(const WindowId, const xcb_atom_t) noexcept;
			[[nodiscard]] std::vector<xcb_atom_t>  atomValueAtom(const WindowId, const xcb_atom_t, xcb_generic_error_t&) noexcept;
			[[nodiscard]] std::vector<int32_t>     atomValueData(const WindowId, const xcb_atom_t, const xcb_atom_t, const size_t) noexcept;
			[[nodiscard]] std::vector<int32_t>     atomValueData(const WindowId, const xcb_atom_t, const xcb_atom_t, const size_t, xcb_generic_error_t&) noexcept;
			[[nodiscard]] xcb_atom_t               internAtom(const std::string&, const bool, xcb_generic_error_t&) noexcept;
			[[nodiscard]] xcb_intern_atom_cookie_t internAtomRequest(const std::string&, const bool = true) noexcept;
			[[nodiscard]] xcb_atom_t               internAtomReply(const xcb_intern_atom_cookie_t, xcb_generic_error_t&) noexcept;

			// }}}
			// {{{ XCB : RandR

			int randr_error_base          = 0;
			int randr_event_base          = 0;
			int randr_query_version_major = 0;
			int randr_query_version_minor = 0;

			// -------------------------------------------------- //

			[[nodiscard]] std::error_code randrInit() noexcept;

			void randrEvent(const xcb_randr_crtc_change_t*) noexcept;
			void randrEvent(const xcb_randr_output_change_t*) noexcept;
			void randrEvent(const xcb_randr_notify_event_t*) noexcept;
			void randrEvent(const xcb_randr_screen_change_notify_event_t*) noexcept;

			// }}}
			// {{{ XCB : Utility
			
			[[nodiscard]] bool requestCheckHasError(const xcb_void_cookie_t&) noexcept;
			[[nodiscard]] bool requestCheckHasError(const xcb_void_cookie_t&, xcb_generic_error_t&) noexcept;

			// }}}
			// {{{ XCB : XKB

			struct XkbControls
			{
				uint32_t repeat_delay_ms    = 600;
				uint32_t repeat_interval_ms =  50;
			};

			struct KeyData
			{
				Xenium::Key         key         = { 0 };
				Xenium::KeyModifier modifier    = { 0 };
				Xenium::WindowId    window_id   = { 0 };
				uint32_t            repeat_time = { 0 };
			};

			using KeyDataArray = std::array<KeyData, 256>;

			// -------------------------------------------------- //

			KeyDataArray key_data_array       = { 0 };
			KeyModifier  key_modifier         = { 0 };
			XkbControls  xkb_controls         = {};
			uint16_t     xkb_modifier_pressed = 0;
			
			// -------------------------------------------------- //

			inline void                   keyDataArrayClear() noexcept;
			inline void                   keyDataArrayProcess() noexcept;
			[[nodiscard]] std::error_code xkbInit() noexcept;
			void                          xkbControlsUpdate() noexcept;
			void                          xkbIndicatorStateUpdate() noexcept;

			// }}}
			// {{{ Utility

			std::mutex xenium_window_mutex = {};

			// -------------------------------------------------- //

			[[nodiscard]] std::pair<float, float>     convertPixelToMm(const Xenium::Output&, int32_t, int32_t) const noexcept;
			[[nodiscard]] std::pair<float, float>     convertPixelToPercent(const Xenium::Output&, int32_t, int32_t) const noexcept;
			[[nodiscard]] std::pair<int32_t, int32_t> convertMmToPixel(const Xenium::Output&, float, float) const noexcept;
			[[nodiscard]] std::pair<int32_t, int32_t> convertPercentToPixel(const Xenium::Output&, float, float) const noexcept;

			// }}}

			Xenium(const Xenium&) = delete;
			Xenium& operator=(const Xenium&) = delete;
	}; // class Xenium

	// }}}
	// {{{ Convenience

	[[nodiscard]] std::string to_string(const std::vector<xcb_atom_t>&) noexcept;
	[[nodiscard]] std::string to_string(const std::vector<int32_t>&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_generic_error_t&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_button_press_event_t&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_client_message_event_t&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_configure_notify_event_t&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_enter_notify_event_t&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_expose_event_t&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_focus_in_event_t&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_generic_event_t&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_gravity_notify_event_t&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_key_press_event_t&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_map_notify_event_t&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_motion_notify_event_t&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_property_notify_event_t&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_reparent_notify_event_t&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_unmap_notify_event_t&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_format_t&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_screen_t&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_setup_t&) noexcept;
	[[nodiscard]] std::string to_string(const xcb_randr_screen_change_notify_event_t&) noexcept;
	[[nodiscard]] std::string to_string(const Xenium::Key&) noexcept;
	[[nodiscard]] std::string to_string(const Xenium::KeyModifier&) noexcept;
	[[nodiscard]] std::string to_string(const Xenium::KeyState) noexcept;
	[[nodiscard]] std::string to_string(const Xenium::Output&) noexcept;
	[[nodiscard]] std::string to_string(const Xenium::PointMm) noexcept;
	[[nodiscard]] std::string to_string(const Xenium::PointPercent) noexcept;
	[[nodiscard]] std::string to_string(const Xenium::PointPixel) noexcept;
	[[nodiscard]] std::string to_string(const Xenium::PointerAxis&) noexcept;
	[[nodiscard]] std::string to_string(const Xenium::PointerAxisSource) noexcept;
	[[nodiscard]] std::string to_string(const Xenium::PointerAxisType) noexcept;
	[[nodiscard]] std::string to_string(const Xenium::PointerButton&) noexcept;
	[[nodiscard]] std::string to_string(const Xenium::PointerButtonState&) noexcept;
	[[nodiscard]] std::string to_string(const Xenium::SizeMm&) noexcept;
	[[nodiscard]] std::string to_string(const Xenium::SizePercent&) noexcept;
	[[nodiscard]] std::string to_string(const Xenium::SizePixel&) noexcept;
	[[nodiscard]] std::string to_string(const Xenium::WindowDecorations) noexcept;
	[[nodiscard]] std::string to_string(const Xenium::WindowMode) noexcept;

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
// {{{ Macros : XCB

#ifndef _NET_WM_STATE_REMOVE
#define _NET_WM_STATE_REMOVE 0
#endif

#ifndef _NET_WM_STATE_ADD
#define _NET_WM_STATE_ADD 1
#endif

#ifndef _NET_WM_STATE_TOGGLE
#define _NET_WM_STATE_TOGGLE 2
#endif

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
 * \def ZAKERO_XENIUM_DEBUG_STREAM
 *
 * \brief The stream to use for debugging output.
 *
 * This macro holds the stream that will be written to for the debug messages.  
 * If this macro is undefined at compile time, then `std::cerr` will be used.
 *
 * \parcode
 * #define ZAKERO_XENIUM_DEBUG_STREAM MyLogger::errorLogStream()
 * \endparcode
 */
#ifndef ZAKERO_XENIUM_DEBUG_STREAM
#define ZAKERO_XENIUM_DEBUG_STREAM std::cerr
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
#define ZAKERO_XENIUM__DEBUG                                     \
	if(ZAKERO_XENIUM__DEBUG_DISABLED) {}                     \
	else ZAKERO_XENIUM_DEBUG_STREAM                          \
		<< "pid(" << std::to_string(int64_t(ZAKERO_PID)) \
		<< ") " __FILE__ "("                             \
		<< std::to_string(__LINE__)                      \
		<< ") "                                          \
		<< __PRETTY_FUNCTION__                           \
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
 * \brief Write debugging message.
 *
 * Does the samething as `ZAKERO_XENIUM__DEBUG` but is specialized to work with 
 * std::error_code values.
 *
 * \parcode
 * std::error_code error = ZAKERO_XENIUM__ERROR(Error_Unknown)
 * ZAKERO_XENIUM__DEBUG_ERROR(error);
 * \endparcode
 *
 * \param var_ The variable to write.
 */
#define ZAKERO_XENIUM__DEBUG_ERROR(var_)                \
	ZAKERO_XENIUM__DEBUG                            \
		<< "Error: " << zakero::to_string(var_) \
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

/**
 * \internal
 *
 * \brief Convert the enum to a string.
 */
#define ZAKERO_XENIUM__OUTPUT_TRANSFORM \
	X(XCB_RANDR_TRANSFORM_UNIT       , "Unit"       ) \
	X(XCB_RANDR_TRANSFORM_SCALE_UP   , "Scale Up"   ) \
	X(XCB_RANDR_TRANSFORM_SCALE_DOWN , "Scale Down" ) \
	X(XCB_RANDR_TRANSFORM_PROJECTIVE , "Projective" ) \

// }}}

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

	static constexpr uint32_t Size_Max = (uint32_t)std::numeric_limits<int32_t>::max();

	constexpr uint8_t  XCB_KEY_REPEAT = XCB_KEY_PRESS | 0x80;
	constexpr uint32_t XCB_XKB_INDICATOR_STATE_CAPSLOCK = 0x00000001;
	constexpr uint32_t XCB_XKB_INDICATOR_STATE_NUMLOCK  = 0x00000002;

	constexpr std::array<uint32_t, 8> Pointer_Button_Event_Code =
	{	BTN_LEFT	// 0x110  272
	,	BTN_MIDDLE	// 0x112  274
	,	BTN_RIGHT	// 0x111  273
	,	BTN_SIDE	// 0x113  275
	,	BTN_EXTRA	// 0x114  276
	,	BTN_FORWARD	// 0x115  277
	,	BTN_BACK	// 0x116  278
	,	BTN_TASK	// 0x116  279
	};

	const uint32_t Default_Value_Mask = 0
		//| XCB_CW_BACK_PIXMAP       // Not Used
		| XCB_CW_BACK_PIXEL        // Not Used
		//| XCB_CW_BORDER_PIXMAP     // Not Used
		//| XCB_CW_BORDER_PIXEL      // Not Used
		| XCB_CW_BIT_GRAVITY
		| XCB_CW_WIN_GRAVITY
		| XCB_CW_BACKING_STORE
		//| XCB_CW_BACKING_PLANES    // Not Used
		//| XCB_CW_BACKING_PIXEL     // Not Used
		//| XCB_CW_OVERRIDE_REDIRECT // Future?
		| XCB_CW_SAVE_UNDER
		| XCB_CW_EVENT_MASK
		//| XCB_CW_DONT_PROPAGATE    // Future?
		| XCB_CW_COLORMAP
		//| XCB_CW_CURSOR            // Future?
		;

	xcb_create_window_value_list_t Default_Value_List =
	{	.background_pixmap     = XCB_BACK_PIXMAP_NONE
	,	.background_pixel      = 0
	,	.border_pixmap         = XCB_BACK_PIXMAP_NONE
	,	.border_pixel          = 0
	,	.bit_gravity           = XCB_GRAVITY_CENTER
	,	.win_gravity           = XCB_GRAVITY_NORTH_EAST
	,	.backing_store         = XCB_BACKING_STORE_NOT_USEFUL
	,	.backing_planes        = 0
	,	.backing_pixel         = 0
	,	.override_redirect     = 0
	,	.save_under            = 0
	,	.event_mask            = 0
		                         | XCB_EVENT_MASK_KEY_PRESS
		                         | XCB_EVENT_MASK_KEY_RELEASE
		                         | XCB_EVENT_MASK_BUTTON_PRESS
		                         | XCB_EVENT_MASK_BUTTON_RELEASE
		                         | XCB_EVENT_MASK_ENTER_WINDOW
		                         | XCB_EVENT_MASK_LEAVE_WINDOW
		                         | XCB_EVENT_MASK_POINTER_MOTION
		                         ////| XCB_EVENT_MASK_POINTER_MOTION_HINT
		                         ////| XCB_EVENT_MASK_BUTTON_1_MOTION
		                         ////| XCB_EVENT_MASK_BUTTON_2_MOTION
		                         ////| XCB_EVENT_MASK_BUTTON_3_MOTION
		                         ////| XCB_EVENT_MASK_BUTTON_4_MOTION
		                         ////| XCB_EVENT_MASK_BUTTON_5_MOTION
		                         ////| XCB_EVENT_MASK_BUTTON_MOTION
		                         ////| XCB_EVENT_MASK_KEYMAP_STATE
		                         | XCB_EVENT_MASK_EXPOSURE
		                         //| XCB_EVENT_MASK_VISIBILITY_CHANGE
		                         | XCB_EVENT_MASK_STRUCTURE_NOTIFY
		                         ////| XCB_EVENT_MASK_RESIZE_REDIRECT
		                         ////| XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
		                         ////| XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT
		                         | XCB_EVENT_MASK_FOCUS_CHANGE
		                         | XCB_EVENT_MASK_PROPERTY_CHANGE
		                         ////| XCB_EVENT_MASK_COLOR_MAP_CHANGE
		                         ////| XCB_EVENT_MASK_OWNER_GRAB_BUTTON
	,	.do_not_propogate_mask = XCB_EVENT_MASK_NO_EVENT
	,	.colormap              = XCB_COPY_FROM_PARENT
	,	.cursor                = 0
	};

	
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

		if((max.width > 0)
			&& (min.width > max.width)
			)
		{
			return ZAKERO_XENIUM__ERROR(Xenium::Error_Minimum_Size_Greater_Than_Maximum_Size);
		}

		if((max.height > 0)
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
 *
 * \bug When setting the size of the window, the Window Manager's Frame Extents 
 * must be taken into account.  (Window Size - Frame Extents = content size)
 * See _NET_FRAME_EXTENTS usage for hints on how to get this data.  _Also, when 
 * the window borders are removed, the window content is resized to fill the 
 * space used by the window border._
 *
 * \todo Convert all the `Error_Unknown` to actual errors.
 *
 * \todo Create event "pixelsPerMillimeterChange()".  This event will be 
 * triggerd with the output device configuration changes and when the window is 
 * moved to a different output device. (Might need this in Zakero_Yetani)
 *
 * \todo Use a future/promise barrier to wait for the event loop.
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
	: event_loop()
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
		delete xenium;

		return nullptr;
	}

	xenium->eventLoopStart();

	error = xenium->atomInit();
	if(error)
	{
		delete xenium;

		return nullptr;
	}

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
std::error_code Xenium::init(xcb_connection_t* connection    ///< The XCB Connection
	, int                                  screen_number ///< The Screen being used.
	) noexcept
{
	this->connection = connection;

	// --- X11 Server Setup Information --- //
	this->setup = xcb_get_setup(this->connection);
	ZAKERO_XENIUM__DEBUG_VAR(to_string(*setup));

	// --- Find the current screen --- //
	xcb_screen_iterator_t screen_iterator = xcb_setup_roots_iterator(this->setup);

	for(int i = 0; i < screen_number; i++)
	{
		xcb_screen_next(&screen_iterator);
	}

	this->screen = screen_iterator.data;
	ZAKERO_XENIUM__DEBUG_VAR(to_string(*screen));

	// --- XXB Extension --- //
	std::error_code error;

	error = xkbInit();
	if(error)
	{
		return error;
	}
	
	// --- Initialize The Internals --- //
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
// {{{ Keyboard

int32_t Xenium::keyRepeatDelay() const noexcept
{
	return xkb_controls.repeat_delay_ms;
}


int32_t Xenium::keyRepeatRate() const noexcept
{
	return xkb_controls.repeat_interval_ms;
}

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
	const int Randr_Notify_Event = xenium->randr_event_base + XCB_RANDR_NOTIFY;

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
				xenium->keyDataArrayProcess();

				break;
			}

			switch(event->response_type & 0x7f)
			{
				case XCB_CLIENT_MESSAGE:
					xenium->xcbEvent(
						(xcb_client_message_event_t*)event
						);
					break;

				// --- XCB_EVENT_MASK_BUTTON_PRESS ------------ //
				// --- XCB_EVENT_MASK_BUTTON_RELEASE ---------- //
				case XCB_BUTTON_PRESS:
					[[fallthrough]];
				case XCB_BUTTON_RELEASE:
					xenium->xcbEvent(
						(xcb_button_press_event_t*)event
						);
					break;

				// --- XCB_EVENT_MASK_ENTER_WINDOW ------------ //
				// --- XCB_EVENT_MASK_LEAVE_WINDOW ------------ //
				case XCB_ENTER_NOTIFY: [[fallthrough]];
				case XCB_LEAVE_NOTIFY:
					xenium->xcbEvent(
						(xcb_enter_notify_event_t*)event
						);
					break;

				// --- XCB_EVENT_MASK_EXPOSURE ---------------- //
				case XCB_EXPOSE:
					xenium->xcbEvent(
						(xcb_expose_event_t*)event
						);
					break;

				// --- XCB_EVENT_MASK_FOCUS_CHANGE ------------ //
				case XCB_FOCUS_IN:	[[fallthrough]];
				case XCB_FOCUS_OUT:
					xenium->xcbEvent(
						(xcb_focus_in_event_t*)event
						);
					break;

				// --- XCB_EVENT_MASK_KEY_PRESS --------------- //
				// --- XCB_EVENT_MASK_KEY_RELEASE ------------- //
				case XCB_KEY_PRESS:	[[fallthrough]];
				case XCB_KEY_RELEASE:
					xenium->xcbEvent(
						(xcb_key_press_event_t*)event
						);
					break;

				// --- XCB_EVENT_MASK_POINTER_MOTION ---------- //
				case XCB_MOTION_NOTIFY:
					xenium->xcbEvent(
						(xcb_motion_notify_event_t*)event
						);
					break;

				// --- XCB_EVENT_MASK_PROPERTY_CHANGE --------- //
				case XCB_PROPERTY_NOTIFY:
					xenium->xcbEvent(
						(xcb_property_notify_event_t*)event
						);
					break;

				// --- XCB_EVENT_MASK_STRUCTURE_NOTIFY -------- //
				case XCB_CONFIGURE_NOTIFY:
					xenium->xcbEvent(
						(xcb_configure_notify_event_t*)event
						);
					break;

				case XCB_GRAVITY_NOTIFY:
					xenium->xcbEvent(
						(xcb_gravity_notify_event_t*)event
						);
					break;

				case XCB_MAP_NOTIFY:
					xenium->xcbEvent(
						(xcb_map_notify_event_t*)event
						);
					break;

				case XCB_REPARENT_NOTIFY:
					xenium->xcbEvent(
						(xcb_reparent_notify_event_t*)event
						);
					break;

				case XCB_UNMAP_NOTIFY:
					xenium->xcbEvent(
						(xcb_unmap_notify_event_t*)event
						);
					break;

				// --- Other Event Types ---------------------- //
				default:
					if(event->response_type == Randr_Notify_Event)
					{
						xenium->randrEvent(
							(xcb_randr_notify_event_t*)event
							);
std::cout << "RandR Event:     " << to_string(*event) << '\n';
					}
					else
					{
std::cout << "Unknown:         " << to_string(*event) << '\n';
					}

				// --- ------------------------------------ --- //
				// --- Unused event masks at this time      --- //
				// --- ------------------------------------ --- //
				// --- XCB_EVENT_MASK_BUTTON_1_MOTION --------- //
				// --- XCB_EVENT_MASK_BUTTON_2_MOTION --------- //
				// --- XCB_EVENT_MASK_BUTTON_3_MOTION --------- //
				// --- XCB_EVENT_MASK_BUTTON_4_MOTION --------- //
				// --- XCB_EVENT_MASK_BUTTON_5_MOTION --------- //
				// --- XCB_EVENT_MASK_BUTTON_MOTION ----------- //
				// --- XCB_EVENT_MASK_COLOR_MAP_CHANGE -------- //
				// --- XCB_EVENT_MASK_KEYMAP_STATE ------------ //
				// --- XCB_EVENT_MASK_OWNER_GRAB_BUTTON ------- //
				// --- XCB_EVENT_MASK_POINTER_MOTION_HINT ----- //
				// --- XCB_EVENT_MASK_RESIZE_REDIRECT --------- //
				// --- XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY ----- //
				// --- XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT --- //
				// --- XCB_EVENT_MASK_VISIBILITY_CHANGE ------- //
			}

			free(event);
		}

		xenium->xenium_window_mutex.lock();

		for(Xenium::WindowCreateData* window_data : xenium->window_to_create)
		{
			xenium->windowCreate(window_data);
		}

		for(Xenium::WindowCreateData* window_data : xenium->window_to_create)
		{
			window_data->barrier.set_value();
		}

		xenium->window_to_create.clear();

		for(Xenium::WindowDestroyData* window_data : xenium->window_to_destroy)
		{
			xenium->xcbWindowDestroy(window_data);
			window_data->barrier.set_value();
		}

		xenium->window_to_destroy.clear();

		xenium->xenium_window_mutex.unlock();

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
	std::lock_guard<std::mutex> lock(output_mutex);

	if(output_map.contains(output_id) == false)
	{
		ZAKERO_XENIUM__DEBUG
			<< "Invalid output_id: "
			<< std::to_string(output_id)
			;

		return {};
	}

	return output_map.at(output_id);
}


const Xenium::Output& Xenium::output(const int16_t x
	, const int16_t                            y
	, OutputId&                                output_id
	) noexcept
{
	for(const auto& iter : output_map)
	{
		const Output& output = iter.second;

		if(x >= output.x
			&& x < (output.x + output.width)
			&& y >= output.y
			&& y < (output.y + output.height)
			)
		{
			output_id = iter.first;
			return output;
		}
	}

	uint64_t distance = std::numeric_limits<uint64_t>::max();

	for(const auto& iter : output_map)
	{
		const Output& output = iter.second;

		int64_t output_x = (std::abs(output.x) + output.width) / 2;
		int64_t output_y = (std::abs(output.y) + output.height) / 2;
		
		uint64_t dist = std::abs(x - output_x) + std::abs(y - output_y);

		if(dist < distance)
		{
			output_id = iter.first;
		}
	}

	return output_map[output_id];
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
	Xenium::VectorOutputId vector(output_map.size());
	vector.clear();

	std::lock_guard<std::mutex> lock(output_mutex);

	for(const auto& iter : output_map)
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
 * \brief Get a human readable string.
 *
 * The `Xenium::Output::subpixel` is an XCB RandR enum value and this method 
 * will convert that value into a descriptive name string.  If an invalid value 
 * is passed, then an empty string will be returned.
 *
 * \return The name of the transform.
 *
 * \thread_user
 */
std::string Xenium::outputTransformName(int32_t transform ///< The Transform
	) noexcept
{
	switch(transform)
	{
#define X(value_, name_) \
		case value_: return name_;
		ZAKERO_XENIUM__OUTPUT_TRANSFORM
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
	std::lock_guard<std::mutex> lock(output_mutex);

	if(output_map.contains(output_id) == false)
	{
		return { point.time, 0, 0 };
	}

	const Xenium::Output& output = output_map.at(output_id);

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
	std::lock_guard<std::mutex> lock(output_mutex);

	if(output_map.contains(output_id) == false)
	{
		return { point.time, 0, 0 };
	}

	const Xenium::Output& output = output_map.at(output_id);

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
	std::lock_guard<std::mutex> lock(output_mutex);

	if(output_map.contains(output_id) == false)
	{
		return { point.time, 0, 0 };
	}

	const Xenium::Output& output = output_map.at(output_id);

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
	std::lock_guard<std::mutex> lock(output_mutex);

	if(output_map.contains(output_id) == false)
	{
		return { point.time, 0, 0 };
	}

	const Xenium::Output& output = output_map.at(output_id);

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
	std::lock_guard<std::mutex> lock(output_mutex);

	if(output_map.contains(output_id) == false)
	{
		return { 0, 0 };
	}

	const Xenium::Output& output = output_map.at(output_id);

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
	std::lock_guard<std::mutex> lock(output_mutex);

	if(output_map.contains(output_id) == false)
	{
		return { 0, 0 };
	}

	const Xenium::Output& output = output_map.at(output_id);

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
	std::lock_guard<std::mutex> lock(output_mutex);

	if(output_map.contains(output_id) == false)
	{
		return { 0, 0 };
	}

	const Xenium::Output& output = output_map.at(output_id);

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
	std::lock_guard<std::mutex> lock(output_mutex);

	if(output_map.contains(output_id) == false)
	{
		return { 0, 0 };
	}

	const Xenium::Output& output = output_map.at(output_id);

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
	if(lambda == nullptr)
	{
		output_on_add = LambdaOutputId_DoNothing;
	}
	else
	{
		output_on_add = lambda;
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
	if(lambda == nullptr)
	{
		output_on_change = LambdaOutputId_DoNothing;
	}
	else
	{
		output_on_change = lambda;
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
	if(lambda == nullptr)
	{
		output_on_remove = LambdaOutputId_DoNothing;
	}
	else
	{
		output_on_remove = lambda;
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
			free(output_info);
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
			free(crtc_info);
			free(output_info);
			continue;
		}

		outputAdd(crtc_info, output_info);

		free(crtc_info);
		free(output_info);
	}

	free(screen_resources);

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
		free(screen_resources);

		return ZAKERO_XENIUM__ERROR(Error_RandR_Output_Info_Not_Found);
	}

	if(output_info->mm_width == 0
		|| output_info->mm_height == 0
		)
	{
		free(output_info);
		free(screen_resources);

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
		free(output_info);
		free(screen_resources);

		return ZAKERO_XENIUM__ERROR(Error_RandR_CRTC_Info_Not_Found);
	}

	outputAdd(crtc_info, output_info);

	free(crtc_info);
	free(output_info);
	free(screen_resources);

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

	this->output_map[output_id] =
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

ZAKERO_XENIUM__DEBUG_VAR(output_id)
ZAKERO_XENIUM__DEBUG_VAR(this->output_map[output_id].name)
}


// }}}
// {{{ Window
// {{{ Window : Documentation

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

// }}}

/**
 * \brief Create a window.
 *
 * Create a new Window with the specified \p size.  A pointer to the Window 
 * will be returned.  If there was a problem, a `nullptr` will returned.
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
	return windowCreate(size, Default_Value_Mask, Default_Value_List, error);
}


/**
 * \brief Create a window.
 *
 * Create a new Window with the specified \p size and values.  The \p 
 * value_mask and \p value_list work together.  The \p value_list is a 
 * collection of values that will be applied to the Window.  The \p value_mask 
 * bit field determines which values will be used in the \p value_list.
 *
 * The \p error parameter will be set to Error_None if the window was 
 * successfully created or it will be set to an appropriate error code.
 *
 * \note The size of a window __must__ be greater than `0`.
 *
 * \return A pointer to the new Window or `nullptr` on error.
 *
 * \thread_user
 */
Xenium::Window* Xenium::windowCreate(const Xenium::SizeMm& size_mm    ///< The window size
	, const uint32_t                                   value_mask ///< The value mask
	, xcb_create_window_value_list_t&                  value_list ///< The value list
	, std::error_code&                                 error      ///< The error state
	) noexcept
{
	Xenium::WindowCreateData data =
	{	.barrier        = {}
	,	.error          = {}
	,	.window_id      = 0
	,	.output_id      = 0
	,	.atom           = 0
	,	.size_unit      = Xenium::SizeUnit::Millimeter
	,	.size_mm        = size_mm
	,	.size_percent   = {}
	,	.size_pixel     = {}
	,	.value_mask     = value_mask
	,	.value_list     = value_list
	};

	std::future<void> barrier = data.barrier.get_future();

	windowCreateAddToQueue(&data);

	barrier.wait();

	if(data.error)
	{
		error = data.error;

		ZAKERO_XENIUM__DEBUG_ERROR(error);

		return nullptr;
	}

	Xenium::Window* window = new Xenium::Window(this, (void*)&data);

	windowReadyWait(data.window_id);

	return window;
}


void Xenium::windowCreate(Xenium::WindowCreateData* window_data
	) noexcept
{
	window_data->output_id = output_map.begin()->first;
	Output& output         = output_map.begin()->second;

	if(window_data->size_unit == Xenium::SizeUnit::Millimeter)
	{
		auto pixel = convertMmToPixel(output
			, window_data->size_mm.width
			, window_data->size_mm.height
			);
		window_data->size_pixel = {pixel.first, pixel.second};

		auto percent = convertPixelToPercent(output
			, window_data->size_pixel.width
			, window_data->size_pixel.height
			);
		window_data->size_percent = {percent.first, percent.second};
	}
	else if(window_data->size_unit == Xenium::SizeUnit::Percent)
	{
		auto pixel = convertPercentToPixel(output
			, window_data->size_percent.width
			, window_data->size_percent.height
			);
		window_data->size_pixel = {pixel.first, pixel.second};

		auto mm = convertPixelToMm(output
			, window_data->size_pixel.width
			, window_data->size_pixel.height
			);
		window_data->size_mm = {mm.first, mm.second};
	}
	else if(window_data->size_unit == Xenium::SizeUnit::Pixel)
	{
		auto mm = convertPixelToMm(output
			, window_data->size_pixel.width
			, window_data->size_pixel.height
			);
		window_data->size_mm = {mm.first, mm.second};

		auto percent = convertPixelToPercent(output
			, window_data->size_pixel.width
			, window_data->size_pixel.height
			);
		window_data->size_percent = {percent.first, percent.second};
	}
	else
	{
		window_data->error = ZAKERO_XENIUM__ERROR(Error_Unknown);

		return;
	}

	xcbWindowCreate(window_data);

	if(window_data->error)
	{
		return;
	}

	xcbWindowInit(window_data);
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
	return windowCreate(size, Default_Value_Mask, Default_Value_List, error);
}


/**
 * \brief Create a window.
 *
 * Create a new Window with the specified \p size and values.  The \p 
 * value_mask and \p value_list work together.  The \p value_list is a 
 * collection of values that will be applied to the Window.  The \p value_mask 
 * bit field determines which values will be used in the \p value_list.
 *
 * The \p error parameter will be set to Error_None if the window was 
 * successfully created or it will be set to an appropriate error code.
 *
 * \note The size of a window __must__ be greater than `0`.
 *
 * \return A pointer to the new Window or `nullptr` on error.
 *
 * \thread_user
 */
Xenium::Window* Xenium::windowCreate(const Xenium::SizePercent& size_percent ///< The window size
	, const uint32_t                                        value_mask   ///< The value mask
	, xcb_create_window_value_list_t&                       value_list   ///< The value list
	, std::error_code&                                      error        ///< The error state
	) noexcept
{
	Xenium::WindowCreateData data =
	{	.barrier        = {}
	,	.error          = {}
	,	.window_id      = 0
	,	.output_id      = 0
	,	.atom           = 0
	,	.size_unit      = Xenium::SizeUnit::Percent
	,	.size_mm        = {}
	,	.size_percent   = size_percent
	,	.size_pixel     = {}
	,	.value_mask     = value_mask
	,	.value_list     = value_list
	};

	std::future<void> barrier = data.barrier.get_future();

	windowCreateAddToQueue(&data);

	barrier.wait();

	if(data.error)
	{
		error = data.error;

		ZAKERO_XENIUM__DEBUG_ERROR(error);

		return nullptr;
	}

	Xenium::Window* window = new Xenium::Window(this, (void*)&data);

	windowReadyWait(data.window_id);

	return window;
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
	return windowCreate(size, Default_Value_Mask, Default_Value_List, error);
}


/**
 * \brief Create a window.
 *
 * Create a new Window with the specified \p size and values.  The \p 
 * value_mask and \p value_list work together.  The \p value_list is a 
 * collection of values that will be applied to the Window.  The \p value_mask 
 * bit field determines which values will be used in the \p value_list.
 *
 * The \p error parameter will be set to Error_None if the window was 
 * successfully created or it will be set to an appropriate error code.
 *
 * \note The size of a window __must__ be greater than `0`.
 *
 * \return A pointer to the new Window or `nullptr` on error.
 *
 * \thread_user
 */
Xenium::Window* Xenium::windowCreate(const Xenium::SizePixel& size_pixel ///< The window size
	, const uint32_t                                      value_mask ///< The value mask
	, xcb_create_window_value_list_t&                     value_list ///< The value list
	, std::error_code&                                    error      ///< The error state
	) noexcept
{
	Xenium::WindowCreateData data =
	{	.barrier        = {}
	,	.error          = {}
	,	.window_id      = 0
	,	.output_id      = 0
	,	.atom           = 0
	,	.size_unit      = Xenium::SizeUnit::Pixel
	,	.size_mm        = {}
	,	.size_percent   = {}
	,	.size_pixel     = size_pixel
	,	.value_mask     = value_mask
	,	.value_list     = value_list
	};

	std::future<void> barrier = data.barrier.get_future();

	windowCreateAddToQueue(&data);

	barrier.wait();

	if(data.error)
	{
		error = data.error;

		ZAKERO_XENIUM__DEBUG_ERROR(error);

		return nullptr;
	}

	Xenium::Window* window = new Xenium::Window(this, (void*)&data);

	windowReadyWait(data.window_id);

	return window;
}


void Xenium::xcbWindowDestroy(Xenium::WindowDestroyData* window_data
	) noexcept
{
	xcb_destroy_window(this->connection, window_data->window_id);

	window_ready_map.erase(window_data->window_id);
	window_output_map.erase(window_data->window_id);
	window_on_motion_map.erase(window_data->window_id);
	window_on_leave_map.erase(window_data->window_id);
	window_keyboard.erase(window_data->window_id);
	window_on_key_map.erase(window_data->window_id);
	window_on_enter_map.erase(window_data->window_id);
	window_on_button_map.erase(window_data->window_id);
	window_on_axis_map.erase(window_data->window_id);
	window_focus_map.erase(window_data->window_id);
	window_delete_map.erase(window_data->window_id);
	window_decorations_map.erase(window_data->window_id);
	window_map.erase(window_data->window_id);
}


bool Xenium::windowPropertySet(WindowId window_id
	, const xcb_atom_t              property
	, const xcb_atom_t              value
	, xcb_generic_error_t&          generic_error
	) noexcept
{
	xcb_void_cookie_t void_cookie =
		xcb_change_property_checked(this->connection
			, XCB_PROP_MODE_REPLACE // mode
			, window_id             // window
			, property              // property
			, XCB_ATOM_ATOM         // type
			, 32                    // format : 32-bit pointer
			, 1                     // data_len
			, &value                // data
			);

	if(requestCheckHasError(void_cookie, generic_error))
	{
		ZAKERO_XENIUM__DEBUG_VAR(to_string(generic_error));
		return false;
	}

	return true;
}


bool Xenium::windowPropertySet(WindowId window_id
	, const xcb_atom_t              property
	, const std::string&            value
	, xcb_generic_error_t&          generic_error
	) noexcept
{
	xcb_void_cookie_t void_cookie =
		xcb_change_property_checked(this->connection
			, XCB_PROP_MODE_REPLACE // mode
			, window_id             // window
			, property              // property
			, XCB_ATOM_STRING       // type
			, 8                     // format : 32-bit pointer
			, value.length()        // data_len
			, value.data()          // data
			);
	if(requestCheckHasError(void_cookie, generic_error))
	{
		ZAKERO_XENIUM__DEBUG_VAR(to_string(generic_error));
		return false;
	}

	return true;
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
Xenium::Window::Window(Xenium* xenium
	, void*                data
	)
	: xenium(xenium)
	, frame_buffer(nullptr)
	, frame_buffer_size(0, 0)
	, window_id(((WindowCreateData*)data)->window_id)
	, gc(((WindowCreateData*)data)->gc)
	, frame_buffer_length(0)
	, frame_time(0)
{
}


/**
 * \brief Destroy a %Window.
 */
Xenium::Window::~Window()
{
	Xenium::WindowDestroyData data =
	{	.barrier   = {}
	,	.window_id = this->window_id
	};

	std::future<void> barrier = data.barrier.get_future();

	xenium->windowDestroyAddToQueue(&data);

	barrier.wait();

	xcb_free_gc(xenium->connection, gc);

	ZAKERO_FREE(frame_buffer);

	window_id = 0;
	xenium    = nullptr;
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
void Xenium::Window::classSet(const std::string& class_name ///< The class name
	) noexcept
{
	bool                success;
	xcb_generic_error_t generic_error;

	success = xenium->windowPropertySet(window_id
		, XCB_ATOM_WM_CLASS
		, class_name
		, generic_error
		);

	if(success == false)
	{
		ZAKERO_XENIUM__DEBUG_VAR(to_string(generic_error));
	}
}


/**
 * \brief Change the window title.
 *
 * The window's title can be changed by using this method.  Changing the title 
 * does not change the window's name.
 */
void Xenium::Window::titleSet(const std::string& title ///< The window title
	) noexcept
{
	bool                success;
	xcb_generic_error_t generic_error;

	success = xenium->windowPropertySet(window_id
		, XCB_ATOM_WM_NAME
		, title
		, generic_error
		);

	if(success == false)
	{
		ZAKERO_XENIUM__DEBUG_VAR(to_string(generic_error));
	}
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
std::error_code Xenium::Window::decorationsSet(const Xenium::WindowDecorations decorations ///< The requested decorations
	) noexcept
{
	std::error_code error = ZAKERO_XENIUM__ERROR(Error_None);

	if(decorations == WindowDecorations::Client_Side)
	{
		// The application will render the borders
		error = xenium->windowBorder(window_id, false);
	}
	else
	{
		// The X11 will render the borders
		error = xenium->windowBorder(window_id, true);
	}

	return error;
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
void Xenium::Window::decorationsOnChange(Xenium::LambdaWindowDecorations lambda ///< The lambda
	) noexcept
{
	WindowDecorationsData& window_decorations = xenium->window_decorations_map[window_id];

	if(lambda)
	{
		window_decorations.lambda = lambda;
	}
	else
	{
		window_decorations.lambda = LambdaWindowDecorations_DoNothing;
	}
}

// }}}
// {{{ Window : Size

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
 *
 * \thread_user
 */
std::error_code Xenium::Window::sizeSet(const Xenium::SizeMm& size ///< The %Window size
	) noexcept
{
	std::lock_guard lock(xenium->output_mutex);

	Output& output = xenium->output_map[xenium->window_output_map[window_id]];

	Xenium::WindowSizeData& window_size = xenium->window_size_map[window_id];
	window_size.unit = SizeUnit::Millimeter;

	auto pixel = xenium->convertMmToPixel(output
		, size.width
		, size.height
		);

	if(pixel.first < Window_Size_Minimum
		|| pixel.second < Window_Size_Minimum
		)
	{
		return ZAKERO_XENIUM__ERROR(Error_Window_Size_Too_Small);
	}

	if(pixel.first == window_size.pixel.width
		&& pixel.second == window_size.pixel.height
		)
	{
		return ZAKERO_XENIUM__ERROR(Error_None);
	}

	SizePixel size_pixel = {pixel.first, pixel.second};

	std::error_code error = xenium->windowSizeSet(window_id, size_pixel);

	return error;
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
 *
 * \thread_user
 */
std::error_code Xenium::Window::sizeSet(const Xenium::SizePercent& size ///< The %Window size
	) noexcept
{
	std::lock_guard lock(xenium->output_mutex);

	Output& output = xenium->output_map[xenium->window_output_map[window_id]];

	Xenium::WindowSizeData& window_size = xenium->window_size_map[window_id];
	window_size.unit = SizeUnit::Percent;

	auto pixel = xenium->convertPercentToPixel(output
		, size.width
		, size.height
		);

	if(pixel.first < Window_Size_Minimum
		|| pixel.second < Window_Size_Minimum
		)
	{
		return ZAKERO_XENIUM__ERROR(Error_Window_Size_Too_Small);
	}

	if(pixel.first == window_size.pixel.width
		&& pixel.second == window_size.pixel.height
		)
	{
		return ZAKERO_XENIUM__ERROR(Error_None);
	}

	SizePixel size_pixel = {pixel.first, pixel.second};

	std::error_code error = xenium->windowSizeSet(window_id, size_pixel);

	return error;
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
 * \note This method __will__ trigger a Resize Event.
 *
 * \return An error code.  If there was no error, then `error_code.value() == 
 * 0`.
 *
 * \thread_user
 */
std::error_code Xenium::Window::sizeSet(const Xenium::SizePixel& size ///< The %Window size
	) noexcept
{
	if(size.width < Window_Size_Minimum
		|| size.height < Window_Size_Minimum
		)
	{
		return ZAKERO_XENIUM__ERROR(Error_Window_Size_Too_Small);
	}

	Xenium::WindowSizeData& window_size = xenium->window_size_map[window_id];
	window_size.unit = Xenium::SizeUnit::Pixel;

	if(window_size.pixel.width == size.width
		&& window_size.pixel.height == size.height
		)
	{
		return ZAKERO_XENIUM__ERROR(Error_None);
	}

	std::error_code error = xenium->windowSizeSet(window_id, size);

	return error;
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
 *
 * \thread_user
 */
std::error_code Xenium::Window::sizeSetMinMax(const Xenium::SizeMm& size_min ///< The minimum size
	, const Xenium::SizeMm&                                     size_max ///< The maximum size
	) noexcept
{
	std::error_code error;

	error = validateMinMax<Xenium::SizeMm>(size_min, size_max);
	if(error)
	{
		return error;
	}

	std::lock_guard lock(xenium->output_mutex);

	Xenium::WindowSizeData& window_size = xenium->window_size_map[window_id];

	window_size.unit       = Xenium::SizeUnit::Millimeter;
	window_size.mm_minimum = size_min;
	window_size.mm_maximum = size_max;

	Output& output = xenium->output_map[xenium->window_output_map[window_id]];

	error = xenium->windowSizeSetMinMax(output, window_id, window_size);

	return error;
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
 *
 * \thread_user
 */
std::error_code Xenium::Window::sizeSetMinMax(const Xenium::SizePercent& size_min ///< The minimum size
	, const Xenium::SizePercent&                                     size_max ///< The maximum size
	) noexcept
{
	std::error_code error;

	error = validateMinMax<Xenium::SizePercent>(size_min, size_max);
	if(error)
	{
		return error;
	}

	std::lock_guard lock(xenium->output_mutex);

	Xenium::WindowSizeData& window_size = xenium->window_size_map[window_id];

	window_size.unit            = Xenium::SizeUnit::Percent;
	window_size.percent_minimum = size_min;
	window_size.percent_maximum = size_max;

	Output& output = xenium->output_map[xenium->window_output_map[window_id]];

	error = xenium->windowSizeSetMinMax(output, window_id, window_size);

	return error;
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
 *
 * \thread_user
 */
std::error_code Xenium::Window::sizeSetMinMax(const Xenium::SizePixel& size_min ///< The minimum size
	, const Xenium::SizePixel&                                     size_max ///< The maximum size
	) noexcept
{
	std::error_code error;

	error = validateMinMax<Xenium::SizePixel>(size_min, size_max);
	if(error)
	{
		return error;
	}

	std::lock_guard lock(xenium->output_mutex);

	Xenium::WindowSizeData& window_size = xenium->window_size_map[window_id];

	window_size.unit          = Xenium::SizeUnit::Pixel;
	window_size.pixel_minimum = size_min;
	window_size.pixel_maximum = size_max;

	Output& output = xenium->output_map[xenium->window_output_map[window_id]];

	error = xenium->windowSizeSetMinMax(output, window_id, window_size);

	return error;
}


std::error_code Xenium::windowSizeSetMinMax(const Xenium::Output& output      ///< The output device
	, const Xenium::WindowId                                  window_id
	, Xenium::WindowSizeData&                                     window_size ///< The window size
	) noexcept
{
	if(window_size.unit == SizeUnit::Millimeter)
	{
		// --- Window Minimum Size --- //
		auto pixel = convertMmToPixel(output
			, window_size.mm_minimum.width
			, window_size.mm_minimum.height
			);
		window_size.pixel_minimum = {pixel.first, pixel.second};

		auto percent = convertPixelToPercent(output
			, window_size.pixel_minimum.width
			, window_size.pixel_minimum.height
			);
		window_size.percent_minimum = {percent.first, percent.second};

		// --- Window Maximum Size --- //
		pixel = convertMmToPixel(output
			, window_size.mm_maximum.width
			, window_size.mm_maximum.height
			);
		window_size.pixel_maximum = {pixel.first, pixel.second};

		percent = convertPixelToPercent(output
			, window_size.pixel_maximum.width
			, window_size.pixel_maximum.height
			);
		window_size.percent_maximum = {percent.first, percent.second};
	}
	else if(window_size.unit == SizeUnit::Percent)
	{
		// --- Window Minimum Size --- //
		auto pixel = convertPercentToPixel(output
			, window_size.percent_minimum.width
			, window_size.percent_minimum.height
			);
		window_size.pixel_minimum = {pixel.first, pixel.second};

		auto mm = convertPixelToMm(output
			, window_size.pixel_minimum.width
			, window_size.pixel_minimum.height
			);
		window_size.mm_minimum = {mm.first, mm.second};

		// --- Window Maximum Size --- //
		pixel = convertPercentToPixel(output
			, window_size.percent_maximum.width
			, window_size.percent_maximum.height
			);
		window_size.pixel_maximum = {pixel.first, pixel.second};

		mm = convertPixelToMm(output
			, window_size.pixel_maximum.width
			, window_size.pixel_maximum.height
			);
		window_size.mm_maximum = {mm.first, mm.second};
	}
	else
	{
		// --- Window Minimum Size --- //
		auto mm = convertPixelToMm(output
			, window_size.pixel_minimum.width
			, window_size.pixel_minimum.height
			);
		window_size.mm_minimum = {mm.first, mm.second};

		auto percent = convertPixelToPercent(output
			, window_size.pixel_minimum.width
			, window_size.pixel_minimum.height
			);
		window_size.percent_minimum = {percent.first, percent.second};

		// --- Window Maximum Size --- //
		mm = convertPixelToMm(output
			, window_size.pixel_maximum.width
			, window_size.pixel_maximum.height
			);
		window_size.mm_maximum = {mm.first, mm.second};

		percent = convertPixelToPercent(output
			, window_size.pixel_maximum.width
			, window_size.pixel_maximum.height
			);
		window_size.percent_maximum = {percent.first, percent.second};
	}

	// --- Set Window Min/Max Size --- //
	std::error_code error;

	error = windowSizeSetMinMax(window_id
		, window_size.pixel_minimum.width, window_size.pixel_minimum.height
		, window_size.pixel_maximum.width, window_size.pixel_maximum.height
		);

	return error;
}


std::error_code Xenium::windowSizeSetMinMax(const WindowId window_id
	, const int32_t                                    min_width
	, const int32_t                                    min_height
	, const int32_t                                    max_width
	, const int32_t                                    max_height
	) noexcept
{
	xcb_get_property_cookie_t property_cookie =
		xcb_get_property(this->connection
			, 0                        // delete
			, window_id                // window
			, XCB_ATOM_WM_NORMAL_HINTS // property
			, XCB_ATOM_WM_SIZE_HINTS   // type
			, 0                        // offset to data (32-bit)
			, 18                       // number of 32-bit values
			);

	xcb_generic_error_t* error_ptr = nullptr;
	xcb_get_property_reply_t* property_reply =
		xcb_get_property_reply(this->connection
			, property_cookie
			, &error_ptr
			);
	if(error_ptr)
	{
		ZAKERO_XENIUM__DEBUG << "Error: " << to_string(*error_ptr) << '\n';

		return ZAKERO_XENIUM__ERROR(Error_Unknown);
	}

	xcb_size_hints_t* size_hints =
		(xcb_size_hints_t*)xcb_get_property_value(property_reply);

	if(min_width == 0 && min_height == 0)
	{
		size_hints->flags &= (~XCB_ICCCM_SIZE_HINT_P_MIN_SIZE);
	}
	else
	{
		size_hints->flags |= XCB_ICCCM_SIZE_HINT_P_MIN_SIZE;
	}

	size_hints->min_width  = min_width;
	size_hints->min_height = min_height;

	if(max_width == 0 && max_height == 0)
	{
		size_hints->flags &= (~XCB_ICCCM_SIZE_HINT_P_MAX_SIZE);
	}
	else
	{
		size_hints->flags |= XCB_ICCCM_SIZE_HINT_P_MAX_SIZE;
	}

	size_hints->max_width  = max_width;
	size_hints->max_height = max_height;

	xcb_void_cookie_t cookie =
		xcb_change_property_checked(this->connection
			, XCB_PROP_MODE_REPLACE
			, window_id
			, XCB_ATOM_WM_NORMAL_HINTS
			, XCB_ATOM_WM_SIZE_HINTS
			, 32 // 32-bit values
			, 18 // 18 values
			, size_hints
			);

	xcb_generic_error_t generic_error;

	if(requestCheckHasError(cookie, generic_error))
	{
		ZAKERO_XENIUM__DEBUG << "Error: " << to_string(generic_error) << '\n';

		return ZAKERO_XENIUM__ERROR(Error_Unknown);
	}

	return ZAKERO_XENIUM__ERROR(Error_None);
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
void Xenium::Window::sizeOnChange(Xenium::LambdaSizeMm lambda ///< The lambda
	) noexcept
{
	Xenium::WindowSizeData& window_size = xenium->window_size_map[window_id];

	if(lambda)
	{
		window_size.mm_lambda = lambda;
	}
	else
	{
		window_size.mm_lambda = LambdaSizeMm_DoNothing;
	}
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
void Xenium::Window::sizeOnChange(Xenium::LambdaSizePercent lambda ///< The lambda
	) noexcept
{
	Xenium::WindowSizeData& window_size = xenium->window_size_map[window_id];

	if(lambda)
	{
		window_size.percent_lambda = lambda;
	}
	else
	{
		window_size.percent_lambda = LambdaSizePercent_DoNothing;
	}
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
void Xenium::Window::sizeOnChange(Xenium::LambdaSizePixel lambda ///< The lambda
	) noexcept
{
	Xenium::WindowSizeData& window_size = xenium->window_size_map[window_id];

	if(lambda)
	{
		window_size.pixel_lambda = lambda;
	}
	else
	{
		window_size.pixel_lambda = LambdaSizePixel_DoNothing;
	}
}

// }}}
// {{{ Window : Window Mode

/**
 * \brief Get the current WindowMode
 *
 * Provides the current WindowMode of the %Window.
 *
 * \return Xenium::WindowMode
 */
Xenium::WindowMode Xenium::Window::windowMode(
	) const noexcept
{
	return xenium->window_mode_map[window_id].window_mode;
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
	) const noexcept
{
	return (window_mode == this->windowMode());
}


/**
 * \brief Change the window mode.
 *
 * The current mode of a window can be changed programmatically by using this 
 * method.
 *
 * \see Xenium::WindowMode
 */
std::error_code Xenium::Window::windowModeSet(const Xenium::WindowMode window_mode ///< The WindowMode
	) noexcept
{
	Xenium::WindowModeData& data = xenium->window_mode_map[window_id];

	if(data.window_mode == window_mode)
	{
		return ZAKERO_XENIUM__ERROR(Error_None);
	}

	std::error_code error = xenium->windowModeSet(window_id
		, data.window_mode // current window mode
		, window_mode      // new window mode
		);

	if(error)
	{
		return error;
	}

	data.window_mode = window_mode;

	return ZAKERO_XENIUM__ERROR(Error_None);
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
	Xenium::WindowModeData& data = xenium->window_mode_map[window_id];

	if(lambda == nullptr)
	{
		data.lambda = LambdaWindowMode_DoNothing;
	}
	else
	{
		data.lambda = lambda;
	}
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
	if(frame_buffer)
	{
		free(frame_buffer);
	}

	frame_buffer_size   = xenium->window_size_map[window_id].pixel;
	frame_buffer_length = frame_buffer_size.width * frame_buffer_size.height * 4;
	frame_buffer        = (uint8_t*)malloc(sizeof(uint8_t) * frame_buffer_length);

	image = frame_buffer;
	size  = frame_buffer_size;

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
	xcb_put_image(xenium->connection
		, XCB_IMAGE_FORMAT_Z_PIXMAP
		, window_id
		, gc
		, frame_buffer_size.width
		, frame_buffer_size.height
		, 0
		, 0
		, 0
		, xenium->screen->root_depth
		, frame_buffer_length
		, frame_buffer
		);

	frame_time = ZAKERO_STEADY_TIME_NOW(milliseconds);
}


/**
 * \brief When the last frame was rendered.
 *
 * Access the time, in milliseconds, of most recent window update.  The delta 
 * between two window timestamps can be used to determine the 
 * Frames-Per-Second.
 *
 * \note This is not based on wall-time.
 *
 * \return The time.
 */
uint32_t Xenium::Window::time() const noexcept
{
	return frame_time;
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
	return 4; // ARGB8888
}


/**
 * \brief Minimize the window.
 *
 * Using this method will remove the window from view.  The user will have to 
 * use the Desktop Environment to have the window redisplayed, perhaps using 
 * the task viewer.
 */
std::error_code Xenium::Window::minimize() noexcept
{
	std::error_code error = xenium->windowMinimize(window_id);

	return error;
}

// }}}
// {{{ Window : Conversion

/**
 * \brief Unit conversion.
 *
 * The provided \p point will be converted to millimeters.
 *
 * \return The converted point.
 *
 * \thread_user
 */
Xenium::PointMm Xenium::Window::convertToMm(const Xenium::PointPixel& point ///< The point to convert
	) const noexcept
{
	std::lock_guard lock(xenium->output_mutex);

	Output& output = xenium->output_map[xenium->window_output_map[window_id]];

	auto value = xenium->convertPixelToMm(output, point.x, point.y);

	return {0, value.first, value.second};
}


/**
 * \brief Unit conversion.
 *
 * The provided \p point will be converted to a percentage.
 *
 * \return The converted point.
 *
 * \thread_user
 */
Xenium::PointPercent Xenium::Window::convertToPercent(const Xenium::PointPixel& point ///< The point to convert
	) const noexcept
{
	std::lock_guard lock(xenium->output_mutex);

	Output& output = xenium->output_map[xenium->window_output_map[window_id]];

	auto value = xenium->convertPixelToPercent(output, point.x, point.y);

	return {0, value.first, value.second};
}


/**
 * \brief Unit conversion.
 *
 * The provided \p point will be converted to pixels.
 *
 * \return The converted point.
 *
 * \thread_user
 */
Xenium::PointPixel Xenium::Window::convertToPixel(const Xenium::PointMm& point ///< The point to convert
	) const noexcept
{
	std::lock_guard lock(xenium->output_mutex);

	Output& output = xenium->output_map[xenium->window_output_map[window_id]];

	auto value = xenium->convertMmToPixel(output, point.x, point.y);

	return {0, value.first, value.second};
}


/**
 * \brief Unit conversion.
 *
 * The provided \p point will be converted to pixels.
 *
 * \return The converted point.
 *
 * \thread_user
 */
Xenium::PointPixel Xenium::Window::convertToPixel(const Xenium::PointPercent& point ///< The point to convert
	) const noexcept
{
	std::lock_guard lock(xenium->output_mutex);

	Output& output = xenium->output_map[xenium->window_output_map[window_id]];

	auto value = xenium->convertPercentToPixel(output, point.x, point.y);

	return {0, value.first, value.second};

}


/**
 * \brief Unit conversion.
 *
 * The provided \p size will be converted to millimeters.
 *
 * \return The converted size.
 *
 * \thread_user
 */
Xenium::SizeMm Xenium::Window::convertToMm(const Xenium::SizePixel& size ///< The size to convert
	) const noexcept
{
	std::lock_guard lock(xenium->output_mutex);

	Output& output = xenium->output_map[xenium->window_output_map[window_id]];

	auto value = xenium->convertPixelToMm(output, size.width, size.height);

	return {value.first, value.second};
}


/**
 * \brief Unit conversion.
 *
 * The provided \p size will be converted to a percentage.
 *
 * \return The converted size.
 *
 * \thread_user
 */
Xenium::SizePercent Xenium::Window::convertToPercent(const Xenium::SizePixel& size ///< The size to convert
	) const noexcept
{
	std::lock_guard lock(xenium->output_mutex);

	Output& output = xenium->output_map[xenium->window_output_map[window_id]];

	auto value = xenium->convertPixelToPercent(output, size.width, size.height);

	return {value.first, value.second};
}


/**
 * \brief Unit conversion.
 *
 * The provided \p size will be converted to pixels.
 *
 * \return The converted size.
 *
 * \thread_user
 */
Xenium::SizePixel Xenium::Window::convertToPixel(const Xenium::SizeMm& size ///< The size to convert
	) const noexcept
{
	std::lock_guard lock(xenium->output_mutex);

	Output& output = xenium->output_map[xenium->window_output_map[window_id]];

	auto value = xenium->convertMmToPixel(output, size.width, size.height);

	return {value.first, value.second};
}


/**
 * \brief Unit conversion.
 *
 * The provided \p size will be converted to pixels.
 *
 * \return The converted size.
 *
 * \thread_user
 */
Xenium::SizePixel Xenium::Window::convertToPixel(const Xenium::SizePercent& size ///< The size to convert
	) const noexcept
{
	std::lock_guard lock(xenium->output_mutex);

	Output& output = xenium->output_map[xenium->window_output_map[window_id]];

	auto value = xenium->convertPercentToPixel(output, size.width, size.height);

	return {value.first, value.second};
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
	WindowDeleteData& window_delete = xenium->window_delete_map[window_id];

	if(lambda == nullptr)
	{
		window_delete.close_request_lambda = Lambda_DoNothing;
	}
	else
	{
		window_delete.close_request_lambda = lambda;
	}
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
	if(lambda == nullptr)
	{
		xenium->window_focus_map[window_id] = LambdaBool_DoNothing;
	}
	else
	{
		xenium->window_focus_map[window_id] = lambda;
	}
}


/**
 * \brief Respond to "Keyboard Enter" events.
 *
 * When a window gains keyboard focus, the provided \p lambda will be called.
 */
void Xenium::Window::keyboardOnEnter(Xenium::Lambda lambda ///< The lambda
	) noexcept
{
	if(lambda == nullptr)
	{
		xenium->window_keyboard[window_id].on_enter = Lambda_DoNothing;
	}
	else
	{
		xenium->window_keyboard[window_id].on_enter = lambda;
	}
}


/**
 * \brief Respond to "Keyboard Leave" events.
 *
 * When a window loses keyboard focus, the provided \p lambda will be called.
 */
void Xenium::Window::keyboardOnLeave(Xenium::Lambda lambda ///< The lambda
	) noexcept
{
	if(lambda == nullptr)
	{
		xenium->window_keyboard[window_id].on_leave = Lambda_DoNothing;
	}
	else
	{
		xenium->window_keyboard[window_id].on_leave = lambda;
	}
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
	if(lambda == nullptr)
	{
		xenium->window_on_key_map[window_id] = LambdaKey_DoNothing;
	}
	else
	{
		xenium->window_on_key_map[window_id] = lambda;
	}
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
	WindowOnButtonData& on_button = xenium->window_on_button_map[window_id];

	if(lambda == nullptr)
	{
		on_button.lambda_mm = LambdaButtonMm_DoNothing;
	}
	else
	{
		on_button.lambda_mm = lambda;
	}
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
	WindowOnButtonData& on_button = xenium->window_on_button_map[window_id];

	if(lambda == nullptr)
	{
		on_button.lambda_percent = LambdaButtonPercent_DoNothing;
	}
	else
	{
		on_button.lambda_percent = lambda;
	}

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
	WindowOnButtonData& on_button = xenium->window_on_button_map[window_id];

	if(lambda == nullptr)
	{
		on_button.lambda_pixel = LambdaButtonPixel_DoNothing;
	}
	else
	{
		on_button.lambda_pixel = lambda;
	}
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
	WindowOnEnterData& on_enter = xenium->window_on_enter_map[window_id];

	if(lambda == nullptr)
	{
		on_enter.lambda_mm = LambdaPointMm_DoNothing;
	}
	else
	{
		on_enter.lambda_mm = lambda;
	}
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
	WindowOnEnterData& on_enter = xenium->window_on_enter_map[window_id];

	if(lambda == nullptr)
	{
		on_enter.lambda_percent = LambdaPointPercent_DoNothing;
	}
	else
	{
		on_enter.lambda_percent = lambda;
	}
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
	WindowOnEnterData& on_enter = xenium->window_on_enter_map[window_id];

	if(lambda == nullptr)
	{
		on_enter.lambda_pixel = LambdaPointPixel_DoNothing;
	}
	else
	{
		on_enter.lambda_pixel = lambda;
	}
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
	if(lambda == nullptr)
	{
		xenium->window_on_leave_map[window_id] = Lambda_DoNothing;
	}
	else
	{
		xenium->window_on_leave_map[window_id] = lambda;
	}
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
	WindowOnMotionData& on_motion = xenium->window_on_motion_map[window_id];

	if(lambda == nullptr)
	{
		on_motion.lambda_mm = LambdaPointMm_DoNothing;
	}
	else
	{
		on_motion.lambda_mm = lambda;
	}
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
	WindowOnMotionData& on_motion = xenium->window_on_motion_map[window_id];

	if(lambda == nullptr)
	{
		on_motion.lambda_percent = LambdaPointPercent_DoNothing;
	}
	else
	{
		on_motion.lambda_percent = lambda;
	}
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
	WindowOnMotionData& on_motion = xenium->window_on_motion_map[window_id];

	if(lambda == nullptr)
	{
		on_motion.lambda_pixel = LambdaPointPixel_DoNothing;
	}
	else
	{
		on_motion.lambda_pixel = lambda;
	}
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
	if(lambda == nullptr)
	{
		xenium->window_on_axis_map[window_id] = LambdaAxis_DoNothing;
	}
	else
	{
		xenium->window_on_axis_map[window_id] = lambda;
	}
}


/*
 * \brief Respond to "Pointer Axis Source" events.
 *
 * The provided \p lambda will be called when the "Pointer Axis Source" events 
 * occur.
void Xenium::Window::pointerOnAxisSource(Xenium::Lambda lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}
 */


/*
 * \brief Respond to "Pointer Axis Stop" events.
 *
 * The provided \p lambda will be called when the "Pointer Axis Stop" events 
 * occur.
void Xenium::Window::pointerOnAxisStop(Xenium::Lambda lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}
 */


/*
 * \brief Respond to "Pointer Axis Discrete" events.
 *
 * The provided \p lambda will be called when the "Pointer Axis Discrete" 
 * events occur.
void Xenium::Window::pointerOnAxisDiscrete(Xenium::Lambda lambda ///< The lambda
	) noexcept
{
	ZAKERO_UNUSED(lambda);
}
 */

// }}}
// {{{ Window : Helpers

void Xenium::windowCreateAddToQueue(Xenium::WindowCreateData* window_data
	) noexcept
{
	xenium_window_mutex.lock();

	window_to_create.push_back(window_data);

	xenium_window_mutex.unlock();
}


void Xenium::windowDestroyAddToQueue(Xenium::WindowDestroyData* window_data
	) noexcept
{
	xenium_window_mutex.lock();

	window_to_destroy.push_back(window_data);

	xenium_window_mutex.unlock();
}


/**
 * \todo After an X11 connection has been established, create the frequently 
 * used Atoms so that they don't have to be created or retrieved every time.
 */
std::error_code Xenium::windowBorder(const WindowId window_id
	, const bool                                enable
	) noexcept
{
	xcb_generic_error_t generic_error;

	xcb_atom_t motif_wm_hints_atom = internAtom("_MOTIF_WM_HINTS"
		, true
		, generic_error
		);

	if(motif_wm_hints_atom == XCB_ATOM_NONE)
	{
		ZAKERO_XENIUM__DEBUG_VAR(to_string(generic_error));
		return ZAKERO_XENIUM__ERROR(Error_Unknown);
	}

	MotifHints hints_data =
	{	.flags       = 2
	,	.functions   = 0
	,	.decorations = enable
	,	.input_mode  = 0
	,	.status      = 0
	};

	xcb_void_cookie_t void_cookie =
		xcb_change_property_checked(this->connection
			, XCB_PROP_MODE_REPLACE // mode
			, window_id             // window
			, motif_wm_hints_atom   // property
			, motif_wm_hints_atom   // type
			, 32                    // format : pointer to 32-bit data
			, 5                     // data_len
			, &hints_data           // data
			);

	if(requestCheckHasError(void_cookie, generic_error))
	{
		ZAKERO_XENIUM__DEBUG_VAR(to_string(generic_error));
		return ZAKERO_XENIUM__ERROR(Error_Unknown);
	}

	return ZAKERO_XENIUM__ERROR(Error_None);
}


/**
 * \note Not currently used.
 */
std::error_code Xenium::windowLocationSet(const WindowId window_id
	, const Xenium::PointPixel&              point      ///< The %Window size
	) noexcept
{
	xcb_configure_window_value_list_t value_list =
	{	.x            = point.x
	,	.y            = point.y
	,	.width        = 0
	,	.height       = 0
	,	.border_width = 0
	,	.sibling      = 0
	,	.stack_mode   = 0
	};

	xcb_void_cookie_t void_cookie =
		xcb_configure_window_aux_checked(this->connection
			, window_id
			, 0
				| XCB_CONFIG_WINDOW_X
				| XCB_CONFIG_WINDOW_Y
			, &value_list
			);

	xcb_generic_error_t generic_error;
	if(requestCheckHasError(void_cookie, generic_error))
	{
		ZAKERO_XENIUM__DEBUG_VAR(to_string(generic_error));

		return ZAKERO_XENIUM__ERROR(Error_Unknown);
	}

	return ZAKERO_XENIUM__ERROR(Error_None);
}


std::error_code Xenium::windowSizeSet(const WindowId window_id
	, const Xenium::SizePixel&                   size      ///< The %Window size
	) noexcept
{
	xcb_configure_window_value_list_t value_list =
	{	.x            = 0
	,	.y            = 0
	,	.width        = (uint32_t)size.width
	,	.height       = (uint32_t)size.height
	,	.border_width = 0
	,	.sibling      = 0
	,	.stack_mode   = 0
	};

	xcb_void_cookie_t void_cookie =
		xcb_configure_window_aux_checked(this->connection
			, window_id
			, 0
				| XCB_CONFIG_WINDOW_WIDTH
				| XCB_CONFIG_WINDOW_HEIGHT
			, &value_list
			);

	xcb_generic_error_t generic_error;
	if(requestCheckHasError(void_cookie, generic_error))
	{
		ZAKERO_XENIUM__DEBUG_VAR(to_string(generic_error));

		return ZAKERO_XENIUM__ERROR(Error_Unknown);
	}

	return ZAKERO_XENIUM__ERROR(Error_None);
}


void Xenium::windowResizeTo(const Output&     output
	, Xenium::WindowSizeData&                         window_size
	, const xcb_configure_notify_event_t* event ///< XCB Event
	) noexcept
{
	bool update_size = false;

	if(window_size.unit == SizeUnit::Millimeter)
	{
		auto pixel = convertMmToPixel(output
			, window_size.mm.width
			, window_size.mm.height
			);

		if(pixel.first != window_size.pixel.width
			|| pixel.second != window_size.pixel.height
			)
		{
			update_size = true;
		}

		window_size.pixel = {pixel.first, pixel.second};

		auto percent = convertPixelToPercent(output
			, window_size.pixel.width
			, window_size.pixel.height
			);
		window_size.percent = {percent.first, percent.second};
	}
	else if(window_size.unit == SizeUnit::Percent)
	{
		auto pixel = convertPercentToPixel(output
			, window_size.percent.width
			, window_size.percent.height
			);

		if(pixel.first != window_size.pixel.width
			|| pixel.second != window_size.pixel.height
			)
		{
			update_size = true;
			window_size.pixel = {pixel.first, pixel.second};
		}

		auto mm = convertPixelToMm(output
			, window_size.pixel.width
			, window_size.pixel.height
			);

		window_size.mm = {mm.first, mm.second};
	}
	else
	{
		if(event->width != window_size.pixel.width
			|| event->height != window_size.pixel.height
			)
		{
			update_size = true;
		}

		window_size.pixel = {event->width, event->height};

		auto mm = convertPixelToMm(output
			, window_size.pixel.width
			, window_size.pixel.height
			);
		window_size.mm = {mm.first, mm.second};

		auto percent = convertPixelToPercent(output
			, window_size.pixel.width
			, window_size.pixel.height
			);
		window_size.percent = {percent.first, percent.second};
	}

	windowSizeSetMinMax(output, event->window, window_size);

	if(update_size)
	{
		window_size.pixel_lambda(window_size.pixel);
		window_size.percent_lambda(window_size.percent);
		window_size.mm_lambda(window_size.mm);

		windowSizeSet(event->window, window_size.pixel);
	}
}

// }}}
// {{{ XCB

void Xenium::xcbEvent(const xcb_client_message_event_t* event
	) noexcept
{
//std::cout << "Client Message:  " << to_string(*event) << '\n';
	if(window_delete_map.contains(event->window))
	{
		WindowDeleteData& window_delete = window_delete_map[event->window];
		if(event->data.data32[0] == window_delete.atom)
		{
			window_delete.close_request_lambda();
		}
	}
}


void Xenium::xcbEvent(const xcb_button_press_event_t* event
	) noexcept
{
//std::cout << "Button Press:  " << to_string(*event) << '\n';

	const WindowId window_id = event->event;

	uint32_t button_code = event->detail;

	if(button_code <= 3 || button_code >= 8)
	{
		if(button_code > 9)
		{
			button_code = 0;
		}
		else if(button_code >= 8)
		{
			button_code -= 5;
		}
		else
		{
			button_code--;
		}

		PointerButton button =
		{	.code = Pointer_Button_Event_Code[button_code]
		,	.state = event->response_type == XCB_BUTTON_PRESS
				? Xenium::PointerButtonState::Pressed
				: Xenium::PointerButtonState::Released
		};

		OutputId output_id = window_output_map[window_id];
		Output output = output_map.at(output_id);

		PointPixel point_pixel = {0, event->event_x, event->event_y};

		auto mm = convertPixelToMm(output
			, event->event_x
			, event->event_y
			);
		PointMm point_mm = {0, mm.first, mm.second};

		Xenium::WindowSizeData& window_size = window_size_map[window_id];
		PointPercent point_percent =
		{	event->time
		,	(float)event->event_x / (float)window_size.pixel.width
		,	(float)event->event_y / (float)window_size.pixel.height
		};

		window_on_button_map[window_id].lambda_mm(button, point_mm, key_modifier);
		window_on_button_map[window_id].lambda_percent(button, point_percent, key_modifier);
		window_on_button_map[window_id].lambda_pixel(button, point_pixel, key_modifier);
	}
	else if(event->response_type == XCB_BUTTON_PRESS)
	{
		if(button_code == 4 || button_code == 5)
		{
			PointerAxis pointer_axis =
			{	.time     = event->time
			,	.steps    = button_code == 4 ? -1 : 1
			,	.distance = button_code == 4 ? -15.0f : 15.0f
			,	.source   = PointerAxisSource::Wheel
			,	.type     = PointerAxisType::Vertical
			};

			window_on_axis_map[window_id](pointer_axis, key_modifier);
		}
		else if(button_code == 6 || button_code == 7)
		{
			PointerAxis pointer_axis =
			{	.time     = event->time
			,	.steps    = button_code == 6 ? -1 : 1
			,	.distance = button_code == 6 ? -15.0f : 15.0f
			,	.source   = PointerAxisSource::Wheel
			,	.type     = PointerAxisType::Horizontal
			};

			window_on_axis_map[window_id](pointer_axis, key_modifier);
		}
	}
}


/**
 * \brief XCB Event handler
 *
 * Many modern X11 Servers send two Configure Notify events:
 * - The "raw" location, with X and Y set to 0
 * - The "compositor" location, with X and Y set to screen coordinates
 *
 * To tell the difference, check the MSB of the `response_type`.  If the bit is 
 * `0`, then the Configuration Notify is the "raw" configuration.  If the bit 
 * is `1`, then the Configuration Notify is from the compositor.
 *
 * This convention is use with the following X11 Servers
 * - KWin (KDE Plasma)
 * - KWin (KDE Wayland with X11 support)
 *
 * Unconfirmed X11 Servers
 * - Enlightenment
 * - Enlightenment (e16)
 * - Gnome
 * - Metacity
 * - Openbox
 * - Xfwm
 *
 * \thread_xenium
 */
void Xenium::xcbEvent(const xcb_configure_notify_event_t* event ///! XCB Event
	) noexcept
{
//std::cout << "Configue Notify: " << to_string(*event) << '\n';
	if((event->response_type & 0x80) == 0)
	{
		// Only care about events with the "synthetic bit" set
		return;
	}

	const WindowId window_id = event->window;

	auto iter = window_size_map.find(window_id);

	if(iter == std::end(window_size_map))
	{
		return;
	}

	Xenium::WindowSizeData& window_size = iter->second;

	OutputId output_id;
	const Output& output = this->output(event->x, event->y, output_id);

	if(window_output_map[window_id] != output_id)
	{
		window_output_map[window_id] = output_id;

		windowResizeTo(output, window_size, event);

		return;
	}

	if(window_size.pixel.width == event->width
		&& window_size.pixel.height == event->height
		)
	{
		return;
	}

	window_size.pixel = {event->width, event->height};

	auto mm = convertPixelToMm(output
		, window_size.pixel.width
		, window_size.pixel.height
		);
	window_size.mm = {mm.first, mm.second};

	auto percent = convertPixelToPercent(output
		, window_size.pixel.width
		, window_size.pixel.height
		);
	window_size.percent = {percent.first, percent.second};

	window_size.pixel_lambda(window_size.pixel);
	window_size.percent_lambda(window_size.percent);
	window_size.mm_lambda(window_size.mm);
}


/**
 * \bug X11 emulation in Wayland maybe broken with KDE/kwin
 *      - When a window loses focus, an additional Enter notify is generated
 *      - When click in a window that has focus, generates an Enter Notify
 */
void Xenium::xcbEvent(const xcb_enter_notify_event_t* event
	) noexcept
{
//std::cout << "Enter Notify:    " << to_string(*event) << '\n';

	const WindowId window_id = event->event;

	if(event->response_type == XCB_LEAVE_NOTIFY)
	{
		window_on_leave_map[window_id]();
		window_keyboard[window_id].on_leave();
		return;
	}

	xkbIndicatorStateUpdate();

	OutputId output_id = window_output_map[window_id];
	Output output = output_map.at(output_id);

	PointPixel point_pixel = {0, event->event_x, event->event_y};

	auto mm = convertPixelToMm(output
		, event->event_x
		, event->event_y
		);
	PointMm point_mm = {0, mm.first, mm.second};

	Xenium::WindowSizeData& window_size = window_size_map[window_id];
	PointPercent point_percent =
	{	0
	,	(float)event->event_x / (float)window_size.pixel.width
	,	(float)event->event_y / (float)window_size.pixel.height
	};

	window_on_enter_map[window_id].lambda_mm(point_mm, key_modifier);
	window_on_enter_map[window_id].lambda_percent(point_percent, key_modifier);
	window_on_enter_map[window_id].lambda_pixel(point_pixel, key_modifier);

	window_keyboard[window_id].on_enter();
}


void Xenium::xcbEvent(const xcb_expose_event_t* event
	) noexcept
{
//std::cout << "Expose:          " << to_string(*event) << '\n';
	windowReadySet(event->window);
}


void Xenium::xcbEvent(const xcb_focus_in_event_t* event
	) noexcept
{
//std::cout << "Pocus:           " << to_string(*event) << '\n';

	const WindowId window_id = event->event;

	if(window_focus_map.contains(window_id) == false)
	{
		return;
	}

	if(event->response_type == XCB_FOCUS_IN)
	{
		xkbControlsUpdate();
		xkbIndicatorStateUpdate();
		window_focus_map[window_id](true);
	}
	else
	{
		keyDataArrayClear();
		window_focus_map[window_id](false);
	}
}


void Xenium::xcbEvent(const xcb_gravity_notify_event_t* event
	) noexcept
{
//std::cout << "Gravity Notify:  " << to_string(*event) << '\n';
}


/**
 * \brief XCB Event Handler.
 *
 * \par Phantom XCB Key State
 * \parblock
 * The `xcb_xkb_press_event_t` field `state` is used by XCB to track the key 
 * modifiers, however this functionality can not be used.  The problem is that 
 * the modifier `state` is still set to true even after the modifier key has 
 * been released.
 *
 * From the point of veiw of the event handler, the `state` value is only reset
 * on the next key press event which results in a _phantom_ `state` between key 
 * release and press events.
 *
 * For key events this `state` behavior is fine but for mouse/pointer input, 
 * the key modifiers reflect the _phantom_ `state` and not the current state.  
 * To work around this issue, the key modifiers are tracked manually.
 *
 * An example of this problem:
 * - Press a shift key
 * - Record the modifier `state` (No Shift key state)
 * - Move the mouse, get the modifier `state` (No Shift key state)
 * - Release the shift key
 * - Record the modifier `state` (Shift key state is `true`)
 * - Move the mouse, get the modifier `state` (Shift key state is `true`)
 * \endparblock
 */
void Xenium::xcbEvent(const xcb_key_press_event_t* event
	) noexcept
{
//std::cout << "Key Press:       " << to_string(*event) << '\n';

	uint32_t key_code  = (uint32_t)event->detail - 8;

	constexpr uint16_t CapsLock      = 0b0000'0010'0000'0000;
	constexpr uint16_t NumLock       = 0b0000'0001'0000'0000;
	constexpr uint16_t Alt_Left      = 0b0000'0000'1000'0000;
	constexpr uint16_t Alt_Right     = 0b0000'0000'0100'0000;
	constexpr uint16_t Control_Left  = 0b0000'0000'0010'0000;
	constexpr uint16_t Control_Right = 0b0000'0000'0001'0000;
	constexpr uint16_t Meta_Left     = 0b0000'0000'0000'1000;
	constexpr uint16_t Meta_Right    = 0b0000'0000'0000'0100;
	constexpr uint16_t Shift_Left    = 0b0000'0000'0000'0010;
	constexpr uint16_t Shift_Right   = 0b0000'0000'0000'0001;

	if(event->response_type == XCB_KEY_PRESS)
	{
		switch(key_code)
		{
			case KEY_CAPSLOCK:
				xkbIndicatorStateUpdate();
				xkb_modifier_pressed |= CapsLock;
				break;

			case KEY_NUMLOCK:
				xkbIndicatorStateUpdate();
				xkb_modifier_pressed |= NumLock;
				break;

			case KEY_LEFTALT:
				xkb_modifier_pressed |= Alt_Left;
				break;

			case KEY_RIGHTALT:
				xkb_modifier_pressed |= Alt_Right;
				break;

			case KEY_LEFTCTRL:
				xkb_modifier_pressed |= Control_Left;
				break;

			case KEY_RIGHTCTRL:
				xkb_modifier_pressed |= Control_Right;
				break;

			case KEY_LEFTMETA:
				xkb_modifier_pressed |= Meta_Left;
				break;

			case KEY_RIGHTMETA:
				xkb_modifier_pressed |= Meta_Right;
				break;

			case KEY_LEFTSHIFT:
				xkb_modifier_pressed |= Shift_Left;
				break;

			case KEY_RIGHTSHIFT:
				xkb_modifier_pressed |= Shift_Right;
				break;
		}
	}
	else
	{
		switch(key_code)
		{
			case KEY_CAPSLOCK:
				xkbIndicatorStateUpdate();
				xkb_modifier_pressed &= (~CapsLock);
				break;

			case KEY_NUMLOCK:
				xkbIndicatorStateUpdate();
				xkb_modifier_pressed &= (~NumLock);
				break;

			case KEY_LEFTALT:
				xkb_modifier_pressed &= (~Alt_Left);
				break;

			case KEY_RIGHTALT:
				xkb_modifier_pressed &= (~Alt_Right);
				break;

			case KEY_LEFTCTRL:
				xkb_modifier_pressed &= (~Control_Left);
				break;

			case KEY_RIGHTCTRL:
				xkb_modifier_pressed &= (~Control_Right);
				break;

			case KEY_LEFTMETA:
				xkb_modifier_pressed &= (~Meta_Left);
				break;

			case KEY_RIGHTMETA:
				xkb_modifier_pressed &= (~Meta_Right);
				break;

			case KEY_LEFTSHIFT:
				xkb_modifier_pressed &= (~Shift_Left);
				break;

			case KEY_RIGHTSHIFT:
				xkb_modifier_pressed &= (~Shift_Right);
				break;
		}
	}

	key_modifier.pressed = 0;

	if(xkb_modifier_pressed & CapsLock)
	{
		key_modifier.pressed |= KeyModifier_CapsLock;
	}

	if(xkb_modifier_pressed & NumLock)
	{
		key_modifier.pressed |= KeyModifier_NumLock;
	}

	if(xkb_modifier_pressed & (Alt_Left | Alt_Right))
	{
		key_modifier.pressed |= KeyModifier_Alt;
	}

	if(xkb_modifier_pressed & (Control_Left | Control_Right))
	{
		key_modifier.pressed |= KeyModifier_Control;
	}

	if(xkb_modifier_pressed & (Meta_Left | Meta_Right))
	{
		key_modifier.pressed |= KeyModifier_Meta;
	}

	if(xkb_modifier_pressed & (Shift_Left | Shift_Right))
	{
		key_modifier.pressed |= KeyModifier_Shift;
	}

	const WindowId window_id = event->event;

	key_data_array[key_code].window_id = window_id;

	Key& key = key_data_array[key_code].key;
	key.code = key_code;

	if(event->response_type == XCB_KEY_PRESS)
	{
		key_data_array[key_code].modifier = key_modifier;

		if(key.time == event->time)
		{
			key.state = KeyState::Repeat;
		}
		else
		{
			key.time  = event->time;
			key.state = KeyState::Pressed;

			key_data_array[key_code].repeat_time =
				event->time + xkb_controls.repeat_delay_ms
				;

			window_on_key_map[window_id](key, key_modifier);
		}
	}
	/* If another window is active and a key is pressed then the cursor is 
	 * moved into a Xenium window while the key remains pressed, it is 
	 * possible for the first key event to be recieved is the 
	 * XCB_KEY_RELEASE.  This event must be ignored because there was no 
	 * precedding XCB_KEY_PRESS event.  To identify the bad XCB_KEY_RELEASE 
	 * event from the good event, the key.time value will be `0` if the the 
	 * pressed event is missing.  A non-`0` key.time value means there was 
	 * a preceding XCB_KEY_PRESS event.
	 */
	else if(key.time != 0) // && (event->response_type == XCB_KEY_RELEASE)
	{
		key.time  = event->time;
		key.state = KeyState::Released;

		key_data_array[key_code].modifier = key_modifier;
	}
}


void Xenium::xcbEvent(const xcb_map_notify_event_t* event
	) noexcept
{
//std::cout << "Map Netify:      " << to_string(*event) << '\n';
}


void Xenium::xcbEvent(const xcb_motion_notify_event_t* event
	) noexcept
{
//std::cout << "Motion Notify:   " << to_string(*event) << '\n';

	const WindowId window_id = event->event;

	OutputId output_id = window_output_map[window_id];
	Output output = output_map.at(output_id);

	PointPixel point_pixel = {0, event->event_x, event->event_y};

	auto mm = convertPixelToMm(output
		, event->event_x
		, event->event_y
		);
	PointMm point_mm = {0, mm.first, mm.second};

	Xenium::WindowSizeData& window_size = window_size_map[window_id];
	PointPercent point_percent =
	{	0
	,	(float)event->event_x / (float)window_size.pixel.width
	,	(float)event->event_y / (float)window_size.pixel.height
	};

	window_on_motion_map[window_id].lambda_mm(point_mm, key_modifier);
	window_on_motion_map[window_id].lambda_percent(point_percent, key_modifier);
	window_on_motion_map[window_id].lambda_pixel(point_pixel, key_modifier);
}


void Xenium::xcbEvent(const xcb_property_notify_event_t* event
	) noexcept
{
//std::cout << "Property Notify: " << to_string(*event) << '\n';

	xcb_generic_error_t generic_error = {0};

	if(event->atom == atom_net_frame_extents)
	{
		std::vector<int32_t> atom_data = Xenium::atomValueData(event->window
			, atom_net_frame_extents
			, XCB_ATOM_CARDINAL
			, 4
			, generic_error
			);

		struct
		{
			int32_t left;
			int32_t right;
			int32_t top;
			int32_t bottom;
		}
		frame_extents =
		{	.left   = atom_data[0]
		,	.right  = atom_data[1]
		,	.top    = atom_data[2]
		,	.bottom = atom_data[3]
		};

		WindowDecorationsData& window = window_decorations_map[event->window];

		WindowDecorations window_decorations = window.window_decorations;

		if(frame_extents.left == 0
			&& frame_extents.right == 0
			&& frame_extents.top == 0
			&& frame_extents.bottom == 0
			)
		{
			window.window_decorations = WindowDecorations::Client_Side;
		}
		else
		{
			window.window_decorations = WindowDecorations::Server_Side;
		}

		if(window.window_decorations != window_decorations)
		{
			window.lambda(window.window_decorations);
		}

		return;
	}

	if(event->atom == atom_net_wm_state)
	{
		std::vector<xcb_atom_t> value =
			atomValueAtom(event->window, event->atom, generic_error);

		WindowMode new_window_mode = WindowMode::Normal;

		if(zakero::vectorContains(value, atom_net_wm_state_maximized_horz)
			&& zakero::vectorContains(value, atom_net_wm_state_maximized_vert)
			)
		{
			new_window_mode = WindowMode::Maximized;
		}
		else if(zakero::vectorContains(value, atom_net_wm_state_fullscreen))
		{
			new_window_mode = WindowMode::Fullscreen;
		}

		if(window_mode_map.contains(event->window))
		{
			WindowModeData& data = window_mode_map[event->window];
			LambdaWindowMode lambda = LambdaWindowMode_DoNothing;
			if(data.window_mode != new_window_mode)
			{
				data.window_mode = new_window_mode;
				lambda = data.lambda;
			}

			lambda(new_window_mode);
		}

		return;
	}
}

void Xenium::xcbEvent(const xcb_reparent_notify_event_t* event
	) noexcept
{
//std::cout << "Reparent Notify: " << to_string(*event) << '\n';
}


void Xenium::xcbEvent(const xcb_unmap_notify_event_t* event
	) noexcept
{
//std::cout << "Unmap Notify:    " << to_string(*event) << '\n';
}


void Xenium::xcbWindowCreate(Xenium::WindowCreateData* data
	) noexcept
{
	data->window_id = xcb_generate_id(this->connection);

	xcb_void_cookie_t cookie =
		xcb_create_window_aux_checked(this->connection
			, this->screen->root_depth      // depth
			, data->window_id               // requested id
			, this->screen->root            // parent window id
			, 0, 0                          // location x,y
			, data->size_pixel.width        // width
			, data->size_pixel.height       // height
			, 0                             // border width
			, XCB_WINDOW_CLASS_INPUT_OUTPUT // "class"
			, this->screen->root_visual     // visual
			, data->value_mask
			, &data->value_list
			);

	xcb_generic_error_t generic_error;

	if(requestCheckHasError(cookie, generic_error))
	{
		ZAKERO_XENIUM__DEBUG << "Error: " << to_string(generic_error) << '\n';

		data->error = ZAKERO_XENIUM__ERROR(Error_Unknown);

		return;
	}

	data->atom = atomCreateDeleteWindow(data->window_id
		, generic_error
		);

	if(data->atom == XCB_ATOM_NONE)
	{
		ZAKERO_XENIUM__DEBUG << "Error: " << to_string(generic_error) << '\n';

		xcb_destroy_window(this->connection, data->window_id);
		data->window_id = 0;

		data->error = ZAKERO_XENIUM__ERROR(Error_Unknown);

		return;
	}

	xcb_size_hints_t size_hints = { 0 };
	xcb_change_property_checked(this->connection
		, XCB_PROP_MODE_REPLACE
		, data->window_id
		, XCB_ATOM_WM_NORMAL_HINTS
		, XCB_ATOM_WM_SIZE_HINTS
		, 32 // 32-bit values
		, 18 // 18 values
		, &size_hints
		);

	if(requestCheckHasError(cookie, generic_error))
	{
		ZAKERO_XENIUM__DEBUG << "Error: " << to_string(generic_error) << '\n';

		data->error = ZAKERO_XENIUM__ERROR(Error_Unknown);

		return;
	}

	data->error = ZAKERO_XENIUM__ERROR(Error_None);

	data->gc = xcb_generate_id(this->connection);
	cookie = xcb_create_gc_checked(this->connection
		, data->gc
		, data->window_id
		, 0
		, nullptr
		);

	if(requestCheckHasError(cookie, generic_error))
	{
		ZAKERO_XENIUM__DEBUG << "Error: " << to_string(generic_error) << '\n';

		data->error = ZAKERO_XENIUM__ERROR(Error_Unknown);

		return;
	}

	return;
}


std::error_code Xenium::windowMinimize(const Xenium::WindowId window_id
	) noexcept
{
	xcb_client_message_event_t event =
	{	.response_type = XCB_CLIENT_MESSAGE
	,	.format        = 32
	,	.sequence      = 0
	,	.window        = window_id
	,	.type          = atom_wm_change_state
	,	.data          = { 0 }
	};
	event.data.data32[0] = XCB_ICCCM_WM_STATE_ICONIC;

	xcb_send_event(this->connection
		, 0 // do not propagate
		, this->screen->root
		, XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
		, (const char*)&event
		);

	xcb_flush(this->connection);

	return ZAKERO_XENIUM__ERROR(Error_None);
}


std::error_code Xenium::windowModeSet(const Xenium::WindowId window_id
	, const Xenium::WindowMode                           current_mode
	, const Xenium::WindowMode                           new_mode
	) noexcept
{
	xcb_client_message_event_t event =
	{	.response_type = XCB_CLIENT_MESSAGE
	,	.format        = 32
	,	.sequence      = 0
	,	.window        = window_id
	,	.type          = atom_net_wm_state
	,	.data          = { 0 }
	};

	if(current_mode == Xenium::WindowMode::Normal)
	{
		// Do nothing
	}
	else
	{
		event.data.data32[0] = _NET_WM_STATE_REMOVE;

		if(current_mode == Xenium::WindowMode::Fullscreen)
		{
			event.data.data32[1] = atom_net_wm_state_fullscreen;
			event.data.data32[2] = 0;
		}
		else // if(current_mode == Xenium::WindowMode::Maximized)
		{
			event.data.data32[1] = atom_net_wm_state_maximized_horz;
			event.data.data32[2] = atom_net_wm_state_maximized_vert;
		}

		xcb_send_event(this->connection
			, 0 // do not propagate
			, this->screen->root
			, XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
			, (const char*)&event
			);
	}

	if(new_mode == Xenium::WindowMode::Normal)
	{
		// Do nothing
	}
	else
	{
		event.data.data32[0] = _NET_WM_STATE_ADD;

		if(new_mode == Xenium::WindowMode::Fullscreen)
		{
			event.data.data32[1] = atom_net_wm_state_fullscreen;
			event.data.data32[2] = 0;
		}
		else // if(current_mode == Xenium::WindowMode::Maximized)
		{
			event.data.data32[1] = atom_net_wm_state_maximized_horz;
			event.data.data32[2] = atom_net_wm_state_maximized_vert;
		}

		xcb_send_event(this->connection
			, 0 // do not propagate
			, this->screen->root
			, XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
			, (const char*)&event
			);
	}

	xcb_flush(this->connection);

	return ZAKERO_XENIUM__ERROR(Error_None);
}

// }}}
// {{{ XCB : Atom

std::error_code Xenium::atomInit() noexcept
{
	auto cookie_wm_change_state             = internAtomRequest("WM_CHANGE_STATE");
	auto cookie_wm_delete_window            = internAtomRequest("WM_DELETE_WINDOW");
	auto cookie_wm_protocols                = internAtomRequest("WM_PROTOCOLS");
	auto cookie_net_frame_extents           = internAtomRequest("_NET_FRAME_EXTENTS");
	auto cookie_net_wm_state                = internAtomRequest("_NET_WM_STATE");
	auto cookie_net_wm_state_fullscreen     = internAtomRequest("_NET_WM_STATE_FULLSCREEN");
	auto cookie_net_wm_state_hidden         = internAtomRequest("_NET_WM_STATE_HIDDEN");
	auto cookie_net_wm_state_maximized_horz = internAtomRequest("_NET_WM_STATE_MAXIMIZED_HORZ");
	auto cookie_net_wm_state_maximized_vert = internAtomRequest("_NET_WM_STATE_MAXIMIZED_VERT");

	xcb_generic_error_t generic_error;

	atom_wm_change_state             = internAtomReply(cookie_wm_change_state, generic_error);
	atom_wm_delete_window            = internAtomReply(cookie_wm_delete_window, generic_error);
	atom_wm_protocols                = internAtomReply(cookie_wm_protocols, generic_error);
	atom_net_frame_extents           = internAtomReply(cookie_net_frame_extents, generic_error);
	atom_net_wm_state                = internAtomReply(cookie_net_wm_state, generic_error);
	atom_net_wm_state_fullscreen     = internAtomReply(cookie_net_wm_state_fullscreen, generic_error);
	atom_net_wm_state_hidden         = internAtomReply(cookie_net_wm_state_hidden, generic_error);
	atom_net_wm_state_maximized_horz = internAtomReply(cookie_net_wm_state_maximized_horz, generic_error);
	atom_net_wm_state_maximized_vert = internAtomReply(cookie_net_wm_state_maximized_vert, generic_error);

	if(atom_wm_delete_window == XCB_ATOM_NONE)
	{
		return ZAKERO_XENIUM__ERROR(Error_Xcb_WM_Delete_Window_Not_Available);
	}

	if(atom_wm_protocols == XCB_ATOM_NONE)
	{
		return ZAKERO_XENIUM__ERROR(Error_Xcb_WM_Protocols_Not_Available);
	}

	if(atom_net_wm_state == XCB_ATOM_NONE)
	{
		return ZAKERO_XENIUM__ERROR(Error_Xcb_NETWM_State_Not_Available);
	}

	if(atom_net_wm_state_fullscreen == XCB_ATOM_NONE)
	{
		return ZAKERO_XENIUM__ERROR(Error_Xcb_Fullscreen_Not_Available);
	}

	if(atom_net_wm_state_hidden == XCB_ATOM_NONE)
	{
		return ZAKERO_XENIUM__ERROR(Error_Xcb_Hidden_Not_Available);
	}

	if((atom_net_wm_state_maximized_horz == XCB_ATOM_NONE)
		|| (atom_net_wm_state_maximized_vert == XCB_ATOM_NONE)
		)
	{
		return ZAKERO_XENIUM__ERROR(Error_Xcb_Maximized_Window_Not_Available);
	}

	return ZAKERO_XENIUM__ERROR(Error_None);
}


xcb_atom_t Xenium::atomCreateDeleteWindow(const WindowId window_id
	, xcb_generic_error_t&                           generic_error
	) noexcept
{
	bool property_was_set = windowPropertySet(window_id
		, atom_wm_protocols
		, atom_wm_delete_window
		, generic_error
		);

	if(property_was_set == false)
	{
		ZAKERO_XENIUM__DEBUG_VAR(to_string(generic_error));
		return XCB_ATOM_NONE;
	}

	return atom_wm_delete_window;
}


std::string Xenium::atomName(const xcb_atom_t atom
	) noexcept
{
	if(atom == XCB_ATOM_NONE)
	{
		return "";
	}

	xcb_get_atom_name_cookie_t cookie =
		xcb_get_atom_name(this->connection, atom);

	xcb_generic_error_t* error = nullptr;

	xcb_get_atom_name_reply_t* reply =
		xcb_get_atom_name_reply(this->connection
			, cookie
			, &error
			);

	if(reply == nullptr)
	{
		return "";
	}

	char* name = xcb_get_atom_name_name(reply);

	std::string atom_name(name);

	free(reply);

	return atom_name;
}


std::vector<xcb_atom_t> Xenium::atomValueAtom(const WindowId window_id
	, const xcb_atom_t                                   atom
	) noexcept
{
	xcb_generic_error_t generic_error = { 0 };

	return atomValueAtom(window_id, atom, generic_error);
}


std::vector<xcb_atom_t> Xenium::atomValueAtom(const WindowId window_id
	, const xcb_atom_t                                   atom
	, xcb_generic_error_t&                               generic_error
	) noexcept
{
	xcb_get_property_cookie_t property_cookie =
		xcb_get_property(this->connection
			, 0 // Don't Delete
			, window_id
			, atom
			, XCB_ATOM_ATOM
			, 0 // Data
			, 2 // Number of 32-bit values
			);

	xcb_generic_error_t* error = &generic_error;

	xcb_get_property_reply_t* property =
		xcb_get_property_reply(this->connection
			, property_cookie
			, &error
			);
	
	int count = xcb_get_property_value_length(property) / 4;

	std::vector<xcb_atom_t> retval(count);

	xcb_atom_t* value = (xcb_atom_t*)xcb_get_property_value(property);

	for(int i = 0; i < count; i++)
	{
		retval[i] = value[i];
	}

	free(property);

	return retval;
}


std::vector<int32_t> Xenium::atomValueData(const WindowId window_id
	, const xcb_atom_t                                  property
	, const xcb_atom_t                                  type
	, const size_t                                      count
	) noexcept
{
	xcb_generic_error_t generic_error;

	return atomValueData(window_id, property, type, count, generic_error);
}


std::vector<int32_t> Xenium::atomValueData(const WindowId window_id
	, const xcb_atom_t                                  property
	, const xcb_atom_t                                  type
	, const size_t                                      count
	, xcb_generic_error_t&                              generic_error
	) noexcept
{
	xcb_get_property_cookie_t property_cookie =
		xcb_get_property(this->connection
			, 0 // Don't Delete
			, window_id
			, property
			, type
			, 0 // Data
			, count // Number of 32-bit values
			);

	xcb_generic_error_t* error = &generic_error;

	xcb_get_property_reply_t* property_reply =
		xcb_get_property_reply(this->connection
			, property_cookie
			, &error
			);

	if(property_reply == nullptr)
	{
		return {};
	}
	
	int length = xcb_get_property_value_length(property_reply) / 4;

	std::vector<int32_t> vector(length);

	int32_t* value = (int32_t*)xcb_get_property_value(property_reply);

	for(int i = 0; i < length; i++)
	{
		vector[i] = value[i];
	}

	free(property_reply);

	return vector;
}


xcb_atom_t Xenium::internAtom(const std::string& atom_name
	, const bool                             create_if_needed
	, xcb_generic_error_t&                   generic_error
	) noexcept
{
	xcb_generic_error_t* error = nullptr;

	xcb_intern_atom_reply_t* atom_reply =
		xcb_intern_atom_reply(this->connection
			, xcb_intern_atom(this->connection
				// 0: Create if needed
				// 1: result XCB_ATOM_NONE if not exist
				, create_if_needed ? 0 : 1
				, atom_name.length()
				, atom_name.c_str()
				)
			, &error
			);

	xcb_atom_t atom;

	if(error != nullptr)
	{
		atom = XCB_ATOM_NONE;
		generic_error = *error;
		free(error);

		ZAKERO_XENIUM__DEBUG << "Error: " << to_string(generic_error) << '\n';
	}
	else if(atom_reply->atom == XCB_ATOM_NONE
		&& atom_name != "XCB_ATOM_NONE"
		)
	{
		ZAKERO_XENIUM__DEBUG << "Error: Failed to get \""
			<< atom_name
			<< "\" atom.\n"
			;

		atom = XCB_ATOM_NONE;
	}
	else
	{
		atom = atom_reply->atom;
	}

	free(atom_reply);

	return atom;
}


xcb_intern_atom_cookie_t Xenium::internAtomRequest(const std::string& atom_name
	, const bool                                                  create_if_needed
	) noexcept
{
	xcb_intern_atom_cookie_t cookie =
		xcb_intern_atom(this->connection
			// 0: Create if needed
			// 1: result XCB_ATOM_NONE if not exist
			, !create_if_needed
			, atom_name.length()
			, atom_name.c_str()
			);

	return cookie;
}


xcb_atom_t Xenium::internAtomReply(const xcb_intern_atom_cookie_t intern_atom_cookie
	, xcb_generic_error_t&                                    generic_error
	) noexcept
{
	xcb_generic_error_t* error = nullptr;

	xcb_intern_atom_reply_t* atom_reply =
		xcb_intern_atom_reply(this->connection
			, intern_atom_cookie
			, &error
			);

	xcb_atom_t atom;

	if(error != nullptr)
	{
		atom = XCB_ATOM_NONE;
		generic_error = *error;
		free(error);

		ZAKERO_XENIUM__DEBUG << "Error: " << to_string(generic_error) << '\n';
	}
	else if(atom_reply->atom == XCB_ATOM_NONE)
	{
		ZAKERO_XENIUM__DEBUG << "Error: Failed to get atom.\n";

		atom = XCB_ATOM_NONE;
	}
	else
	{
		atom = atom_reply->atom;
	}

	free(atom_reply);

	return atom;
}

// }}}
// {{{ XCB : XKB

void Xenium::keyDataArrayClear() noexcept
{
	const auto time_now = ZAKERO_STEADY_TIME_NOW(milliseconds);

	for(auto& key_data : key_data_array)
	{
		Key& key = key_data.key;

		if(key.time == 0)
		{
			continue;
		}

		key.state = KeyState::Released;
		key.time  = time_now;

		const KeyModifier& modifier  = key_data.modifier;
		const WindowId&    window_id = key_data.window_id;

		window_on_key_map[window_id](key, modifier);

		key.time = 0;
	}
}


void Xenium::keyDataArrayProcess() noexcept
{
	for(auto& key_data : key_data_array)
	{
		Key& key = key_data.key;

		if(key.time == 0)
		{
			continue;
		}

		if(key.state == KeyState::Pressed)
		{
			key.state = KeyState::Repeat;

			continue;
		}

		const WindowId& window_id = key_data.window_id;

		if(key.state == KeyState::Released)
		{
			window_on_key_map[window_id](key, key_data.modifier);
			key.time = 0;

			continue;
		}

		auto& repeat_time = key_data.repeat_time;

		const auto time_now = ZAKERO_STEADY_TIME_NOW(milliseconds);

		if(key.state == KeyState::Repeat
			&& repeat_time < time_now
			)
		{
			window_on_key_map[window_id](key, key_modifier);
			key.time = repeat_time;
			repeat_time += xkb_controls.repeat_interval_ms;

			continue;
		}
	}
}


std::error_code Xenium::xkbInit() noexcept
{
	xcb_xkb_use_extension_reply_t* extension_reply =
		xcb_xkb_use_extension_reply(this->connection
			, xcb_xkb_use_extension(this->connection, 1, 0)
			, nullptr
			);

	if(extension_reply == nullptr)
	{
		return ZAKERO_XENIUM__ERROR(Error_Xcb_Xkb_Not_Available);
	}

	free(extension_reply);


	return ZAKERO_XENIUM__ERROR(Error_None);
}


void Xenium::xkbControlsUpdate() noexcept
{
	xcb_xkb_get_controls_reply_t* controls_reply =
		xcb_xkb_get_controls_reply(this->connection
			, xcb_xkb_get_controls(this->connection
				, XCB_XKB_ID_USE_CORE_KBD
				)
			, nullptr
			);

	if(controls_reply == nullptr)
	{
		return;
	}

	xkb_controls.repeat_delay_ms    = controls_reply->repeatDelay;
	xkb_controls.repeat_interval_ms = controls_reply->repeatInterval;

	free(controls_reply);
}


void Xenium::xkbIndicatorStateUpdate() noexcept
{
	xcb_xkb_get_indicator_state_reply_t* reply =
		xcb_xkb_get_indicator_state_reply(this->connection
			, xcb_xkb_get_indicator_state(this->connection
				, XCB_XKB_ID_USE_CORE_KBD
				)
			, nullptr
			);

	if(reply == nullptr)
	{
		return;
	}

	const uint32_t state     = reply->state;
	const uint32_t caps_lock = KeyModifier_CapsLock;
	const uint32_t num_lock  = KeyModifier_NumLock;

	key_modifier.locked = 0
		| (bool(state & XCB_XKB_INDICATOR_STATE_CAPSLOCK) * caps_lock)
		| (bool(state & XCB_XKB_INDICATOR_STATE_NUMLOCK)  * num_lock)
		;

	free(reply);

	return;
}

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
//std::cout << "RandR CRTC Chang:" << *event << '\n';
	OutputId output_id = event->crtc;

	if(this->screen->root != event->window
		|| output_map.contains(output_id) == false
		)
	{
		return;
	}

	Output& output = output_map[output_id];

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

	output_on_change(output_id);
}


/**
 * \brief Handle the XCB RandR Output Change event
 */
void Xenium::randrEvent(const xcb_randr_output_change_t* event ///< The event
	) noexcept
{
//std::cout << "RandR Output Chg:" << *event << '\n';

	if(this->screen->root != event->window)
	{
		return;
	}

	OutputId output_id = event->crtc;

	if(event->connection == XCB_RANDR_CONNECTION_DISCONNECTED)
	{
		if(output_map.contains(output_id))
		{
			output_on_remove(output_id);
		}

		std::lock_guard<std::mutex> lock(output_mutex);

		output_map.erase(output_id);
	}
	else if((event->connection == XCB_RANDR_CONNECTION_CONNECTED)
		|| (output_map.contains(output_id) == false)
		)
	{
		std::error_code error;
		{
			std::lock_guard<std::mutex> lock(output_mutex);

			error = outputAdd(event->crtc, event->output);
		}

		if(!error)
		{
			output_on_add(output_id);
		}
	}
	else
	{
		Output& output = output_map[output_id];

		if(output.subpixel != event->subpixel_order
			|| output.transform != event->rotation
			)
		{
			output.subpixel  = event->subpixel_order;
			output.transform = event->rotation;

			output_on_change(output_id);
		}
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
std::cout << "RandR ScrnChange:" << to_string(*event) << '\n';
}

// }}}
// {{{ XCB : Utility

bool Xenium::requestCheckHasError(const xcb_void_cookie_t& void_cookie
	) noexcept
{
	xcb_generic_error_t* error = xcb_request_check(this->connection, void_cookie);

	if(error != nullptr)
	{
		std::cout << "requestCheck Error: " << to_string(*error) << '\n';

		free(error);

		return true;
	}

	return false;
}


bool Xenium::requestCheckHasError(const xcb_void_cookie_t& void_cookie
	, xcb_generic_error_t&                             generic_error
	) noexcept
{
	xcb_generic_error_t* error = xcb_request_check(this->connection, void_cookie);

	if(error != nullptr)
	{
		std::cout << "requestCheck Error: " << to_string(*error) << '\n';

		generic_error = *error;

		free(error);

		return true;
	}

	return false;
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


void Xenium::xcbWindowInit(Xenium::WindowCreateData* data
	) noexcept
{
	window_size_map[data->window_id] =
	{	.mm              = data->size_mm
	,	.mm_minimum      = {0, 0}
	,	.mm_maximum      = {0, 0}
	,	.mm_lambda       = LambdaSizeMm_DoNothing
	,	.percent         = data->size_percent
	,	.percent_minimum = {0, 0}
	,	.percent_maximum = {0, 0}
	,	.percent_lambda  = LambdaSizePercent_DoNothing
	,	.pixel           = data->size_pixel
	,	.pixel_minimum   = {0, 0}
	,	.pixel_maximum   = {0, 0}
	,	.pixel_lambda    = LambdaSizePixel_DoNothing
	,	.unit            = data->size_unit
	};

	window_mode_map[data->window_id] = 
	{	.window_mode = WindowMode::Normal
	,	.lambda      = LambdaWindowMode_DoNothing
	};

	window_output_map[data->window_id] = data->output_id;

	window_keyboard[data->window_id] =
	{	.on_enter = Lambda_DoNothing
	,	.on_leave = Lambda_DoNothing
	};

	window_on_key_map[data->window_id]   = LambdaKey_DoNothing;
	window_on_leave_map[data->window_id] = Lambda_DoNothing;
	window_on_axis_map[data->window_id]  = LambdaAxis_DoNothing;

	window_on_motion_map[data->window_id] =
	{	.lambda_mm      = LambdaPointMm_DoNothing
	,	.lambda_percent = LambdaPointPercent_DoNothing
	,	.lambda_pixel   = LambdaPointPixel_DoNothing
	};

	window_on_button_map[data->window_id] =
	{	.lambda_mm      = LambdaButtonMm_DoNothing
	,	.lambda_percent = LambdaButtonPercent_DoNothing
	,	.lambda_pixel   = LambdaButtonPixel_DoNothing
	};

	window_on_enter_map[data->window_id] =
	{	.lambda_mm      = LambdaPointMm_DoNothing
	,	.lambda_percent = LambdaPointPercent_DoNothing
	,	.lambda_pixel   = LambdaPointPixel_DoNothing
	};

	window_decorations_map[data->window_id] =
	{	.window_decorations = WindowDecorations::Server_Side
	,	.lambda             = LambdaWindowDecorations_DoNothing
	};

	window_focus_map[data->window_id] = LambdaBool_DoNothing;

	window_delete_map[data->window_id] =
	{	.close_request_lambda = Lambda_DoNothing
	,	.atom                 = data->atom
	};

	window_ready_map[data->window_id] = false;
}


void Xenium::windowReadySet(const WindowId window_id
	) noexcept
{
	window_ready_map[window_id] = true;
}


void Xenium::windowReadyWait(const WindowId window_id
	) noexcept
{
	xcb_map_window(this->connection, window_id);
	xcb_flush(this->connection);

	while(window_ready_map[window_id] == false)
	{
		usleep(42);
	}
}

// }}}
// {{{ Convenience

/**
 * \brief Convert an xcb_generic_error_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_generic_error_t& generic_error ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"response_type\": " + std::to_string(generic_error.response_type)
		+ ", \"error_code\": "    + std::to_string(generic_error.error_code)
		+ ", \"sequence\": "      + std::to_string(generic_error.sequence)
		+ ", \"resource_id\": "   + std::to_string(generic_error.resource_id)
		+ ", \"minor_code\": "    + std::to_string(generic_error.minor_code)
		+ ", \"major_code\": "    + std::to_string(generic_error.major_code)
		+ ", \"full_sequence\": " + std::to_string(generic_error.full_sequence)
		+ " }";
}


/**
 * \brief Convert an xcb_randr_screen_change_notify_event_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_randr_screen_change_notify_event_t& event ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"response_type\": "    + std::to_string(event.response_type)
		+ ", \"rotation\": "         + std::to_string(event.rotation)
		+ ", \"sequence\": "         + std::to_string(event.sequence)
		+ ", \"timestamp\": "        + std::to_string(event.timestamp)
		+ ", \"config_timestamp\": " + std::to_string(event.config_timestamp)
		+ ", \"root\": "             + std::to_string(event.root)
		+ ", \"request_window\": "   + std::to_string(event.request_window)
		+ ", \"sizeID\": "           + std::to_string(event.sizeID)
		+ ", \"subpixel_order\": "   + std::to_string(event.subpixel_order)
		+ ", \"width\": "            + std::to_string(event.width)
		+ ", \"height\": "           + std::to_string(event.height)
		+ ", \"mwidth\": "           + std::to_string(event.mwidth)
		+ ", \"mheight\": "          + std::to_string(event.mheight)
		+ " }";
}

/**
 * \brief Convert an xcb_button_press_event_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_button_press_event_t& event ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"response_type\": " + std::to_string(event.response_type)
		+ ", \"detail\": "        + std::to_string(event.detail)
		+ ", \"sequence\": "      + std::to_string(event.sequence)
		+ ", \"time\": "          + std::to_string(event.time)
		+ ", \"root\": "          + std::to_string(event.root)
		+ ", \"event\": "         + std::to_string(event.event)
		+ ", \"child\": "         + std::to_string(event.child)
		+ ", \"root_x\": "        + std::to_string(event.root_x)
		+ ", \"root_y\": "        + std::to_string(event.root_y)
		+ ", \"event_x\": "       + std::to_string(event.event_x)
		+ ", \"event_y\": "       + std::to_string(event.event_y)
		+ ", \"state\": "         + std::to_string(event.state)
		+ ", \"same_screen\": "   + std::to_string(event.same_screen)
		+ ", \"pad0\": "          + std::to_string(event.pad0)
		+ " }";
}


/**
 * \brief Convert an xcb_client_message_event_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_client_message_event_t& event ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"response_type\": " + std::to_string(event.response_type)
		+ ", \"format\": "        + std::to_string(event.format)
		+ ", \"sequence\": "      + std::to_string(event.sequence)
		+ ", \"window\": "        + std::to_string(event.window)
		+ ", \"type\": "          + std::to_string(event.type)
		+ ", \"data\": [ 0x"      + std::to_string(event.data.data8[ 0])
		+	", 0x"            + std::to_string(event.data.data8[ 1])
		+	", 0x"            + std::to_string(event.data.data8[ 2])
		+	", 0x"            + std::to_string(event.data.data8[ 3])
		+	", 0x"            + std::to_string(event.data.data8[ 4])
		+	", 0x"            + std::to_string(event.data.data8[ 5])
		+	", 0x"            + std::to_string(event.data.data8[ 6])
		+	", 0x"            + std::to_string(event.data.data8[ 7])
		+	", 0x"            + std::to_string(event.data.data8[ 8])
		+	", 0x"            + std::to_string(event.data.data8[ 9])
		+	", 0x"            + std::to_string(event.data.data8[10])
		+	", 0x"            + std::to_string(event.data.data8[11])
		+	", 0x"            + std::to_string(event.data.data8[12])
		+	", 0x"            + std::to_string(event.data.data8[13])
		+	", 0x"            + std::to_string(event.data.data8[14])
		+	", 0x"            + std::to_string(event.data.data8[15])
		+	", 0x"            + std::to_string(event.data.data8[16])
		+	", 0x"            + std::to_string(event.data.data8[17])
		+	", 0x"            + std::to_string(event.data.data8[18])
		+	", 0x"            + std::to_string(event.data.data8[19])
		+	" ]"
		+ " }";
}


/**
 * \brief Convert an xcb_configure_notify_event_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_configure_notify_event_t& event ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"response_type\": "     + std::to_string(event.response_type)
		+ ", \"pad0\": "              + std::to_string(event.pad0)
		+ ", \"sequence\": "          + std::to_string(event.sequence)
		+ ", \"event\": "             + std::to_string(event.event)
		+ ", \"window\": "            + std::to_string(event.window)
		+ ", \"above_sibling\": "     + std::to_string(event.above_sibling)
		+ ", \"x\": "                 + std::to_string(event.x)
		+ ", \"y\": "                 + std::to_string(event.y)
		+ ", \"width\": "             + std::to_string(event.width)
		+ ", \"height\": "            + std::to_string(event.height)
		+ ", \"border_width\": "      + std::to_string(event.border_width)
		+ ", \"override_redirect\": " + std::to_string(event.override_redirect)
		+ ", \"pad1\": "              + std::to_string(event.pad1)
		+ " }";
}


/**
 * \brief Convert an xcb_enter_notify_event_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_enter_notify_event_t& event ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"response_type\": "     + std::to_string(event.response_type)
		+ ", \"detail\": "            + std::to_string(event.detail)
		+ ", \"sequence\": "          + std::to_string(event.sequence)
		+ ", \"time\": "              + std::to_string(event.time)
		+ ", \"root\": "              + std::to_string(event.root)
		+ ", \"event\": "             + std::to_string(event.event)
		+ ", \"root_x\": "            + std::to_string(event.root_x)
		+ ", \"root_y\": "            + std::to_string(event.root_y)
		+ ", \"event_x\": "           + std::to_string(event.event_x)
		+ ", \"event_y\": "           + std::to_string(event.event_y)
		+ ", \"state\": "             + std::to_string(event.state)
		+ ", \"mode\": "              + std::to_string(event.mode)
		+ ", \"same_screen_focus\": " + std::to_string(event.same_screen_focus)
		+ " }";
}


/**
 * \brief Convert an xcb_expose_event_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_expose_event_t& event ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"response_type\": " + std::to_string(event.response_type)
		+ ", \"pad0\": "          + std::to_string(event.pad0)
		+ ", \"sequence\": "      + std::to_string(event.sequence)
		+ ", \"window\": "        + std::to_string(event.window)
		+ ", \"x\": "             + std::to_string(event.x)
		+ ", \"y\": "             + std::to_string(event.y)
		+ ", \"width\": "         + std::to_string(event.width)
		+ ", \"height\": "        + std::to_string(event.height)
		+ ", \"count\": "         + std::to_string(event.count)
		+ ", \"pad1\": [ 0x"      + std::to_string(event.pad1[0])
		+	", 0x"            + std::to_string(event.pad1[1])
		+	" ]"
		+ " }";
}


/**
 * \brief Convert an xcb_focus_in_event_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_focus_in_event_t& event ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"response_type\": " + std::to_string(event.response_type)
		+ ", \"detail\": "        + std::to_string(event.detail)
		+ ", \"sequence\": "      + std::to_string(event.sequence)
		+ ", \"event\": "         + std::to_string(event.event)
		+ ", \"mode\": "          + std::to_string(event.mode)
		+ ", \"pad0\": [ 0x"      + std::to_string(event.pad0[0])
		+	", 0x"            + std::to_string(event.pad0[1])
		+	", 0x"            + std::to_string(event.pad0[2])
		+	" ]"
		+ " }";
}


/**
 * \brief Convert an xcb_generic_event_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_generic_event_t& event ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"response_type\": " + std::to_string(event.response_type)
		+ ", \"pad0\": "          + std::to_string(event.pad0)
		+ ", \"sequence\": "      + std::to_string(event.sequence)
		+ ", \"pad\": [ 0x"       + std::to_string(event.pad[0])
		+	", 0x"            + std::to_string(event.pad[1])
		+	", 0x"            + std::to_string(event.pad[2])
		+	", 0x"            + std::to_string(event.pad[3])
		+	", 0x"            + std::to_string(event.pad[4])
		+	", 0x"            + std::to_string(event.pad[5])
		+	", 0x"            + std::to_string(event.pad[6])
		+	" ]"
		+ ", \"full_sequence\": " + std::to_string(event.full_sequence)
		+ " }";
}


/**
 * \brief Convert an xcb_gravity_notify_event_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_gravity_notify_event_t& event ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"response_type\": " + std::to_string(event.response_type)
		+ ", \"pad0\": "          + std::to_string(event.pad0)
		+ ", \"sequence\": "      + std::to_string(event.sequence)
		+ ", \"event\": "         + std::to_string(event.event)
		+ ", \"window\": "        + std::to_string(event.window)
		+ ", \"x\": "             + std::to_string(event.x)
		+ ", \"y\": "             + std::to_string(event.y)
		+ " }";
}


/**
 * \brief Convert an xcb_key_press_event_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_key_press_event_t& event ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"response_type\": " + std::to_string(event.response_type)
		+ ", \"detail\": "        + std::to_string(event.detail)
		+ ", \"sequence\": "      + std::to_string(event.sequence)
		+ ", \"time\": "          + std::to_string(event.time)
		+ ", \"root\": "          + std::to_string(event.root)
		+ ", \"event\": "         + std::to_string(event.event)
		+ ", \"child\": "         + std::to_string(event.child)
		+ ", \"root_x\": "        + std::to_string(event.root_x)
		+ ", \"root_y\": "        + std::to_string(event.root_y)
		+ ", \"event_x\": "       + std::to_string(event.event_x)
		+ ", \"event_y\": "       + std::to_string(event.event_y)
		+ ", \"state\": "         + std::to_string(event.state)
		+ ", \"same_screen\": "   + std::to_string(event.same_screen)
		+ ", \"pad0\": "          + std::to_string(event.pad0)
		+ " }";
}


/**
 * \brief Convert an xcb_map_notify_event_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_map_notify_event_t& event ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"response_type\": "     + std::to_string(event.response_type)
		+ ", \"pad0\": "              + std::to_string(event.pad0)
		+ ", \"sequence\": "          + std::to_string(event.sequence)
		+ ", \"event\": "             + std::to_string(event.event)
		+ ", \"window\": "            + std::to_string(event.window)
		+ ", \"override_redirect\": " + std::to_string(event.override_redirect)
		+ ", \"pad1\": [ 0x"          + std::to_string(event.pad1[0])
		+	", 0x"                + std::to_string(event.pad1[1])
		+	", 0x"                + std::to_string(event.pad1[2])
		+	" ]"
		+ " }";
}


/**
 * \brief Convert an xcb_motion_notify_event_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_motion_notify_event_t& event ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"response_type\": " + std::to_string(event.response_type)
		+ ", \"detail\": "        + std::to_string(event.detail)
		+ ", \"sequence\": "      + std::to_string(event.sequence)
		+ ", \"time\": "          + std::to_string(event.time)
		+ ", \"root\": "          + std::to_string(event.root)
		+ ", \"event\": "         + std::to_string(event.event)
		+ ", \"child\": "         + std::to_string(event.child)
		+ ", \"root_x\": "        + std::to_string(event.root_x)
		+ ", \"root_y\": "        + std::to_string(event.root_y)
		+ ", \"event_x\": "       + std::to_string(event.event_x)
		+ ", \"event_y\": "       + std::to_string(event.event_y)
		+ ", \"state\": "         + std::to_string(event.state)
		+ ", \"same_screen\": "   + std::to_string(event.same_screen)
		+ ", \"pad0\": "          + std::to_string(event.pad0)
		+ " }";
}


/**
 * \brief Convert an xcb_property_notify_event_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_property_notify_event_t& event ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"response_type\": " + std::to_string(event.response_type)
		+ ", \"pad0\": "          + std::to_string(event.pad0)
		+ ", \"sequence\": "      + std::to_string(event.sequence)
		+ ", \"window\": "        + std::to_string(event.window)
		+ ", \"atom\": "          + std::to_string(event.atom)
		+ ", \"time\": "          + std::to_string(event.time)
		+ ", \"state\": "         + std::to_string(event.state)
		+ ", \"pad1\": [ 0x"      + std::to_string(event.pad1[0])
		+	", 0x"            + std::to_string(event.pad1[1])
		+	", 0x"            + std::to_string(event.pad1[2])
		+	" ]"
		+ " }";
}


/**
 * \brief Convert an xcb_reparent_notify_event_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_reparent_notify_event_t& event ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"response_type\": "     + std::to_string(event.response_type)
		+ ", \"pad0\": "              + std::to_string(event.pad0)
		+ ", \"sequence\": "          + std::to_string(event.sequence)
		+ ", \"event\": "             + std::to_string(event.event)
		+ ", \"window\": "            + std::to_string(event.window)
		+ ", \"parent\": "            + std::to_string(event.parent)
		+ ", \"x\": "                 + std::to_string(event.x)
		+ ", \"y\": "                 + std::to_string(event.y)
		+ ", \"override_redirect\": " + std::to_string(event.override_redirect)
		+ ", \"pad1\": [ 0x"          + std::to_string(event.pad1[0])
		+	", 0x"                + std::to_string(event.pad1[1])
		+	", 0x"                + std::to_string(event.pad1[2])
		+	" ]"
		+ " }";
}


/**
 * \brief Convert an xcb_unmap_notify_event_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_unmap_notify_event_t& event ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"response_type\": "  + std::to_string(event.response_type)
		+ ", \"pad0\": "           + std::to_string(event.pad0)
		+ ", \"sequence\": "       + std::to_string(event.sequence)
		+ ", \"event\": "          + std::to_string(event.event)
		+ ", \"window\": "         + std::to_string(event.window)
		+ ", \"from_configure\": " + std::to_string(event.from_configure)
		+ ", \"pad1\": [ 0x"       + std::to_string(event.pad1[0])
		+	", 0x"             + std::to_string(event.pad1[1])
		+	", 0x"             + std::to_string(event.pad1[2])
		+	" ]"
		+ " }";
}


/**
 * \brief Convert an xcb_format_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_format_t& format ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"depth\": "          + std::to_string(format.depth)
		+ ", \"bits_per_pixel\": " + std::to_string(format.bits_per_pixel)
		+ ", \"scanline_pad\": "   + std::to_string(format.scanline_pad)
		+ ", \"pad0\": [ 0x"       + std::to_string(format.pad0[0])
		+	", 0x"             + std::to_string(format.pad0[1])
		+	", 0x"             + std::to_string(format.pad0[2])
		+	", 0x"             + std::to_string(format.pad0[3])
		+	", 0x"             + std::to_string(format.pad0[4])
		+	" ]"
		+ " }";
}


/**
 * \brief Convert an xcb_screen_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_screen_t& screen ///< The value to convert.
	) noexcept
{
	return std::string()
		+ "{ \"root\": "                  + std::to_string(screen.root)
		+ ", \"default_colormap\": "      + std::to_string(screen.default_colormap)
		+ ", \"white_pixel\": "           + std::to_string(screen.white_pixel)
		+ ", \"black_pixel\": "           + std::to_string(screen.black_pixel)
		+ ", \"current_input_masks\": "   + std::to_string(screen.current_input_masks)
		+ ", \"width_in_pixels\": "       + std::to_string(screen.width_in_pixels)
		+ ", \"height_in_pixels\": "      + std::to_string(screen.height_in_pixels)
		+ ", \"width_in_millimeters\": "  + std::to_string(screen.width_in_millimeters)
		+ ", \"height_in_millimeters\": " + std::to_string(screen.height_in_millimeters)
		+ ", \"min_installed_maps\": "    + std::to_string(screen.min_installed_maps)
		+ ", \"max_installed_maps\": "    + std::to_string(screen.max_installed_maps)
		+ ", \"root_visual\": "           + std::to_string(screen.root_visual)
		+ ", \"backing_stores\": "        + std::to_string(screen.backing_stores)
		+ ", \"save_unders\": "           + std::to_string(screen.save_unders)
		+ ", \"root_depth\": "            + std::to_string(screen.root_depth)
		+ ", \"allowed_depths_len\": "    + std::to_string(screen.allowed_depths_len)
		+ " }";
}


/**
 * \brief Convert a vector into a string.
 *
 * \return The string.
 */
std::string to_string(const std::vector<xcb_atom_t>& vector ///< The value to convert
	) noexcept
{
	std::string string = "[ ";
	std::string delim = "";

	for(const auto& atom : vector)
	{
		string += delim + ' ' + std::to_string(atom);

		delim = ",";
	}

	string += " ]";

	return string;
}


/**
 * \brief Convert a vector into a string.
 *
 * \return The string.
 */
std::string to_string(const std::vector<int32_t>& vector ///< The value to convert
	) noexcept
{
	std::string string = "[ ";
	std::string delim = "";

	for(const int32_t& value : vector)
	{
		string += delim + ' ' + std::to_string(value);

		delim = ",";
	}

	string += " ]";

	return string;
}


/**
 * \brief Convert an xcb_setup_t into a string.
 *
 * \return The string.
 */
std::string to_string(const xcb_setup_t& setup ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"status\": "                      + std::to_string(setup.status)
		+ ", \"pad0\": "                        + std::to_string(setup.pad0)
		+ ", \"protocol_major_version\": "      + std::to_string(setup.protocol_major_version)
		+ ", \"protocol_minor_version\": "      + std::to_string(setup.protocol_minor_version)
		+ ", \"length\": "                      + std::to_string(setup.length)
		+ ", \"release_number\": "              + std::to_string(setup.release_number)
		+ ", \"resource_id_base\": "            + std::to_string(setup.resource_id_base)
		+ ", \"resource_id_mask\": "            + std::to_string(setup.resource_id_mask)
		+ ", \"motion_buffer_size\": "          + std::to_string(setup.motion_buffer_size)
		+ ", \"vendor_len\": "                  + std::to_string(setup.vendor_len)
		+ ", \"maximum_request_length\": "      + std::to_string(setup.maximum_request_length)
		+ ", \"roots_len\": "                   + std::to_string(setup.roots_len)
		+ ", \"pixmap_formats_len\": "          + std::to_string(setup.pixmap_formats_len)
		+ ", \"image_byte_order\": "            + std::to_string(setup.image_byte_order)
		+ ", \"bitmap_format_bit_order\": "     + std::to_string(setup.bitmap_format_bit_order)
		+ ", \"bitmap_format_scanline_unit\": " + std::to_string(setup.bitmap_format_scanline_unit)
		+ ", \"bitmap_format_scanline_pad\": "  + std::to_string(setup.bitmap_format_scanline_pad)
		+ ", \"min_keycode\": "                 + std::to_string(setup.min_keycode)
		+ ", \"max_keycode\": "                 + std::to_string(setup.max_keycode)
		+ ", \"pad1\": [ 0x"                    + std::to_string(setup.pad1[0])
		+	", 0x"                          + std::to_string(setup.pad1[1])
		+	", 0x"                          + std::to_string(setup.pad1[2])
		+	", 0x"                          + std::to_string(setup.pad1[3])
		+	" ]"
		+ " }";
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p key data will be converted into a JSON formatted std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::Key& key ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"time\": "    + std::to_string(key.time)
		+ ", \"code\": "    + std::to_string(key.code)
		+ ", \"state\": \"" + zakero::to_string(key.state) + "\""
		+ " }"
		;
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p key_modifier data will be converted into a JSON formatted 
 * std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::KeyModifier& key_modifier ///< The value to convert
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

		if(m & Xenium::KeyModifier_NumLock)
		{
			s += delim + "\"NumLock\"";
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
std::string to_string(const Xenium::KeyState key_state ///< The value to convert
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
std::string to_string(const Xenium::Output& output ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"name\": \""                     + output.name + "\""
		+ ", \"x\": "                        + std::to_string(output.x)
		+ ", \"y\": "                        + std::to_string(output.y)
		+ ", \"width\": "                    + std::to_string(output.width)
		+ ", \"height\": "                   + std::to_string(output.height)
		+ ", \"physical_width_mm\": "        + std::to_string(output.physical_width_mm)
		+ ", \"physical_height_mm:\" "       + std::to_string(output.physical_height_mm)
		+ ", \"subpixel\": "                 + std::to_string(output.subpixel)
		+ ", \"transform\": "                + zakero::Xenium::outputTransformName(output.transform)
		+ ", \"pixels_per_mm_horizontal\": " + std::to_string(output.pixels_per_mm_horizontal)
		+ ", \"pixels_per_mm_vertical\": "   + std::to_string(output.pixels_per_mm_vertical)
		//+ ", \"make\": \""                   + output.make + "\""
		//+ ", \"model\": \""                  + output.model + "\""
		//+ ", \"refresh_mHz\": "              + std::to_string(output.refresh_mHz)
		//+ ", \"scale_factor\": "             + std::to_string(output.scale_factor)
		+ " }";
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p source will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::PointMm point ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"time\": " + std::to_string(point.time)
		+ ", \"x\": "    + std::to_string(point.x)
		+ ", \"y\": "    + std::to_string(point.y)
		+ " }"
		;
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p source will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::PointPercent point ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"time\": " + std::to_string(point.time)
		+ ", \"x\": "    + std::to_string(point.x)
		+ ", \"y\": "    + std::to_string(point.y)
		+ " }"
		;
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p source will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::PointPixel point ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"time\": " + std::to_string(point.time)
		+ ", \"x\": "    + std::to_string(point.x)
		+ ", \"y\": "    + std::to_string(point.y)
		+ " }"
		;
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p source will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::PointerAxis& pointer_axis ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"time\": "     + std::to_string(pointer_axis.time)
		+ ", \"steps\": "    + std::to_string(pointer_axis.steps)
		+ ", \"distance\": " + std::to_string(pointer_axis.distance)
		+ ", \"source\": \"" + zakero::to_string(pointer_axis.source) + "\""
		+ ", \"type\": \""   + zakero::to_string(pointer_axis.type)   + "\""
		+ " }"
		;
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p source will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::PointerAxisSource source ///< The value to convert
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
std::string to_string(const Xenium::PointerAxisType type ///< The value to convert
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
 * The \p button will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::PointerButton& button ///< The value to convert
	) noexcept
{
	std::string str = std::string()
		+ "{ \"code\": " + std::to_string(button.code)
		+ ", \"state\": " + zakero::to_string(button.state)
		+ " }"
		;
	
	return str;
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p button_state will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::PointerButtonState& button_state ///< The value to convert
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
 * The \p size will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::SizeMm& size ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"width\": "  + std::to_string(size.width)
		+ ", \"height\": " + std::to_string(size.height)
		+ " }";
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p size will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::SizePercent& size ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"width\": "  + std::to_string(size.width)
		+ ", \"height\": " + std::to_string(size.height)
		+ " }";
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p size will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::SizePixel& size ///< The value to convert
	) noexcept
{
	return std::string()
		+ "{ \"width\": "  + std::to_string(size.width)
		+ ", \"height\": " + std::to_string(size.height)
		+ " }";
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p window_decorations will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Xenium::WindowDecorations window_decorations ///< The value to convert
	) noexcept
{
	switch(window_decorations)
	{
		case Xenium::WindowDecorations::Client_Side: return "Client Side";
		case Xenium::WindowDecorations::Server_Side: return "Server Side";
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
