/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/24 17:32:07 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/30 21:44:20 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"
#include "HTTP.hpp"
#include "WebServ.hpp"
#include <string>
#include <iostream>
#include <cctype>

namespace ft
{
	namespace http
	{
		const char*	RequestParser::m_method[] =
		{
			"GET",
			"POST",
			"DELETE"
		};

		const char*	RequestParser::m_field_name[] =
		{
			"host",
			"connection"
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
			   ' ',     '!',      0,      '#',     '$',     '%',     '&',    '\'',
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
			: m_size(), m_current_state(), m_info()
		{}

		RequestParser::~RequestParser()
		{}

		/* Using a simple state machine to parse the request.
		 * That is: no memory allocation, no system call... */

		RequestParser::State	RequestParser::parse(const std::string& buffer)
		{
			char	ch;

			for (std::string::const_iterator it = buffer.begin(); it != buffer.end(); it++)
			{
				ch = *it;
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
						// Check if the method is implemented.
						if (ch == m_method[m_info.method][m_index])
							m_index++;
						else if (m_method[m_info.method][m_index] == '\0')
						{
							if (ch != ' ')
								return (_errorState(BadRequest));
							_changeState(P_PARSE_REQ_LINE), m_index = 0;
						}
						else
							return (_errorState(NotImplemented));
						break;

					case P_PARSE_REQ_LINE:
						if (m_index >= MaxRequestLineSize)
							return (_errorState(UriTooLong));
						if (ch == ' ')
							_changeState(P_HTTP), m_index = 0;
						else
							m_info.req_line.push_back(ch);
						break;

					case P_HTTP:
						if (ch == m_http[m_index])
							m_index++;
						else
							return (_errorState(BadRequest));
						if (m_http[m_index] == '\0')
							_changeState(P_HTTP_MAJOR_VER);
						break;

					case P_HTTP_MAJOR_VER:
						if (!std::isdigit(ch))
							return (_errorState(BadRequest));
						m_info.ver_major = ch - '0';
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
						if (!std::isdigit(ch))
							return (_errorState(BadRequest));
						m_info.ver_minor = ch - '0';
						_changeState(P_END);
						break;

					case P_END:
						_transitionState(P_CRLF, P_DONE);

					case P_CRLF:
						if (ch == '\r')
							;
						else
						{
							if (ch != '\n')
								return (_errorState(BadRequest));
							_changeState(m_next_state);
						}
						break;

					default:
						break;
				};
			}
			return (m_current_state);
		}

		inline void	RequestParser::_transitionState(RequestParser::State new_state,
				RequestParser::State next_state)
		{
			m_current_state = new_state;
			m_next_state = next_state;
		}

		inline void	RequestParser::_changeState(State s)
		{
			m_current_state = s;
		}
	}
}


