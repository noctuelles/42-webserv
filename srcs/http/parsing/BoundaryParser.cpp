/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BoundaryParser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/28 14:41:45 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/29 16:10:12 by plouvel          ###   ########.fr       */
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
	{
	}

	Buffer::const_iterator	BoundaryParser::operator()(const Buffer& buff, Buffer::const_iterator it)
	{
		while (it != buff.end() && m_current_state != ST_DONE)
		{
			switch (m_current_state)
			{
				case ST_DASH1:
					if (*it != '-')
						throw (Exception(it));
					else
					{
						changeState(ST_DASH2);
						m_data.push_back('-');
					}
					break;

				case ST_DASH2:
					if (*it != '-')
						throw (Exception(it));
					else
					{
						changeState(ST_BOUNDARY);
						m_data.push_back('-');
					}
					break;

				case ST_BOUNDARY:
					// Two possible ending for a boundary : either a CRLF, or a --CRLF indicating the end.
					if (m_cmp_it == m_boundary.end())
					{
						m_eat = false;
						changeState(ST_DONE);
					}
					else if (*m_cmp_it != *it)
						throw (Exception(it));
					else
					{
						m_data.push_back(*it);
						m_cmp_it++;
					}
					break;

				case ST_CRLF:
					if (*it != '\r')
					{
						if (*it != '\n')
							throw (Exception(it));
						else m_data.push_back(*it);
						changeState(m_next_state);
					}
					else m_data.push_back(*it);
					break;

			}
			if (!m_eat)
				m_eat = true;
			else
				it++;
		}
		return (it);
	}

	void BoundaryParser::reset(bool crlf_prefix)
	{
		m_cmp_it = m_boundary.begin();
		if (crlf_prefix)
			transitionState(ST_CRLF, ST_DASH1);
		else
			m_current_state = ST_DASH1;
		m_data.clear();
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
