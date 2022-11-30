/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodSend.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:41:28 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/30 11:33:45 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileUtils.hpp"
#include "RequestHandler.hpp"
#include "StatusInfoPages.hpp"

#include <iostream>

namespace HTTP
{
	void	RequestHandler::_methodSendGet()
	{
		if (m_request_type == FILE)
		{
			m_file_handle.read(reinterpret_cast<char *>(m_data_buff.data()), m_data_buff.size());

			m_data_to_send = m_data_buff.data();
			m_data_to_send_size = m_file_handle.gcount();

			if (m_file_handle.eof())
			{
				m_file_handle.close();
				m_state = DONE;
			}
		}
		else if (m_request_type == AUTOINDEX)
		{
			m_data_to_send = m_page_to_send.data();
			m_data_to_send_size = m_page_to_send.size();
			m_state = DONE;
		}
	}

	void	RequestHandler::_methodSendPost()
	{
		m_data_to_send = NULL;
		m_data_to_send_size = 0;
		m_state = DONE;
	}

	void	RequestHandler::_methodSendDelete()
	{

	}

	void	RequestHandler::_methodSendError()
	{
		if (m_request_type == ERROR)
		{
			m_data_to_send = StatusInfoPages::get()[m_status_code].page.data();
			m_data_to_send_size = StatusInfoPages::get()[m_status_code].page.size();
		}
		else if (m_request_type == REDIRECT_ERROR)
		{
			m_data_to_send = NULL;
			m_data_to_send_size = 0;
		}
		m_state = DONE;
	}
}
