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
		fd_set			master_set, working_set;
		int			max_fd;

		myServerSock.setReusableMode(true);
		myServerSock.setBlockingMode(false);
		myServerSock.bind(INADDR_ANY, 8080);
		myServerSock.listen(42);

		max_fd = myServerSock.getFd();
		FD_ZERO(&master_set);
		FD_SET(myServerSock.getFd(), &master_set);
		while (true)
		{
			char	buffer[BUFFSIZE];
			int rfds, working_rfds;

			memcpy(&working_set, &master_set, sizeof(master_set));

			std::cout << "Waiting on select()...\n"; // select is a blocking call !
			if ((rfds = select(max_fd + 1, &working_set, NULL, NULL, NULL)) < 0)
				throw (std::runtime_error("select"));

			working_rfds = rfds;
			for (int i = 0; i <= max_fd && working_rfds > 0; i++)
			{
				if (FD_ISSET(i, &working_set))
				{
					working_rfds--;
					if (i == myServerSock.getFd()) // this is our server socket, meaning that we have a new connection to accept
					{
						int	incoming_fd;

						// Accept each new incoming connection.
						// if myServerSock.accept() fails, the functions throw an exception and the program ends.
						// if myServerSock.accept() fails with EGAIN | EWOULDBLOCK, the functions return -1.
						// if myServerSock.accept() sucedeed, it returns the fd of the new incoming connection.
						while ((incoming_fd = myServerSock.accept()) > 0)
						{
							std::cout << "Accepting new incoming connection - " << incoming_fd << " !\n";
							if (fcntl(incoming_fd, F_SETFL, fcntl(incoming_fd, F_GETFL) | O_NONBLOCK) < 0)
								throw (std::runtime_error("fcntl"));
							else
								std::cout << "New connection is set to non blocking\n";
							FD_SET(incoming_fd, &master_set);
							if (incoming_fd > max_fd)
								max_fd = incoming_fd;
						}
					}
					else
					{
						int	rc;

						std::cout << "Existing connection is readable - " << i << " !\n";	
						std::cout << (fcntl(i, F_GETFD) & O_NONBLOCK) << '\n';
						while ((rc = recv(i, buffer, sizeof(buffer), 0)) > 0)
						{
							std::cout << rc << " bytes received.\n";
							write(STDOUT_FILENO, buffer, rc);
						}
						if (rc == 0)
						{
							std::cout << "Closing the connection with " << i << " !\n";
							close(i);
							FD_CLR(i, &master_set);
							while (!FD_ISSET(max_fd, &master_set))
								max_fd--;
						}
						else if (rc < 0 && (errno != EAGAIN || errno != EWOULDBLOCK))
							throw (std::runtime_error("recv"));
					} // End of existing connection readable.
				} // End of checking is the file descriptor has something to say.
			} // End of the for loop
		} // End of the server while loop
	}
	catch (const std::exception& e)
	{
		std::cerr << "webserv: " << e.what() << ": " << strerror(errno) << '\n';
	}
}
