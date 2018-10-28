#pragma once

#include <Windows.h>
#include <string>
#include <thread>

namespace LAB3
{
	class EventTest
	{
	public:
		explicit EventTest(std::string ev_name);
		~EventTest();

		std::thread* get_thread();

	private:
		void task();

	private:
		HANDLE event;
		std::thread* launched_thread;
	};
}