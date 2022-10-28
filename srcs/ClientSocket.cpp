/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 18:34:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/28 14:42:37 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientSocket.hpp"
#include <string>
#include <unistd.h>

std::string ClientSocket::m_buffer;

ClientSocket::ClientSocket(int fd, ListeningSocket* sock)
	: InternetSocket(fd), m_iterator(), m_state(FETCHING_DATA), m_socket(sock)
{
	setBlockingMode(false);
}

ClientSocket::ClientSocket(int fd, const struct sockaddr_in& sockaddr, socklen_t slen, ListeningSocket* sock)
	: InternetSocket(fd, sockaddr, slen), m_iterator(), m_state(FETCHING_DATA), m_socket(sock)
{
	setBlockingMode(false);
}

ClientSocket::ClientSocket(const ClientSocket& other)
	: InternetSocket(other), m_iterator(other.m_iterator), m_socket(other.m_socket)
{}

ClientSocket&	ClientSocket::operator=(const ClientSocket& rhs)
{
	if (this == &rhs)
		return (*this);
	InternetSocket::operator=(rhs);
	m_iterator = rhs.m_iterator;
	m_socket = rhs.m_socket;
	return (*this);
}

void	ClientSocket::receiveAndParse()
{
	char	buffer[BufferSize];
	ssize_t	recv_bytes;

	recv_bytes = recv(m_fd, buffer, BufferSize, 0);
	if (recv_bytes < 0)
		throw (Disconnected());
	m_buffer.assign(buffer, recv_bytes);
}

ClientSocket::State	ClientSocket::getState() const
{
	return (m_state);
}

ListeningSocket*	ClientSocket::getBindedSocket()
{
	return (m_socket);
}

void	ClientSocket::setIterator(const std::list<ClientSocket>::iterator& it)
{
	m_iterator = it;
}

const std::list<ClientSocket>::iterator&	ClientSocket::getIterator() const
{
	return (m_iterator);
}

ClientSocket::~ClientSocket()
{}
