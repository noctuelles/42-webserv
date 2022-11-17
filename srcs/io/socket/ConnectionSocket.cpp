/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionSocket.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 15:51:35 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/17 15:55:29 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConnectionSocket.hpp"
#include "Utils.hpp"
#include "WebServ.hpp"

#include <ctime>

namespace ft
{
	ConnectionSocket::ConnectionSocket(int fd, const VirtServInfo::VirtServMap& virt_serv_map)
		: InternetSocket(fd),
		m_last_activity(std::time(NULL)),
		m_request_handler(virt_serv_map),
		m_state(READING),
		m_peer_sockaddr(),
		m_recv_buff(MaxRecvBufferSize),
		m_recv_bytes(0),
		m_sent_bytes(0)
	{
		this->setBlockingMode(false);
		m_len = sizeof(struct sockaddr_in);
		if (getsockname(fd, reinterpret_cast<struct sockaddr*>(&m_sockaddr), &m_len) < 0)
			throw (std::runtime_error("::getsockname"));
		if (getpeername(fd, reinterpret_cast<struct sockaddr*>(&m_peer_sockaddr), &m_len) < 0)
			throw (std::runtime_error("::getpeername"));
		m_request_handler.setConnectionBoundedSocket(m_sockaddr);
	}

	ConnectionSocket::~ConnectionSocket()
	{
		using std::clog;

		// clog << inet_ntoa(m_peer_sockaddr.sin_addr) << " - - " << '[' << utils::getRFC822NowDate() << ']' << " \"" << m_request_handler.getAbsPath() << "\" " << m_request_handler.getStatusCode() << ' ' << m_sent_bytes << '\n';
	}

	int	ConnectionSocket::recv()
	{
		_updateLastActivity();
		m_recv_bytes = ::recv(*this, m_recv_buff.data(), m_recv_buff.size(), 0);
		std::cout << m_recv_buff.data() << '\n';
		if (m_recv_bytes <= 0)
			return (DISCONNECT);
		if (m_request_handler.fetchIncomingData(m_recv_buff, m_recv_bytes) == RequestHandler::PROCESSING_RESPONSE_HEADER)
			m_state = WRITING;
		return (m_state);
	}

	int	ConnectionSocket::send()
	{
		try
		{
			if (m_request_handler.prepareOutcomingData() == RequestHandler::DONE)
				m_state = DISCONNECT;
		}
		catch(...)
		{
			std::cerr << "exception of some sort thrown\n";
			return (DISCONNECT);
		}

		RequestHandler::DataInfo	data_info = m_request_handler.getDataToSend();

		_updateLastActivity();
		if (::send(*this, data_info.first, data_info.second, 0) < 0)
			m_state = DISCONNECT;
		else
			m_sent_bytes += data_info.second;
		return (m_state);
	}

	bool	ConnectionSocket::isTimeout()
	{
		return (time(NULL) - m_last_activity >= WebServ::ConnectionTimeout);
	}
}
