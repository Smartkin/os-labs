#include <iostream>
#include "sockets.hpp"
#include <unistd.h>


int main(int argv, char* argc[])
{
	unlink("/tmp/client_sock_0.sock");
	SocketUse sock("/tmp/client_sock_0.sock", 49000);
	auto serv_1 = socket(AF_UNIX, SOCK_STREAM, 0);
	if (serv_1 == -1) { perror("Socket: "); exit(EXIT_FAILURE); }
	std::string req("FETCH DATA");
	const char* request = req.c_str();
	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, "/tmp/serv_sock_1.sock", sizeof(addr.sun_path) - 1);
	auto ret = connect(serv_1, (const struct sockaddr *) &addr, sizeof(struct sockaddr_un));
	if (ret == -1) { perror("Server is down"); exit(EXIT_FAILURE); }
	size_t str_len = req.size();
	SocketUse::write(serv_1, &str_len, sizeof(size_t));
	SocketUse::write(serv_1, request, req.size());
	close(serv_1);
	std::cout << "Finished sending request to server 1!" << std::endl;
	auto data_sock = accept(sock.getSockfd(), NULL, NULL);
	size_t* answer_size = (size_t*)SocketUse::read(data_sock, sizeof(size_t));
	std::cout << *answer_size << std::endl;
	char* answer = (char*)SocketUse::read(data_sock, *answer_size);
	std::cout << answer;
	std::cout << std::endl;
	delete[] answer;
	delete answer_size;
	close(data_sock);
	while(true)
	{
		auto serv_2 = socket(AF_UNIX, SOCK_STREAM, 0);
		if (serv_2 == -1) { perror("Socket: "); exit(EXIT_FAILURE); }
		std::string req2;
		std::cout << "Enter request to make(ADD, SUB, END): ";
		std::getline(std::cin, req2);
		const char* request2 = req2.c_str();
		addr.sun_family = AF_UNIX;
		strncpy(addr.sun_path, "/tmp/serv_sock_2.sock", sizeof(addr.sun_path) - 1);
		ret = connect(serv_2, (const struct sockaddr *) &addr, sizeof(struct sockaddr_un));
		if (ret == -1) { perror("[CLIENT] Server is down"); exit(EXIT_FAILURE); }
		str_len = req2.size();
		SocketUse::write(serv_2, &str_len, sizeof(size_t));
		SocketUse::write(serv_2, request2, req2.size());
		close(serv_2);
		std::cout << "Finished sending request to server 2!" << std::endl;
		data_sock = accept(sock.getSockfd(), NULL, NULL);
		size_t* answer_size2 = (size_t*)SocketUse::read(data_sock, sizeof(size_t));
		std::cout << *answer_size2 << std::endl;
		int* answer2 = (int*)SocketUse::read(data_sock, *answer_size2);
		std::cout << *answer2;
		std::cout << std::endl;
		delete[] answer2;
		delete answer_size2;
		close(data_sock);
		if (strncmp(request2, "END", 3) == 0)
		{
			std::cout << "Server shut down!" << std::endl;
			break;
		}
	}
	return 0;
}
