/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 19:14:03 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/12 14:53:19 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"
#include <stdexcept>
#include <sys/socket.h>
#include <string.h>

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

void	ServerSocket::accept(ClientSocket& client_sock)
{
	int	fd;

	fd = ::accept(_M_fd,
			reinterpret_cast<struct sockaddr *>(client_sock.getSocketAddrPointer()),
			client_sock.getSocketLenPointer());
	if (fd < 0)
		throw (std::runtime_error("accept"));
	client_sock.setFd(fd);
}

void	ServerSocket::allowReusable() const
{
	int	on = 1;

	if (setsockopt(_M_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on)) < 0)
		throw (std::runtime_error("setsockopt"));
}

ServerSocket::~ServerSocket()
{}
