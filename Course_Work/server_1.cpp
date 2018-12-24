#include <iostream>
#include "sockets.hpp"



int main(int argc, char* argv[])
{
	const std::string data = " This is some data that you probably fetched.\
	Handle it with care and only transmit between\
	client and the server. Thank you.";
	
	unlink("/tmp/serv_sock_1.sock");
	
	SocketUse sock("/tmp/serv_sock_1.sock", 50000);
	auto data_sock = accept(sock.getSockfd(), NULL, NULL);
	if (data_sock == -1) { perror("[SERVER] Accept"); exit(EXIT_FAILURE); }
	size_t* req_size = (size_t*)SocketUse::read(data_sock, sizeof(size_t));
	char* buf = (char*)SocketUse::read(data_sock, *req_size);
	if (strcmp(buf, "FETCH DATA") == 0)
	{
		struct sockaddr_un addr;
		auto ans_sock = socket(AF_UNIX, SOCK_STREAM, 0);
		addr.sun_family = AF_UNIX;
		strncpy(addr.sun_path, "/tmp/client_sock_0.sock", sizeof(addr.sun_path) - 1);
		connect(ans_sock, (const struct sockaddr *) &addr, sizeof(struct sockaddr_un));
		size_t data_size = data.size();
		SocketUse::write(ans_sock, &data_size, sizeof(size_t));
		SocketUse::write(ans_sock, data.c_str(), data_size);
		close(ans_sock);
	}
	delete[] buf;
	delete req_size;
	close(data_sock);
	return 0;
}
