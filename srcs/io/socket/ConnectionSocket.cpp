/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionSocket.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 15:51:35 by plouvel           #+#    #+#             */
/*   Updated: 2022/12/05 16:35:43 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionSocket.hpp"
#include "Utils.hpp"
#include "WebServ.hpp"
#include "Log.hpp"

#include <cctype>
#include <ctime>

namespace IO
{
	ConnectionSocket::ConnectionSocket(int fd, const VirtServInfo::VirtServMap& virt_serv_map)
		: InternetSocket(fd),
		m_last_activity(std::time(NULL)),
		m_request_handler(virt_serv_map),
		m_state(READING),
		m_next_state(),
		m_peer_sockaddr(),
		m_to_send(),
		m_recv_buff(MaxRecvBufferSize),
		m_recv_bytes(0),
		m_sent_bytes(0)
	{
		this->setBlockingMode(false);
		this->setCloseOnExecMode(true);
		m_len = sizeof(struct sockaddr_in);
		if (getsockname(fd, reinterpret_cast<struct sockaddr*>(&m_sockaddr), &m_len) < 0)
			throw (std::runtime_error("::getsockname"));
		if (getpeername(fd, reinterpret_cast<struct sockaddr*>(&m_peer_sockaddr), &m_len) < 0)
			throw (std::runtime_error("::getpeername"));
		m_request_handler.setConnPeerSock(m_peer_sockaddr);
		m_request_handler.setConnBoundedSock(m_sockaddr);
	}

	const struct sockaddr_in&	ConnectionSocket::getPeerSock() const
	{
		return (m_peer_sockaddr);
	}

	ConnectionSocket::~ConnectionSocket()
	{
	}

	int	ConnectionSocket::recv()
	{
		_updateLastActivity();
		m_recv_buff.resize(MaxRecvBufferSize);
		m_recv_bytes = ::recv(*this, m_recv_buff.data(), m_recv_buff.size(), 0);
		if (m_recv_bytes <= 0)
			return (DISCONNECT);
		m_recv_buff.resize(m_recv_bytes);
		if (m_request_handler.fetchIncomingData(m_recv_buff) == RequestHandler::PROCESSING_RESPONSE_HEADER)
			m_state = FETCH_SEND_DATA;
		return (m_state);
	}

	int	ConnectionSocket::send()
	{
		if (m_state == FETCH_SEND_DATA)
		{
			try
			{
				m_state = SENDING;
				if (m_request_handler.prepareOutcomingData() == RequestHandler::DONE)
					m_next_state = DISCONNECT;
				else
					m_next_state = FETCH_SEND_DATA;
				m_to_send = m_request_handler.getDataToSend();
			}
			catch (...)
			{
				return (DISCONNECT);
			}
		}
		if (m_state == SENDING)
		{
			ssize_t	sent_bytes = 0;
			size_t	data_remaining = m_to_send.second - m_sent_bytes;

			_updateLastActivity();
			sent_bytes = ::send(*this, static_cast<const uint8_t*>(m_to_send.first) + m_sent_bytes, data_remaining, MSG_NOSIGNAL);
			if (sent_bytes < 0)
				return (DISCONNECT);
			else
			{
				m_sent_bytes += static_cast<size_t>(sent_bytes); // safe: sent_bytes is a positive integer.
				if (m_sent_bytes == m_to_send.second) // eq. to: if (data_remaining == sent_bytes)
				{
					m_sent_bytes = 0;
					m_state = m_next_state;
				}
			}
		}
		return (m_state);
	}

	bool	ConnectionSocket::isTimeout()
	{
		return (time(NULL) - m_last_activity >= WebServ::ConnectionTimeout);
	}
}
