/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 19:14:03 by plouvel           #+#    #+#             */
/*   Updated: 2022/12/02 15:47:41 by tpouget          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ListeningSocket.hpp"
#include "EPoll.hpp"
#include "WebServ.hpp"
#include "Log.hpp"
#include <stdexcept>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <utility>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>

namespace IO
{
	ListeningSocket::ListeningSocket()
		: InternetSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
	{
		setReusableMode(true);
		setBlockingMode(false);
		setCloseOnExecMode(true);
	}

	ListeningSocket::ListeningSocket(in_addr_t addr, in_port_t port)
		: InternetSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
	{
		setReusableMode(true);
		setBlockingMode(false);
		setCloseOnExecMode(true);
		bind(addr, port);
		listen(WebServ::MaxPendingConnection);
	}

	ListeningSocket::ListeningSocket(sockaddr_in sockaddr)
		: InternetSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
	{
		m_sockaddr = sockaddr;
		setReusableMode(true);
		setBlockingMode(false);
		setCloseOnExecMode(true);
		bind(sockaddr);
		listen(WebServ::MaxPendingConnection);
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
		sockaddr_in	sockaddr;
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_port = htons(port);
		sockaddr.sin_addr.s_addr = htonl(addr);

		bind(sockaddr);
	}

	void	ListeningSocket::bind(sockaddr_in)
	{
		if (::bind(m_fd, (struct sockaddr*) &m_sockaddr, sizeof(m_sockaddr)) < 0)
		{
			if (errno != EADDRINUSE)
			{
				stringstream ss;
				ss << "Bad address in after listen directive: ";
				ss << inet_ntoa(m_sockaddr.sin_addr);
				throw VirtServInfo::ConfigFileError(ss.str().c_str());
			}
			else
				Log().get(WARNING) << "Attempt to bind socket "<< m_fd
					<< " on " << inet_ntoa(m_sockaddr.sin_addr) << ":" << htons(m_sockaddr.sin_port)
					<<". host:port already bound to another socket\n";
		}
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
