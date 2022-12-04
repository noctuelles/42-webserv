/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 11:10:47 by plouvel           #+#    #+#             */
/*   Updated: 2022/12/04 23:09:15 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

#include <algorithm>
#include <iterator>
# include <sstream>
# include <string>
# include <vector>
# include "FileDescriptor.hpp"
# include "VirtServ.hpp"

namespace Utils
{
	extern const size_t	DateBufferSize;
	typedef std::pair<IO::FileDescriptor, IO::FileDescriptor>	FdPair;
	typedef std::vector<char>	CStr;

	// Return an RFC 822-compliant date format.
	std::string	getRFC822NowDate();
	std::string	formatTimeToRFC822(struct tm* ptrTm);
	FdPair	pipe();
	std::vector<char>	getCStr(const std::string& str);
	std::string			getFileExtension(const std::string& str);
	std::string			basename(const std::string& str);

	const VirtServ::RouteOptions&	findRoute(const std::string& uri, const VirtServ& virtserv);

	template <class T>
		inline std::string	integralToString(T val)
		{
			return (static_cast<std::ostringstream &>(std::ostringstream() << std::dec << val).str());
		}

	template <class T>
		inline T	stringToIntegral(const std::string& str)
		{
			std::stringstream	ss(str);
			T					ret;

			ss >> ret;
			return (ret);
		}

	template <class T>
		inline T	charToIntegral(char c)
		{
			return (c - '0');
		}

	template <typename Container, typename BinaryPredicate>
		Container	RemoveDuplicate(Container& c, BinaryPredicate pred)
		{
			Container	c_cpy;

			c_cpy.reserve(c.size());
			std::unique_copy(c.begin(), c.end(), std::back_inserter(c_cpy), pred);
			return (c_cpy);
		}
}

#endif
