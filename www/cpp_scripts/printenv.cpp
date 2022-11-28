#include <iostream>

int main(int ac, char **av, char **envp)
{
	std::cout << "Content-type: text/plain";
	std::cout << "\r\n\r\n";
	if (envp)
	{
		for (; *envp != NULL ; ++envp)
		{
			std::cout << *envp << '\n';
		}
	}
}
