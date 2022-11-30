/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 11:11:22 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/30 16:31:46 by plouvel          ###   ########.fr       */
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

	FdPair	pipe()
	{
		int	fd[2];

		if (::pipe(fd) < 0)
			throw (HTTP::RequestHandler::Exception(HTTP::InternalServerError));
		return (std::make_pair(IO::FileDescriptor(fd[0]), IO::FileDescriptor(fd[1])));
	}

	std::vector<char>	getCStr(const std::string& str)
	{
		std::vector<char>	cstr(str.begin(), str.end());

		cstr.push_back('\0');
		return (cstr);
	}

	std::string	getFileExtension(const std::string& str)
	{
		std::string::size_type	dot_pos = str.rfind('.');

		if (dot_pos == std::string::npos)
			return (str);
		else
			return (str.substr(dot_pos, str.length()));
	}

	const VirtServ::RouteOptions&	findRoute(const std::string& uri, const VirtServ& virtserv)
	{
		typedef vector<VirtServ::RouteOptions>::const_iterator	RouteOptionsIt;

		const vector<VirtServ::RouteOptions>&					routes = virtserv.m_routes_vec;
		vector<RouteOptionsIt>									matching_candidate;

		for (RouteOptionsIt it = routes.begin(); it != routes.end(); it++)
		{
			if (uri.compare(0, it->m_location_match.length(), it->m_location_match) == 0)
				matching_candidate.push_back(it);
		}

		// Select the longest matching location match.
		vector<RouteOptionsIt>::const_iterator	best_candidate = std::max_element(matching_candidate.begin(), matching_candidate.end());

		// Either our route vector was empty or no matching candidate
		if (best_candidate != matching_candidate.end())
			return (**best_candidate);
		else
			return(virtserv.m_default_route_options);
	}
}
