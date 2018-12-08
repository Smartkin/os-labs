#include "pipe.hpp"
#include <bitset>
#include <iostream>

PipeUse::PipeUse()
{
	pipe(_pipefd);
}

PipeUse::~PipeUse()
{
	close(_pipefd[0]);
	close(_pipefd[1]);
}

void PipeUse::write(void* buf, size_t bytes)
{
	int* data = (int*)buf;
	while(*data)
	{
		std::cout << *(data++) << " ";
	}
	std::cout << std::endl;
	std::cout << "Writing to pipe..." << std::endl;
	::write(_pipefd[1], buf, bytes);
}

char* PipeUse::read(size_t bytes)
{
	std::cout << "Attempting to read..." << std::endl;
	char* buf = new char[bytes];
	::read(_pipefd[0], buf, bytes);
	std::cout << "Finished reading" << std::endl;
	return buf;
}
