/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 18:34:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/07 18:39:14 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientSocket.hpp"
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
				int ret = m_parser.parse(m_recv_buffer, static_cast<size_t>(m_recv_bytes));

#ifndef NDEBUG
				m_parser.report();
#endif
				switch (ret) // parse could throw exception.
				{
					case http::RequestParser::P_DONE:
						// Parsing is done.
						break;

					case http::RequestParser::P_DONE_ERR:
						std::cout << "woups, this is wrong.\n";
						// An error occured during parsing.
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
		return (m_state);
	}

	bool	ClientSocket::isTimeout()
	{
		return (time(NULL) - m_last_activity >= WebServ::ConnectionTimeout);
	}

	ClientSocket::~ClientSocket()
	{}
}
