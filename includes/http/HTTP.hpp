/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 18:05:51 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/30 21:31:19 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HPP
# define HTTP_HPP

#include <exception>
# include <utility>
# include <string>
# include <map>

# define HTTP_ERRPAGE(X) (ft::http::StatusInfo(\
				X, \
					"<html>\n"\
						"\t<head><title>"X"</title></head>\n"\
						"\t<body>\n"\
							"\t\t<center><h1>"X"</h1></center>\n"\
							"\t\t<hr>\n"\
							"\t\t<center><i>webserv</i></center>\n"\
						"\t</body>\n"\
					"</html>\n"))

# define HTTP_STATUS(X) (ft::http::StatusInfo(\
				X, \
				""))

namespace ft
{
	namespace http
	{
		typedef std::pair<std::string, std::string>				StringPair;
		typedef std::pair<const std::string, const std::string>	ConstStringPair;
		typedef std::map<const std::string, std::string>		HeaderFieldMap;

		typedef enum eStatusCode
		{
			OK                  = 200,
			BadRequest          = 400,
			Forbidden           = 403,
			NotFound            = 404,
			RequestTimeout      = 408,
			UriTooLong          = 414,
			NotImplemented      = 501,
			VersionNotSupported = 505,
			MaxStatusCode       = 600
		} StatusCode;

		typedef enum eMethod
		{
			Get,
			Post,
			Delete,
			NbrAvailableMethod
		} Method;

		struct StatusInfo
		{
			StatusInfo()
				: phrase(), page()
			{}

			StatusInfo(const std::string& phrase, const std::string& page_content)
				: phrase(phrase), page()
			{
				page.first = page_content.size();
				page.second = page_content;
			}

			std::string				phrase;
			std::pair<size_t, std::string>	page;
		};

		extern const char*	CRLF;

	}
}

#endif
