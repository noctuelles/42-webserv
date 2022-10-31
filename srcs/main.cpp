#include "RequestParser.hpp"

using namespace ft;

int main()
{
	std::string	myBuffer("GET / HTTP/1.1\r\nBonjour:   \tToi!   cava    \r\n\r\n");
	ft::http::RequestParser	reqPar;

	reqPar.parse(myBuffer);

	return (1);
}
