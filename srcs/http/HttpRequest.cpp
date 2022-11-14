/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 16:11:40 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/14 17:07:00 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

namespace ft
{
	namespace http
	{
		const HttpRequest::methodInitFnct		HttpRequest::m_method_init_fnct[NbrAvailableMethod] = 
		{
			&HttpRequest::_methodInitGet,
			&HttpRequest::_methodInitPost,
			&HttpRequest::_methodInitDelete
		};

		const HttpRequest::methodHeaderFnct	HttpRequest::m_method_header_fnct[NbrAvailableMethod] = 
		{
			&HttpRequest::_methodHeaderGet,
			&HttpRequest::_methodHeaderPost,
			&HttpRequest::_methodHeaderDelete
		};

		const HttpRequest::methodSendFnct		HttpRequest::m_method_send_fnct[NbrAvailableMethod] = 
		{
			&HttpRequest::_methodSendGet,
			&HttpRequest::_methodSendPost,
			&HttpRequest::_methodSendDelete
		};

		HttpRequest::HttpRequest(int fd, const VirtServInfo::VirtServMap& virt_serv_map)
			: ConnectionSocket(fd),
			m_file_handle(),
			m_parser(),
			m_status_code(OK),
			m_virtserv_map(virt_serv_map)
		{}

		int	HttpRequest::recv()
		{
			if (ConnectionSocket::recv() == DISCONNECT)
				return (DISCONNECT);
			try
			{
				if (m_state == FETCHING_REQUEST_HEADER)
				{
				}
				if (m_state == FETCHING_REQUEST_BODY)
				{
					if (m_parser.getMethod() != Get)
					{

					}
					// Request body is ignored in GET.
					// To be use ONLY for POST request (chunked request, multipart/formdata...)
				}
			}
			catch (const RequestException& e)
			{
				m_state = SENDING_RESPONSE_HEADER;
				m_status_code = e.what();
			}

			return (m_state);
		}

		int	HttpRequest::send()
		{
			return (m_state);
		}
	}
}
