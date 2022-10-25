/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileDescriptor.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 08:13:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/20 09:31:19 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileDescriptor.hpp"
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

FileDescriptor::FileDescriptor(int fd)
	: _M_fd(fd), _M_should_close(true)
{}

FileDescriptor::FileDescriptor(const FileDescriptor& other)
	: _M_fd(other._M_fd), _M_should_close(true)
{
	other._M_should_close = false;
}

FileDescriptor::~FileDescriptor()
{
	if (_M_should_close)
		close(_M_fd);
}

int	FileDescriptor::getFd() const
{
	return (_M_fd);
}

int	FileDescriptor::getFdFlags() const
{
	int ret;

	if ((ret = fcntl(_M_fd, F_GETFL)) < 0)
		throw (std::runtime_error("fcntl(F_GETFL)"));
	return (ret);
}

void	FileDescriptor::setFdFlags(int flags) const
{
	if (fcntl(_M_fd, F_SETFL, flags) < 0)
		throw (std::runtime_error("fcntl(F_SETFL)"));
}
