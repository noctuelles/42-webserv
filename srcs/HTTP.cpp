/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 18:43:06 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 23:54:40 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTP.hpp"
#include <utility>

namespace ft
{
	namespace http
	{
		const std::pair<const char*, const char*>	InfoOK = std::make_pair("200 OK", static_cast<const char*>(0));

		const std::pair<const char*, const char*>	InfoBadRequest = std::make_pair(
				"400 Bad Request",
					"<html>\n"\
						"\t<head><title>400 Bad Request</title></head>\n"\
						"\t<body>\n"\
							"\t\t<center><h1>400 Bad Request</h1></center>\n"\
							"\t\t<hr>\n"\
							"\t\t<center><i>webserv</i></center>\n"\
						"\t</body>\n"\
					"</html>\n");

		const std::pair<const char*, const char*>	InfoForbidden = std::make_pair(
				"403 Forbidden",
					"<html>\n"\
						"\t<head><title>403 Forbidden</title></head>\n"\
						"\t<body>\n"\
							"\t\t<center><h1>403 Forbidden</h1></center>\n"\
							"\t\t<hr>\n"\
							"\t\t<center><i>webserv</i></center>\n"\
						"\t</body>\n"\
					"</html>\n");

		const std::pair<const char*, const char*>	InfoNotFound = std::make_pair(
				"404 Not Found",
					"<html>\n"\
						"\t<head><title>404 Not Found</title></head>\n"\
						"\t<body>\n"\
							"\t\t<center><h1>404 Not Found</h1></center>\n"\
							"\t\t<hr>\n"\
							"\t\t<center><i>webserv</i></center>\n"\
						"\t</body>\n"\
					"</html>\n");

		const std::pair<const char*, const char*>	InfoNotImplemented = std::make_pair(
				"501 Not Implemented",
					"<html>\n"\
						"\t<head><title>501 Not Implemented</title></head>\n"\
						"\t<body>\n"\
							"\t\t<center><h1>501 Not Implemented</h1></center>\n"\
							"\t\t<hr>\n"\
							"\t\t<center><i>webserv</i></center>\n"\
						"\t</body>\n"\
					"</html>\n");

		const std::pair<const char*, const char*>	InfoVersionNotSupported = std::make_pair(
				"505 HTTP Version Not Supported",
					"<html>\n"\
						"\t<head><title>505 HTTP Version Not Supported</title></head>\n"\
						"\t<body>\n"\
							"\t\t<center><h1>505 HTTP Version Not Supported</h1></center>\n"\
							"\t\t<hr>\n"\
							"\t\t<center><i>webserv</i></center>\n"\
						"\t</body>\n"\
					"</html>\n");

	}
}
