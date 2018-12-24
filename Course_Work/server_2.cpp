#include <iostream>
#include "sockets.hpp"



int main(int argc, char* argv[])
{
	unlink("/tmp/serv_sock_2.sock");
	
	SocketUse sock("/tmp/serv_sock_2.sock", 50000);
	while(true)
	{
		bool break_f = false;
		auto data_sock = accept(sock.getSockfd(), NULL, NULL);
		if (data_sock == -1) { perror("[SERVER] Accept"); exit(EXIT_FAILURE); }
		size_t* req_size = (size_t*)SocketUse::read(data_sock, sizeof(size_t));
		std::cout << *req_size << std::endl;
		char* buf = (char*)SocketUse::read(data_sock, *req_size);
		std::cout << buf;
		std::cout << std::endl;
		int data = 0;
		std::string expr(buf);
		std::string op(buf);
		auto sp = op.find(' ');
		std::string numbers(expr, sp + 1);
		int num1 = atoi(std::string(numbers.c_str(), numbers.find(' ')).c_str());
		int num2 = atoi(numbers.substr(numbers.find(' ')).c_str());
		op.resize(sp);
		if (strcmp(op.c_str(), "ADD") == 0)
		{
			data = num1 + num2;
		}
		else if (strcmp(op.c_str(), "SUB") == 0)
		{
			data = num1 - num2;
		}
		else if (strcmp(op.c_str(), "END") == 0)
		{
			data = 1;
			break_f = true;
		}
		else
		{
			std::cerr << "[SERVER] Error: Operation not supported!";
			exit(EXIT_FAILURE);
		}
		std::cout << "[SERVER] Data: " << data << std::endl;
		struct sockaddr_un addr;
		auto ans_sock = socket(AF_UNIX, SOCK_STREAM, 0);
		addr.sun_family = AF_UNIX;
		strncpy(addr.sun_path, "/tmp/client_sock_0.sock", sizeof(addr.sun_path) - 1);
		connect(ans_sock, (const struct sockaddr *) &addr, sizeof(struct sockaddr_un));
		size_t data_size = sizeof(int);
		SocketUse::write(ans_sock, &data_size, sizeof(size_t));
		SocketUse::write(ans_sock, &data, sizeof(int));
		close(ans_sock);
		delete[] buf;
		delete req_size;
		close(data_sock);
		if (break_f) break;
	}
	return 0;
}
