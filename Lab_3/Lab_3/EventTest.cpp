#include "EventTest.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace LAB3
{
	EventTest::EventTest(std::string ev_name)
	{
		event = CreateEvent(NULL, FALSE, TRUE, ev_name.c_str());

		launched_thread = new std::thread(&EventTest::task, this);
	}

	EventTest::~EventTest()
	{
		CloseHandle(event);
	}

	std::thread* EventTest::get_thread()
	{
		return launched_thread;
	}

	void EventTest::task()
	{
		bool started_convo = false;
		while (true)
		{
			if (!started_convo)
			{
				auto res = WaitForSingleObject(event, 0);
				if (res == WAIT_TIMEOUT)
				{
					started_convo = true;
				}
				else if (res == WAIT_OBJECT_0)
				{
					std::string message;

					std::fstream file("..//..//event.txt", std::fstream::out);
					std::cout << "Initialized conversation...\n";

					std::cout << "Enter message to send over: ";
					std::cin.ignore();
					std::getline(std::cin, message);
					file << message;
					file.close();
					SetEvent(event);
					if (std::strcmp(message.c_str(), "/exit") == 0)
					{
						break;
					}

					started_convo = true;
					continue;
				}
			}
			else
			{
				std::string message;
				std::stringstream str;
				std::cout << "Waiting for reply...\n";

				WaitForSingleObject(event, INFINITE);

				std::fstream file("..//..//event.txt", std::fstream::in);
				str << file.rdbuf();
				std::cout << "Get a reply: " << str.str() << "\n";
				file.close();
				if (std::strcmp(str.str().c_str(), "/exit") == 0)
				{
					std::cout << "Received conversation end request...\n";
					SetEvent(event);
					break;
				}
				
				file.open("..//..//event.txt", std::fstream::out);

				std::cout << "Enter message to send over: ";
				std::cin.clear();
				std::getline(std::cin, message);
				file << message;
				file.close();
				SetEvent(event);
				if (std::strcmp(message.c_str(), "/exit") == 0)
				{
					break;
				}
			}
		}
		std::cout << "Converstaion ended.\n";
	}
}