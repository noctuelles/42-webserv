/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/26 16:41:16 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/29 13:12:50 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "Http.hpp"
#include <exception>

namespace HTTP
{
	template <class T>
		class Parser
		{
			public:

				Parser(int initial_state) :
					m_previous_state(),
					m_current_state(initial_state),
					m_next_state(),
					m_ws_buffer(),
					m_data(),
					m_nbr_parsed(0),
					m_eat(true)
				{}

				virtual ~Parser()
				{}

				struct Exception : public std::exception
				{
					Exception(Buffer::const_iterator it) :
						m_it(it)
					{}

					Buffer::const_iterator m_it;
				};

				virtual			Buffer::const_iterator	operator()(const Buffer& buff, Buffer::const_iterator it) = 0;

				int				getState() const		{return (m_current_state);			}
				int				nextState() const		{return (m_next_state);				}
				int				previousState() const	{return (m_previous_state);		}
				T				get() const				{return (m_data);}
				size_t			getNbrCharParsed() const	{return (m_nbr_parsed);}

				static inline bool		isVChar(unsigned char ch)	{return (ch > ' ' && ch < 0x7F);	}
				static inline bool		isTknChar(unsigned char ch)	{return (toTknChar(ch) != 0);			}
				static inline bool		isWS(unsigned char ch)		{return (ch == ' ' || ch == '\t');	}
				static inline bool		isCRLF(unsigned char ch)	{return (ch == '\r' || ch == '\n');	}
				static unsigned char	toTknChar(unsigned char ch)	{return (TokenMap[ch]);}

			protected:

				virtual void	changeState(int new_state) = 0;

				int					m_previous_state, m_current_state, m_next_state;
				std::string			m_ws_buffer;
				T					m_data;
				size_t				m_nbr_parsed;
				bool				m_eat;
		};
}

#endif
