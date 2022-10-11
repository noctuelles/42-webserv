/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 19:21:09 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/11 19:28:13 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientSocket.hpp"

ClientSocket::ClientSocket()
	: SimpleSocket()
{}

ClientSocket::~ClientSocket()
{}

void	ClientSocket::setFd(int fd)
{
	_M_fd = fd;
}
