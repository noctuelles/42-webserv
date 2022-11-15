/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodSend.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:41:28 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/15 14:32:37 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestHandler.hpp"
#include "StatusInfoPages.hpp"

namespace ft
{
	namespace http
	{
		void	HttpRequestHandler::_methodSendGet()
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

		void	HttpRequestHandler::_methodSendPost()
		{

		}

		void	HttpRequestHandler::_methodSendDelete()
		{

		}

		void	HttpRequestHandler::_methodSendError()
		{
			// TODO: la page d'erreur custom.
			m_data_to_send = StatusInfoPages::get()[m_status_code].page.c_str();
			m_data_to_send_size = StatusInfoPages::get()[m_status_code].page.size();
			m_state = DONE;
		}
	}
}
