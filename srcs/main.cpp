#include "ListeningSocket.hpp"
#include "SocketTypes.hpp"
#include <exception>
#include <stdexcept>
#include <string.h>
#include <errno.h>
#include <list>
#include <iostream>
#include "EPoll.hpp"
#include "HTTPClient.hpp"
#include "WebServ.hpp"

# define BUFFSIZE 1024 

int main()
{
	try
	{
		WebServ	server;
		EPoll&	epoll = server.getPoller();

		server.addListener(8080);
		server.addListener(25565);
		while (server.loop())
		{
			for (EPoll::iterator it = epoll.begin(); it != epoll.end(); it++)
			{
				InternetSocket*		sockPtr = static_cast<InternetSocket*>(it->data.ptr);

				if (it->events & EPOLLIN)
				{
					if (server.isAListener(sockPtr))
					{
						std::cout << "Accepting new connection\n";
						static_cast<ListeningSocket*>(sockPtr)->acceptConnection(epoll);
					}
					else
					{
						HTTPClient*	client = static_cast<HTTPClient*>(sockPtr);

						char		buffer[BUFFSIZE] = {0};
						ssize_t		received_bytes;

						if ((received_bytes = recv(client->getFd(), buffer, BUFFSIZE, 0)) < 0)
							throw (std::runtime_error("recv"));
						if (received_bytes > 0)
						{
						}
						else
						{
							std::cout << "Connection removed\n";
							client->getBindedSocket().removeConnection(client);
						}
					}
				}
				if (it->events & EPOLLOUT)
				{
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "webserv: " << e.what() << ": " << strerror(errno) << '\n';
	}
}
