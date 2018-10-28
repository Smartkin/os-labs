#pragma once

#include <Windows.h>
#include <string>

namespace LAB3
{
	class MutexTest
	{
	public:
		explicit MutexTest(std::string mutex_name = "", int file_read_count = 5);
		~MutexTest();

		bool is_finished();

	private:
		void task();

	private:
		HANDLE mutex;

		int file_read_times = 0;
		int read_count = 0;
	};
}