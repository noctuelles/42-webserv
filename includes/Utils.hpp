/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 11:10:47 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/09 13:03:45 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <sstream>
# include <string>

namespace ft
{
	namespace utils
	{
		extern size_t	DateBufferSize;

		// Return an RFC 822-compliant date format.
		std::string	getRFC822NowDate();
		std::string	formatTimeToRFC822(struct tm* ptrTm);

		template <class T>
			inline std::string	integralToString(T val)
			{
				return (static_cast<std::ostringstream &>(std::ostringstream() << std::dec << val).str());
			}

		template <class T>
			inline T	charToIntegral(char c)
			{
				return (c - '0');
			}
	}
}

#endif
