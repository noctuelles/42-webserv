/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiPartHandler.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 15:49:40 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/27 16:08:35 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MultiPartHandler.hpp"
#include "RequestHandler.hpp"
#include "FieldParser.hpp"
#include <iterator>

namespace HTTP
{

	MultiPartHandler::MultiPartHandler(const std::string& ressource_path, size_t content_lenght, const std::string& boundary) :
		Parser(ST_CHECK_BOUND_DASH1),
		m_hfield_parser(),
		m_content_lenght(content_lenght),
		m_ressource_path(ressource_path),
		m_boundary(boundary),
		cmp_it(m_boundary.begin()),
		m_ofile_handle()
	{}

	Buffer::const_iterator	MultiPartHandler::operator()(const Buffer& buff, Buffer::const_iterator it)
	{
		while (it != buff.end() && m_current_state != ST_DONE)
		{
			switch (m_current_state)
			{
				case ST_CHECK_BOUND_DASH1:
					if (*it != '-')
						throw (RequestHandler::Exception(BadRequest));
					else
						changeState(ST_CHECK_BOUND_DASH2);
					break;
				case ST_CHECK_BOUND_DASH2:
					if (*it != '-')
						throw (RequestHandler::Exception(BadRequest));
					else
						changeState(ST_CHECK_BOUND);
					break;
				case ST_CHECK_BOUND:
					if (cmp_it == m_boundary.end())
					{
						if (!isCRLF(*it)) // Boundary doesn't match.
							throw (RequestHandler::Exception(BadRequest));
						m_eat = false;
						transitionState(ST_CRLF, ST_PARSE_HEADER_FIELD);
					}
					else if(*it != *cmp_it)
						throw (RequestHandler::Exception(BadRequest));
					else
						cmp_it++;
					break;

				case ST_PARSE_HEADER_FIELD:
					it = m_hfield_parser(buff, it);
					if (m_hfield_parser.getState() == HeaderFieldParser::ST_DONE)
					{
						m_data.header_field = m_hfield_parser.get();
						m_eat = false;
						initFileWriting();
						changeState(ST_FILE_ENTRY);
					}
					break;

				case ST_FILE_ENTRY:
					m_eat = false;
					m_start_it = it;
					changeState(ST_FILE_CONTENT);
					break;

				case ST_FILE_CONTENT:
					break;

				case ST_CRLF:
					if (*it != '\r')
					{
						if (*it != '\n')
							throw (RequestHandler::Exception(BadRequest));
						changeState(m_next_state);
					}
				break;

				default:
					;
			}
			if (!m_eat)
				m_eat = true;
			else
				it++;
		}

		if (m_current_state == ST_FILE_CONTENT)
		{

		}
		return (it);
	}

	void	MultiPartHandler::initFileWriting()
	{
		const HeaderFieldMap::const_iterator	content_disp = m_data.header_field.find(Field::ContentDisposition());

		if (content_disp == m_data.header_field.end())
			throw (RequestHandler::Exception(BadRequest));

		ContentInfo									content_info = FieldParser()(content_disp->second);
		const map<string, string>::const_iterator	filename = content_info.param.find("filename");

		if (content_info.value != "form-data")
			throw (RequestHandler::Exception(BadRequest));
		if (filename == content_info.param.end())
			throw (RequestHandler::Exception(BadRequest));
		else if (filename->second.length() == 0)
			throw (RequestHandler::Exception(BadRequest));

		m_ofile_handle.open(std::string(m_ressource_path).append(filename->second).c_str(), std::ios::out | std::ios::binary);
		if (!m_ofile_handle.is_open())
			throw (RequestHandler::Exception(InternalServerError));
	}

	void	MultiPartHandler::writeDataBatch(Buffer::const_iterator end_it)
	{
		m_ofile_handle.write(reinterpret_cast<const char *>(m_start_it.base()), std::distance(m_start_it, end_it));
	}

	void	MultiPartHandler::transitionState(int new_state, int next_state)
	{
		m_previous_state = m_current_state;
		changeState(new_state);
		m_next_state = next_state;
	}

	void	MultiPartHandler::changeState(int new_state)
	{
		m_previous_state = m_current_state;
		m_current_state = new_state;
	}
}
