/******************************************************************************
 * Copyright 2020-2022 Andrew Moore
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef zakero_Yetani_h
#define zakero_Yetani_h

/**
 * \file
 *
 * \brief Zakero Yetani
 *
|                      |                         |                         |                         |                        |                        |                            |
|----------------------|-------------------------|-------------------------|-------------------------|------------------------|------------------------|----------------------------|
| \api{zakero::Yetani} | \refdeps{zakero_yetani} | \reftldr{zakero_yetani} | \refwhat{zakero_yetani} | \refwhy{zakero_yetani} | \refhow{zakero_yetani} | \refversion{zakero_yetani} |
 *
 * Making the Wayland windowing system easy to use.
 *
 *
 * \pardeps{zakero_yetani}
 * - Zakero_Base.h
 * - Zakero_MemoryPool.h
 * \endpardeps
 *
 *
 * \partldr{zakero_yetani}
 * This library makes it very easy to connect to Wayland and display windows.
 * The developer must write pixel data directly into the window.  No UI or 
 * graphics functionality is provided.
 *
 * To use:
 * 1. Add the implementation to a source code file:
 *    \code
 *    #define ZAKERO_MEMORYPOOL_IMPLEMENTATION
 *    #define ZAKERO_YETANI_IMPLEMENTATION
 *    #include "Zakero_Yetani.h"
 *    \endcode
 * 2. Add the library to where it is used:
 *    \code
 *    #include "Zakero_Yetani.h"
 *    \endcode
 * 3. Link to the Wayland client library and pthread when building:
 *    \code
 *    -lpthread -lwayland-client
 *    \endcode
 * \endpartldr
 *
 *
 * \parwhat{zakero_yetani}
 * Wayland is a Windowing System and was intended to strip all the cruft that 
 * has built up in X11.  The part of Wayland that does the rendering is called 
 * the "Wayland Compositor".  The "Wayland Client" is the application that the 
 * user interacts with.  Communication between the two is done using the 
 * "Wayland Protocol".  _This is a simplification.  If you want more details, 
 * please do a search on the internet._
 *
 * Implementing the "Wayland Protocol" for a "Wayland Client" is easy to do.  
 * However, this code is verbose and tedious.  Plus, once you start working on 
 * your second and third "Wayland Client", much of this code is duplicated.  
 * Consolidating this redundant code is one of the reasons behind Yetani.
 *
 * The zakero::Yetani class interacts with the basic Wayland functionality, 
 * such as determining what pixel formats are supported and which Output 
 * devices are currently available.  Custom graphics for cursors is also 
 * supported.
 *
 * The zakero::Yetani::Window class makes creating a window effortless.
 * Rendering graphics has been simplified to the point where you just provide 
 * the graphics data to the window and it will take care of the rest.
 * zakero::Yetani::Window also supports sizes in percentages of the output 
 * device size as well as in millimeters, in additional to the traditional 
 * pixel sizes.
 * \endparwhat
 *
 *
 * \parwhy{zakero_yetani}
 * Wayland was intended to strip all the cruft that has built up in X11 and 
 * focus on three things.  The part of Wayland that does the rendering is the 
 * "Wayland Compositor".  The part that is the application is the "Wayland 
 * Client".  And they talk using the "Wayland Protocol". _Again, this is a 
 * simplification.  If you want more details, please do a search on the 
 * internet._
 *
 * Implementing the "Wayland Protocol" in "Wayland Client" is easy to do.  
 * However, this code is verbose and tedious.  Once you start working on your 
 * second and third "Wayland Client", much of this code is duplicated.  
 * Consolidating the redundant code is the purpose of the Yetani.
 * \endparwhy
 *
 *
 * \parhow{zakero_yetani}
 * __Step 0__
 *
 * Your compiler must support at least the C++20 standard.  The location of the 
 * `Zakero_*.h` header files _must_ be in your compiler's include path.
 *
 * __Step 1__
 *
 * The first step is to select which C++ source code file will contain the 
 * _Zakero Yetani_ implementation.  Once the location has been determined, add 
 * the following to that file:
 *
 * ~~~
 * #define ZAKERO_MEMORYPOOL_IMPLEMENTATION
 * #define ZAKERO_YETANI_IMPLEMENTATION
 * #include "Zakero_Yetani.h"
 * ~~~
 *
 * The macro \ref ZAKERO_YETANI_IMPLEMENTATION tells the header file to include 
 * the implementation of the Yetani.
 *
 * Notice that the _Zakero MemoryPool_ implementation, 
 * ZAKERO_MEMORYPOLL_IMPLEMENTATION, is included before the `Zakero_Yetain.h` 
 * header.  _Zakero Yetani_ depends on the memory pool for shared memory with 
 * the Wayland compositor.  If the _Zakero MemoryPool_ implementation was added 
 * else where, this define can be omitted.
 *
 * In all other files that will use Yetani, they only need to include the 
 * header.
 *
 * ~~~
 * #include "Zakero_Yetani.h"
 * ~~~
 *
 * __Step 2__
 *
 * Below is a simple program that will conncet to the Wayland Compositor and 
 * display a window.
 *
 * ~~~
 * #define ZAKERO_MEMORYPOOL_IMPLEMENTATION
 * #define ZAKERO_YETANI_IMPLEMENTATION
 * #include "Zakero_Yetani.h"
 *
 * int main()
 * {
 * 	std::error_code error;
 * 
 * 	zakero::Yetani* yetani = zakero::Yetani::connect(error);
 * 	if(error)
 * 	{
 * 		std::cout << "Error: " << zakero::to_string(error) << '\n';
 * 		return 1;
 * 	}
 * 
 * 	zakero::Yetani::SizePixel window_size = { 640, 480 };
 * 	auto* window = yetani->windowCreate(window_size, error);
 * 	if(error)
 * 	{
 * 		std::cout << "Error: " << zakero::to_string(error) << '\n';
 * 		return 1;
 * 	}
 * 
 * 	window->titleSet("Yetani");
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
 * 	delete yetani;
 * 
 * 	return 0;
 * }
 * ~~~
 *
 * The following commands will build and run the example program.
 * ~~~
 * > g++ -std=c++20 -lpthread -lwayland-client -o example example.cpp
 * > ./example
 * ~~~
 * \endparhow
 *
 *
 * \parversion{zakero_yetani}
 * __v0.6.1__
 * - Moved to_string(std::error_code) to Zakero_Base.h
 * - Moved equalish() to Zakero_Base.h
 * - Documentation improvements
 *
 * __v0.6.0__
 * - Fully Documented
 * - No Known Bugs
 *
 * __v0.5.0__
 * - Cursor Support
 * - Support many Surfaces
 *
 * __v0.4.0__
 * - Keyboard Input Support (uses Linux Input Event codes)
 *
 * __v0.3.0__
 * - Mouse Input Support
 *
 * __v0.2.0__
 * - Memory Manager for handling dynamic allocations from a single memory pool.
 *
 * __v0.1.0__
 * - Wayland Output device hot-plug support
 * - Support for many Yetani instances
 * - Support for many Windows in each Yetani object
 * - Server Side Decorations support when available
 * - Window resizing
 * - Window image/surface rendering
 * - Fully multi-threaded, all Wayland execution happens in a separate thread.
 * - Flexible sizing options: Millimeters, Percent, and Pixel
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

// Wayland
#include <wayland/wayland-client.h>

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
 *    The error name will be accessible as zakero::Yetani::_ErrorName_
 *    <br><br>
 * -# __ErrorValue__<br>
 *    The integer value of the error and will be available as `int` values in 
 *    the zakero::Yetani class.
 *    <br><br>
 * -# __ErrorMessage__<br>
 *    The text that will be used by `std::error_code.message()`
 */
#define ZAKERO_YETANI__ERROR_DATA \
	X(Error_None                                   ,  0 , "No Error"                                                         ) \
	X(Error_Compositor_Was_Not_Found               ,  1 , "Could not find the Compositor object in the Global Repository."   ) \
	X(Error_Connection_Failed                      ,  2 , "Failed to connect to the Wayland Server."                         ) \
	X(Error_Cursor_Already_Exists                  ,  3 , "A cursor with that name already exists."                          ) \
	X(Error_Cursor_Does_Not_Exist                  ,  4 , "No cursors exists with that name."                                ) \
	X(Error_Cursor_Frame_Time_Too_Large            ,  5 , "The cursor time per frame is too large, must be <= Size_Max."     ) \
	X(Error_Cursor_Frame_Time_Too_Small            ,  6 , "The cursor time per frame is too small, must be greater than 0."  ) \
	X(Error_Cursor_Image_Data_Is_Empty             ,  7 , "The cursor image data can not be empty."                          ) \
	X(Error_Cursor_Name_Is_Invalid                 ,  8 , "The cursor name is invalid."                                      ) \
	X(Error_Cursor_Not_Attached                    ,  9 , "The specified cursor is not attached/in-use."                     ) \
	X(Error_Cursor_Size_Too_Small                  , 10 , "The cursor size, both width and height must be greater than 0."   ) \
	X(Error_Invalid_Display_Name                   , 11 , "An invalid dispaly name was given to the Wayland Server."         ) \
	X(Error_Minimum_Size_Greater_Than_Maximum_Size , 12 , "The minimum window size is larger than the maximum window size."  ) \
	X(Error_No_Output_Available                    , 13 , "No output devices are available."                                 ) \
	X(Error_Registry_Not_Available                 , 14 , "Unable to get the registery."                                     ) \
	X(Error_Server_Side_Decorations_Not_Available  , 15 , "The Wayland Compositor does not support Server Side Decorations." ) \
	X(Error_Shm_Was_Not_Found                      , 16 , "Could not find the Shm object in the Global Repository."          ) \
	X(Error_Wayland_Not_Available                  , 17 , "Could not find the Wayland Server."                               ) \
	X(Error_Window_Initialization_Failed           , 18 , "The window was not able to be initialized."                       ) \
	X(Error_Window_Size_Too_Small                  , 19 , "The window size was too small."                                   ) \
	X(Error_Xdg_WM_Base_Was_Not_Found              , 20 , "Could not find the XDG WM Base object the Global Repository."     ) \

	/* --- To Be Deleted --- */
	/*
	X(Error_Window_Already_Exists                  , 100 , "A window with that name already exists."                          ) \
	X(Error_Window_Does_Not_Exist                  , 101 , "No windows exists with that name."                                ) \
	X(Error_Window_Name_Can_Not_Be_Empty           , 102 , "Windows can not have empty names."                                ) \
	X(Error_EventLoop_Is_Already_Running           , 103 , "Can't start the event loop since it is already running."          ) \
	X(Error_Connection_Not_Initialized             , 104 , "Not connected to the Wayland Server."                             ) \
	X(Error_Connection_Already_Established         , 105 , "Must disconnect before establishing a new connection."            ) \
	X(Error_Seat_Was_Not_Found                     , 106 , "Could not find the Seat object in the Global Repository."         ) \
	X(Error_Present_Current_Image_First            , 107 , "The next image has already been retrieved and must be presented." ) \
	X(Error_No_Image_Is_Available                  , 108 , "No image is available to be retrieved."                           ) \
	X(Error_Window_Is_Locked                       , 109 , "The window access is restricted due to being locked."             ) \
	X(Error_Cursor_Image_Data_Size_Is_Invalid      , 110 , "The cursor image data size does not match the configuration."     ) \
	*/

// }}}

/******************************************************************************
 * Generated Code
 *
 * The code is this section was created by "wayland-protocol/protocol.sh".  The 
 * generated code is required since the XDG/Wayland headers are not officially 
 * distributed.  Plus, there are many versions of the generated code with 
 * slight variations.  By including the code directly, the generated code is 
 * from a known baseline.
 *
 * TL;DR: The code you are looking for is in the Yetani class.
 */

/**
 * \cond Doxygen_Ignore
 */

/*
 * The "wayland-protocol/protocol.sh" script uses this fold as markers to know 
 * where to add and remove code.
 */
// {{{ Generated Code
// {{{ xdg-decoration-unstable-v1
struct xdg_toplevel;
struct zxdg_decoration_manager_v1;
struct zxdg_toplevel_decoration_v1;
extern const struct wl_interface zxdg_decoration_manager_v1_interface;
extern const struct wl_interface zxdg_toplevel_decoration_v1_interface;
static inline void
zxdg_decoration_manager_v1_set_user_data(struct zxdg_decoration_manager_v1 *zxdg_decoration_manager_v1, void *user_data)
{
 wl_proxy_set_user_data((struct wl_proxy *) zxdg_decoration_manager_v1, user_data);
}
static inline void *
zxdg_decoration_manager_v1_get_user_data(struct zxdg_decoration_manager_v1 *zxdg_decoration_manager_v1)
{
 return wl_proxy_get_user_data((struct wl_proxy *) zxdg_decoration_manager_v1);
}
static inline uint32_t
zxdg_decoration_manager_v1_get_version(struct zxdg_decoration_manager_v1 *zxdg_decoration_manager_v1)
{
 return wl_proxy_get_version((struct wl_proxy *) zxdg_decoration_manager_v1);
}
static inline void
zxdg_decoration_manager_v1_destroy(struct zxdg_decoration_manager_v1 *zxdg_decoration_manager_v1)
{
 wl_proxy_marshal((struct wl_proxy *) zxdg_decoration_manager_v1,
    0);
 wl_proxy_destroy((struct wl_proxy *) zxdg_decoration_manager_v1);
}
static inline struct zxdg_toplevel_decoration_v1 *
zxdg_decoration_manager_v1_get_toplevel_decoration(struct zxdg_decoration_manager_v1 *zxdg_decoration_manager_v1, struct xdg_toplevel *toplevel)
{
 struct wl_proxy *id;
 id = wl_proxy_marshal_constructor((struct wl_proxy *) zxdg_decoration_manager_v1,
    1, &zxdg_toplevel_decoration_v1_interface, NULL, toplevel);
 return (struct zxdg_toplevel_decoration_v1 *) id;
}
enum zxdg_toplevel_decoration_v1_error {
 ZXDG_TOPLEVEL_DECORATION_V1_ERROR_UNCONFIGURED_BUFFER = 0,
 ZXDG_TOPLEVEL_DECORATION_V1_ERROR_ALREADY_CONSTRUCTED = 1,
 ZXDG_TOPLEVEL_DECORATION_V1_ERROR_ORPHANED = 2,
};
enum zxdg_toplevel_decoration_v1_mode {
 ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE = 1,
 ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE = 2,
};
struct zxdg_toplevel_decoration_v1_listener {
 void (*configure)(void *data,
     struct zxdg_toplevel_decoration_v1 *zxdg_toplevel_decoration_v1,
     uint32_t mode);
};
static inline int
zxdg_toplevel_decoration_v1_add_listener(struct zxdg_toplevel_decoration_v1 *zxdg_toplevel_decoration_v1,
      const struct zxdg_toplevel_decoration_v1_listener *listener, void *data)
{
 return wl_proxy_add_listener((struct wl_proxy *) zxdg_toplevel_decoration_v1,
         (void (**)(void)) listener, data);
}
static inline void
zxdg_toplevel_decoration_v1_set_user_data(struct zxdg_toplevel_decoration_v1 *zxdg_toplevel_decoration_v1, void *user_data)
{
 wl_proxy_set_user_data((struct wl_proxy *) zxdg_toplevel_decoration_v1, user_data);
}
static inline void *
zxdg_toplevel_decoration_v1_get_user_data(struct zxdg_toplevel_decoration_v1 *zxdg_toplevel_decoration_v1)
{
 return wl_proxy_get_user_data((struct wl_proxy *) zxdg_toplevel_decoration_v1);
}
static inline uint32_t
zxdg_toplevel_decoration_v1_get_version(struct zxdg_toplevel_decoration_v1 *zxdg_toplevel_decoration_v1)
{
 return wl_proxy_get_version((struct wl_proxy *) zxdg_toplevel_decoration_v1);
}
static inline void
zxdg_toplevel_decoration_v1_destroy(struct zxdg_toplevel_decoration_v1 *zxdg_toplevel_decoration_v1)
{
 wl_proxy_marshal((struct wl_proxy *) zxdg_toplevel_decoration_v1,
    0);
 wl_proxy_destroy((struct wl_proxy *) zxdg_toplevel_decoration_v1);
}
static inline void
zxdg_toplevel_decoration_v1_set_mode(struct zxdg_toplevel_decoration_v1 *zxdg_toplevel_decoration_v1, uint32_t mode)
{
 wl_proxy_marshal((struct wl_proxy *) zxdg_toplevel_decoration_v1,
    1, mode);
}
static inline void
zxdg_toplevel_decoration_v1_unset_mode(struct zxdg_toplevel_decoration_v1 *zxdg_toplevel_decoration_v1)
{
 wl_proxy_marshal((struct wl_proxy *) zxdg_toplevel_decoration_v1,
    2);
}
#ifdef ZAKERO_YETANI_IMPLEMENTATION
extern const struct wl_interface xdg_toplevel_interface;
extern const struct wl_interface zxdg_toplevel_decoration_v1_interface;
static const struct wl_interface *xdg_decoration_unstable_v1_types[] = {
 NULL,
 &zxdg_toplevel_decoration_v1_interface,
 &xdg_toplevel_interface,
};
static const struct wl_message zxdg_decoration_manager_v1_requests[] = {
 { "destroy", "", xdg_decoration_unstable_v1_types + 0 },
 { "get_toplevel_decoration", "no", xdg_decoration_unstable_v1_types + 1 },
};
const struct wl_interface zxdg_decoration_manager_v1_interface = {
 "zxdg_decoration_manager_v1", 1,
 2, zxdg_decoration_manager_v1_requests,
 0, NULL,
};
static const struct wl_message zxdg_toplevel_decoration_v1_requests[] = {
 { "destroy", "", xdg_decoration_unstable_v1_types + 0 },
 { "set_mode", "u", xdg_decoration_unstable_v1_types + 0 },
 { "unset_mode", "", xdg_decoration_unstable_v1_types + 0 },
};
static const struct wl_message zxdg_toplevel_decoration_v1_events[] = {
 { "configure", "u", xdg_decoration_unstable_v1_types + 0 },
};
const struct wl_interface zxdg_toplevel_decoration_v1_interface = {
 "zxdg_toplevel_decoration_v1", 1,
 3, zxdg_toplevel_decoration_v1_requests,
 1, zxdg_toplevel_decoration_v1_events,
};
#endif // ZAKERO_YETANI_IMPLEMENTATION
// }}}
// {{{ xdg-shell
struct wl_output;
struct wl_seat;
struct wl_surface;
struct xdg_popup;
struct xdg_positioner;
struct xdg_surface;
struct xdg_toplevel;
struct xdg_wm_base;
extern const struct wl_interface xdg_wm_base_interface;
extern const struct wl_interface xdg_positioner_interface;
extern const struct wl_interface xdg_surface_interface;
extern const struct wl_interface xdg_toplevel_interface;
extern const struct wl_interface xdg_popup_interface;
enum xdg_wm_base_error {
 XDG_WM_BASE_ERROR_ROLE = 0,
 XDG_WM_BASE_ERROR_DEFUNCT_SURFACES = 1,
 XDG_WM_BASE_ERROR_NOT_THE_TOPMOST_POPUP = 2,
 XDG_WM_BASE_ERROR_INVALID_POPUP_PARENT = 3,
 XDG_WM_BASE_ERROR_INVALID_SURFACE_STATE = 4,
 XDG_WM_BASE_ERROR_INVALID_POSITIONER = 5,
};
struct xdg_wm_base_listener {
 void (*ping)(void *data,
       struct xdg_wm_base *xdg_wm_base,
       uint32_t serial);
};
static inline int
xdg_wm_base_add_listener(struct xdg_wm_base *xdg_wm_base,
    const struct xdg_wm_base_listener *listener, void *data)
{
 return wl_proxy_add_listener((struct wl_proxy *) xdg_wm_base,
         (void (**)(void)) listener, data);
}
static inline void
xdg_wm_base_set_user_data(struct xdg_wm_base *xdg_wm_base, void *user_data)
{
 wl_proxy_set_user_data((struct wl_proxy *) xdg_wm_base, user_data);
}
static inline void *
xdg_wm_base_get_user_data(struct xdg_wm_base *xdg_wm_base)
{
 return wl_proxy_get_user_data((struct wl_proxy *) xdg_wm_base);
}
static inline uint32_t
xdg_wm_base_get_version(struct xdg_wm_base *xdg_wm_base)
{
 return wl_proxy_get_version((struct wl_proxy *) xdg_wm_base);
}
static inline void
xdg_wm_base_destroy(struct xdg_wm_base *xdg_wm_base)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_wm_base,
    0);
 wl_proxy_destroy((struct wl_proxy *) xdg_wm_base);
}
static inline struct xdg_positioner *
xdg_wm_base_create_positioner(struct xdg_wm_base *xdg_wm_base)
{
 struct wl_proxy *id;
 id = wl_proxy_marshal_constructor((struct wl_proxy *) xdg_wm_base,
    1, &xdg_positioner_interface, NULL);
 return (struct xdg_positioner *) id;
}
static inline struct xdg_surface *
xdg_wm_base_get_xdg_surface(struct xdg_wm_base *xdg_wm_base, struct wl_surface *surface)
{
 struct wl_proxy *id;
 id = wl_proxy_marshal_constructor((struct wl_proxy *) xdg_wm_base,
    2, &xdg_surface_interface, NULL, surface);
 return (struct xdg_surface *) id;
}
static inline void
xdg_wm_base_pong(struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_wm_base,
    3, serial);
}
enum xdg_positioner_error {
 XDG_POSITIONER_ERROR_INVALID_INPUT = 0,
};
enum xdg_positioner_anchor {
 XDG_POSITIONER_ANCHOR_NONE = 0,
 XDG_POSITIONER_ANCHOR_TOP = 1,
 XDG_POSITIONER_ANCHOR_BOTTOM = 2,
 XDG_POSITIONER_ANCHOR_LEFT = 3,
 XDG_POSITIONER_ANCHOR_RIGHT = 4,
 XDG_POSITIONER_ANCHOR_TOP_LEFT = 5,
 XDG_POSITIONER_ANCHOR_BOTTOM_LEFT = 6,
 XDG_POSITIONER_ANCHOR_TOP_RIGHT = 7,
 XDG_POSITIONER_ANCHOR_BOTTOM_RIGHT = 8,
};
enum xdg_positioner_gravity {
 XDG_POSITIONER_GRAVITY_NONE = 0,
 XDG_POSITIONER_GRAVITY_TOP = 1,
 XDG_POSITIONER_GRAVITY_BOTTOM = 2,
 XDG_POSITIONER_GRAVITY_LEFT = 3,
 XDG_POSITIONER_GRAVITY_RIGHT = 4,
 XDG_POSITIONER_GRAVITY_TOP_LEFT = 5,
 XDG_POSITIONER_GRAVITY_BOTTOM_LEFT = 6,
 XDG_POSITIONER_GRAVITY_TOP_RIGHT = 7,
 XDG_POSITIONER_GRAVITY_BOTTOM_RIGHT = 8,
};
enum xdg_positioner_constraint_adjustment {
 XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_NONE = 0,
 XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_X = 1,
 XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_Y = 2,
 XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_X = 4,
 XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_Y = 8,
 XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_RESIZE_X = 16,
 XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_RESIZE_Y = 32,
};
static inline void
xdg_positioner_set_user_data(struct xdg_positioner *xdg_positioner, void *user_data)
{
 wl_proxy_set_user_data((struct wl_proxy *) xdg_positioner, user_data);
}
static inline void *
xdg_positioner_get_user_data(struct xdg_positioner *xdg_positioner)
{
 return wl_proxy_get_user_data((struct wl_proxy *) xdg_positioner);
}
static inline uint32_t
xdg_positioner_get_version(struct xdg_positioner *xdg_positioner)
{
 return wl_proxy_get_version((struct wl_proxy *) xdg_positioner);
}
static inline void
xdg_positioner_destroy(struct xdg_positioner *xdg_positioner)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_positioner,
    0);
 wl_proxy_destroy((struct wl_proxy *) xdg_positioner);
}
static inline void
xdg_positioner_set_size(struct xdg_positioner *xdg_positioner, int32_t width, int32_t height)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_positioner,
    1, width, height);
}
static inline void
xdg_positioner_set_anchor_rect(struct xdg_positioner *xdg_positioner, int32_t x, int32_t y, int32_t width, int32_t height)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_positioner,
    2, x, y, width, height);
}
static inline void
xdg_positioner_set_anchor(struct xdg_positioner *xdg_positioner, uint32_t anchor)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_positioner,
    3, anchor);
}
static inline void
xdg_positioner_set_gravity(struct xdg_positioner *xdg_positioner, uint32_t gravity)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_positioner,
    4, gravity);
}
static inline void
xdg_positioner_set_constraint_adjustment(struct xdg_positioner *xdg_positioner, uint32_t constraint_adjustment)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_positioner,
    5, constraint_adjustment);
}
static inline void
xdg_positioner_set_offset(struct xdg_positioner *xdg_positioner, int32_t x, int32_t y)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_positioner,
    6, x, y);
}
static inline void
xdg_positioner_set_reactive(struct xdg_positioner *xdg_positioner)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_positioner,
    7);
}
static inline void
xdg_positioner_set_parent_size(struct xdg_positioner *xdg_positioner, int32_t parent_width, int32_t parent_height)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_positioner,
    8, parent_width, parent_height);
}
static inline void
xdg_positioner_set_parent_configure(struct xdg_positioner *xdg_positioner, uint32_t serial)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_positioner,
    9, serial);
}
enum xdg_surface_error {
 XDG_SURFACE_ERROR_NOT_CONSTRUCTED = 1,
 XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED = 2,
 XDG_SURFACE_ERROR_UNCONFIGURED_BUFFER = 3,
};
struct xdg_surface_listener {
 void (*configure)(void *data,
     struct xdg_surface *xdg_surface,
     uint32_t serial);
};
static inline int
xdg_surface_add_listener(struct xdg_surface *xdg_surface,
    const struct xdg_surface_listener *listener, void *data)
{
 return wl_proxy_add_listener((struct wl_proxy *) xdg_surface,
         (void (**)(void)) listener, data);
}
static inline void
xdg_surface_set_user_data(struct xdg_surface *xdg_surface, void *user_data)
{
 wl_proxy_set_user_data((struct wl_proxy *) xdg_surface, user_data);
}
static inline void *
xdg_surface_get_user_data(struct xdg_surface *xdg_surface)
{
 return wl_proxy_get_user_data((struct wl_proxy *) xdg_surface);
}
static inline uint32_t
xdg_surface_get_version(struct xdg_surface *xdg_surface)
{
 return wl_proxy_get_version((struct wl_proxy *) xdg_surface);
}
static inline void
xdg_surface_destroy(struct xdg_surface *xdg_surface)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_surface,
    0);
 wl_proxy_destroy((struct wl_proxy *) xdg_surface);
}
static inline struct xdg_toplevel *
xdg_surface_get_toplevel(struct xdg_surface *xdg_surface)
{
 struct wl_proxy *id;
 id = wl_proxy_marshal_constructor((struct wl_proxy *) xdg_surface,
    1, &xdg_toplevel_interface, NULL);
 return (struct xdg_toplevel *) id;
}
static inline struct xdg_popup *
xdg_surface_get_popup(struct xdg_surface *xdg_surface, struct xdg_surface *parent, struct xdg_positioner *positioner)
{
 struct wl_proxy *id;
 id = wl_proxy_marshal_constructor((struct wl_proxy *) xdg_surface,
    2, &xdg_popup_interface, NULL, parent, positioner);
 return (struct xdg_popup *) id;
}
static inline void
xdg_surface_set_window_geometry(struct xdg_surface *xdg_surface, int32_t x, int32_t y, int32_t width, int32_t height)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_surface,
    3, x, y, width, height);
}
static inline void
xdg_surface_ack_configure(struct xdg_surface *xdg_surface, uint32_t serial)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_surface,
    4, serial);
}
enum xdg_toplevel_resize_edge {
 XDG_TOPLEVEL_RESIZE_EDGE_NONE = 0,
 XDG_TOPLEVEL_RESIZE_EDGE_TOP = 1,
 XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM = 2,
 XDG_TOPLEVEL_RESIZE_EDGE_LEFT = 4,
 XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT = 5,
 XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT = 6,
 XDG_TOPLEVEL_RESIZE_EDGE_RIGHT = 8,
 XDG_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT = 9,
 XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT = 10,
};
enum xdg_toplevel_state {
 XDG_TOPLEVEL_STATE_MAXIMIZED = 1,
 XDG_TOPLEVEL_STATE_FULLSCREEN = 2,
 XDG_TOPLEVEL_STATE_RESIZING = 3,
 XDG_TOPLEVEL_STATE_ACTIVATED = 4,
 XDG_TOPLEVEL_STATE_TILED_LEFT = 5,
 XDG_TOPLEVEL_STATE_TILED_RIGHT = 6,
 XDG_TOPLEVEL_STATE_TILED_TOP = 7,
 XDG_TOPLEVEL_STATE_TILED_BOTTOM = 8,
};
struct xdg_toplevel_listener {
 void (*configure)(void *data,
     struct xdg_toplevel *xdg_toplevel,
     int32_t width,
     int32_t height,
     struct wl_array *states);
 void (*close)(void *data,
        struct xdg_toplevel *xdg_toplevel);
};
static inline int
xdg_toplevel_add_listener(struct xdg_toplevel *xdg_toplevel,
     const struct xdg_toplevel_listener *listener, void *data)
{
 return wl_proxy_add_listener((struct wl_proxy *) xdg_toplevel,
         (void (**)(void)) listener, data);
}
static inline void
xdg_toplevel_set_user_data(struct xdg_toplevel *xdg_toplevel, void *user_data)
{
 wl_proxy_set_user_data((struct wl_proxy *) xdg_toplevel, user_data);
}
static inline void *
xdg_toplevel_get_user_data(struct xdg_toplevel *xdg_toplevel)
{
 return wl_proxy_get_user_data((struct wl_proxy *) xdg_toplevel);
}
static inline uint32_t
xdg_toplevel_get_version(struct xdg_toplevel *xdg_toplevel)
{
 return wl_proxy_get_version((struct wl_proxy *) xdg_toplevel);
}
static inline void
xdg_toplevel_destroy(struct xdg_toplevel *xdg_toplevel)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_toplevel,
    0);
 wl_proxy_destroy((struct wl_proxy *) xdg_toplevel);
}
static inline void
xdg_toplevel_set_parent(struct xdg_toplevel *xdg_toplevel, struct xdg_toplevel *parent)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_toplevel,
    1, parent);
}
static inline void
xdg_toplevel_set_title(struct xdg_toplevel *xdg_toplevel, const char *title)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_toplevel,
    2, title);
}
static inline void
xdg_toplevel_set_app_id(struct xdg_toplevel *xdg_toplevel, const char *app_id)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_toplevel,
    3, app_id);
}
static inline void
xdg_toplevel_show_window_menu(struct xdg_toplevel *xdg_toplevel, struct wl_seat *seat, uint32_t serial, int32_t x, int32_t y)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_toplevel,
    4, seat, serial, x, y);
}
static inline void
xdg_toplevel_move(struct xdg_toplevel *xdg_toplevel, struct wl_seat *seat, uint32_t serial)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_toplevel,
    5, seat, serial);
}
static inline void
xdg_toplevel_resize(struct xdg_toplevel *xdg_toplevel, struct wl_seat *seat, uint32_t serial, uint32_t edges)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_toplevel,
    6, seat, serial, edges);
}
static inline void
xdg_toplevel_set_max_size(struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_toplevel,
    7, width, height);
}
static inline void
xdg_toplevel_set_min_size(struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_toplevel,
    8, width, height);
}
static inline void
xdg_toplevel_set_maximized(struct xdg_toplevel *xdg_toplevel)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_toplevel,
    9);
}
static inline void
xdg_toplevel_unset_maximized(struct xdg_toplevel *xdg_toplevel)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_toplevel,
    10);
}
static inline void
xdg_toplevel_set_fullscreen(struct xdg_toplevel *xdg_toplevel, struct wl_output *output)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_toplevel,
    11, output);
}
static inline void
xdg_toplevel_unset_fullscreen(struct xdg_toplevel *xdg_toplevel)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_toplevel,
    12);
}
static inline void
xdg_toplevel_set_minimized(struct xdg_toplevel *xdg_toplevel)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_toplevel,
    13);
}
enum xdg_popup_error {
 XDG_POPUP_ERROR_INVALID_GRAB = 0,
};
struct xdg_popup_listener {
 void (*configure)(void *data,
     struct xdg_popup *xdg_popup,
     int32_t x,
     int32_t y,
     int32_t width,
     int32_t height);
 void (*popup_done)(void *data,
      struct xdg_popup *xdg_popup);
 void (*repositioned)(void *data,
        struct xdg_popup *xdg_popup,
        uint32_t token);
};
static inline int
xdg_popup_add_listener(struct xdg_popup *xdg_popup,
         const struct xdg_popup_listener *listener, void *data)
{
 return wl_proxy_add_listener((struct wl_proxy *) xdg_popup,
         (void (**)(void)) listener, data);
}
static inline void
xdg_popup_set_user_data(struct xdg_popup *xdg_popup, void *user_data)
{
 wl_proxy_set_user_data((struct wl_proxy *) xdg_popup, user_data);
}
static inline void *
xdg_popup_get_user_data(struct xdg_popup *xdg_popup)
{
 return wl_proxy_get_user_data((struct wl_proxy *) xdg_popup);
}
static inline uint32_t
xdg_popup_get_version(struct xdg_popup *xdg_popup)
{
 return wl_proxy_get_version((struct wl_proxy *) xdg_popup);
}
static inline void
xdg_popup_destroy(struct xdg_popup *xdg_popup)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_popup,
    0);
 wl_proxy_destroy((struct wl_proxy *) xdg_popup);
}
static inline void
xdg_popup_grab(struct xdg_popup *xdg_popup, struct wl_seat *seat, uint32_t serial)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_popup,
    1, seat, serial);
}
static inline void
xdg_popup_reposition(struct xdg_popup *xdg_popup, struct xdg_positioner *positioner, uint32_t token)
{
 wl_proxy_marshal((struct wl_proxy *) xdg_popup,
    2, positioner, token);
}
#ifdef ZAKERO_YETANI_IMPLEMENTATION
extern const struct wl_interface wl_output_interface;
extern const struct wl_interface wl_seat_interface;
extern const struct wl_interface wl_surface_interface;
extern const struct wl_interface xdg_popup_interface;
extern const struct wl_interface xdg_positioner_interface;
extern const struct wl_interface xdg_surface_interface;
extern const struct wl_interface xdg_toplevel_interface;
static const struct wl_interface *xdg_shell_types[] = {
 NULL,
 NULL,
 NULL,
 NULL,
 &xdg_positioner_interface,
 &xdg_surface_interface,
 &wl_surface_interface,
 &xdg_toplevel_interface,
 &xdg_popup_interface,
 &xdg_surface_interface,
 &xdg_positioner_interface,
 &xdg_toplevel_interface,
 &wl_seat_interface,
 NULL,
 NULL,
 NULL,
 &wl_seat_interface,
 NULL,
 &wl_seat_interface,
 NULL,
 NULL,
 &wl_output_interface,
 &wl_seat_interface,
 NULL,
 &xdg_positioner_interface,
 NULL,
};
static const struct wl_message xdg_wm_base_requests[] = {
 { "destroy", "", xdg_shell_types + 0 },
 { "create_positioner", "n", xdg_shell_types + 4 },
 { "get_xdg_surface", "no", xdg_shell_types + 5 },
 { "pong", "u", xdg_shell_types + 0 },
};
static const struct wl_message xdg_wm_base_events[] = {
 { "ping", "u", xdg_shell_types + 0 },
};
const struct wl_interface xdg_wm_base_interface = {
 "xdg_wm_base", 3,
 4, xdg_wm_base_requests,
 1, xdg_wm_base_events,
};
static const struct wl_message xdg_positioner_requests[] = {
 { "destroy", "", xdg_shell_types + 0 },
 { "set_size", "ii", xdg_shell_types + 0 },
 { "set_anchor_rect", "iiii", xdg_shell_types + 0 },
 { "set_anchor", "u", xdg_shell_types + 0 },
 { "set_gravity", "u", xdg_shell_types + 0 },
 { "set_constraint_adjustment", "u", xdg_shell_types + 0 },
 { "set_offset", "ii", xdg_shell_types + 0 },
 { "set_reactive", "3", xdg_shell_types + 0 },
 { "set_parent_size", "3ii", xdg_shell_types + 0 },
 { "set_parent_configure", "3u", xdg_shell_types + 0 },
};
const struct wl_interface xdg_positioner_interface = {
 "xdg_positioner", 3,
 10, xdg_positioner_requests,
 0, NULL,
};
static const struct wl_message xdg_surface_requests[] = {
 { "destroy", "", xdg_shell_types + 0 },
 { "get_toplevel", "n", xdg_shell_types + 7 },
 { "get_popup", "n?oo", xdg_shell_types + 8 },
 { "set_window_geometry", "iiii", xdg_shell_types + 0 },
 { "ack_configure", "u", xdg_shell_types + 0 },
};
static const struct wl_message xdg_surface_events[] = {
 { "configure", "u", xdg_shell_types + 0 },
};
const struct wl_interface xdg_surface_interface = {
 "xdg_surface", 3,
 5, xdg_surface_requests,
 1, xdg_surface_events,
};
static const struct wl_message xdg_toplevel_requests[] = {
 { "destroy", "", xdg_shell_types + 0 },
 { "set_parent", "?o", xdg_shell_types + 11 },
 { "set_title", "s", xdg_shell_types + 0 },
 { "set_app_id", "s", xdg_shell_types + 0 },
 { "show_window_menu", "ouii", xdg_shell_types + 12 },
 { "move", "ou", xdg_shell_types + 16 },
 { "resize", "ouu", xdg_shell_types + 18 },
 { "set_max_size", "ii", xdg_shell_types + 0 },
 { "set_min_size", "ii", xdg_shell_types + 0 },
 { "set_maximized", "", xdg_shell_types + 0 },
 { "unset_maximized", "", xdg_shell_types + 0 },
 { "set_fullscreen", "?o", xdg_shell_types + 21 },
 { "unset_fullscreen", "", xdg_shell_types + 0 },
 { "set_minimized", "", xdg_shell_types + 0 },
};
static const struct wl_message xdg_toplevel_events[] = {
 { "configure", "iia", xdg_shell_types + 0 },
 { "close", "", xdg_shell_types + 0 },
};
const struct wl_interface xdg_toplevel_interface = {
 "xdg_toplevel", 3,
 14, xdg_toplevel_requests,
 2, xdg_toplevel_events,
};
static const struct wl_message xdg_popup_requests[] = {
 { "destroy", "", xdg_shell_types + 0 },
 { "grab", "ou", xdg_shell_types + 22 },
 { "reposition", "3ou", xdg_shell_types + 24 },
};
static const struct wl_message xdg_popup_events[] = {
 { "configure", "iiii", xdg_shell_types + 0 },
 { "popup_done", "", xdg_shell_types + 0 },
 { "repositioned", "3u", xdg_shell_types + 0 },
};
const struct wl_interface xdg_popup_interface = {
 "xdg_popup", 3,
 3, xdg_popup_requests,
 3, xdg_popup_events,
};
#endif // ZAKERO_YETANI_IMPLEMENTATION
// }}}
// }}}

/**
 * \endcond
 */

namespace zakero
{
	// {{{ Declaration

	class Yetani
	{
		public:
#define X(name_, val_, mesg_) \
			static constexpr int name_ = val_;
			ZAKERO_YETANI__ERROR_DATA
#undef X

			virtual ~Yetani() noexcept;

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

				friend bool operator==(Yetani::PointMm&, Yetani::PointMm&) noexcept;
			};

			struct PointPercent
			{
				uint32_t time;
				float    x;
				float    y;

				friend bool operator==(Yetani::PointPercent&, Yetani::PointPercent&) noexcept;
			};

			struct PointPixel
			{
				uint32_t time;
				int32_t  x;
				int32_t  y;

				friend bool operator==(Yetani::PointPixel&, Yetani::PointPixel&) noexcept;
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
				Yetani::PointerAxisSource source;
				Yetani::PointerAxisType   type;
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
				Yetani::PointerButtonState state;
			};

			// }}}
			// {{{ Type : Size

			struct SizeMm
			{
				float width;
				float height;

				friend bool operator==(Yetani::SizeMm&, Yetani::SizeMm&) noexcept;
			};

			struct SizePercent
			{
				float width;
				float height;

				friend bool operator==(Yetani::SizePercent&, Yetani::SizePercent&) noexcept;
			};

			struct SizePixel
			{
				int32_t width;
				int32_t height;

				friend bool operator==(Yetani::SizePixel&, Yetani::SizePixel&) noexcept;
			};

			// }}}
			// {{{ Connection

			static Yetani* connect() noexcept;
			static Yetani* connect(const std::string&) noexcept;
			static Yetani* connect(std::error_code&) noexcept;
			static Yetani* connect(const std::string&, std::error_code&) noexcept;

			// }}}
			// {{{ Cursor

			struct CursorConfig
			{
				Yetani::SizePixel         size           = {};
				wl_shm_format             format         = WL_SHM_FORMAT_ARGB8888;
				int32_t                   hotspot_x      = 0;
				int32_t                   hotspot_y      = 0;
				std::chrono::milliseconds time_per_frame = std::chrono::milliseconds(0);
				const std::vector<void*>& image_data;
			};

			// -------------------------------------------------- //

			std::error_code cursorCreate(const std::string&, const Yetani::CursorConfig&) noexcept;
			std::error_code cursorDestroy(const std::string&) noexcept;

			// }}}
			// {{{ Keyboard

			int32_t keyRepeatDelay() const noexcept;
			int32_t keyRepeatRate() const noexcept;

			// }}}
			// {{{ Output : Wayland

			struct Output
			{
				std::string make                     = "";
				std::string model                    = "";
				int32_t     x                        = 0;
				int32_t     y                        = 0;
				int32_t     width                    = 0;
				int32_t     height                   = 0;
				uint32_t    physical_width_mm        = 0;
				uint32_t    physical_height_mm       = 0;
				int32_t     subpixel                 = 0;
				int32_t     refresh_mHz              = 0;
				int32_t     scale_factor             = 0;
				int32_t     transform                = 0;
				uint32_t    flags                    = 0;
				float       pixels_per_mm_horizontal = 0.0;
				float       pixels_per_mm_vertical   = 0.0;
			};

			// -------------------------------------------------- //

			using OutputId = uint32_t;

			using LambdaOutputId = std::function<void(const Yetani::OutputId)>;

			using VectorOutputId = std::vector<OutputId>;

			// -------------------------------------------------- //

			Yetani::Output         output(const Yetani::OutputId) const noexcept;
			Yetani::VectorOutputId outputVector() const noexcept;
			static std::string     outputSubpixelName(int32_t) noexcept;
			static std::string     outputTransformName(int32_t) noexcept;

			Yetani::PointMm        outputConvertToMm(const Yetani::OutputId, const Yetani::PointPixel&) const noexcept;
			Yetani::PointPercent   outputConvertToPercent(const Yetani::OutputId, const Yetani::PointPixel&) const noexcept;
			Yetani::PointPixel     outputConvertToPixel(const Yetani::OutputId, const Yetani::PointMm&) const noexcept;
			Yetani::PointPixel     outputConvertToPixel(const Yetani::OutputId, const Yetani::PointPercent&) const noexcept;

			Yetani::SizeMm         outputConvertToMm(const Yetani::OutputId, const Yetani::SizePixel&) const noexcept;
			Yetani::SizePercent    outputConvertToPercent(const Yetani::OutputId, const Yetani::SizePixel&) const noexcept;
			Yetani::SizePixel      outputConvertToPixel(const Yetani::OutputId, const Yetani::SizeMm&) const noexcept;
			Yetani::SizePixel      outputConvertToPixel(const Yetani::OutputId, const Yetani::SizePercent&) const noexcept;

			void                   outputOnAdd(Yetani::LambdaOutputId) noexcept;
			void                   outputOnChange(Yetani::LambdaOutputId) noexcept;
			void                   outputOnRemove(Yetani::LambdaOutputId) noexcept;

			// }}}
			// {{{ Output : Xdg

			/* Future
			struct XdgOutput
			{
				std::string xdg_name;
				std::string xdg_description;
				int32_t     xdg_logical_x;
				int32_t     xdg_logical_y;
				int32_t     xdg_logical_width;
				int32_t     xdg_logical_height;
			};

			using MapXdgOutputData = std::unordered_map<OutputId, XdgOutput>;

			// -------------------------------------------------- //

			const MapOutputIdXdgOutput& xdgOutputMap() const noexcept;
			*/

			// }}}
			// {{{ Shared Memory

			using VectorShmFormat = std::vector<wl_shm_format>;

			// -------------------------------------------------- //

			const Yetani::VectorShmFormat& shmFormatAvailable() const noexcept;
			static uint8_t                 shmFormatBytesPerPixel(const wl_shm_format) noexcept;
			static std::string             shmFormatDescription(const wl_shm_format) noexcept;
			static std::string             shmFormatName(const wl_shm_format) noexcept;

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
			using LambdaKey               = std::function<void(const Yetani::Key&, const Yetani::KeyModifier&)>;
			using LambdaAxis              = std::function<void(const Yetani::PointerAxis&, const Yetani::KeyModifier&)>;
			using LambdaButtonMm          = std::function<void(const Yetani::PointerButton&, const Yetani::PointMm&, const Yetani::KeyModifier&)>;
			using LambdaButtonPercent     = std::function<void(const Yetani::PointerButton&, const Yetani::PointPercent&, const Yetani::KeyModifier&)>;
			using LambdaButtonPixel       = std::function<void(const Yetani::PointerButton&, const Yetani::PointPixel&, const Yetani::KeyModifier&)>;
			using LambdaPointMm           = std::function<void(const Yetani::PointMm&, const Yetani::KeyModifier&)>;
			using LambdaPointPercent      = std::function<void(const Yetani::PointPercent&, const Yetani::KeyModifier&)>;
			using LambdaPointPixel        = std::function<void(const Yetani::PointPixel&, const Yetani::KeyModifier&)>;
			using LambdaBool              = std::function<void(bool)>;
			using LambdaWindowDecorations = std::function<void(Yetani::WindowDecorations)>;
			using LambdaWindowMode        = std::function<void(Yetani::WindowMode)>;
			using LambdaSizeMm            = std::function<void(const Yetani::SizeMm&)>;
			using LambdaSizePercent       = std::function<void(const Yetani::SizePercent&)>;
			using LambdaSizePixel         = std::function<void(const Yetani::SizePixel&)>;

			// -------------------------------------------------- //

			class Window
			{
				public:
					Window(void*);
					virtual ~Window();

					// {{{ Configuration

					void                 classSet(const std::string&) noexcept;
					void                 titleSet(const std::string&) noexcept;

					// }}}
					// {{{ Decorations

					std::error_code      decorationsSet(const Yetani::WindowDecorations) noexcept;
					void                 decorationsOnChange(Yetani::LambdaWindowDecorations) noexcept;

					// }}}
					// {{{ Size

					std::error_code      sizeSet(const Yetani::SizeMm&) noexcept;
					std::error_code      sizeSet(const Yetani::SizePercent&) noexcept;
					std::error_code      sizeSet(const Yetani::SizePixel&) noexcept;
					std::error_code      sizeSetMinMax(const Yetani::SizeMm&, const Yetani::SizeMm&) noexcept;
					std::error_code      sizeSetMinMax(const Yetani::SizePercent&, const Yetani::SizePercent&) noexcept;
					std::error_code      sizeSetMinMax(const Yetani::SizePixel&, const Yetani::SizePixel&) noexcept;
					void                 sizeOnChange(Yetani::LambdaSizeMm) noexcept;
					void                 sizeOnChange(Yetani::LambdaSizePercent) noexcept;
					void                 sizeOnChange(Yetani::LambdaSizePixel) noexcept;

					// }}}
					// {{{ Window Mode

					Yetani::WindowMode   windowMode() noexcept;
					bool                 windowModeIs(const Yetani::WindowMode) noexcept;
					void                 windowModeSet(const Yetani::WindowMode) noexcept;
					void                 windowModeOnChange(Yetani::LambdaWindowMode) noexcept;

					void                 minimize() noexcept;

					// }}}
					// {{{ Rendering

					std::error_code      imageNext(uint8_t*&, Yetani::SizePixel&) noexcept;
					void                 imagePresent() noexcept;
					uint32_t             time() const noexcept;
					uint8_t              bytesPerPixel() const noexcept;

					// }}}
					// {{{ Conversion

					Yetani::PointMm      convertToMm(const Yetani::PointPixel&) const noexcept;
					Yetani::PointPercent convertToPercent(const Yetani::PointPixel&) const noexcept;
					Yetani::PointPixel   convertToPixel(const Yetani::PointMm&) const noexcept;
					Yetani::PointPixel   convertToPixel(const Yetani::PointPercent&) const noexcept;

					Yetani::SizeMm       convertToMm(const Yetani::SizePixel&) const noexcept;
					Yetani::SizePercent  convertToPercent(const Yetani::SizePixel&) const noexcept;
					Yetani::SizePixel    convertToPixel(const Yetani::SizeMm&) const noexcept;
					Yetani::SizePixel    convertToPixel(const Yetani::SizePercent&) const noexcept;

					// }}}
					// {{{ Cursor

					std::error_code      cursorUse(const std::string&) noexcept;
					void                 cursorHide() noexcept;
					void                 cursorShow() noexcept;

					// }}}
					// {{{ Keyboard

					void                 keyboardOnEnter(Yetani::Lambda) noexcept;
					void                 keyboardOnLeave(Yetani::Lambda) noexcept;
					void                 keyboardOnKey(Yetani::LambdaKey) noexcept;

					// }}}
					// {{{ Pointer

					void                 pointerOnAxis(Yetani::LambdaAxis) noexcept;
					void                 pointerOnButton(Yetani::LambdaButtonMm) noexcept;
					void                 pointerOnButton(Yetani::LambdaButtonPercent) noexcept;
					void                 pointerOnButton(Yetani::LambdaButtonPixel) noexcept;
					void                 pointerOnEnter(Yetani::LambdaPointMm) noexcept;
					void                 pointerOnEnter(Yetani::LambdaPointPercent) noexcept;
					void                 pointerOnEnter(Yetani::LambdaPointPixel) noexcept;
					void                 pointerOnLeave(Yetani::Lambda) noexcept;
					void                 pointerOnMotion(Yetani::LambdaPointMm) noexcept;
					void                 pointerOnMotion(Yetani::LambdaPointPercent) noexcept;
					void                 pointerOnMotion(Yetani::LambdaPointPixel) noexcept;

					// Not Used? Or Future?
					void                 pointerOnAxisSource(Yetani::Lambda) noexcept;
					void                 pointerOnAxisStop(Yetani::Lambda) noexcept;
					void                 pointerOnAxisDiscrete(Yetani::Lambda) noexcept;

					// }}}
					// {{{ Events

					void                 onCloseRequest(Yetani::Lambda) noexcept;
					void                 onFocusChange(Yetani::LambdaBool) noexcept;

					// }}}

					struct Memory
					{
						struct wl_shm_pool* wl_shm_pool;
						zakero::MemoryPool  memory_pool;
					};

				private:
					Yetani*                             yetani;
					struct wl_buffer*                   wl_buffer;
					struct wl_surface*                  wl_surface;
					struct xdg_surface*                 xdg_surface;
					struct xdg_toplevel*                xdg_toplevel;
					struct zxdg_toplevel_decoration_v1* xdg_decoration;
					Yetani::Window::Memory              window_memory;
					wl_shm_format                       pixel_format;

					Window(const Window&) = delete;
					Window& operator=(const Window&) = delete;
			};

			// -------------------------------------------------- //

			static constexpr wl_shm_format SHM_FORMAT_DEFAULT = WL_SHM_FORMAT_XRGB8888;

			// -------------------------------------------------- //

			Yetani::Window* windowCreate(const Yetani::SizeMm&, std::error_code&) noexcept;
			Yetani::Window* windowCreate(const Yetani::SizeMm&, const wl_shm_format = SHM_FORMAT_DEFAULT) noexcept;
			Yetani::Window* windowCreate(const Yetani::SizeMm&, const wl_shm_format, std::error_code&) noexcept;
			Yetani::Window* windowCreate(const Yetani::SizePercent&, std::error_code&) noexcept;
			Yetani::Window* windowCreate(const Yetani::SizePercent&, const wl_shm_format = SHM_FORMAT_DEFAULT) noexcept;
			Yetani::Window* windowCreate(const Yetani::SizePercent&, const wl_shm_format, std::error_code&) noexcept;
			Yetani::Window* windowCreate(const Yetani::SizePixel&, std::error_code&) noexcept;
			Yetani::Window* windowCreate(const Yetani::SizePixel&, const wl_shm_format = SHM_FORMAT_DEFAULT) noexcept;
			Yetani::Window* windowCreate(const Yetani::SizePixel&, const wl_shm_format, std::error_code&) noexcept;

			// }}}

		private:
			static constexpr uint32_t Size_Max = (uint32_t)std::numeric_limits<int32_t>::max();

			Yetani() noexcept;

			// {{{ Type

			using VectorWlSurface = std::vector<struct wl_surface*>;

			// }}}
			// {{{ Connection

			void disconnect() noexcept;

			// }}}
			// {{{ Cursor

			struct Cursor
			{
				struct wl_surface*        wl_surface      = nullptr;
				std::vector<::wl_buffer*> buffer_vector   = {};
				wl_shm_format             format          = WL_SHM_FORMAT_ARGB8888;
				int64_t                   next_frame_time = 0;
				size_t                    buffer_index    = 0;
				uint32_t                  time_per_frame  = 0;
				int32_t                   width           = 0;
				int32_t                   height          = 0;
				int32_t                   hotspot_x       = 0;
				int32_t                   hotspot_y       = 0;
			};

			using MapStringCursor = std::unordered_map<std::string, Yetani::Cursor>;

			MapStringCursor cursor_map;

			// -------------------------------------------------- //

			struct CursorSurface
			{
				struct wl_pointer* wl_pointer;
				struct wl_surface* wl_surface;
				uint32_t           serial;
				int32_t            hotspot_x;
				int32_t            hotspot_y;
				bool               is_visible;
			};

			using MapCursorSurface = std::unordered_map<struct wl_surface*, Yetani::CursorSurface>;

			MapCursorSurface cursor_surface_map;

			// -------------------------------------------------- //

			zakero::MemoryPool  cursor_memory_pool;
			mutable std::mutex  cursor_mutex;
			struct wl_shm_pool* cursor_shm_pool;
			struct wl_pointer*  cursor_pointer;

			// -------------------------------------------------- //

			void            cursorAnimate() noexcept;
			std::error_code cursorCreateCursor(const std::string&, const Yetani::CursorConfig&) noexcept;
			void            cursorEnter(struct wl_pointer*, uint32_t, struct wl_surface*) noexcept;
			void            cursorLeave(struct wl_surface*) noexcept;
			void            cursorHide(struct wl_surface*) noexcept;
			void            cursorShow(struct wl_surface*) noexcept;
			bool            cursorIsHidden(struct wl_surface*) const noexcept;
			void            cursorSetup() noexcept;
			void            cursorTeardown() noexcept;
			std::error_code cursorAttach(const std::string&, struct wl_surface*) noexcept;
			std::error_code cursorDetach(struct wl_surface*) noexcept;

			// }}}
			// {{{ Event Loop

			std::jthread      event_loop;
			std::atomic<bool> event_loop_is_running;

			// -------------------------------------------------- //

			void        eventLoopStart() noexcept;
			static void eventLoop(std::stop_token, Yetani*) noexcept;

			// }}}
			// {{{ Wayland

			struct wl_compositor*   compositor;
			struct wl_display*      display;
			struct wl_registry*     registry;
			struct wl_shm*          shm;
			Yetani::VectorShmFormat shm_format_vector;

			// }}}
			// {{{ Wayland : Seat

			struct Seat
			{
				struct wl_keyboard* wl_keyboard = nullptr;
				struct wl_pointer*  wl_pointer  = nullptr;
				struct wl_touch*    wl_touch    = nullptr;
				std::string         name        = "";
				uint32_t            version     = 0;
			};

			using MapSeat     = std::map<struct wl_seat*, Seat>;
			using MapIdWlSeat = std::map<uint32_t, struct wl_seat*>;

			Yetani::MapSeat     seat_map;
			Yetani::MapIdWlSeat id_to_seat;

			// -------------------------------------------------- //

			struct wl_seat* seat;

			// -------------------------------------------------- //

			void seatDestroy(struct wl_seat*&) noexcept;

			// }}}
			// {{{ Wayland : Seat : Keyboard

			struct KeyRepeatData
			{
				std::chrono::time_point<std::chrono::steady_clock> trigger_time = {};
				uint32_t                                           base_time    = 0;
			};

			using KeyRepeatMap = std::map<uint32_t, Yetani::KeyRepeatData>;

			// -------------------------------------------------- //

			struct KeyboardEvent
			{
				Yetani::Lambda    on_enter = {};
				Yetani::Lambda    on_leave = {};
				Yetani::LambdaKey on_key   = {};
			};

			using MapKeyboardEvent = std::unordered_map<struct wl_surface*, Yetani::KeyboardEvent>;

			// -------------------------------------------------- //

			struct Keyboard
			{
				struct wl_surface*       wl_surface   = nullptr;
				Yetani::KeyboardEvent*   event        = nullptr;
				Yetani::MapKeyboardEvent event_map    = {};
				Yetani::KeyModifier      modifier     = {};
				Yetani::KeyRepeatMap     repeat_map   = {};
				char*                    keymap       = nullptr;
				uint32_t                 keymap_size  = 0;
				int32_t                  repeat_delay = 0;
				int32_t                  repeat_rate  = 0;
			};

			Yetani::Keyboard keyboard;

			// -------------------------------------------------- //

			static void keyboardDestroy(Yetani::Keyboard&) noexcept;
			static void keyboardRepeat(Yetani::Keyboard&) noexcept;
			static void keyboardRepeatAdd(Yetani::Keyboard&, uint32_t, uint32_t) noexcept;
			static void keyboardRepeatReleaseAll(Yetani::Keyboard&) noexcept;
			static void keyboardRepeatRemove(Yetani::Keyboard&, uint32_t) noexcept;

			// }}}
			// {{{ Wayland : Seat : Pointer

			struct PointerEvent
			{
				Yetani::LambdaAxis          on_axis           = {};
				Yetani::Lambda              on_axis_discrete  = {};
				Yetani::Lambda              on_axis_source    = {};
				Yetani::Lambda              on_axis_stop      = {};
				Yetani::LambdaButtonMm      on_button_mm      = {};
				Yetani::LambdaButtonPercent on_button_percent = {};
				Yetani::LambdaButtonPixel   on_button_pixel   = {};
				Yetani::LambdaPointMm       on_enter_mm       = {};
				Yetani::LambdaPointPercent  on_enter_percent  = {};
				Yetani::LambdaPointPixel    on_enter_pixel    = {};
				Yetani::Lambda              on_leave          = {};
				Yetani::LambdaPointMm       on_motion_mm      = {};
				Yetani::LambdaPointPercent  on_motion_percent = {};
				Yetani::LambdaPointPixel    on_motion_pixel   = {};
			};

			using MapPointerEvent = std::unordered_map<struct wl_surface*, Yetani::PointerEvent>;

			// -------------------------------------------------- //

			struct Pointer
			{
				// --- Common --- //
				Yetani*                 yetani            = nullptr;
				struct wl_surface*      wl_surface        = nullptr;
				struct wl_pointer*      wl_pointer        = nullptr;
				Yetani::PointerEvent*   event             = nullptr;
				Yetani::MapPointerEvent event_map         = {};

				// --- Processed Data --- //
				Yetani::PointMm         point_mm          = {};
				Yetani::PointPercent    point_percent     = {};
				Yetani::PointPixel      point_pixel       = {};

				// --- Axis --- //
				Yetani::PointerAxis     axis              = {};

				// --- Button --- //
				Yetani::PointerButton   button            = {};
				uint32_t                button_event_code = 0;
				bool                    button_is_pressed = false;
				uint32_t                button_time       = {};

				// --- Enter --- //
				struct wl_surface*      enter_surface     = nullptr;
				Yetani::PointPixel      enter_point       = {};
				uint32_t                enter_serial      = 0;

				// --- Leave --- //
				struct wl_surface*      leave_surface     = nullptr;

				// --- Motion --- //
				Yetani::PointPixel      motion_point      = {};
			};

			Yetani::Pointer pointer;

			// -------------------------------------------------- //

			static void pointerClear(struct Pointer&) noexcept;

			// }}}
			// {{{ Wayland : Output

			enum struct OutputState
			{	Done
			,	Added
			,	Changed
			};

			// -------------------------------------------------- //

			using VectorWlOutput = std::vector<struct wl_output*>;

			using MapWlOutputOutputState     = std::unordered_map<struct wl_output*, Yetani::OutputState>;
			using MapOutputIdWlOutput        = std::unordered_map<Yetani::OutputId, struct wl_output*>;
			using MapWlOutputOutputId        = std::unordered_map<struct wl_output*, Yetani::OutputId>;
			using MapWlSurfaceVectorWlOutput = std::unordered_map<struct wl_surface*, Yetani::VectorWlOutput>;
			using MapWlOutputOutput          = std::unordered_map<struct wl_output*, Output>;

			// -------------------------------------------------- //

			struct OutputData
			{
				Yetani::MapWlSurfaceVectorWlOutput surface_output_map   = {};
				Yetani::MapOutputIdWlOutput        outputid_to_wloutput = {};
				Yetani::MapWlOutputOutput          output_map           = {};
				Yetani::MapWlOutputOutputId        wloutput_to_outputid = {};
				mutable std::mutex                 mutex                = {};
			};

			Yetani::OutputData output_data;

			// -------------------------------------------------- //

			Yetani::LambdaOutputId         on_output_add;
			Yetani::LambdaOutputId         on_output_change;
			Yetani::LambdaOutputId         on_output_remove;

			Yetani::MapWlOutputOutput      output_changes_map;
			Yetani::MapWlOutputOutputState output_state_map;

			Yetani::VectorWlSurface        output_notify_surface_vector;

			// -------------------------------------------------- //

			void                        convertPixel(struct wl_surface*, const int32_t, const int32_t, float&, float&, float&, float&) const noexcept;

			std::pair<float, float>     convertPixelToMm(const Yetani::Output&, int32_t, int32_t) const noexcept;
			std::pair<float, float>     convertPixelToPercent(const Yetani::Output&, int32_t, int32_t) const noexcept;
			std::pair<int32_t, int32_t> convertMmToPixel(const Yetani::Output&, float, float) const noexcept;
			std::pair<int32_t, int32_t> convertPercentToPixel(const Yetani::Output&, float, float) const noexcept;

			static void                 outputNotifySurface(Yetani*, struct wl_output*, struct wl_surface*) noexcept;

			// }}}
			// {{{ Wayland : Buffer

			struct SurfaceSize;

			struct BufferData
			{
				MemoryPool* memory_pool = nullptr;
				off_t       offset      = 0;
			};

			using MapBufferData = std::unordered_map<struct wl_buffer*, BufferData>;

			// -------------------------------------------------- //

			struct Buffer
			{
				MapBufferData map   = {};
				std::mutex    mutex = {};
			};

			Buffer buffer;

			// -------------------------------------------------- //

			static wl_buffer* bufferCreate(Yetani::SurfaceSize&, Yetani::Window::Memory*, Yetani::Buffer*) noexcept;
			static void       bufferDestroy(struct wl_buffer*&) noexcept;

			// }}}
			// {{{ Wayland : Surface

			struct SurfaceEvent
			{
				Yetani::LambdaSizeMm      on_size_mm_change      = {};
				Yetani::LambdaSizePercent on_size_percent_change = {};
				Yetani::LambdaSizePixel   on_size_pixel_change   = {};
			};

			using MapSurfaceEvent = std::map<struct wl_surface*, Yetani::SurfaceEvent>;

			MapSurfaceEvent surface_event_map;

			// -------------------------------------------------- //

			enum struct SizeUnit
			{	Millimeter
			,	Percent
			,	Pixel
			};

			// This is the data that needs to be locked from resizing
			// Use mainly by XdgSurface related methods
			struct SurfaceExtent
			{
				Yetani::SizeUnit    preferred_unit    = {};
				Yetani::SizeMm      preferred_mm      = {};
				Yetani::SizePercent preferred_percent = {};
				Yetani::SizeMm      size_mm           = {};
				Yetani::SizePercent size_percent      = {};
				Yetani::SizePixel   size_pixel        = {};
				Yetani::SizePixel   size_pixel_max    = {};
				Yetani::SizePixel   size_pixel_min    = {};
			};

			using MapSurfaceExtent = std::unordered_map<struct wl_surface*, Yetani::SurfaceExtent>;

			MapSurfaceExtent surface_extent_map;
			std::mutex       surface_extent_mutex; // For surface adding and removing

			// -------------------------------------------------- //

			struct SurfaceFrame
			{
				struct wl_surface*      wl_surface  = nullptr;
				std::atomic<wl_buffer*> buffer_next = {};
				uint32_t                width       = 0;
				uint32_t                height      = 0;
				uint32_t                time_ms     = 0;
			};

			using MapSurfaceFrame = std::unordered_map<struct wl_surface*, Yetani::SurfaceFrame>;

			MapSurfaceFrame surface_frame_map;

			// -------------------------------------------------- //

			struct SurfaceSize
			{
				int32_t       width;
				int32_t       height;
				int32_t       stride;
				uint32_t      in_bytes;
				wl_shm_format pixel_format;
				uint8_t       bytes_per_pixel;
			};

			using MapSurfaceSize = std::unordered_map<struct wl_surface*, Yetani::SurfaceSize>;

			MapSurfaceSize surface_size_map;

			// -------------------------------------------------- //

			using MapSurfaceResizeMutex = std::unordered_map<struct wl_surface*, std::mutex>;

			MapSurfaceResizeMutex surface_resize_mutex_map;

			// -------------------------------------------------- //

			static void               surfaceCalculateSize(Yetani*, struct wl_surface*, const Yetani::SizePixel&) noexcept;
			static struct wl_surface* surfaceCreate(Yetani*, const wl_shm_format, const Yetani::SizePixel&, Yetani::Window::Memory&) noexcept;
			static void               surfaceDestroy(Yetani*, struct wl_surface*&) noexcept;

			// }}}
			// {{{ Window

			struct WindowData
			{
				Yetani*             yetani;
				struct wl_shm*      wl_shm;
				struct wl_output*   wl_output;
				std::string         file_name;
				Yetani::SizeMm      size_mm;
				Yetani::SizePercent size_percent;
				Yetani::SizePixel   size_pixel;
				Yetani::SizeUnit    size_unit;
				wl_shm_format       pixel_format;
				std::error_code     error;
			};

			// -------------------------------------------------- //

			Yetani::Window* windowCreate(const Yetani::SizeUnit, const Yetani::SizeMm&, const Yetani::SizePercent&, const Yetani::SizePixel&, const wl_shm_format, std::error_code&) noexcept;
			void            windowDataInit(Yetani::WindowData&) noexcept;
			void            windowDataInitOutput(Yetani::WindowData&) noexcept;
			void            windowInitMemory(Yetani::WindowData&, Yetani::Window::Memory&) noexcept;
			void            windowInitOutput(Yetani::WindowData&, struct wl_surface*) noexcept;
			void            windowEraseMemory(Yetani::Window::Memory&) noexcept;
			void            windowEraseOutput(struct wl_surface*) noexcept;
			void            windowEraseSurfaceExtent(struct wl_surface*) noexcept;

			// }}}
			// {{{ Utility

			std::vector<Yetani::Window*> window_vector;
			std::mutex                   window_vector_mutex;

			// -------------------------------------------------- //

			void windowAdd(Yetani::Window*) noexcept;
			void windowRemove(Yetani::Window*) noexcept;

			// }}}
			// {{{ XDG

			enum XdgState : int32_t
			{	Unknown                    = 0
			,	Toplevel_Active            = 1
			,	Toplevel_Attach_Buffer     = 2
			,	Toplevel_Resizing          = 3
			,	Toplevel_Window_Fullscreen = 4
			,	Toplevel_Window_Maximized  = 5
			,	Toplevel_Window_Normal     = 6
			,	Toplevel_Decoration        = 7
			};

			using VectorXdgStateChange = std::vector<int32_t>;
			using MapXdgStateChange    = std::unordered_map<struct xdg_surface*, Yetani::VectorXdgStateChange>;

			Yetani::MapXdgStateChange xdg_state_change_map;
			std::mutex                xdg_state_change_mutex;

			// -------------------------------------------------- //

			struct xdg_wm_base* xdg_wm_base;

			// -------------------------------------------------- //

			static Yetani::WindowMode toWindowMode(const Yetani::XdgState) noexcept;
			static Yetani::XdgState   toXdgState(const Yetani::WindowMode) noexcept;

			// }}}
			// {{{ XDG : Surface

			struct XdgSurface
			{
				Yetani*            yetani     = nullptr;
				struct wl_surface* wl_surface = nullptr;
			};

			using MapXdgSurface = std::unordered_map<struct wl_surface*, Yetani::XdgSurface>;

			Yetani::MapXdgSurface xdg_surface_map;

			// -------------------------------------------------- //

			struct xdg_surface* xdgSurfaceCreate(struct wl_surface*) noexcept;
			void                xdgSurfaceDestroy(struct wl_surface*, struct xdg_surface*&) noexcept;
			void                xdgSurfaceSetExtent(struct wl_surface*, const Yetani::SizeUnit&, const Yetani::SizeMm&, const Yetani::SizePercent&, const Yetani::SizePixel&) noexcept;

			// }}}
			// {{{ XDG : Toplevel

			struct XdgToplevel
			{
				Yetani::VectorXdgStateChange* state_change         = nullptr;
				Yetani::Lambda                close_request_lambda = {};
				Yetani::LambdaBool            is_active_lambda     = {};
				bool                          is_active            = false;
				Yetani::XdgState              window_state         = XdgState::Unknown;
				Yetani::LambdaWindowMode      window_state_lambda  = {};
				Yetani::SizePixel             previous_size        = {};
				struct xdg_toplevel*          xdg_toplevel         = nullptr;
			};

			using MapXdgToplevel = std::unordered_map<struct xdg_surface*, Yetani::XdgToplevel>;

			MapXdgToplevel xdg_toplevel_map;

			// -------------------------------------------------- //

			struct xdg_toplevel* xdgToplevelCreate(struct xdg_surface*) noexcept;
			void                 xdgToplevelDestroy(struct xdg_surface*, struct xdg_toplevel*&) noexcept;
			static void          xdgToplevelSizeChange(Yetani*, struct wl_surface*, const Yetani::SizePixel&) noexcept;
			static void          xdgToplevelSizeMinMaxChange(Yetani*, struct xdg_toplevel*, struct wl_surface*, const Yetani::SizePixel&, const Yetani::SizePixel&) noexcept;
			static void          xdgToplevelWindowChange(Yetani*, struct wl_surface*, Yetani::XdgToplevel&, const Yetani::XdgState, const Yetani::SizePixel&) noexcept;

			// }}}
			// {{{ XDG : Decoration Manager (Unstable)

			struct XdgDecoration
			{
				Yetani::VectorXdgStateChange*   state_change = nullptr;
				Yetani::LambdaWindowDecorations lambda       = {};
				uint32_t                        state        = 0;
				bool                            is_present   = false;
				//struct zxdg_toplevel_decoration_v1* xdg_decoration;
			};

			using MapXdgDecoration = std::unordered_map<struct xdg_surface*, Yetani::XdgDecoration>;

			MapXdgDecoration xdg_decoration_map;

			// -------------------------------------------------- //

			struct zxdg_decoration_manager_v1* decoration_manager;

			// -------------------------------------------------- //

			struct zxdg_toplevel_decoration_v1* xdgDecorationCreate(struct xdg_surface*, struct xdg_toplevel*) noexcept;
			void                                xdgDecorationDestroy(struct xdg_surface*, struct xdg_toplevel*, struct zxdg_toplevel_decoration_v1*&) noexcept;
			static void                         xdgDecorationChange(Yetani::XdgDecoration&, const uint32_t) noexcept;

			// }}}
			// {{{ Listener Handlers : Wayland

			static struct wl_buffer_listener   buffer_listener;
			static struct wl_callback_listener frame_callback_listener;
			static struct wl_keyboard_listener keyboard_listener;
			static struct wl_output_listener   output_listener;
			static struct wl_pointer_listener  pointer_listener;
			static struct wl_registry_listener registry_listener;
			static struct wl_seat_listener     seat_listener;
			static struct wl_shm_listener      shm_listener;
			static struct wl_surface_listener  surface_listener;

			// -------------------------------------------------- //

			static void handlerBufferRelease(void*, struct wl_buffer*) noexcept;

			static void handlerKeyboardEnter(void*, struct wl_keyboard*, uint32_t, struct wl_surface*, struct wl_array*) noexcept;
			static void handlerKeyboardKey(void*, struct wl_keyboard*, uint32_t, uint32_t, uint32_t, uint32_t) noexcept;
			static void handlerKeyboardKeymap(void*, struct wl_keyboard*, uint32_t, int32_t, uint32_t) noexcept;
			static void handlerKeyboardLeave(void*, struct wl_keyboard*, uint32_t, struct wl_surface*) noexcept;
			static void handlerKeyboardModifiers(void*, struct wl_keyboard*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) noexcept;
			static void handlerKeyboardRepeatInfo(void*, struct wl_keyboard*, int32_t, int32_t) noexcept;

			static void handlerOutputDone(void*, struct wl_output*) noexcept;
			static void handlerOutputGeometry(void*, struct wl_output*, int32_t, int32_t, int32_t, int32_t, int32_t, const char*, const char*, int32_t) noexcept;
			static void handlerOutputMode(void*, struct wl_output*, uint32_t, int32_t, int32_t, int32_t) noexcept;
			static void handlerOutputScale(void*, struct wl_output*, int32_t) noexcept;
			static void handlerPointerAxis(void*, struct wl_pointer*, uint32_t, uint32_t, wl_fixed_t) noexcept;
			static void handlerPointerAxisDiscrete(void*, struct wl_pointer*, uint32_t, int32_t) noexcept;
			static void handlerPointerAxisSource(void*, struct wl_pointer*, uint32_t) noexcept;
			static void handlerPointerAxisStop(void*, struct wl_pointer*, uint32_t, uint32_t) noexcept;
			static void handlerPointerButton(void*, struct wl_pointer*, uint32_t, uint32_t, uint32_t, uint32_t) noexcept;
			static void handlerPointerEnter(void*, struct wl_pointer*, uint32_t, struct wl_surface*, wl_fixed_t, wl_fixed_t) noexcept;
			static void handlerPointerFrame(void*, struct wl_pointer*) noexcept;
			static void handlerPointerLeave(void*, struct wl_pointer*, uint32_t, struct wl_surface*) noexcept;
			static void handlerPointerMotion(void*, struct wl_pointer*, uint32_t, wl_fixed_t, wl_fixed_t) noexcept;
			static void handlerRegistryGlobal(void*, struct wl_registry*, uint32_t, const char*, uint32_t) noexcept;
			static void handlerRegistryRemove(void*, struct wl_registry*, uint32_t) noexcept;
			static void handlerSeatCapabilities(void*, struct wl_seat*, uint32_t) noexcept;
			static void handlerSeatName(void*, struct wl_seat*, const char*) noexcept;
			static void handlerShmFormat(void*, struct wl_shm*, uint32_t) noexcept;
			static void handlerSurfaceEnter(void*, struct wl_surface*, struct wl_output*) noexcept;
			static void handlerSurfaceLeave(void*, struct wl_surface*, struct wl_output*) noexcept;
			static void handlerSwapBuffers(void*, struct wl_callback*, uint32_t) noexcept;

			// }}}
			// {{{ Listener Handlers : Wayland Unstable
			// }}}
			// {{{ Listener Handlers : XDG

			static struct xdg_wm_base_listener  xdg_wm_base_listener;
			static struct xdg_surface_listener  xdg_surface_listener;
			static struct xdg_toplevel_listener xdg_toplevel_listener;

			// -------------------------------------------------- //

			static void handlerXdgSurfaceConfigure(void*, struct xdg_surface*, uint32_t) noexcept;
			static void handlerXdgToplevelClose(void*, struct xdg_toplevel*) noexcept;
			static void handlerXdgToplevelConfigure(void*, struct xdg_toplevel*, int32_t, int32_t, struct wl_array*) noexcept;
			static void handlerXdgWmBasePing(void*, struct xdg_wm_base*, uint32_t) noexcept;

			// }}}
			// {{{ Listener Handlers : XDG Unstable

			static struct zxdg_toplevel_decoration_v1_listener xdg_toplevel_decoration_listener;

			// -------------------------------------------------- //

			static void handlerXdgToplevelDecorationConfigure(void*, struct zxdg_toplevel_decoration_v1*, uint32_t mode) noexcept;

			// }}}

			Yetani(const Yetani&) = delete;
			Yetani& operator=(const Yetani&) = delete;
	}; // class Yetani

	// }}}
	// {{{ Convenience

	std::string to_string(const wl_shm_format&) noexcept;
	std::string to_string(const std::error_code&) noexcept;
	std::string to_string(const Yetani::Key&) noexcept;
	std::string to_string(const Yetani::KeyModifier&) noexcept;
	std::string to_string(const Yetani::KeyState&) noexcept;
	std::string to_string(const Yetani::Output&) noexcept;
	std::string to_string(const Yetani::PointMm&) noexcept;
	std::string to_string(const Yetani::PointPercent&) noexcept;
	std::string to_string(const Yetani::PointPixel&) noexcept;
	std::string to_string(const Yetani::PointerAxis&) noexcept;
	std::string to_string(const Yetani::PointerAxisSource&) noexcept;
	std::string to_string(const Yetani::PointerAxisType&) noexcept;
	std::string to_string(const Yetani::PointerButtonState&) noexcept;
	std::string to_string(const Yetani::WindowMode&) noexcept;

	// }}}
}

// {{{ Implementation

#ifdef ZAKERO_YETANI_IMPLEMENTATION

// {{{ Macros

// {{{ Macros : Doxygen

#ifdef ZAKERO__DOXYGEN_DEFINE_DOCS

// Only used for generating Doxygen documentation

/**
 * \brief Activate the implementation code.
 *
 * Defining this macro will cause the _Zakero Yetani_ implementation to be 
 * included.  This should only be done once, since compiler and/or linker 
 * errors will typically be generated if more than a single implementation is 
 * found.
 *
 * \note It does not matter if the macro is given a value or not, only its 
 * existence is checked.
 */
#define ZAKERO_YETANI_IMPLEMENTATION

/**
 * \brief Enable debugging output
 *
 * Enabled debugging messages to be written.
 *
 * \internal
 * These messages are written by using `ZAKERO_YETANI__DEBUG` macros.
 */
#define ZAKERO_YETANI_ENABLE_DEBUG

/**
 * \brief Add extra error checking.
 *
 * Before a method executes, it should validate the argument values that it 
 * receives.  This is useful while developing code but adds overhead.  If the 
 * calling code can guarantee that all argument data is valid, this macro can 
 * be enabled to reduce the validation checks and overhead.
 */
#define ZAKERO_YETANI_ENABLE_SAFE_MODE

#endif // ZAKERO__DOXYGEN_DEFINE_DOCS

// }}}

/**
 * \internal
 * 
 * \def ZAKERO_YETANI__DEBUG_DISABLED
 *
 * \brief A disabled flag
 *
 * The inverse of ZAKERO_YETANI_DEBUG_ENABLED and is used to enable or disable 
 * the debugging messages.
 *
 * If `ZAKERO_YETANI_ENABLE_DEBUG` is `true` then this flag is `false`.
 * If `ZAKERO_YETANI_ENABLE_DEBUG` is `false` then this flag is `true`.
 *
 * The purpose of this is to make other macros easier to read.
 */
#ifdef ZAKERO_YETANI_ENABLE_DEBUG
#define ZAKERO_YETANI__DEBUG_DISABLED false
#else
#define ZAKERO_YETANI__DEBUG_DISABLED true
#endif


/**
 * \def ZAKERO_YETANI_ENABLE_DEBUG_STREAM
 *
 * \brief The stream to use for debugging output.
 *
 * This macro holds the stream that will be written to for the debug messages.  
 * If this macro is undefined at compile time, then `std::cerr` will be used.
 *
 * \parcode
 * #define ZAKERO_YETANI_ENABLE_DEBUG_STREAM MyLogger::errorLogStream()
 * \endparcode
 */
#ifndef ZAKERO_YETANI_ENABLE_DEBUG_STREAM
#define ZAKERO_YETANI_ENABLE_DEBUG_STREAM std::cerr
#endif


/**
 * \internal
 *
 * \brief Write debugging message.
 *
 * These macros are available to help make debugging easier.
 *
 * If the `ZAKERO_YETANI_ENABLE_DEBUG` macro is enabled, this macro will write 
 * to the stream defined by ZAKERO_YETANI_ENABLE_DEBUG_STREAM.
 *
 * \parcode
 * ZAKERO_YETANI__DEBUG << "Insert debugging message here.\n";
 * \endparcode
 *
 * \todo Use std::source_location instead of macros once that feature is no 
 * longer experimental.
 */
#define ZAKERO_YETANI__DEBUG                   \
	if(ZAKERO_YETANI__DEBUG_DISABLED) {}   \
	else ZAKERO_YETANI_ENABLE_DEBUG_STREAM \
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
 * Does the samething as `ZAKERO_YETANI__DEBUG` but is specialized to work with 
 * std::ostream compatible objects.  Casting may be required to get the desired 
 * output.
 *
 * \parcode
 * uint64_t foo = 42;
 * ZAKERO_YETANI__DEBUG_VAR(foo);
 * \endparcode
 *
 * \param var_ The variable to write.
 */
#define ZAKERO_YETANI__DEBUG_VAR(var_)   \
	ZAKERO_YETANI__DEBUG             \
		<< #var_ << ": " << var_ \
		<< "\n";

/**
 * \internal
 *
 * \brief Write debugging message.
 *
 * Does the samething as `ZAKERO_YETANI__DEBUG` but is specialized to work with 
 * boolean values.
 *
 * \parcode
 * bool foo = true
 * ZAKERO_YETANI__DEBUG_BOOL(foo);
 * \endparcode
 *
 * \param var_ The variable to write.
 */
#define ZAKERO_YETANI__DEBUG_BOOL(var_)                    \
	ZAKERO_YETANI__DEBUG                               \
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
#define ZAKERO_YETANI__ERROR(err_) std::error_code(err_, YetaniErrorCategory)


/**
 * \internal
 *
 * \brief All Wayland supported shared memory formats.
 *
 * An X-Macro table of pixel formats. The columns are:
 * -# __Wayland Enum__<br>
 *    The Wayland enum name.
 *    <br><br>
 * -# __Bytes Per Pixel__<br>
 *    The number of bytes needed for each pixel.
 *    <br><br>
 * -# __Description__<br>
 *    The name of the format, and a break down of how the bits are used (if 
 *    known).
 */
#define ZAKERO_YETANI__SHM_FORMAT \
	X(WL_SHM_FORMAT_ARGB8888        , 4 , "32-bit ARGB format, [31:0] A:R:G:B 8:8:8:8 little endian"                                          ) \
	X(WL_SHM_FORMAT_XRGB8888        , 4 , "32-bit RGB format, [31:0] x:R:G:B 8:8:8:8 little endian"                                           ) \
	X(WL_SHM_FORMAT_C8              , 1 , "8-bit color index format, [7:0] C"                                                                 ) \
	X(WL_SHM_FORMAT_RGB332          , 1 , "8-bit RGB format, [7:0] R:G:B 3:3:2"                                                               ) \
	X(WL_SHM_FORMAT_BGR233          , 1 , "8-bit BGR format, [7:0] B:G:R 2:3:3"                                                               ) \
	X(WL_SHM_FORMAT_XRGB4444        , 2 , "16-bit xRGB format, [15:0] x:R:G:B 4:4:4:4 little endian"                                          ) \
	X(WL_SHM_FORMAT_XBGR4444        , 2 , "16-bit xBGR format, [15:0] x:B:G:R 4:4:4:4 little endian"                                          ) \
	X(WL_SHM_FORMAT_RGBX4444        , 2 , "16-bit RGBx format, [15:0] R:G:B:x 4:4:4:4 little endian"                                          ) \
	X(WL_SHM_FORMAT_BGRX4444        , 2 , "16-bit BGRx format, [15:0] B:G:R:x 4:4:4:4 little endian"                                          ) \
	X(WL_SHM_FORMAT_ARGB4444        , 2 , "16-bit ARGB format, [15:0] A:R:G:B 4:4:4:4 little endian"                                          ) \
	X(WL_SHM_FORMAT_ABGR4444        , 2 , "16-bit ABGR format, [15:0] A:B:G:R 4:4:4:4 little endian"                                          ) \
	X(WL_SHM_FORMAT_RGBA4444        , 2 , "16-bit RBGA format, [15:0] R:G:B:A 4:4:4:4 little endian"                                          ) \
	X(WL_SHM_FORMAT_BGRA4444        , 2 , "16-bit BGRA format, [15:0] B:G:R:A 4:4:4:4 little endian"                                          ) \
	X(WL_SHM_FORMAT_XRGB1555        , 2 , "16-bit xRGB format, [15:0] x:R:G:B 1:5:5:5 little endian"                                          ) \
	X(WL_SHM_FORMAT_XBGR1555        , 2 , "16-bit xBGR 1555 format, [15:0] x:B:G:R 1:5:5:5 little endian"                                     ) \
	X(WL_SHM_FORMAT_RGBX5551        , 2 , "16-bit RGBx 5551 format, [15:0] R:G:B:x 5:5:5:1 little endian"                                     ) \
	X(WL_SHM_FORMAT_BGRX5551        , 2 , "16-bit BGRx 5551 format, [15:0] B:G:R:x 5:5:5:1 little endian"                                     ) \
	X(WL_SHM_FORMAT_ARGB1555        , 2 , "16-bit ARGB 1555 format, [15:0] A:R:G:B 1:5:5:5 little endian"                                     ) \
	X(WL_SHM_FORMAT_ABGR1555        , 2 , "16-bit ABGR 1555 format, [15:0] A:B:G:R 1:5:5:5 little endian"                                     ) \
	X(WL_SHM_FORMAT_RGBA5551        , 2 , "16-bit RGBA 5551 format, [15:0] R:G:B:A 5:5:5:1 little endian"                                     ) \
	X(WL_SHM_FORMAT_BGRA5551        , 2 , "16-bit BGRA 5551 format, [15:0] B:G:R:A 5:5:5:1 little endian"                                     ) \
	X(WL_SHM_FORMAT_RGB565          , 2 , "16-bit RGB 565 format, [15:0] R:G:B 5:6:5 little endian"                                           ) \
	X(WL_SHM_FORMAT_BGR565          , 2 , "16-bit BGR 565 format, [15:0] B:G:R 5:6:5 little endian"                                           ) \
	X(WL_SHM_FORMAT_RGB888          , 3 , "24-bit RGB format, [23:0] R:G:B little endian"                                                     ) \
	X(WL_SHM_FORMAT_BGR888          , 3 , "24-bit BGR format, [23:0] B:G:R little endian"                                                     ) \
	X(WL_SHM_FORMAT_XBGR8888        , 4 , "32-bit xBGR format, [31:0] x:B:G:R 8:8:8:8 little endian"                                          ) \
	X(WL_SHM_FORMAT_RGBX8888        , 4 , "32-bit RGBx format, [31:0] R:G:B:x 8:8:8:8 little endian"                                          ) \
	X(WL_SHM_FORMAT_BGRX8888        , 4 , "32-bit BGRx format, [31:0] B:G:R:x 8:8:8:8 little endian"                                          ) \
	X(WL_SHM_FORMAT_ABGR8888        , 4 , "32-bit ABGR format, [31:0] A:B:G:R 8:8:8:8 little endian"                                          ) \
	X(WL_SHM_FORMAT_RGBA8888        , 4 , "32-bit RGBA format, [31:0] R:G:B:A 8:8:8:8 little endian"                                          ) \
	X(WL_SHM_FORMAT_BGRA8888        , 4 , "32-bit BGRA format, [31:0] B:G:R:A 8:8:8:8 little endian"                                          ) \
	X(WL_SHM_FORMAT_XRGB2101010     , 4 , "32-bit xRGB format, [31:0] x:R:G:B 2:10:10:10 little endian"                                       ) \
	X(WL_SHM_FORMAT_XBGR2101010     , 4 , "32-bit xBGR format, [31:0] x:B:G:R 2:10:10:10 little endian"                                       ) \
	X(WL_SHM_FORMAT_RGBX1010102     , 4 , "32-bit RGBx format, [31:0] R:G:B:x 10:10:10:2 little endian"                                       ) \
	X(WL_SHM_FORMAT_BGRX1010102     , 4 , "32-bit BGRx format, [31:0] B:G:R:x 10:10:10:2 little endian"                                       ) \
	X(WL_SHM_FORMAT_ARGB2101010     , 4 , "32-bit ARGB format, [31:0] A:R:G:B 2:10:10:10 little endian"                                       ) \
	X(WL_SHM_FORMAT_ABGR2101010     , 4 , "32-bit ABGR format, [31:0] A:B:G:R 2:10:10:10 little endian"                                       ) \
	X(WL_SHM_FORMAT_RGBA1010102     , 4 , "32-bit RGBA format, [31:0] R:G:B:A 10:10:10:2 little endian"                                       ) \
	X(WL_SHM_FORMAT_BGRA1010102     , 4 , "32-bit BGRA format, [31:0] B:G:R:A 10:10:10:2 little endian"                                       ) \
	X(WL_SHM_FORMAT_YUYV            , 4 , "packed YCbCr format, [31:0] Cr0:Y1:Cb0:Y0 8:8:8:8 little endian"                                   ) \
	X(WL_SHM_FORMAT_YVYU            , 4 , "packed YCbCr format, [31:0] Cb0:Y1:Cr0:Y0 8:8:8:8 little endian"                                   ) \
	X(WL_SHM_FORMAT_UYVY            , 4 , "packed YCbCr format, [31:0] Y1:Cr0:Y0:Cb0 8:8:8:8 little endian"                                   ) \
	X(WL_SHM_FORMAT_VYUY            , 4 , "packed YCbCr format, [31:0] Y1:Cb0:Y0:Cr0 8:8:8:8 little endian"                                   ) \
	X(WL_SHM_FORMAT_AYUV            , 4 , "packed AYCbCr format, [31:0] A:Y:Cb:Cr 8:8:8:8 little endian"                                      ) \
	X(WL_SHM_FORMAT_NV12            , 8 , "2 plane YCbCr Cr:Cb format, 2x2 subsampled Cr:Cb plane"                                            ) \
	X(WL_SHM_FORMAT_NV21            , 8 , "2 plane YCbCr Cb:Cr format, 2x2 subsampled Cb:Cr plane"                                            ) \
	X(WL_SHM_FORMAT_NV16            , 8 , "2 plane YCbCr Cr:Cb format, 2x1 subsampled Cr:Cb plane"                                            ) \
	X(WL_SHM_FORMAT_NV61            , 8 , "2 plane YCbCr Cb:Cr format, 2x1 subsampled Cb:Cr plane"                                            ) \
	X(WL_SHM_FORMAT_YUV410          , 8 , "3 plane YCbCr format, 4x4 subsampled Cb (1) and Cr (2) planes"                                     ) \
	X(WL_SHM_FORMAT_YVU410          , 8 , "3 plane YCbCr format, 4x4 subsampled Cr (1) and Cb (2) planes"                                     ) \
	X(WL_SHM_FORMAT_YUV411          , 8 , "3 plane YCbCr format, 4x1 subsampled Cb (1) and Cr (2) planes"                                     ) \
	X(WL_SHM_FORMAT_YVU411          , 8 , "3 plane YCbCr format, 4x1 subsampled Cr (1) and Cb (2) planes"                                     ) \
	X(WL_SHM_FORMAT_YUV420          , 8 , "3 plane YCbCr format, 2x2 subsampled Cb (1) and Cr (2) planes"                                     ) \
	X(WL_SHM_FORMAT_YVU420          , 8 , "3 plane YCbCr format, 2x2 subsampled Cr (1) and Cb (2) planes"                                     ) \
	X(WL_SHM_FORMAT_YUV422          , 8 , "3 plane YCbCr format, 2x1 subsampled Cb (1) and Cr (2) planes"                                     ) \
	X(WL_SHM_FORMAT_YVU422          , 8 , "3 plane YCbCr format, 2x1 subsampled Cr (1) and Cb (2) planes"                                     ) \
	X(WL_SHM_FORMAT_YUV444          , 8 , "3 plane YCbCr format, non-subsampled Cb (1) and Cr (2) planes"                                     ) \
	X(WL_SHM_FORMAT_YVU444          , 8 , "3 plane YCbCr format, non-subsampled Cr (1) and Cb (2) planes"                                     ) \
	X(WL_SHM_FORMAT_R8              , 1 , "[7:0] R"                                                                                           ) \
	X(WL_SHM_FORMAT_R16             , 2 , "[15:0] R little endian"                                                                            ) \
	X(WL_SHM_FORMAT_RG88            , 2 , "[15:0] R:G 8:8 little endian"                                                                      ) \
	X(WL_SHM_FORMAT_GR88            , 2 , "[15:0] G:R 8:8 little endian"                                                                      ) \
	X(WL_SHM_FORMAT_RG1616          , 4 , "[31:0] R:G 16:16 little endian"                                                                    ) \
	X(WL_SHM_FORMAT_GR1616          , 4 , "[31:0] G:R 16:16 little endian"                                                                    ) \
	X(WL_SHM_FORMAT_XRGB16161616F   , 8 , "[63:0] x:R:G:B 16:16:16:16 little endian"                                                          ) \
	X(WL_SHM_FORMAT_XBGR16161616F   , 8 , "[63:0] x:B:G:R 16:16:16:16 little endian"                                                          ) \
	X(WL_SHM_FORMAT_ARGB16161616F   , 8 , "[63:0] A:R:G:B 16:16:16:16 little endian"                                                          ) \
	X(WL_SHM_FORMAT_ABGR16161616F   , 8 , "[63:0] A:B:G:R 16:16:16:16 little endian"                                                          ) \
	X(WL_SHM_FORMAT_XYUV8888        , 4 , "[31:0] X:Y:Cb:Cr 8:8:8:8 little endian"                                                            ) \
	X(WL_SHM_FORMAT_VUY888          , 3 , "[23:0] Cr:Cb:Y 8:8:8 little endian"                                                                ) \
	X(WL_SHM_FORMAT_VUY101010       , 4 , "Y followed by U then V, 10:10:10. Non-linear modifier only"                                        ) \
	X(WL_SHM_FORMAT_Y210            , 8 , "[63:0] Cr0:0:Y1:0:Cb0:0:Y0:0 10:6:10:6:10:6:10:6 little endian per 2 Y pixels"                     ) \
	X(WL_SHM_FORMAT_Y212            , 8 , "[63:0] Cr0:0:Y1:0:Cb0:0:Y0:0 12:4:12:4:12:4:12:4 little endian per 2 Y pixels"                     ) \
	X(WL_SHM_FORMAT_Y216            , 8 , "[63:0] Cr0:Y1:Cb0:Y0 16:16:16:16 little endian per 2 Y pixels"                                     ) \
	X(WL_SHM_FORMAT_Y410            , 4 , "[31:0] A:Cr:Y:Cb 2:10:10:10 little endian"                                                         ) \
	X(WL_SHM_FORMAT_Y412            , 8 , "[63:0] A:0:Cr:0:Y:0:Cb:0 12:4:12:4:12:4:12:4 little endian"                                        ) \
	X(WL_SHM_FORMAT_Y416            , 8 , "[63:0] A:Cr:Y:Cb 16:16:16:16 little endian"                                                        ) \
	X(WL_SHM_FORMAT_XVYU2101010     , 4 , "[31:0] X:Cr:Y:Cb 2:10:10:10 little endian"                                                         ) \
	X(WL_SHM_FORMAT_XVYU12_16161616 , 8 , "[63:0] X:0:Cr:0:Y:0:Cb:0 12:4:12:4:12:4:12:4 little endian"                                        ) \
	X(WL_SHM_FORMAT_XVYU16161616    , 8 , "[63:0] X:Cr:Y:Cb 16:16:16:16 little endian"                                                        ) \
	X(WL_SHM_FORMAT_Y0L0            , 8 , "[63:0] A3:A2:Y3:0:Cr0:0:Y2:0:A1:A0:Y1:0:Cb0:0:Y0:0  1:1:8:2:8:2:8:2:1:1:8:2:8:2:8:2 little endian" ) \
	X(WL_SHM_FORMAT_X0L0            , 8 , "[63:0] X3:X2:Y3:0:Cr0:0:Y2:0:X1:X0:Y1:0:Cb0:0:Y0:0  1:1:8:2:8:2:8:2:1:1:8:2:8:2:8:2 little endian" ) \
	X(WL_SHM_FORMAT_Y0L2            , 8 , "[63:0] X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10 little endian"                   ) \
	X(WL_SHM_FORMAT_X0L2            , 8 , "[63:0] X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10 little endian"                   ) \
	X(WL_SHM_FORMAT_YUV420_8BIT     , 8 , "[63:0] X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10 little endian"                   ) \
	X(WL_SHM_FORMAT_YUV420_10BIT    , 8 , "[63:0] X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10 little endian"                   ) \
	X(WL_SHM_FORMAT_XRGB8888_A8     , 8 , "[63:0] X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10 little endian"                   ) \
	X(WL_SHM_FORMAT_XBGR8888_A8     , 8 , "[63:0] X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10 little endian"                   ) \
	X(WL_SHM_FORMAT_RGBX8888_A8     , 8 , "[63:0] X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10 little endian"                   ) \
	X(WL_SHM_FORMAT_BGRX8888_A8     , 8 , "[63:0] X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10 little endian"                   ) \
	X(WL_SHM_FORMAT_RGB888_A8       , 8 , "[63:0] X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10 little endian"                   ) \
	X(WL_SHM_FORMAT_BGR888_A8       , 8 , "[63:0] X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10 little endian"                   ) \
	X(WL_SHM_FORMAT_RGB565_A8       , 8 , "[63:0] X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10 little endian"                   ) \
	X(WL_SHM_FORMAT_BGR565_A8       , 8 , "[63:0] X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10 little endian"                   ) \
	X(WL_SHM_FORMAT_NV24            , 0 , "[UNKNOWN SIZE] non-subsampled Cr:Cb plane"                                                         ) \
	X(WL_SHM_FORMAT_NV42            , 0 , "[UNKNOWN SIZE] non-subsampled Cb:Cr plane"                                                         ) \
	X(WL_SHM_FORMAT_P210            , 0 , "[UNKNOWN SIZE] 2x1 subsampled Cr:Cb plane, 10 bits per channel"                                    ) \
	X(WL_SHM_FORMAT_P010            , 0 , "[UNKNOWN SIZE] 2x2 subsampled Cr:Cb plane, 10 bits per channel"                                    ) \
	X(WL_SHM_FORMAT_P012            , 0 , "[UNKNOWN SIZE] 2x2 subsampled Cr:Cb plane, 12 bits per channel"                                    ) \
	X(WL_SHM_FORMAT_P016            , 0 , "[UNKNOWN SIZE] 2x2 subsampled Cr:Cb plane, 16 bits per channel"                                    ) \


/**
 * \internal
 *
 * \brief Convert the enum to a string.
 */
#define ZAKERO_YETANI__OUTPUT_SUBPIXEL \
	X(WL_OUTPUT_SUBPIXEL_UNKNOWN        , "Unkown Subpixel Format" ) \
	X(WL_OUTPUT_SUBPIXEL_NONE           , "No Subpixels"           ) \
	X(WL_OUTPUT_SUBPIXEL_HORIZONTAL_RGB , "Horizontal RGB"         ) \
	X(WL_OUTPUT_SUBPIXEL_HORIZONTAL_BGR , "Horizontal BGR"         ) \
	X(WL_OUTPUT_SUBPIXEL_VERTICAL_RGB   , "Vertical RGB"           ) \
	X(WL_OUTPUT_SUBPIXEL_VERTICAL_BGR   , "Vertical BGR"           ) \


/**
 * \internal
 *
 * \brief Convert the enum to a string.
 */
#define ZAKERO_YETANI__OUTPUT_TRANSFORM \
	X(WL_OUTPUT_TRANSFORM_NORMAL      , "No Transform"                                  ) \
	X(WL_OUTPUT_TRANSFORM_90          , "90 degrees Counter-Clockwise"                  ) \
	X(WL_OUTPUT_TRANSFORM_180         , "180 degrees Counter-Clockwise"                 ) \
	X(WL_OUTPUT_TRANSFORM_270         , "270 degrees Counter-Clockwise"                 ) \
	X(WL_OUTPUT_TRANSFORM_FLIPPED     , "180 degree flip around a vertical axis"        ) \
	X(WL_OUTPUT_TRANSFORM_FLIPPED_90  , "Flig and rotate 90 degrees counter-clockwise"  ) \
	X(WL_OUTPUT_TRANSFORM_FLIPPED_180 , "Flig and rotate 180 degrees counter-clockwise" ) \


/**
 * \internal
 *
 * \brief Iterate over a Wayland Array.
 *
 * C++20 does not allow incrementing "void*".  This means the C macro 
 * wl_array_for_each() is no longer valid C++ code.
 * - Will Wayland fix their macro to be C++ compliant?
 * - Or will Wayland tell C++ developers to eff-off?
 *
 * The official response: Others have worked around it, you can too.
 *
 * \parcode
 * ZAKERO_YETANI__ARRAY_FOR_EACH(uint32_t*, the_iter, some_array)
 * {
 * 	std::cout << "Array value: " << std::to_string(*the_iter) << "\n";
 * }
 * \endparcode
 *
 * \param type_  The array's element data-type
 * \param pos_   The name of the iterator
 * \param array_ The Wayland Array
 */
#define ZAKERO_YETANI__ARRAY_FOR_EACH(type_, pos_, array_)                                   \
	for(type_ pos_ = (type_)(array_)->data                                       \
		; (const char*)pos_ < ((const char*)(array_)->data + (array_)->size) \
		; (pos_)++                                                           \
		)

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

	/**
	 * \brief Error Categories.
	 *
	 * This class implements the std::error_category interface to provide 
	 * consistent access to error code values and messages.
	 *
	 * See https://en.cppreference.com/w/cpp/error/error_category for 
	 * details.
	 */
	class YetaniErrorCategory_
		: public std::error_category
	{
		public:
			constexpr YetaniErrorCategory_() noexcept
			{
			}

			const char* name() const noexcept override
			{
				return "zakero.Yetani";
			}

			std::string message(int condition
				) const noexcept override
			{
				switch(condition)
				{
#define X(name_, val_, mesg_) \
					case val_: return mesg_;
					ZAKERO_YETANI__ERROR_DATA
#undef X
				}

				return "Unknown error condition";
			}
	} YetaniErrorCategory;


	/**
	 * \name Lambda's that do nothing.
	 * \{
	 */
	Yetani::Lambda                  Lambda_DoNothing                  = []() noexcept {};
	Yetani::LambdaKey               LambdaKey_DoNothing               = [](const Yetani::Key&, const Yetani::KeyModifier&) noexcept {};
	Yetani::LambdaAxis              LambdaAxis_DoNothing              = [](const Yetani::PointerAxis&, const Yetani::KeyModifier&) noexcept {};
	Yetani::LambdaButtonMm          LambdaButtonMm_DoNothing          = [](const Yetani::PointerButton&, const Yetani::PointMm&, const Yetani::KeyModifier&) noexcept {};
	Yetani::LambdaButtonPercent     LambdaButtonPercent_DoNothing     = [](const Yetani::PointerButton&, const Yetani::PointPercent&, const Yetani::KeyModifier&) noexcept {};
	Yetani::LambdaButtonPixel       LambdaButtonPixel_DoNothing       = [](const Yetani::PointerButton&, const Yetani::PointPixel&, const Yetani::KeyModifier&) noexcept {};
	Yetani::LambdaPointMm           LambdaPointMm_DoNothing           = [](const Yetani::PointMm&, const Yetani::KeyModifier&) noexcept {};
	Yetani::LambdaPointPercent      LambdaPointPercent_DoNothing      = [](const Yetani::PointPercent&, const Yetani::KeyModifier&) noexcept {};
	Yetani::LambdaPointPixel        LambdaPointPixel_DoNothing        = [](const Yetani::PointPixel&, const Yetani::KeyModifier&) noexcept {};
	Yetani::LambdaBool              LambdaBool_DoNothing              = [](const bool) noexcept {};
	Yetani::LambdaOutputId          LambdaOutputId_DoNothing          = [](const Yetani::OutputId) noexcept {};
	Yetani::LambdaWindowDecorations LambdaWindowDecorations_DoNothing = [](const Yetani::WindowDecorations) noexcept {};
	Yetani::LambdaWindowMode        LambdaWindowMode_DoNothing        = [](const Yetani::WindowMode) noexcept {};
	Yetani::LambdaSizeMm            LambdaSizeMm_DoNothing            = [](const Yetani::SizeMm&) noexcept {};
	Yetani::LambdaSizePercent       LambdaSizePercent_DoNothing       = [](const Yetani::SizePercent&) noexcept {};
	Yetani::LambdaSizePixel         LambdaSizePixel_DoNothing         = [](const Yetani::SizePixel&) noexcept {};
	/**
	 * \}
	 */


	/**
	 * \brief Calculate memory requirements.
	 *
	 * Using the provided \p size and \p format, determine the amount of 
	 * memory in bytes.
	 *
	 * \return The size in byets.
	 *
	 * \todo Due to the size of the Yetani::shmFormatBytesPerPixel() 
	 * method, this will probably never be inlined.  Instead of calling 
	 * this method, try to cache the bytes-per-pixel value.
	 */
	inline size_t sizeInBytes(const Yetani::SizePixel& size   ///< The size
		, const wl_shm_format                      format ///< The pixel format
		) noexcept
	{
		return size.width
			* size.height
			* Yetani::shmFormatBytesPerPixel(format)
			;
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
			return ZAKERO_YETANI__ERROR(Yetani::Error_Window_Size_Too_Small);
		}

		if((min.width > 0)
			&& (max.width > 0)
			&& (min.width > max.width)
			)
		{
			return ZAKERO_YETANI__ERROR(Yetani::Error_Minimum_Size_Greater_Than_Maximum_Size);
		}

		if((min.height > 0)
			&& (max.height > 0)
			&& (min.height > max.height)
			)
		{
			return ZAKERO_YETANI__ERROR(Yetani::Error_Minimum_Size_Greater_Than_Maximum_Size);
		}

		return ZAKERO_YETANI__ERROR(Yetani::Error_None);
	}
}

// }}}
// {{{ Documentation

/**
 * \class Yetani
 *
 * \brief A wrapper class for Wayland.
 *
 * Refer to Zakero_Yetani.h to learn how to include this library.
 *
 * The intent of this class is to make creating programs that interface 
 * directly with Wayland easier.  While there are toolkits out there that do 
 * this (GTK, Qt, SDL, etc.), they also come with a lot of overhead.  This 
 * class will abstract much of Wayland and provide a simple event driven 
 * interface.
 *
 * As soon as a connection to the Wayland server has been established, a thread 
 * will be created to move all the communications to the background.  This 
 * means that there is no "main loop" in the Yetani object to hook into.  The 
 * benefit to this is that Window contents/graphics will not block event 
 * handling.
 *
 * Refer to \refhow{zakero_yetani} for a simple example.
 *
 * \par %Window
 * \parblock
 * Using Yetani to connect to a Wayland Compositor is just the first step.  The 
 * next step is to create a Window that can display _something_.  This is done 
 * using one of the %windowCreate() methods.  There are 3 sets of methods:
 * - Millimeters
 *   - windowCreate(const Yetani::SizeMm&, std::error_code&)
 *   - windowCreate(const Yetani::SizeMm&, const wl_shm_format)
 *   - windowCreate(const Yetani::SizeMm&, const wl_shm_format, std::error_code&)
 * - Percent
 *   - windowCreate(const Yetani::SizePercent&, std::error_code&)
 *   - windowCreate(const Yetani::SizePercent&, const wl_shm_format)
 *   - windowCreate(const Yetani::SizePercent&, const wl_shm_format, std::error_code&)
 * - Pixels
 *   - windowCreate(const Yetani::SizePixel&, std::error_code&)
 *   - windowCreate(const Yetani::SizePixel&, const wl_shm_format)
 *   - windowCreate(const Yetani::SizePixel&, const wl_shm_format, std::error_code&)
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
 * resolutions?  That is why Yetani has the SizePercent object.
 *
 * What about millimeters?  Using millimeters gives the developer more 
 * confidence that size of an area will be physically consistent across 
 * devices.  To illustrate this consider defining a 300x150 button.  This 
 * button may be easily clickable on most monitors but may be too small on a 4k 
 * display.  Defining a button to be 20mm x 10mm, it will be the same physical 
 * size on all monitors.
 *
 * What's wrong with [DPI](https://en.wikipedia.org/wiki/Dots_per_inch)?  DPI 
 * has become a mess in the computer world and is resolution, not a size.
 * \endparblock
 *
 * \par Thread (not) Safe
 * \parblock
 * The main Wayland event loop runs in a dedicated thread.  Because of this, 
 * there are race-conditions where execution uses the same data.  The most 
 * likely thread conflict is the resizing of a surface:
 * - User Thread: Calling one of the window's "size" methods
 * - Wayland Thread: Maximizing/Fullscreen/resizing the window
 * .
 * While the above is mostly protected via mutexs' there is still a chance.
 *
 * There are other potential problems such as the User Thread deleting a window 
 * while the Wayland Thread is resizing the same window.  To resolve this issue 
 * will require a complete evaluation of data and thread activities.
 *
 * Or take the easy way out and rewrite Yetani to be single-threaded...
 * \endparblock
 *
 * \todo XDGOutput Support
 * \todo Client-Side Border
 * \todo Mouse-Look Support
 * \todo Touch Input Support
 * \todo Clipboard Support
 * \todo cursorAliasAdd() (maybe)
 * \todo cursorAliasRemove() (maybe)
 * \todo cursorAvailableNames() (maybe)
 * \todo cursorXDGLoad() (maybe)
 * \todo Use ZAKERO_DELETE() where it makes sense
 *
 * \internal
 *
 * If a struct contains a mutex, that mutex should be locked before interacting 
 * with the contents of the struct.
 *
 * \todo Are Wayland sizes signed or unsigned int's? It might be better to use 
 * uint32_t regardless.  However this will make the conversion code more 
 * complicated (Point and Size can no longer use the same methods).
 *
 * \todo Window::imageNext() always returns Error_None.  Consider changing the 
 * API.  Also, the API may change any way to fix the Maximized/Fullscreen 
 * update lag.
 *
 * \todo Window::imageNext() currently always creates a new wl_buffer.  While 
 * this process is fast, it could be even faster if the wl_buffer was reused.  
 * Try to make this happen.
 *
 * \todo SurfaceFrame should be renamed to RenderFrame since it is only used 
 * for rendering.  Window::imageNext() needs to set the following as an atomic 
 * operation:
 * - The new wl_buffer
 * - The buffer width
 * - The buffer height
 * .
 * handleSwapBuffers() will then atomically copy that data to render the next 
 * buffer.  (Basically, the same as the current process.)
 *
 * \todo handleSwapBuffers() should use the wl_buffer size as the damage area 
 * and not the surface size.  This will fix the most glaring/obvious resizing 
 * artifacts.
 *
 * \todo bufferCreate() should return the memory pool offset.  This will 
 * removed the need for additional map lookups.
 *
 * \todo Using the Wayland render events does not provide a consistent frame 
 * rate.  Move the rendering directly into the eventLoop.
 *
 * \todo Make the API consistent:
 * - Window::onFocusChange() : Lambda handles both "is active" and "not active"
 * - Window::keyboardOn{Enter|Leave}() : One lambda for each state
 * - Window::pointerOn{Enter|Leave}() : One lambda for each state
 * - May need to revisit the Window::pointerOnAxis*() methods
 *
 * \todo Size_Max: This is only used for animated cursors and this use does not 
 * make sense.  If the time per frame is a uint32_t, then allow the full value 
 * to be used and don't limit it to Size_Max.
 * - This may require the cursor timer to be changed to 64-bit variable.
 * - Remove Size_Max if it is no longer needed/used.
 *
 * \todo [Maybe] Add XCursor support.  Might need to do a full source port.
 * - https://www.x.org/releases/X11R7.5/doc/man/man3/Xcursor.3.html
 * - https://www.freedesktop.org/wiki/Specifications/cursor-spec/
 *
 * \bug What if Yetani is started with the CapsLock and/or NumLock in the "on" 
 * state? Will the Key events still be accurate?
 *
 * \todo What happens if another key is pressed while the CapsLock key is 
 * pressed?  Are the "pressed" flags cleared?
 */

/**
 * \enum Yetani::KeyState
 *
 * \brief %Keyboard key state
 *
 * When using a keyboard, a key will emit one of these states.
 */
/* Disabled because Doxygen does not support "enum classes"
 *
 * \var Yetani::Released
 * \brief The key was released
 *
 * \var Yetani::Pressed
 * \brief The key was pressed
 *
 * \var Yetani::Repeat
 * \brief The key is being held down
 */

/**
 * \struct Yetani::Key
 *
 * \brief %Key event information.
 *
 * Time is based on the "steady clock" and not system time.
 *
 * \var Yetani::Key::time
 * \brief When the key event happened.
 *
 * \var Yetani::Key::code
 * \brief The key code of the event.
 *
 * \var Yetani::Key::state
 * \brief The state of the key.
 */

/**
 * \var Yetani::KeyModifier_Alt
 * \brief %Key Modifier flag.
 *
 * \var Yetani::KeyModifier_CapsLock
 * \brief %Key Modifier flag.
 *
 * \var Yetani::KeyModifier_Control
 * \brief %Key Modifier flag.
 *
 * \var Yetani::KeyModifier_Meta
 * \brief %Key Modifier flag.
 *
 * \var Yetani::KeyModifier_Shift
 * \brief %Key Modifier flag.
 */

/**
 * \struct Yetani::KeyModifier
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
 * \var Yetani::KeyModifier::pressed
 * \brief A collection of pressed modifiers.
 *
 * \var Yetani::KeyModifier::latched
 * \brief A collection of latched modifiers.
 *
 * \var Yetani::KeyModifier::locked
 * \brief A collection of locked modifiers.
 *
 * \var Yetani::KeyModifier::group
 * \brief The keyboard layout
 */

/**
 * \struct Yetani::PointMm
 *
 * \brief A location that uses millimeters.
 *
 * \var Yetani::PointMm::time
 * \brief Where in time the point is (if > 0).
 *
 * \var Yetani::PointMm::x
 * \brief Where in the X-Axis the point is.
 *
 * \var Yetani::PointMm::y
 * \brief Where in the Y-Axis the point is.
 */

/**
 * \struct Yetani::PointPercent
 *
 * \brief A location that uses percentages.
 *
 * The percentage range of values is 0.0...1.0, where 0 is the left (or top) 
 * and 1 is the right (or bottom).
 *
 * \var Yetani::PointPercent::time
 * \brief Where in time the point is (if > 0).
 *
 * \var Yetani::PointPercent::x
 * \brief Where in the X-Axis the point is.
 *
 * \var Yetani::PointPercent::y
 * \brief Where in the Y-Axis the point is.
 */

/**
 * \struct Yetani::PointPixel
 *
 * \brief A location that uses pixels.
 *
 * \var Yetani::PointPixel::time
 * \brief Where in time the point is (if > 0).
 *
 * \var Yetani::PointPixel::x
 * \brief Where in the X-Axis the point is.
 *
 * \var Yetani::PointPixel::y
 * \brief Where in the Y-Axis the point is.
 */

/**
 * \enum Yetani::PointerAxisSource
 *
 * \brief Where the axis information came from.
 */
/* Disabled because Doxygen does not support "enum classes"
 *
 * \var Yetani::Unknown
 * \brief Unknown
 *
 * \var Yetani::Continuous
 * \brief Continuous
 *
 * \var Yetani::Finger
 * \brief Finger
 *
 * \var Yetani::Wheel
 * \brief Wheel
 *
 * \var Yetani::Wheel_Tilt
 * \brief Wheel Tilt
 */

/**
 * \enum Yetani::PointerAxisType
 *
 * \brief The direction of the axis movement.
 */
/* Disabled because Doxygen does not support "enum classes"
 *
 * \var Yetani::Unknown
 * \brief Unknown
 *
 * \var Yetani::Horizontal
 * \brief Horizontal
 *
 * \var Yetani::Vertical
 * \brief Vertical
 */

/**
 * \struct Yetani::PointerAxis
 *
 * \brief Information about an Axis event.
 *
 * \var Yetani::PointerAxis::time
 * \brief When the event occurred
 *
 * \var Yetani::PointerAxis::steps
 * \brief The number of rotation steps
 *
 * \var Yetani::PointerAxis::distance
 * \brief The distance traveled
 *
 * \var Yetani::PointerAxis::source
 * \brief The source of the event
 *
 * \var Yetani::PointerAxis::type
 * \brief The type of Axis
 */

/**
 * \enum Yetani::PointerButtonState
 *
 * \brief Mouse button state.
 */
/* Disabled because Doxygen does not support "enum classes"
 *
 * \var Yetani::Released
 * \brief Released
 *
 * \var Yetani::Pressed
 * \brief Pressed
 */

/**
 * \struct Yetani::PointerButton
 *
 * \brief Information about a pointer button event.
 *
 * \var Yetani::PointerButton::code
 * \brief The event code
 *
 * \var Yetani::PointerButton::state
 * \brief The button state
 */

/**
 * \struct Yetani::SizeMm
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
 * \struct Yetani::SizePercent
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
 * \struct Yetani::SizePixel
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
// {{{ Listener Handler : Wayland

struct wl_buffer_listener Yetani::buffer_listener =
{	.release = &Yetani::handlerBufferRelease
};

struct wl_callback_listener Yetani::frame_callback_listener =
{	.done = &Yetani::handlerSwapBuffers
};

struct wl_keyboard_listener Yetani::keyboard_listener =
{	.keymap      = &Yetani::handlerKeyboardKeymap
,	.enter       = &Yetani::handlerKeyboardEnter
,	.leave       = &Yetani::handlerKeyboardLeave
,	.key         = &Yetani::handlerKeyboardKey
,	.modifiers   = &Yetani::handlerKeyboardModifiers
,	.repeat_info = &Yetani::handlerKeyboardRepeatInfo
};

struct wl_output_listener Yetani::output_listener =
{	.geometry = &Yetani::handlerOutputGeometry
,	.mode     = &Yetani::handlerOutputMode
,	.done     = &Yetani::handlerOutputDone
,	.scale    = &Yetani::handlerOutputScale
};

struct wl_pointer_listener Yetani::pointer_listener =
{	.enter         = &Yetani::handlerPointerEnter
,	.leave         = &Yetani::handlerPointerLeave
,	.motion        = &Yetani::handlerPointerMotion
,	.button        = &Yetani::handlerPointerButton
,	.axis          = &Yetani::handlerPointerAxis
,	.frame         = &Yetani::handlerPointerFrame
,	.axis_source   = &Yetani::handlerPointerAxisSource
,	.axis_stop     = &Yetani::handlerPointerAxisStop
,	.axis_discrete = &Yetani::handlerPointerAxisDiscrete
};

struct wl_registry_listener Yetani::registry_listener =
{	.global        = &Yetani::handlerRegistryGlobal
,	.global_remove = &Yetani::handlerRegistryRemove
};

struct wl_seat_listener Yetani::seat_listener =
{	.capabilities = &Yetani::handlerSeatCapabilities
,	.name         = &Yetani::handlerSeatName
};

struct wl_shm_listener Yetani::shm_listener =
{	.format = &Yetani::handlerShmFormat
};

struct wl_surface_listener  Yetani::surface_listener =
{	.enter = &Yetani::handlerSurfaceEnter
,	.leave = &Yetani::handlerSurfaceLeave
};

// }}}
// {{{ Listener Handler : Wayland Unstable
// }}}
// {{{ Listener Handler : XDG

struct xdg_wm_base_listener Yetani::xdg_wm_base_listener =
{	.ping = &Yetani::handlerXdgWmBasePing
};

struct xdg_surface_listener Yetani::xdg_surface_listener =
{	.configure = &Yetani::handlerXdgSurfaceConfigure
};

struct xdg_toplevel_listener Yetani::xdg_toplevel_listener =
{	.configure = &Yetani::handlerXdgToplevelConfigure
,	.close     = &Yetani::handlerXdgToplevelClose
};

// }}}
// {{{ Listener Handler : XDG Unstable

struct zxdg_toplevel_decoration_v1_listener Yetani::xdg_toplevel_decoration_listener =
{	.configure = &Yetani::handlerXdgToplevelDecorationConfigure
};

// }}}
// }}}
// {{{ Constructor / Destructor

/**
 * \brief Constructor.
 *
 * Create a new instance of Yetani.
 */
Yetani::Yetani() noexcept
	: cursor_map()
	, cursor_surface_map()
	, cursor_memory_pool(
		std::string("Zakero.Yetani.")
		+ std::to_string(ZAKERO_STEADY_TIME_NOW(nanoseconds))
		)
	, cursor_mutex()
	, cursor_shm_pool(nullptr)
	, cursor_pointer(nullptr)
	, event_loop()
	, event_loop_is_running(false)
	, compositor(nullptr)
	, display(nullptr)
	, registry(nullptr)
	, shm(nullptr)
	, shm_format_vector()
	, seat_map()
	, id_to_seat()
	, seat(nullptr)
	, keyboard()
	, pointer()
	, output_data()
	, on_output_add(LambdaOutputId_DoNothing)
	, on_output_change(LambdaOutputId_DoNothing)
	, on_output_remove(LambdaOutputId_DoNothing)
	, output_changes_map()
	, output_state_map()
	, output_notify_surface_vector()
	, buffer()
	, surface_event_map()
	, surface_extent_map()
	, surface_extent_mutex()
	, surface_frame_map()
	, surface_size_map()
	, surface_resize_mutex_map()
	, window_vector()
	, window_vector_mutex()
	, xdg_state_change_map()
	, xdg_state_change_mutex()
	, xdg_wm_base(nullptr)
	, xdg_surface_map()
	, xdg_toplevel_map()
	, xdg_decoration_map()
	, decoration_manager(nullptr)
{
}


/**
 * \brief Destructor.
 *
 * Deleting the Yetani object will automatically disconnect from the Wayland 
 * server.
 *
 * Because Yetani is multithreaded, this method will block until all 
 * connections to Wayland have been closed.
 */
Yetani::~Yetani() noexcept
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
 * \brief Establish a connection with the Wayland Compositor.
 *
 * Establish a connection with the Wayland Compositor.  The following values 
 * will be used to determine which Wayland Compositor to connect to:
 * -# __WAYLAND_DISPLAY__<br>
 *    Use the value of this environment variable.
 * -# <b>"wayland-0"</b><br>
 *    This is the default display name used by %Wayland.
 *
 * If a connection was successful, then a pointer to a new Yetani instance will 
 * be returned.  This pointer will need to be `delete`d by the caller.  If 
 * there was an error connecting to the Wayland Compositor, \nullptr will be 
 * returned.
 *
 * \note A connection to the Wayland Compositor must be made before any windows 
 * are created.
 *
 * \return A pointer to a Yetani instance or a \nullptr.
 *
 * \thread_user
 */
Yetani* Yetani::connect() noexcept
{
	std::error_code error; 

	return Yetani::connect("", error);
}


/**
 * \brief Establish a connection with the Wayland Compositor.
 *
 * Establish a connection with the Wayland Compositor.  The following values 
 * will be used to determine which Wayland Compositor to connect to:
 * -# \p __display__<br>
 *    If the value in not empty, this display name will be used.
 * -# __WAYLAND_DISPLAY__<br>
 *    Use the value of this environment variable.
 * -# <b>"wayland-0"</b><br>
 *    This is the default display name used by %Wayland.
 *
 * If a connection was successful, then a pointer to a new Yetani instance will 
 * be returned.  This pointer will need to be `delete`d by the caller.  If 
 * there was an error connecting to the Wayland Compositor, \nullptr will be 
 * returned.
 *
 * \note A connection to the Wayland Compositor must be made before any windows 
 * are created.
 *
 * \return A pointer to a Yetani instance or a \nullptr.
 *
 * \thread_user
 */
Yetani* Yetani::connect(const std::string& display ///< The Display Name or ID
	) noexcept
{
	std::error_code error; 

	return Yetani::connect(display, error);
}


/**
 * \brief Establish a connection with the Wayland Compositor.
 *
 * Establish a connection with the Wayland Compositor.  The following values 
 * will be used to determine which Wayland Compositor to connect to:
 * -# __WAYLAND_DISPLAY__<br>
 *    Use the value of this environment variable.
 * -# <b>"wayland-0"</b><br>
 *    This is the default display name used by %Wayland.
 *
 * If a connection was successful, then a pointer to a new Yetani instance will 
 * be returned.  This pointer will need to be `delete`d by the caller.  If 
 * there was an error connecting to the Wayland Compositor, \nullptr will be 
 * returned.
 *
 * The provided \p error will be set to Yetani::Error_None or the reason for 
 * the connection failure.  An error code of `Yetani::Error_Connection_Failed` 
 * or `Yetani::Error_Invalid_Display_Name` may indicate that there is no 
 * Wayland Compositor available.
 *
 * \note A connection to the Wayland Compositor must be made before any windows 
 * are created.
 *
 * \return A pointer to a Yetani instance or a \nullptr.
 *
 * \thread_user
 */
Yetani* Yetani::connect(std::error_code& error ///< The error code
	) noexcept
{
	return Yetani::connect("", error);
}

/**
 * \brief Establish a connection with the Wayland Compositor.
 *
 * Establish a connection with the Wayland Compositor.  The following values 
 * will be used to determine which Wayland Compositor to connect to:
 * -# \p __display__<br>
 *    If the value in not empty, this display name will be used.
 * -# __WAYLAND_DISPLAY__<br>
 *    Use the value of this environment variable.
 * -# <b>"wayland-0"</b><br>
 *    This is the default display name used by %Wayland.
 *
 * If a connection was successful, then a pointer to a new Yetani instance will 
 * be returned.  This pointer will need to be `delete`d by the caller.  If 
 * there was an error connecting to the Wayland Compositor, \nullptr will be 
 * returned.
 *
 * The provided \p error will be set to Yetani::Error_None or the reason for 
 * the connection failure.  An error code of `Yetani::Error_Connection_Failed` 
 * or `Yetani::Error_Invalid_Display_Name` may indicate that there is no 
 * Wayland Compositor available.
 *
 * \note A connection to the Wayland Compositor must be made before any windows 
 * are created.
 *
 * \return A pointer to a Yetani instance or a \nullptr.
 *
 * \thread_user
 */
Yetani* Yetani::connect(const std::string& display ///< The Display Name or ID
	, std::error_code&                 error   ///< The error status
	) noexcept
{
	Yetani* yetani = new Yetani();

	const char* display_name = nullptr;

	if(display.empty() == false)
	{
		display_name = display.c_str();
	}

	// --- Get the Display --- //
	yetani->display = wl_display_connect(display_name);
	if(yetani->display == nullptr)
	{
		delete yetani;

		const char* session = getenv("XDG_SESSION_TYPE");

		if(session != nullptr
			&& strcasecmp(session, "wayland") != 0
			)
		{
			error = ZAKERO_YETANI__ERROR(Error_Wayland_Not_Available);
		}
		else if(display.empty())
		{
			error = ZAKERO_YETANI__ERROR(Error_Connection_Failed);
		}
		else
		{
			error = ZAKERO_YETANI__ERROR(Error_Invalid_Display_Name);
		}

		return nullptr;
	}

	// --- Get the Registry --- //
	yetani->registry = wl_display_get_registry(yetani->display);
	if(yetani->registry == nullptr)
	{
		delete yetani;

		error = ZAKERO_YETANI__ERROR(Error_Registry_Not_Available);

		return nullptr;
	}

	wl_registry_add_listener(yetani->registry, &registry_listener, yetani);

	// --- Wait for all Global Objects to be registered --- //
	wl_display_dispatch(yetani->display);
	wl_display_roundtrip(yetani->display);

	// --- Validate required Global Objects --- //
	if(yetani->compositor == nullptr)
	{
		delete yetani;

		error = ZAKERO_YETANI__ERROR(Error_Compositor_Was_Not_Found);

		return nullptr;
	}

	if(yetani->shm == nullptr)
	{
		delete yetani;

		error = ZAKERO_YETANI__ERROR(Error_Shm_Was_Not_Found);

		return nullptr;
	}

	if(yetani->xdg_wm_base == nullptr)
	{
		delete yetani;

		error = ZAKERO_YETANI__ERROR(Error_Xdg_WM_Base_Was_Not_Found);

		return nullptr;
	}

	yetani->cursorSetup();

	yetani->eventLoopStart();

	error = ZAKERO_YETANI__ERROR(Error_None);

	return yetani;
}


/**
 * \brief Disconnect from Wayland.
 *
 * In addition to disconnecting from Wayland, all allocated resources will be 
 * destroyed.  Any windows created from this instance will be invalidated and 
 * any further use of those windows will result in undefined behavior.
 *
 * \thread_user
 */
void Yetani::disconnect() noexcept
{
	cursorTeardown();

	if(decoration_manager != nullptr)
	{
		zxdg_decoration_manager_v1_destroy(decoration_manager);
		decoration_manager = nullptr;
	}

	if(xdg_wm_base != nullptr)
	{
		xdg_wm_base_destroy(xdg_wm_base);
		xdg_wm_base = nullptr;
	}

	if(shm != nullptr)
	{
		wl_shm_destroy(shm);
		shm = nullptr;
	}

	// Seat
	id_to_seat.clear();

	while(seat_map.empty() == false)
	{
		auto iter = std::begin(seat_map);

		struct wl_seat* wl_seat = iter->first;

		seatDestroy(wl_seat);
	}

	// Wayland Output Devices
	{
		std::lock_guard<std::mutex> lock(output_data.mutex);

		for(auto& iter : output_data.output_map)
		{
			struct wl_output* wayland_output = iter.first;

			wl_output_destroy(wayland_output);
		}

		output_changes_map.clear();
		output_state_map.clear();
		output_data.output_map.clear();
		output_data.wloutput_to_outputid.clear();
		output_data.outputid_to_wloutput.clear();
	}

	if(registry != nullptr)
	{
		wl_registry_destroy(registry);
		registry = nullptr;
	}

	if(compositor != nullptr)
	{
		wl_compositor_destroy(compositor);
		compositor = nullptr;
	}

	if(display != nullptr)
	{
		wl_display_disconnect(display);
		display = nullptr;
	}

	return;
}

// }}}
// {{{ Cursor

/**
 * \struct Yetani::CursorConfig
 *
 * \brief %Cursor configuration.
 *
 * \var Yetani::CursorConfig::size
 * \brief The size of the cursor
 *
 * \var Yetani::CursorConfig::format
 * \brief The pixel format
 *
 * \var Yetani::CursorConfig::hotspot_x
 * \brief The X location of the hotspot
 *
 * \var Yetani::CursorConfig::hotspot_y
 * \brief The Y location of the hotspot
 *
 * \var Yetani::CursorConfig::time_per_frame
 * \brief How long to display each frame of animation
 *
 * \var Yetani::CursorConfig::image_data
 * \brief A collection of image data.
 */


/**
 * \struct Yetani::Cursor
 *
 * \brief %Cursor data
 *
 * The source of data for cursors.  All data that is needed to maintain and 
 * manage cursors is in this object.
 *
 * \var Yetani::Cursor::wl_surface
 * \brief The Wayland Surface
 *
 * The Wayland surface that is used to render the cursor.
 *
 * \var Yetani::Cursor::buffer_vector
 * \brief A collection of buffers
 *
 * All the buffers needed to render the cursor.
 * 
 * \var Yetani::Cursor::format
 * \brief The pixel format
 *
 * The pixel format of the cursor image data.
 *
 * \var Yetani::Cursor::next_frame_time
 * \brief When to display the next frame.
 *
 * When to display the next frame in milliseconds.
 *
 * \var Yetani::Cursor::buffer_index
 * \brief The current buffer
 *
 * The index of the buffer that is currently attached to the surface.
 *
 * \var Yetani::Cursor::time_per_frame
 * \brief How long to display each frame of animation
 *
 * The amount of time to display each frame of the image data.  Approximately 
 * 33 milliseconds per frame, will create an animate cursor that plays at 30 
 * frames per second.
 *
 * \var Yetani::Cursor::width
 * \brief The width of the cursor
 *
 * The pixel width of the cursor.
 *
 * \var Yetani::Cursor::height
 * \brief The height of the cursor
 *
 * The pixel height of the cursor.
 *
 * \var Yetani::Cursor::hotspot_x
 * \brief The X location of the hotspot
 *
 * The X location of the hotspot
 *
 * \var Yetani::Cursor::hotspot_y
 * \brief The Y location of the hotspot
 *
 * The Y location of the hotspot
 */


/**
 * \struct Yetani::CursorSurface
 *
 * \brief %Cursor event data.
 *
 * This structure only holds the cursor data need to respond to events and 
 * display requests.
 *
 * \var Yetani::CursorSurface::wl_pointer
 * \brief The Wayland Pointer
 *
 * The Wayland Pointer that triggered the cursor to be displayed.
 *
 * \var Yetani::CursorSurface::wl_surface
 * \brief The Wayland Surface
 *
 * The Wayland surface that is used to render the cursor.
 *
 * \var Yetani::CursorSurface::serial
 * \brief The event id.
 *
 * The most recent event id that the cursor has responded to.
 *
 * \var Yetani::CursorSurface::hotspot_x
 * \brief The X location of the hotspot
 *
 * The X location of the hotspot
 *
 * \var Yetani::CursorSurface::hotspot_y
 * \brief The Y location of the hotspot
 *
 * The Y location of the hotspot
 *
 * \var Yetani::CursorSurface::is_visible
 * \brief A visibility flag
 *
 * Stores the visibility of the cursor.
 */


/**
 * \brief Animate all cursors.
 *
 * \thread_wayland
 */
void Yetani::cursorAnimate() noexcept
{
	int64_t time_now = ZAKERO_STEADY_TIME_NOW(milliseconds);

	std::lock_guard<std::mutex> lock(cursor_mutex);

	for(auto& iter : cursor_map)
	{
		Yetani::Cursor& cursor = iter.second;

		if(cursor.next_frame_time <= time_now)
		{
			const int64_t time_over = time_now - cursor.next_frame_time;
			cursor.next_frame_time = time_now + cursor.time_per_frame - time_over;

			cursor.buffer_index = (cursor.buffer_index + 1) % cursor.buffer_vector.size();

			wl_surface_attach(cursor.wl_surface, cursor.buffer_vector[cursor.buffer_index], 0, 0);
			wl_surface_damage(cursor.wl_surface
				, 0, 0
				, cursor.width, cursor.height
				);

			wl_surface_commit(cursor.wl_surface);
		}
	}
}


/**
 * \brief Create a cursor.
 *
 * A cursor with the given \p name will be created.  The \p config data will be 
 * used to determine how the cursor will be configured.
 *
 * \par Cursor Configuration
 * \parblock
 *
 * The CursorConfig::image_data holds the graphic image data for each frame of 
 * the cursor.  If the cursor is not animated, then only one frame needs to be 
 * provided.
 *
 * Each frame of the cursor (including non-animated cursors, i.e.: a single 
 * frame) must have the dimension specified by CursorConfig::size.  And the 
 * data in each frame must conform to the value of CursorConfig::format.  The 
 * required amount of data for each frame is:
 * `size_in_bytes = size.width * size.height * shmFormatBytesPerPixel(format)`
 *
 * If more than one frame is provided in CursorConfig::image_data, then the 
 * amount of time to display each frame must be placed in 
 * CursorConfig::time_per_frame.
 *
 * Once the last frame of the cursor has been displayed, the first frame will 
 * be displayed next creating a continuous looping animation.
 *
 * Every cursor needs to have a spot that is used to determine where the 
 * pointer is located called a _hotspot_.  The location of the hotspot is 
 * relative to the top-left corner (0x0) and should be assigned to 
 * CursorConfig::hotspot_x and CursorConfig::hotspot_y.
 * \endparblock
 *
 * If the configuration was valid and the cursor was successfully created, then 
 * `Yetani::Error_None` will be returned.  Otherwise, another error will be 
 * returned to indicate the encountered problem.
 *
 * \return An error code.
 *
 * \thread_user
 */
std::error_code Yetani::cursorCreate(const std::string& name   ///< The name of the cursor
	, const Yetani::CursorConfig&                   config ///< The cursor configuration
	) noexcept
{
	if(name.empty())
	{
		return ZAKERO_YETANI__ERROR(Error_Cursor_Name_Is_Invalid);
	}

	if(cursor_map.contains(name) == true)
	{
		return ZAKERO_YETANI__ERROR(Error_Cursor_Already_Exists);
	}

	if(config.size.width <= 0 || config.size.height <= 0)
	{
		return ZAKERO_YETANI__ERROR(Error_Cursor_Size_Too_Small);
	}

	if(config.image_data.empty())
	{
		return ZAKERO_YETANI__ERROR(Error_Cursor_Image_Data_Is_Empty);
	}
	else if(config.image_data.size() > 1)
	{
		if(config.time_per_frame.count() <= 0)
		{
			return ZAKERO_YETANI__ERROR(Error_Cursor_Frame_Time_Too_Small);
		}

		if(config.time_per_frame.count() > Size_Max)
		{
			return ZAKERO_YETANI__ERROR(Error_Cursor_Frame_Time_Too_Large);
		}
	}

	std::error_code error = cursorCreateCursor(name, config);

	return error;
}


/**
 * \brief Create the cursor.
 *
 * cursorCreate() only validates the \p cursor_name and \p cursor_config.  This 
 * method does the actual creation.
 *
 * \return An error code.
 *
 * \thread_user
 */
std::error_code Yetani::cursorCreateCursor(const std::string& cursor_name   ///< The name of the cursor
	, const Yetani::CursorConfig&                         cursor_config ///< The cursor configuration
	) noexcept
{
	const uint8_t bytes_per_pixel = shmFormatBytesPerPixel(cursor_config.format);
	const size_t  frame_count     = cursor_config.image_data.size();

	Yetani::Cursor cursor =
	{	.wl_surface      = wl_compositor_create_surface(compositor)
	,	.buffer_vector   = { frame_count, nullptr }
	,	.format          = cursor_config.format
	,	.next_frame_time = ZAKERO_STEADY_TIME_NOW(milliseconds)
	,	.buffer_index    = 0
	,	.time_per_frame  = uint32_t(cursor_config.time_per_frame.count())
	,	.width           = cursor_config.size.width
	,	.height          = cursor_config.size.height
	,	.hotspot_x       = cursor_config.hotspot_x
	,	.hotspot_y       = cursor_config.hotspot_y
	};

	if(cursor.time_per_frame == 0)
	{
		cursor.time_per_frame = Size_Max;
	}

	const int    stride     = cursor.width * bytes_per_pixel;
	const size_t image_size = stride * cursor.height;

	for(size_t i = 0; i < frame_count; i++)
	{
		std::error_code error;

		off_t offset = cursor_memory_pool.alloc(image_size, error);
		if(error)
		{
			while(i > 0)
			{
				i--;

				struct wl_buffer* buffer = cursor.buffer_vector[i];
				cursor.buffer_vector[i] = nullptr;

				off_t offset = (off_t)wl_buffer_get_user_data(buffer);
				wl_buffer_destroy(buffer);

				cursor_memory_pool.free(offset);
			}

			return error;
		}

		uint32_t* p = (uint32_t*)cursor_memory_pool.addressOf(offset);
		memcpy(p, (uint8_t*)cursor_config.image_data[i], image_size);

		cursor.buffer_vector[i] = wl_shm_pool_create_buffer(cursor_shm_pool
			, offset
			, cursor.width
			, cursor.height
			, stride
			, cursor.format
			);

		wl_buffer_set_user_data(cursor.buffer_vector[i], (void*)offset);
	}

	wl_surface_attach(cursor.wl_surface, cursor.buffer_vector[0], 0, 0);
	wl_surface_commit(cursor.wl_surface);

	std::lock_guard<std::mutex> lock(cursor_mutex);

	cursor_map[cursor_name] = cursor;

	return ZAKERO_YETANI__ERROR(Error_None);
}


/**
 * \brief Destroy a cursor.
 *
 * The cursor with the specified \p name will be destroyed.  If any windows are 
 * using the cursor, they will report an error.
 *
 * \return An error code.
 *
 * \thread_user
 */
std::error_code Yetani::cursorDestroy(const std::string& name ///< The name of the cursor
	) noexcept
{
	Yetani::Cursor cursor;

	{
		std::lock_guard<std::mutex> lock(cursor_mutex);

		if(cursor_map.contains(name) == false)
		{
			return ZAKERO_YETANI__ERROR(Error_Cursor_Does_Not_Exist);
		}

		cursor = cursor_map[name];

		cursor_map.erase(name);
	}

	auto iter = std::begin(cursor_surface_map);
	auto iter_end = std::end(cursor_surface_map);
	while(iter != iter_end)
	{
		if(cursor.wl_surface == iter->second.wl_surface)
		{
			iter = cursor_surface_map.erase(iter);
		}
		else
		{
			iter++;
		}
	}

	if(cursor.wl_surface)
	{
		wl_surface_destroy(cursor.wl_surface);
		cursor.wl_surface = nullptr;
	}

	for(wl_buffer* buffer : cursor.buffer_vector)
	{
		off_t offset = (off_t)wl_buffer_get_user_data(buffer);
		wl_buffer_destroy(buffer);

		cursor_memory_pool.free(offset);
	}

	return ZAKERO_YETANI__ERROR(Error_None);
}


/**
 * \brief Change the cursor.
 *
 * When a pointer enters a surface, check if that surface has a preferred 
 * cursor.  If there is a cursor, then change the cursor.
 *
 * \thread_wayland
 */
void Yetani::cursorEnter(wl_pointer* wl_pointer ///< The current pointer
	, uint32_t                   serial     ///< The event id
	, struct wl_surface*         wl_surface ///< The surface that was entered
	) noexcept
{
	std::lock_guard<std::mutex> lock(cursor_mutex);

	cursor_pointer = wl_pointer;

	if(cursor_surface_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::CursorSurface& cursor_surface = cursor_surface_map[wl_surface];

	cursor_surface.wl_pointer = wl_pointer;
	cursor_surface.serial     = serial;

	if(cursor_surface.is_visible)
	{
		wl_pointer_set_cursor(cursor_surface.wl_pointer
			, cursor_surface.serial
			, cursor_surface.wl_surface
			, cursor_surface.hotspot_x
			, cursor_surface.hotspot_y
			);
	}
	else
	{
		wl_pointer_set_cursor(cursor_surface.wl_pointer
			, cursor_surface.serial
			, nullptr
			, 0
			, 0
			);
	}
}


/**
 * \brief Release the cursor.
 *
 * When a pointer leaves a surface, check if that surface has a preferred 
 * cursor.  If there is a cursor, then update the cursor to be in an unused 
 * state.
 *
 * \thread_wayland
 */
void Yetani::cursorLeave(struct wl_surface* wl_surface ///< The surface that was left
	) noexcept
{
	std::lock_guard<std::mutex> lock(cursor_mutex);

	cursor_pointer = nullptr;

	if(cursor_surface_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::CursorSurface& cursor_surface = cursor_surface_map[wl_surface];

	cursor_surface.wl_pointer = nullptr;
	cursor_surface.serial     = 0;
}


/**
 * \brief Hide the cursor.
 *
 * If the specified \p wl_surface has a cursor, hide it.
 *
 * \thread_user
 */
void Yetani::cursorHide(struct wl_surface* wl_surface ///< The surface
	) noexcept
{
	std::lock_guard<std::mutex> lock(cursor_mutex);

	if(cursor_surface_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::CursorSurface& cursor_surface = cursor_surface_map[wl_surface];

	cursor_surface.is_visible = false;

	if(cursor_surface.wl_pointer != nullptr)
	{
		wl_pointer_set_cursor(cursor_surface.wl_pointer
			, cursor_surface.serial
			, nullptr
			, 0
			, 0
			);
	}
}


/**
 * \brief Show the cursor.
 *
 * If the specified \p wl_surface has a cursor, show it.
 *
 * \thread_user
 */
void Yetani::cursorShow(struct wl_surface* wl_surface ///< The surface
	) noexcept
{
	std::lock_guard<std::mutex> lock(cursor_mutex);

	if(cursor_surface_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::CursorSurface& cursor_surface = cursor_surface_map[wl_surface];

	cursor_surface.is_visible = true;

	if(cursor_surface.wl_pointer != nullptr)
	{
		wl_pointer_set_cursor(cursor_surface.wl_pointer
			, cursor_surface.serial
			, cursor_surface.wl_surface
			, cursor_surface.hotspot_x
			, cursor_surface.hotspot_y
			);
	}
}


/**
 * \brief Is the cursor hidden?
 *
 * If the cursor is hidden, then return `true'.  Otherwise, return `false`.  
 *
 * \retval true  The cursor is hidden.
 * \retval false The cursor is shown.
 *
 * \thread_user
 */
bool Yetani::cursorIsHidden(struct wl_surface* wl_surface ///< The surface
	) const noexcept
{
	std::lock_guard<std::mutex> lock(cursor_mutex);

	if(cursor_surface_map.contains(wl_surface) == false)
	{
		return true;
	}

	const Yetani::CursorSurface& cursor_surface = cursor_surface_map.at(wl_surface);

	return !(cursor_surface.is_visible);
}


/**
 * \brief Setup the cursor subsystem.
 *
 * This method will setup the required shared memory for the cursor data.
 *
 * \thread_user
 */
void Yetani::cursorSetup() noexcept
{
	cursor_map.clear();

	uint64_t bytes = zakero::convert((uint64_t)4, zakero::Storage::Kilobyte, zakero::Storage::Byte);
	cursor_memory_pool.init(bytes, true, zakero::MemoryPool::Alignment::Bits_32);

	cursor_memory_pool.sizeOnChange([&](size_t new_size)
	{
		wl_shm_pool_resize(cursor_shm_pool, new_size);
	});

	cursor_shm_pool = wl_shm_create_pool(shm, cursor_memory_pool.fd(), cursor_memory_pool.size());
}


/**
 * \brief Teardown the cursor subsystem.
 *
 * All created cursors will be destroyed as well as the shared memory.
 *
 * \thread_user
 */
void Yetani::cursorTeardown() noexcept
{
	while(cursor_map.empty() == false)
	{
		const auto& iter = cursor_map.begin();

		const std::string& name = iter->first;

		cursorDestroy(name);
	}

	if(cursor_shm_pool != nullptr)
	{
		wl_shm_pool_destroy(cursor_shm_pool);
	}
}


/**
 * \brief Associate a cursor with a surface.
 *
 * The cursor with the given \p cursor_name will be attached to the \p 
 * wl_surface.  Whenever a pointer enters that \p wl_surface, this specified 
 * cursor will be displayed.
 *
 * \return An error code.
 *
 * \thread_user
 */
std::error_code Yetani::cursorAttach(const std::string& cursor_name ///< The cursor name
	, struct wl_surface*                            wl_surface  ///< The surface
	) noexcept
{
	std::lock_guard<std::mutex> lock(cursor_mutex);

	if(cursor_map.contains(cursor_name) == false)
	{
		return ZAKERO_YETANI__ERROR(Error_Cursor_Does_Not_Exist);
	}

	if(cursor_surface_map.contains(wl_surface) == false)
	{
		cursor_surface_map[wl_surface] =
		{	.wl_pointer = cursor_pointer
		,	.wl_surface = nullptr
		,	.serial     = 0
		,	.hotspot_x  = 0
		,	.hotspot_y  = 0
		,	.is_visible = true
		};
	}

	Yetani::CursorSurface& cursor_surface = cursor_surface_map[wl_surface];
	Yetani::Cursor&        cursor         = cursor_map[cursor_name];

	cursor_surface.wl_surface = cursor.wl_surface;
	cursor_surface.hotspot_x  = cursor.hotspot_x;
	cursor_surface.hotspot_y  = cursor.hotspot_y;

	if(cursor_surface.wl_pointer != nullptr)
	{
		if(cursor_surface.is_visible)
		{
			wl_pointer_set_cursor(cursor_surface.wl_pointer
				, cursor_surface.serial
				, cursor_surface.wl_surface
				, cursor_surface.hotspot_x
				, cursor_surface.hotspot_y
				);
		}
		else
		{
			wl_pointer_set_cursor(cursor_surface.wl_pointer
				, cursor_surface.serial
				, nullptr
				, 0
				, 0
				);
		}
	}

	return ZAKERO_YETANI__ERROR(Error_None);
}


/**
 * \brief Disassociate a cursor with a surface.
 *
 * The cursor with the given \p cursor_name will be detached from the \p 
 * wl_surface.  Whenever a pointer enters the \p wl_surface, the current cursor 
 * will be used, or the Wayland cursor will be undefined.
 *
 * \return An error code.
 *
 * \thread_user
 */
std::error_code Yetani::cursorDetach(struct wl_surface* wl_surface ///< The surface
	) noexcept
{
	std::lock_guard<std::mutex> lock(cursor_mutex);

	if(cursor_surface_map.contains(wl_surface) == false)
	{
		return ZAKERO_YETANI__ERROR(Error_Cursor_Not_Attached);
	}

	Yetani::CursorSurface& cursor_surface = cursor_surface_map[wl_surface];

	if(cursor_surface.wl_pointer != nullptr)
	{
		wl_pointer_set_cursor(cursor_surface.wl_pointer
			, cursor_surface.serial
			, nullptr
			, 0
			, 0
			);
	}

	cursor_surface_map.erase(wl_surface);

	return ZAKERO_YETANI__ERROR(Error_None);
}

// }}}
// {{{ Event Loop

//#define ZAKERO_YETANI__ENABLE_THREAD_SCHEDULER

/**
 * \brief Start the event loop.
 *
 * Calling this method will start the Event Loop and block until the Event Loop 
 * has started.
 *
 * \thread_user
 */
void Yetani::eventLoopStart() noexcept
{
	event_loop = std::jthread(&Yetani::eventLoop, this);

	while(event_loop_is_running.load() == false)
	{
		// Wait for the thread to start
		std::this_thread::sleep_for(std::chrono::nanoseconds(42));
	}

	#ifdef ZAKERO_YETANI__ENABLE_THREAD_SCHEDULER
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
 * The Yetani Event Loop handles all the messages between the Wayland client 
 * and server.  Without this communication, programs that use the Yetani object 
 * will not be able to do anything.
 *
 * The usual Wayland event loop is not used because `wl_display_dispatch()` 
 * will block if there are no events to process.  And even if events are 
 * created later on, `wl_display_dispatch()` stays in a blocked state.  To work 
 * around this (bug?), `wl_display_dispatch()` is only called when there are 
 * events to be read (POLLIN).
 *
 * However, `wl_display_dispatch()` also sends data to the Wayland Server, so 
 * nothing will happen unless data is explicitly flushed.  This is the reason 
 * for the check to see if data can be written (POLLOUT) and if so, flush any 
 * pending data.
 *
 * \todo When a window mode is maximize/fullscreen, there are strange delays in 
 * this event loop.  The problem sees to be the Surface frame callback is not 
 * being send-by-Wayland or not getting received in a timely manner.  Note that 
 * animated cursors do not have animation skips even when the window surface 
 * is.  Look into manually swapping buffers in the while loop instead of 
 * waiting for the frame callback.
 *
 * \thread_wayland
 */
void Yetani::eventLoop(std::stop_token thread_token ///< Used to signal thread termination
	, Yetani*                      yetani       ///< The Yetani instance to use
	) noexcept
{
	struct pollfd fd_status =
	{	.fd      = wl_display_get_fd(yetani->display)
	,	.events  = POLLIN | POLLOUT
	,	.revents = 0
	};

	yetani->event_loop_is_running.store(true);

	// Handle events and render window contents
	while(thread_token.stop_requested() == false)
	{
		poll(&fd_status, 1, 1);

		if(fd_status.revents & POLLIN)
		{
			wl_display_dispatch(yetani->display);
		}

		yetani->cursorAnimate();

		keyboardRepeat(yetani->keyboard);

		if(fd_status.revents & POLLOUT)
		{
			wl_display_flush(yetani->display);
		}

		#ifdef ZAKERO_YETANI__ENABLE_THREAD_SCHEDULER
		std::this_thread::yield();
		#endif
	}

	yetani->event_loop_is_running.store(false);
}

// }}}
// {{{ Shared Memory

/**
 * \brief Get all the support color formats.
 *
 * All of the supported wl_shm_format values, color formats, are provided in 
 * the returned container.
 *
 * \return The supported formats.
 *
 * \thread_user
 */
const Yetani::VectorShmFormat& Yetani::shmFormatAvailable() const noexcept
{
	return shm_format_vector;
}


/**
 * \brief Determine bytes-per-pixel
 *
 * When allocating buffers in %Wayland, the number of bytes required per pixel 
 * is important so that the correct amount of memory is known.  This method 
 * will determine the bytes-per-pixel based on the provided \p shm_format.
 *
 * \return The number of bytes-per-pixel
 *
 * \thread_user
 */
uint8_t Yetani::shmFormatBytesPerPixel(const wl_shm_format shm_format ///< The format
	) noexcept
{
	switch(shm_format)
	{
#define X(value_, bytes_, desc_) \
		case value_: return bytes_;
		ZAKERO_YETANI__SHM_FORMAT
#undef X
		default: return 0;
	}
}


/**
 * \brief Get a description of the format.
 *
 * In the documented code of %Wayland, there are descriptions for each of the 
 * supported color formats.  By providing the \p shm_format, the description of 
 * the format will be returned.
 *
 * \return A description
 *
 * \thread_user
 */
std::string Yetani::shmFormatDescription(const wl_shm_format shm_format ///< The format
	) noexcept
{
	switch(shm_format)
	{
#define X(value_, bytes_, desc_) \
		case value_: return desc_;
		ZAKERO_YETANI__SHM_FORMAT
#undef X
		default: return "";
	}
}


/**
 * \brief Get the name of the format.
 *
 * Converts the enum name of the \p shm_format to a string.
 *
 * \return The name
 *
 * \thread_user
 */
std::string Yetani::shmFormatName(const wl_shm_format shm_format ///< The format
	) noexcept
{
	switch(shm_format)
	{
#define X(value_, bytes_, desc_) \
		case value_: return #value_;
		ZAKERO_YETANI__SHM_FORMAT
#undef X
		default: return "";
	}
}

// }}}
// {{{ Utility


// }}}
// {{{ Wayland : Buffer

/**
 * \struct Yetani::BufferData
 *
 * \brief %Buffer data location.
 *
 * The variables in this structure are used to locate the wl_buffer's data in 
 * the shared memory.
 *
 * \var Yetani::BufferData::memory_pool
 * \brief The MemoryPool
 *
 * \var Yetani::BufferData::offset
 * \brief The offset in the MemoryPool
 */

/**
 * \struct Yetani::Buffer
 *
 * \brief %Buffer collection.
 *
 * A collection of BufferData and a mutex for thread protected access.
 *
 * \var Yetani::Buffer::map
 * \brief A collection of BufferData
 *
 * \var Yetani::Buffer::mutex
 * \brief Thread protection
 */

/**
 * \brief Create a Waylond buffer.
 *
 * Allocates memory from the \p window_memory that will provide enough room for 
 * the \p surface_size.  The allocation information will be stored in the \p 
 * buffer.
 *
 * \return A pointer to a wl_buffer.
 *
 * \thread_user
 */
struct wl_buffer* Yetani::bufferCreate(Yetani::SurfaceSize& surface_size  ///< The size of the Surface
	, Yetani::Window::Memory*                           window_memory ///< The memory pool to use
	, Yetani::Buffer*                                   buffer        ///< Where to save the data
	) noexcept
{
	off_t offset = window_memory->memory_pool.alloc(surface_size.in_bytes);

	struct wl_buffer* wl_buffer = wl_shm_pool_create_buffer(window_memory->wl_shm_pool
		, offset
		, surface_size.width
		, surface_size.height
		, surface_size.stride
		, surface_size.pixel_format
		);

	wl_buffer_set_user_data(wl_buffer, buffer);

	buffer->mutex.lock();
	{
		buffer->map[wl_buffer] =
		{	.memory_pool = &window_memory->memory_pool
		,	.offset      = offset
		};
	}
	buffer->mutex.unlock();

	return wl_buffer;
}


/**
 * \brief Destroy a Wayland buffer.
 *
 * All associated data with the \p wl_buffer will be destroyed along with the 
 * \p wl_buffer.
 *
 * \thread_user
 * \thread_wayland
 */
void Yetani::bufferDestroy(struct wl_buffer*& wl_buffer ///< The buffer to destroy
	) noexcept
{
	Yetani::Buffer* buffer = (Yetani::Buffer*)wl_buffer_get_user_data(wl_buffer);

	wl_buffer_destroy(wl_buffer);

	buffer->mutex.lock();
	{
		BufferData& buffer_data = buffer->map[wl_buffer];

		buffer_data.memory_pool->free(buffer_data.offset);

		buffer->map.erase(wl_buffer);
	}
	buffer->mutex.unlock();

	wl_buffer = nullptr;
}

// }}}
// {{{ Wayland : Output

/**
 * \struct Yetani::Output
 *
 * \brief Information about a output device.
 *
 * All the Wayland information about an output device in a single structure.
 *
 * \var Yetani::Output::make
 * \brief Description of the manufacturer.
 *
 * \var Yetani::Output::model
 * \brief Description of the model.
 *
 * \var Yetani::Output::x
 * \brief The X position within the global compositor.
 *
 * \var Yetani::Output::y
 * \brief The Y position within the global compositor.
 *
 * \var Yetani::Output::width
 * \brief The width of the device in hardware units.
 *
 * \var Yetani::Output::height
 * \brief The height of the device in hardware units.
 *
 * \var Yetani::Output::physical_width_mm
 * \brief The width of the device in millimeters.
 *
 * \var Yetani::Output::physical_height_mm
 * \brief The height of the device in millimeters.
 *
 * \var Yetani::Output::subpixel
 * \brief The device's subpixel orientation.
 *
 * \var Yetani::Output::refresh_mHz
 * \brief The current refresh rate of the device.
 *
 * \var Yetani::Output::scale_factor
 * \brief The scaling factor between the device and compositor.
 *
 * \var Yetani::Output::transform
 * \brief Transform that maps framebuffer to output.
 *
 * \var Yetani::Output::flags
 * \brief wl_output_mode bitfield properties.
 *
 * \var Yetani::Output::pixels_per_mm_horizontal
 * \brief A pre-calculated value.
 *
 * \var Yetani::Output::pixels_per_mm_vertical
 * \brief A pre-calculated value.
 */

/**
 * \enum Yetani::OutputState
 *
 * \brief %Output information state
 */

/**
 * \struct Yetani::OutputData
 *
 * \brief A collection of all the %Output data.
 *
 * \var Yetani::OutputData::surface_output_map
 * \brief wl_surface to wl_output look-up table.
 *
 * \var Yetani::OutputData::outputid_to_wloutput
 * \brief OutputId to wl_output look-up table.
 *
 * \var Yetani::OutputData::output_map
 * \brief A container of all the output data.
 *
 * \var Yetani::OutputData::wloutput_to_outputid
 * \brief wl_output to OutputId look-up table.
 *
 * \var Yetani::OutputData::mutex
 * \brief Thread protection
 */


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
Yetani::Output Yetani::output(const Yetani::OutputId output_id ///< The Output Id
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.outputid_to_wloutput.contains(output_id) == false)
	{
		ZAKERO_YETANI__DEBUG
			<< "Invalid output_id: "
			<< std::to_string(output_id)
			;

		return {};
	}

	struct wl_output* wl_output = output_data.outputid_to_wloutput.at(output_id);

	return output_data.output_map.at(wl_output);
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
Yetani::VectorOutputId Yetani::outputVector() const noexcept
{
	Yetani::VectorOutputId vector;

	std::lock_guard<std::mutex> lock(output_data.mutex);

	for(const auto& iter : output_data.outputid_to_wloutput)
	{
		vector.push_back(iter.first);
	}

	return vector;
}


/**
 * \brief Get a human readable string.
 *
 * The `Yetani::Output::subpixel` is a Wayland enum value and this method will 
 * convert that value into a descriptive name string.  If an invalid value is 
 * passed, then an empty string will be returned.
 *
 * \return The name of the subpixel format.
 *
 * \thread_user
 */
std::string Yetani::outputSubpixelName(int32_t subpixel_format ///< The Subpixel format
	) noexcept
{
	switch(subpixel_format)
	{
#define X(value_, name_) \
		case value_: return name_;
		ZAKERO_YETANI__OUTPUT_SUBPIXEL
#undef X
		default: return "";
	}
}


/**
 * \brief Get a human readable string.
 *
 * The `Yetani::Output::transform` is a Wayland enum value and this method will 
 * convert that value into a descriptive name string.  If an invalid value is 
 * passed, then an empty string will be returned.
 *
 * \return The name of the transform.
 *
 * \thread_user
 */
std::string Yetani::outputTransformName(int32_t transform ///< The transform value
	) noexcept
{
	switch(transform)
	{
#define X(value_, name_) \
		case value_: return name_;
		ZAKERO_YETANI__OUTPUT_TRANSFORM
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
Yetani::PointMm Yetani::outputConvertToMm(const Yetani::OutputId output_id ///< The output device for the conversion
	, const Yetani::PointPixel&                              point     ///< The point to convert
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.outputid_to_wloutput.contains(output_id) == false)
	{
		return { point.time, 0, 0 };
	}

	struct wl_output*     wl_output = output_data.outputid_to_wloutput.at(output_id);
	const Yetani::Output& output    = output_data.output_map.at(wl_output);

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
Yetani::PointPercent Yetani::outputConvertToPercent(const Yetani::OutputId output_id ///< The output device for the conversion
	, const Yetani::PointPixel&                                        point     ///< The point to convert
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.outputid_to_wloutput.contains(output_id) == false)
	{
		return { point.time, 0, 0 };
	}

	struct wl_output*     wl_output = output_data.outputid_to_wloutput.at(output_id);
	const Yetani::Output& output    = output_data.output_map.at(wl_output);

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
Yetani::PointPixel Yetani::outputConvertToPixel(const Yetani::OutputId output_id ///< The output device for the conversion
	, const Yetani::PointMm&                                       point     ///< The point to convert
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.outputid_to_wloutput.contains(output_id) == false)
	{
		return { point.time, 0, 0 };
	}

	struct wl_output*     wl_output = output_data.outputid_to_wloutput.at(output_id);
	const Yetani::Output& output    = output_data.output_map.at(wl_output);

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
Yetani::PointPixel Yetani::outputConvertToPixel(const Yetani::OutputId output_id ///< The output device for the conversion
	, const Yetani::PointPercent&                                  point     ///< The point to convert
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.outputid_to_wloutput.contains(output_id) == false)
	{
		return { point.time, 0, 0 };
	}

	struct wl_output*     wl_output = output_data.outputid_to_wloutput.at(output_id);
	const Yetani::Output& output    = output_data.output_map.at(wl_output);

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
Yetani::SizeMm Yetani::outputConvertToMm(const Yetani::OutputId output_id ///< The output device for the conversion
	, const Yetani::SizePixel&                              size      ///< The size to convert
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.outputid_to_wloutput.contains(output_id) == false)
	{
		return { 0, 0 };
	}

	struct wl_output*     wl_output = output_data.outputid_to_wloutput.at(output_id);
	const Yetani::Output& output    = output_data.output_map.at(wl_output);

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
Yetani::SizePercent Yetani::outputConvertToPercent(const Yetani::OutputId output_id ///< The output device for the conversion
	, const Yetani::SizePixel&                                        size      ///< The size to convert
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.outputid_to_wloutput.contains(output_id) == false)
	{
		return { 0, 0 };
	}

	struct wl_output*     wl_output = output_data.outputid_to_wloutput.at(output_id);
	const Yetani::Output& output    = output_data.output_map.at(wl_output);

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
Yetani::SizePixel Yetani::outputConvertToPixel(const Yetani::OutputId output_id ///< The output device for the conversion
	, const Yetani::SizeMm&                                       size      ///< The size to convert
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.outputid_to_wloutput.contains(output_id) == false)
	{
		return { 0, 0 };
	}

	struct wl_output*     wl_output = output_data.outputid_to_wloutput.at(output_id);
	const Yetani::Output& output    = output_data.output_map.at(wl_output);

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
Yetani::SizePixel Yetani::outputConvertToPixel(const Yetani::OutputId output_id ///< The output device for the conversion
	, const Yetani::SizePercent&                                  size      ///< The size to convert
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.outputid_to_wloutput.contains(output_id) == false)
	{
		return { 0, 0 };
	}

	struct wl_output*     wl_output = output_data.outputid_to_wloutput.at(output_id);
	const Yetani::Output& output    = output_data.output_map.at(wl_output);

	auto p = convertPercentToPixel(output, size.width, size.height);

	return { p.first, p.second };
}


/**
 * \brief Notification of adding an Output device.
 *
 * When a Wayland output device has been added, the \p lambda that was provided 
 * to this method will be called.  To disable these notifications, pass 
 * \nullptr as the value of \p lambda.
 *
 * \thread_user
 */
void Yetani::outputOnAdd(LambdaOutputId lambda ///< The lambda to call
	) noexcept
{
	if(lambda == nullptr)
	{
		on_output_add = LambdaOutputId_DoNothing;
	}
	else
	{
		on_output_add = lambda;
	}
}


/**
 * \brief Notification that an Output device has changed.
 *
 * When a Wayland output device's configuration has been changed, the \p lambda 
 * that was provided to this method will be called.  To disable these 
 * notifications, pass \nullptr as the value of \p lambda.
 *
 * \thread_user
 */
void Yetani::outputOnChange(LambdaOutputId lambda ///< The lambda to call
	) noexcept
{
	if(lambda == nullptr)
	{
		on_output_change = LambdaOutputId_DoNothing;
	}
	else
	{
		on_output_change = lambda;
	}
}


/**
 * \brief Notification of removing an Output device.
 *
 * When a Wayland output device has been removed, the \p lambda that was 
 * provided to this method will be called.  To disable these notifications, 
 * pass \nullptr as the value of \p lambda.
 *
 * \thread_user
 */
void Yetani::outputOnRemove(LambdaOutputId lambda ///< The lambda to call
	) noexcept
{
	if(lambda == nullptr)
	{
		on_output_remove = LambdaOutputId_DoNothing;
	}
	else
	{
		on_output_remove = lambda;
	}
}


/**
 * \brief Convert pixel to both millimeter and percentage values.
 *
 * Using the output device that the \p wl_surface is occupying, convert the 
 * provided pixel values into both millimeter and percentage values.
 *
 * \thread_wayland
 */
void Yetani::convertPixel(struct wl_surface* wl_surface ///< The surface for the conversion
	, const int32_t                      pixel_xw   ///< The pixel x or width value
	, const int32_t                      pixel_yh   ///< The pixel y or height value
	, float&                             mm_xw      ///< The calculated millimeter x or width value
	, float&                             mm_yh      ///< The calculated millimeter y or height value
	, float&                             pc_xw      ///< The calculated percentage x or width value
	, float&                             pc_yh      ///< The calculated percentage y or height value
	) const noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	const Yetani::VectorWlOutput& vector    = output_data.surface_output_map.at(wl_surface);
	struct wl_output*             wl_output = vector.front();
	const Yetani::Output&         output    = output_data.output_map.at(wl_output);

	auto mm = convertPixelToMm(output, pixel_xw, pixel_yh);
	mm_xw = mm.first;
	mm_yh = mm.second;

	auto pc = convertPixelToPercent(output, pixel_xw, pixel_yh);
	pc_xw = pc.first;
	pc_yh = pc.second;
}


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
std::pair<float, float> Yetani::convertPixelToMm(const Yetani::Output& output ///< The output for the conversion
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
std::pair<float, float> Yetani::convertPixelToPercent(const Yetani::Output& output ///< The output for the conversion
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
std::pair<int32_t, int32_t> Yetani::convertMmToPixel(const Yetani::Output& output ///< The output for the conversion
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
std::pair<int32_t, int32_t> Yetani::convertPercentToPixel(const Yetani::Output& output ///< The output for the conversion
	, float                                                                 xw     ///< The x or width value
	, float                                                                 yh     ///< The y or height value
	) const noexcept
{
	return
	{	int32_t(xw * output.width)
	,	int32_t(yh * output.height)
	};
}


/**
 * \brief Update surface size based on output changes.
 *
 * When an output device configuration has changed, update the size of the 
 * surface that is using that output.  The surface size only needs to be 
 * changed if its preferred size unit is Millimeters or Percentage.
 *
 * Since the surface size is adapting to the output changes, it is not 
 * technically changing size.  `10mm` is still `10mm` even after an output 
 * change.  Because of this reason, no size change events are needed.
 */
void Yetani::outputNotifySurface(Yetani* yetani     ///< The %Yetani instance
	, struct wl_output*              wl_output  ///< The output that changed
	, struct wl_surface*             wl_surface ///< The surface to resize
	) noexcept
{
	Yetani::SurfaceExtent& surface_extent = yetani->surface_extent_map[wl_surface];
	if(surface_extent.preferred_unit == Yetani::SizeUnit::Pixel)
	{
		return;
	}

	Yetani::OutputData& output_data = yetani->output_data;

	std::lock_guard<std::mutex> lock(output_data.mutex);

	Yetani::VectorWlOutput& output_vector = output_data.surface_output_map[wl_surface];

	struct wl_output* output_current = output_vector.front();

	if(output_current != wl_output)
	{
		return;
	}

	Yetani::Output&   output   = output_data.output_map.at(wl_output);
	Yetani::SizePixel new_size = surface_extent.size_pixel;

	if(surface_extent.preferred_unit == Yetani::SizeUnit::Millimeter)
	{
		auto p = yetani->convertMmToPixel(output, surface_extent.size_mm.width, surface_extent.size_mm.height);
		new_size = { p.first, p.second };
	}
	else if(surface_extent.preferred_unit == Yetani::SizeUnit::Percent)
	{
		auto p = yetani->convertPercentToPixel(output, surface_extent.size_percent.width, surface_extent.size_percent.height);
		new_size = { p.first, p.second };
	}

	if(new_size.width <= 0)
	{
		new_size.width = 1;
	}

	if(new_size.height <= 0)
	{
		new_size.height = 1;
	}

	yetani->surface_resize_mutex_map[wl_surface].lock();
	{
		surface_extent.size_pixel = new_size;
		surfaceCalculateSize(yetani, wl_surface, new_size);
	}
	yetani->surface_resize_mutex_map[wl_surface].unlock();
}

// }}}
// {{{ Wayland : Seat

/**
 * \struct Yetani::Seat
 *
 * \brief All the input devices.
 *
 * A _seat_ in Wayland is a collection of input devices: Anything a user can 
 * use for input while _sitting_ at a computer.  This structure maintains all 
 * the supported input devices for a _seat_.
 *
 * \var Yetani::Seat::wl_keyboard
 * \brief A pointer to a keyboard device
 *
 * \var Yetani::Seat::wl_pointer
 * \brief A pointer to a mouse/pointer device
 *
 * \var Yetani::Seat::wl_touch
 * \brief A pointer to a touch device
 *
 * \var Yetani::Seat::name
 * \brief The name of the Seat
 *
 * \var Yetani::Seat::wl_keyboard
 * \brief The version of the protocol
 */


/**
 * \brief Destroy a Seat.
 *
 * In addition to destroying the \p wl_seat, all contained input devices will 
 * also be released.
 */
void Yetani::seatDestroy(struct wl_seat*& wl_seat ///< A Wayland Seat
	) noexcept
{
	Yetani::Seat& seat = seat_map[wl_seat];

	if(seat.wl_keyboard != nullptr)
	{
		wl_keyboard_release(seat.wl_keyboard);
		seat.wl_keyboard = nullptr;
	}

	if(seat.wl_pointer != nullptr)
	{
		wl_pointer_release(seat.wl_pointer);
		seat.wl_pointer = nullptr;
	}

	if(seat.wl_touch != nullptr)
	{
		wl_touch_release(seat.wl_touch);
		seat.wl_touch = nullptr;
	}

	seat_map.erase(wl_seat);

	wl_seat_release(wl_seat);

	wl_seat = nullptr;
}

// }}}
// {{{ Wayland : Seat : Keyboard

/**
 * \struct Yetani::KeyRepeatData
 *
 * \brief %Key repeat timer data.
 *
 * Store time values for the key repeat timer.
 *
 * \var Yetani::KeyRepeatData::trigger_time
 * \brief The time to trigger the key repeat event.
 *
 * \var Yetani::KeyRepeatData::base_time
 * \brief The time counter.
 */

/**
 * \struct Yetani::KeyboardEvent
 *
 * \brief A collection of lambdas.
 *
 * A set of lambdas for each type of key event.
 *
 * \var Yetani::KeyboardEvent::on_enter
 * \brief The lambda when the surface gains focus.
 *
 * \var Yetani::KeyboardEvent::on_leave
 * \brief The lambda when the surface loses focus.
 *
 * \var Yetani::KeyboardEvent::on_key
 * \brief The lambda for a key event.
 */

/**
 * \struct Yetani::Keyboard
 *
 * \brief %Keyboard data.
 *
 * \var Yetani::Keyboard::wl_surface
 * \brief The active surface.
 *
 * \var Yetani::Keyboard::event
 * \brief The active event lambdas.
 *
 * \var Yetani::Keyboard::event_map
 * \brief A map of all event lambdas.
 *
 * \var Yetani::Keyboard::modifier
 * \brief The current key modifiers.
 *
 * \var Yetani::Keyboard::repeat_map
 * \brief A map of all repeat timer data.
 *
 * \var Yetani::Keyboard::keymap
 * \brief The current keymap layout.
 *
 * \var Yetani::Keyboard::keymap_size
 * \brief The size of the keymap layout.
 *
 * \var Yetani::Keyboard::repeat_delay
 * \brief The milliseconds to wait before starting key repeat signals.
 *
 * \var Yetani::Keyboard::repeat_rate
 * \brief The milliseconds between each key repeat signal.
 */


/**
 * \brief The key repeat delay.
 *
 * The _key repeat delay_ is the amount of time in milliseconds that must 
 * elapse from a key press event to change into a key repeat event.
 *
 * \return The key repeat delay.
 */
int32_t Yetani::keyRepeatDelay() const noexcept
{
	return keyboard.repeat_delay;
}


/**
 * \brief The key repeat rate.
 *
 * The _key repeat rate_ is the number of key repeat events that will be 
 * emitted per second.
 *
 * \return The key repeat rate.
 */
int32_t Yetani::keyRepeatRate() const noexcept
{
	return 1000 / keyboard.repeat_rate;
}


/**
 * \brief Destroy a keyboard.
 */
void Yetani::keyboardDestroy(Yetani::Keyboard& keyboard ///< The keyboard to destroy.
	) noexcept
{
	if(keyboard.keymap != nullptr)
	{
		munmap(keyboard.keymap, keyboard.keymap_size);
	}

	keyboard.wl_surface   = nullptr;
	keyboard.event        = nullptr;
	keyboard.modifier     = { 0 };
	keyboard.repeat_rate  = 0;
	keyboard.repeat_delay = {};
	keyboard.keymap       = nullptr;
	keyboard.keymap_size  = 0;
}


/**
 * \brief Trigger key repeat events.
 *
 * Using the provided \p keyboard, determine which keys need to emit repeat 
 * events.
 */
void Yetani::keyboardRepeat(Yetani::Keyboard& keyboard ///< The keyboard to process.
	) noexcept
{
	auto now = std::chrono::steady_clock::now();

	for(auto& iter : keyboard.repeat_map)
	{
		Yetani::KeyRepeatData& key_repeat = iter.second;

		if(now >= key_repeat.trigger_time)
		{
			uint32_t key_code = iter.first;

			Yetani::Key key =
			{	.time  = key_repeat.base_time
			,	.code  = key_code
			,	.state = Yetani::KeyState::Repeat
			};

			keyboard.event->on_key(key, keyboard.modifier);

			key_repeat.trigger_time = now
				+ std::chrono::milliseconds(keyboard.repeat_rate)
				- (now - key_repeat.trigger_time)
				;
			key_repeat.base_time += keyboard.repeat_rate;
		}
	}
}


/**
 * \brief Add a key that may repeat.
 *
 * After a key press on a \p keyboard, that key may emit a repeat event.  
 * Calculate when the time repeat event using the current \p time and add the 
 * key to repeat timer.
 */
void Yetani::keyboardRepeatAdd(Yetani::Keyboard& keyboard ///< The keyboard
	, uint32_t                               key_code ///< The key code
	, uint32_t                               time     ///< The current time
	) noexcept
{
	auto trigger_time = std::chrono::steady_clock::now()
		+ std::chrono::milliseconds(keyboard.repeat_delay)
		;

	keyboard.repeat_map[key_code] =
	{	.trigger_time = trigger_time
	,	.base_time    = time + keyboard.repeat_delay
	};
}


/**
 * \brief Remove all keys from the repeat timer.
 *
 * When a surface loses focus, it can no longer receive repeat events from any 
 * key.  This method will remove all the keys from the repeat timer.
 */
void Yetani::keyboardRepeatReleaseAll(Yetani::Keyboard& keyboard ///< The keyboard
	) noexcept
{
	while(keyboard.repeat_map.empty() == false)
	{
		auto iter = keyboard.repeat_map.begin();

		uint32_t key_code = iter->first;

		Yetani::Key key =
		{	.time  = 0
		,	.code  = key_code
		,	.state = Yetani::KeyState::Released
		};

		keyboard.event->on_key(key, keyboard.modifier);

		keyboard.repeat_map.erase(iter);
	}
}


/**
 * \brief Remove a key from the repeat timer.
 *
 * When a key has been released, it should no longer emit repeat events and so 
 * it needs to be removed from the repeat timer.
 */
void Yetani::keyboardRepeatRemove(Yetani::Keyboard& keyboard ///< The keyboard
	, uint32_t                                  key_code ///< The key to remove
	) noexcept
{
	keyboard.repeat_map.erase(key_code);
}

// }}}
// {{{ Wayland : Seat : Pointer

/**
 * \struct Yetani::PointerEvent
 *
 * \brief A collection of Wayland Pointer events.
 */

/**
 * \struct Yetani::Pointer
 *
 * \brief A data related to a Wayland Pointer.
 */

/**
 * \brief Clear all pointer data.
 */
void Yetani::pointerClear(Yetani::Pointer& pointer ///< The pointer to clear
	) noexcept
{
	pointer.enter_surface     = nullptr;
	pointer.enter_point       = { 0, 0, 0 };
	pointer.leave_surface     = nullptr;
	pointer.motion_point      = { 0, 0, 0 };
	pointer.button_event_code = 0;
	pointer.button_is_pressed = false;
	pointer.button_time       = 0;
	pointer.axis.time         = 0;
	pointer.axis.type         = Yetani::PointerAxisType::Unknown;
	pointer.axis.distance     = 0;
	pointer.axis.source       = Yetani::PointerAxisSource::Unknown;
	pointer.axis.steps        = 0;
}

// }}}
// {{{ Wayland : Surface

/**
 * \struct Yetani::SurfaceEvent
 *
 * \brief A collection of Wayland surface events.
 */

/**
 * \enum Yetani::SizeUnit
 *
 * \brief The unit of measurement for sizes.
 */

/**
 * \struct Yetani::SurfaceExtent
 *
 * \brief A collection of surface size data.
 */

/**
 * \struct Yetani::SurfaceFrame
 *
 * \brief Surface rendering data.
 */

/**
 * \struct Yetani::SurfaceSize
 *
 * \brief A collection of allocation size data.
 */


/**
 * \brief Update the size data.
 *
 * Using the provided parameters, update the various surface support structures 
 * to have the same size.
 */
void Yetani::surfaceCalculateSize(Yetani* yetani     ///< Yetani
	, struct wl_surface*              wl_surface ///< The Wayland surface
	, const Yetani::SizePixel&        size       ///< The new size
	) noexcept
{
	Yetani::SurfaceSize& surface_size = yetani->surface_size_map[wl_surface];
	surface_size.width    = size.width;
	surface_size.height   = size.height;
	surface_size.stride   = size.width * surface_size.bytes_per_pixel;
	surface_size.in_bytes = surface_size.stride * surface_size.height;

	Yetani::SurfaceFrame& surface_frame = yetani->surface_frame_map[wl_surface];
	surface_frame.width  = size.width;
	surface_frame.height = size.height;
}


/**
 * \brief Create a Wayland Surface
 *
 * \todo There are two ways to render buffers in a surface.
 *       1. Submit the buffer to the surface then wait for the buffer to be 
 *       released/returned.
 *       2. Give the buffer to the surface then destroy the buffer when it is 
 *       no longer needed.
 *       .
 *       These are mutually exclusive and should be determined by the "type" of 
 *       surface. Or the buffer itself should determine if it should be 
 *       destroyed or saved.
 *
 * Create a new Wayland surface and all other supporting data.
 *
 * \return A pointer to the Wayland Surface.
 */
struct wl_surface* Yetani::surfaceCreate(Yetani* yetani        ///< Yetani
	, const wl_shm_format                    pixel_format  ///< The pixel format
	, const Yetani::SizePixel&               size          ///< The surface size
	, Yetani::Window::Memory&                window_memory ///< The memory pool to use
	) noexcept
{
	struct wl_surface* wl_surface = wl_compositor_create_surface(yetani->compositor);

	Yetani::SurfaceSize& surface_size = yetani->surface_size_map[wl_surface];
	surface_size.pixel_format    = pixel_format;
	surface_size.bytes_per_pixel = shmFormatBytesPerPixel(pixel_format);

	Yetani::SurfaceFrame& surface_frame = yetani->surface_frame_map[wl_surface];
	surface_frame.wl_surface = wl_surface;
	surface_frame.time_ms    = 0;
	
	surfaceCalculateSize(yetani, wl_surface, size);

	surface_frame.buffer_next = bufferCreate(surface_size, &window_memory, &yetani->buffer);

	// A future configuration setting
	bool event_keyboard = true;
	if(event_keyboard)
	{
		yetani->keyboard.event_map[wl_surface] =
		{	.on_enter = Lambda_DoNothing
		,	.on_leave = Lambda_DoNothing
		,	.on_key   = LambdaKey_DoNothing
		};
	}

	// A future configuration setting
	bool event_pointer = true;
	if(event_pointer)
	{
		yetani->pointer.event_map[wl_surface] =
		{	.on_axis           = LambdaAxis_DoNothing
		,	.on_axis_discrete  = Lambda_DoNothing
		,	.on_axis_source    = Lambda_DoNothing
		,	.on_axis_stop      = Lambda_DoNothing
		,	.on_button_mm      = LambdaButtonMm_DoNothing
		,	.on_button_percent = LambdaButtonPercent_DoNothing
		,	.on_button_pixel   = LambdaButtonPixel_DoNothing
		,	.on_enter_mm       = LambdaPointMm_DoNothing
		,	.on_enter_percent  = LambdaPointPercent_DoNothing
		,	.on_enter_pixel    = LambdaPointPixel_DoNothing
		,	.on_leave          = Lambda_DoNothing
		,	.on_motion_mm      = LambdaPointMm_DoNothing
		,	.on_motion_percent = LambdaPointPercent_DoNothing
		,	.on_motion_pixel   = LambdaPointPixel_DoNothing
		};
	}

	yetani->surface_event_map[wl_surface] =
	{	.on_size_mm_change      = LambdaSizeMm_DoNothing
	,	.on_size_percent_change = LambdaSizePercent_DoNothing
	,	.on_size_pixel_change   = LambdaSizePixel_DoNothing
	};

	wl_surface_add_listener(wl_surface
		, &surface_listener
		, yetani
		);

	return wl_surface;
}


/**
 * \brief Destroy a surface.
 *
 * Destroy a Wayland Surface as well as all supporting data structures.
 */
void Yetani::surfaceDestroy(Yetani* yetani     ///< Yetani
	, struct wl_surface*&       wl_surface ///< The surface to destroy
	) noexcept
{
	if(wl_surface == nullptr)
	{
		return;
	}

	if(yetani->surface_frame_map.contains(wl_surface))
	{
		Yetani::SurfaceFrame& surface_frame = yetani->surface_frame_map[wl_surface];

		struct wl_buffer* wl_buffer = nullptr;
		wl_buffer = surface_frame.buffer_next.exchange(nullptr);

		if(wl_buffer != nullptr)
		{
			bufferDestroy(wl_buffer);
		}
	}

	if(yetani->keyboard.event_map.contains(wl_surface))
	{
		yetani->keyboard.event_map.erase(wl_surface);
	}

	if(yetani->pointer.event_map.contains(wl_surface))
	{
		yetani->pointer.event_map.erase(wl_surface);
	}

	yetani->surface_size_map.erase(wl_surface);
	yetani->surface_frame_map.erase(wl_surface);
	yetani->surface_event_map.erase(wl_surface);

	yetani->cursorDetach(wl_surface);

	wl_surface_destroy(wl_surface);
	wl_surface = nullptr;
}

// }}}
// {{{ Wayland : Listener Handlers : Buffer

/**
 * \brief Wayland is done with a wl_buffer.
 *
 * After a wl_buffer is attached to a wl_surface and that wl_surface has been 
 * commited, Wayland will call this method once it has finished using the 
 * wl_buffer.
 *
 * \note There is no guarantee that this method will be called in a timely 
 * manner.
 *
 * \thread_wayland
 */
void Yetani::handlerBufferRelease(void* //data
	, struct wl_buffer*             wl_buffer ///< The buffer that was released
	) noexcept
{
	Yetani::bufferDestroy(wl_buffer);
}

// }}}
// {{{ Wayland : Listener Handlers : Keyboard

/**
 * \brief Wayland Surface gains keyboard focus.
 */
void Yetani::handlerKeyboardEnter(void* data        ///< Keyboard pointer
	, struct wl_keyboard*        // wl_keyboard
	, uint32_t                   // serial
	, struct wl_surface*            wl_surface  ///< The Wayland surface
	, struct wl_array*              key_array   ///< Active keys
	) noexcept
{
	Yetani::Keyboard& keyboard = *((Yetani::Keyboard*)data);

	if(keyboard.wl_surface != nullptr)
	{
		keyboardRepeatReleaseAll(keyboard);

		keyboard.event->on_leave();
	}

	keyboard.wl_surface = wl_surface;

	if(keyboard.event_map.contains(wl_surface))
	{
		keyboard.event = &(keyboard.event_map[wl_surface]);
	}
	else
	{
		keyboard.event = &(keyboard.event_map[nullptr]);
	}

	keyboard.event->on_enter();

	if(key_array->size > 0)
	{
		Yetani::Key key =
		{	.time  = 0
		,	.code  = 0
		,	.state = Yetani::KeyState::Pressed
		};

		ZAKERO_YETANI__ARRAY_FOR_EACH(uint32_t*, key_iter, key_array)
		{
			key.code = *key_iter;

			keyboard.event->on_key(key, keyboard.modifier);

			keyboardRepeatAdd(keyboard, key.code, 0);
		}
	}
}


/**
 * \brief Key event.
 */
void Yetani::handlerKeyboardKey(void* data        ///< Keyboard pointer
	, struct wl_keyboard*      // wl_keyboard
	, uint32_t                 // serial
	, uint32_t                    time        ///< Event time
	, uint32_t                    key_code    ///< The key
	, uint32_t                    state       ///< The key state
	) noexcept
{
	Yetani::Keyboard& keyboard = *((Yetani::Keyboard*)data);

	Yetani::Key key =
	{	.time = time
	,	.code = key_code
	,	.state = (state == WL_KEYBOARD_KEY_STATE_PRESSED)
			? Yetani::KeyState::Pressed
			: Yetani::KeyState::Released
	};

	keyboard.event->on_key(key, keyboard.modifier);

	if(key.state == Yetani::KeyState::Pressed
		&& keyboard.repeat_rate > 0
		)
	{
		keyboardRepeatAdd(keyboard, key_code, time);
	}
	else if(key.state == Yetani::KeyState::Released)
	{
		keyboardRepeatRemove(keyboard, key_code);
	}
}


/**
 * \brief Notification of available keymap.
 */
void Yetani::handlerKeyboardKeymap(void* data        ///< Keyboard pointer
	, struct wl_keyboard*         // wl_keyboard
	, uint32_t                       format      ///< Keymap format
	, int32_t                        fd          ///< Location of the keymap
	, uint32_t                       size        ///< Keymap size (in bytes)
	) noexcept
{
	Yetani::Keyboard& keyboard = *((Yetani::Keyboard*)data);

	if(format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1)
	{
		if(keyboard.keymap != nullptr)
		{
			munmap(keyboard.keymap
				, keyboard.keymap_size
				);
		}

		keyboard.keymap = (char*)mmap(nullptr
			, size
			, PROT_READ
			, MAP_NORESERVE | MAP_PRIVATE
			, fd
			, 0
			);
		keyboard.keymap_size = size;
	}
	else // (format == WL_KEYBOARD_KEYMAP_FORMAT_NO_KEYMAP)
	{
		if(keyboard.keymap != nullptr)
		{
			munmap(keyboard.keymap
				, keyboard.keymap_size
				);

			keyboard.keymap      = nullptr;
			keyboard.keymap_size = 0;
		}
	}
}


/**
 * \brief Wayland Surface loses keyboard focus.
 */
void Yetani::handlerKeyboardLeave(void* data        ///< Keyboard pointer
	, struct wl_keyboard*        // wl_keyboard
	, uint32_t                   // serial
	, struct wl_surface*            wl_surface  ///< The Wayland surface
	) noexcept
{
	Yetani::Keyboard& keyboard = *((Yetani::Keyboard*)data);

	if(keyboard.wl_surface == wl_surface)
	{
		keyboardRepeatReleaseAll(keyboard);

		keyboard.event->on_leave();

		keyboard.event = &(keyboard.event_map[nullptr]);
		keyboard.wl_surface = nullptr;
	}
}


/**
 * \brief Key modifiers changed.
 */
void Yetani::handlerKeyboardModifiers(void* data         ///< Keyboard pointer
	, struct wl_keyboard*            // wl_keyboard
	, uint32_t                       // serial
	, uint32_t                          mods_pressed ///< Pressed modifiers
	, uint32_t                          mods_latched ///< Latched modifiers
	, uint32_t                          mods_locked  ///< Locked modifiers
	, uint32_t                          group        ///< Keyboard layout
	) noexcept
{
	Yetani::Keyboard& keyboard = *((Yetani::Keyboard*)data);

	keyboard.modifier.pressed = mods_pressed;
	keyboard.modifier.latched = mods_latched;
	keyboard.modifier.locked  = mods_locked;
	keyboard.modifier.group   = group;
}


/**
 * \brief Wayland key repeat information
 */
void Yetani::handlerKeyboardRepeatInfo(void* data        ///< Keyboard pointer
	, struct wl_keyboard*             // wl_keyboard
	, int32_t                            rate        ///< Repeat rate
	, int32_t                            delay       ///< Repeat delay
	) noexcept
{
	Yetani::Keyboard& keyboard = *((Yetani::Keyboard*)data);

	keyboard.repeat_delay = delay;
	keyboard.repeat_rate  = 1000 / rate;
}

// }}}
// {{{ Wayland : Listener Handlers : Output

/**
 * \brief Handle Wayland Output done events
 */
void Yetani::handlerOutputDone(void* data      ///< User data
	, struct wl_output*          wl_output ///< The output device
	) noexcept
{
	Yetani*          yetani    = (Yetani*)data;
	Yetani::Output&  output    = yetani->output_data.output_map[wl_output];
	Yetani::Output&  changes   = yetani->output_changes_map[wl_output];
	Yetani::OutputId output_id = yetani->output_data.wloutput_to_outputid[wl_output];

	/**
	 * \todo Test if the Output's width and height values need to be 
	 * swapped when the screen is rotated.
	 * \code
	switch(output.transform)
	{
		case WL_OUTPUT_TRANSFORM_NORMAL:
		case WL_OUTPUT_TRANSFORM_180:
		case WL_OUTPUT_TRANSFORM_FLIPPED:
		case WL_OUTPUT_TRANSFORM_FLIPPED_180:
			output.pixels_per_mm_horizontal = output.width / float(output.physical_width_mm);
			output.pixels_per_mm_vertical   = output.height / float(output.physical_height_mm);
			break;

		// If the output was rotated 90 or 270 degrees, the
		// pixels_per_mm width and height will need to be swapped
		case WL_OUTPUT_TRANSFORM_90:
		case WL_OUTPUT_TRANSFORM_270:
		case WL_OUTPUT_TRANSFORM_FLIPPED_90:
		case WL_OUTPUT_TRANSFORM_FLIPPED_270:
			output.pixels_per_mm_horizontal = output.height / float(output.physical_height_mm);
			output.pixels_per_mm_vertical   = output.width / float(output.physical_width_mm);
			break;
	}
	 * \endcode
	 */

	output = changes;
	output.pixels_per_mm_horizontal = output.width / float(output.physical_width_mm);
	output.pixels_per_mm_vertical   = output.height / float(output.physical_height_mm);

	ZAKERO_YETANI__DEBUG << "\n" << to_string(output) << "\n";

	switch(yetani->output_state_map[wl_output])
	{
		case Yetani::OutputState::Done:
			// Do Nothing
			break;

		case Yetani::OutputState::Added:
			yetani->on_output_add(output_id);
			break;

		case Yetani::OutputState::Changed:
			yetani->on_output_change(output_id);

			for(auto wl_surface : yetani->output_notify_surface_vector)
			{
				outputNotifySurface(yetani, wl_output, wl_surface);
			}

			break;
	}

	yetani->output_state_map[wl_output] = Yetani::OutputState::Done;
}


/**
 * \brief Handle Wayland Output geometry events
 */
void Yetani::handlerOutputGeometry(void* data            ///< User data
	, struct wl_output*              wl_output       ///< The output device
	, int32_t                        x               ///< X position within the global compositor
	, int32_t                        y               ///< Y position within the global compositor
	, int32_t                        physical_width  ///< Width in millimeters of the output
	, int32_t                        physical_height ///< Height in millimeters of the output
	, int32_t                        subpixel        ///< Subpixel orientation of the output
	, const char*                    make            ///< Description of the manufacturer
	, const char*                    model           ///< Description of the model
	, int32_t                        transform       ///< Transform that maps framebuffer to output
	) noexcept
{
	Yetani*         yetani         = (Yetani*)data;
	Yetani::Output& output_changes = yetani->output_changes_map[wl_output];

	if(yetani->output_state_map[wl_output] != Yetani::OutputState::Added)
	{
		yetani->output_state_map[wl_output] = Yetani::OutputState::Changed;
	}

	output_changes.x                  = x;
	output_changes.y                  = y;
	output_changes.physical_width_mm  = physical_width;
	output_changes.physical_height_mm = physical_height;
	output_changes.subpixel           = subpixel;
	output_changes.make               = std::string(make);
	output_changes.model              = std::string(model);
	output_changes.transform          = transform;
}


/**
 * \brief Handle Wayland Output mode events
 */
void Yetani::handlerOutputMode(void* data      ///< User data
	, struct wl_output*          wl_output ///< The output device
	, uint32_t                   flags     ///< Bitfield of mode flags
	, int32_t                    width     ///< Width of the mode in hardware units
	, int32_t                    height    ///< Height of the mode in hardware units
	, int32_t                    refresh   ///< Vertical refresh rate
	) noexcept
{
	Yetani*         yetani         = (Yetani*)data;
	Yetani::Output& output_changes = yetani->output_changes_map[wl_output];

	if(yetani->output_state_map[wl_output] != Yetani::OutputState::Added)
	{
		yetani->output_state_map[wl_output] = Yetani::OutputState::Changed;
	}

	output_changes.flags       = flags;
	output_changes.width       = width;
	output_changes.height      = height;
	output_changes.refresh_mHz = refresh;
}


/**
 * \brief Handle Wayland Output scale events
 */
void Yetani::handlerOutputScale(void* data      ///< User data
	, struct wl_output*           wl_output ///< The output device
	, int32_t                     factor    ///< The scaling factor
	) noexcept
{
	Yetani* yetani         = (Yetani*)data;
	Output& output_changes = yetani->output_changes_map[wl_output];

	if(yetani->output_state_map[wl_output] != Yetani::OutputState::Added)
	{
		yetani->output_state_map[wl_output] = Yetani::OutputState::Changed;
	}

	output_changes.scale_factor = factor;
}

// }}}
// {{{ Wayland : Listener Handlers : Pointer

/**
 * \brief Handle Wayland Pointer Axis events
 */
void Yetani::handlerPointerAxis(void* data       ///< Handler data
	, struct wl_pointer*          //wl_pointer ///< The Wayland Pointer
	, uint32_t                    time       ///< Event time
	, uint32_t                    axis       ///< The axis
	, wl_fixed_t                  value      ///< Axis movement
	) noexcept
{
	Yetani::Pointer& pointer = *((Yetani::Pointer*)data);

	pointer.axis.time     = time;
	pointer.axis.distance = (float)wl_fixed_to_double(value);

	if(axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL)
	{
		pointer.axis.type = Yetani::PointerAxisType::Horizontal;
	}
	else if(axis == WL_POINTER_AXIS_VERTICAL_SCROLL)
	{
		pointer.axis.type = Yetani::PointerAxisType::Vertical;
	}
	else
	{
		pointer.axis.type = Yetani::PointerAxisType::Unknown;
	}
}


/**
 * \brief Handle Wayland Pointer Axis events
 */
void Yetani::handlerPointerAxisDiscrete(void* data       ///< Handler data
	, struct wl_pointer*                  //wl_pointer ///< The Wayland Pointer
	, uint32_t                            //axis       ///< The axis
	, int32_t                             discrete   ///< The steps
	) noexcept
{
	Yetani::Pointer& pointer = *((Yetani::Pointer*)data);

	pointer.axis.steps = discrete;
}


/**
 * \brief Handle Wayland Pointer Axis events
 */
void Yetani::handlerPointerAxisSource(void* data        ///< Handler data
	, struct wl_pointer*                //wl_pointer  ///< The Wayland Pointer
	, uint32_t                          axis_source ///< The source of the event
	) noexcept
{
	Yetani::Pointer& pointer = *((Yetani::Pointer*)data);

	switch(axis_source)
	{
		case WL_POINTER_AXIS_SOURCE_WHEEL:
			pointer.axis.source = Yetani::PointerAxisSource::Wheel;
			break;

		case WL_POINTER_AXIS_SOURCE_FINGER:
			pointer.axis.source = Yetani::PointerAxisSource::Finger;
			break;

		case WL_POINTER_AXIS_SOURCE_CONTINUOUS:
			pointer.axis.source = Yetani::PointerAxisSource::Continuous;
			break;

		case WL_POINTER_AXIS_SOURCE_WHEEL_TILT:
			pointer.axis.source = Yetani::PointerAxisSource::Wheel_Tilt;
			break;

		default:
			pointer.axis.source = Yetani::PointerAxisSource::Unknown;
	}
}


/**
 * \brief Handle Wayland Pointer Axis events
 */
void Yetani::handlerPointerAxisStop(void*
	, struct wl_pointer*
	, uint32_t
	, uint32_t
	) noexcept
{
	// --- Ignored ---
	//
	// Wayland documentation suggests that the "axis stop" can be used for
	// kinetic scrolling or to determine when one axis motions begins anew.
	// 
	// This is not needed.
	// - For kinetic scrolling: Have the default scrolling be kinetic and
	//   treat axis input as a new (or additive) velocity.
	// - For separation of axis motion, why?  If there is a pause in the
	//   motion, that reflects the user input.
}


/**
 * \brief Handle Wayland Pointer Button events
 */
void Yetani::handlerPointerButton(void* data       ///< Handler data
	, struct wl_pointer*            //wl_pointer ///< The Wayland Pointer
	, uint32_t                      //serial     ///< The Event Id
	, uint32_t                      time       ///< Event time
	, uint32_t                      button     ///< The button
	, uint32_t                      state      ///< The button state
	) noexcept
{
	Yetani::Pointer& pointer = *((Yetani::Pointer*)data);

	pointer.button.code = button;
	pointer.button_time = time;

	if(state == WL_POINTER_BUTTON_STATE_RELEASED)
	{
		pointer.button.state = Yetani::PointerButtonState::Released;
	}
	else if(state == WL_POINTER_BUTTON_STATE_PRESSED)
	{
		pointer.button.state = Yetani::PointerButtonState::Pressed;
	}
}


/**
 * \brief Handle Wayland Pointer Enter events
 */
void Yetani::handlerPointerEnter(void* data       ///< Handler data
	, struct wl_pointer*           //wl_pointer ///< The Wayland Pointer
	, uint32_t                     serial     ///< Event Id
	, struct wl_surface*           wl_surface ///< The entered surface
	, wl_fixed_t                   surface_x  ///< The surface location
	, wl_fixed_t                   surface_y  ///< The surface location
	) noexcept
{
	Yetani::Pointer& pointer = *((Yetani::Pointer*)data);

	pointer.enter_serial  = serial;
	pointer.enter_surface = wl_surface;

	pointer.enter_point =
	{	.time = 0
	,	.x    = wl_fixed_to_int(surface_x)
	,	.y    = wl_fixed_to_int(surface_y)
	};
}


/**
 * \brief Handle Wayland Pointer Frame events
 */
void Yetani::handlerPointerFrame(void* data       ///< Handler data
	, struct wl_pointer*           wl_pointer ///< The Wayland Pointer
	) noexcept
{
	Yetani::Pointer& pointer = *((Yetani::Pointer*)data);
	Yetani*          yetani  = pointer.yetani;

	if(pointer.enter_surface != nullptr)
	{
		if(pointer.wl_surface != nullptr)
		{
			yetani->cursorLeave(pointer.wl_surface);
			pointer.event_map[pointer.wl_surface].on_leave();
		}

		yetani->cursorEnter(wl_pointer
			, pointer.enter_serial
			, pointer.enter_surface
			);

		pointer.wl_surface  = pointer.enter_surface;
		pointer.point_pixel = pointer.enter_point;

		if(pointer.event_map.contains(pointer.wl_surface))
		{
			pointer.event = &(pointer.event_map[pointer.wl_surface]);
		}
		else
		{
			pointer.event = &(pointer.event_map[nullptr]);
		}

		yetani->convertPixel(pointer.enter_surface
			, pointer.point_pixel.x  , pointer.point_pixel.y
			, pointer.point_mm.x     , pointer.point_mm.y
			, pointer.point_percent.x, pointer.point_percent.y
			);

		pointer.event->on_enter_pixel(pointer.point_pixel
			, yetani->keyboard.modifier
			);
		pointer.event->on_enter_mm(pointer.point_mm
			, yetani->keyboard.modifier
			);
		pointer.event->on_enter_percent(pointer.point_percent
			, yetani->keyboard.modifier
			);
	}

	if((pointer.motion_point.time != 0)
		&& (pointer.wl_surface != nullptr)
		)
	{
		pointer.point_pixel = pointer.motion_point;

		yetani->convertPixel(pointer.wl_surface
			, pointer.point_pixel.x  , pointer.point_pixel.y
			, pointer.point_mm.x     , pointer.point_mm.y
			, pointer.point_percent.x, pointer.point_percent.y
			);
		pointer.point_mm.time      = pointer.point_pixel.time;
		pointer.point_percent.time = pointer.point_pixel.time;

		pointer.event->on_motion_pixel(pointer.point_pixel
			, yetani->keyboard.modifier
			);
		pointer.event->on_motion_mm(pointer.point_mm
			, yetani->keyboard.modifier
			);
		pointer.event->on_motion_percent(pointer.point_percent
			, yetani->keyboard.modifier
			);
	}

	if((pointer.button_time != 0)
		&& (pointer.wl_surface != nullptr)
		)
	{
		pointer.point_mm.time      = pointer.button_time;
		pointer.point_percent.time = pointer.button_time;
		pointer.point_pixel.time   = pointer.button_time;

		pointer.event->on_button_pixel(pointer.button
			, pointer.point_pixel
			, yetani->keyboard.modifier
			);
		pointer.event->on_button_mm(pointer.button
			, pointer.point_mm
			, yetani->keyboard.modifier
			);
		pointer.event->on_button_percent(pointer.button
			, pointer.point_percent
			, yetani->keyboard.modifier
			);
	}

	if((pointer.axis.time != 0)
		&& (pointer.wl_surface != nullptr)
		)
	{
		pointer.event->on_axis(pointer.axis
			, yetani->keyboard.modifier
			);
	}

	if((pointer.leave_surface != nullptr)
		&& (pointer.leave_surface == pointer.wl_surface)
		)
	{
		yetani->cursorLeave(pointer.leave_surface);

		pointer.event->on_leave();

		pointer.event = &(pointer.event_map[nullptr]);
		pointer.wl_surface = nullptr;
	}

	pointerClear(pointer);
}


/**
 * \brief Handle Wayland Pointer Leave events
 */
void Yetani::handlerPointerLeave(void* data       ///< Handler data
	, struct wl_pointer*           //wl_pointer ///< The Wayland Pointer
	, uint32_t                     //serial     ///< The Event Id
	, struct wl_surface*           wl_surface ///< The Wayland Surface
	) noexcept
{
	Yetani::Pointer& pointer = *((Yetani::Pointer*)data);

	pointer.leave_surface = wl_surface;
}


/**
 * \brief Handle Wayland Pointer Motion events
 */
void Yetani::handlerPointerMotion(void* data       ///< Handler data
	, struct wl_pointer*            //wl_pointer ///< The Wayland Pointer
	, uint32_t                      time       ///< The Event Time
	, wl_fixed_t                    surface_x  ///< The surface location
	, wl_fixed_t                    surface_y  ///< The surface location
	) noexcept
{
	Yetani::Pointer& pointer = *((Yetani::Pointer*)data);

	pointer.motion_point =
	{	.time = time
	,	.x    = wl_fixed_to_int(surface_x)
	,	.y    = wl_fixed_to_int(surface_y)
	};
}

// }}}
// {{{ Wayland : Listener Handlers : Registry

/**
 * \brief Handle Wayland Registry Global events
 */
void Yetani::handlerRegistryGlobal(void* data      ///< User data
	, struct wl_registry*            registry  ///< The registry object
	, uint32_t                       id        ///< The ID of the thing
	, const char*                    interface ///< The name of the thing
	, uint32_t                       version   ///< The version of the thing
	) noexcept
{
	Yetani* yetani = (Yetani*)data;

	std::string_view interface_name(interface);
	ZAKERO_YETANI__DEBUG_VAR(interface_name);

	if(interface_name == wl_compositor_interface.name)
	{
		yetani->compositor = (struct wl_compositor*)
			wl_registry_bind(registry
				, id
				, &wl_compositor_interface
				, 1
				);

		return;
	}

	if(interface_name == wl_output_interface.name)
	{
		struct wl_output* wl_output = (struct wl_output*)wl_registry_bind(registry
			, id
			, &wl_output_interface
			, 2
			);

		yetani->output_data.wloutput_to_outputid[wl_output] = id;
		yetani->output_data.outputid_to_wloutput[id]        = wl_output;
		yetani->output_data.output_map[wl_output]           = {};
		yetani->output_changes_map[wl_output]               = {};
		yetani->output_state_map[wl_output]                 = Yetani::OutputState::Added;

		wl_output_add_listener(wl_output
			, &Yetani::output_listener
			, yetani
			);

		return;
	}

	if(interface_name == wl_seat_interface.name)
	{
		yetani->seat = (struct wl_seat*)
			wl_registry_bind(registry
				, id
				, &wl_seat_interface
				, version
				);

		yetani->id_to_seat[id] = yetani->seat;

		wl_seat_add_listener(yetani->seat
			, &yetani->seat_listener
			, yetani
			);

		return;
	}

	if(interface_name == wl_shm_interface.name)
	{
		yetani->shm = (struct wl_shm*)
			wl_registry_bind(registry
				, id
				, &wl_shm_interface
				, version
				);

		wl_shm_add_listener(yetani->shm
			, &yetani->shm_listener
			, yetani
			);

		return;
	}

	if(interface_name == xdg_wm_base_interface.name)
	{
		yetani->xdg_wm_base = (struct xdg_wm_base*)
			wl_registry_bind(registry
				, id
				, &xdg_wm_base_interface
				, 1
				);

		xdg_wm_base_add_listener(yetani->xdg_wm_base
			, &yetani->xdg_wm_base_listener
			, yetani
			);
	}

	if(interface_name == zxdg_decoration_manager_v1_interface.name)
	{
		yetani->decoration_manager = (struct zxdg_decoration_manager_v1*)
			wl_registry_bind(registry
				, id
				, &zxdg_decoration_manager_v1_interface
				, 1
				);
		ZAKERO_YETANI__DEBUG << "--- Using UNSTABLE Decoration Manager ---\n";
	}
}


/**
 * \brief Handle Wayland Registry Global events
 */
void Yetani::handlerRegistryRemove(void* data     ///< User data
	, struct wl_registry*            //registry ///< The registry object
	, uint32_t                       id       ///< The ID of the thing
	) noexcept
{
	Yetani* yetani = (Yetani*)data;

	printf("Got a registry remove event for id %d\n", id);

	// Output
	{
		std::lock_guard<std::mutex> lock(yetani->output_data.mutex);

		if(yetani->output_data.outputid_to_wloutput.contains(id))
		{
			struct wl_output* wl_output = yetani->output_data.outputid_to_wloutput[id];

			yetani->output_data.outputid_to_wloutput.erase(id);
			yetani->output_data.wloutput_to_outputid.erase(wl_output);

			yetani->output_changes_map.erase(wl_output);
			yetani->output_state_map.erase(wl_output);

			yetani->on_output_remove(id);
			yetani->output_data.output_map.erase(wl_output);

			return;
		}
	}

	// Seat
	{
		if(yetani->id_to_seat.contains(id))
		{
			struct wl_seat* wl_seat = yetani->id_to_seat[id];

			yetani->seatDestroy(wl_seat);

			yetani->id_to_seat.erase(id);
		}
	}
}

// }}}
// {{{ Wayland : Listener Handlers : Seat

/**
 * \brief Handle Wayland Seat notifications
 *
 * \todo Add "bool Yetani::keyboardAvailable() const"
 * \todo Add "bool Yetani::pointerAvailable() const"
 * \todo Add "bool Yetani::touchAvailable() const"
 */
void Yetani::handlerSeatCapabilities(void* data         ///< Handler data
	, struct wl_seat*                  wl_seat      ///< The Wayland Seat
	, uint32_t                         capabilities ///< Capability flags
	) noexcept
{
	ZAKERO_YETANI__DEBUG_VAR((uint64_t)wl_seat);

	Yetani*       yetani = (Yetani*)data;
	Yetani::Seat& seat   = yetani->seat_map[wl_seat];

	seat.version = wl_seat_get_version(wl_seat);

	if(capabilities & WL_SEAT_CAPABILITY_KEYBOARD)
	{
		ZAKERO_YETANI__DEBUG << "-- Got a keyboard device --\n";

		yetani->keyboard.event_map[nullptr] =
		{	.on_enter = Lambda_DoNothing
		,	.on_leave = Lambda_DoNothing
		,	.on_key   = LambdaKey_DoNothing
		};

		yetani->keyboard.event = &(yetani->keyboard.event_map[nullptr]);

		seat.wl_keyboard = wl_seat_get_keyboard(wl_seat);

		wl_keyboard_add_listener(seat.wl_keyboard
			, &Yetani::keyboard_listener
			, &yetani->keyboard
			);
	}

	if(capabilities & WL_SEAT_CAPABILITY_POINTER)
	{
		ZAKERO_YETANI__DEBUG << "-- Got a pointer device --\n";

		yetani->pointer.yetani = yetani;

		yetani->pointer.event_map[nullptr] =
		{	.on_axis           = LambdaAxis_DoNothing
		,	.on_axis_discrete  = Lambda_DoNothing
		,	.on_axis_source    = Lambda_DoNothing
		,	.on_axis_stop      = Lambda_DoNothing
		,	.on_button_mm      = LambdaButtonMm_DoNothing
		,	.on_button_percent = LambdaButtonPercent_DoNothing
		,	.on_button_pixel   = LambdaButtonPixel_DoNothing
		,	.on_enter_mm       = LambdaPointMm_DoNothing
		,	.on_enter_percent  = LambdaPointPercent_DoNothing
		,	.on_enter_pixel    = LambdaPointPixel_DoNothing
		,	.on_leave          = Lambda_DoNothing
		,	.on_motion_mm      = LambdaPointMm_DoNothing
		,	.on_motion_percent = LambdaPointPercent_DoNothing
		,	.on_motion_pixel   = LambdaPointPixel_DoNothing
		};

		yetani->pointer.event = &(yetani->pointer.event_map[nullptr]);

		seat.wl_pointer = wl_seat_get_pointer(wl_seat);

		wl_pointer_add_listener(seat.wl_pointer
			, &Yetani::pointer_listener
			, &yetani->pointer
			);
	}

	if(capabilities & WL_SEAT_CAPABILITY_TOUCH)
	{
		ZAKERO_YETANI__DEBUG << "-- Got a touch device --\n";
		seat.wl_touch = wl_seat_get_touch(wl_seat);

		//wl_touch_add_listener(seat.wl_touch
		//	, &Yetani::touch_listener
		//	, data
		//	);
	}
}


/**
 * \brief Handle Seat name notifications.
 */
void Yetani::handlerSeatName(void* data    ///< Handler data
	, struct wl_seat*          wl_seat ///< The Wayland Seat
	, const char*              name    ///< The name of the Seat
	) noexcept
{
	ZAKERO_YETANI__DEBUG_VAR((uint64_t)wl_seat);
	ZAKERO_YETANI__DEBUG_VAR(name);

	Yetani* yetani = (Yetani*)data;

	yetani->seat_map[wl_seat].name = name;
}

// }}}
// {{{ Wayland : Listener Handlers : SHM

/**
 * \brief Handle Wayland Registry Global events
 */
void Yetani::handlerShmFormat(void* data ///< User data
	, struct wl_shm*  /* Unused */
	, uint32_t value                 ///< The pixel format value
	) noexcept
{
	Yetani* yetani = (Yetani*)data;

	wl_shm_format format = (wl_shm_format)value;

	if(vectorContains(yetani->shm_format_vector, format))
	{
		return;
	}

	yetani->shm_format_vector.push_back(format);
}

// }}}
// {{{ Wayland : Listener Handlers : Surface

/**
 * \brief Handle Wayland Surface enter events
 */
void Yetani::handlerSurfaceEnter(void* data       ///< User data
	, struct wl_surface*           wl_surface ///< The surface
	, struct wl_output*            wl_output  ///< The output
	) noexcept
{
	//ZAKERO_YETANI__DEBUG << "-------------------------------------------\n";
	//ZAKERO_YETANI__DEBUG_VAR(data);
	//ZAKERO_YETANI__DEBUG_VAR(wl_surface);
	//ZAKERO_YETANI__DEBUG_VAR(wl_output);

	Yetani* yetani = (Yetani*)data;
	Yetani::OutputData& output_data = yetani->output_data;

	output_data.mutex.lock();
	output_data.surface_output_map[wl_surface].push_back(wl_output);
	output_data.mutex.unlock();
}


/**
 * \brief Handle Wayland Surface Leave events
 */
void Yetani::handlerSurfaceLeave(void* data       ///< User data
	, struct wl_surface*           wl_surface ///< The surface
	, struct wl_output*            wl_output  ///< The wayland_output
	) noexcept
{
	//ZAKERO_YETANI__DEBUG << "-------------------------------------------\n";
	//ZAKERO_YETANI__DEBUG_VAR(data);
	//ZAKERO_YETANI__DEBUG_VAR(wl_surface);
	//ZAKERO_YETANI__DEBUG_VAR(wl_output);

	Yetani* yetani = (Yetani*)data;

	Yetani::OutputData& output_data = yetani->output_data;
	std::lock_guard<std::mutex> lock(output_data.mutex);

	Yetani::VectorWlOutput& output_vector = output_data.surface_output_map[wl_surface];

	// Save the current wl_output device id
	struct wl_output* current_output = output_vector.front();

	// Remove the wl_output device id
	zakero::vectorErase(output_vector, wl_output);

	// Check if the current wl_output device id was removed
	if(current_output == output_vector.front())
	{
		// Current wl_output device was not removed
		// so nothing to do.

		return;
	}

	Yetani::SurfaceExtent& surface_extent = yetani->surface_extent_map[wl_surface];

	if(surface_extent.preferred_unit == Yetani::SizeUnit::Pixel)
	{
		return;
	}

	// Convert the relative window size from the old wl_output device
	// to the new wl_output device.
	
	current_output = output_vector.front();

	Yetani::Output&   output   = output_data.output_map.at(current_output);
	Yetani::SizePixel new_size;

	if(surface_extent.preferred_unit == Yetani::SizeUnit::Millimeter)
	{
		auto p = yetani->convertMmToPixel(output
			, surface_extent.size_mm.width
			, surface_extent.size_mm.height
			);

		new_size = { p.first, p.second };
	}
	else // if(surface_extent.preferred_unit == Yetani::SizeUnit::Percent)
	{
		auto p = yetani->convertPercentToPixel(output
			, surface_extent.size_percent.width
			, surface_extent.size_percent.height
			);

		new_size = { p.first, p.second };
	}

	if(new_size.width <= 0)
	{
		new_size.width = 1;
	}

	if(new_size.height <= 0)
	{
		new_size.height = 1;
	}

	if((new_size.width != surface_extent.size_pixel.width)
		&& (new_size.height != surface_extent.size_pixel.height)
		)
	{
		yetani->surface_resize_mutex_map[wl_surface].lock();
		{
			XdgSurface& surface = yetani->xdg_surface_map[wl_surface];

			surface_extent.size_pixel = new_size;
			surfaceCalculateSize(surface.yetani, surface.wl_surface, new_size);
		}
		yetani->surface_resize_mutex_map[wl_surface].unlock();

		Yetani::SurfaceEvent& event = yetani->surface_event_map[wl_surface];
		event.on_size_pixel_change(surface_extent.size_pixel);
	}
}

// }}}
// {{{ Wayland : Listener Handlers : SwapBuffers

/**
 * \brief Handle Wayland Registry Global events
 */
void Yetani::handlerSwapBuffers(void* data     ///< User data
	, struct wl_callback*         callback ///< The triggering callback
	, uint32_t                    time_ms  ///< The time
	) noexcept
{
	Yetani::SurfaceFrame* surface_frame = (Yetani::SurfaceFrame*)data;

	wl_callback_destroy(callback);

	callback = wl_surface_frame(surface_frame->wl_surface);

	wl_callback_add_listener(callback
		, &frame_callback_listener
		, data
		);

	struct wl_buffer* wl_buffer = surface_frame->buffer_next.exchange(nullptr);
	if(wl_buffer != nullptr)
	{
		wl_buffer_add_listener(wl_buffer
			, &Yetani::buffer_listener
			, wl_buffer_get_user_data(wl_buffer)
			);

		surface_frame->time_ms = time_ms;

		wl_surface_attach(surface_frame->wl_surface, wl_buffer, 0, 0);

		wl_surface_damage(surface_frame->wl_surface
			, 0, 0
			, surface_frame->width, surface_frame->height
			);
	}

	wl_surface_commit(surface_frame->wl_surface);
}

// }}}
// {{{ Wayland : Listener Handlers (Unstable)


// }}}
// {{{ XDG

/**
 * \enum Yetani::XdgState
 *
 * \brief XDG State IDs
 */


/**
 * \brief Convert XdgState to WindowMode
 *
 * \return The Yetani::WindowMode
 */
Yetani::WindowMode Yetani::toWindowMode(const Yetani::XdgState state ///< The XdgState to convert
	) noexcept
{
	switch(state)
	{
		case Yetani::XdgState::Toplevel_Window_Fullscreen:
			return Yetani::WindowMode::Fullscreen;

		case Yetani::XdgState::Toplevel_Window_Maximized:
			return Yetani::WindowMode::Maximized;

		default:
		case Yetani::XdgState::Toplevel_Window_Normal:
			return Yetani::WindowMode::Normal;
	}
}


/**
 * \brief Convert WindowMode to XdgState
 *
 * \return The Yetani::XdgState
 */
Yetani::XdgState Yetani::toXdgState(const Yetani::WindowMode window_mode ///< The WindowMode to convert
	) noexcept
{
	switch(window_mode)
	{
		case Yetani::WindowMode::Fullscreen:
			return Yetani::XdgState::Toplevel_Window_Fullscreen;

		case Yetani::WindowMode::Maximized:
			return Yetani::XdgState::Toplevel_Window_Maximized;

		default:
		case Yetani::WindowMode::Normal:
			return Yetani::XdgState::Toplevel_Window_Normal;
	}
}

// }}}
// {{{ XDG : Surface

/**
 * \struct Yetani::XdgSurface
 *
 * \brief XDG Surface data.
 *
 * \var Yetani::XdgSurface::yetani
 * \brief The owning Yetani instance
 *
 * \var Yetani::XdgSurface::wl_surface
 * \brief The Wayland Surface
 */


/**
 * \brief Create an XdgSurface.
 *
 * Use the provided \p wl_surface as the basis for creating a XDG Surface.
 *
 * \return A xdg_surface pointer.
 */
struct xdg_surface* Yetani::xdgSurfaceCreate(struct wl_surface* wl_surface ///< The Wayland Surface
	) noexcept
{
	XdgSurface& surface = xdg_surface_map[wl_surface];

	surface.yetani     = this;
	surface.wl_surface = wl_surface;

	surface_extent_mutex.lock();
	{
		Yetani::SurfaceExtent& surface_extent = surface_extent_map[wl_surface];

		surface_extent.preferred_unit    = Yetani::SizeUnit::Pixel;
		surface_extent.preferred_mm      = { 160, 90 };    // 16:9 * 10mm
		surface_extent.preferred_percent = { 0.32, 0.18 }; // 16:9 * 0.02
		surface_extent.size_mm           = { 160, 90 };    // 16:9 + 10mm
		surface_extent.size_percent      = { 0.32, 0.18 }; // 16:9 * 0.02
		surface_extent.size_pixel        = { 800, 450 };   // 16:9 * 50 pixels
		surface_extent.size_pixel_max    = { 0, 0 };       // No maximum size
		surface_extent.size_pixel_min    = { 0, 0 };       // No minimum size
	}
	surface_extent_mutex.unlock();

	output_notify_surface_vector.push_back(wl_surface);

	struct xdg_surface* xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base
		, wl_surface
		);

	xdg_state_change_mutex.lock();
	{
		xdg_state_change_map[xdg_surface] = {};
	}
	xdg_state_change_mutex.unlock();

	xdg_surface_add_listener(xdg_surface
		, &xdg_surface_listener
		, &surface
		);

	return xdg_surface;
}


/**
 * \brief Destroy the XDG Surface.
 *
 * In addition to destroying the \p xdg_surface, all associated structures will 
 * also be destroyed.
 *
 * The foundational \p wl_surface will __not__ be destroyed.
 */
void Yetani::xdgSurfaceDestroy(struct wl_surface* wl_surface  ///< The Wayland Surface
	, struct xdg_surface*&                    xdg_surface ///< The XDG Surface
	) noexcept
{
	if(xdg_surface)
	{
		xdg_surface_destroy(xdg_surface);
	}

	zakero::vectorErase(output_notify_surface_vector, wl_surface);

	if(xdg_surface_map.contains(wl_surface))
	{
		xdg_surface_map.erase(wl_surface);
	}

	xdg_state_change_mutex.lock();
	{
		if(xdg_state_change_map.contains(xdg_surface))
		{
			xdg_state_change_map.erase(xdg_surface);
		}
	}
	xdg_state_change_mutex.unlock();

	surface_extent_mutex.lock();
	{
		if(surface_extent_map.contains(wl_surface))
		{
			surface_extent_map.erase(wl_surface);
		}
	}
	surface_extent_mutex.unlock();

	xdg_surface = nullptr;
}


/**
 * \brief Initialize and set all possible sizes.
 */
void Yetani::xdgSurfaceSetExtent(struct wl_surface* wl_surface   ///< The Surface
	, const Yetani::SizeUnit&                   size_unit    ///< The preferred unit
	, const Yetani::SizeMm&                     size_mm      ///< The size in millimeters
	, const Yetani::SizePercent&                size_percent ///< The size as a percentage
	, const Yetani::SizePixel&                  size_pixel   ///< The size in pixels
	) noexcept
{
	Yetani::SurfaceExtent& surface_extent = surface_extent_map[wl_surface];

	surface_extent.preferred_unit    = size_unit;
	surface_extent.preferred_mm      = size_mm;
	surface_extent.preferred_percent = size_percent;
	surface_extent.size_mm           = size_mm;
	surface_extent.size_percent      = size_percent;
	surface_extent.size_pixel        = size_pixel;
	surface_extent.size_pixel_max    = { 0, 0 };
	surface_extent.size_pixel_min    = { 0, 0 };
}

// }}}
// {{{ XDG : Toplevel

/**
 * \struct Yetani::XdgToplevel
 *
 * \brief XDG Toplevel data.
 *
 * \var Yetani::XdgToplevel::state_change
 * \brief A pointer to the XDG Surface byte-code buffer
 *
 * \var Yetani::XdgToplevel::close_request_lambda
 * \brief An event handler
 *
 * \var Yetani::XdgToplevel::is_active_lambda
 * \brief An event handler
 *
 * \var Yetani::XdgToplevel::is_active
 * \brief A flag to indicate that the surface is active
 *
 * \var Yetani::XdgToplevel::window_state
 * \brief The current state
 *
 * \var Yetani::XdgToplevel::window_state_lambda
 * \brief An event handler
 *
 * \var Yetani::XdgToplevel::previous_size
 * \brief Save the window size to restore later
 *
 * \var Yetani::XdgToplevel::xdg_toplevel
 * \brief The XDG Toplevel
 */


/**
 * \brief Create a XDG Toplevel.
 *
 * Create a new XDG Toplevel using \p xdg_surface as the basis.
 *
 * \return A xdg_toplevel pointer.
 */
struct xdg_toplevel* Yetani::xdgToplevelCreate(struct xdg_surface* xdg_surface ///< The XDG Surface
	) noexcept
{
	Yetani::XdgToplevel& toplevel = xdg_toplevel_map[xdg_surface];

	toplevel.close_request_lambda = Lambda_DoNothing;
	toplevel.state_change         = &(xdg_state_change_map[xdg_surface]);
	toplevel.is_active            = false;
	toplevel.window_state         = Yetani::XdgState::Toplevel_Window_Normal;
	toplevel.is_active_lambda     = LambdaBool_DoNothing;
	toplevel.window_state_lambda  = LambdaWindowMode_DoNothing;
	toplevel.previous_size        = { 0, 0 };
	toplevel.xdg_toplevel         = nullptr;

	/**
	 * \note xdg_toplevel requires that no buffers can be attached to the 
	 * surface until __after__ the first configure event.  So, add the 
	 * "Attach Buffer" to the _state change_ vector.
	 */
	toplevel.state_change->push_back(Yetani::XdgState::Toplevel_Attach_Buffer);

	struct xdg_toplevel* xdg_toplevel = nullptr;

	xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
	toplevel.xdg_toplevel = xdg_toplevel;

	xdg_toplevel_add_listener(xdg_toplevel
		, &xdg_toplevel_listener
		, &toplevel
		);

	return xdg_toplevel;
}


/**
 * \brief Destroy a XDG Toplevel.
 *
 * The \p xdg_toplevel and associated data will be destroyed.  However, the \p 
 * xdg_surface will __not__ be destroyed.
 */
void Yetani::xdgToplevelDestroy(struct xdg_surface* xdg_surface  ///< The XDG Surface
	, struct xdg_toplevel*&                     xdg_toplevel ///< The XDG Toplevel
	) noexcept
{
	if(xdg_toplevel != nullptr)
	{
		xdg_toplevel_destroy(xdg_toplevel);
	}

	if(xdg_toplevel_map.contains(xdg_surface))
	{
		XdgToplevel& toplevel = xdg_toplevel_map[xdg_surface];

		toplevel.close_request_lambda = nullptr;
		toplevel.state_change         = nullptr;
		toplevel.window_state         = Yetani::XdgState::Unknown;
		toplevel.is_active            = false;
		toplevel.is_active_lambda     = nullptr;
		toplevel.window_state_lambda  = nullptr;

		xdg_toplevel_map.erase(xdg_surface);
	}

	xdg_toplevel = nullptr;
}


/**
 * \brief Update the size of the XDG Toplevel.
 *
 * To change the size of a XDG Toplevel, the underling Wayland Surface must be 
 * resized.  (XDG Toplevel is based on a XDG Surface which is based on a 
 * Wayland Surface.)
 *
 * After changing the size trigger the `on_size_*` events.
 */
void Yetani::xdgToplevelSizeChange(Yetani* yetani     ///< Yetani
	, struct wl_surface*               wl_surface ///< The Wayland Surface
	, const Yetani::SizePixel&         size_pixel ///< The new size
	) noexcept
{
	Yetani::SurfaceExtent& surface_extent = yetani->surface_extent_map[wl_surface];
	Yetani::SizePixel      new_size       = surface_extent.size_pixel;

	if(((surface_extent.size_pixel_min.width == 0) || (size_pixel.width >= surface_extent.size_pixel_min.width))
		&& ((surface_extent.size_pixel_max.width == 0) || (size_pixel.width <= surface_extent.size_pixel_max.width))
		)
	{
		new_size.width = size_pixel.width;
	}

	if(((surface_extent.size_pixel_min.height == 0) || (size_pixel.height >= surface_extent.size_pixel_min.height))
		&& ((surface_extent.size_pixel_max.height == 0) || (size_pixel.height <= surface_extent.size_pixel_max.height))
		)
	{
		new_size.height = size_pixel.height;
	}

	if((new_size.width == surface_extent.size_pixel.width)
		&& (new_size.height == surface_extent.size_pixel.height)
		)
	{
		return;
	}

	// Calculate Sizes
	Yetani::SizeMm      size_mm;
	Yetani::SizePercent size_percent;
	yetani->convertPixel(wl_surface
		, size_pixel.width  , size_pixel.height
		, size_mm.width     , size_mm.height
		, size_percent.width, size_percent.height
		);

	yetani->surface_resize_mutex_map[wl_surface].lock();
	{
		surface_extent.size_pixel = new_size;
		surfaceCalculateSize(yetani, wl_surface, new_size);
	}
	yetani->surface_resize_mutex_map[wl_surface].unlock();

	Yetani::SurfaceEvent& event = yetani->surface_event_map[wl_surface];
	event.on_size_pixel_change(surface_extent.size_pixel);
	event.on_size_mm_change(surface_extent.size_mm);
	event.on_size_percent_change(surface_extent.size_percent);
}


/**
 * \brief Update the XDG Toplevel size restrictions.
 */
void Yetani::xdgToplevelSizeMinMaxChange(Yetani* yetani         ///< Yetani
	, struct xdg_toplevel*                   xdg_toplevel   ///< The XDG Toplevel
	, struct wl_surface*                     wl_surface     ///< The Waylnad Surface
	, const Yetani::SizePixel&               size_pixel_min ///< The new minimum size
	, const Yetani::SizePixel&               size_pixel_max ///< The new maximum size
	) noexcept
{
	Yetani::SurfaceExtent& surface_extent = yetani->surface_extent_map[wl_surface];

	Yetani::SizePixel size_pixel     = surface_extent.size_pixel;
	bool              need_to_resize = false;

	if(size_pixel_max.width > 0
		&& size_pixel_max.width < surface_extent.size_pixel.width
		)
	{
		need_to_resize = true;
		size_pixel.width = size_pixel_max.width;
	}

	if(size_pixel_max.height > 0
		&& size_pixel_max.height < surface_extent.size_pixel.height
		)
	{
		need_to_resize = true;
		size_pixel.height = size_pixel_max.height;
	}

	if(size_pixel_min.width > 0
		&& size_pixel_min.width > surface_extent.size_pixel.width
		)
	{
		need_to_resize = true;
		size_pixel.width = size_pixel_min.width;
	}

	if(size_pixel_min.height > 0
		&& size_pixel_min.height > surface_extent.size_pixel.height
		)
	{
		need_to_resize = true;
		size_pixel.height = size_pixel_min.height;
	}

	if(need_to_resize)
	{
		xdg_toplevel_set_max_size(xdg_toplevel, 0, 0);
		xdg_toplevel_set_min_size(xdg_toplevel, 0, 0);

		yetani->surface_resize_mutex_map[wl_surface].lock();
		{
			surface_extent.size_pixel = size_pixel;
			surfaceCalculateSize(yetani, wl_surface, size_pixel);
		}
		yetani->surface_resize_mutex_map[wl_surface].unlock();
	}

	xdg_toplevel_set_min_size(xdg_toplevel
		, size_pixel_min.width
		, size_pixel_min.height
		);

	xdg_toplevel_set_max_size(xdg_toplevel
		, size_pixel_max.width
		, size_pixel_max.height
		);

	surface_extent.size_pixel_min = size_pixel_min;
	surface_extent.size_pixel_max = size_pixel_max;
}


/**
 * \brief Change the XDG Toplevel's window type.
 */
void Yetani::xdgToplevelWindowChange(Yetani* yetani       ///< Yetani
	, struct wl_surface*                 wl_surface   ///< Wayland Surface
	, Yetani::XdgToplevel&               toplevel     ///< XDG Toplevel
	, const Yetani::XdgState             window_state ///< The new window state/type
	, const Yetani::SizePixel&           size_pixel   ///< The new size
	) noexcept
{
	Yetani::SurfaceExtent& surface_extent = yetani->surface_extent_map[wl_surface];
	Yetani::SizePixel      new_size{1, 1};

	toplevel.window_state = window_state;

	if((toplevel.window_state == Yetani::XdgState::Toplevel_Window_Fullscreen)
		|| (toplevel.window_state == Yetani::XdgState::Toplevel_Window_Maximized)
		)
	{
		if(toplevel.previous_size.width == 0)
		{
			xdg_toplevel_set_max_size(toplevel.xdg_toplevel, 0, 0);
			xdg_toplevel_set_min_size(toplevel.xdg_toplevel, 0, 0);

			toplevel.previous_size = surface_extent.size_pixel;
		}

		if((size_pixel.width != 0)
			&& (size_pixel.height != 0)
			)
		{
			new_size = size_pixel;
		}
	}
	else if(toplevel.window_state == Yetani::XdgState::Toplevel_Window_Normal)
	{
		xdg_toplevel_set_max_size(toplevel.xdg_toplevel
			, surface_extent.size_pixel_max.width
			, surface_extent.size_pixel_max.height
			);

		xdg_toplevel_set_min_size(toplevel.xdg_toplevel
			, surface_extent.size_pixel_min.width
			, surface_extent.size_pixel_min.height
			);

		new_size = toplevel.previous_size;
		toplevel.previous_size.width = 0;
	}

	if(new_size == surface_extent.size_pixel)
	{
		return;
	}

	// Calculate Size
	Yetani::SizeMm      size_mm;
	Yetani::SizePercent size_percent;
	yetani->convertPixel(wl_surface
		, size_pixel.width  , size_pixel.height
		, size_mm.width     , size_mm.height
		, size_percent.width, size_percent.height
		);

	yetani->surface_resize_mutex_map[wl_surface].lock();
	{
		surface_extent.size_mm      = size_mm;
		surface_extent.size_percent = size_percent;
		surface_extent.size_pixel   = new_size;
		surfaceCalculateSize(yetani, wl_surface, new_size);
	}
	yetani->surface_resize_mutex_map[wl_surface].unlock();

	toplevel.window_state_lambda(Yetani::toWindowMode(toplevel.window_state));

	Yetani::SurfaceEvent& event = yetani->surface_event_map[wl_surface];
	event.on_size_pixel_change(surface_extent.size_pixel);
	event.on_size_mm_change(surface_extent.size_mm);
	event.on_size_percent_change(surface_extent.size_percent);
}

// }}}
// {{{ XDG : Decoration (Unstable)

/**
 * \struct Yetani::XdgDecoration
 *
 * \brief All data related to XDG Decorations.
 *
 * \var Yetani::XdgDecoration::state_change
 * \brief A pointer to the XDG Surface byte-code buffer
 *
 * \var Yetani::XdgDecoration::lambda
 * \brief The event handler
 *
 * \var Yetani::XdgDecoration::state
 * \brief The current Decoration state
 *
 * \var Yetani::XdgDecoration::is_present
 * \brief A flag to indicate if decorations are present
 */

/**
 * \brief Create a XDG Toplevel Decoration.
 *
 * __This feature is not part of Wayland(Stable)__
 */
struct zxdg_toplevel_decoration_v1* Yetani::xdgDecorationCreate(struct xdg_surface* xdg_surface  ///< The XDG Surface
	, struct xdg_toplevel*                                                      xdg_toplevel ///< The XDG Toplevel
	) noexcept
{
	if(decoration_manager == nullptr)
	{
		return nullptr;
	}

	/* *** From the xdg-decoration-unstable-v1 documentation ***
	 *
	 * Creating an xdg_toplevel_decoration from an xdg_toplevel which has a 
	 * buffer attached or committed is a client error.
	 */

	zxdg_toplevel_decoration_v1* xdg_decoration =
		zxdg_decoration_manager_v1_get_toplevel_decoration(decoration_manager
			, xdg_toplevel
			);

	Yetani::XdgDecoration& decoration = xdg_decoration_map[xdg_surface];
	decoration.state_change = &(xdg_state_change_map[xdg_surface]);
	decoration.lambda       = LambdaWindowDecorations_DoNothing;
	decoration.state        = 0;
	decoration.is_present   = false;

	zxdg_toplevel_decoration_v1_add_listener(xdg_decoration
		, &xdg_toplevel_decoration_listener
		, &decoration
		);

	return xdg_decoration;
}


/**
 * \brief Destroy the XDG Toplevel Decoration.
 *
 * __This feature is not part of Wayland(Stable)__
 */
void Yetani::xdgDecorationDestroy(struct xdg_surface* xdg_surface    ///< The XDG Surface
	, struct xdg_toplevel*                        //xdg_toplevel   ///< The XDG Toplevel
	, struct zxdg_toplevel_decoration_v1*&        xdg_decoration ///< The XDG Toplevel Decoration
	) noexcept
{
	zxdg_toplevel_decoration_v1_destroy(xdg_decoration);

	if(xdg_decoration_map.contains(xdg_surface))
	{
		Yetani::XdgDecoration& decoration = xdg_decoration_map[xdg_surface];
		decoration.state_change = nullptr;
		decoration.lambda       = nullptr;
		decoration.state        = 0;
		decoration.is_present   = false;

		xdg_decoration_map.erase(xdg_surface);
	}

	xdg_decoration = nullptr;
}


/**
 * \brief Change the XDG Toplevel Decoration state.
 *
 * __This feature is not part of Wayland(Stable)__
 */
void Yetani::xdgDecorationChange(Yetani::XdgDecoration& decoration       ///< The XDG Toplevel Decoration
	, const uint32_t                                decoration_state ///< The new decoration state
	) noexcept
{
	if(decoration_state == ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE)
	{
		if(decoration.state != decoration_state)
		{
			decoration.state    = decoration_state;
			decoration.is_present = false;

			decoration.lambda(
				Yetani::WindowDecorations::Client_Side
				);
		}
	}
	else if(decoration_state == ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE)
	{
		if(decoration.state != decoration_state)
		{
			decoration.state    = decoration_state;
			decoration.is_present = true;
		}
		else
		{
			decoration.is_present = !decoration.is_present;
		}

		if(decoration.is_present == true)
		{
			decoration.lambda(
				Yetani::WindowDecorations::Server_Side
				);
		}
		else
		{
			decoration.lambda(
				Yetani::WindowDecorations::Client_Side
				);
		}
	}
}

// }}}
// {{{ XDG : Listener Handlers : Surface

/**
 * \brief Handle XDG Surface configuration changes.
 *
 * The XDG Surface is required to handle all changes from "extending" XDG 
 * classes, such as XDG Toplevel and XDG Toplevel Decorations.  To ease this 
 * complexity, everything that the XDG Surface handles must be converted into a 
 * simple byte-code.  This method will then interpret the byte-code to figure 
 * out what needs to be done.
 *
 * \see Yetani::XdgState
 * \see Yetani::handlerXdgToplevelConfigure()
 * \see Yetani::handlerXdgToplevelDecorationConfigure()
 */
void Yetani::handlerXdgSurfaceConfigure(void* data        ///< User data
	, xdg_surface*                        xdg_surface ///< The XDG surface
	, uint32_t                            serial      ///< Event ID
	) noexcept
{
	Yetani::XdgSurface& surface = *((Yetani::XdgSurface*)data);
	Yetani* yetani = surface.yetani;

	xdg_surface_ack_configure(xdg_surface, serial);

	VectorXdgStateChange& state_change = yetani->xdg_state_change_map[xdg_surface];

	if(state_change.empty())
	{
		return;
	}

	auto iter = std::begin(state_change);
	auto iter_end = std::end(state_change);

	while(iter != iter_end)
	{
		switch(*iter)
		{
			case Yetani::XdgState::Toplevel_Active:
			{
				Yetani::XdgToplevel& toplevel = yetani->xdg_toplevel_map[xdg_surface];

				iter++;
				bool is_active = bool(*iter);
				if(toplevel.is_active != is_active)
				{
					toplevel.is_active = is_active;

					toplevel.is_active_lambda(is_active);
				}
			} break;

			case Yetani::XdgState::Toplevel_Attach_Buffer:
			{
				struct wl_surface* wl_surface = surface.wl_surface;

				SurfaceFrame& surface_frame =
					yetani->surface_frame_map[wl_surface];

				wl_surface_attach(surface_frame.wl_surface
					, surface_frame.buffer_next
					, 0, 0
					);

				struct wl_callback* callback = wl_surface_frame(surface_frame.wl_surface);

				wl_callback_add_listener(callback
					, &frame_callback_listener
					, &surface_frame
					);

				wl_surface_commit(surface_frame.wl_surface);
			} break;

			case Yetani::XdgState::Toplevel_Window_Normal:
			case Yetani::XdgState::Toplevel_Window_Maximized:
			case Yetani::XdgState::Toplevel_Window_Fullscreen:
			{
				struct wl_surface* wl_surface = surface.wl_surface;

				Yetani::XdgToplevel& toplevel = yetani->xdg_toplevel_map[xdg_surface];

				Yetani::XdgState window_state = XdgState(*iter);

				Yetani::SizePixel size_pixel;

				iter++;
				size_pixel.width = *iter;

				iter++;
				size_pixel.height = *iter;

				if(toplevel.window_state != window_state)
				{
					xdgToplevelWindowChange(yetani
						, wl_surface
						, toplevel
						, window_state
						, size_pixel
						);
				}
			} break;

			case Yetani::XdgState::Toplevel_Resizing:
			{
				struct wl_surface* wl_surface = surface.wl_surface;

				Yetani::SizePixel size_pixel;

				iter++;
				size_pixel.width = *iter;

				iter++;
				size_pixel.height = *iter;

				if(size_pixel.width > 0
					&& size_pixel.height > 0
					)
				{
					xdgToplevelSizeChange(yetani
						, wl_surface
						, size_pixel
						);
				}
			} break;

			case Yetani::XdgState::Toplevel_Decoration:
			{
				iter++;
				uint32_t decoration_state = *iter;

				Yetani::XdgDecoration& decoration = yetani->xdg_decoration_map[xdg_surface];

				xdgDecorationChange(decoration, decoration_state);
			} break;
		}

		iter++;
	}

	state_change.clear();
}

// }}}
// {{{ XDG : Listener Handlers : Toplevel

/**
 * \brief Handle XDG Toplevel close requests.
 */
void Yetani::handlerXdgToplevelClose(void* data         ///< User data
	, struct xdg_toplevel*             //xdg_toplevel ///< The toplevel surface
	) noexcept
{
	Yetani::XdgToplevel* toplevel = (Yetani::XdgToplevel*)data;

	toplevel->close_request_lambda();
}


/**
 * \brief Handle XDG Toplevel configuration changes.
 */
void Yetani::handlerXdgToplevelConfigure(void* data         ///< User data
	, struct xdg_toplevel*                 //xdg_toplevel ///< The toplevel surface
	, int32_t                              width        ///< Suggested width
	, int32_t                              height       ///< Suggested height
	, struct wl_array*                     state_array  ///< Active states
	) noexcept
{
	Yetani::XdgToplevel* toplevel = (Yetani::XdgToplevel*)data;

	Yetani::XdgState window_state = Yetani::XdgState::Toplevel_Window_Normal;
	int32_t is_active = 0;

	ZAKERO_YETANI__ARRAY_FOR_EACH(xdg_toplevel_state*, state_iter, state_array)
	{
		xdg_toplevel_state state = *state_iter;

		switch(state)
		{
			case XDG_TOPLEVEL_STATE_MAXIMIZED:
				window_state = Yetani::XdgState::Toplevel_Window_Maximized;
				break;
			case XDG_TOPLEVEL_STATE_FULLSCREEN:
				window_state = Yetani::XdgState::Toplevel_Window_Fullscreen;
				break;
			case XDG_TOPLEVEL_STATE_RESIZING:
				toplevel->state_change->push_back(Yetani::XdgState::Toplevel_Resizing);
				toplevel->state_change->push_back(width);
				toplevel->state_change->push_back(height);
				break;
			case XDG_TOPLEVEL_STATE_ACTIVATED:
				is_active = 1;
				break;
			case XDG_TOPLEVEL_STATE_TILED_LEFT:
				break;
			case XDG_TOPLEVEL_STATE_TILED_RIGHT:
				break;
			case XDG_TOPLEVEL_STATE_TILED_TOP:
				break;
			case XDG_TOPLEVEL_STATE_TILED_BOTTOM:
				break;
			default:
				break;
		}
	}

	toplevel->state_change->push_back(window_state);
	toplevel->state_change->push_back(width);
	toplevel->state_change->push_back(height);

	toplevel->state_change->push_back(Yetani::XdgState::Toplevel_Active);
	toplevel->state_change->push_back(is_active);
}

// }}}
// {{{ XDG : Listener Handlers : WM Base

/**
 * \brief Tell the Wayland Compositor that we are alive.
 */
void Yetani::handlerXdgWmBasePing(void* //data        ///< User data
	, struct xdg_wm_base*           xdg_wm_base ///< Window Manager
	, uint32_t                      serial      ///< Event ID
	) noexcept
{
	xdg_wm_base_pong(xdg_wm_base, serial);
}

// }}}
// {{{ XDG : Listener Handlers (Unstable) : Decoration

/**
 * \brief Handle XDG Toplevel Decoration configuration changes.
 */
void Yetani::handlerXdgToplevelDecorationConfigure(void* data       ///< User data
	, struct zxdg_toplevel_decoration_v1*            //decoration ///< The Decoration object
	, uint32_t                                       mode       ///< The new decoration mode
	) noexcept
{
	Yetani::XdgDecoration* deco = (Yetani::XdgDecoration*)data;

	deco->state_change->push_back(Yetani::XdgState::Toplevel_Decoration);
	deco->state_change->push_back(mode);
}

// }}}
// {{{ Window

/**
 * \var Yetani::SHM_FORMAT_DEFAULT
 * 
 * \brief The default pixel format.
 */

/**
 * \enum Yetani::WindowDecorations
 *
 * \brief Who is responsible for rendering the decorations.
 */
/* Disabled because Doxygen does not support "enum classes"
 *
 * \var Yetani::Client_Side
 * \brief The user app must draw the decorations.
 *
 * \var Yetani::Server_Side
 * \brief The Wayland Compositor will draw the decorations.
 */

/**
 * \enum Yetani::WindowMode
 *
 * All the available window modes.
 */
/* Disabled because Doxygen does not support "enum classes"
 *
 * \var Yetani::Normal
 * \brief A normal window.
 *
 * \var Yetani::Fullscreen
 * \brief A window that uses the entire screen, no borders.
 *
 * \var Yetani::Maximized
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
 * \typedef Yetani::Lambda
 *
 * \brief A Lambda that has no parameters.
 */

/**
 * \typedef Yetani::LambdaKey
 *
 * \brief A Lambda that has parameters: Key and KeyModifier.
 */

/**
 * \typedef Yetani::LambdaAxis
 *
 * \brief A Lambda that has parameters: PointerAxis and KeyModifier.
 */

/**
 * \typedef Yetani::LambdaButtonMm
 *
 * \brief A Lambda that has parameters: PointerButton, PointMm and KeyModifier.
 */

/**
 * \typedef Yetani::LambdaButtonPercent
 *
 * \brief A Lambda that has parameters: PointerButton, PointPercent and 
 * KeyModifier.
 */

/**
 * \typedef Yetani::LambdaButtonPixel
 *
 * \brief A Lambda that has parameters: PointerButton, PointPixel and 
 * KeyModifier.
 */

/**
 * \typedef Yetani::LambdaPointMm
 *
 * \brief A Lambda that has parameters: PointMm and KeyModifier.
 */

/**
 * \typedef Yetani::LambdaPointPercent
 *
 * \brief A Lambda that has parameters: PointPercent and KeyModifier.
 */

/**
 * \typedef Yetani::LambdaPointPixel
 *
 * \brief A Lambda that has parameters: PointPixel and KeyModifier.
 */

/**
 * \typedef Yetani::LambdaBool
 *
 * \brief A Lambda that has a parameter: bool.
 */

/**
 * \typedef Yetani::LambdaWindowDecorations
 *
 * \brief A Lambda that has a parameter: WindowDecorations.
 */

/**
 * \typedef Yetani::LambdaWindowMode
 *
 * \brief A Lambda that has a parameter: WindowMode.
 */

/**
 * \typedef Yetani::LambdaSizeMm
 *
 * \brief A Lambda that has a parameter: SizeMm.
 */

/**
 * \typedef Yetani::LambdaSizePercent
 *
 * \brief A Lambda that has a parameter: SizePercent.
 */

/**
 * \typedef Yetani::LambdaSizePixel
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
 * be returned.  If there was a problem, a \nullptr will returned.
 *
 * The \p error parameter will be set to Yetani::Error_None on success or to an 
 * appropriate error if there was a problem.
 *
 * \note The size of a window __must__ be greater than `0`.
 *
 * \return A pointer to the new Window or \nullptr on error.
 */
Yetani::Window* Yetani::windowCreate(const Yetani::SizeMm& size  ///< The window size
	, std::error_code&                                 error ///< The error state
	) noexcept
{
	return windowCreate(Yetani::SizeUnit::Millimeter
		, size
		, { 0, 0 }
		, { 0, 0 }
		, SHM_FORMAT_DEFAULT
		, error
		);
}


/**
 * \brief Create a window.
 *
 * Create a new Window of the specified \p size.  A pointer to the Window will 
 * be returned.  If there was a problem, a \nullptr will returned.
 *
 * The shared memory used by the Window will use the pixel \p format.
 *
 * \note The size of a window __must__ be greater than `0`.
 *
 * \return A pointer to the new Window or \nullptr on error.
 */
Yetani::Window* Yetani::windowCreate(const Yetani::SizeMm& size   ///< The window size
	, const wl_shm_format                              format ///< The pixel format
	) noexcept
{
	std::error_code error;

	return windowCreate(Yetani::SizeUnit::Millimeter
		, size
		, { 0, 0 }
		, { 0, 0 }
		, format
		, error
		);
}


/**
 * \brief Create a window.
 *
 * Create a new Window of the specified \p size.  A pointer to the Window will 
 * be returned.  If there was a problem, a \nullptr will returned.
 *
 * The shared memory used by the Window will use the pixel \p format.
 *
 * The \p error parameter will be set to Yetani::Error_None on success or to an 
 * appropriate error if there was a problem.
 *
 * \note The size of a window __must__ be greater than `0`.
 *
 * \return A pointer to the new Window or \nullptr on error.
 */
Yetani::Window* Yetani::windowCreate(const Yetani::SizeMm& size   ///< The window size
	, const wl_shm_format                              format ///< The pixel format
	, std::error_code&                                 error  ///< The error state
	) noexcept
{
	return windowCreate(Yetani::SizeUnit::Millimeter
		, size
		, { 0, 0 }
		, { 0, 0 }
		, format
		, error
		);
}


/**
 * \brief Create a window.
 *
 * Create a new Window of the specified \p size.  A pointer to the Window will 
 * be returned.  If there was a problem, a \nullptr will returned.
 *
 * The \p error parameter will be set to Yetani::Error_None on success or to an 
 * appropriate error if there was a problem.
 *
 * \note The size of a window __must__ be greater than `0`.
 *
 * \return A pointer to the new Window or \nullptr on error.
 */
Yetani::Window* Yetani::windowCreate(const Yetani::SizePercent& size  ///< The window size
	, std::error_code&                                      error ///< The error state
	) noexcept
{
	return windowCreate(Yetani::SizeUnit::Percent
		, { 0, 0 }
		, size
		, { 0, 0 }
		, SHM_FORMAT_DEFAULT
		, error
		);
}


/**
 * \brief Create a window.
 *
 * Create a new Window of the specified \p size.  A pointer to the Window will 
 * be returned.  If there was a problem, a \nullptr will returned.
 *
 * The shared memory used by the Window will use the pixel \p format.
 *
 * \note The size of a window __must__ be greater than `0`.
 *
 * \return A pointer to the new Window or \nullptr on error.
 */
Yetani::Window* Yetani::windowCreate(const Yetani::SizePercent& size   ///< The window size
	, const wl_shm_format                                   format ///< The pixel format
	) noexcept
{
	std::error_code error;

	return windowCreate(Yetani::SizeUnit::Percent
		, { 0, 0 }
		, size
		, { 0, 0 }
		, format
		, error
		);
}


/**
 * \brief Create a window.
 *
 * Create a new Window of the specified \p size.  A pointer to the Window will 
 * be returned.  If there was a problem, a \nullptr will returned.
 *
 * The shared memory used by the Window will use the pixel \p format.
 *
 * The \p error parameter will be set to Yetani::Error_None on success or to an 
 * appropriate error if there was a problem.
 *
 * \note The size of a window __must__ be greater than `0`.
 *
 * \return A pointer to the new Window or \nullptr on error.
 */
Yetani::Window* Yetani::windowCreate(const Yetani::SizePercent& size   ///< The window size
	, const wl_shm_format                                   format ///< The pixel format
	, std::error_code&                                      error  ///< The error state
	) noexcept
{
	return windowCreate(Yetani::SizeUnit::Percent
		, { 0, 0 }
		, size
		, { 0, 0 }
		, format
		, error
		);
}


/**
 * \brief Create a window.
 *
 * Create a new Window of the specified \p size.  A pointer to the Window will 
 * be returned.  If there was a problem, a \nullptr will returned.
 *
 * The \p error parameter will be set to Yetani::Error_None on success or to an 
 * appropriate error if there was a problem.
 *
 * \note The size of a window __must__ be greater than `0`.
 *
 * \return A pointer to the new Window or \nullptr on error.
 */
Yetani::Window* Yetani::windowCreate(const Yetani::SizePixel& size  ///< The window size
	, std::error_code&                                    error ///< The error state
	) noexcept
{
	return windowCreate(Yetani::SizeUnit::Pixel
		, { 0, 0 }
		, { 0, 0 }
		, size
		, SHM_FORMAT_DEFAULT
		, error
		);
}


/**
 * \brief Create a window.
 *
 * Create a new Window of the specified \p size.  A pointer to the Window will 
 * be returned.  If there was a problem, a \nullptr will returned.
 *
 * The shared memory used by the Window will use the pixel \p format.
 *
 * \note The size of a window __must__ be greater than `0`.
 *
 * \return A pointer to the new Window or \nullptr on error.
 */
Yetani::Window* Yetani::windowCreate(const Yetani::SizePixel& size   ///< The window size
	, const wl_shm_format                                 format ///< The pixel format
	) noexcept
{
	std::error_code error;

	return windowCreate(Yetani::SizeUnit::Pixel
		, { 0, 0 }
		, { 0, 0 }
		, size
		, format
		, error
		);
}


/**
 * \brief Create a window.
 *
 * Create a new Window of the specified \p size.  A pointer to the Window will 
 * be returned.  If there was a problem, a \nullptr will returned.
 *
 * The shared memory used by the Window will use the pixel \p format.
 *
 * The \p error parameter will be set to Yetani::Error_None on success or to an 
 * appropriate error if there was a problem.
 *
 * \note The size of a window __must__ be greater than `0`.
 *
 * \return A pointer to the new Window or \nullptr on error.
 */
Yetani::Window* Yetani::windowCreate(const Yetani::SizePixel& size   ///< The window size
	, const wl_shm_format                                 format ///< The pixel format
	, std::error_code&                                    error  ///< The error state
	) noexcept
{
	return windowCreate(Yetani::SizeUnit::Pixel
		, { 0, 0 }
		, { 0, 0 }
		, size
		, format
		, error
		);
}


/**
 * \brief Create a Window.
 *
 * Create a new window.
 *
 * \return A pointer to the new Window
 */
Yetani::Window* Yetani::windowCreate(const Yetani::SizeUnit size_unit    ///< The preferred size unit
	, const Yetani::SizeMm&                             size_mm      ///< The size in millimeters
	, const Yetani::SizePercent&                        size_percent ///< The size as a percentage
	, const Yetani::SizePixel&                          size_pixel   ///< The size in pixels
	, const wl_shm_format                               pixel_format ///< The pixel format
	, std::error_code&                                  error        ///< Resulting error code
	) noexcept
{
	if((size_unit == Yetani::SizeUnit::Millimeter)
		&& (size_mm.width <= 0 || size_mm.height <= 0)
		)
	{
		error = ZAKERO_YETANI__ERROR(Error_Window_Size_Too_Small);

		return nullptr;
	}

	if((size_unit == Yetani::SizeUnit::Percent)
		&& (size_percent.width <= 0 || size_percent.height <= 0)
		)
	{
		error = ZAKERO_YETANI__ERROR(Error_Window_Size_Too_Small);

		return nullptr;
	}

	if((size_unit == Yetani::SizeUnit::Pixel)
		&& (size_pixel.width <= 0 || size_pixel.height <= 0)
		)
	{
		error = ZAKERO_YETANI__ERROR(Error_Window_Size_Too_Small);

		return nullptr;
	}

	const std::string file_name = "Zakero.Yetani."
		+ std::to_string(ZAKERO_STEADY_TIME_NOW(nanoseconds))
		;

	struct WindowData window_data =
	{	.yetani       = this
	,	.wl_shm       = shm
	,	.wl_output    = nullptr
	,	.file_name    = file_name
	,	.size_mm      = size_mm
	,	.size_percent = size_percent
	,	.size_pixel   = size_pixel
	,	.size_unit    = size_unit
	,	.pixel_format = pixel_format
	,	.error        = ZAKERO_YETANI__ERROR(Error_None)
	};

	windowDataInit(window_data);

	if(window_data.error)
	{
		error = window_data.error;

		return nullptr;
	}
     
	Yetani::Window* window = new Window(&window_data);

	if(window_data.error)
	{
		delete window;
		window = nullptr;

		ZAKERO_YETANI__DEBUG << to_string(window_data.error) << "\n";

		error = ZAKERO_YETANI__ERROR(Error_Window_Initialization_Failed);

		return nullptr;
	}

	error = ZAKERO_YETANI__ERROR(Error_None);

	return window;
}


/**
 * \brief Initialize the WindowData
 */
void Yetani::windowDataInit(Yetani::WindowData& window_data ///< The window data
	) noexcept
{
	windowDataInitOutput(window_data);

	if(window_data.error)
	{
		return;
	}

	window_data.size_pixel.width  = std::max(1, window_data.size_pixel.width);
	window_data.size_pixel.height = std::max(1, window_data.size_pixel.height);

	window_data.error = ZAKERO_YETANI__ERROR(Error_None);
}


/**
 * \brief Initialize the WindowData: Output
 */
void Yetani::windowDataInitOutput(Yetani::WindowData& window_data ///< The window data
	) noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.output_map.empty())
	{
		window_data.error = ZAKERO_YETANI__ERROR(Error_No_Output_Available);

		return;
	}

	const auto& iter = output_data.output_map.begin();

	window_data.wl_output  = iter->first;
	Yetani::Output& output = iter->second;

	if(window_data.size_unit == Yetani::SizeUnit::Millimeter)
	{
		auto px = convertMmToPixel(output
			, window_data.size_mm.width
			, window_data.size_mm.height
			);

		auto pc = convertPixelToPercent(output
			, px.first
			, px.second
			);

		window_data.size_mm      = window_data.size_mm;
		window_data.size_percent = { pc.first, pc.second };
		window_data.size_pixel   = { px.first, px.second };
	}
	else if(window_data.size_unit == Yetani::SizeUnit::Percent)
	{
		auto px = convertPercentToPixel(output
			, window_data.size_percent.width
			, window_data.size_percent.height
			);

		auto mm = convertPixelToMm(output
			, px.first
			, px.second
			);

		window_data.size_mm      = { mm.first, mm.second };
		window_data.size_percent = window_data.size_percent;
		window_data.size_pixel   = { px.first, px.second };
	}
	else if(window_data.size_unit == Yetani::SizeUnit::Pixel)
	{
		auto mm = convertPixelToMm(output
			, window_data.size_pixel.width
			, window_data.size_pixel.height
			);

		auto pc = convertPixelToPercent(output
			, window_data.size_pixel.width
			, window_data.size_pixel.height
			);

		window_data.size_mm      = { mm.first, mm.second };
		window_data.size_percent = { pc.first, pc.second };
		window_data.size_pixel   = window_data.size_pixel;
	}

	window_data.error = ZAKERO_YETANI__ERROR(Error_None);
}


/**
 * \brief Initialize the shared memory
 */
void Yetani::windowInitMemory(Yetani::WindowData& window_data   ///< The window data
	, Yetani::Window::Memory&                 window_memory ///< The shared memory
	) noexcept
{
	size_t size_in_bytes = sizeInBytes(window_data.size_pixel
		, window_data.pixel_format
		) * 3;

	window_data.error = window_memory.memory_pool.init(size_in_bytes
		, true
		, zakero::MemoryPool::Alignment::Bits_32
		);

	if(window_data.error)
	{
		return;
	}

	window_memory.wl_shm_pool = wl_shm_create_pool(window_data.wl_shm
		, window_memory.memory_pool.fd()
		, window_memory.memory_pool.size()
		);

	window_memory.memory_pool.sizeOnChange([&](size_t new_size)
	{
		wl_shm_pool_resize(window_memory.wl_shm_pool, new_size);
	});

	window_data.error = ZAKERO_YETANI__ERROR(Error_None);
}


/**
 * \brief Initialize the output
 */
void Yetani::windowInitOutput(Yetani::WindowData& window_data ///< The window data
	, struct wl_surface*                      wl_surface  ///< The Wayland Surface
	) noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	output_data.surface_output_map[wl_surface].push_back(window_data.wl_output);
}


/**
 * \brief Release the shared memory
 */
void Yetani::windowEraseMemory(Yetani::Window::Memory& window_memory ///< The shared memory
	) noexcept
{
	if(window_memory.wl_shm_pool)
	{
		wl_shm_pool_destroy(window_memory.wl_shm_pool);
		window_memory.wl_shm_pool = nullptr;
	}
}


/**
 * \brief Remove output references
 */
void Yetani::windowEraseOutput(struct wl_surface* wl_surface ///< The Wayland Surface
	) noexcept
{
	std::lock_guard<std::mutex> lock(output_data.mutex);

	if(output_data.surface_output_map.contains(wl_surface))
	{
		output_data.surface_output_map.erase(wl_surface);
	}
}


/**
 * \brief Removed the window size data.
 */
void Yetani::windowEraseSurfaceExtent(struct wl_surface* wl_surface ///< The Wayland Surface
	) noexcept
{
	std::lock_guard<std::mutex> lock(surface_extent_mutex);

	if(surface_extent_map.contains(wl_surface))
	{
		surface_extent_map.erase(wl_surface);
	}
}


/**
 * \brief Record the creation of a Window.
 */
void Yetani::windowAdd(Yetani::Window* window ///< The window to add.
	) noexcept
{
	std::lock_guard<std::mutex> lock(window_vector_mutex);

	window_vector.push_back(window);
}


/**
 * \brief Forget about a Window.
 */
void Yetani::windowRemove(Yetani::Window* window ///< The window to remove.
	) noexcept
{
	std::lock_guard<std::mutex> lock(window_vector_mutex);

	zakero::vectorErase(window_vector, window);
}

// }}}
// {{{ Window : Documentation

/**
 * \class Yetani::Window
 *
 * \brief A %Window
 *
 * The %Window is the real work-horse of Yetani.  This is the object that the 
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
 *
 * If this situation makes your life complicated, find and complain to the 
 * Wayland developers.  So far, they just don't care.
 * \endparblock
 *
 * \par Rendering
 * \parblock
 * Updating the contents of the window is a two-step process.  The first step 
 * is to get an "image" from the window (Yetani::Window::imageNext()).  The 
 * %Window will provide a pointer to it's internal graphics buffer, which can 
 * then be used for rendering.
 *
 * How do you do this rendering?  Well that is up to you, _Zakero %Yetani_ does 
 * not provide any graphics API.  You can write your own graphics library (not 
 * that hard), or use one that can accept a pointer and use it.  For (a bad) 
 * example: Qt5's QImage can be used with a raw data pointer.
 *
 * After writing all the required data to the "image", the second step is to 
 * tell the %Window to present the image (Yetani::Window::imagePresent()).  
 * Then the %Window will tell the Wayland Compositor to update the window 
 * contents on-screen.
 *
 * Why the method names "imageNext()" and "imagePresent()"?  That is to match 
 * the same language that the _Vulkan_ API uses.  _Zakero %Yetani_ is not 
 * compatible with _Vulkan_ at this point in time.
 * \endparblock
 *
 * \par Cursors
 * \parblock
 * Cursors in Wayland is a tricky topic and as such may not behave as expected.
 * The Wayland spec says that it is the window's (actually, the wl_surface) 
 * responsiblity to set the cursor when the pointer enters its area.  If the 
 * cursor is not set, then the cursor is undefined.
 *
 * What does this mean?  For some Wayland Compositors, the cursor that has been 
 * defined by the Desktop Environment will be used.  With other Wayland 
 * Compositors, the cursor will just "disappear".  And errors are reported in 
 * yet a different Wayland Compositor.  So, to have a consistent cursor 
 * behavior across all Wayland Compositors, a cursor should be created 
 * (Yetani::cursorCreate()) and used (Yetani::Window::cursorUse()).
 *
 * Simple, right?  Not quite.  If one window sets the cursor and another window 
 * does not, again for some Wayland Compositors, both windows will use the 
 * cursor set by window one.  This "default cursor" behavior seems to be based 
 * on the connection to the Wayland Compositor.
 *
 * In conclusion, not setting the cursor is undefined behavior and all windows 
 * should set a cursor to use.
 * \endparblock
 *
 * \par Focus
 * \parblock
 * In Wayland there are three types of "focus".
 * - __%Window Focus__ (active)<br>
 *   When a window is "active", it has the focus of the Desktop Environment.  
 *   The window may be the top-most or the decorations, if present, may have so 
 *   type of visual indication that the window is "active".
 *   <br><br>
 * - __%Keyboard Focus__<br>
 *   Certain activities in the Desktop Environment make it possible for a 
 *   %Window to have %Keyboard Focus.  When a %Window has %Keyboard Focus all 
 *   keyboard events will be sent to that %Window.
 *   <br><br>
 * - __%Pointer Focus__<br>
 *   %Windows gain %Pointer Focus with the pointer device, such as a mouse, 
 *   enters the window area (the decorations don't count).  Just as with 
 *   %Keyboard Focus, when a %Window has %Pointer Focus, it will receive all 
 *   the pointer events.
 *
 * Because there are three types of "focus" and each has a different triggering 
 * mechanism, it is possible for a %Window to have only one or two out of the 
 * three focus times.  Yes, a %Window could have only %Keyboard Focus.
 *
 * One possible way to deal with this is to ignore all events until the %Window 
 * is active, but that may break user expectations in their Desktop 
 * Environment.  Try to figure out what makes the most sense with your 
 * application.
 * \endparblock
 *
 * \internal
 *
 * The %Window object just binds together data that is in the Yetani object.  
 * Since %Window is a sub-class of Yetani, it has full access to Yetani's 
 * private methods.
 */

/**
 * \struct Yetani::Window::Memory
 *
 * \brief The shared memory.
 *
 * This structure holds the shared memory used by the %Window.
 *
 * \var Yetani::Window::Memory::wl_shm_pool
 * \brief A pointer to the Wayland Shared Memory Pool
 *
 * \var Yetani::Window::Memory::memory_pool
 * \brief The %Window's Memory Pool
 */

/**
 * \struct Yetani::WindowData
 *
 * \brief Data needed to create a Window.
 *
 * \var Yetani::WindowData::yetani
 * \brief The owning Yetani instance
 *
 * \var Yetani::WindowData::wl_shm
 * \brief The Wayland Shared Memory pointer
 *
 * \var Yetani::WindowData::wl_output
 * \brief The Wayland output device
 *
 * \var Yetani::WindowData::file_name
 * \brief The file name for the Yetani::MemoryPool
 *
 * \var Yetani::WindowData::size_mm
 * \brief The initial size in millimeters
 *
 * \var Yetani::WindowData::size_percent
 * \brief The initial size in percent
 *
 * \var Yetani::WindowData::size_pixel
 * \brief The initial size in pixels
 *
 * \var Yetani::WindowData::pixel_format
 * \brief The pixel format to use
 *
 * \var Yetani::WindowData::error
 * \brief The error that occurred
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
 * - Yetani::windowCreate(const Yetani::SizeMm&, std::error_code&)
 * - Yetani::windowCreate(const Yetani::SizeMm&, const wl_shm_format)
 * - Yetani::windowCreate(const Yetani::SizeMm&, const wl_shm_format, std::error_code&)
 * - Yetani::windowCreate(const Yetani::SizePercent&, std::error_code&)
 * - Yetani::windowCreate(const Yetani::SizePercent&, const wl_shm_format)
 * - Yetani::windowCreate(const Yetani::SizePercent&, const wl_shm_format, std::error_code&)
 * - Yetani::windowCreate(const Yetani::SizePixel&, std::error_code&)
 * - Yetani::windowCreate(const Yetani::SizePixel&, const wl_shm_format)
 * - Yetani::windowCreate(const Yetani::SizePixel&, const wl_shm_format, std::error_code&)
 *
 * \internal
 *
 * The \p ptr is the Yetani::WindowData in disguise.  The constructor will use 
 * the WindowData to create the requested window.  If there is an error during 
 * construction, %Window will be in an unknown state and the 
 * Yetani::WindowData.error will be set appropriately.
 *
 * It is expected that the caller will check the WindowData error and delete 
 * the incomplete %Window if needed.
 */
Yetani::Window::Window(void* ptr ///< data
	)
	: yetani         {((Yetani::WindowData*)ptr)->yetani}
	, wl_buffer      {nullptr}
	, wl_surface     {nullptr}
	, xdg_surface    {nullptr}
	, xdg_toplevel   {nullptr}
	, xdg_decoration {nullptr}
	, window_memory  {nullptr, ((Yetani::WindowData*)ptr)->file_name}
	, pixel_format   {((Yetani::WindowData*)ptr)->pixel_format}
{
	Yetani::WindowData& window_data = *((Yetani::WindowData*)ptr);

	yetani->windowInitMemory(window_data, window_memory);
	if(window_data.error)
	{
		return;
	}

	wl_surface = yetani->surfaceCreate(yetani
		, pixel_format
		, window_data.size_pixel
		, window_memory
		);

	xdg_surface = yetani->xdgSurfaceCreate(wl_surface);

	yetani->xdgSurfaceSetExtent(wl_surface
		, window_data.size_unit
		, window_data.size_mm
		, window_data.size_percent
		, window_data.size_pixel
		);

	xdg_toplevel = yetani->xdgToplevelCreate(xdg_surface);

	xdg_decoration = yetani->xdgDecorationCreate(xdg_surface, xdg_toplevel);

	wl_surface_commit(wl_surface);

	yetani->windowInitOutput(window_data, wl_surface);
	if(window_data.error)
	{
		return;
	}

	yetani->windowAdd(this);
}


/**
 * \brief Destroy a %Window.
 */
Yetani::Window::~Window()
{
	yetani->windowRemove(this);

	if(xdg_decoration != nullptr)
	{
		yetani->xdgDecorationDestroy(xdg_surface, xdg_toplevel, xdg_decoration);
	}

	if(xdg_toplevel != nullptr)
	{
		yetani->xdgToplevelDestroy(xdg_surface, xdg_toplevel);
	}

	if(xdg_surface != nullptr)
	{
		yetani->xdgSurfaceDestroy(wl_surface, xdg_surface);
	}

	if(wl_surface != nullptr)
	{
		yetani->windowEraseOutput(wl_surface);
		yetani->surfaceDestroy(yetani, wl_surface);
		yetani->windowEraseSurfaceExtent(wl_surface);
	}

	yetani->windowEraseMemory(window_memory);
}

// }}}
// {{{ Window : Cursor

/**
 * \brief Use a cursor.
 *
 * After creating a cursor, Yetani::cursorCreate(), a window can use that 
 * cursor by specifying the cursor name.  The cursor can be changed at anytime 
 * by calling this method.
 *
 * To stop using a cursor, pass an empty string as the \p name parameter.
 *
 * \note Not specifying a cursor is _undefined behavior_ in the Wayland spec.  
 *
 * \return An error code.
 */
std::error_code Yetani::Window::cursorUse(const std::string& name ///< The cursor name
	) noexcept
{
	if(name.empty())
	{
		return yetani->cursorDetach(wl_surface);
	}

	return yetani->cursorAttach(name, wl_surface);
}


/**
 * \brief Hide the cursor.
 *
 * The cursor will no longer be visible after calling this method.
 */
void Yetani::Window::cursorHide() noexcept
{
	yetani->cursorHide(wl_surface);
}


/**
 * \brief Show the cursor.
 *
 * The cursor will be visible when it is in the window.
 */
void Yetani::Window::cursorShow() noexcept
{
	yetani->cursorShow(wl_surface);
}

// }}}
// {{{ Window : Settings

/**
 * \brief Change the window class.
 *
 * The \p class_name of a window is a name that is used to group windows which 
 * the Desktop Environment may be able to use.  An example of this grouping 
 * would be give all the windows a \p class_name of the application name.  
 * Another example would be to give a "file browser" \p class_name to a window 
 * that allows the user to navigate the file system.
 *
 * It is suggested to use a \p class_name that matches the basename of the 
 * application's .desktop file.  For example, "org.freedesktop.FooViewer" where 
 * the .desktop file is "org.freedesktop.FooViewer.desktop".
 *
 * \note See http://standards.freedesktop.org/desktop-entry-spec for more 
 * details.
 */
void Yetani::Window::classSet(const std::string& class_name ///< The class name
	) noexcept
{
	xdg_toplevel_set_app_id(xdg_toplevel, class_name.c_str());
}


/**
 * \brief Change the window title.
 *
 * The window's title can be changed by using this method.  Changing the title 
 * does not change the window's name.
 */
void Yetani::Window::titleSet(const std::string& title ///< The window title
	) noexcept
{
	xdg_toplevel_set_title(xdg_toplevel, title.c_str());
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
std::error_code Yetani::Window::decorationsSet(const Yetani::WindowDecorations decorations ///< The requested decorations
	) noexcept
{
	if(yetani->decoration_manager == nullptr)
	{
		return ZAKERO_YETANI__ERROR(Error_Server_Side_Decorations_Not_Available);
	}

	uint32_t decoration_state = decorations == Yetani::WindowDecorations::Server_Side
		? ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE
		: ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE
		;

	zxdg_toplevel_decoration_v1_set_mode(xdg_decoration
		, decoration_state
		);

	return ZAKERO_YETANI__ERROR(Error_None);
}


/**
 * \brief Get the current WindowMode
 *
 * Provides the current WindowMode of the %Window.
 *
 * \return Yetani::WindowMode
 */
Yetani::WindowMode Yetani::Window::windowMode() noexcept
{
	XdgToplevel& toplevel = yetani->xdg_toplevel_map[xdg_surface];

	Yetani::WindowMode window_mode = Yetani::toWindowMode(toplevel.window_state);

	return window_mode;
}


/**
 * \brief Check the WindowMode
 *
 * Compare the provided \p window_mode with the current window mode.
 *
 * \retval true  The WindowMode matches
 * \retval false The WindowMode's are different
 */
bool Yetani::Window::windowModeIs(const Yetani::WindowMode window_mode ///< The WindowMode
	) noexcept
{
	XdgState window_state = Yetani::toXdgState(window_mode);

	XdgToplevel& toplevel = yetani->xdg_toplevel_map[xdg_surface];

	if(toplevel.window_state == window_state)
	{
		return true;
	}

	return false;
}


/**
 * \brief Change the window mode.
 *
 * The current mode of a window can be changed programmatically by using this 
 * method.
 *
 * \see Yetani::WindowMode
 */
void Yetani::Window::windowModeSet(const Yetani::WindowMode window_mode ///< The WindowMode
	) noexcept
{
	XdgState window_state = Yetani::toXdgState(window_mode);

	XdgToplevel& toplevel = yetani->xdg_toplevel_map[xdg_surface];

	if(toplevel.window_state == window_state)
	{
		return;
	}

	switch(window_mode)
	{
		case WindowMode::Fullscreen:
			xdg_toplevel_set_fullscreen(xdg_toplevel, nullptr);
			break;

		case WindowMode::Maximized:
			xdg_toplevel_set_maximized(xdg_toplevel);
			break;

		case WindowMode::Normal:
			xdg_toplevel_unset_fullscreen(xdg_toplevel);
			xdg_toplevel_unset_maximized(xdg_toplevel);
			break;
	}
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
std::error_code Yetani::Window::sizeSet(const Yetani::SizeMm& size ///< The %Window size
	) noexcept
{
	if(size.width <= 0 || size.height <= 0)
	{
		return ZAKERO_YETANI__ERROR(Error_Window_Size_Too_Small);
	}

	Yetani::SizePixel size_pixel = convertToPixel(size);

	if(size_pixel.width <= 0)
	{
		size_pixel.width = 1;
	}

	if(size_pixel.height <= 0)
	{
		size_pixel.height = 1;
	}

	yetani->surface_resize_mutex_map[wl_surface].lock();
	{
		Yetani::SurfaceExtent& surface_extent = yetani->surface_extent_map[wl_surface];

		surface_extent.preferred_unit = Yetani::SizeUnit::Millimeter;
		surface_extent.preferred_mm   = size;
		surface_extent.size_pixel     = size_pixel;

		surfaceCalculateSize(yetani, wl_surface, size_pixel);
	}
	yetani->surface_resize_mutex_map[wl_surface].unlock();

	return ZAKERO_YETANI__ERROR(Error_None);
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
std::error_code Yetani::Window::sizeSet(const Yetani::SizePercent& size ///< The %Window size
	) noexcept
{
	if(size.width <= 0 || size.height <= 0)
	{
		return ZAKERO_YETANI__ERROR(Error_Window_Size_Too_Small);
	}

	Yetani::SizePixel size_pixel = convertToPixel(size);

	if(size_pixel.width <= 0)
	{
		size_pixel.width = 1;
	}

	if(size_pixel.height <= 0)
	{
		size_pixel.height = 1;
	}

	yetani->surface_resize_mutex_map[wl_surface].lock();
	{
		Yetani::SurfaceExtent& surface_extent = yetani->surface_extent_map[wl_surface];

		surface_extent.preferred_unit    = Yetani::SizeUnit::Percent;
		surface_extent.preferred_percent = size;
		surface_extent.size_pixel        = size_pixel;

		surfaceCalculateSize(yetani, wl_surface, size_pixel);
	}
	yetani->surface_resize_mutex_map[wl_surface].unlock();

	return ZAKERO_YETANI__ERROR(Error_None);
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
std::error_code Yetani::Window::sizeSet(const Yetani::SizePixel& size ///< The %Window size
	) noexcept
{
	if(size.width <= 0 || size.height <= 0)
	{
		return ZAKERO_YETANI__ERROR(Error_Window_Size_Too_Small);
	}

	yetani->surface_resize_mutex_map[wl_surface].lock();
	{
		Yetani::SurfaceExtent& surface_extent = yetani->surface_extent_map[wl_surface];

		surface_extent.preferred_unit = Yetani::SizeUnit::Pixel;
		surface_extent.size_pixel     = size;

		surfaceCalculateSize(yetani, wl_surface, size);
	}
	yetani->surface_resize_mutex_map[wl_surface].unlock();

	return ZAKERO_YETANI__ERROR(Error_None);
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
std::error_code Yetani::Window::sizeSetMinMax(const Yetani::SizeMm& size_min ///< The minimum size
	, const Yetani::SizeMm&                                     size_max ///< The maximum size
	) noexcept
{
	std::error_code error = validateMinMax<Yetani::SizeMm>(size_min, size_max);
	if(error)
	{
		return error;
	}

	Yetani::SizePixel size_pixel_min;
	Yetani::SizePixel size_pixel_max;

	{
		Yetani::OutputData& output_data = yetani->output_data;

		std::lock_guard<std::mutex> lock(output_data.mutex);

		Yetani::VectorWlOutput& vector    = output_data.surface_output_map[wl_surface];
		struct wl_output*       wl_output = vector.front();
		const Yetani::Output&   output    = output_data.output_map[wl_output];

		auto min = yetani->convertMmToPixel(output, size_min.width, size_min.height);
		size_pixel_min = { min.first, min.second };

		auto max = yetani->convertMmToPixel(output, size_max.width, size_max.height);
		size_pixel_max = { max.first, max.second };
	}

	yetani->xdgToplevelSizeMinMaxChange(yetani
		, xdg_toplevel
		, wl_surface
		, size_pixel_min
		, size_pixel_max
		);

	return ZAKERO_YETANI__ERROR(Error_None);
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
std::error_code Yetani::Window::sizeSetMinMax(const Yetani::SizePercent& size_min ///< The minimum size
	, const Yetani::SizePercent&                                     size_max ///< The maximum size
	) noexcept
{
	std::error_code error = validateMinMax<Yetani::SizePercent>(size_min, size_max);
	if(error)
	{
		return error;
	}

	Yetani::SizePixel size_pixel_min;
	Yetani::SizePixel size_pixel_max;

	{
		Yetani::OutputData& output_data = yetani->output_data;

		std::lock_guard<std::mutex> lock(output_data.mutex);

		Yetani::VectorWlOutput& vector    = output_data.surface_output_map[wl_surface];
		struct wl_output*       wl_output = vector.front();
		const Yetani::Output&   output    = output_data.output_map[wl_output];

		auto min = yetani->convertPercentToPixel(output, size_min.width, size_min.height);
		size_pixel_min = { min.first, min.second };

		auto max = yetani->convertPercentToPixel(output, size_max.width, size_max.height);
		size_pixel_max = { max.first, max.second };
	}

	yetani->xdgToplevelSizeMinMaxChange(yetani
		, xdg_toplevel
		, wl_surface
		, size_pixel_min
		, size_pixel_max
		);

	return ZAKERO_YETANI__ERROR(Error_None);
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
std::error_code Yetani::Window::sizeSetMinMax(const Yetani::SizePixel& size_min ///< The minimum size
	, const Yetani::SizePixel&                                     size_max ///< The maximum size
	) noexcept
{
	std::error_code error = validateMinMax<Yetani::SizePixel>(size_min, size_max);
	if(error)
	{
		return error;
	}

	yetani->xdgToplevelSizeMinMaxChange(yetani
		, xdg_toplevel
		, wl_surface
		, size_min
		, size_max
		);

	return ZAKERO_YETANI__ERROR(Error_None);
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
 * Yetani::SizePixel size;
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
std::error_code Yetani::Window::imageNext(uint8_t*& image ///< The image data
	, Yetani::SizePixel&                        size  ///< The image size
	) noexcept
{
	if(wl_buffer != nullptr)
	{
		bufferDestroy(wl_buffer);
	}

	Yetani::SurfaceSize& surface_size = yetani->surface_size_map[wl_surface];

	yetani->surface_resize_mutex_map[wl_surface].lock();
	{
		wl_buffer = bufferCreate(surface_size
			, &window_memory
			, &yetani->buffer
			);
	}
	yetani->surface_resize_mutex_map[wl_surface].unlock();

	image = window_memory.memory_pool.addressOf(
		yetani->buffer.map[wl_buffer].offset
		);

	size = { surface_size.width, surface_size.height }; 

	return ZAKERO_YETANI__ERROR(Error_None);
}


/**
 * \brief Render the image.
 *
 * Once the image data has been updated, this method will schedule the data to 
 * be rendered.
 */
void Yetani::Window::imagePresent() noexcept
{
	if(wl_buffer == nullptr)
	{
		// This nullptr check is needed because:
		// - If imagePresent() is called before imageNext() then 
		//   wl_buffer could be nullptr.
		// - There is a chance that a valid "buffer_next" can be
		//   replaced with a nullptr, and this will cause a
		//   frame-drop.

		return;
	}

	Yetani::SurfaceFrame& surface_frame = yetani->surface_frame_map[wl_surface];

	wl_buffer = surface_frame.buffer_next.exchange(wl_buffer);
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
uint32_t Yetani::Window::time() const noexcept
{
	Yetani::SurfaceFrame& surface_frame = yetani->surface_frame_map[wl_surface];

	return surface_frame.time_ms;
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
uint8_t Yetani::Window::bytesPerPixel() const noexcept
{
	return yetani->shmFormatBytesPerPixel(pixel_format);
}


/**
 * \brief Minimize the window.
 *
 * Using this method will remove the window from view.  The user will have to 
 * use the Desktop Environment to have the window redisplayed, perhaps using 
 * the task viewer.
 */
void Yetani::Window::minimize() noexcept
{
	xdg_toplevel_set_minimized(xdg_toplevel);
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
Yetani::PointMm Yetani::Window::convertToMm(const Yetani::PointPixel& point ///< The point to convert
	) const noexcept
{
	const Yetani::OutputData& output_data = yetani->output_data;

	std::lock_guard<std::mutex> lock(output_data.mutex);

	struct wl_output*     wl_output = output_data.surface_output_map.at(wl_surface).front();
	const Yetani::Output& output    = output_data.output_map.at(wl_output);

	auto p = yetani->convertPixelToMm(output, point.x, point.y);

	return { point.time, p.first, p.second };
}


/**
 * \brief Unit conversion.
 *
 * The provided \p point will be converted to a percentage.
 *
 * \return The converted point.
 */
Yetani::PointPercent Yetani::Window::convertToPercent(const Yetani::PointPixel& point ///< The point to convert
	) const noexcept
{
	const Yetani::OutputData& output_data = yetani->output_data;

	std::lock_guard<std::mutex> lock(output_data.mutex);

	struct wl_output*     wl_output = output_data.surface_output_map.at(wl_surface).front();
	const Yetani::Output& output    = output_data.output_map.at(wl_output);

	auto p = yetani->convertPixelToPercent(output, point.x, point.y);

	return { point.time, p.first, p.second };
}


/**
 * \brief Unit conversion.
 *
 * The provided \p point will be converted to pixels.
 *
 * \return The converted point.
 */
Yetani::PointPixel Yetani::Window::convertToPixel(const Yetani::PointMm& point ///< The point to convert
	) const noexcept
{
	Yetani::OutputData& output_data = yetani->output_data;

	std::lock_guard<std::mutex> lock(output_data.mutex);

	struct wl_output*     wl_output = output_data.surface_output_map[wl_surface].front();
	const Yetani::Output& output    = output_data.output_map[wl_output];

	auto p = yetani->convertMmToPixel(output, point.x, point.y);

	return { point.time, p.first, p.second };
}


/**
 * \brief Unit conversion.
 *
 * The provided \p point will be converted to pixels.
 *
 * \return The converted point.
 */
Yetani::PointPixel Yetani::Window::convertToPixel(const Yetani::PointPercent& point ///< The point to convert
	) const noexcept
{
	Yetani::OutputData& output_data = yetani->output_data;

	std::lock_guard<std::mutex> lock(output_data.mutex);

	struct wl_output*     wl_output = output_data.surface_output_map[wl_surface].front();
	const Yetani::Output& output    = output_data.output_map[wl_output];

	auto p = yetani->convertPercentToPixel(output, point.x, point.y);

	return { point.time, p.first, p.second };
}


/**
 * \brief Unit conversion.
 *
 * The provided \p size will be converted to millimeters.
 *
 * \return The converted size.
 */
Yetani::SizeMm Yetani::Window::convertToMm(const Yetani::SizePixel& size ///< The size to convert
	) const noexcept
{
	Yetani::OutputData& output_data = yetani->output_data;

	std::lock_guard<std::mutex> lock(output_data.mutex);

	struct wl_output*     wl_output = output_data.surface_output_map[wl_surface].front();
	const Yetani::Output& output    = output_data.output_map[wl_output];

	auto p = yetani->convertPixelToMm(output, size.width, size.height);

	return { p.first, p.second };
}


/**
 * \brief Unit conversion.
 *
 * The provided \p size will be converted to a percentage.
 *
 * \return The converted size.
 */
Yetani::SizePercent Yetani::Window::convertToPercent(const Yetani::SizePixel& size ///< The size to convert
	) const noexcept
{
	Yetani::OutputData& output_data = yetani->output_data;

	std::lock_guard<std::mutex> lock(output_data.mutex);

	struct wl_output*     wl_output = output_data.surface_output_map[wl_surface].front();
	const Yetani::Output& output    = output_data.output_map[wl_output];

	auto p = yetani->convertPixelToPercent(output, size.width, size.height);

	return { p.first, p.second };
}


/**
 * \brief Unit conversion.
 *
 * The provided \p size will be converted to pixels.
 *
 * \return The converted size.
 */
Yetani::SizePixel Yetani::Window::convertToPixel(const Yetani::SizeMm& size ///< The size to convert
	) const noexcept
{
	Yetani::OutputData& output_data = yetani->output_data;

	std::lock_guard<std::mutex> lock(output_data.mutex);

	struct wl_output*     wl_output = output_data.surface_output_map[wl_surface].front();
	const Yetani::Output& output    = output_data.output_map[wl_output];

	auto p = yetani->convertMmToPixel(output, size.width, size.height);

	return { p.first, p.second };
}


/**
 * \brief Unit conversion.
 *
 * The provided \p size will be converted to pixels.
 *
 * \return The converted size.
 */
Yetani::SizePixel Yetani::Window::convertToPixel(const Yetani::SizePercent& size ///< The size to convert
	) const noexcept
{
	Yetani::OutputData& output_data = yetani->output_data;

	std::lock_guard<std::mutex> lock(output_data.mutex);

	struct wl_output*     wl_output = output_data.surface_output_map[wl_surface].front();
	const Yetani::Output& output    = output_data.output_map[wl_output];

	auto p = yetani->convertPercentToPixel(output, size.width, size.height);

	return { p.first, p.second };
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
 * \nullptr as the \p lambda value.
 */
void Yetani::Window::onCloseRequest(Yetani::Lambda lambda ///< The lambda
	) noexcept
{
	XdgToplevel& toplevel = yetani->xdg_toplevel_map[xdg_surface];

	if(lambda == nullptr)
	{
		toplevel.close_request_lambda = Lambda_DoNothing;
	}
	else
	{
		toplevel.close_request_lambda = lambda;
	}
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
void Yetani::Window::decorationsOnChange(Yetani::LambdaWindowDecorations lambda ///< The lambda
	) noexcept
{
	XdgDecoration& decoration = yetani->xdg_decoration_map[xdg_surface];

	if(lambda == nullptr)
	{
		decoration.lambda = LambdaWindowDecorations_DoNothing;
	}
	else
	{
		decoration.lambda = lambda;
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
 * \nullptr as the \p lambda value.
 *
 * \note Execution of the lambda will block the Yetani object's event handling.  
 * So keep the lambda as small and simple as possible for best performance.
 */
void Yetani::Window::onFocusChange(Yetani::LambdaBool lambda ///< The lambda
	) noexcept
{
	XdgToplevel& toplevel = yetani->xdg_toplevel_map[xdg_surface];

	if(lambda == nullptr)
	{
		toplevel.is_active_lambda = LambdaBool_DoNothing;
	}
	else
	{
		toplevel.is_active_lambda = lambda;
	}
}


/**
 * \brief Respond to "Keyboard Enter" events.
 *
 * When a window gains keyboard focus, the provided \p lambda will be called.
 */
void Yetani::Window::keyboardOnEnter(Yetani::Lambda lambda ///< The lambda
	) noexcept
{
	if(yetani->keyboard.event_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::KeyboardEvent& event = yetani->keyboard.event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_enter = Lambda_DoNothing;
	}
	else
	{
		event.on_enter = lambda;
	}
}


/**
 * \brief Respond to "Keyboard Leave" events.
 *
 * When a window loses keyboard focus, the provided \p lambda will be called.
 */
void Yetani::Window::keyboardOnLeave(Yetani::Lambda lambda ///< The lambda
	) noexcept
{
	if(yetani->keyboard.event_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::KeyboardEvent& event = yetani->keyboard.event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_leave = Lambda_DoNothing;
	}
	else
	{
		event.on_leave = lambda;
	}
}


/**
 * \brief Respond to "Keyboard Key" events.
 *
 * When a Window has _Keyboard Focus_, it will be notified of key press, key 
 * repeat, and key release events.  The provided \p lambda will be called when 
 * these events happen.
 */
void Yetani::Window::keyboardOnKey(Yetani::LambdaKey lambda ///< The lambda
	) noexcept
{
	if(yetani->keyboard.event_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::KeyboardEvent& event = yetani->keyboard.event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_key = LambdaKey_DoNothing;
	}
	else
	{
		event.on_key = lambda;
	}
}


/**
 * \brief Respond to "Window Mode" events.
 *
 * The Desktop Environment is able to change the %Window's mode.  When that 
 * event happens, the provided \p lambda will be called.
 */
void Yetani::Window::windowModeOnChange(Yetani::LambdaWindowMode lambda ///< The lambda
	) noexcept
{
	XdgToplevel& toplevel = yetani->xdg_toplevel_map[xdg_surface];

	if(lambda == nullptr)
	{
		toplevel.window_state_lambda = LambdaWindowMode_DoNothing;
	}
	else
	{
		toplevel.window_state_lambda = lambda;
	}
}


/**
 * \brief Respond to "Resize" events.
 *
 * "Resize" events are unique to Yetani.  After a window has been resized, the 
 * provided \p lambda will be called.  This event is triggered by the user 
 * manually resizing the %Window.
 *
 * If a lambda has been previously set that needs to be removed, then pass a 
 * \nullptr as the \p lambda value.
 *
 * \note Execution of the lambda will block the Yetani object's event handling.  
 * So keep the lambda as small and simple as possible for best performance.
 */
void Yetani::Window::sizeOnChange(Yetani::LambdaSizeMm lambda ///< The lambda
	) noexcept
{
	Yetani::SurfaceEvent& event = yetani->surface_event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_size_mm_change = LambdaSizeMm_DoNothing;
	}
	else
	{
		event.on_size_mm_change = lambda;
	}
}


/**
 * \brief Respond to "Resize" events.
 *
 * "Resize" events are unique to Yetani.  After a window has been resized, the 
 * provided \p lambda will be called.  This event is triggered by the user 
 * manually resizing the %Window.
 *
 * If a lambda has been previously set that needs to be removed, then pass a 
 * \nullptr as the \p lambda value.
 *
 * \note Execution of the lambda will block the Yetani object's event handling.  
 * So keep the lambda as small and simple as possible for best performance.
 */
void Yetani::Window::sizeOnChange(Yetani::LambdaSizePercent lambda ///< The lambda
	) noexcept
{
	Yetani::SurfaceEvent& event = yetani->surface_event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_size_percent_change = LambdaSizePercent_DoNothing;
	}
	else
	{
		event.on_size_percent_change = lambda;
	}
}


/**
 * \brief Respond to "Resize" events.
 *
 * "Resize" events are unique to Yetani.  After a window has been resized, the 
 * provided \p lambda will be called.  This event is triggered by the user 
 * manually resizing the %Window.
 *
 * If a lambda has been previously set that needs to be removed, then pass a 
 * \nullptr as the \p lambda value.
 *
 * \note Execution of the lambda will block the Yetani object's event handling.  
 * So keep the lambda as small and simple as possible for best performance.
 */
void Yetani::Window::sizeOnChange(Yetani::LambdaSizePixel lambda ///< The lambda
	) noexcept
{
	Yetani::SurfaceEvent& event = yetani->surface_event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_size_pixel_change = LambdaSizePixel_DoNothing;
	}
	else
	{
		event.on_size_pixel_change = lambda;
	}
}


/**
 * \brief Respond to "Pointer Button" events.
 *
 * When the user clicks on a _mouse button_ or some other pointer device, it 
 * generates a "Pointer Button" event.  The provided \p lambda will be called 
 * when that event occurs.
 */
void Yetani::Window::pointerOnButton(Yetani::LambdaButtonMm lambda ///< The lambda
	) noexcept
{
	if(yetani->pointer.event_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::PointerEvent& event = yetani->pointer.event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_button_mm = LambdaButtonMm_DoNothing;
	}
	else
	{
		event.on_button_mm = lambda;
	}
}


/**
 * \brief Respond to "Pointer Button" events.
 *
 * When the user clicks on a _mouse button_ or some other pointer device, it 
 * generates a "Pointer Button" event.  The provided \p lambda will be called 
 * when that event occurs.
 */
void Yetani::Window::pointerOnButton(Yetani::LambdaButtonPercent lambda ///< The lambda
	) noexcept
{
	if(yetani->pointer.event_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::PointerEvent& event = yetani->pointer.event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_button_percent = LambdaButtonPercent_DoNothing;
	}
	else
	{
		event.on_button_percent = lambda;
	}
}


/**
 * \brief Respond to "Pointer Button" events.
 *
 * When the user clicks on a _mouse button_ or some other pointer device, it 
 * generates a "Pointer Button" event.  The provided \p lambda will be called 
 * when that event occurs.
 */
void Yetani::Window::pointerOnButton(Yetani::LambdaButtonPixel lambda ///< The lambda
	) noexcept
{
	if(yetani->pointer.event_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::PointerEvent& event = yetani->pointer.event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_button_pixel = LambdaButtonPixel_DoNothing;
	}
	else
	{
		event.on_button_pixel = lambda;
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
void Yetani::Window::pointerOnEnter(Yetani::LambdaPointMm lambda ///< The lambda
	) noexcept
{
	if(yetani->pointer.event_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::PointerEvent& event = yetani->pointer.event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_enter_mm = LambdaPointMm_DoNothing;
	}
	else
	{
		event.on_enter_mm = lambda;
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
void Yetani::Window::pointerOnEnter(Yetani::LambdaPointPercent lambda ///< The lambda
	) noexcept
{
	if(yetani->pointer.event_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::PointerEvent& event = yetani->pointer.event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_enter_percent = LambdaPointPercent_DoNothing;
	}
	else
	{
		event.on_enter_percent = lambda;
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
void Yetani::Window::pointerOnEnter(Yetani::LambdaPointPixel lambda ///< The lambda
	) noexcept
{
	if(yetani->pointer.event_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::PointerEvent& event = yetani->pointer.event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_enter_pixel = LambdaPointPixel_DoNothing;
	}
	else
	{
		event.on_enter_pixel = lambda;
	}
}


/**
 * \brief Respond to "Pointer Leave" events.
 *
 * When the _mouse_ or some other pointer device leave the %Window, a "Pointer 
 * Leave" event occurs.  The provided \p lambda will be called when that event 
 * happens.
 */
void Yetani::Window::pointerOnLeave(Yetani::Lambda lambda ///< The lambda
	) noexcept
{
	if(yetani->pointer.event_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::PointerEvent& event = yetani->pointer.event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_leave = Lambda_DoNothing;
	}
	else
	{
		event.on_leave = lambda;
	}
}


/**
 * \brief Respond to "Pointer Motion" events.
 *
 * When the _mouse_ or some other pointer device moves in the %Window, a 
 * "Pointer Motion" event occurs.  The provided \p lambda will be called when 
 * that event happens.
 */
void Yetani::Window::pointerOnMotion(Yetani::LambdaPointMm lambda ///< The lambda
	) noexcept
{
	if(yetani->pointer.event_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::PointerEvent& event = yetani->pointer.event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_motion_mm = LambdaPointMm_DoNothing;
	}
	else
	{
		event.on_motion_mm = lambda;
	}
}


/**
 * \brief Respond to "Pointer Motion" events.
 *
 * When the _mouse_ or some other pointer device moves in the %Window, a 
 * "Pointer Motion" event occurs.  The provided \p lambda will be called when 
 * that event happens.
 */
void Yetani::Window::pointerOnMotion(Yetani::LambdaPointPercent lambda ///< The lambda
	) noexcept
{
	if(yetani->pointer.event_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::PointerEvent& event = yetani->pointer.event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_motion_percent = LambdaPointPercent_DoNothing;
	}
	else
	{
		event.on_motion_percent = lambda;
	}
}


/**
 * \brief Respond to "Pointer Motion" events.
 *
 * When the _mouse_ or some other pointer device moves in the %Window, a 
 * "Pointer Motion" event occurs.  The provided \p lambda will be called when 
 * that event happens.
 */
void Yetani::Window::pointerOnMotion(Yetani::LambdaPointPixel lambda ///< The lambda
	) noexcept
{
	if(yetani->pointer.event_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::PointerEvent& event = yetani->pointer.event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_motion_pixel = LambdaPointPixel_DoNothing;
	}
	else
	{
		event.on_motion_pixel = lambda;
	}
}


/**
 * \brief Respond to "Pointer Axis" events.
 *
 * Some pointer devices have a "wheel" that can be rotated.  The provided \p 
 * lambda will be called when these "Pointer Axis" events happen.
 */
void Yetani::Window::pointerOnAxis(Yetani::LambdaAxis lambda ///< The lambda
	) noexcept
{
	if(yetani->pointer.event_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::PointerEvent& event = yetani->pointer.event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_axis = LambdaAxis_DoNothing;
	}
	else
	{
		event.on_axis = lambda;
	}
}


/**
 * \brief Respond to "Pointer Axis Source" events.
 *
 * The provided \p lambda will be called when the "Pointer Axis Source" events 
 * occur.
 */
void Yetani::Window::pointerOnAxisSource(Yetani::Lambda lambda ///< The lambda
	) noexcept
{
	if(yetani->pointer.event_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::PointerEvent& event = yetani->pointer.event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_axis_source = Lambda_DoNothing;
	}
	else
	{
		event.on_axis_source = lambda;
	}
}


/**
 * \brief Respond to "Pointer Axis Stop" events.
 *
 * The provided \p lambda will be called when the "Pointer Axis Stop" events 
 * occur.
 */
void Yetani::Window::pointerOnAxisStop(Yetani::Lambda lambda ///< The lambda
	) noexcept
{
	if(yetani->pointer.event_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::PointerEvent& event = yetani->pointer.event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_axis_stop = Lambda_DoNothing;
	}
	else
	{
		event.on_axis_stop = lambda;
	}
}


/**
 * \brief Respond to "Pointer Axis Discrete" events.
 *
 * The provided \p lambda will be called when the "Pointer Axis Discrete" 
 * events occur.
 */
void Yetani::Window::pointerOnAxisDiscrete(Yetani::Lambda lambda ///< The lambda
	) noexcept
{
	if(yetani->pointer.event_map.contains(wl_surface) == false)
	{
		return;
	}

	Yetani::PointerEvent& event = yetani->pointer.event_map[wl_surface];

	if(lambda == nullptr)
	{
		event.on_axis_discrete = Lambda_DoNothing;
	}
	else
	{
		event.on_axis_discrete = lambda;
	}
}

// }}}
// {{{ Window : Helpers


// }}}
// {{{ Convenience

/**
 * \brief Convert a value to a std::string.
 *
 * The %Wayland SHM color format will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const wl_shm_format& shm_format ///< The value
	) noexcept
{
	return Yetani::shmFormatName(shm_format);
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p key data will be converted into a JSON formatted std::string.
 *
 * \return A string
 */
std::string to_string(const Yetani::Key& key ///< The value
	) noexcept
{
	return std::string()
		+ "{ \"time\": "    + std::to_string(key.time)
		+ ", \"code\": "    + std::to_string(key.code)
		+ ", \"state\": \"" + zakero::to_string(key.state) + "\""
		+ " }";
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p key_modifier data will be converted into a JSON formatted 
 * std::string.
 *
 * \return A string
 */
std::string to_string(const Yetani::KeyModifier& key_modifier ///< The value
	) noexcept
{
	auto mod_to_str = [](std::string& s, uint32_t m)
	{
		s += "[";
		std::string delim = "";

		if(m & Yetani::KeyModifier_Shift)
		{
			s += delim + "\"Shift\"";
			delim = ",";
		}

		if(m & Yetani::KeyModifier_CapsLock)
		{
			s += delim + "\"CapsLock\"";
			delim = ",";
		}

		if(m & Yetani::KeyModifier_Control)
		{
			s += delim + "\"Control\"";
			delim = ",";
		}

		if(m & Yetani::KeyModifier_Alt)
		{
			s += delim + "\"Alt\"";
			delim = ",";
		}

		if(m & Yetani::KeyModifier_NumLock)
		{
			s += delim + "\"NumLock\"";
		}

		if(m & Yetani::KeyModifier_Meta)
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
std::string to_string(const Yetani::KeyState& key_state ///< The value
	) noexcept
{
	switch(key_state)
	{
		case Yetani::KeyState::Pressed:  return "Pressed";
		case Yetani::KeyState::Released: return "Released";
		case Yetani::KeyState::Repeat:   return "Repeat";
		default:                         return "";
	}
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p output data will be converted into a JSON formatted std::string.
 *
 * \return A string
 */
std::string to_string(const Yetani::Output& output ///< The value
	) noexcept
{
	return std::string()
		+ "{ \"x\": "                        + std::to_string(output.x)
		+ ", \"y\": "                        + std::to_string(output.y)
		+ ", \"physical_width_mm\": "        + std::to_string(output.physical_width_mm)
		+ ", \"physical_height_mm\": "       + std::to_string(output.physical_height_mm)
		+ ", \"subpixel\": "                 + std::to_string(output.subpixel)
		+ ", \"subpixel_name\": \""          + Yetani::outputSubpixelName(output.subpixel) + "\""
		+ ", \"make\": \""                   + output.make + "\""
		+ ", \"model\": \""                  + output.model + "\""
		+ ", \"transform\": "                + std::to_string(output.transform)
		+ ", \"transform_name\": \""         + Yetani::outputTransformName(output.transform) + "\""
		+ ", \"flags\": "                    + std::to_string(output.flags)
		+ ", \"width\": "                    + std::to_string(output.width)
		+ ", \"height\": "                   + std::to_string(output.height)
		+ ", \"refresh_mHz\": "              + std::to_string(output.refresh_mHz)
		+ ", \"scale_factor\": "             + std::to_string(output.scale_factor)
		+ ", \"pixels_per_mm_horizontal\": " + std::to_string(output.pixels_per_mm_horizontal)
		+ ", \"pixels_per_mm_vertical\": "   + std::to_string(output.pixels_per_mm_vertical)
		+ " }";
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p point will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Yetani::PointMm& point ///< The value
	) noexcept
{
	return std::string()
		+ "{ \"time\": " + std::to_string(point.time)
		+ ", \"x\": "    + std::to_string(point.x)
		+ ", \"y\": "    + std::to_string(point.y)
		+ " }";
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p point will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Yetani::PointPercent& point ///< The value
	) noexcept
{
	return std::string()
		+ "{ \"time\": " + std::to_string(point.time)
		+ ", \"x\": "    + std::to_string(point.x)
		+ ", \"y\": "    + std::to_string(point.y)
		+ " }";
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p point will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Yetani::PointPixel& point ///< The value
	) noexcept
{
	return std::string()
		+ "{ \"time\": " + std::to_string(point.time)
		+ ", \"x\": "    + std::to_string(point.x)
		+ ", \"y\": "    + std::to_string(point.y)
		+ " }";
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p axis will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Yetani::PointerAxis& axis ///< The value
	) noexcept
{
	return std::string()
		+ "{ \"time\": "     + std::to_string(axis.time)
		+ ", \"steps\": "    + std::to_string(axis.steps)
		+ ", \"distance\": " + std::to_string(axis.distance)
		+ ", \"source\": "   + zakero::to_string(axis.source)
		+ ", \"type\": "     + zakero::to_string(axis.type)
		+ " }";
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p source will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Yetani::PointerAxisSource& source ///< The value
	) noexcept
{
	switch(source)
	{
		case Yetani::PointerAxisSource::Continuous: return "Continuous";
		case Yetani::PointerAxisSource::Finger:     return "Finger";
		case Yetani::PointerAxisSource::Wheel:      return "Wheel";
		case Yetani::PointerAxisSource::Wheel_Tilt: return "Wheel Tilt";
		case Yetani::PointerAxisSource::Unknown:    [[fallthrough]];
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
std::string to_string(const Yetani::PointerAxisType& type ///< The value
	) noexcept
{
	switch(type)
	{
		case Yetani::PointerAxisType::Horizontal: return "Horizontal";
		case Yetani::PointerAxisType::Vertical:   return "Vertical";
		case Yetani::PointerAxisType::Unknown:    [[fallthrough]];
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
std::string to_string(const Yetani::PointerButton& button ///< The value
	) noexcept
{
	return std::string()
		+ "{ \"code\": " + std::to_string(button.code)
		+ ", \"state\": " + zakero::to_string(button.state)
		+ " }";
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p button_state will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Yetani::PointerButtonState& button_state ///< The value
	) noexcept
{
	switch(button_state)
	{
		case Yetani::PointerButtonState::Pressed:  return "Pressed";
		case Yetani::PointerButtonState::Released: return "Released";
		default:                                   return "";
	}
}


/**
 * \brief Convert a value to a std::string.
 *
 * The \p size will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Yetani::SizeMm& size ///< The value
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
std::string to_string(const Yetani::SizePercent& size ///< The value
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
std::string to_string(const Yetani::SizePixel& size ///< The value
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
 * The \p window_mode value will be converted into a std::string.
 *
 * \return A string
 */
std::string to_string(const Yetani::WindowMode& window_mode ///< The value
	) noexcept
{
	switch(window_mode)
	{
		case Yetani::WindowMode::Fullscreen: return "Fullscreen";
		case Yetani::WindowMode::Maximized:  return "Maximized";
		case Yetani::WindowMode::Normal:     return "Normal";
		default:                             return "";
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
bool operator==(Yetani::PointMm& lhs ///< Left-Hand side
	, Yetani::PointMm&       rhs ///< Right-Hand side
	) noexcept
{
	return zakero::equalish(lhs.x, rhs.x, 0.001)
		&&  zakero::equalish(lhs.y, rhs.y, 0.001)
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
bool operator==(Yetani::PointPercent& lhs ///< Left-Hand side
	, Yetani::PointPercent&       rhs ///< Right-Hand side
	) noexcept
{
	return zakero::equalish(lhs.x, rhs.x, 0.00001)
		&&  zakero::equalish(lhs.y, rhs.y, 0.00001)
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
bool operator==(Yetani::PointPixel& lhs ///< Left-Hand side
	, Yetani::PointPixel&       rhs ///< Right-Hand side
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
bool operator==(Yetani::SizeMm& lhs ///< Left-Hand side
	, Yetani::SizeMm&       rhs ///< Right-Hand side
	) noexcept
{
	return zakero::equalish(lhs.width, rhs.width, 0.001)
		&&  zakero::equalish(lhs.height, rhs.height, 0.001)
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
bool operator==(Yetani::SizePercent& lhs ///< Left-Hand side
	, Yetani::SizePercent&       rhs ///< Right-Hand side
	) noexcept
{
	return zakero::equalish(lhs.width, rhs.width, 0.00001)
		&&  zakero::equalish(lhs.height, rhs.height, 0.00001)
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
bool operator==(Yetani::SizePixel& lhs ///< Left-Hand side
	, Yetani::SizePixel&       rhs ///< Right-Hand side
	) noexcept
{
	return (lhs.width == rhs.width) && (lhs.height == rhs.height);
}

// }}}

}

#endif // ZAKERO_YETANI_IMPLEMENTATION

// }}}

#endif // zakero_Yetani_h
