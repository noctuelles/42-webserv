#include "ListeningSocket.hpp"
#include "SocketTypes.hpp"
#include <exception>
#include <stdexcept>
#include <string.h>
#include <fstream>
#include <errno.h>
#include <list>
#include <iostream>
#include <typeinfo>
#include <vector>
#include "EPoll.hpp"
#include "ClientSocket.hpp"
#include "FileUtils.hpp"
#include "WebServ.hpp"
#include "HTTP.hpp"
#include "ResponseHeader.hpp"

# define BUFFSIZE 1024 

using namespace ft;

int main()
{
	ft::http::ResponseHeader	respHeader(1, 1, ft::http::InfoBadRequest.first);

	respHeader.addField("Server", "webserv/0.1");
	respHeader.addField("Content-Lenght", "12982");
	respHeader.addField("Connection", "closed");

	std::cout << respHeader.toString() << '\n';
}

/*int main()
{
	WebServ					server;
	EPoll&					epoll = server.getPoller();
	std::vector<uint8_t>	buffer(BUFFSIZE);

	server.addListener(8080);
	server.initListener();
	std::cout << server.getStatusCodePage(http::UriTooLong) << '\n';

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
					switch (clientPtr->getState())
					{
						case ClientSocket::FETCHING_DATA:
							clientPtr->receive();
							break;
						case ClientSocket::READY_FOR_RESPONSE:
							epoll.modify(clientPtr->getFd(), EPOLLOUT, clientPtr);
							break;
					}
				}
			}
			if (it->events & EPOLLOUT)
			{
			}
		}
	}
	return (1);
}*/
