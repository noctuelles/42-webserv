/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPClient.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 18:34:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/17 20:57:12 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPClient.hpp"

HTTPClient::HTTPClient()
	: InternetSocket(), _M_buffer()
{}

HTTPClient::HTTPClient(int fd)
	: InternetSocket(), _M_buffer()
{
	setFd(fd);
}

bool	HTTPClient::operator==(const HTTPClient& rhs)
{
	return (getFd() == rhs.getFd());
}

void	HTTPClient::appendToBuffer(const char *buffer, size_t bytes)
{
	_M_buffer.append(buffer, bytes);
}

const std::string&	HTTPClient::getBuffer() const
{
	return (_M_buffer);
}

HTTPClient::~HTTPClient()
{}
