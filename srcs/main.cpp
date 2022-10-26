#include "ListeningSocket.hpp"
#include "SocketTypes.hpp"
#include <exception>
#include <stdexcept>
#include <string.h>
#include <errno.h>
#include <list>
#include <iostream>
#include <typeinfo>
#include "EPoll.hpp"
#include "ClientSocket.hpp"
#include "WebServ.hpp"

# define BUFFSIZE 1024 

int main()
{
	try
	{
		ft::WebServ	server;
		EPoll&	epoll = server.getPoller();

		server.initListener();
		while (server.loop())
		{
			for (EPoll::iterator it = epoll.begin(); it != epoll.end(); it++)
			{
				InternetSocket*		inSockPtr       = static_cast<InternetSocket*>(it->data.ptr);
				ListeningSocket*	listenSockPtr   = dynamic_cast<ListeningSocket*>(inSockPtr);
				ClientSocket*		clientPtr       = dynamic_cast<ClientSocket*>(inSockPtr);

				// Check if error condition happened on the associated file descriptor watched by epoll.
				if (it->events & EPOLLERR || it->events & EPOLLHUP)
				{
					if (listenSockPtr)
						server.removeListener(listenSockPtr->getFd());
					else
						clientPtr->getBindedSocket()->removeConnection(clientPtr);
				}
				if (it->events & EPOLLIN)
				{
					if (listenSockPtr)
					{
						std::cout << "Accepting new connection\n";
						listenSockPtr->acceptConnection(epoll);
					}
					else
					{
						char		buffer[BUFFSIZE] = {0};
						ssize_t		received_bytes;

						if ((received_bytes = recv(clientPtr->getFd(), buffer, BUFFSIZE, 0)) < 0)
							throw (std::runtime_error("recv"));
						if (received_bytes > 0)
						{
							// When parsing is done, use
							// epoll.modify(clientPtr->getFd(), EPOLLOUT, clientPtr);
							// to write a response to a client.
						}
						if (received_bytes == 0 || it->events & EPOLLRDHUP)
						{
							std::cout << "Bye bye\n";
							clientPtr->getBindedSocket()->removeConnection(clientPtr);
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
		ft::WebServ::Logger::reason(e.what(), strerror(errno));
	}
	return (1);
}
