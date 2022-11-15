#include <cstring>
#include <errno.h>
#include <exception>
#include <iostream>

#include "FileUtils.hpp"
#include "RequestParser.hpp"
#include "ResponseHeader.hpp"
#include "Utils.hpp"
#include "WebServ.hpp"
#include "HttpMIME.hpp"
#include "AutoIndex.hpp"
#include "Traits.hpp"

using namespace ft;

int main(int ac, char **av)
{
	if (ac > 2)
	{
		std::cout << "Too many args. Usage: ./webserv /path/to/config/file\n";
		return -1;
	}

	WebServ	serv(av[1]);

	try {
		serv.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << ": " << strerror(errno) << '\n';
	}

	return 0;
}
