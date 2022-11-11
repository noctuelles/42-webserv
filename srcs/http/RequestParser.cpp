/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/24 17:32:07 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/11 21:24:03 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"
#include "FileUtils.hpp"
#include "HTTP.hpp"
#include "WebServ.hpp"
#include <string>
#include <iostream>
#include <cctype>

#include "Utils.hpp"

#ifndef NDEBUG
# include "DebugColors.h"
# include <iomanip>
#endif

namespace ft
{
	namespace http
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
			"P_HEADER_FIELD_VALUE_OWS",
			"P_CRLF",
			"P_OWS",
			"P_WS",
			"P_END",
			"P_DONE",
			"P_DONE_ERR"
		};

		const char*	RequestParser::m_http = "HTTP/";

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

		RequestParser::RequestParser()
			: m_header_size(0),
			m_index(0),
			m_previous_state(),
			m_current_state(P_START_REQUEST_LINE),
			m_next_state(),
			m_eat(true),
			m_callback_fnct(NULL),
			m_buffer(),
			m_ws_buffer(),
			m_info()
		{}

		RequestParser::~RequestParser()
		{}

		/* Using a simple state machine to parse the request.
		 * That is: no memory allocation, no system call... */

		RequestParser::State	RequestParser::parse(const std::vector<uint8_t>& buffer, size_t recv_bytes)
		{
			unsigned char							ch = 0;
			std::vector<uint8_t>::const_iterator	it = buffer.begin();

			while (recv_bytes--)
			{
				ch = *it;
				if (m_header_size++ >= MaxHeaderSize)
					return (_errorState(BadRequest));
				switch (m_current_state)
				{
					case P_START_REQUEST_LINE:
						// Determine the method.
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
								return (_errorState(BadRequest));
						};
						break;

					case P_PARSE_METHOD:
						if (ch != MethodTable[m_info.method][m_index])
						{
							if (_isWS(ch) && MethodTable[m_info.method][m_index] == '\0')
								_transitionState(P_OWS, P_PARSE_REQ_LINE);
							else
								return (_errorState(NotImplemented));
						} else m_index++;
						break;

					case P_PARSE_REQ_LINE:
						if (m_info.req_line.size() >= MaxRequestLineSize)
							return (_errorState(UriTooLong));
						if (_isWS(ch))
							_transitionState(P_OWS, P_HTTP), m_index = 0;
						else
							m_info.req_line.push_back(ch);
						break;

					case P_HTTP:
						if (ch != m_http[m_index])
						{
							if (m_http[m_index] == '\0')
								_dontEat(), _changeState(P_HTTP_MAJOR_VER);
							else
								return (_errorState(BadRequest));
						} else m_index++;
						break;

					case P_HTTP_MAJOR_VER:
						if (!isdigit(ch))
							return (_errorState(BadRequest));
						m_info.ver_major = utils::charToIntegral<int>(ch);
						if (m_info.ver_major != MajorVersionSupported)
							return (_errorState(VersionNotSupported));
						_changeState(P_HTTP_DOT);
						break;

					case P_HTTP_DOT:
						if (ch != '.')
							return (_errorState(BadRequest));
						_changeState(P_HTTP_MINOR_VER);
						break;

					case P_HTTP_MINOR_VER:
						if (!isdigit(ch))
							return (_errorState(BadRequest));
						m_info.ver_minor = utils::charToIntegral<int>(ch);
						if (m_info.ver_minor != MinorVersionSupported)
							return (_errorState(VersionNotSupported));
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
										return (_errorState(BadRequest));

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
									return (_errorState(BadRequest));
							}
						}
						else
						{
							if (m_buffer.first.size() >= MaxHeaderFieldSize)
								return (_errorState(BadRequest));
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
								return (_errorState(BadRequest));
							m_buffer.second.push_back(ch);
						}

						break;
					}

					case P_CRLF:
						if (ch != '\r')
						{
							if (ch != '\n')
								return (_errorState(BadRequest));
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

					case P_DONE:
						return (P_DONE);
						break;

					default:
						break;
				};

				if (!m_eat)
					m_eat = true, recv_bytes++;
				else
					it++;
			}
			return (m_current_state);
		}

#ifndef NDEBUG
		void	RequestParser::report()
		{
			using namespace std;
			cout << "\t## " << UYEL << "Parsing report" << CRST << " ##\n\n";
			cout << RED << "Method" << CRST << ": " << GRN << MethodTable[getMethod()] << '\n';
			cout << RED << "Internal state" << CRST << ": " << UCYN << StateTable[m_current_state] << '\n';
			cout << RED << "HTTP Version" << CRST << ": " << GRN <<  getMajorVersion() << '.' << getMinorVersion() << CRST << "\n\n";
			cout << "\t## " << UYEL << "Parsed header" << CRST << " ##\n";
			for (HeaderFieldMap::const_iterator i = getHeaderFields().begin(); i != getHeaderFields().end(); i++)
				std::cout << GRN << i->first << CRST << ": " << BLU << i->second << CRST << "\\n\n";
			cout << "\t## " << UYEL << "End of parsing report" << CRST << " ##\n\n";
		}
#endif

	}
}
