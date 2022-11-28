#include <iostream>

int main(void)
{
	std::cout << "Content-type: text/html";
	std::cout << "\r\n\r\n";
	std::cout << "<html><head>Generated Hello World</head><body>";
	std::cout << "Hello World";
	std::cout << "</body><html>";
}
