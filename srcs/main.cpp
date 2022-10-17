#include "ServerSocket.hpp"
#include "SocketTypes.hpp"
#include <exception>
#include <stdexcept>
#include <string.h>
#include <errno.h>
#include <list>
#include <iostream>
#include "EPoll.hpp"
#include "HTTPClient.hpp"

# define BUFFSIZE 1024

enum HttpMethod {GET, POST, HEAD};

struct response
{
	struct initial_line
	{
		HttpMethod	method;
	};
};

int main()
{
	try
	{
		ServerSocket		myServerSock;
		EPoll					myEPoll;
		std::list<HTTPClient>	myClient; // list or vector ? list provide constant time removal.

		myServerSock.setReusableMode(true);
		myServerSock.setBlockingMode(false);
		myServerSock.bind(INADDR_ANY, 8080);
		myServerSock.listen(42);

		myEPoll.setEventData(&myServerSock);
		myEPoll.setEventMask(EPOLLIN);
		myEPoll.add(myServerSock.getFd());

		while (true)
		{
			std::cout << "epoll_wait() is blocking...\n";
			myEPoll.waitForEvents(EPoll::NOTIMEOUT);

			std::cout << "\tThere's " << myEPoll.getEventsNbr() << " event(s) to look for.\n";

			for (EPoll::iterator it = myEPoll.beginEvent(); it != myEPoll.endEvent(); it++)
			{
				InternetSocket*	ptr = static_cast<InternetSocket*>(it->data.ptr);

				ptr = static_cast<InternetSocket*>(it->data.ptr); // updown cast
				if (myServerSock.getFd() == ptr->getFd())
				{
					int	incoming_fd;

					while ((incoming_fd = myServerSock.accept()) > 0) // accept all incoming connection
					{
						HTTPClient	theClient(incoming_fd);

						std::cout << "\t\tAccepting connection - " << incoming_fd << ".\n";
						theClient.setBlockingMode(false);
						myClient.push_back(theClient);
						myEPoll.setEvent(EPOLLIN, &myClient.back());
						myEPoll.add(theClient.getFd());
						theClient.shouldBeClose(false); // prevent RAII closing the fd :x(
					}
				}
				else
				{
					if (it->events & EPOLLIN)
					{
						char buffer[BUFFSIZE] = {0};
						ssize_t		received_bytes;

						std::cout << "\t\tConnection - " << ptr->getFd() << " - has something to say.\n";
						if ((received_bytes = recv(ptr->getFd(), buffer, BUFFSIZE, 0)) < 0)
							throw (std::runtime_error("recv"));
						if (received_bytes > 0)
							std::cout << "\t\t\tI just readed " << received_bytes << " bytes!\n";
						else
						{
							std::cout << "\t\t\tNothing to read: i'm gonna close this connection now.\n";
							myEPoll.remove(ptr->getFd());
							myClient.remove(*static_cast<HTTPClient*>(ptr)); // oups, this is O(n), the fd is closed automatically.
																			 // note that downcasting is safe here because we're sure that's
																			 // gonna be an httpclient.
						}
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
