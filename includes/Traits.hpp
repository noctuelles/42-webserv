/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Traits.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 14:06:45 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/12 12:25:01 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TRAITS_HPP
# define TRAITS_HPP

# include <ostream>
# include <string>

using std::toupper;

struct	ci_char_traits : std::char_traits<char>
{
	static bool	eq(char c1, char c2)
	{
		return (toupper(c1) == toupper(c2));
	}

	static bool	lt(char c1, char c2)
	{
		return (toupper(c1) < toupper(c2));
	}

	static int	compare(const char* s1, const char* s2, std::size_t n)
	{
		while (n-- != 0)
		{
			if (toupper(*s1) < toupper(*s2))
				return (-1);
			if (toupper(*s1) > toupper(*s2))
				return (1);
			++s1; ++s2;
		}
		return (0);
	}
    static const char* find(const char* s, std::size_t n, char a) {
        char const ua (toupper(a));
        while ( n-- != 0 ) 
        {
            if (toupper(*s) == ua)
                return s;
            s++;
        }
        return NULL;
    }
};

typedef	std::basic_string<char, ci_char_traits> ci_string;

std::ostream&	operator<<(std::ostream& out, const ci_string& str);

#endif
