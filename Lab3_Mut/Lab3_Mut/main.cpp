#include <Windows.h>
#include <iostream>
#include <thread>
#include <random>
#include <fstream>
#include <string>

//This is a helper project for Lab_3
//The app emulates random access to a file and changes its content
int main(int argc, char** argv)
{
	HANDLE mutex = 0x0;
	std::random_device device("Device");
	std::string mut_name = "";

	std::cout << "Enter mutex name to use: ";
	std::cin >> mut_name;
	//Check for opened mutex
	while (mutex == 0x0)
	{
		mutex = OpenMutex(MUTEX_ALL_ACCESS, TRUE, mut_name.c_str());
		std::cout << "Waiting for mutex...\n";
		Sleep(1000);
	}
	std::cout << "Mutex found!\n";
	int num = 0;
	while (true)
	{
		Sleep(device() % 100 + 10);
		DWORD res = WaitForSingleObject(mutex, INFINITE);
		if (res == WAIT_FAILED || res == WAIT_ABANDONED || res == WAIT_OBJECT_0) break;
		std::ofstream file("..\\..\\mutex.txt",std::ofstream::app);
		file << "We keep on changing the content by adding rising number here on the end of each line " << num++ << "\n";
		file.close();
		ReleaseMutex(mutex);
	}
	std::cout << "Finished working!" << std::endl;
	system("pause");
	return 0;
}