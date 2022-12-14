/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 18:05:51 by plouvel           #+#    #+#             */
/*   Updated: 2022/12/02 13:04:53 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HPP 
# define HTTP_HPP 


# include "SocketTypes.hpp"
# include "Traits.hpp"

# include <cctype>
# include <functional>
# include <ostream>
# include <utility>
# include <string>
# include <map>
# include <vector>
# include <algorithm>

# define HTTP_ERRPAGE(X) (HTTP::StatusInfo(\
				X, \
					"<html>\n"\
						"\t<head><title>"X"</title></head>\n"\
						"\t<body>\n"\
							"\t\t<center><h1>"X"</h1></center>\n"\
							"\t\t<hr>\n"\
							"\t\t<center><i>webserv</i></center>\n"\
						"\t</body>\n"\
					"</html>\n"))

# define HTTP_STATUS(X) (HTTP::StatusInfo(\
				X, \
				""))

namespace HTTP
{
	using std::string;

	typedef std::pair<ci_string, std::string>	HeaderField;
	typedef std::map<ci_string, std::string>	HeaderFieldMap;
	typedef std::vector<HeaderField>			HeaderFieldVector;

	typedef std::vector<uint8_t>				Buffer;
	typedef std::pair<const void*, size_t>		DataInfo;

	/* Supported status code. Vim users, press 'gx' to open links (with the cursor under the link obviously). */

	typedef enum eStatusCode
	{
		OK							= 200,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-200-ok */
		Created						= 201,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-201-created */
		MovedPermanently			= 301,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-301-moved-permanently */ 
		SeeOther					= 303,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-303-see-other */
		BadRequest					= 400,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-400-bad-request */
		Forbidden					= 403,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-403-forbidden */
		NotFound					= 404,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-408-request-timeout */
		MethodNotAllowed			= 405,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-405-method-not-allowed */
		RequestTimeout				= 408,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-408-request-timeout */
		ContentTooLarge				= 413,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-413-content-too-large */
		UnsupportedMediaType		= 415,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-415-unsupported-media-type */
		UriTooLong					= 414,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-414-uri-too-long */
		UnprocessableContent		= 422,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-422-unprocessable-content */
		InternalServerError			= 500,	/* https://www.rfc-editor.org/rfc/rfc9110.html#name-500-internal-server-error */
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
		NbrAvailableMethod = 3,						/* Not used, only a placeholder. */
		Err = 3							/* RESERVED */
	} Method;

	struct StatusInfo
	{
		StatusInfo()
			: phrase(), page()
		{}

		StatusInfo(const std::string& phrase, const std::string& page_content)
			: phrase(phrase), page(page_content)
		{
		}

		string	phrase;
		string	page;
	};


	struct UriInfo
	{
		UriInfo()
			: absolute_path("/"), query()
		{}

		std::string	absolute_path, query;
	};

	struct HeaderInfo
	{
		HeaderInfo() :
			method(),
			ver_major(),
			ver_minor(),
			uri(),
			request_line(),
			header_field()
		{}

		Method			method;
		int				ver_major, ver_minor;
		UriInfo			uri;
		std::string		request_line;
		HeaderFieldMap	header_field;
	};

	struct MultiPartInfo
	{
		HeaderFieldMap	header_field;

		std::pair<Buffer::const_iterator, Buffer::const_iterator>	data_it;
	};

	struct ContentInfo
	{
		std::string	value;
		std::map<string, string>	param;
	};

	class Field
	{
		public:

			operator	const ci_string&() const {return (m_str);}

			static inline Field	Server()					{return Field("Server");}
			static inline Field	Date()						{return Field("Date");}
			static inline Field	ContentLength()				{return Field("Content-Length");}
			static inline Field	ContentDisposition()		{return Field("Content-Disposition");}
			static inline Field	ContentTransferEncoding()	{return Field("Content-Transfer-Encoding");}
			static inline Field	ContentType()				{return Field("Content-Type");}
			static inline Field	Connection()				{return Field("Connection");}
			static inline Field	Host()						{return Field("Host");}
			static inline Field	SetCookie()					{return Field("Set-Cookie");}
			static inline Field	Allow()						{return Field("Allow");}
			static inline Field	TransferEncoding()			{return Field("Transfer-Encoding");}
			static inline Field	Location()					{return Field("Location");}
			static inline Field	Status()					{return Field("Status");}

			inline const ci_string&	str() const
			{
				return (m_str);
			}

		private:

			Field();
			Field(const ci_string& field)
				: m_str(field)
			{}

			ci_string	m_str;
	};

	struct IsHostField
	{
		inline bool	operator()(const HeaderFieldMap::value_type& val)
		{
			if (val.first.compare(Field::Host()) == 0)
				return (true);
			return (false);
		}
	};

	extern const char*	CRLF;
	extern const char	TokenMap[256];
	extern const char*	MethodTable[];
	extern const char*	RFC822_DateFormat;
	extern const char*	HttpSlash;
}

#endif
