#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

//Helper for Lab_3
//Emulates chat between 2 processes
int main(int argc, char** argv)
{
	HANDLE event = 0x0;

	std::string ev_name;
	std::cout << "Enter event name: ";
	std::cin >> ev_name;
	std::cin.ignore();

	while (event == 0x0)
	{
		event = OpenEvent(EVENT_ALL_ACCESS, TRUE, ev_name.c_str());
		std::cout << "Waiting for opened event...\n";
		Sleep(1000);
	}

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

	system("pause");
	return 0;
}