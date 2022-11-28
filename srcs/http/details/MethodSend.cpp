/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodSend.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:41:28 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/22 23:06:26 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIScriptHandler.hpp"
#include "FileUtils.hpp"
#include "RequestHandler.hpp"
#include "StatusInfoPages.hpp"

#include <iostream>
#include <utility>

using std::make_pair;

namespace HTTP
{
	void	RequestHandler::_methodSendGet()
	{
		if (m_request_type == FILE)
		{
			m_file_handle.read(reinterpret_cast<char *>(m_data_buff.data()), m_data_buff.size());

			m_data_pair = make_pair(m_data_buff.data(), m_file_handle.gcount());

			if (m_file_handle.eof())
			{
				m_file_handle.close();
				m_state = DONE;
			}
		}
		else if (m_request_type == AUTOINDEX)
		{
			m_data_pair = make_pair(m_page_to_send.c_str(), m_page_to_send.size());
			m_state = DONE;
		}
		else if (m_request_type == CGI)
		{
			m_data_pair = m_cgi_handler.read();
			if (m_data_pair.second == 0) 
				_setState(DONE);
		}
	}

	void	RequestHandler::_methodSendPost()
	{
		if (m_request_type == CGI)
		{
			m_data_pair = m_cgi_handler.read();
			if (m_data_pair.second == 0)
				_setState(DONE);
		}
	}

	void	RequestHandler::_methodSendDelete()
	{

	}

	void	RequestHandler::_methodSendError()
	{
		// TODO: la page d'erreur custom.
		m_data_pair = make_pair(StatusInfoPages::get()[m_status_code].page.c_str(), StatusInfoPages::get()[m_status_code].page.size() );
		m_state = DONE;
	}
}
