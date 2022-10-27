/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/24 17:32:09 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/27 14:04:33 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_CLASS_HPP
# define REQUESTPARSER_CLASS_HPP

#include <cstddef>
# include <map>
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
					s_end_request_line,
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
					int	ver_major;
					int	ver_minor;
					char	req_line[MaxRequestLineSize];
				} t_parse_info;

				RequestParser();
				~RequestParser();

				void	init(const char *data, std::size_t size);
				t_parse_info&	getParsingInfo();
				int		parse();

			private:

				const char*	m_data;
				const char*	m_data_end;
				size_t		m_size;
				size_t		m_index;

				size_t		m_header_size;
				State		m_current_state;
				Field		m_curr_field;

				t_parse_info	m_info;

				inline void	_changeState(State s);
				inline void	_changeField(Field f);
				void	_advance(std::size_t n);

				static const char*	m_http;
				static const char*	m_method[];
				static const char*	m_field_name[];
				static const char	m_token[256];
		};
	}
}

#endif
