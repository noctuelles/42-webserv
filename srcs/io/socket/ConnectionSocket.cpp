/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionSocket.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 15:51:35 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/14 21:55:02 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionSocket.hpp"
#include "WebServ.hpp"

#include <ctime>

namespace ft
{
	ConnectionSocket::ConnectionSocket(int fd)
		: InternetSocket(fd),
		m_last_activity(std::time(NULL)),
		m_state(FETCHING_REQUEST_HEADER),
		m_bounded_sockaddr(),
		m_data_to_send(NULL),
		m_data_size(0),
		m_recv_buff(MaxRecvBufferSize),
		m_send_buff(MaxSendBufferSize),
		m_recv_bytes(0)
	{
		setBlockingMode(false);
		m_len = sizeof(struct sockaddr_in);
		if (getsockname(fd, reinterpret_cast<struct sockaddr*>(&m_bounded_sockaddr), &m_len) < 0)
			throw (std::runtime_error("::getsockname"));
		if (getpeername(fd, reinterpret_cast<struct sockaddr*>(&m_sockaddr), &m_len) < 0)
			throw (std::runtime_error("::getpeername"));
	}

	ConnectionSocket::~ConnectionSocket()
	{}

	int	ConnectionSocket::recv()
	{
		_updateLastActivity();
		m_recv_bytes = ::recv(*this, m_recv_buff.data(), m_recv_buff.size(), 0);
		if (m_recv_bytes <= 0)
			m_state = DISCONNECT;
		return (m_state);
	}

	int	ConnectionSocket::send()
	{
		_updateLastActivity();
		if (::send(*this, m_data_to_send, m_send_bytes, 0) < 0)
			m_state = DISCONNECT;
		return (m_state);
	}

	bool	ConnectionSocket::isTimeout()
	{
		return (time(NULL) - m_last_activity >= WebServ::ConnectionTimeout);
	}
}
