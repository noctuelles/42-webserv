/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/24 17:32:09 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/30 14:06:10 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef  REQUESTPARSER_CLASS_HPP
# define REQUESTPARSER_CLASS_HPP

# include <cstddef>
# include <string>
# include <utility>
# include <cctype>

# include <stdint.h>
# include "HTTP.hpp"

namespace ft
{
	namespace http
	{
		class RequestParser
		{
			public:

				static const size_t	MaxHeaderSize      = 1024;
				static const size_t	MaxRequestLineSize = 1024;

				static const uint8_t		MajorVersionSupported = 1;

				/* Errors that the parser can return. */

				enum State
				{
					P_START_REQUEST_LINE,
					P_PARSE_METHOD,
					P_PARSE_REQ_LINE,
					P_HTTP,
					P_HTTP_MAJOR_VER,
					P_HTTP_DOT,
					P_HTTP_MINOR_VER,
					P_CRLF,
					P_END,
					P_DONE,
					P_DONE_ERR
				};

				enum Field
				{
					f_host,
					f_connection
				};

				RequestParser();
				~RequestParser();

				// Note: these functions are implicitly declared as inlined.
				Method	getMethod()						const {return (m_info.method);   }
				uint8_t	getMajorVersion()				const {return (m_info.ver_major);}
				uint8_t	getMinorVersion()				const {return (m_info.ver_minor);}
				StatusCode	getErrorCode()				const {return (m_info.err_code); }
				const std::string&	getRequestLine()	const {return (m_info.req_line); }

				State		parse(const std::string& buffer);

			private:

				struct ParseInfo
				{
					ParseInfo()
						: method(), ver_major(), ver_minor(), err_code(), req_line(), header_fields()
					{
						req_line.reserve(MaxRequestLineSize);
					}

					Method		method;
					uint8_t		ver_major;
					uint8_t		ver_minor;
					StatusCode	err_code;
					std::string	req_line;

					std::map<std::string, std::string>	header_fields;
				};

				size_t		m_size;
				size_t		m_index;

				State		m_current_state;
				State		m_next_state;

				ParseInfo	m_info;

				inline void	_transitionState(State new_state, State next_state);
				inline void	_changeState(State s);
				inline State	_errorState(StatusCode err_code)
				{
					m_info.err_code = err_code;
					return (P_DONE_ERR);
				};

				/* Static variables */ 

				static const char*	m_http;
				static const char*	m_method[];
				static const char*	m_field_name[];
				static const char	m_token[256];
		};
	}
}

#endif
