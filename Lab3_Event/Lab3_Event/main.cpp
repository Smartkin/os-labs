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
	std::string search_str;
	std::cout << "Enter event name: ";
	std::cin >> ev_name; std::cin.ignore();
	std::cout << "Enter string to find: ";
	std::cin >> search_str; std::cin.ignore();

	std::ofstream task_f("..//..//event.txt", std::ofstream::out);
	task_f << search_str;
	task_f.close();

	std::cout << "Waiting for opened event...\n";
	while (event == 0x0)
	{
		event = OpenEvent(SYNCHRONIZE, TRUE, ev_name.c_str());
	}

	WaitForSingleObject(event, INFINITE);
	std::ifstream file("..//..//event.txt", std::ifstream::in);
	std::stringstream res;
	res << file.rdbuf();
	std::cout << res.str() << std::endl;
	file.close();

	system("pause");
	return 0;
}