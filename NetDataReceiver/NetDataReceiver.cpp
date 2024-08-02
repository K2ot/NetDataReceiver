#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "Response.h"

//TODO: określić nagłówki ramki dla zdjęcia i wiadomości



std::mutex mutex;
std::condition_variable conditionVariable;
bool stopFlag = false; // Flaga używana do zatrzymania pętli


int main(int argc, char* argv[])
{
	std::locale::global(std::locale("Polish_Poland.1250"));

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
	std::cout << R"(W celu zakończenia napisz : "stop", "koniec" lub "end")" << std::endl;
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

	std::cout << std::endl;
	std::string myAddressIP = "127.0.0.1";
	std::string myPortTCP = "24";

	if (argc > 1)
	{ // Sprawdzamy, czy podano przynajmniej jeden argument oprócz nazwy programu
		myAddressIP = argv[1]; // Przypisujemy pierwszy argument do stringa
		std::cout << "Podany adres IP to: " << myAddressIP << std::endl;
	}

	try
	{
		DataManager::GetInstance().InitializeHandlers();
	}
	catch (const std::exception& e)
	{
		std::cerr << __FUNCTION__ << e.what() << std::endl;
	}

	std::unique_ptr<TCPServer> server;
	std::unique_ptr<Response> response;

	try
	{
		server = std::make_unique<TCPServer>(myAddressIP, myPortTCP);
		response = std::make_unique<Response>(*server);
		server->Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << __FUNCTION__ << e.what() << std::endl;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	std::thread inputThread([&]()
		{
			while (!stopFlag)
			{
				std::string stopSTR{ "" };
				std::getline(std::cin, stopSTR);
				if (stopSTR == "stop" || stopSTR == "koniec" || stopSTR == "end")
				{
					std::lock_guard<std::mutex> lock(mutex);
					stopFlag = true;
					conditionVariable.notify_one(); // Powiadomienie, że warunek został spełniony
				}
				stopSTR.clear();
			}
		});

	std::unique_lock<std::mutex> lock(mutex);
	conditionVariable.wait(lock, [] { return stopFlag; });

	if (inputThread.joinable())
	{
		inputThread.join();
	}
	return 0;
}
