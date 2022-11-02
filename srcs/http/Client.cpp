/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 18:34:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/02 16:39:23 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "HTTP.hpp"
#include "RequestParser.hpp"
#include "WebServ.hpp"
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
	std::vector<uint8_t>	Client::m_recv_buffer(MaxRecvBufferSize);
	std::vector<uint8_t>	Client::m_send_buffer(MaxSendBufferSize);

	Client::Client(int fd)
		: InternetSocket(fd),
		m_recv_bytes(),
		m_sent_bytes(),
		m_parser(),
		m_state(FETCHING_REQUEST_HEADER),
		m_status_code(http::OK),
		m_file_handle()
	{
		setBlockingMode(false);
	}

	Client::Client(int fd, const struct sockaddr_in& sockaddr, socklen_t slen)
		: InternetSocket(fd, sockaddr, slen),
		m_recv_bytes(),
		m_sent_bytes(),
		m_parser(),
		m_state(FETCHING_REQUEST_HEADER),
		m_status_code(http::OK),
		m_file_handle()
	{
		setBlockingMode(false);
	}

	Client::Client(const Client& other)
		: InternetSocket(other),
		  m_recv_bytes(other.m_recv_bytes),
		  m_parser(other.m_parser),
		  m_state(other.m_state),
		m_status_code(other.m_status_code)
	{}

	Client&	Client::operator=(const Client& rhs)
	{
		if (this == &rhs)
			return (*this);
		InternetSocket::operator=(rhs);
		// to be completed
		return (*this);
	}

	int	Client::recv()
	{
		m_recv_bytes = ::recv(*this, m_recv_buffer.data(), m_recv_buffer.size(), 0); // noexcept
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
				switch (m_parser.parse(m_recv_buffer, m_recv_bytes)) // parse could throw exception.
				{
#ifndef NDEBUG
					m_parser.report();
#endif
					case http::RequestParser::P_DONE:
						// Parsing is done.
						break;

					case http::RequestParser::P_DONE_ERR:
						// An error occured during parsing.
						break;

					default:
						// The parser is still.. parsing.
						;
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

	int	Client::send()
	{
		return (m_state);
	}

	Client::~Client()
	{}
}
