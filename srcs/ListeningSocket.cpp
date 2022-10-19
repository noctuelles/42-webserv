/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 19:14:03 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/19 17:49:31 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ListeningSocket.hpp"
#include <stdexcept>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <utility>

ListeningSocket::ListeningSocket()
	: SimpleSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP), _M_con(_M_default_con_size, HTTPClient())
{
	setReusableMode(true);
	setBlockingMode(false);
}

ListeningSocket::ListeningSocket(in_addr_t addr, in_port_t port)
	: SimpleSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP), _M_con(_M_default_con_size, HTTPClient())
{
	setReusableMode(true);
	setBlockingMode(false);
	bind(addr, port);
}

HTTPClient&	ListeningSocket::operator[](size_t i)
{
	return (_M_con[i]);
}

void	ListeningSocket::bind(in_addr_t addr, in_port_t port)
{
	_M_sockaddr.sin_family = AF_INET;
	_M_sockaddr.sin_port = htons(port);
	_M_sockaddr.sin_addr.s_addr = htonl(addr);

	if (::bind(_M_fd,
				(struct sockaddr*) &_M_sockaddr,
				sizeof(_M_sockaddr)) < 0)
		throw (std::runtime_error("bind"));
}

void	ListeningSocket::listen(int backlog)
{
	if (::listen(_M_fd, backlog) != 0)
		throw (std::runtime_error("listen"));
}

void	ListeningSocket::acceptConnections()
{
	int	sa_fd;

	while ((sa_fd = ::accept(_M_fd, NULL, NULL)) > 0)
	{
		if (static_cast<size_t>(sa_fd) > _M_con.size())
			_M_con.resize(_M_con.size() * 2, HTTPClient());
		_M_con[sa_fd].init(sa_fd, false);					// Init the connection (set the fd, set to non-blocking mode...)
	}
	if (sa_fd < 0)
	{
		if (!(errno == EAGAIN || errno == EWOULDBLOCK))
			throw (std::runtime_error("accept"));
	}
}

ListeningSocket::~ListeningSocket()
{}
