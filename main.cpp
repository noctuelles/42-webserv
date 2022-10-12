#include "ServerSocket.hpp"
#include "SocketTypes.hpp"
#include <exception>
#include <stdexcept>
#include <string.h>
#include <errno.h>
#include <iostream>

# define BUFFSIZE 1024

int main()
{
	try
	{
		ServerSocket		myServerSock;
		InternetSocket		myClient;

		myServerSock.setReusableMode(true);
		myServerSock.setBlockingMode(true);
		myServerSock.bind(INADDR_ANY, 8080);
		myServerSock.listen(42);

		while (1)
		{
			char	buffer[BUFFSIZE];
			ssize_t	read_bytes;

			memset(buffer, 0, sizeof(char) * BUFFSIZE);
			while (myServerSock.accept(myClient) == false)
			{
				write(STDOUT_FILENO, ".", 1);
				usleep(100000);
			}

			std::cout << "\n" << "Client accepted !\n";
			std::cout << "Is the client a blocking socket ? " << myClient.isBlocking() << '\n';

			while ((read_bytes = recv(myClient.getFd(), buffer, BUFFSIZE, 0)) > 0)
				write(STDOUT_FILENO, buffer, read_bytes);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "webserv: " << e.what() << ": " << strerror(errno) << '\n';
	}
}
