/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 18:43:06 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/28 18:47:11 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTP.hpp"
#include <utility>

# define DEF_ERRPAGE(X) std::make_pair(\
				X, \
					"<html>\n"\
						"\t<head><title>"X"</title></head>\n"\
						"\t<body>\n"\
							"\t\t<center><h1>"X"</h1></center>\n"\
							"\t\t<hr>\n"\
							"\t\t<center><i>webserv</i></center>\n"\
						"\t</body>\n"\
					"</html>\n")

namespace ft
{
	namespace http
	{
		const char*	CRLF = "\r\n";

		const std::pair<const char*, const char*>	InfoOK = std::make_pair("200 OK", static_cast<const char*>(0));

		const std::pair<const char*, const char*>	InfoBadRequest          = DEF_ERRPAGE("400 Bad Request");
		const std::pair<const char*, const char*>	InfoForbidden           = DEF_ERRPAGE("403 Forbidden");
		const std::pair<const char*, const char*>	InfoNotFound            = DEF_ERRPAGE("404 Not Found");
		const std::pair<const char*, const char*>	InfoRequestTimeout      = DEF_ERRPAGE("408 Request Timeout");
		const std::pair<const char*, const char*>	InfoUriTooLong          = DEF_ERRPAGE("414 Uri Too Long");
		const std::pair<const char*, const char*>	InfoNotImplemented      = DEF_ERRPAGE("501 Not Implemented"); 
		const std::pair<const char*, const char*>	InfoVersionNotSupported = DEF_ERRPAGE("505 HTTP Version Not Supported");
	}
}
