/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileUtils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 16:38:10 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/31 12:15:22 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <sstream>
# include <vector>
# include <limits>
# include <string>
# include <sys/types.h>

namespace ft
{
	namespace io
	{
		size_t	getFileSize(const char *filename);

		std::string	loadFileContent(const char* filename, size_t maxSize = std::numeric_limits<size_t>::max());
	}

	namespace utils
	{
		template <class T>
			std::string	integralToString(T val)
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
