#include <iostream>
#include "CritSectionTest.hpp"
#include "MutextTest.hpp"
#include "EventTest.hpp"

enum SYNC_METHODS
{
	CRIT_SEC = 1,
	MUTEX,
	EVENT
};

int get_choice();
void launch_task(SYNC_METHODS method);

int main(int argc, char** argv)
{
	int sync_method = 0;

	do
	{
		sync_method = get_choice();
		launch_task((SYNC_METHODS)sync_method);
	} while (sync_method != 0);
	return 0;
}

int get_choice()
{
	int choice = 0;
	do
	{
		std::cout << "What thread syncronization method would you like to use?\n";
		std::cout << "1. Critical section\n";
		std::cout << "2. Mutex\n";
		std::cout << "3. Event\n";
		std::cout << "Enter choice(0 for exit): ";
		std::cin >> choice;
		if (choice < 0 || choice > 3)
		{
			std::cerr << "Choice is out of range! Retry\n";
		}
	} while (choice < 0 || choice > 3);
	return choice;
}

void launch_task(SYNC_METHODS method)
{
	switch (method)
	{
	case CRIT_SEC:
	{
		LAB3::CritSectionTest crit_test;
		crit_test.launch();
		std::cout << "Waiting for threads to finish...";
		while (!crit_test.is_finished())
		{
			Sleep(500);
			std::cout << ".";
		}
		std::cout << "\nFinished! With result " << crit_test.get_result() << std::endl;
	}
		break;
	case MUTEX:
	{
		std::string mut_name;
		std::cout << "Enter mutex name to work with: ";
		std::cin >> mut_name;
		
		LAB3::MutexTest mut_test(mut_name);

		while (!mut_test.is_finished()) {}
		std::cout << "Finished reading the file...\n";

	}
		break;
	case EVENT:
	{
		std::string ev_name;
		std::cout << "Enter event name to work with: ";
		std::cin >> ev_name;

		LAB3::EventTest ev_test(ev_name);

		ev_test.get_thread()->join();
		
	}
		break;
	}

	system("pause");
	system("cls");
}