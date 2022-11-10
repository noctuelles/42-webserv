/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 18:05:51 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/10 21:56:07 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HPP
# define HTTP_HPP


# include "SocketTypes.hpp"

#include <cctype>
#include <functional>
# include <utility>
# include <string>
# include <map>
# include <vector>
# include <algorithm>

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

		typedef std::pair<std::string, std::string>			HeaderField;
		typedef std::map<std::string, std::string>			HeaderFieldMap;
		typedef std::vector<HeaderField>					HeaderFieldVector;

		/* Supported status code. Vim users, press 'gx' to open links (with the cursor under the link obviously). */

		typedef enum eStatusCode
		{
			OK							= 200,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-200-ok */
			BadRequest					= 400,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-400-bad-request */
			Forbidden					= 403,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-403-forbidden */
			NotFound					= 404,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-408-request-timeout */
			MethodNotAllowed			= 405,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-405-method-not-allowed */
			RequestTimeout				= 408,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-408-request-timeout */
			ContentTooLarge				= 413,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-413-content-too-large */
			UnsupportedMediaType		= 415,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-415-unsupported-media-type */
			UriTooLong					= 414,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-414-uri-too-long */
			NotImplemented				= 501,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-501-not-implemented */
			VersionNotSupported			= 505,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-505-http-version-not-suppor */
			MaxStatusCode				= 600 	/* Not used, only a placeholder for allocating the right amount of size of memory. */
		} StatusCode;

		/* Supported method */

		typedef enum eMethod
		{
			Get,								/* https://www.rfc-editor.org/rfc/rfc9110.html#name-get */
			Post,								/* https://www.rfc-editor.org/rfc/rfc9110.html#name-post */
			Delete,								/* https://www.rfc-editor.org/rfc/rfc9110.html#name-delete */
			NbrAvailableMethod						/* Not used, only a placeholder. */
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

			std::string						phrase;
			std::pair<size_t, std::string>	page;
		};

		class Field
		{
			public:

				operator	std::string()
				{
					return (m_str);
				}

				static inline Field	Server()			{return Field("Server");}
				static inline Field	Date()				{return Field("Date");}
				static inline Field	ContentLenght()		{return Field("Content-Lenght");}
				static inline Field	ContentType()		{return Field("Content-Type");}
				static inline Field	Connection()		{return Field("Connection");}
				static inline Field	Host()				{return Field("Host");}

				inline const std::string&	str() const
				{
					return (m_str);
				}

				inline const std::string	toLower()
				{
					std::transform(m_str.begin(), m_str.end(), m_str.begin(), LowerCase());
					return (m_str);
				}

			private:

				struct LowerCase : std::unary_function<char, char>
				{
					inline char	operator()(char c)
					{
						return (std::tolower(c));
					}
				};

				Field();
				Field(const std::string& field)
					: m_str(field)
				{}

				std::string	m_str;
		};

		struct IsHostField
		{
			inline bool	operator()(const HeaderFieldMap::value_type& val)
			{
				if (val.first.compare(Field::Host().toLower()) == 0)
					return (true);
				return (false);
			}
		};

		extern const char*	CRLF;
		extern const char*	MethodTable[];
		extern const char*	RFC822_DateFormat;
	}
}

#endif
