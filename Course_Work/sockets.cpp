#include "sockets.hpp"
#include <cstring>
#include <iostream>
using std::cout;
using std::endl;

SocketUse::SocketUse(const std::string name, int port)
{
	_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	_name.sun_family = AF_UNIX;
	_sock_name = name;
	std::strncpy(_name.sun_path, name.c_str(), sizeof(_name.sun_path) - 1);
	auto res = bind(_sockfd, (const struct sockaddr *) &_name, sizeof(struct sockaddr_un));
	if (res == -1) { perror("Bind: "); exit(EXIT_FAILURE); }
	res = listen(_sockfd, port);
	if (res == -1) { perror("Listen: "); exit(EXIT_FAILURE); }
}

SocketUse::~SocketUse()
{
	close(_sockfd);
	unlink(_sock_name.c_str());
}

void SocketUse::write(int data_sock, const void* data, size_t bytes)
{
	cout << "Wrote bytes: " << bytes << endl;
	::write(data_sock, data, bytes);
}

void* SocketUse::read(int data_sock, size_t bytes)
{
	cout << "Reading " << bytes << " bytes." << endl;
	void* buf = new char[bytes] { '\0' };
	::read(data_sock, buf, bytes);
	//cout << "Finished reading." << endl;
	return buf;
}

int& SocketUse::getSockfd() { return _sockfd; }
