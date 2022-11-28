/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BoundaryParser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/28 14:41:45 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/28 15:14:38 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BoundaryParser.hpp"
#include "RequestHandler.hpp"

namespace HTTP
{
	BoundaryParser::BoundaryParser(const std::string& boundary) :
		Parser(ST_DASH1),
		m_boundary(boundary),
		m_cmp_it(m_boundary.begin())
	{}

	Buffer::const_iterator	BoundaryParser::operator()(const Buffer& buff, Buffer::const_iterator it)
	{
		while (it != buff.end() && m_current_state < ST_DONE_CRLF)
		{
			switch (m_current_state)
			{
				case ST_DASH1:
					if (*it != '-')
						throw (RequestHandler::Exception(BadRequest));
					else
						changeState(ST_DASH2);
					break;

				case ST_DASH2:
					if (*it != '-')
						throw (RequestHandler::Exception(BadRequest));
					else
						changeState(ST_BOUNDARY);
					break;

				case ST_BOUNDARY:
					// Two possible ending for a boundary : either a CRLF, or a --CRLF indicating the end.
					if (m_cmp_it == m_boundary.end())
					{
						m_eat = false;
						changeState(ST_ENDING);
					}
					else if (*m_cmp_it != *it)
						throw (RequestHandler::Exception(BadRequest));
					else
					{
						m_data.push_back(*it);
						m_cmp_it++;
					}
					break;

				case ST_ENDING:
					if (isCRLF(*it))
					{
						m_eat = false;
						transitionState(ST_CRLF, ST_DONE_CRLF);
					}
					else if (*it == '-')
						changeState(ST_ENDING_DASH);
					else
						throw (RequestHandler::Exception(BadRequest));
					break;

				case ST_ENDING_DASH:
					if (*it != '-')
						throw (RequestHandler::Exception(BadRequest));
					else
						changeState(ST_DONE_DASH);

				case ST_CRLF:
					if (*it != '\r')
					{
						if (*it != '\n')
							throw (RequestHandler::Exception(BadRequest));
						changeState(m_next_state);
					}
					break;
			}
			if (!m_eat)
				m_eat = true;
			else
				it++;
		}
		return (it);
	}

	void	BoundaryParser::transitionState(int new_state, int next_state)
	{
		m_previous_state = m_current_state;
		changeState(new_state);
		m_next_state = next_state;
	}

	void	BoundaryParser::changeState(int new_state)
	{
		m_previous_state = m_current_state;
		m_current_state = new_state;
	}
}
