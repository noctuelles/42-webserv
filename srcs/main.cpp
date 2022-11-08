#include "RequestParser.hpp"
#include "ResponseHeader.hpp"
#include "Utils.hpp"
#include "WebServ.hpp"
#include <exception>
#include <iostream>
#include <errno.h>
#include <cstring>

using namespace ft;

int main()
{
	WebServ	serv;

	try {
		serv.addListener(8080);
		serv.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << ": " << strerror(errno) << '\n';
	}

	return (0);
}
