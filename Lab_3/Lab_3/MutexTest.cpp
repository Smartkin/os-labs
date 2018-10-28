#include "MutextTest.hpp"
#include <TlHelp32.h>
#include <iostream>
#include <fstream>
#include <thread>

namespace LAB3
{
	MutexTest::MutexTest(std::string mutex_name, int file_read_count) : file_read_times(0), read_count(file_read_count)
	{
		mutex = CreateMutex(NULL, FALSE, mutex_name.c_str());

		std::thread file_checker(&MutexTest::task, this);
		file_checker.detach();
	}

	MutexTest::~MutexTest()
	{
		CloseHandle(mutex);
	}

	void MutexTest::task()
	{
		Sleep(100);
		while (true)
		{
			WaitForSingleObject(mutex, INFINITE);
			std::ifstream file("..\\..\\mutex.txt");
			if (file.is_open())
			{
				std::cout << "Current file contents:\n" << file.rdbuf();
				file.close();

				if (++file_read_times >= read_count)
				{
					ReleaseMutex(mutex);
					break;
				}
			}
			ReleaseMutex(mutex);
			//Reread the file after 1 second
			Sleep(1000);
		}
	}

	bool MutexTest::is_finished()
	{
		return file_read_times >= read_count;
	}
}