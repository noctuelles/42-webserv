/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPClient.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 18:34:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/19 17:05:53 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPClient.hpp"

HTTPClient::HTTPClient()
	: InternetSocket(), _M_buffer(), _M_state(NO_CONN)
{}

void	HTTPClient::init(int fd, bool blocking)
{
	setFd(fd);
	setBlockingMode(blocking);
	setState(FETCHING_DATA);
}

void	HTTPClient::terminate()
{
	close(_M_fd);
	_M_buffer.clear();
	setState(NO_CONN);
}

void	HTTPClient::setState(HTTPState state)
{
	_M_state = state;
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
