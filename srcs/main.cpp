#include "RequestParser.hpp"
#include "WebServ.hpp"
#include <iostream>

using namespace ft;

int main()
{
	WebServ	serv;

	serv.addListener(8080);
	serv.initListener();
	serv.run();

	return (0);
}
