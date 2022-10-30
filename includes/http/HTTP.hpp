/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 18:05:51 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/30 14:29:54 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HPP
# define HTTP_HPP

#include <exception>
# include <utility>
# include <string>
# include <map>

namespace ft
{
	namespace http
	{
		typedef std::pair<std::string, std::string>				StringPair;
		typedef std::pair<const std::string, const std::string>	ConstStringPair;
		typedef std::map<const char*, std::string>				HeaderFieldMap;

		typedef enum eStatusCode
		{
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
			StatusInfo(const char* phrase, const char* page, size_t page_size)
				: phrase(phrase), page(page), page_size(page_size)
			{}

			StatusInfo(const char* phrase, const char* page)
				: phrase(phrase), page(page), page_size()
			{
				page_size = sizeof(page);
			}

			const char*	phrase;
			const char*	page;
			size_t		page_size;
		};

		extern const StatusInfo	StatusInfo_OK;
		extern const StatusInfo	StatusInfo_BadRequest;
		extern const StatusInfo	StatusInfo_Forbidden;
		extern const StatusInfo	StatusInfo_NotFound;
		extern const StatusInfo	StatusInfo_RequestTimeout;
		extern const StatusInfo	StatusInfo_NotImplemented;
		extern const StatusInfo	StatusInfo_VersionNotSupported;
		extern const StatusInfo	StatusInfo_UriTooLong;

		extern const char*	CRLF;

	}
}

#endif
