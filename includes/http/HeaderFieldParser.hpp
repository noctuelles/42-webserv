/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderFieldParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/26 16:44:44 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/26 23:07:41 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADERFIELDPARSER_HPP
# define HEADERFIELDPARSER_HPP

# include "Parser.hpp"

namespace HTTP
{
	class HeaderFieldParser : public Parser<HeaderFieldMap>
	{
		public:

			static const size_t	MaxHeaderFieldSize = 512;
			static const size_t	MaxHeaderField = 1024 * 4;

			enum State
			{
				ST_HEADER_FIELD_NAME,
				ST_HEADER_FIELD_VALUE,
				ST_VALUE,
				ST_PARAM_KEY,
				ST_PARAM_VALUE,
				ST_CRLF,
				ST_WS,
				ST_OWS,
				ST_DONE
			};

			HeaderFieldParser();
			~HeaderFieldParser();
			Buffer::const_iterator	operator()(const Buffer& buff, Buffer::const_iterator it);

		private:

			typedef void	(HeaderFieldParser::*callBackFnct)();

			void	transitionState(int new_state, int next_state, callBackFnct f = NULL);
			void	changeState(int new_state);
			void	insertField();

			HeaderField		m_buffer;
			bool			m_eat;
			callBackFnct	m_f;
	};
}

#endif
