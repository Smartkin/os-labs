#include "pipe.hpp"
#include "sockets.hpp"
#include <exception>
#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

#define PROT_ALL PROT_EXEC | PROT_READ | PROT_WRITE

using std::cout;
using std::cin;
using std::endl;

enum IPC_TYPE
{
	SOCKETS = 1,
	PIPE,
	MMAP
};

void processChoice(IPC_TYPE ch, std::vector<int> to_sort);
std::vector<int> gen_arr(int size = 100);

int main(int argc, char** argv)
{
	int choice = -1;
	for(;choice != 0;)
	{
		int arr_size = 100;
		cout << "Enter array size to generate: ";
		cin >> arr_size;
		if (arr_size <= 0) arr_size = 100;
		auto to_sort = gen_arr(arr_size);
		for(auto i = 0; i < arr_size; ++i)
		{
			cout << i+1 << ": " << to_sort[i] << endl;
		}
		cout << "Enter interprocess communication method (0 to exit): " << endl;
		cout << "1. Sockets" << endl;
		cout << "2. Pipe" << endl;
		cout << "3. Memory Map" << endl;
		cout << "Choice: ";
		cin >> choice; cin.ignore();
		if (choice < 0 || choice > 3) { choice = -1; continue; }
		processChoice((IPC_TYPE)choice, to_sort);
	}

	return 0;
}


std::vector<int> do_sort(PipeUse& p, std::vector<int> to_sort, bool main_proc)
{
	if (to_sort.size() >= 2)
	{
		auto cpid = fork();
		if (cpid == 0)
		{
			std::vector<int> new_sort(to_sort.begin() + to_sort.size()/2, to_sort.end());
			do_sort(p, new_sort, false);
			_exit(EXIT_SUCCESS);
		}
		else
		{
			cout << "Waiting on child!" << endl;
			wait(NULL);
			cout << "Child finished!" << endl;
			std::vector<int> one_sort;
			int int_size = sizeof(int);
			char* buf = p.read(to_sort.size()/2 * int_size);
			cout << "Read amount of bytes: " << to_sort.size()/2 * int_size << endl;
			for(int i = 0; i < to_sort.size()/2 * int_size; i += int_size)
			{
				try
				{
					one_sort.push_back(((buf[i+int_size-1] << 24) | (buf[i+int_size-2] << 16) | (buf[i+int_size-3] << 8) | (buf[i])));
				}
				catch(std::exception e)
				{
					std::cerr << "Got exception: " << e.what() << endl;
				}
			}
			cout << "Got new array: ";
			for(int i = 0; i < one_sort.size(); ++i)
			{
				cout << one_sort[i] << " ";
			}
			cout << endl;
			delete[] buf;
			std::vector<int> another_sort(to_sort.begin(), to_sort.begin() + to_sort.size()/2);
			std::sort(another_sort.begin(), another_sort.end(), [](int a, int b) { return a < b; });
			std::vector<int> res_sort;
			for(int i = 0; i < one_sort.size(); ++i)
			{
				res_sort.push_back(one_sort[i]);
			}
			for(int i = 0; i < another_sort.size(); ++i)
			{
				res_sort.push_back(another_sort[i]);
			}
			std::sort(res_sort.begin(), res_sort.end(), [](int a,int b) { return a < b; });
			if (!main_proc)
			{
				p.write(res_sort.data(), res_sort.size() * sizeof(int));
				_exit(EXIT_SUCCESS);
			}
			return res_sort;
		}
	}
	else
	{
		cout << "Came to last stuff" << endl;
		std::sort(to_sort.begin(),to_sort.end(), [](int a,int b) { return a < b; });
		p.write(to_sort.data(), to_sort.size() * sizeof(int));
		_exit(EXIT_SUCCESS);
	}
	return std::vector<int>();
}


int sock_name_id = 1;
std::vector<int> do_sort(std::vector<int> to_sort, bool main_proc)
{
	if (to_sort.size() >= 2)
	{
		const std::string SOCK_NAME = std::string("/tmp/383nfa92jf")+std::to_string(sock_name_id++)+std::string(".socket");
		auto cpid = fork();
		if (cpid == 0)
		{
			std::vector<int> new_sort(to_sort.begin() + to_sort.size()/2, to_sort.end());
			do_sort(new_sort, false);
			_exit(EXIT_SUCCESS);		
		}
		else
		{
			const std::string SOCK_PREV_NAME = std::string("/tmp/383nfa92jf")+std::to_string(sock_name_id-2)+std::string(".socket");
			SocketUse sock(SOCK_NAME);
			cout << "Litening on socket name: " << SOCK_NAME << endl;
			listen(sock.getSockfd(), 1000);
			auto data_sock = accept(sock.getSockfd(), NULL, NULL);
			std::vector<int> one_sort;
			int int_size = sizeof(int);
			char* btr = SocketUse::read(data_sock, sizeof(int));
			cout << "Read amount to read" << endl;
			int read_amt = ((btr[int_size-1] << 24) | (btr[int_size-2] << 16) | (btr[int_size-3] << 8) | (btr[0]));
			char* buf = SocketUse::read(data_sock, read_amt);
			cout << "Read amount of bytes: " << read_amt << endl;
			for(int i = 0; i < read_amt; i += int_size)
			{
				try
				{
					one_sort.push_back(((buf[i+int_size-1] << 24) | (buf[i+int_size-2] << 16) | (buf[i+int_size-3] << 8) | (buf[i])));
				}
				catch(std::exception e)
				{
					std::cerr << "Got exception: " << e.what() << endl;
				}
			}
			cout << "Got new array: ";
			for(int i = 0; i < one_sort.size(); ++i)
			{
				cout << one_sort[i] << " ";
			}
			cout << endl;
			delete[] buf;
			delete btr;
			std::vector<int> another_sort(to_sort.begin(), to_sort.begin() + to_sort.size()/2);
			std::sort(another_sort.begin(), another_sort.end(), [](int a, int b) { return a < b; });
			std::vector<int> res_sort;

			cout << "Concating array: ";
			for(int i = 0; i < another_sort.size(); ++i)
			{
				cout << another_sort[i] << " ";
			}
			cout << endl;
			for(int i = 0; i < one_sort.size(); ++i)
			{
				res_sort.push_back(one_sort[i]);
			}
			for(int i=0;i<another_sort.size(); ++i)
			{
				res_sort.push_back(another_sort[i]);
			}
			std::sort(res_sort.begin(), res_sort.end(), [](int a,int b) { return a < b; });
			if (!main_proc)
			{
				cout << "Connecting to socket: " << SOCK_PREV_NAME << endl;
				// Connect to socket
				struct sockaddr_un addr;
				auto data_sock = socket(AF_UNIX, SOCK_STREAM, 0);
				addr.sun_family = AF_UNIX;
				strncpy(addr.sun_path, SOCK_PREV_NAME.c_str(), sizeof(addr.sun_path) - 1);
				connect(data_sock, (const struct sockaddr *) &addr, sizeof(struct sockaddr_un));
				int bytes_to_read = res_sort.size() * sizeof(int);
				SocketUse::write(data_sock, &bytes_to_read, sizeof(int));
				SocketUse::write(data_sock, res_sort.data(), res_sort.size() * sizeof(int));
				close(data_sock);
				sock.~SocketUse();
				_exit(EXIT_SUCCESS);
			}
			wait(NULL);
			return res_sort;
		}
	}
	else
	{
		cout << "Came to last stuff!" << endl;
		const std::string SOCK_PREV_NAME = std::string("/tmp/383nfa92jf")+std::to_string(sock_name_id-1)+std::string(".socket");
		sleep(1);
		cout << "Telling socket: " << SOCK_PREV_NAME << endl;
		// Connect to socket
		struct sockaddr_un addr;
		auto data_sock = socket(AF_UNIX, SOCK_STREAM, 0);
		auto bytes_to_read = sizeof(int);
		addr.sun_family = AF_UNIX;
		strncpy(addr.sun_path, SOCK_PREV_NAME.c_str(), sizeof(addr.sun_path) - 1);
		connect(data_sock, (const struct sockaddr *) &addr, sizeof(struct sockaddr_un));
		std::sort(to_sort.begin(), to_sort.end(), [](int a,int b) { return a < b; });
		SocketUse::write(data_sock, &bytes_to_read, sizeof(int));
		SocketUse::write(data_sock, to_sort.data(), to_sort.size() * sizeof(int));
		close(data_sock);
		_exit(EXIT_SUCCESS);
	}
	return std::vector<int>();
}

std::vector<int> do_sort(std::vector<int> to_sort, int* memory, unsigned int write_off, bool main_proc)
{
	if (to_sort.size() >= 2)
	{
		auto p_id = fork();
		if (p_id == 0)
		{
			std::vector<int> new_sort(to_sort.begin() + to_sort.size()/2, to_sort.end());
			cout << "Offset writing set to: " << (write_off + new_sort.size()) << endl;
			do_sort(new_sort, memory, write_off + new_sort.size(), false);
			_exit(EXIT_SUCCESS);
		}
		else
		{
			cout << "Waiting on child..." << endl;
			wait(NULL);
			cout << "Child finished" << endl;
			std::vector<int> one_sort;
			auto int_size = sizeof(int);
			cout << "Data in buffer: " << endl;
			int* buf = memory + write_off + to_sort.size()/2;
			//while(buf)
			//{
			//	cout << buf++ << endl;
			//}
			//buf = buf2;
			for(int i = 0; i < to_sort.size()/2; ++i)
			{
				try
				{
					one_sort.push_back(buf[i]);
				}
				catch(std::exception e)
				{
					std::cerr << "Got exception: " << e.what() << endl;
				}
			}
			cout << "Got new array: ";
			for(int i = 0; i < one_sort.size(); ++i)
			{
				cout << one_sort[i] << " ";
			}
			cout << endl;
			std::vector<int> another_sort(to_sort.begin(), to_sort.begin() + to_sort.size()/2);
			std::sort(another_sort.begin(), another_sort.end(), [](int a, int b) { return a < b; });
			std::vector<int> res_sort;
			for(int i = 0; i < one_sort.size(); ++i)
			{
				res_sort.push_back(one_sort[i]);
			}
			for(int i = 0; i < another_sort.size(); ++i)
			{
				res_sort.push_back(another_sort[i]);
			}
			cout << "Res array: ";
			for(int i = 0; i < res_sort.size(); ++i)
			{
				cout << res_sort[i] << " ";
			}
			cout << endl;
			std::sort(res_sort.begin(), res_sort.end(), [](int a,int b) { return a < b; });
			cout << "Sorted res array: ";
			for(int i = 0; i < res_sort.size(); ++i)
			{
				cout << res_sort[i] << " ";
			}
			cout << endl;
			if (!main_proc)
			{
				int* data = memory + write_off;
				for(int val : res_sort)
				{
					*data = val;
					data++;
				}
				_exit(EXIT_SUCCESS);
			}
			return res_sort;
		}
	}
	else
	{
		cout << "Came to last stuff" << endl;
		std::sort(to_sort.begin(),to_sort.end(), [](int a,int b) { return a < b; });
		int* data = memory + write_off;
		int* buf2 = data;
		cout << "Data in buffer: " << endl;
		cout << "Array size sort: " << to_sort.size() << endl;
		//int c = 0;
		//cout << "Is everything ok? " << (c < to_sort.size()) << endl;
		//while(c < to_sort.size())
		//cout << c << ": ";
		cout << (*buf2) << endl;
		//buf2++;
		//c++;
		for(int val : to_sort)
		{
			*data = val;
			data++;
		}
		_exit(EXIT_SUCCESS);
	}
	return std::vector<int>();
}

void processChoice(IPC_TYPE ch, std::vector<int> to_sort)
{
	switch(ch)
	{
		case SOCKETS:
			{
				to_sort = do_sort(to_sort, true);
			}
			break;
		case PIPE:
			{
				PipeUse p;
				to_sort = do_sort(p, to_sort, true);
			}
			break;
		case MMAP:
			{
				//Create temp file and map it onto the memory
				auto fd = open("memory_map.txt", O_CREAT|O_RDWR, S_IRWXU);
				cout << "File descriptor: " << fd << endl;
				auto w_op = write(fd, to_sort.data(), to_sort.size() * sizeof(int));
				if (w_op == -1) { perror("write"); exit(EXIT_FAILURE); }
				auto m = mmap(NULL, sizeof(int) * to_sort.size(), PROT_ALL, MAP_SHARED, fd, 0);
				int* m2 = (int*)m;
				cout << "Data: " << endl;
				for(auto i = 0; i < to_sort.size(); ++i)
				{
					cout << i << ": ";
					cout << *m2 << endl;
					m2++;
				}
				//Sort
				to_sort = do_sort(to_sort, (int*)m, 0, true);
				// Unmap memory as well as close temporary file descriptor
				munmap(m, sizeof(int) * to_sort.size());
				close(fd);
				system("rm memory_map.txt");
			}
			break;
		default:
			return;
			break;
	}
	for (int i = 0; i < to_sort.size(); ++i)
	{
		cout << i+1 << ": " << to_sort[i] << endl;
	}

}



std::vector<int> gen_arr(int size)
{
	std::vector<int> arr;
	std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> distribution(-5*size,5*size);
	for (auto i = 0; i < size; ++i)
	{
		arr.push_back(distribution(gen));
	}
	return arr;
}



