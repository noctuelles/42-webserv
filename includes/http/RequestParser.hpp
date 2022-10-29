/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/24 17:32:09 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/29 18:07:35 by plouvel          ###   ########.fr       */
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
					P_DONE
				};

				enum Field
				{
					f_host,
					f_connection
				};

				typedef struct s_parse_info
				{
					Method	method;
					uint8_t	ver_major;
					uint8_t	ver_minor;
					char	req_line[MaxRequestLineSize];
				} t_parse_info;

				RequestParser();
				~RequestParser();

				const t_parse_info&	getInfo() const;

				State		parse(const std::string& buffer);

			private:

				size_t		m_size;
				size_t		m_index;

				State		m_current_state;
				State		m_next_state;

				t_parse_info	m_info;

				inline void	_transitionState(State new_state, State next_state);
				inline void	_changeState(State s);

				/* Static variables */ 

				static const char*	m_http;
				static const char*	m_method[];
				static const char*	m_field_name[];
				static const char	m_token[256];
		};
	}
}

#endif
