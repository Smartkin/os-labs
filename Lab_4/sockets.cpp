#include "sockets.hpp"
#include <cstring>
#include <iostream>
using std::cout;
using std::endl;

SocketUse::SocketUse(const std::string name)
{
	_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	_name.sun_family = AF_UNIX;
	_sock_name = name;
	std::strncpy(_name.sun_path, name.c_str(), sizeof(_name.sun_path) - 1);
	bind(_sockfd, (const struct sockaddr *) &_name, sizeof(struct sockaddr_un));
	listen(_sockfd, 20);
}

SocketUse::~SocketUse()
{
	close(_sockfd);
	unlink(_sock_name.c_str());
}

void SocketUse::write(int data_sock, void* data, size_t bytes)
{
	int* buf = (int*)data;
	cout << "Writing data: ";
	while(*buf)
	{
		cout << *(buf++) << " ";
	}
	cout << endl;
	cout << "Wrote bytes: " << bytes << endl;
	::write(data_sock, data, bytes);
}

char* SocketUse::read(int data_sock, size_t bytes)
{
	char* buf = new char[bytes];
	::read(data_sock, buf, bytes);
	return buf;
}

int SocketUse::getSockfd() const { return _sockfd; }
