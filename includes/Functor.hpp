#ifndef FUNCTOR_HPP
# define FUNCTOR_HPP

# include <functional>

namespace Functor 
{
	struct CharDuplicate : public std::binary_function<char, char, bool>
	{
		CharDuplicate(char c) :
			c(c) {}

		result_type	operator()(first_argument_type c1, first_argument_type c2)
		{
			return (c1 == c && c2 == c);
		}

		char c;
	};
}

#endif
