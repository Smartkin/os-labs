#include "EventTest.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace LAB3
{
	EventTest::EventTest(std::string ev_name)
	{
		event = CreateEvent(NULL, TRUE, FALSE, ev_name.c_str());
		thr_num = 5;

		res_found = false;

		simple_table[0] = "age";
		simple_table[1] = "number";
		simple_table[2] = "house_number";
		simple_table[3] = "index";
		simple_table[4] = "budget";
		simple_table[5] = "age2";
		simple_table[6] = "number2";
		simple_table[7] = "house_number2";
		simple_table[8] = "index2";
		simple_table[9] = "budget2";

		std::ifstream f("..//..//event.txt", std::ifstream::in);
		std::stringstream str;
		str << f.rdbuf();
		search_str = str.str();
		f.close();

		launched_thread = new std::thread[thr_num];

		for (auto i = 0; i < thr_num; ++i)
		{
			launched_thread[i] = std::thread(&EventTest::task, this, simple_table.size() / thr_num * i, simple_table.size() / thr_num);
			launched_thread[i].detach();
		}
	}

	EventTest::~EventTest()
	{
		if (!res_found)
		{
			std::cout << "Not found asked string" << std::endl;
			std::ofstream task_f("..//..//event.txt", std::ofstream::out);
			task_f << "String that was searched for was not found!";
			task_f.close();
			SetEvent(event);
		}
		delete[] launched_thread;
		CloseHandle(event);
	}

	HANDLE EventTest::get_event()
	{
		return event;
	}

	bool EventTest::get_result()
	{
		return res_found || thrs_finished >= thr_num;
	}

	void EventTest::task(int start_ind, int amt)
	{
		for (auto i = start_ind; i < start_ind + amt; ++i)
		{
			if (simple_table[i] == search_str)
			{
				res_found = true;
				std::cout << "Found the asked string" << std::endl;
				std::ofstream task_f("..//..//event.txt", std::ofstream::out);
				task_f << "Index of the searched string is " << i;
				task_f.close();
				SetEvent(event);
			}
		}
		thrs_finished++;
	}
}