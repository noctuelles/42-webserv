/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 11:11:22 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/23 13:08:58 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"
#include "Http.hpp"
#include "RequestHandler.hpp"
#include <ctime>
#include <stdexcept>
#include <string>

namespace Utils
{
	size_t	const DateBufferSize = 1024;

	std::string	getRFC822NowDate()
	{
		char		buffer[DateBufferSize];
		size_t		length;
		time_t		time;

		time = std::time(NULL); // this call can't fail.
		length = std::strftime(buffer, DateBufferSize, HTTP::RFC822_DateFormat, std::gmtime(&time));
		return (std::string(buffer, length));
	}

	std::string	formatTimeToRFC822(struct tm* ptrTm)
	{
		char		buffer[DateBufferSize];
		size_t		length;

		length = std::strftime(buffer, DateBufferSize, HTTP::RFC822_DateFormat, ptrTm);
		if (length == 0)
			throw (std::logic_error("std::strftime"));
		return (std::string(buffer, length));
	}

	bool		suffixMatch(std::string str, std::string ending)
	{
		return not str.compare(str.length() - ending.length(), ending.length(), ending);
	}

	FdPair	pipe()
	{
		int	fd[2];

		if (::pipe(fd) < 0)
			throw (HTTP::RequestHandler::Exception(HTTP::InternalServerError));
		return (std::make_pair(IO::FileDescriptor(fd[0]), IO::FileDescriptor(fd[1])));
	}
}
