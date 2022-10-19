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

# define BUFFSIZE 1024 

enum HttpMethod {GET, POST, HEAD};

struct response
{
	struct initial_line
	{
		HttpMethod	method;
	};
};

namespace http
{
	namespace utils
	{
		inline bool	shouldConnectionBeClose(uint32_t events)
		{
			return (events & EPOLLRDHUP || events & EPOLLHUP || events & EPOLLERR);
		}
	}
}

int main()
{
	try
	{
		ListeningSocket		myServerSock(INADDR_ANY, 8080);
		EPoll				myEPoll(myServerSock.getFd(), EPOLLIN, myServerSock.getFd());

		myServerSock.listen(42);
		while (true)
		{
			myEPoll.waitForEvents(EPoll::NOTIMEOUT);
			for (EPoll::iterator it = myEPoll.begin(); it != myEPoll.end(); it++)
			{
				InternetSocket*	ptr = static_cast<InternetSocket*>(it->data.ptr);

				ptr = static_cast<InternetSocket*>(it->data.ptr); // updown cast
				if (it->events & EPOLLRDHUP)
				{
					std::cout << "EPOLL HUP ! \n";
					return (0);
				}
				if (it->events & EPOLLIN)
				{
					if (myServerSock.getFd() == ptr->getFd())
						myServerSock.acceptConnections();
					else
					{
						HTTPClient&	client = myServerSock[it->data.fd]; // downcast is OK here.

						char buffer[BUFFSIZE] = {0};
						ssize_t		received_bytes;

						std::cout << "\t\tConnection - " << client.getFd() << " - has something to say.\n";
						if ((received_bytes = recv(client.getFd(), buffer, BUFFSIZE, 0)) < 0)
							throw (std::runtime_error("recv"));
						if (received_bytes > 0)
						{
							std::cout << "\t\t\tI just readed " << received_bytes << " bytes!\n";
							client.appendToBuffer(buffer, received_bytes);
						}
						else
						{
							std::cout << "\t\t\tNothing to read: i'm gonna close this connection now.\n";
							std::cout << "\t\t\tThis is what i received throughout our connection:\n" << client.getBuffer();
							client.terminate();
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
