#ifndef __SOCKET_USE_
#define __SOCKET_USE_

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

class SocketUse
{
	public:
		SocketUse(const std::string name);
		~SocketUse();

		static void write(int data_sock, void* data, size_t bytes);
		static char* read(int data_sock, size_t bytes);
			
		int getSockfd() const;
	private:
		int _sockfd;
		std::string _sock_name;
		struct sockaddr_un _name;
};



#endif // __SOCKET_USE_
