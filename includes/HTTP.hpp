/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 18:05:51 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 18:57:08 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HPP
# define HTTP_HPP

namespace ft
{
	namespace http
	{
		typedef enum eStatusCode
		{
			BadRequest          = 400,
			Forbidden           = 403,
			NotFound            = 404,
			NotImplemented      = 501,
			VersionNotSupported = 505
		} StatusCode;

		extern const char*	DefaultPageBadRequest;
		extern const char*	DefaultPageForbidden;
		extern const char*	DefaultPageNotFound;
		extern const char*	DefaultPageNotImplemented;
		extern const char*	DefaultPageVersionNotSupported;
	}
}

#endif
