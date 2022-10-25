/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/24 17:32:07 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/25 13:13:04 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"
#include <string.h>
#include <cctype>

const char*	RequestParser::_M_method[] =
{
	"GET",
	"POST",
	"DELETE"
};

const char*	RequestParser::_M_field_name[] =
{
	"connection"
};

const char*	RequestParser::_M_http = "HTTP/";

const char	RequestParser::_M_token[256]
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
	: _M_data(), _M_size(), _M_current_state(s_start_method), _M_info()
{}

RequestParser::~RequestParser()
{}

void	RequestParser::init(const char *data, std::size_t size)
{
	_M_data = data;
	_M_data_end = data + size;
	_M_size = size;
}

RequestParser::t_parse_info&	RequestParser::getParsingInfo()
{
	return (_M_info);
}

/* Using a simple state machine to parse the request.
 * That is: no memory allocation, no system call... */

int	RequestParser::parse()
{
	char	ch;

	_M_index = 1;
	while (_M_data != _M_data_end)
	{
		ch = *_M_data;
		switch (_M_current_state)
		{
			case s_start_method:
				// Determine the method.
				switch (ch)
				{
					case 'G':
						_M_info.method = m_get;
						_changeState(s_parse_method);
						break;
					case 'P':
						_M_info.method = m_post;
						_changeState(s_parse_method);
						break;
					case 'D':
						_M_info.method = m_delete;
						_changeState(s_parse_method);
						break;
					default:
						return (ErrBadRequest);
				};
				break;

			case s_parse_method:
				// Check if the method is implemented.
				if (ch == _M_method[_M_info.method][_M_index])
					_M_index++;
				else if (ch == ' ' && _M_method[_M_info.method][_M_index] == '\0')
					_changeState(s_parse_req_line), _M_index = 0;
				else
					return (ErrMethodNotImplemented);
				break;

			case s_parse_req_line:
				if (_M_index >= MaxRequestLineSize)
					return (ErrUriTooLong);
				if (ch == ' ')
					_changeState(s_http), _M_info.req_line[_M_index] = '\0', _M_index = 0;
				else
					_M_info.req_line[_M_index++] = ch;
				break;

			case s_http:
				if (ch == _M_http[_M_index])
					_M_index++;
				else
					return (ErrBadRequest);
				if (_M_http[_M_index] == '\0')
					_changeState(s_http_major_ver);
				break;

			case s_http_major_ver:
				if (!std::isdigit(ch))
					return (ErrBadRequest);
				_M_info.ver_major = ch - '0';
				if (_M_info.ver_major != MajorVersionSupported)
					return (ErrHttpVersionNotSupported);
				_changeState(s_http_dot);
				break;

			case s_http_dot:
				if (ch != '.')
					return (ErrBadRequest);
				_changeState(s_http_minor_ver);
				break;

			case s_http_minor_ver:
				if (!std::isdigit(ch))
					return (ErrBadRequest);
				_M_info.ver_minor = ch - '0';
				_changeState(s_cr);
				break;

			case s_cr:
				if (ch != '\r')
				{
					if (ch != '\n')
						return (ErrBadRequest);
					_changeState(s_done);
				}
				else
					_changeState(s_lf);
				break;

			case s_lf:
				if (ch != '\n')
					return (ErrBadRequest);
				_changeState(s_done);
				break;

			default:
				break;
		};
		_M_data++;
	}
	return (0);
}

inline void	RequestParser::_changeState(State s)
{
	_M_current_state = s;
}
