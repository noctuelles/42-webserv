#include "WebServ.hpp"

using namespace ft;

int main()
{
	WebServ					server;

	server.addListener(8080);
	server.initListener();
	server.run();

	return (1);
}
