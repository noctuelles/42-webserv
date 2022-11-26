/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/24 17:32:07 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/26 16:38:40 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"
#include "FileUtils.hpp"
#include "Http.hpp"
#include "WebServ.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
#include <cctype>

#include "Utils.hpp"
#include "RequestHandler.hpp"

#ifndef NDEBUG
# include "DebugColors.h"
# include <iomanip>
#endif

namespace HTTP
{
	const char*	RequestParser::StateTable[] = 
	{
		"P_START_REQUEST_LINE",
		"P_PARSE_METHOD",
		"P_PARSE_REQ_LINE",
		"P_HTTP",
		"P_HTTP_MAJOR_VER",
		"P_HTTP_DOT",
		"P_HTTP_MINOR_VER",
		"P_HEADER_FIELD_NAME",
		"P_HEADER_FIELD_VALUE",
		"P_CRLF",
		"P_WS",
		"P_OWS",
		"P_DONE"
	};

	const char*	RequestParser::m_http	= "HTTP/";
	const char*	RequestParser::m_scheme	= "http://";

	const char	RequestParser::m_token[256] = 
	{
	/*   0 nul    1 soh    2 stx    3 etx    4 eot    5 enq    6 ack    7 bel  */
			0,       0,       0,       0,       0,       0,       0,       0,
	/*   8 bs     9 ht    10 nl    11 vt    12 np    13 cr    14 so    15 si   */
			0,       0,       0,       0,       0,       0,       0,       0,
	/*  16 dle   17 dc1   18 dc2   19 dc3   20 dc4   21 nak   22 syn   23 etb */
			0,       0,       0,       0,       0,       0,       0,       0,
	/*  24 can   25 em    26 sub   27 esc   28 fs    29 gs    30 rs    31 us  */
			0,       0,       0,       0,       0,       0,       0,       0,
	/*  32 sp    33  !    34  "    35  #    36  $    37  %    38  &    39  '  */
			0,     '!',      0,      '#',     '$',     '%',     '&',    '\'',
	/*  40  (    41  )    42  *    43  +    44  ,    45  -    46  .    47  /  */
			0,       0,      '*',     '+',      0,      '-',     '.',      0,
	/*  48  0    49  1    50  2    51  3    52  4    53  5    54  6    55  7  */
		   '0',     '1',     '2',     '3',     '4',     '5',     '6',     '7',
	/*  56  8    57  9    58  :    59  ;    60  <    61  =    62  >    63  ?  */
		   '8',     '9',      0,       0,       0,       0,       0,       0,
	/*  64  @    65  A    66  B    67  C    68  D    69  E    70  F    71  G  */
			0,      'a',     'b',     'c',     'd',     'e',     'f',     'g',
	/*  72  H    73  I    74  J    75  K    76  L    77  M    78  N    79  O  */
		   'h',     'i',     'j',     'k',     'l',     'm',     'n',     'o',
	/*  80  P    81  Q    82  R    83  S    84  T    85  U    86  V    87  W  */
		   'p',     'q',     'r',     's',     't',     'u',     'v',     'w',
	/*  88  X    89  Y    90  Z    91  [    92  \    93  ]    94  ^    95  _  */
		   'x',     'y',     'z',      0,       0,       0,      '^',     '_',
	/*  96  `    97  a    98  b    99  c   100  d   101  e   102  f   103  g  */
		   '`',     'a',     'b',     'c',     'd',     'e',     'f',     'g',
	/* 104  h   105  i   106  j   107  k   108  l   109  m   110  n   111  o  */
		   'h',     'i',     'j',     'k',     'l',     'm',     'n',     'o',
	/* 112  p   113  q   114  r   115  s   116  t   117  u   118  v   119  w  */
		   'p',     'q',     'r',     's',     't',     'u',     'v',     'w',
	/* 120  x   121  y   122  z   123  {   124  |   125  }   126  ~   127 del */
		   'x',     'y',     'z',      0,      '|',      0,      '~',       0
	};

	RequestParser::RequestParser(HeaderInfo& header_info, UriInfo& uri_info)
		: m_header_size(0),
		m_index(0),
		m_previous_state(),
		m_current_state(P_START_REQUEST_LINE),
		m_next_state(),
		m_eat(true),
		m_callback_fnct(NULL),
		m_buffer(),
		m_ws_buffer(),
		m_info(header_info),
		m_uri_info(uri_info)
	{
		m_buffer.first.reserve(MaxHeaderFieldSize);
		m_buffer.second.reserve(MaxHeaderFieldSize);
		m_ws_buffer.reserve(MaxHeaderFieldSize);
	}

	RequestParser::~RequestParser()
	{}

	/* Using a simple state machine to parse the request.
	 * That is: no memory allocation, no system call... */

	Buffer::const_iterator	RequestParser::parseHeader(const Buffer& buffer)
	{
		unsigned char							ch = 0;
		std::vector<uint8_t>::const_iterator	it = buffer.begin();

		while (it != buffer.end() && m_current_state != P_DONE)
		{
			ch = *it;

			switch (m_current_state)
			{
				case P_START_REQUEST_LINE:
					switch (ch)
					{
						case 'G':
							m_info.method = Get;
							m_index = 1;
							_changeState(P_PARSE_METHOD);
							break;
						case 'P':
							m_info.method = Post;
							m_index = 1;
							_changeState(P_PARSE_METHOD);
							break;
						case 'D':
							m_info.method = Delete;
							m_index = 1;
							_changeState(P_PARSE_METHOD);
							break;
						default:
							throw (RequestHandler::Exception(NotImplemented));
					};
					break;

				case P_PARSE_METHOD:
					if (ch != MethodTable[m_info.method][m_index])
					{
						if (_isWS(ch) && MethodTable[m_info.method][m_index] == '\0')
							_transitionState(P_OWS, P_PARSE_REQ_LINE);
						else
							throw (RequestHandler::Exception(BadRequest));
					} else m_index++;
					break;

				case P_PARSE_REQ_LINE:
					if (m_info.uri.size() >= MaxUriSize)
						throw (RequestHandler::Exception(UriTooLong));
					switch (ch)
					{
						case ' ':
						case '\t':
							this->_parseURI(m_info.uri);
							_transitionState(P_OWS, P_HTTP), m_index = 0;
							break;
						/* https://httpwg.org/specs/rfc9112.html#request.target */
						case '\n':
						case '\f':
						case '\r':
							throw (RequestHandler::Exception(BadRequest));
							break;
						default:
							m_info.uri.push_back(ch);
					}
					break;

				case P_HTTP:
					if (ch != m_http[m_index])
					{
						if (m_http[m_index] == '\0')
							_dontEat(), _changeState(P_HTTP_MAJOR_VER);
						else
							throw (RequestHandler::Exception(BadRequest));
					} else m_index++;
					break;

				case P_HTTP_MAJOR_VER:
					if (!isdigit(ch))
						throw (RequestHandler::Exception(BadRequest));

					m_info.ver_major = Utils::charToIntegral<int>(ch);
					if (m_info.ver_major != MajorVersionSupported)
						throw (RequestHandler::Exception(VersionNotSupported));

					_changeState(P_HTTP_DOT);
					break;

				case P_HTTP_DOT:
					if (ch != '.')
						throw (RequestHandler::Exception(BadRequest));
					_changeState(P_HTTP_MINOR_VER);
					break;

				case P_HTTP_MINOR_VER:
					if (!isdigit(ch))
						throw (RequestHandler::Exception(BadRequest));

					m_info.ver_minor = Utils::charToIntegral<int>(ch);
					if (m_info.ver_minor != MinorVersionSupported)
						throw (RequestHandler::Exception(VersionNotSupported));
					_transitionState(P_CRLF, P_HEADER_FIELD_NAME);
					break;

				case P_HEADER_FIELD_NAME:
				{
					unsigned char	transformed_ch = m_token[ch];

					if (!transformed_ch) // ch is not a token char.
					{
						switch (ch)
						{
							case ':':
								// Empty field name, bad request.
								if (m_buffer.first.empty())
									throw (RequestHandler::Exception(BadRequest));

								_transitionState(P_OWS, P_HEADER_FIELD_VALUE);
								break;

							case '\r':
							case '\n':
								_dontEat();
								if (m_buffer.first.empty())
									_transitionState(P_CRLF, P_DONE);
								else
								{
									m_buffer.first.clear();
									_transitionState(P_CRLF, P_HEADER_FIELD_NAME);
								}
								break;

							default: // WS is not allowed in field_name (trailing and leading).
								throw (RequestHandler::Exception(BadRequest));
						}
					}
					else
					{
						if (m_buffer.first.size() >= MaxHeaderFieldSize)
							throw (RequestHandler::Exception(BadRequest));
						m_buffer.first.push_back(transformed_ch);
					}
					break;
				}

				case P_HEADER_FIELD_VALUE:
				{
					if (!_isVChar(ch))
					{
						switch (ch)
						{
							case '\r':
							case '\n':
								_dontEat();
								_transitionState(P_CRLF, P_HEADER_FIELD_NAME, &RequestParser::_insertField);
								break;

							case ' ':
							case '\t':
								_dontEat();
								_transitionState(P_WS, P_HEADER_FIELD_VALUE);
								break;

							default:
								;
						}
					}
					else
					{
						if (m_buffer.second.size() >= MaxHeaderFieldSize)
							throw (RequestHandler::Exception(BadRequest));
						m_buffer.second.push_back(ch);
					}

					break;
				}

				case P_CRLF:
					if (ch != '\r')
					{
						if (ch != '\n')
							throw (RequestHandler::Exception(BadRequest));
						if (m_callback_fnct)
							CALL_MEMBER_FN(m_callback_fnct)();
						_changeState(m_next_state);
					}
					break;

				case P_WS:
					if (!_isWS(ch))
					{
						if (!_isCRLF(ch))
							m_buffer.second.append(m_ws_buffer.begin(), m_ws_buffer.end());
						m_ws_buffer.clear();
						_dontEat();
						_changeState(m_next_state);
					}
					else
						m_ws_buffer.push_back(ch);

					break;

				case P_OWS:
					if (!_isWS(ch))
					{
						_dontEat();
						_changeState(m_next_state);
					}
					break;

				default:
					break;
			};

			if (!m_eat)
				m_eat = true;
			else
			{
				if (m_header_size++ >= MaxHeaderSize)
					throw (RequestHandler::Exception(BadRequest));
				it++;
			}

			return (it);
		}

		if (m_current_state == P_DONE)
		{
			/* https://www.rfc-editor.org/rfc/rfc7230.html#section-5.4 */

			m_info.request_line
				.append(MethodTable[m_info.method])
				.append(" ")
				.append(m_info.uri)
				.append(" ")
				.append("HTTP/1.1");

			try
			{
				m_info.header_fields.at(Field::Host());
			}
			catch (const std::logic_error& e)
			{
				throw (RequestHandler::Exception(BadRequest));
			}
			return (true);
		}
		else
			return (false);
	}

	void	RequestParser::_parseURI(const string& uri)
	{
		char	ch = 0;

		m_current_state = P_START_URI;
		for (string::const_iterator it = uri.begin(); it != uri.end(); it++)
		{
			ch = *it;
			switch (m_current_state)
			{
				case P_START_URI:
					ch = std::tolower(ch);
					switch (ch)
					{
						case '/':
							/* https://httpwg.org/specs/rfc9112.html#origin-form */
							_changeState(P_ABSOLUTE_PATH);
							break;
						default:
							throw (RequestHandler::Exception(BadRequest));
					}
					break;

				case P_ABSOLUTE_PATH:
					ch = *it;
					if (ch == '?')
						_changeState(P_QUERY);
					else
						m_uri_info.absolute_path.push_back(ch);
				break;

				case P_QUERY:
					ch = *it;
					m_uri_info.query.push_back(ch);
				break;

				default:
					;
			}
		}

		//std::clog << "Abs path : " << m_uri_info.absolute_path << '\n';
		//std::clog << "Query : " << m_uri_info.query << '\n';
		if (m_current_state == P_ABSOLUTE_FORM || m_uri_info.absolute_path.find("../") != string::npos)
			throw (RequestHandler::Exception(BadRequest));
		//std::clog << "Authority: " << m_uri_info.authority << '\n';
	}

	void	RequestParser::_insertField()
	{
		std::pair<HeaderFieldMap::iterator, bool>	ret = m_info.header_fields.insert(m_buffer);

		if (!ret.second)
		{
			/* https://www.rfc-editor.org/rfc/rfc7230.html#section-5.4 */
			if (ret.first->first == Field::Host().str())
				throw (RequestHandler::Exception(BadRequest));
			/* https://httpwg.org/specs/rfc9110.html#fields.order */
			ret.first->second
				.append(", ")
				.append(m_buffer.second);
		}
		m_buffer.first.clear();
		m_buffer.second.clear();
	}

#ifndef NDEBUG
	void	RequestParser::report()
	{
		using namespace std;
		cout << "\t## " << UYEL << "Parsing report" << CRST << " ##\n\n";
		cout << RED << "Method" << CRST << ": " << GRN << MethodTable[m_info.method] << '\n';
		cout << RED << "Internal state" << CRST << ": " << UCYN << StateTable[m_current_state] << '\n';
		cout << RED << "HTTP Version" << CRST << ": " << GRN <<  m_info.ver_major << '.' << m_info.ver_minor << CRST << "\n\n";
		cout << "\t## " << UYEL << "Parsed header" << CRST << " ##\n";
		for (HeaderFieldMap::const_iterator i = m_info.header_fields.begin(); i != m_info.header_fields.end(); i++)
			std::cout << GRN << i->first << CRST << ": " << BLU << i->second << CRST << "\\n\n";
		cout << "\t## " << UYEL << "End of parsing report" << CRST << " ##\n\n";
	}
#endif
}
