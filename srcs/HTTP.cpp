/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 18:43:06 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 18:58:35 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTP.hpp"

namespace ft
{
	namespace http
	{
		const char*	DefaultPageBadRequest =
					"<html>\n"\
						"\t<head><title>400 Bad Request</title></head>\n"\
						"\t<body>\n"\
							"\t\t<center><h1>400 Bad Request</h1></center>\n"\
							"\t\t<hr>\n"\
							"\t\t<center><i>webserv</i></center>\n"\
						"\t</body>\n"\
					"</html>\n";

		const char*	DefaultPageForbidden =
					"<html>\n"\
						"\t<head><title>403 Forbidden</title></head>\n"\
						"\t<body>\n"\
							"\t\t<center><h1>403 Forbidden</h1></center>\n"\
							"\t\t<hr>\n"\
							"\t\t<center><i>webserv</i></center>\n"\
						"\t</body>\n"\
					"</html>\n";

		const char*	DefaultPageNotFound =
					"<html>\n"\
						"\t<head><title>404 Not Found</title></head>\n"\
						"\t<body>\n"\
							"\t\t<center><h1>404 Not Found</h1></center>\n"\
							"\t\t<hr>\n"\
							"\t\t<center><i>webserv</i></center>\n"\
						"\t</body>\n"\
					"</html>\n";

		const char*	DefaultPageNotImplemented =
					"<html>\n"\
						"\t<head><title>501 Not Implemented</title></head>\n"\
						"\t<body>\n"\
							"\t\t<center><h1>501 Not Implemented</h1></center>\n"\
							"\t\t<hr>\n"\
							"\t\t<center><i>webserv</i></center>\n"\
						"\t</body>\n"\
					"</html>\n";

		const char*	DefaultPageVersionNotSupported =
					"<html>\n"\
						"\t<head><title>505 HTTP Version Not Supported</title></head>\n"\
						"\t<body>\n"\
							"\t\t<center><h1>505 HTTP Version Not Supported</h1></center>\n"\
							"\t\t<hr>\n"\
							"\t\t<center><i>webserv</i></center>\n"\
						"\t</body>\n"\
					"</html>\n";

	}
}
