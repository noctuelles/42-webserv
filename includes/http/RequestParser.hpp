/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/24 17:32:09 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/14 22:09:16 by plouvel          ###   ########.fr       */
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

# include "Http.hpp"

using std::string;

#define CALL_MEMBER_FN(ptrToMember)  ((this)->*(ptrToMember))

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

				static const size_t	MaxHeaderSize					= 1024 * 80; // 80kb header
				static const size_t	MaxRequestLineSize				= 1024 * 8;  // 8kb uri
				static const size_t	MaxHeaderFieldSize				= 1024;
				static const size_t	DefaultAllocatedHeaderField		= 1024;
				static const int	MajorVersionSupported			= 1;
				static const int	MinorVersionSupported			= 1;
				static const char*	StateTable[]; //debugging purpose.

				struct HeaderInfo 
				{
					HeaderInfo()
						: method(), ver_major(), ver_minor(), err_code(), req_line(), header_fields()
					{
						req_line.reserve(MaxRequestLineSize);
					}

					Method		method;
					int			ver_major;
					int			ver_minor;
					StatusCode	err_code;
					string		req_line;

					HeaderFieldMap	header_fields;
				};

				enum State
				{
					P_START_REQUEST_LINE = 0,
					P_PARSE_METHOD,
					P_PARSE_REQ_LINE,
					P_HTTP,
					P_HTTP_MAJOR_VER,
					P_HTTP_DOT,
					P_HTTP_MINOR_VER,
					P_HEADER_FIELD_NAME,
					P_HEADER_FIELD_VALUE,
					P_CRLF,
					P_WS,
					P_OWS,
					P_DONE
				};

				RequestParser(HeaderInfo& header_info);
				~RequestParser();

				bool	parse(const std::vector<uint8_t>& buffer, size_t recv_bytes);

#ifndef NDEBUG
				void	report();
#endif

				inline size_t			getHeaderSize()			{return (m_header_size);}

			private:

				typedef void	(RequestParser::*callBackFnct)();

				static const char*	m_http;
				static const char	m_token[256];



				size_t			m_header_size;
				size_t			m_index;

				State			m_previous_state;
				State			m_current_state;
				State			m_next_state;
				bool			m_eat;

				callBackFnct	m_callback_fnct;

				HeaderField		m_buffer;
				string			m_ws_buffer;

				HeaderInfo&		m_info;

				inline bool			_isVChar(unsigned char ch)		{return (ch > ' ' && ch < 0x7F);		}
				inline bool			_isTknChar(unsigned char ch)	{return (m_token[ch] != 0);				}
				inline bool			_isWS(unsigned char ch)			{return (ch == ' ' || ch == '\t');		}
				inline bool			_isCRLF(unsigned char ch)		{return (ch == '\r' || ch == '\n');		}

				inline void			_dontEat()						{m_eat = false;}

				inline void			_transitionState(State new_state, State next_state, callBackFnct fnct = NULL)
				{
					_changeState(new_state);
					m_next_state = next_state;
					m_callback_fnct = fnct;
				}

				inline void			_changeState(State s)
				{
					m_previous_state = m_current_state;
					m_current_state = s;
				}

				void	_insertField();
		};
	}
}

#endif
