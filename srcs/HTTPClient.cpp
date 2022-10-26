/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPClient.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 18:34:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 13:27:26 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPClient.hpp"

HTTPClient::HTTPClient(int fd, ListeningSocket& sock)
	: InternetSocket(fd), m_iterator(), m_socket(sock) 
{
	setBlockingMode(false);
}

HTTPClient::HTTPClient(int fd, const struct sockaddr_in& sockaddr, socklen_t slen, ListeningSocket& sock)
	: InternetSocket(fd, sockaddr, slen), m_iterator(), m_socket(sock)
{
	setBlockingMode(false);
}

ListeningSocket&	HTTPClient::getBindedSocket()
{
	return (m_socket);
}

void	HTTPClient::setIterator(const std::list<HTTPClient>::iterator& it)
{
	m_iterator = it;
}

const std::list<HTTPClient>::iterator&	HTTPClient::getIterator() const
{
	return (m_iterator);
}

HTTPClient::~HTTPClient()
{}
