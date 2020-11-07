/*
g++ -std=c++20 -g -Wall -Werror -lpthread -lxcb -lxcb-randr -I../../include -o Manual_Window Manual_Window.cpp && ./Manual_Window
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

	printf("Hello\n");

	/*
	zakero::Xenium::SizeMm size{40, 40};
	zakero::Xenium::SizePercent size{0.5, 0.5};
	zakero::Xenium::SizePixel size{400, 400};
	*/
	zakero::Xenium::SizeMm size{16.0 * 15, 9.0 * 15};
	auto* window = xenium->windowCreate(size, error);
	if(error)
	{
		std::cout << error << '\n';
	}
	window->setClass("Xenium : Manual_Window");
	window->setTitle("Xenium");

	bool time_to_die = false;
	window->onCloseRequest([&]()
	{
		printf("--- Closing Time\n");
		time_to_die = true;
	});

	/*
	window->setDecorations(zakero::Xenium::WindowDecorations::Client_Side);
	window->setDecorations(zakero::Xenium::WindowDecorations::Server_Side);
	window->setSize(zakero::Xenium::SizePixel{500,300});
	*/

	while(!time_to_die)
	{
		usleep(10);
	}

	delete window;
	delete xenium;
	printf("Good Bye\n");

	return 0;
}

