#include <cstring>
#include <errno.h>
#include <exception>
#include <iostream>

#include "FileUtils.hpp"
#include "RequestParser.hpp"
#include "ResponseHeader.hpp"
#include "Utils.hpp"
#include "WebServ.hpp"
#include "AutoIndex.hpp"
#include "Traits.hpp"
#include "StatusInfoPages.hpp"
#include "StringArray.hpp"

int main(int ac, char **av)
{
	if (ac > 2)
	{
		std::cout << "Too many args. Usage: ./webserv /path/to/config/file\n";
		return -1;
	}

	WebServ	serv(av[1]);

	if (serv.run() < 0)
		return (1);

	return 0;
}
