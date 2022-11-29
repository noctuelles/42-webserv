/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiPartHandler.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 15:49:40 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/29 15:19:07 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MultiPartHandler.hpp"
#include "RequestHandler.hpp"
#include "FieldParser.hpp"
#include <algorithm>
#include <iterator>
#include <iostream>

namespace HTTP
{

	MultiPartHandler::MultiPartHandler(const std::string& ressource_path, size_t content_lenght, const std::string& boundary) :
		Parser(ST_START_BOUND),
		m_hfield_parser(),
		m_boundary(boundary),
		m_boundary_parser(boundary),
		m_content_lenght(content_lenght),
		m_ressource_path(ressource_path),
		m_data_it(),
		m_ofile_handle()
	{}

	MultiPartHandler::~MultiPartHandler()
	{
		if (m_ofile_handle.is_open())
			m_ofile_handle.close();
	}

	Buffer::const_iterator	MultiPartHandler::operator()(const Buffer& buff, Buffer::const_iterator it)
	{
		if (m_current_state == ST_FILE_CONTENT)
			m_data_it = it;

		while (it != buff.end() && m_current_state != ST_DONE)
		{
			switch (m_current_state)
			{
				case ST_START_BOUND: 
				{
					try
					{
						it = m_boundary_parser(buff, it);
						m_eat = false;
						if (m_boundary_parser.getState() == BoundaryParser::ST_DONE)
							transitionState(ST_CRLF, ST_PARSE_HEADER_FIELD);
					}
					catch (const BoundaryParser::Exception& e)
					{
						throw (RequestHandler::Exception(BadRequest));
					}
				}
					break;

				case ST_PARSE_HEADER_FIELD:
					it = m_hfield_parser(buff, it);
					m_eat = false;
					if (m_hfield_parser.getState() == HeaderFieldParser::ST_DONE)
					{
						m_data.header_field = m_hfield_parser.get();
						initFileWriting();
						m_data_it = it;
						changeState(ST_FILE_CONTENT);
					}
					break;

				case ST_FILE_CONTENT:
					if (isCRLF(*it))
					{
						m_boundary_parser.reset(true);

						m_eat = false;
						writeDataBatch(m_data_it, it);
						changeState(ST_FILE_BOUND);
					}
					break;

				case ST_FILE_BOUND:
				{
					try
					{
						it = m_boundary_parser(buff, it);
						m_eat = false;
						if (m_boundary_parser.getState() == BoundaryParser::ST_DONE)
							changeState(ST_FILE_BOUND_END);
					}
					catch (const BoundaryParser::Exception& e) // failed to check the boundary here.
					{
						Buffer	bound_buff = m_boundary_parser.get();

						writeDataBatch(bound_buff.begin(), bound_buff.end());
						m_eat = false;
						it = e.m_it;
						m_data_it = it;
						changeState(ST_FILE_CONTENT);
					}
				}
					break;

				case ST_FILE_BOUND_END:
					m_eat = false;
					if (isCRLF(*it))
					{
						m_boundary_parser.reset(false);
						m_hfield_parser.reset();
						m_ofile_handle.close();
						transitionState(ST_CRLF, ST_PARSE_HEADER_FIELD);
					}
					else if (*it == '-')
					{
						m_boundary_parser.reset(false);
						m_ofile_handle.close();
						changeState(ST_FILE_BOUND_END_DASH1);
					}
					else
						throw (RequestHandler::Exception(BadRequest));
					break;

				case ST_FILE_BOUND_END_DASH1:
					if (*it != '-')
						throw (RequestHandler::Exception(BadRequest));
					else
						changeState(ST_FILE_BOUND_END_DASH2);
					break;

				case ST_FILE_BOUND_END_DASH2:
					if (*it != '-')
						throw (RequestHandler::Exception(BadRequest));
					else
						transitionState(ST_CRLF, ST_DONE);
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

		if (m_current_state == ST_FILE_CONTENT && it == buff.end())
			writeDataBatch(m_data_it, it);
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

	void	MultiPartHandler::writeDataBatch(Buffer::const_iterator begin, Buffer::const_iterator end)
	{
		std::iterator_traits<Buffer::const_iterator>::difference_type	write_size = std::distance(begin, end);

		if (write_size == 0)
			return ;
		m_ofile_handle.write(reinterpret_cast<const char *>(begin.base()), write_size);
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
