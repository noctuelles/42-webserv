/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 16:11:40 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/14 18:02:46 by plouvel          ###   ########.fr       */
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
				if (_state(FETCHING_REQUEST_HEADER))
				{
					if (m_parser.parse(m_recv_buff, m_recv_bytes))
					{
						// Le HOST qui va nous permettre de selectionner votre virtual server.
						// Parsing des headers GENERAL.
						// Une requete POST va peut etre avoir besoin de plus de header: ce n'est pas un header general.
						//
						_parseRequestHeaderFields();

						(this->*m_method_init_fnct[m_parser.getMethod()])();
						_setState(FETCHING_REQUEST_BODY);
					}
				}
				if (_state(FETCHING_REQUEST_BODY))
				{
					// Request body is ignored in GET... :)
					if (m_parser.getMethod() != Get)
					{
					}
				}
			}
			catch (const Exception& e)
			{
				_setState(SENDING_RESPONSE_HEADER, e.what());
			}
			return (m_state);
		}

		int	HttpRequest::send()
		{
			if (_state(SENDING_RESPONSE_HEADER))
			{
				ResponseHeader	respHeader;

				std::memcpy(m_send_buff.data(), respHeader.toCString(), respHeader.size());
			}
			else if (_state(SENDING_RESPONSE_BODY))
			{

			}
			ConnectionSocket::send();
			return (m_state);
		}
	}
}
