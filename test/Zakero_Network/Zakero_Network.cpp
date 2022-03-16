/*
g++ -std=c++20 -Wall -Werror -o Zakero_Network Zakero_Network.cpp && ./Zakero_Network
 */
#define DOCTEST_CONFIG_IMPLEMENT
#include "../doctest.h"

#ifdef ZAKERO_NETWORK_TEST_IP
const std::string Test_IP = ZAKERO_NETWORK_TEST_IP;
#else
const std::string Test_IP = "140.82.112.3"; // github.com
#endif

#define ZAKERO_NETWORK_IMPLEMENTATION
#define ZAKERO_NETWORK_IMPLEMENTATION_TEST
#include "../../include/Zakero_Network.h"

std::jthread create_tcpserver_thread(zakero::network::IP* ip
	, const uint16_t port
	, int32_t        timeout_ms
	) noexcept
{
	using namespace std::chrono_literals;

	bool ready = false;

	auto thread = std::jthread([=, &ready](std::stop_token token)
	{
		std::error_code error = {};
		zakero::network::TCPServer* server = zakero::network::TCPServer::create(ip, port, error);
		if(error)
		{
			fprintf(stderr, "zakero::network::TCPServer::create() Error: %s\n", error.message().c_str());
		}

		ready = true;

		while(token.stop_requested() == false)
		{
			zakero::network::TCP* client = server->acceptConnection(timeout_ms, error);
			if(error)
			{
				if(error == zakero::network::Error_Timeout)
				{
					continue;
				}

				fprintf(stderr, "zakero::network::TCPServer::acceptConnection() Error: %s\n", error.message().c_str());
			}

			delete client;
		}

		delete server;
	});

	while(ready == false)
	{
		std::this_thread::sleep_for(50ms);
	}

	std::this_thread::sleep_for(50ms);

	return thread;
}

/*
std::jthread server_connect_and_delete =
	create_tcpserver_thread(zakero::network::IPv4::create("127.0.0.1"), 65535, 100);
	*/


int main(int argc, char** argv)
{
	doctest::Context context;

	// --- Configuration: Defaults --- //
	context.setOption("order-by" , "name");
	context.setOption("rand-seed", "0"   );

	// --- Configuration: Parse Command-Line --- //
	context.applyCommandLine(argc, argv);

	// --- Configuration: Overrides --- //

	// --- Run Tests --- //
	int result = context.run();

	// --- Get Results --- ///
	if(context.shouldExit())
	{
		return result;
	}

	// --- Misc Stuff --- ///
	//server_connect_and_delete.request_stop();

	// --- Done --- ///
	return result;
}
