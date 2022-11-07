/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 19:14:03 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/05 01:07:31 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ListeningSocket.hpp"
#include "EPoll.hpp"
#include "WebServ.hpp"
#include <stdexcept>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <utility>

namespace ft
{
	ListeningSocket::ListeningSocket()
		: InternetSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
	{
		setReusableMode(true);
		setBlockingMode(false);
	}

	ListeningSocket::ListeningSocket(in_addr_t addr, in_port_t port)
		: InternetSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
	{
		setReusableMode(true);
		setBlockingMode(false);
		bind(addr, port);
	}

	ListeningSocket::ListeningSocket(const ListeningSocket& other)
		: InternetSocket(other)
	{}

	ListeningSocket&	ListeningSocket::operator=(const ListeningSocket& rhs)
	{
		if (this == &rhs)
			return (*this);
		InternetSocket::operator=(rhs);
		return (*this);
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

	void	ListeningSocket::listen(int backlog) const
	{
		if (::listen(m_fd, backlog) != 0)
			throw (std::runtime_error("listen"));
	}

	int	ListeningSocket::recv()
	{
		int					sa_fd;

		sa_fd = ::accept(m_fd, NULL, NULL);
		if (sa_fd < 0)
			throw (std::runtime_error("accept"));
		return (sa_fd);
	}

	int	ListeningSocket::send() {return (0);}

	bool	ListeningSocket::isTimeout()
	{
		return (false);
	}

	ListeningSocket::~ListeningSocket()
	{}
}
