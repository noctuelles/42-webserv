/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/24 17:32:09 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/28 14:22:10 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef  REQUESTPARSER_CLASS_HPP
# define REQUESTPARSER_CLASS_HPP

# include <cstddef>
# include <string>
# include <utility>
# include <cctype>

# include <stdint.h>

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
					s_start_request_line,
					s_parse_method,
					s_parse_req_line,
					s_http,
					s_http_major_ver,
					s_http_dot,
					s_http_minor_ver,
					s_crlf,
					s_end,
					s_done
				};

				enum Method
				{
					m_get,
					m_post,
					m_delete
				};

				enum Field
				{
					f_host,
					f_connection
				};

				typedef struct s_parse_info
				{
					Method	method;
					int		ver_major;
					int		ver_minor;
					char	req_line[MaxRequestLineSize];
				} t_parse_info;

				RequestParser();
				~RequestParser();

				const t_parse_info&	getInfo() const;

				int		parse(const std::string& buffer);

			private:

				size_t		m_size;
				size_t		m_index;

				State		m_current_state;
				State		m_next_state;

				t_parse_info	m_info;

				inline void	_transitionState(State new_state, State next_state);
				inline void	_changeState(State s);

				static const char*	m_http;
				static const char*	m_method[];
				static const char*	m_field_name[];
				static const char	m_token[256];
		};
	}
}

#endif
