/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/24 17:32:09 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/25 13:11:46 by plouvel          ###   ########.fr       */
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
			s_done,
			s_err_state
		};

		enum Method
		{
			m_get,
			m_post,
			m_delete
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

		const char*	_M_data;
		const char*	_M_data_end;
		std::size_t	_M_size;
		std::size_t	_M_index;

		std::size_t	_M_header_size;
		State		_M_current_state;

		t_parse_info	_M_info;

		inline void	_changeState(State s);
		void	_advance(std::size_t n);

		static const char*	_M_http;
		static const char*	_M_method[];
		static const char*	_M_field_name[];
		static const char	_M_token[256];
};

#endif
