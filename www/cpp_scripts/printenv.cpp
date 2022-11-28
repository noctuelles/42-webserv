#include <iostream>

int main(int ac, char **av, char **envp)
{
	if (envp)
	{
		for (; *envp != NULL ; ++envp)
		{
			std::cout << *envp << '\n';
		}
	}
}
