/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 18:34:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/31 11:05:18 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "HTTP.hpp"
#include "RequestParser.hpp"
#include "ResponseHeader.hpp"
#include "WebServ.hpp"
#include <ios>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <sstream>

namespace ft
{
	std::string				Client::m_buffer;
	std::vector<uint8_t>	Client::m_recv_buffer(MaxBufferSize);

	Client::Client(int fd, ListeningSocket* sock)
		: InternetSocket(fd),
		m_recv_bytes(),
		m_parser(),
		m_iterator(),
		m_state(FETCHING_REQUEST_HEADER),
		m_socket(sock),
		m_status_code(http::OK),
		m_file_handle()
	{
		setBlockingMode(false);
	}

	Client::Client(int fd, const struct sockaddr_in& sockaddr, socklen_t slen, ListeningSocket* sock)
		: InternetSocket(fd, sockaddr, slen),
		m_recv_bytes(),
		m_parser(),
		m_iterator(),
		m_state(FETCHING_REQUEST_HEADER),
		m_socket(sock),
		m_status_code(http::OK),
		m_file_handle()
	{
		setBlockingMode(false);
	}

	Client::Client(const Client& other)
		: InternetSocket(other),
		  m_recv_bytes(other.m_recv_bytes),
		  m_parser(other.m_parser),
		  m_iterator(other.m_iterator),
		  m_state(other.m_state),
		  m_socket(other.m_socket),
		m_status_code(other.m_status_code)
	{}

	Client&	Client::operator=(const Client& rhs)
	{
		if (this == &rhs)
			return (*this);
		InternetSocket::operator=(rhs);
		m_iterator = rhs.m_iterator;
		m_socket = rhs.m_socket;
		return (*this);
	}

	void	Client::receive()
	{
		switch (m_state)
		{
			case FETCHING_REQUEST_BODY:
			case FETCHING_REQUEST_HEADER:
				m_recv_bytes = recv(m_fd, m_recv_buffer.data(), m_recv_buffer.size(), 0);
				if (m_recv_bytes < 0)
					; // This is an error !
				else if (m_recv_bytes == 0)
					; // Client Disconnect
				else
					std::cout << "Received data :\n" << m_recv_buffer.data() << '\n';
			default:
				return ;
		}
	}

	Client::State	Client::proceed()
	{
		switch (m_state)
		{
			case FETCHING_REQUEST_HEADER:
				m_buffer.assign(m_recv_buffer.begin(), m_recv_buffer.end());
				m_buffer.resize(m_recv_bytes);

				switch (m_parser.parse(m_buffer))
				{
					case http::RequestParser::P_DONE:
						m_state = SENDING_RESPONSE_HEADER;
						if (m_parser.getMethod() == http::Get)
						{
							m_file_handle.open(m_parser.getRequestLine().substr(1).c_str());
							if (m_file_handle.is_open() == false)
							{
								std::cerr << "Cannot open file " << m_parser.getRequestLine() << '\n';
								m_state = SENDING_RESPONSE_ERROR_HEADER;
								m_status_code = http::NotFound;
							}
						}

						break;

					case http::RequestParser::P_DONE_ERR:
						m_state = SENDING_RESPONSE_ERROR_HEADER;
						m_status_code = m_parser.getErrorCode();
						break;

					default:
						break;
				}

				break;
			case FETCHING_REQUEST_BODY:
				break;

			default:
				break ;
		}
		return (m_state);
	}

	Client::State	Client::send(const WebServ& servInstance)
	{
		const http::StatusInfo	statusInfo = servInstance.getStatusCodeInfo(m_status_code);

		switch (m_state)
		{
			case SENDING_RESPONSE_HEADER:
			{
				http::ResponseHeader	respHeader(m_parser.getMajorVersion(), m_parser.getMinorVersion(), statusInfo.phrase);

				respHeader.addField("Server", "webserv/0.1");
				respHeader.addField("Content-Type", "application/octet-stream");
				respHeader.addField("Connection", "closed");

				m_state = SENDING_RESPONSE_BODY;
				::send(*this, respHeader.toString().c_str(), respHeader.toString().size(), 0);
				break;
			}
			case SENDING_RESPONSE_BODY:
			{
				char buffer[MaxBufferSize];

				std::streamsize i = m_file_handle.read(buffer, MaxBufferSize).gcount();

				::send(*this, buffer, i, 0);
				if (i < (long) MaxBufferSize)
					m_state = DONE;
				break;
			}

			case SENDING_RESPONSE_ERROR_HEADER:
			{
				http::ResponseHeader	respHeader(m_parser.getMajorVersion(), m_parser.getMinorVersion(), statusInfo.phrase);

				respHeader.addField("Server", "webserv/0.1");
				respHeader.addField("Content-Type", "text/html");
				respHeader.addField("Content-Lenght", utils::integralToString(statusInfo.page.first));
				respHeader.addField("Connection", "closed");

				m_state = SENDING_RESPONSE_ERROR_BODY;
				::send(*this, respHeader.toString().c_str(), respHeader.toString().size(), 0);
				break;
			}
			case SENDING_RESPONSE_ERROR_BODY:
				m_state = DONE;
				::send(*this, statusInfo.page.second.c_str(), statusInfo.page.second.size(), 0);
				break;
			default:
				;
		}
		return (m_state);
	}

	ListeningSocket*	Client::getBindedSocket()
	{
		return (m_socket);
	}

	void	Client::setIterator(const std::list<Client>::iterator& it)
	{
		m_iterator = it;
	}

	const std::list<Client>::iterator&	Client::getIterator() const
	{
		return (m_iterator);
	}

	Client::~Client()
	{}
}
