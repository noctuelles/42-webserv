#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
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
		ServerSocket	myServerSock;

		myServerSock.allowReusable();
		myServerSock.bind(INADDR_ANY, 8080);
		myServerSock.listen(42);

		fd_set			rfds, cfds;
		int				max_sd;

		FD_ZERO(&cfds);
		max_sd = myServerSock.getFd();
		FD_SET(myServerSock.getFd(), &cfds); // add the socket fd to the set.

		while (true)
		{
			ClientSocket	myClientSock;
			size_t			rdead;
			char			buffer[BUFFSIZE] = {0};

			memcpy(&rfds, &cfds, sizeof(cfds));

			if (select(FD_SETSIZE, &rfds, NULL, NULL, NULL) == -1)
				throw(std::runtime_error("select"));

			for (size_t i = 0; i < FD_SETSIZE; i++)
			{
				if (FD_ISSET(i, &rfds)) // this file descriptor has something to say.
				{
					if (i == myServerSock.getFd()) // add a new connection
					{
						myServerSock.accept(myClientSock);
						FD_SET(myClientSock.getFd(), &cfds);
					}
					else
					{
						while ((rdead = read(i, buffer, BUFFSIZE)) > 0) // read blocks!!
							write(STDOUT_FILENO, buffer, rdead);
						FD_CLR(i, &cfds); // at this point the client has finished talking.
					}
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "webserv: " << e.what() << ": " << strerror(errno) << '\n';
	}
}
