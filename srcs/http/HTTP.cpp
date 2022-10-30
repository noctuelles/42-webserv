/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 18:43:06 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/30 14:30:00 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTP.hpp"
#include <utility>

# define DEF_ERRPAGE(X) (StatusInfo(\
				X, \
					"<html>\n"\
						"\t<head><title>"X"</title></head>\n"\
						"\t<body>\n"\
							"\t\t<center><h1>"X"</h1></center>\n"\
							"\t\t<hr>\n"\
							"\t\t<center><i>webserv</i></center>\n"\
						"\t</body>\n"\
					"</html>\n"))

namespace ft
{
	namespace http
	{
		const char*	CRLF = "\r\n";

		const StatusInfo	StatusInfo_OK                  = DEF_ERRPAGE("400 Bad Request");
		const StatusInfo	StatusInfo_BadRequest          = DEF_ERRPAGE("400 Bad Request");
		const StatusInfo	StatusInfo_Forbidden           = DEF_ERRPAGE("403 Forbidden");
		const StatusInfo	StatusInfo_NotFound            = DEF_ERRPAGE("404 Not Found");
		const StatusInfo	StatusInfo_RequestTimeout      = DEF_ERRPAGE("408 Request Timeout");
		const StatusInfo	StatusInfo_UriTooLong          = DEF_ERRPAGE("414 Uri Too Long");
		const StatusInfo	StatusInfo_NotImplemented      = DEF_ERRPAGE("501 Not Implemented"); 
		const StatusInfo	StatusInfo_VersionNotSupported = DEF_ERRPAGE("505 HTTP Version Not Supported");
	}
}
