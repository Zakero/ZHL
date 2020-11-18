/*
g++ -std=gnu++20 -o Manual_-_Check_Output Manual_-_Check_Output.cpp
 */

#define ZAKERO_PROFILER_ENABLE
#define ZAKERO_PROFILER_IMPLEMENTATION
#include "../../include/Zakero_Profiler.h"

void func1()
{
	ZAKERO_PROFILER_COMPLETE("test", "empty")
}

void func2()
{
	ZAKERO_PROFILER_COMPLETE("test", "work")

	int total = 0;
	for(int i = 1; i < 10; i++)
	{
		if(i % 2)
		{
			total -= i;
		}
		else
		{
			total += i;
		}
	}
}

void func3()
{
	ZAKERO_PROFILER_COMPLETE("test", "layer");

	//ZAKERO_PROFILER_DEACTIVATE

	int total = 0;
	for(int i = 1; i < 10; i++)
	{
		ZAKERO_PROFILER_INSTANT("test", "for_loop")

		if(i % 2)
		{
			ZAKERO_PROFILER_COMPLETE("test", "sub",
				{ { "i", std::to_string(i) }
				})
			total -= i;
		}
		else
		{
			ZAKERO_PROFILER_COMPLETE("test", "add",
				{ { "i", std::to_string(i) }
				})
			total += i;
		}
	}

	//ZAKERO_PROFILER_ACTIVATE
}

int main()
{
	const std::string TestName("Manual_-_Check_Output");
	const std::string TestFile = TestName + ".json";

	ZAKERO_PROFILER_INIT(TestFile
	,	{	{ "application" , TestName }
		,	{ "version"     , "1.0.0" }
		}
	);

	func1();
	func2();
	func3();

	std::cout << "1. Verify that \"" << TestFile << "\" contains data.\n";
	std::cout << "2. Verify \"" << TestFile << "\" is viewable in a browser.\n";
	std::cout << "   - As documented, Profile Timeline should have no errors.\n";

	return 0;
}
