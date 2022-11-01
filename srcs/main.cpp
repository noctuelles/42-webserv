#include "RequestParser.hpp"
#include <iostream>

using namespace ft;

int main()
{
	std::string	myBuffer("GET / HTTP/1.2\r\ncaca:\r\ncacapr: prout\r\n\r\n");
	ft::http::RequestParser	reqPar;

	switch (reqPar.parse(myBuffer))
	{
		case http::RequestParser::P_DONE:
			std::cout << "Parsing done.\n";
			break;
		case http::RequestParser::P_DONE_ERR:
			std::cout << "Parsing failed.\n";
			break;
		default:
			std::cout << "Parsing still going\n";
	}

	for (std::vector<ft::http::HeaderField>::iterator it = reqPar.getHeaderFields().begin(); it != reqPar.getHeaderFields().end(); it++)
	{
		std::cout << it->first << ": " << it->second << "\\n" << '\n';
  }

	std::string	mySecondBuffer("");

	return (0);
}
