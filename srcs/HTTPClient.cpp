/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPClient.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 18:34:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 00:10:45 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPClient.hpp"

HTTPClient::HTTPClient(int fd, ListeningSocket& sock)
	: InternetSocket(fd), _M_iterator(), m_socket(sock) 
{
	setBlockingMode(false);
}

HTTPClient::HTTPClient(int fd, const struct sockaddr_in& sockaddr, socklen_t slen, ListeningSocket& sock)
	: InternetSocket(fd, sockaddr, slen), _M_iterator(), m_socket(sock)
{
	setBlockingMode(false);
}

ListeningSocket&	HTTPClient::getBindedSocket()
{
	return (m_socket);
}

void	HTTPClient::setIterator(const std::list<HTTPClient>::iterator& it)
{
	_M_iterator = it;
}

const std::list<HTTPClient>::iterator&	HTTPClient::getIterator() const
{
	return (_M_iterator);
}

HTTPClient::~HTTPClient()
{}
