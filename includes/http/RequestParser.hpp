/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/24 17:32:09 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/31 17:27:36 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef  REQUESTPARSER_CLASS_HPP
# define REQUESTPARSER_CLASS_HPP

# include <cstddef>
# include <string>
# include <utility>
# include <cctype>
# include <vector>

# include <stdint.h>
# include "HTTP.hpp"

namespace ft
{
	namespace http
	{
		/* ################################## ABNF ################################## */

		/* The parser follow synthax defined by the RFC's ABNF below */

		 /* HTTP-message   = start-line
							 *( header-field CRLF )
							 CRLF
							 [ message-body ]

			header-field   = field-name ":" OWS field-value OWS
			field-name     = token
			field-value    = *field-content -> this means we can have empty field-value.
			field-content  = field-vchar
			                 [ 1*( SP / HTAB / field-vchar ) field-vchar ]
			field-vchar    = VCHAR / obs-text
			obs-text       = %x80-FF -> interpreted as opaque data.

			obs-fold       = CRLF 1*( SP / HTAB )
						   ; obsolete line folding
						   ; see Section 3.2.4

			obs-text       = %x80-FF

			OWS            = *( SP / HTAB )
						   ; optional whitespace

			LWS            = [CRLF] 1*( SP | HT)
		*/

		/*
			message-header = field-name ":" [ field-value ]
			field-name     = token
			field-value    = *( field-content | LWS )
			field-content  = <the OCTETs making up the field-value
			                 and consisting of either *TEXT or combinations
			                 of token, separators, and quoted-string>
		 */

		class RequestParser
		{
			public:

				/* ######################## Request parser settings ######################### */

				static const size_t	MaxHeaderSize      = 1024 * 80; // 80kb header
				static const size_t	MaxRequestLineSize = 1024 * 8;  // 8kb uri
				static const size_t	MaxHeaderFieldSize = 1024;
				static const size_t	MaxHeaderFieldNbr  = 2000;

				static const uint8_t	MajorVersionSupported = 1;

				/* ################################# State ################################## */

				// Note: P_CRLF and P_OWS are so called "transition state".
				enum State
				{
					P_START_REQUEST_LINE,
					P_PARSE_METHOD,
					P_PARSE_REQ_LINE,
					P_HTTP,
					P_HTTP_MAJOR_VER,
					P_HTTP_DOT,
					P_HTTP_MINOR_VER,
					P_HEADER_FIELD_NAME,
					P_HEADER_FIELD_VALUE,
					P_HEADER_FIELD_VALUE_OWS,
					P_CRLF,
					P_OWS,
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

				inline Method	getMethod()						const {return (m_info.method);   }
				inline uint8_t	getMajorVersion()				const {return (m_info.ver_major);}
				inline uint8_t	getMinorVersion()				const {return (m_info.ver_minor);}
				inline StatusCode	getErrorCode()				const {return (m_info.err_code); }
				inline const std::string&	getRequestLine()	const {return (m_info.req_line); }

				State		parse(const std::string& buffer);

			private:

				struct ParseInfo
				{
					ParseInfo()
						: method(), ver_major(), ver_minor(), err_code(), req_line(), header_fields()
					{
						req_line.reserve(MaxRequestLineSize);
						header_fields.reserve(300);
					}

					Method		method;
					uint8_t		ver_major;
					uint8_t		ver_minor;
					StatusCode	err_code;
					std::string	req_line;

					std::vector<HeaderField>	header_fields;
				};

				size_t		m_header_size;
				size_t		m_index;

				State		m_previous_state;
				State		m_current_state;
				State		m_next_state;

				std::string::const_iterator	m_headerf_it;

				ParseInfo	m_info;


				inline bool			_isVChar(unsigned char ch)		{return (ch > ' ' && ch < 0x7F);		}
				inline bool			_isTknChar(unsigned char ch)	{return (m_token[ch] != 0);				}
				inline bool			_isOWS(unsigned char ch)		{return (ch == ' ' || ch == '\t');		}
				inline HeaderField&	_backField()					{return (m_info.header_fields.back());	}


				inline State	_errorState(StatusCode err_code)
				{
					m_info.err_code = err_code;
					return (P_DONE_ERR);
				};

				inline void			_transitionState(State new_state, State next_state)
				{
					_changeState(new_state);
					m_next_state = next_state;
				}

				inline void			_changeState(State s)
				{
					m_previous_state = m_current_state;
					m_current_state = s;
				}

				inline State		_previousState() const
				{
					return (m_previous_state);
				}

				/* Static variables */ 

				static const char*	m_http;
				static const char*	m_method[];
				static const char	m_token[256];
		};
	}
}

#endif
