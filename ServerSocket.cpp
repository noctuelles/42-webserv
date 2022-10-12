/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 19:14:03 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/12 20:23:48 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"
#include <stdexcept>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <utility>

ServerSocket::ServerSocket()
	: SimpleSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
{
}

void	ServerSocket::bind(in_addr_t addr, in_port_t port)
{
	_M_sockaddr.sin_family = AF_INET;
	_M_sockaddr.sin_port = htons(port);
	_M_sockaddr.sin_addr.s_addr = htonl(addr);

	if (::bind(_M_fd,
				(struct sockaddr*) &_M_sockaddr,
				sizeof(_M_sockaddr)) < 0)
		throw (std::runtime_error("bind"));
}

void	ServerSocket::listen(int backlog)
{
	if (::listen(_M_fd, backlog) != 0)
		throw (std::runtime_error("listen"));
}

bool	ServerSocket::accept(InternetSocket& csock)
{
	int	sa_fd;

	sa_fd = ::accept(_M_fd,
			reinterpret_cast<struct sockaddr *>(&csock.getSockAddr()),
			&csock.getSockLen());
	if (sa_fd < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return (false);
		else
			throw (std::runtime_error("accept"));
	}
	csock.setFd(sa_fd);
	return (true);
}

ServerSocket::~ServerSocket()
{}
