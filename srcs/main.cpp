#include "ServerSocket.hpp"
#include "SocketTypes.hpp"
#include <exception>
#include <stdexcept>
#include <string.h>
#include <errno.h>
#include <iostream>
#include "EPoll.hpp"

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
		EPoll				myEPoll;

		myServerSock.setReusableMode(true);
		myServerSock.setBlockingMode(false);
		myServerSock.bind(INADDR_ANY, 8080);
		myServerSock.listen(42);

		myEPoll.setEventData(&myServerSock);
		myEPoll.setEventMask(EPOLLIN);
		myEPoll.add(myServerSock.getFd());

		while (true)
		{
			EPoll::event_iterator_pair it_pair = myEPoll.waitForEvents(EPoll::NOTIMEOUT);

			std::cout << "There's " << it_pair.second - it_pair.first << " event(s) to look for.\n";

			for (EPoll::event_iterator it = it_pair.first; it != it_pair.second; it++)
			{
				InternetSocket*	ptr;

				ptr = static_cast<InternetSocket*>(it->data.ptr);
				if (myServerSock.getFd() == ptr->getFd())
				{
					std::cout << "caca";
					return (0);
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "webserv: " << e.what() << ": " << strerror(errno) << '\n';
	}
}
