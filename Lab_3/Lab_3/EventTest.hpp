#pragma once

#include <Windows.h>
#include <string>
#include <thread>
#include <map>

namespace LAB3
{
	class EventTest
	{
	public:
		explicit EventTest(std::string ev_name);
		~EventTest();

		HANDLE get_event();

		bool get_result();

	private:
		void task(int start_ind, int amt);

	private:
		HANDLE event;
		std::thread* launched_thread;
		std::string search_str;
		int thr_num;
		bool res_found;
		int thrs_finished;
		std::map<int, std::string> simple_table;
	};
}