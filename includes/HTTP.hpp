/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 18:05:51 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/27 16:54:37 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HPP
# define HTTP_HPP

# include <utility>

namespace ft
{
	namespace http
	{
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

		extern const std::pair<const char*, const char*>	InfoOK;
		extern const std::pair<const char*, const char*>	InfoBadRequest;
		extern const std::pair<const char*, const char*>	InfoForbidden;
		extern const std::pair<const char*, const char*>	InfoNotFound;
		extern const std::pair<const char*, const char*>	InfoRequestTimeout;
		extern const std::pair<const char*, const char*>	InfoNotImplemented;
		extern const std::pair<const char*, const char*>	InfoVersionNotSupported;
		extern const std::pair<const char*, const char*>	InfoUriTooLong;
	}
}

#endif
