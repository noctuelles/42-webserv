/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 18:34:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/29 19:50:58 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "RequestParser.hpp"
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <iostream>

namespace ft
{
	std::string				Client::m_buffer;
	std::vector<uint8_t>	Client::m_recv_buffer(MaxBufferSize);

	Client::Client(int fd, ListeningSocket* sock)
		: InternetSocket(fd),
		m_recv_bytes(),
		m_parser(),
		m_iterator(),
		m_state(CONNECTION_ESTABLISHED),
		m_socket(sock)
	{
		setBlockingMode(false);
	}

	Client::Client(int fd, const struct sockaddr_in& sockaddr, socklen_t slen, ListeningSocket* sock)
		: InternetSocket(fd, sockaddr, slen),
		m_recv_bytes(),
		m_parser(),
		m_iterator(),
		m_state(CONNECTION_ESTABLISHED),
		m_socket(sock)
	{
		setBlockingMode(false);
	}

	Client::Client(const Client& other)
		: InternetSocket(other),
		  m_recv_bytes(other.m_recv_bytes),
		  m_parser(other.m_parser),
		  m_iterator(other.m_iterator),
		  m_state(other.m_state),
		  m_socket(other.m_socket)
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
			case CONNECTION_ESTABLISHED:
				m_state = FETCHING_REQUEST_HEADER;
				__attribute__((fallthrough));
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

				if (m_parser.parse(m_buffer) == http::RequestParser::P_DONE)
					m_state = READY_FOR_RESPONSE_BODY;
				break;
			case FETCHING_REQUEST_BODY:
				break;

			default:
				break ;
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

	const http::RequestParser::t_parse_info&	Client::getParseInfo()
	{
		return (m_parser.getInfo());
	}

	Client::~Client()
	{}
}
