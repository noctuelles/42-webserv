/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPClient.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 18:34:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/20 09:25:01 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPClient.hpp"

HTTPClient::HTTPClient(int fd)
	: InternetSocket(fd), _M_buffer(), _M_iterator()
{
	setBlockingMode(false);
}

HTTPClient::HTTPClient(int fd, const struct sockaddr_in& sockaddr, socklen_t slen )
	: InternetSocket(fd, sockaddr, slen), _M_buffer(), _M_iterator()
{
	setBlockingMode(false);
}

void	HTTPClient::setState(HTTPState state)
{
	_M_state = state;
}

void	HTTPClient::appendToBuffer(const char *buffer, size_t bytes)
{
	_M_buffer.append(buffer, bytes);
}

void	HTTPClient::setIterator(const std::list<HTTPClient>::iterator& it)
{
	_M_iterator = it;
}

const std::list<HTTPClient>::iterator&	HTTPClient::getIterator() const
{
	return (_M_iterator);
}

const std::string&	HTTPClient::getBuffer() const
{
	return (_M_buffer);
}

HTTPClient::~HTTPClient()
{}
