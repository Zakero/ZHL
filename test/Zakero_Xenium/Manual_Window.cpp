/*
g++ -std=c++20 -g -Wall -Werror -lpthread -lxcb -lxcb-xkb -lxcb-randr -I../../include -o Manual_Window Manual_Window.cpp && ./Manual_Window
 */

/*
#define ZAKERO_PROFILER_IMPLEMENTATION
#include "Zakero_Profiler.h"
*/

#define ZAKERO_MEMORYPOOL_IMPLEMENTATION
#include "Zakero_MemoryPool.h"

#define ZAKERO_XENIUM_IMPLEMENTATION
#define ZAKERO_XENIUM_ENABLE_DEBUG
#define ZAKERO_XENIUM_ENABLE_SAFE_MODE

#include "Zakero_Xenium.h"

namespace
{
}


int main()
{
	std::error_code error;

	zakero::Xenium* xenium = zakero::Xenium::connect(error);
	if(error)
	{
		std::cout << "Error: " << error << '\n';
		return 1;
	}
	zakero::Xenium::OutputId output_id = xenium->outputVector().front();
	zakero::Xenium::Output output = xenium->output(output_id);
	printf("%s\n", zakero::to_string(output).c_str());

	xenium->outputOnAdd([&](const zakero::Xenium::OutputId id)
	{
		printf("--- Output Added: %u\n", id);
		zakero::Xenium::Output output = xenium->output(id);
		std::cout << zakero::to_string(output) << '\n';
	});
	xenium->outputOnChange([&](const zakero::Xenium::OutputId id)
	{
		printf("--- Output Changed: %u\n", id);
		zakero::Xenium::Output output = xenium->output(id);
		std::cout << zakero::to_string(output) << '\n';
	});
	xenium->outputOnRemove([&](const zakero::Xenium::OutputId id)
	{
		printf("--- Output Removed: %u\n", id);
	});

	/*
	zakero::Xenium::SizeMm size{40, 40};
	zakero::Xenium::SizePercent size{0.5, 0.5};
	zakero::Xenium::SizeMm size{16.0 * 15, 9.0 * 15};
	zakero::Xenium::SizePixel size{450, 450};
	*/
	zakero::Xenium::SizeMm size{80, 80};
	auto* window = xenium->windowCreate(size, error);
	if(error)
	{
		std::cout << error << '\n';
	}
	window->classSet("Xenium : Manual_Window");
	window->titleSet("Xenium Test");

	//window->sizeSet(zakero::Xenium::SizeMm{80, 40});
	//window->sizeSet(zakero::Xenium::SizePixel{100, 100});
	/*
	window->sizeSetMinMax(
		zakero::Xenium::SizePixel{400, 400}, zakero::Xenium::SizePixel{500, 500}
		zakero::Xenium::SizeMm{30, 30}, zakero::Xenium::SizeMm{50, 50}
		);
	*/

	bool time_to_die = false;
	window->onCloseRequest([&]()
	{
		printf("--- Closing Time\n");
		time_to_die = true;
	});

	/*
	window->decorationsSet(zakero::Xenium::WindowDecorations::Client_Side);
	window->decorationsSet(zakero::Xenium::WindowDecorations::Server_Side);
	window->setSize(zakero::Xenium::SizePixel{500,300});
	*/

//	window->sizeOnChange([](const zakero::Xenium::SizeMm& size)
//	{
//		printf("--- SizeMm: %fx%f\n", size.width, size.height);
//	});

//	window->sizeOnChange([](const zakero::Xenium::SizePercent& size)
//	{
//		printf("--- SizePercent: %fx%f\n", size.width, size.height);
//	});

//	window->sizeOnChange([](const zakero::Xenium::SizePixel& size)
//	{
//		printf("--- SizePixel: %dx%d\n", size.width, size.height);
//	});

//	window->windowModeOnChange([](const zakero::Xenium::WindowMode& window_mode)
//	{
//		printf(">>> WindowMode: %s\n", zakero::to_string(window_mode).c_str());
//	});

/*
	sleep(3);
	printf("--- Full Screen\n");
	window->windowModeSet(zakero::Xenium::WindowMode::Fullscreen);
	sleep(3);
	printf("--- Normal\n");
	window->windowModeSet(zakero::Xenium::WindowMode::Normal);
	sleep(3);
	printf("--- Maximized\n");
	window->windowModeSet(zakero::Xenium::WindowMode::Maximized);
	sleep(3);
	printf("--- Normal\n");
	window->windowModeSet(zakero::Xenium::WindowMode::Normal);
*/

//	window->decorationsOnChange([](const zakero::Xenium::WindowDecorations deco)
//	{
//		printf(">>> WindowDecorations: %s\n", zakero::to_string(deco).c_str());
//	});

//	window->onFocusChange([](const bool has_focus)
//	{
//		printf(">>> Focus: %s\n", has_focus ? "true" : "false");
//	});

//	window->pointerOnEnter([](const zakero::Xenium::PointMm& point, const zakero::Xenium::KeyModifier& mod)
//	{
//		printf(">>> Enter (mm): %s %s\n"
//			, zakero::to_string(point).c_str()
//			, zakero::to_string(mod).c_str()
//			);
//	});

//	window->pointerOnEnter([](const zakero::Xenium::PointPercent& point, const zakero::Xenium::KeyModifier& mod)
//	{
//		printf(">>> Enter (Percent): %s %s\n"
//			, zakero::to_string(point).c_str()
//			, zakero::to_string(mod).c_str()
//			);
//	});

//	window->pointerOnEnter([](const zakero::Xenium::PointPixel& point, const zakero::Xenium::KeyModifier& mod)
//	{
//		printf(">>> Enter (Pixel): %s %s\n"
//			, zakero::to_string(point).c_str()
//			, zakero::to_string(mod).c_str()
//			);
//	});

//	window->pointerOnLeave([]()
//	{
//		printf(">>> Leave\n");
//	});

//	window->pointerOnMotion([](const zakero::Xenium::PointMm& point, const zakero::Xenium::KeyModifier& mod)
//	{
//		printf(">>> Motion (mm): %s %s\n"
//			, zakero::to_string(point).c_str()
//			, zakero::to_string(mod).c_str()
//			);
//	});

//	window->pointerOnMotion([](const zakero::Xenium::PointPercent& point, const zakero::Xenium::KeyModifier& mod)
//	{
//		printf(">>> Motion (percent): %s %s\n"
//			, zakero::to_string(point).c_str()
//			, zakero::to_string(mod).c_str()
//			);
//	});

//	window->pointerOnMotion([](const zakero::Xenium::PointPixel& point, const zakero::Xenium::KeyModifier& mod)
//	{
//		printf(">>> Motion (pixel): %s %s\n"
//			, zakero::to_string(point).c_str()
//			, zakero::to_string(mod).c_str()
//			);
//	});

//	window->pointerOnButton([](const zakero::Xenium::PointerButton& button, const zakero::Xenium::PointMm& point, const zakero::Xenium::KeyModifier& mod)
//	{
//		printf(">>> Button (mm): %s %s %s\n"
//			, zakero::to_string(button).c_str()
//			, zakero::to_string(point).c_str()
//			, zakero::to_string(mod).c_str()
//			);
//	});

//	window->pointerOnButton([](const zakero::Xenium::PointerButton& button, const zakero::Xenium::PointPercent& point, const zakero::Xenium::KeyModifier& mod)
//	{
//		printf(">>> Button (percent): %s %s %s\n"
//			, zakero::to_string(button).c_str()
//			, zakero::to_string(point).c_str()
//			, zakero::to_string(mod).c_str()
//			);
//	});

//	window->pointerOnButton([](const zakero::Xenium::PointerButton& button
//		, const zakero::Xenium::PointPixel&                     point
//		, const zakero::Xenium::KeyModifier&                    mod
//		)
//	{
//		printf(">> Button (pixel): %s %s %s\n"
//			, zakero::to_string(button).c_str()
//			, zakero::to_string(point).c_str()
//			, zakero::to_string(mod).c_str()
//			);
//	});

//	window->pointerOnAxis([](const zakero::Xenium::PointerAxis& axis
//		, const zakero::Xenium::KeyModifier&                mod
//		)
//	{
//		printf(">> Pointer Axis: %s, Mod: %s\n"
//			, zakero::to_string(axis).c_str()
//			, zakero::to_string(mod).c_str()
//			);
//	});

//	window->keyboardOnEnter([&]()
//	{
//		printf(">> Keyboard Enter\n");
//	});

//	window->keyboardOnLeave([&]()
//	{
//		printf(">> Keyboard Leave\n");
//	});

//	window->keyboardOnKey([](const zakero::Xenium::Key& key
//		, const zakero::Xenium::KeyModifier& mod
//		)
//	{
//		printf(">> Key: %s, Mod: %s\n"
//			, zakero::to_string(key).c_str()
//			, zakero::to_string(mod).c_str()
//			);
//	});

	printf("--- Looping\n");

	uint32_t x_offset = 0;
	zakero::Xenium::SizePixel window_size = { 1, 1 };

	while(!time_to_die)
	{
		usleep(1);

		uint8_t* pixel;

		window->imageNext(pixel, window_size);

		if(pixel != nullptr)
		{
			x_offset = (x_offset + 1) % window_size.width;

			for(int32_t y = 0; y < window_size.height; y++)
			{
				float yf = float(y) / window_size.height;
				uint8_t yp = yf * 255;

				for(int32_t x = 0; x < window_size.width; x++)
				{
					float xf = float((x + x_offset) % window_size.width) / window_size.width;
					uint8_t xp = xf * 255;

					uint8_t c = xp ^ yp;

					pixel[0] = 0;
					pixel[1] = 0;
					pixel[2] = c;
					pixel[3] = 0xff;

					pixel += 4;
				}
			}

			window->imagePresent();
		}
	}

	delete window;
	delete xenium;
	printf("Good Bye\n");

	return 0;
}

