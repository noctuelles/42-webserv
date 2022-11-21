/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileDescriptor.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 08:13:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/21 17:36:19 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileDescriptor.hpp"
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

namespace IO
{
	FileDescriptor::FileDescriptor(int fd)
		: m_fd(fd), m_should_close(true)
	{}

	FileDescriptor::FileDescriptor(const FileDescriptor& other)
		: m_fd(other.m_fd), m_should_close(true)
	{
		other.m_should_close = false;
	}

	FileDescriptor&	FileDescriptor::operator=(const FileDescriptor& rhs)
	{
		if (this == &rhs)
			return (*this);
		m_fd = rhs.m_fd;
		m_should_close = true;
		rhs.m_should_close = false;
		return (*this);
	}

	FileDescriptor::operator int() const
	{
		return (m_fd);
	}

	FileDescriptor::~FileDescriptor()
	{
		if (m_should_close)
			close(m_fd);
	}

	int	FileDescriptor::getFd() const
	{
		return (m_fd);
	}

	int	FileDescriptor::getFdFlags() const
	{
		int ret;

		if ((ret = fcntl(m_fd, F_GETFL)) < 0)
			throw (std::runtime_error("fcntl(F_GETFL)"));
		return (ret);
	}

	void	FileDescriptor::setFdFlags(int flags) const
	{
		if (fcntl(m_fd, F_SETFL, flags) < 0)
			throw (std::runtime_error("fcntl(F_SETFL)"));
	}
}
