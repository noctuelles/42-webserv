/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 19:14:03 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/28 16:12:22 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ListeningSocket.hpp"
#include "EPoll.hpp"
#include <stdexcept>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <utility>

namespace ft
{
	ListeningSocket::ListeningSocket()
		: InternetSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP), m_con()
	{
		setReusableMode(true);
		setBlockingMode(false);
	}

	ListeningSocket::ListeningSocket(const ListeningSocket& other)
		: InternetSocket(other), m_con(other.m_con)
	{}

	ListeningSocket&	ListeningSocket::operator=(const ListeningSocket& rhs)
	{
		if (this == &rhs)
			return (*this);
		InternetSocket::operator=(rhs);
		m_con = rhs.m_con;
		return (*this);
	}

	ListeningSocket::ListeningSocket(in_addr_t addr, in_port_t port)
		: InternetSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP), m_con()
	{
		setReusableMode(true);
		setBlockingMode(false);
		bind(addr, port);
	}

	void	ListeningSocket::bind(in_addr_t addr, in_port_t port)
	{
		m_sockaddr.sin_family = AF_INET;
		m_sockaddr.sin_port = htons(port);
		m_sockaddr.sin_addr.s_addr = htonl(addr);

		if (::bind(m_fd,
					(struct sockaddr*) &m_sockaddr,
					sizeof(m_sockaddr)) < 0)
			throw (std::runtime_error("bind"));
	}

	void	ListeningSocket::listen(int backlog)
	{
		if (::listen(m_fd, backlog) != 0)
			throw (std::runtime_error("listen"));
	}

	void	ListeningSocket::acceptConnection(EPoll& epollInstance)
	{
		struct sockaddr_in	sa;
		socklen_t			slen = sizeof(struct sockaddr_in);
		int					sa_fd;

		while ((sa_fd = ::accept(m_fd, reinterpret_cast<struct sockaddr*>(&sa), &slen)) > 0)
		{
			m_con.push_back(ClientSocket(sa_fd, sa, slen, this));

			ClientSocket& inserted = m_con.back();
			inserted.setIterator((--m_con.end()));
			epollInstance.add(sa_fd, EPOLLIN | EPOLLRDHUP, &inserted);
		}
		if (sa_fd < 0)
		{
			if (!(errno == EAGAIN || errno == EWOULDBLOCK))
				throw (std::runtime_error("accept"));
		}
	}

	void	ListeningSocket::removeConnection(ClientSocket* clientPtr)
	{
		/* Automatically close the file descriptor and removing the file descriptor
		 * from the interest list. */
		m_con.erase(clientPtr->getIterator());
	}

	ListeningSocket::~ListeningSocket()
	{}
}
