#pragma once
#include <Windows.h>

namespace LAB3
{
	class CritSectionTest
	{
	public:
		CritSectionTest();
		~CritSectionTest();

		bool is_finished();

		void launch();

		int get_result();

	private:
		void task();

	private:
		CRITICAL_SECTION crit_sec;
		int finished_threads;
		bool tasks_finished;

		int test_arr[500] = { 0 };
	};
}