/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodSend.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:41:28 by plouvel           #+#    #+#             */
/*   Updated: 2022/12/01 18:05:30 by plouvel          ###   ########.fr       */
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
		switch (m_request_type)
		{
			case FILE:
				m_file_handle.read(reinterpret_cast<char *>(m_data_buff.data()), m_data_buff.size());

				m_data.first = m_data_buff.data();
				m_data.second = m_file_handle.gcount();

				if (m_file_handle.eof())
				{
					m_file_handle.close();
					m_state = DONE;
				}
				break;

			case AUTOINDEX:
				m_data.first = m_page_to_send.data();
				m_data.second = m_page_to_send.size();
				m_state = DONE;
				break;

			case CGI:
			{
				const CGIScriptHandler::CGIScriptInfo&	script_info = m_cgi_handler.getScriptInfo();

				m_data.first = script_info.body.first.base();
				m_data.second = script_info.content_length;
				m_state = DONE;
			}
				break;
			default:
				;
		}
	}

	void	RequestHandler::_methodSendPost()
	{
		switch (m_request_type)
		{
			case FILE_UPLOAD:
				m_data.first = m_upload_page.data();
				m_data.second = m_upload_page.size();
				m_state = DONE;
				break;
			case CGI:
			{
				const CGIScriptHandler::CGIScriptInfo&	script_info = m_cgi_handler.getScriptInfo();

				m_data.first = script_info.body.first.base();
				m_data.second = script_info.content_length;
				m_state = DONE;
			}
				break;
			default:
				;
		}
		if (m_request_type == FILE_UPLOAD)
		{

		}
	}

	void	RequestHandler::_methodSendDelete()
	{

	}

	void	RequestHandler::_methodSendError()
	{
		if (m_request_type == ERROR)
		{
			m_data.first = StatusInfoPages::get()[m_status_code].page.data();
			m_data.second = StatusInfoPages::get()[m_status_code].page.size();
		}
		else
		{
			m_data.first = NULL;
			m_data.second = 0;
		}
		m_state = DONE;
	}
}
