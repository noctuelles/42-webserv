/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 19:14:03 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 00:05:17 by plouvel          ###   ########.fr       */
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

ListeningSocket::ListeningSocket()
	: InternetSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP), _M_con()
{
	setReusableMode(true);
	setBlockingMode(false);
}

ListeningSocket::ListeningSocket(in_addr_t addr, in_port_t port)
	: InternetSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP), _M_con()
{
	setReusableMode(true);
	setBlockingMode(false);
	bind(addr, port);
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

void	ListeningSocket::acceptConnection(EPoll& epollInstance)
{
	struct sockaddr_in	sa;
	socklen_t			slen = sizeof(struct sockaddr_in);
	int					sa_fd;

	while ((sa_fd = ::accept(_M_fd, reinterpret_cast<struct sockaddr*>(&sa), &slen)) > 0)
	{
		_M_con.push_back(HTTPClient(sa_fd, sa, slen, *this));

		HTTPClient& inserted = _M_con.back();
		inserted.setIterator((--_M_con.end()));
		epollInstance.add(sa_fd, EPOLLIN, &inserted);
	}
	if (sa_fd < 0)
	{
		if (!(errno == EAGAIN || errno == EWOULDBLOCK))
			throw (std::runtime_error("accept"));
	}
}

void	ListeningSocket::removeConnection(HTTPClient* clientPtr)
{
	/* Automatically close the file descriptor and removing the file descriptor
	 * from the interest list. */
	_M_con.erase(clientPtr->getIterator());
}

ListeningSocket::~ListeningSocket()
{}
