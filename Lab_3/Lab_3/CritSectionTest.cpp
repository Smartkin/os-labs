#include "CritSectionTest.hpp"
#include <thread>

namespace LAB3
{
	CritSectionTest::CritSectionTest()
	{
		tasks_finished = false;
		finished_threads = 0;
		InitializeCriticalSection(&crit_sec);
	}

	CritSectionTest::~CritSectionTest()
	{
		DeleteCriticalSection(&crit_sec);
	}

	void CritSectionTest::launch()
	{
		std::thread thrd[10];
		for (auto i = 0; i < 10; ++i)
		{
			thrd[i] = std::thread(&CritSectionTest::task, this);
			thrd[i].detach(); //Let threads execute independently
		}
	}

	bool CritSectionTest::is_finished()
	{
		return tasks_finished;
	}

	void CritSectionTest::task()
	{
		EnterCriticalSection(&crit_sec);
		for (auto i = 0; i < 500; ++i)
		{
			//Sleep to intentionally slow down the working process
			Sleep(1);
			//Sequentially write to array
			test_arr[i] += i;
		}
		//Check if this is the last executing thread
		if (++finished_threads >= 10)
		{
			tasks_finished = true;
		}
		LeaveCriticalSection(&crit_sec);
	}

	int CritSectionTest::get_result()
	{
		//Return the last array element as a result
		return test_arr[499];
	}
}