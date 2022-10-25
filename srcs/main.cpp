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
		EPoll				myEPoll(myServerSock.getFd(), EPOLLIN, &myServerSock);

		myServerSock.listen(42);
		myEPoll.setFdFlags(O_CLOEXEC); // Because we'll be forking and execve'ing for CGI, we DON'T want our child to herit the epoll instance!
		while (true)
		{
			std::cout << "Blocking on epoll_wait()\n";
			myEPoll.waitForEvents(EPoll::NOTIMEOUT);
			for (EPoll::iterator it = myEPoll.begin(); it != myEPoll.end(); it++)
			{
				InternetSocket*	sockPtr = static_cast<InternetSocket*>(it->data.ptr);

				if (it->events & EPOLLIN)
				{
					if (sockPtr->getFd() == myServerSock.getFd())
					{
						std::cout << "Accepting new connection\n";
						myServerSock.acceptConnection(myEPoll);
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
