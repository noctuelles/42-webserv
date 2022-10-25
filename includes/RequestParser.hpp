/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/24 17:32:09 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/25 18:41:56 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_CLASS_HPP
# define REQUESTPARSER_CLASS_HPP

#include <cstddef>
# include <map>
# include <utility>
# include <cctype>

# include <stdint.h>

class RequestParser
{
	public:

		static const size_t	MaxHeaderSize      = 1024;
		static const size_t	MaxRequestLineSize = 1024;

		static const uint8_t		MajorVersionSupported = 1;

		/* Errors that the parser can return. */

		static const int			ErrMethodNotImplemented    = 501;
		static const int			ErrHttpVersionNotSupported = 505;
		static const int			ErrUriTooLong              = 414;
		static const int			ErrBadRequest              = 400;

		static const int			OK                      = 200;

		enum State
		{
			s_start_method,
			s_parse_method,
			s_parse_req_line,
			s_http,
			s_http_major_ver,
			s_http_dot,
			s_http_minor_ver,
			s_cr,
			s_lf,
			s_start_header,
			s_parse_header,
			s_done,
			s_err_state
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

#endif
