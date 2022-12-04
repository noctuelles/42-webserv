/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileDescriptor.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 08:13:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/12/04 16:34:34 by plouvel          ###   ########.fr       */
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
		: m_fd(fd)
	{}

	FileDescriptor::FileDescriptor(const FileDescriptor& other)
		: m_fd(other.m_fd)
	{
		other.m_fd = -1;
	}

	FileDescriptor&	FileDescriptor::operator=(const FileDescriptor& rhs)
	{
		if (this == &rhs)
			return (*this);
		m_fd = rhs.m_fd;
		rhs.m_fd = -1;
		return (*this);
	}

	FileDescriptor::operator int() const
	{
		return (m_fd);
	}

	FileDescriptor::~FileDescriptor()
	{
		release();
	}

	void FileDescriptor::release() const
	{
		if (m_fd != -1)
		{
			close(m_fd);
			m_fd = -1;
		}
	}

	int	FileDescriptor::get() const
	{
		return (m_fd);
	}

	int	FileDescriptor::getFlags() const
	{
		int ret;

		if ((ret = fcntl(m_fd, F_GETFL)) < 0)
			throw (std::runtime_error("fcntl(F_GETFL)"));
		return (ret);
	}

	void	FileDescriptor::setFlags(int flags) const
	{
		if (fcntl(m_fd, F_SETFL, flags) < 0)
			throw (std::runtime_error("fcntl(F_SETFL)"));
	}

	/* Set the current socket to a blocking socket if <blocking> is true.
	 * If <blocking> is false, set the current socket to a non-blocking state. */
	void	FileDescriptor::setBlockingMode(bool blocking) const
	{
		setFlags(blocking ? (getFlags() & ~O_NONBLOCK) : (getFlags() | O_NONBLOCK));
	}

	bool	FileDescriptor::isBlocking() const
	{
		return !(static_cast<bool>(getFlags() & O_NONBLOCK));
	}

	void	FileDescriptor::setCloseOnExecMode(bool blocking) const
	{
		setFlags(blocking ? (getFlags() & ~O_CLOEXEC) : (getFlags() | O_CLOEXEC));
	}
}
