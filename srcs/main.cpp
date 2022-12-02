#include <cstring>
#include <errno.h>
#include <iostream>

#include "WebServ.hpp"

int main(int ac, char **av)
{
	if (ac > 2)
	{
		std::cerr << "Too many args. Usage: ./webserv /path/to/config/file\n";
		return -1;
	}

	try
	{
		WebServ	serv(av[1]);
		if (serv.run() < 0)
			return (2);
	}
	catch (VirtServInfo::ConfigFileError& e)
	{
		std::cerr << e.what();
		return (1);
	}
	catch (std::runtime_error& e)
	{
		std::cerr << BRED << "Error: " << RESET << e.what() << ": " << strerror(errno) << "\n";
		return (1);
	}
	return 0;
}
