/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/26 18:00:00 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/26 18:38:31 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADERPARSER_HPP
# define HEADERPARSER_HPP

# include "Parser.hpp"
# include "HeaderFieldParser.hpp"

namespace HTTP
{
	class HeaderParser : private Parser<HeaderInfo>
	{
		public:

			static const size_t	MaxUriSize = 1024;
			static const size_t	MaxHeaderSize = HeaderFieldParser::MaxHeaderField + MaxUriSize;

			static const int	MajorVersionSupported = 1;
			static const int	MinorVersionSupported = 1;

			enum State
			{
				ST_START_REQUEST_LINE,
				ST_PARSE_METHOD,
				ST_PARSE_REQ_LINE,
				ST_HTTP,
				ST_HTTP_MAJOR_VER,
				ST_HTTP_DOT,
				ST_HTTP_MINOR_VER,
				ST_PARSE_HEADER_FIELD,
				ST_CRLF,
				ST_WS,
				ST_OWS,
				ST_DONE
			};

			HeaderParser();
			Buffer::const_iterator	operator()(const Buffer& buff, Buffer::const_iterator it);

		private:

			void	transitionState(int new_state, int next_state);
			void	changeState(int new_state);

			size_t				m_index;
			HeaderFieldParser	m_hfield_parser;
	};
}

#endif
