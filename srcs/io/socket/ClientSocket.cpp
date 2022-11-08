/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 18:34:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/08 15:43:43 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientSocket.hpp"
#include "ResponseHeader.hpp"
#include "Utils.hpp"
#include "WebServ.hpp"
#include "HTTP.hpp"
#include "RequestParser.hpp"
#include <ios>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <sstream>

#ifndef NDEBUG
# include "DebugColors.h"
#endif

namespace ft
{
	std::vector<uint8_t>	ClientSocket::m_recv_buffer(MaxRecvBufferSize);
	std::vector<uint8_t>	ClientSocket::m_send_buffer(MaxSendBufferSize);

	ClientSocket::ClientSocket(int fd, const std::vector<http::StatusInfo>& stat_info)
		: InternetSocket(fd),
		m_recv_bytes(0),
		m_sent_bytes(0),
		m_parser(),
		m_last_activity(time(NULL)),
		m_state(FETCHING_REQUEST_HEADER),
		m_status_code(http::OK),
		m_file_handle(),
		m_stat_info(stat_info)
	{
		setBlockingMode(false);
		this->m_len = sizeof(struct sockaddr_in);
		if (getsockname(fd, reinterpret_cast<struct sockaddr*>(&this->m_sockaddr), &m_len) < 0)
			throw (std::runtime_error("getsockname"));
	}

	ClientSocket::ClientSocket(const ClientSocket& other)
		: InternetSocket(other),
		  m_recv_bytes(other.m_recv_bytes),
		  m_parser(other.m_parser),
		  m_state(other.m_state),
		  m_status_code(other.m_status_code),
		  m_stat_info(other.m_stat_info)
	{}

	ClientSocket&	ClientSocket::operator=(const ClientSocket& rhs)
	{
		if (this == &rhs)
			return (*this);
		InternetSocket::operator=(rhs);
		// to be completed
		return (*this);
	}

	int	ClientSocket::recv()
	{
		m_last_activity = time(NULL);
		m_recv_bytes = ::recv(*this, m_recv_buffer.data(), m_recv_buffer.size(), 0); // noexcept
		std::cout << "\t## " << UYEL << "Readed " << m_recv_bytes << " bytes" << CRST << " ##\n";

		if (m_recv_bytes <= 0)
			return (DISCONNECT);
#ifndef NDEBUG
		else
		{
			std::cout << "\t## " << UYEL << "Raw Data from the socket" << CRST << " ##\n";
			write(STDOUT_FILENO, m_recv_buffer.data(), m_recv_bytes);
			std::cout << "\n\t## " << UYEL << "End Raw Data" << CRST << " ##\n";
		}
#endif
		// When done, the socket should be registered to EPOLLOUT.
		switch (m_state)
		{
			case FETCHING_REQUEST_HEADER:
			{
				int ret = 0;

				try
				{ ret = m_parser.parse(m_recv_buffer, static_cast<size_t>(m_recv_bytes)); }
				catch (const std::bad_alloc& e)
				{ return (DISCONNECT); }

#ifndef NDEBUG
				m_parser.report();
#endif
				switch (ret) // parse could throw exception.
				{
					case http::RequestParser::P_DONE:
						(this->*m_method_fnct[m_parser.getMethod()])();
						break;

					case http::RequestParser::P_DONE_ERR:
						m_state = SENDING_RESPONSE_HEADER;
						m_status_code = m_parser.getErrorCode();

						std::cout << "woups, this is wrong.\n";
						break;

					default: break;
				}
				break;
			}
			case FETCHING_REQUEST_BODY:
				// To do: chunked request. No parsing is involved otherwise.
				break;
			default:
				;
		}
		return (m_state);
	}

	int	ClientSocket::send()
	{
		m_last_activity = time(NULL);
		switch (m_state)
		{
			case SENDING_RESPONSE_HEADER:
			{
				http::ResponseHeader	respHeader(m_stat_info[m_status_code].phrase);

				// These two headers are mandatory in every HTTP/1.1 request.
				respHeader.addField(http::Field::Server(), WebServ::Version);
				respHeader.addField(http::Field::Date(), utils::getRFC822FormattedDate());

				// Here are detail...

				if (::send(*this, respHeader.toCString(), respHeader.size(), 0) < 0)
					m_state = DISCONNECT;
				else
					m_state = SENDING_RESPONSE_BODY;
			}
				break;
			case SENDING_RESPONSE_BODY:
				if (m_status_code != http::OK)
				{
					::send(*this, m_stat_info[m_status_code].page.second.c_str(), m_stat_info[m_status_code].page.first, 0);
					m_state = DISCONNECT;
				}
				else
				{
					// Sending with get !
				}
				break;
			default:
				;
		}
		return (m_state);
	}

	bool	ClientSocket::isTimeout()
	{
		return (time(NULL) - m_last_activity >= WebServ::ConnectionTimeout);
	}

	ClientSocket::~ClientSocket()
	{}

	void	_methodGet()
	{
	}
}
