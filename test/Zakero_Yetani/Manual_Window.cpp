/*
g++ -std=c++20 -g -Wall -Werror -lpthread -lwayland-client -I../../include -o Manual_Window Manual_Window.cpp && ./Manual_Window
 */

/*
#define ZAKERO_PROFILER_IMPLEMENTATION
#include "Zakero_Profiler.h"
*/

#define ZAKERO_MEMORYPOOL_IMPLEMENTATION
#include "Zakero_MemoryPool.h"

#define ZAKERO_YETANI_IMPLEMENTATION
#define ZAKERO_YETANI_ENABLE_DEBUG
#define ZAKERO_YETANI_ENABLE_SAFE_MODE

#include "Zakero_Yetani.h"

namespace
{
}


int main()
{
	std::error_code error;

	zakero::Yetani* yetani = zakero::Yetani::connect(error);
	if(error)
	{
		std::cout << "Error: " << error << '\n';
		return 1;
	}

	// sizeof check
	printf("size: %ld\n", sizeof(*yetani));

	yetani->outputOnAdd([](const zakero::Yetani::OutputId id)
	{
		printf("--- Output Added: %u\n", id);
	});
	yetani->outputOnChange([](const zakero::Yetani::OutputId id)
	{
		printf("--- Output Changed: %u\n", id);
	});
	yetani->outputOnRemove([](const zakero::Yetani::OutputId id)
	{
		printf("--- Output Removed: %u\n", id);
	});

	printf("Available Pixel Formats:\n");
	auto list = yetani->shmFormatAvailable();
	for(const auto& format : list)
	{
		printf("- %s (%d) %s\n"
			, yetani->shmFormatName(format).c_str()
			, format
			, yetani->shmFormatDescription(format).c_str()
			);
	}

	uint32_t cursor_1_data[4][25] =
	{ { 0xffff'ffff, 0x7fff'ffff, 0x0000'0000, 0x7fff'ffff, 0xffff'ffff
	  , 0x7fff'ffff, 0x7f00'0000, 0x7f00'0000, 0x7f00'0000, 0x7fff'ffff
	  , 0x0000'0000, 0x7f00'0000, 0xffff'ffff, 0x7f00'0000, 0x0000'0000
	  , 0x7fff'ffff, 0x7f00'0000, 0x7f00'0000, 0x7f00'0000, 0x7fff'ffff
	  , 0xffff'ffff, 0x7fff'ffff, 0x0000'0000, 0x7fff'ffff, 0xffff'ffff
	  }
	, { 0x7fff'ffff, 0xffff'ffff, 0x7fff'ffff, 0x0000'0000, 0x7fff'ffff
	  , 0x0000'0000, 0x7f00'0000, 0x7f00'0000, 0x7f00'0000, 0xffff'ffff
	  , 0x7fff'ffff, 0x7f00'0000, 0xffff'ffff, 0x7f00'0000, 0x7fff'ffff
	  , 0xffff'ffff, 0x7f00'0000, 0x7f00'0000, 0x7f00'0000, 0x0000'0000
	  , 0x7fff'ffff, 0x0000'0000, 0x7fff'ffff, 0xffff'ffff, 0x7fff'ffff
	  }
	, { 0x0000'0000, 0x7fff'ffff, 0xffff'ffff, 0x7fff'ffff, 0x0000'0000
	  , 0x7fff'ffff, 0x7f00'0000, 0x7f00'0000, 0x7f00'0000, 0x7fff'ffff
	  , 0xffff'ffff, 0x7f00'0000, 0xffff'ffff, 0x7f00'0000, 0xffff'ffff
	  , 0x7fff'ffff, 0x7f00'0000, 0x7f00'0000, 0x7f00'0000, 0x7fff'ffff
	  , 0x0000'0000, 0x7fff'ffff, 0xffff'ffff, 0x7fff'ffff, 0x0000'0000
	  }
	, { 0x7fff'ffff, 0x0000'0000, 0x7fff'ffff, 0xffff'ffff, 0x7fff'ffff
	  , 0xffff'ffff, 0x7f00'0000, 0x7f00'0000, 0x7f00'0000, 0x0000'0000
	  , 0x7fff'ffff, 0x7f00'0000, 0xffff'ffff, 0x7f00'0000, 0x7fff'ffff
	  , 0x0000'0000, 0x7f00'0000, 0x7f00'0000, 0x7f00'0000, 0xffff'ffff
	  , 0x7fff'ffff, 0xffff'ffff, 0x7fff'ffff, 0x0000'0000, 0x7fff'ffff
	  }
	};

	std::vector<void*> cursor_1(4, nullptr);
	cursor_1[0] = &cursor_1_data[0][0];
	cursor_1[1] = &cursor_1_data[1][0];
	cursor_1[2] = &cursor_1_data[2][0];
	cursor_1[3] = &cursor_1_data[3][0];

	error = yetani->cursorCreate("cursor_1",
		{	.size           = { 5, 5 }
		,	.format         = WL_SHM_FORMAT_ARGB8888
		,	.hotspot_x      = 2
		,	.hotspot_y      = 2
		,	.time_per_frame = std::chrono::milliseconds(64)
		,	.image_data     = cursor_1
		});
	if(error)
	{
		printf("Failed to create cursor: %s\n", error.message().c_str());
	}

	bool time_to_die = false;

	uint32_t red_x_offset = 0;
	zakero::Yetani::SizePixel red_size = { 1, 1 };
	//zakero::Yetani::SizeMm    red_window_size = { 160, 90 };
	//zakero::Yetani::SizeMm    red_window_size = { 30, 30 };
	zakero::Yetani::SizePixel red_window_size = { 640, 400 };
	zakero::Yetani::Window* red = yetani->windowCreate(red_window_size
		, WL_SHM_FORMAT_ARGB8888
		, error
		);
	red->cursorUse("cursor_1");

	red->classSet("Yetani: Manual Window");
	red->titleSet("Red XOR Pattern");
	//red->decorationsSet(zakero::Yetani::WindowDecorations::Client_Side);
	red->decorationsSet(zakero::Yetani::WindowDecorations::Server_Side);
	//red->setMode(zakero::Yetani::WindowMode::Fullscreen);
	//red->setMode(zakero::Yetani::WindowMode::Maximized);
	//red->sizeSet(zakero::Yetani::SizeMm{ 150, 100 });
	//red->sizeSet(zakero::Yetani::SizePercent{ 0.25, 0.5 });
	//red->sizeSet(zakero::Yetani::SizePixel{ 100, 100});
	//red->sizeSetMinMax(zakero::Yetani::SizeMm{ 100, 100} , zakero::Yetani::SizeMm{ 150, 150});
	//red->minimize();

	red->onCloseRequest([&]()
	{
		time_to_die = true;

		printf("************************************************\n");
		printf("** Red: Close Request\n");
		printf("************************************************\n");
	});
/*
	red->decorationsOnChange([&](const zakero::Yetani::WindowDecorations mode)
	{
		printf("************************************************\n");
		printf("** Red: Using %s-Side Borders\n"
			, (mode == zakero::Yetani::WindowDecorations::Server_Side
				? "Server"
				: "Client"
				)
			);
		printf("************************************************\n");
	});
	red->onFocusChange([&](const bool has_focus)
	{
		printf("************************************************\n");
		printf("** Red: %s Focus\n", (has_focus ? "Gained" : "Lost"));
		printf("************************************************\n");
	});
	red->onModeChange([&](const zakero::Yetani::WindowMode window_mode)
	{
		printf("************************************************\n");
		printf("** Red: %s\n"
			, zakero::to_string(window_mode).c_str()
			);
		printf("************************************************\n");
	});
	red->onSizeChange([&](const zakero::Yetani::SizeMm& size)
	{
		printf("************************************************\n");
		printf("** Red Millimeter: %f x %f\n", size.width, size.height);
		printf("************************************************\n");
	});
	red->onSizeChange([&](const zakero::Yetani::SizePercent& size)
	{
		printf("************************************************\n");
		printf("** Red Percent: %f x %f\n", size.width, size.height);
		printf("************************************************\n");
	});
	red->onSizeChange([&](const zakero::Yetani::SizePixel& size)
	{
		printf("************************************************\n");
		printf("** Red Pixel: %d x %d\n", size.width, size.height);
		printf("************************************************\n");
	});
*/
/*
	red->pointerOnEnter([&](const zakero::Yetani::PointMm& point, const zakero::Yetani::KeyModifier&)
	{
		printf("************************************************\n");
		printf("** Red: Pointer Enter = %f x %f\n", point.x, point.y);
		printf("************************************************\n");
	});
	red->pointerOnLeave([&]()
	{
		printf("************************************************\n");
		printf("** Red: Pointer Leave\n");
		printf("************************************************\n");
	});
*/
/*
	red->pointerOnMotion([&](const zakero::Yetani::PointMm& point
		, const zakero::Yetani::KeyModifier& modifier
		)
	{
		printf("************************************************\n");
		printf("** Red: Pointer Motion = %f x %f (%u)\n", point.x, point.y, point.time);
		printf("**    : %s\n", zakero::to_string(modifier).c_str());
		printf("************************************************\n");
	});
*/
/*
	red->pointerOnButton([&](const zakero::Yetani::PointerButton& button
		, const zakero::Yetani::PointMm&     point
		, const zakero::Yetani::KeyModifier& modifier
		)
	{
		printf("************************************************\n");
		printf("** Red: Pointer Button = %f x %f (%u) %u %s\n"
			, point.x
			, point.y
			, point.time
			, button.code
			, zakero::to_string(button.state).c_str()
			);
		printf("**    : %s\n", zakero::to_string(modifier).c_str());
		printf("************************************************\n");
	});
*/

	red->pointerOnAxis([&](const zakero::Yetani::PointerAxis& axis
		, const zakero::Yetani::KeyModifier&              mod
		)
	{
		printf(">> Red: Pointer Axis: %s Mod: %s\n"
			, zakero::to_string(axis).c_str()
			, zakero::to_string(mod).c_str()
			);
		printf("************************************************\n");
		printf("** Red: Pointer Axis = %s %f %d %s (%u) Mod: %s\n"
			, zakero::to_string(axis.type).c_str()
			, axis.distance
			, axis.steps
			, zakero::to_string(axis.source).c_str()
			, axis.time
			, zakero::to_string(mod).c_str()
			);
		printf("************************************************\n");
	});
/*
	red->keyboardOnEnter([&]()
	{
		printf("************************************************\n");
		printf("** Red: Keyboard Enter\n");
		printf("************************************************\n");
	});
	red->keyboardOnLeave([&]()
	{
		printf("************************************************\n");
		printf("** Red: Keyboard Leave\n");
		printf("************************************************\n");
	});
	red->keyboardOnKey([&](const zakero::Yetani::Key& key, const zakero::Yetani::KeyModifier& modifier)
	{
		printf("************************************************\n");
		printf("** Red: Keyboard Key %d %s (%u)\n"
			, key.code
			, zakero::to_string(key.state).c_str()
			, key.time
			);
		printf("**    : %s\n", zakero::to_string(modifier).c_str());
		printf("************************************************\n");
	});
*/



	uint32_t green_y_offset = 0;
	zakero::Yetani::SizePixel green_size = {0, 0};
#if 1 // Window__
	zakero::Yetani::Window* green = nullptr;
#else
	zakero::Yetani::Window* green = yetani->windowCreate(zakero::Yetani::SizeMm{ 160, 90 }
		, WL_SHM_FORMAT_XRGB8888
		, error
		);
	green->classSet("Yetani: Manual Window");
	green->titleSet("Green XOR Pattern");
	//green->sizeSetMinMax(zakero::Yetani::SizeMm{  80,  45}, zakero::Yetani::SizeMm{ 240, 135});
	green->decorationsOnChange([&](const zakero::Yetani::WindowDecorations mode)
	{
		printf("************************************************\n");
		printf("** Green: Using %s-Side Borders\n"
			, (mode == zakero::Yetani::WindowDecorations::Server_Side
				? "Server"
				: "Client"
				)
			);
		printf("************************************************\n");
	});
	green->onFocusChange([&](const bool has_focus)
	{
		printf("************************************************\n");
		printf("** Green: %s Focus\n", (has_focus ? "Gained" : "Lost"));
		printf("************************************************\n");
	});
	green->onSizeChange([&](const zakero::Yetani::SizeMm& size)
	{
		printf("************************************************\n");
		printf("** Green Millimeter: %f x %f\n", size.width, size.height);
		printf("************************************************\n");
	});
	green->onSizeChange([&](const zakero::Yetani::SizePercent& size)
	{
		printf("************************************************\n");
		printf("** Green Percent: %f x %f\n", size.width, size.height);
		printf("************************************************\n");
	});
	green->onSizeChange([&](const zakero::Yetani::SizePixel& size)
	{
		printf("************************************************\n");
		printf("** Green Pixel: %d x %d\n", size.width, size.height);
		printf("************************************************\n");
	});
	green->onCloseRequest([&]()
	{
		time_to_die = true;

		printf("************************************************\n");
		printf("** Green: Close Request\n");
		printf("************************************************\n");

		return false;
	});
#endif



	uint32_t blue_x_offset = 0;
	uint32_t blue_y_offset = 0;
	zakero::Yetani::SizePixel blue_size = {0, 0};
#if 1 // Window__
	zakero::Yetani::Window* blue = nullptr;
#else
	uint32_t cursor_2_data[16][25] =
	{ 0xbbff'ffff, 0xccff'ffff, 0xddff'ffff, 0xeeff'ffff, 0xffff'ffff
	, 0xaaff'ffff, 0xbb00'0000, 0xdd00'0000, 0xff00'0000, 0x00ff'ffff
	, 0x99ff'ffff, 0x9900'0000, 0xffff'ffff, 0x1100'0000, 0x11ff'ffff
	, 0x88ff'ffff, 0x7700'0000, 0x5500'0000, 0x3300'0000, 0x22ff'ffff
	, 0x77ff'ffff, 0x66ff'ffff, 0x55ff'ffff, 0x44ff'ffff, 0x33ff'ffff

	, 0xaaff'ffff, 0xbbff'ffff, 0xccff'ffff, 0xddff'ffff, 0xeeff'ffff
	, 0x99ff'ffff, 0x8800'0000, 0xaa00'0000, 0xcc00'0000, 0xffff'ffff
	, 0x88ff'ffff, 0x6600'0000, 0xffff'ffff, 0xee00'0000, 0x00ff'ffff
	, 0x77ff'ffff, 0x4400'0000, 0x2200'0000, 0x0000'0000, 0x11ff'ffff
	, 0x66ff'ffff, 0x55ff'ffff, 0x44ff'ffff, 0x33ff'ffff, 0x22ff'ffff

	, 0x99ff'ffff, 0xaaff'ffff, 0xbbff'ffff, 0xccff'ffff, 0xddff'ffff
	, 0x88ff'ffff, 0x9900'0000, 0xbb00'0000, 0xdd00'0000, 0xeeff'ffff
	, 0x77ff'ffff, 0x7700'0000, 0xffff'ffff, 0xff00'0000, 0xffff'ffff
	, 0x66ff'ffff, 0x5500'0000, 0x3300'0000, 0x1100'0000, 0x00ff'ffff
	, 0x55ff'ffff, 0x44ff'ffff, 0x33ff'ffff, 0x22ff'ffff, 0x11ff'ffff

	, 0x88ff'ffff, 0x99ff'ffff, 0xaaff'ffff, 0xbbff'ffff, 0xccff'ffff
	, 0x77ff'ffff, 0x6600'0000, 0x8800'0000, 0xaa00'0000, 0xddff'ffff
	, 0x66ff'ffff, 0x4400'0000, 0xffff'ffff, 0xcc00'0000, 0xeeff'ffff
	, 0x55ff'ffff, 0x2200'0000, 0x0000'0000, 0xee00'0000, 0xffff'ffff
	, 0x44ff'ffff, 0x33ff'ffff, 0x22ff'ffff, 0x11ff'ffff, 0x00ff'ffff

	// 4

	, 0x77ff'ffff, 0x88ff'ffff, 0x99ff'ffff, 0xaaff'ffff, 0xbbff'ffff
	, 0x66ff'ffff, 0x7700'0000, 0x9900'0000, 0xbb00'0000, 0xccff'ffff
	, 0x55ff'ffff, 0x5500'0000, 0xffff'ffff, 0xdd00'0000, 0xddff'ffff
	, 0x44ff'ffff, 0x3300'0000, 0x1100'0000, 0xff00'0000, 0xeeff'ffff
	, 0x33ff'ffff, 0x22ff'ffff, 0x11ff'ffff, 0x00ff'ffff, 0xffff'ffff

	, 0x66ff'ffff, 0x77ff'ffff, 0x88ff'ffff, 0x99ff'ffff, 0xaaff'ffff
	, 0x55ff'ffff, 0x4400'0000, 0x6600'0000, 0x8800'0000, 0xbbff'ffff
	, 0x44ff'ffff, 0x2200'0000, 0xffff'ffff, 0xaa00'0000, 0xccff'ffff
	, 0x33ff'ffff, 0x0000'0000, 0xee00'0000, 0xcc00'0000, 0xddff'ffff
	, 0x22ff'ffff, 0x11ff'ffff, 0x00ff'ffff, 0xffff'ffff, 0xeeff'ffff

	, 0x55ff'ffff, 0x66ff'ffff, 0x77ff'ffff, 0x88ff'ffff, 0x99ff'ffff
	, 0x44ff'ffff, 0x5500'0000, 0x7700'0000, 0x9900'0000, 0xaaff'ffff
	, 0x33ff'ffff, 0x3300'0000, 0xffff'ffff, 0xbb00'0000, 0xbbff'ffff
	, 0x22ff'ffff, 0x1100'0000, 0xff00'0000, 0xdd00'0000, 0xccff'ffff
	, 0x11ff'ffff, 0x00ff'ffff, 0xffff'ffff, 0xeeff'ffff, 0xddff'ffff

	, 0x44ff'ffff, 0x55ff'ffff, 0x66ff'ffff, 0x77ff'ffff, 0x88ff'ffff
	, 0x33ff'ffff, 0x2200'0000, 0x4400'0000, 0x6600'0000, 0x99ff'ffff
	, 0x22ff'ffff, 0x0000'0000, 0xffff'ffff, 0x8800'0000, 0xaaff'ffff
	, 0x11ff'ffff, 0xee00'0000, 0xcc00'0000, 0xaa00'0000, 0xbbff'ffff
	, 0x00ff'ffff, 0xffff'ffff, 0xeeff'ffff, 0xddff'ffff, 0xccff'ffff

	// 8

	, 0x33ff'ffff, 0x44ff'ffff, 0x55ff'ffff, 0x66ff'ffff, 0x77ff'ffff
	, 0x22ff'ffff, 0x3300'0000, 0x5500'0000, 0x7700'0000, 0x88ff'ffff
	, 0x11ff'ffff, 0x1100'0000, 0xffff'ffff, 0x9900'0000, 0x99ff'ffff
	, 0x00ff'ffff, 0xff00'0000, 0xdd00'0000, 0xbb00'0000, 0xaaff'ffff
	, 0xffff'ffff, 0xeeff'ffff, 0xddff'ffff, 0xccff'ffff, 0xbbff'ffff

	, 0x22ff'ffff, 0x33ff'ffff, 0x44ff'ffff, 0x55ff'ffff, 0x66ff'ffff
	, 0x11ff'ffff, 0x0000'0000, 0x2200'0000, 0x4400'0000, 0x77ff'ffff
	, 0x00ff'ffff, 0xee00'0000, 0xffff'ffff, 0x6600'0000, 0x88ff'ffff
	, 0xffff'ffff, 0xcc00'0000, 0xaa00'0000, 0x8800'0000, 0x99ff'ffff
	, 0xeeff'ffff, 0xddff'ffff, 0xccff'ffff, 0xbbff'ffff, 0xaaff'ffff

	, 0x11ff'ffff, 0x22ff'ffff, 0x33ff'ffff, 0x44ff'ffff, 0x55ff'ffff
	, 0x00ff'ffff, 0x1100'0000, 0x3300'0000, 0x5500'0000, 0x66ff'ffff
	, 0xffff'ffff, 0xff00'0000, 0xffff'ffff, 0x7700'0000, 0x77ff'ffff
	, 0xeeff'ffff, 0xdd00'0000, 0xbb00'0000, 0x9900'0000, 0x88ff'ffff
	, 0xddff'ffff, 0xccff'ffff, 0xbbff'ffff, 0xaaff'ffff, 0x99ff'ffff

	, 0x00ff'ffff, 0x11ff'ffff, 0x22ff'ffff, 0x33ff'ffff, 0x44ff'ffff
	, 0xffff'ffff, 0xee00'0000, 0x0000'0000, 0x2200'0000, 0x55ff'ffff
	, 0xeeff'ffff, 0xcc00'0000, 0xffff'ffff, 0x4400'0000, 0x66ff'ffff
	, 0xddff'ffff, 0xaa00'0000, 0x8800'0000, 0x6600'0000, 0x77ff'ffff
	, 0xccff'ffff, 0xbbff'ffff, 0xaaff'ffff, 0x99ff'ffff, 0x88ff'ffff

	// 12

	, 0xffff'ffff, 0x00ff'ffff, 0x11ff'ffff, 0x22ff'ffff, 0x33ff'ffff
	, 0xeeff'ffff, 0xff00'0000, 0x1100'0000, 0x3300'0000, 0x44ff'ffff
	, 0xddff'ffff, 0xdd00'0000, 0xffff'ffff, 0x5500'0000, 0x55ff'ffff
	, 0xccff'ffff, 0xbb00'0000, 0x9900'0000, 0x7700'0000, 0x66ff'ffff
	, 0xbbff'ffff, 0xaaff'ffff, 0x99ff'ffff, 0x88ff'ffff, 0x77ff'ffff

	, 0xeeff'ffff, 0xffff'ffff, 0x00ff'ffff, 0x11ff'ffff, 0x22ff'ffff
	, 0xddff'ffff, 0xcc00'0000, 0xee00'0000, 0x0000'0000, 0x33ff'ffff
	, 0xccff'ffff, 0xaa00'0000, 0xffff'ffff, 0x2200'0000, 0x44ff'ffff
	, 0xbbff'ffff, 0x8800'0000, 0x6600'0000, 0x4400'0000, 0x55ff'ffff
	, 0xaaff'ffff, 0x99ff'ffff, 0x88ff'ffff, 0x77ff'ffff, 0x66ff'ffff

	, 0xddff'ffff, 0xeeff'ffff, 0xffff'ffff, 0x00ff'ffff, 0x11ff'ffff
	, 0xccff'ffff, 0xdd00'0000, 0xff00'0000, 0x1100'0000, 0x22ff'ffff
	, 0xbbff'ffff, 0xbb00'0000, 0xffff'ffff, 0x3300'0000, 0x33ff'ffff
	, 0xaaff'ffff, 0x9900'0000, 0x7700'0000, 0x5500'0000, 0x44ff'ffff
	, 0x99ff'ffff, 0x88ff'ffff, 0x77ff'ffff, 0x66ff'ffff, 0x55ff'ffff

	, 0xccff'ffff, 0xddff'ffff, 0xeeff'ffff, 0xffff'ffff, 0x00ff'ffff
	, 0xbbff'ffff, 0xaa00'0000, 0xcc00'0000, 0xee00'0000, 0x11ff'ffff
	, 0xaaff'ffff, 0x8800'0000, 0xffff'ffff, 0x0000'0000, 0x22ff'ffff
	, 0x99ff'ffff, 0x6600'0000, 0x4400'0000, 0x2200'0000, 0x33ff'ffff
	, 0x88ff'ffff, 0x77ff'ffff, 0x66ff'ffff, 0x55ff'ffff, 0x44ff'ffff
	};

	std::vector<void*> cursor_2(16, nullptr);
	cursor_2[ 0] = &cursor_2_data[ 0][0];
	cursor_2[ 1] = &cursor_2_data[ 1][0];
	cursor_2[ 2] = &cursor_2_data[ 2][0];
	cursor_2[ 3] = &cursor_2_data[ 3][0];
	cursor_2[ 4] = &cursor_2_data[ 4][0];
	cursor_2[ 5] = &cursor_2_data[ 5][0];
	cursor_2[ 6] = &cursor_2_data[ 6][0];
	cursor_2[ 7] = &cursor_2_data[ 7][0];
	cursor_2[ 8] = &cursor_2_data[ 8][0];
	cursor_2[ 9] = &cursor_2_data[ 9][0];
	cursor_2[10] = &cursor_2_data[10][0];
	cursor_2[11] = &cursor_2_data[11][0];
	cursor_2[12] = &cursor_2_data[12][0];
	cursor_2[13] = &cursor_2_data[13][0];
	cursor_2[14] = &cursor_2_data[14][0];
	cursor_2[15] = &cursor_2_data[15][0];

	error = yetani->cursorCreate("cursor_2",
		{	.size           = { 5, 5 }
		,	.format         = WL_SHM_FORMAT_ARGB8888
		,	.hotspot_x      = 2
		,	.hotspot_y      = 2
		,	.time_per_frame = std::chrono::milliseconds(64)
		,	.image_data     = cursor_2
		});
	if(error)
	{
		printf("Failed to create cursor: %s\n", error.message().c_str());
	}

	zakero::Yetani::Window* blue = yetani->windowCreate(zakero::Yetani::SizeMm{ 160, 90 }
		, WL_SHM_FORMAT_XRGB8888
		, error
		);
	blue->cursorUse("cursor_2");
	blue->classSet("Yetani: Manual Window");
	blue->titleSet("Blue XOR Pattern");
	blue->onFocusChange([&](const bool has_focus)
	{
		printf("************************************************\n");
		printf("** Blue: %s Focus\n", (has_focus ? "Gained" : "Lost"));
		printf("************************************************\n");
	});
	blue->onCloseRequest([&]()
	{
		time_to_die = true;

		printf("************************************************\n");
		printf("** Blue: Close Request\n");
		printf("************************************************\n");

		return true;
	});

	blue->keyboardOnKey([&](const zakero::Yetani::Key& key, const zakero::Yetani::KeyModifier& modifier)
	{
		if(key.state == zakero::Yetani::KeyState::Released)
		{
			if(key.code == KEY_F1)
			{
				blue->cursorUse("cursor_1");
				return;
			}

			if(key.code == KEY_F2)
			{
				blue->cursorUse("cursor_2");
				return;
			}

			if(key.code == KEY_HOME)
			{
				blue->cursorShow();
				return;
			}

			if(key.code == KEY_END)
			{
				blue->cursorHide();
				return;
			}
		}
	});
#endif

	while(time_to_die == false)
	{
		//std::this_thread::yield();
		usleep(0);

		uint8_t* pixel;

		if(red != nullptr)
		{
#if 1
			red->imageNext(pixel, red_size);

			if(pixel != nullptr)
			{
				red_x_offset = (red_x_offset + 1) % red_size.width;

				for(int32_t y = 0; y < red_size.height; y++)
				{
					float yf = float(y) / red_size.height;
					uint8_t yp = yf * 255;

					for(int32_t x = 0; x < red_size.width; x++)
					{
						float xf = float((x + red_x_offset) % red_size.width) / red_size.width;
						uint8_t xp = xf * 255;

						uint8_t c = xp ^ yp;

						pixel[0] = 0;
						pixel[1] = 0;
						pixel[2] = c;
						pixel[3] = 0xff;

						pixel += 4;
					}
				}

				red->imagePresent();
			}
#else
red->imageNext(pixel, red_size);
red->imagePresent();
			(void)red_x_offset;
			(void)red_size;
#endif
		}

		// ------------------------------------------------------------ //

		if(green != nullptr)
		{
			green->imageNext(pixel, green_size);
			if(pixel != nullptr)
			{
				green_y_offset = (green_y_offset + 1) % green_size.height;

				for(int32_t y = 0; y < green_size.height; y++)
				{
					float yf = float((y + green_y_offset) % green_size.height) / green_size.height;
					uint8_t yp = yf * 255;

					for(int32_t x = 0; x < green_size.width; x++)
					{
						float xf = float(x) / green_size.width;
						uint8_t xp = xf * 255;

						uint8_t c = xp ^ yp;

						pixel[0] = 0;
						pixel[1] = c;
						pixel[2] = 0;
						pixel[3] = 0xff;

						pixel += 4;
					}
				}

				green->imagePresent();
			}
		}

		// ------------------------------------------------------------ //

		if(blue != nullptr)
		{
			blue->imageNext(pixel, blue_size);
			if(pixel != nullptr)
			{
				blue_x_offset = (blue_x_offset + 1) % blue_size.width;
				blue_y_offset = (blue_y_offset + 1) % blue_size.height;

				for(int32_t y = 0; y < blue_size.height; y++)
				{
					float yf = float((y + blue_y_offset) % blue_size.height) / blue_size.height;
					uint8_t yp = yf * 255;

					for(int32_t x = 0; x < blue_size.width; x++)
					{
						float xf = float((x + blue_x_offset) % blue_size.width) / blue_size.width;
						uint8_t xp = xf * 255;

						uint8_t c = xp ^ yp;

						pixel[0] = c;
						pixel[1] = 0;
						pixel[2] = 0;
						pixel[3] = 0xff;

						pixel += 4;
					}
				}

				blue->imagePresent();
			}
		}
	}

	delete red;
	//delete green;
	delete yetani;

	printf("Good Bye\n");

	return 0;
}

