#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include <exception>
#include <string.h>
#include <errno.h>
#include <iostream>

int main()
{
	try
	{
		ServerSocket	myServerSock;
		ClientSocket	myClientSock;

		myServerSock.bind(INADDR_ANY, 8000);
		myServerSock.listen(42);
		myServerSock.accept(myClientSock);

		{
			char	buffer[1024];
			size_t rdead;

			while ((rdead = read(myClientSock.getFd(), buffer, 1024)) > 0)
			{
				write(STDOUT_FILENO, buffer, rdead);
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "webserv: " << e.what() << ": " << strerror(errno) << '\n';
	}
}
