/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderFieldParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/26 16:54:42 by plouvel           #+#    #+#             */
/*   Updated: 2022/12/04 23:14:22 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HeaderFieldParser.hpp"
#include "RequestHandler.hpp"
#include <cassert>

#define CALL_MEMBER_FN(ptrToMember)  ((this)->*(ptrToMember))

namespace HTTP
{
	HeaderFieldParser::HeaderFieldParser() :
		Parser(ST_HEADER_FIELD_NAME),
		m_buffer(),
		m_eat(true),
		m_f(NULL)
	{
		m_buffer.first.reserve(90);
		m_buffer.second.reserve(200);
	}

	HeaderFieldParser::~HeaderFieldParser()
	{}

	Buffer::const_iterator	HeaderFieldParser::operator()(const Buffer& buff, Buffer::const_iterator it)
	{
		unsigned char	ch = 0;

		while (it != buff.end() && m_current_state != ST_DONE)
		{
			ch = *it;
			switch (m_current_state)
			{
				case ST_HEADER_FIELD_NAME:
				{
					unsigned char	transformed_ch = toTknChar(ch);

					if (!transformed_ch) // ch is not a token char.
					{
						switch (ch)
						{
							case ':':
								// Empty field name, bad request.
								if (m_buffer.first.empty())
									throw (RequestHandler::Exception(BadRequest));
								transitionState(ST_OWS, ST_HEADER_FIELD_VALUE);
							break;

							case '\r':
							case '\n':
								m_eat = false;
								if (m_buffer.first.empty())
									transitionState(ST_CRLF, ST_DONE);
								else
								{
									m_buffer.first.clear();
									transitionState(ST_CRLF, ST_HEADER_FIELD_NAME);
								}
							break;

							default: // WS is not allowed in field_name (trailing and leading).
								throw (RequestHandler::Exception(BadRequest));
						}
					}
					else
					{
						if (m_buffer.first.size() >= MaxHeaderFieldSize)
							throw (RequestHandler::Exception(BadRequest));
						m_buffer.first.push_back(transformed_ch);
					}
					break;
				}

				case ST_HEADER_FIELD_VALUE:
				{
					if (!isVChar(ch))
					{
						switch (ch)
						{
							case '\r':
							case '\n':
								m_eat = false;
								transitionState(ST_CRLF, ST_HEADER_FIELD_NAME, &HeaderFieldParser::insertField);
							break;

							case ' ':
							case '\t':
								m_eat = false;
								transitionState(ST_WS, ST_HEADER_FIELD_VALUE);
							break;

							default:
								;
						}
					}
					else
					{
						if (m_buffer.second.size() >= MaxHeaderFieldSize)
							throw (RequestHandler::Exception(BadRequest));
						m_buffer.second.push_back(ch);
					}

					break;
				}

				case ST_CRLF:
					if (ch != '\r')
					{
						if (ch != '\n')
							throw (RequestHandler::Exception(BadRequest));
						if (m_f)
							CALL_MEMBER_FN(m_f)();
						changeState(m_next_state);
					}
					break;

				case ST_WS:
					if (!isWS(ch))
					{
						if (!isCRLF(ch))
							m_buffer.second.append(m_ws_buffer.begin(), m_ws_buffer.end());
						m_ws_buffer.clear();
						m_eat = false;
						changeState(m_next_state);
					}
					else
						m_ws_buffer.push_back(ch);
				break;

				case ST_OWS:
					if (!isWS(ch))
					{
						m_eat = false;
						changeState(m_next_state);
					}
				break;

				default:
					;
			}
			if (!m_eat)
				m_eat = true;
			else
			{
				it++;
				if (++m_nbr_parsed >= MaxHeaderField)
					throw (RequestHandler::Exception(BadRequest));
			}
		}
		return (it);
	}

	void HeaderFieldParser::reset()
	{
		m_current_state = ST_HEADER_FIELD_NAME;
		m_eat = true;
		m_f = NULL;
		m_data.clear();
		m_buffer.first.clear();
		m_buffer.second.clear();
	}

	void	HeaderFieldParser::transitionState(int new_state, int next_state, callBackFnct f)
	{
		m_previous_state = m_current_state;
		changeState(new_state);
		m_next_state = next_state;
		m_f = f;
	}

	void	HeaderFieldParser::changeState(int new_state)
	{
		m_previous_state = m_current_state;
		m_current_state = new_state;
	}

	void	HeaderFieldParser::insertField()
	{
		std::pair<HeaderFieldMap::iterator, bool>	ret = m_data.insert(m_buffer);

		if (!ret.second)
		{
			/* https://www.rfc-editor.org/rfc/rfc7230.html#section-5.4 */
			if (ret.first->first == Field::Host().str())
				throw (RequestHandler::Exception(BadRequest));
			/* https://httpwg.org/specs/rfc9110.html#fields.order */
			ret.first->second
				.append(", ")
				.append(m_buffer.second);
		}
		m_buffer.first.clear();
		m_buffer.second.clear();
	}
}
