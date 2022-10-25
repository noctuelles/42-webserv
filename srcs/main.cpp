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

		server.addSocket(8080);
		server.launch();
		while (true)
		{
			std::cout << "Blocking on epoll_wait()\n";
			myEPoll.waitForEvents(EPoll::NOTIMEOUT);
			for (EPoll::iterator it = myEPoll.begin(); it != myEPoll.end(); it++)
			{
				InternetSocket*		sockPtr = static_cast<InternetSocket*>(it->data.ptr);
				ListeningSocket*	listenSockPtr = NULL;
				HTTPClient*			clientPtr = NULL;

				if (it->events & EPOLLIN)
				{
					if (server.isListeningSocket(sockPtr))
					{
						std::cout << "Accepting new connection\n";
						static_cast<ListeningSocket*>(sockPtr)->acceptConnection(server.getPoller());
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
							client->parser.init(buffer, received_bytes);
						}
						else
						{
							std::cout << "\t\t\tNothing to read: i'm gonna close this connection now.\n";
							std::cout << "\t\t\tThis is what i received throughout our connection:\n" << client->getBuffer();
							myServerSock.removeConnection(client);
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
