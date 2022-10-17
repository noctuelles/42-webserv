/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPClient.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 18:34:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/17 20:38:40 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPClient.hpp"

HTTPClient::HTTPClient()
	: InternetSocket()
{}

HTTPClient::HTTPClient(int fd)
	: InternetSocket()
{
	setFd(fd);
}

bool	HTTPClient::operator==(const HTTPClient& rhs)
{
	return (getFd() == rhs.getFd());
}

HTTPClient::~HTTPClient()
{}
