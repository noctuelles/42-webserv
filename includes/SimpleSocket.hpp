/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SimpleSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 18:40:23 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 14:28:47 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIMPLESOCKET_HPP
# define SIMPLESOCKET_HPP

# include "FileDescriptor.hpp"
# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>
# include <stdexcept>

template <class T>
	class SimpleSocket : public FileDescriptor
	{
		public:

			/* Create a SimpleSocket from connect() or accept() system call. */
			SimpleSocket(int fd)
				: FileDescriptor(fd), m_sockaddr(), m_len()
			{}

			/* Create a SimpleSocket from connect() or accept() system call. */
			SimpleSocket(int fd, const T& sockaddr, socklen_t len)
				: FileDescriptor(fd), m_sockaddr(sockaddr), m_len(len)
			{}

			// Allowing copy-construction with this RAII class, but :
			// The SimpleSocket "other" transfer his ressource to the current instance and will not be
			// released during destruction.
			SimpleSocket(const SimpleSocket& other)
				: FileDescriptor(other), m_sockaddr(other.m_sockaddr), m_len(other.m_len)
			{}

			SimpleSocket&	operator=(const SimpleSocket& rhs)
			{
				if (this == &rhs)
					return (*this);
				FileDescriptor::operator=(rhs);
				m_sockaddr = rhs.m_sockaddr;
				m_len = rhs.m_len;
				return (*this);
			}

			/* Create a SimpleSocket from the socket() system call. */
			SimpleSocket(int domain, int type, int protocol)
				: FileDescriptor(0), m_sockaddr(), m_len()
			{
				if ((m_fd = ::socket(domain, type, protocol)) == -1)
					throw (std::runtime_error("socket"));
			}

			virtual ~SimpleSocket()
			{}

			const T&	getSockAddr()
			{
				return (m_sockaddr);
			}

			socklen_t	getSockLen() const
			{
				return (m_len);
			}

			void	setSockAddr(const T& sockaddr)
			{
				m_sockaddr = sockaddr;
			}

			void	setSockLen(socklen_t socklen)
			{
				m_len = socklen;
			}

			void	setSockOpt(int optname, void *optval, socklen_t optlen) const
			{
				if (setsockopt(this->m_fd, SOL_SOCKET, optname, optval, optlen) < 0)
					throw (std::runtime_error("setsockopt"));
			}

			void	getSockOpt(int optname, void* optval, socklen_t* optlen) const
			{
				if (getsockopt(this->m_fd, SOL_SOCKET, optname, optval, optlen) < 0)
					throw (std::runtime_error("getsockopt"));
			}

			/* These functions are shortcut function that avoid you to use setSockOpt or setFdFlags for common socket operations. */

			void	setReusableMode(bool reusable) const
			{
				int	optval = (reusable) ? 1 : 0;
				setSockOpt(SO_REUSEADDR, static_cast<void *>(&optval), sizeof(int));
			}

			/* Set the current socket to a blocking socket if <blocking> is true.
			 * If <blocking> is false, set the current socket to a non-blocking state. */
			void	setBlockingMode(bool blocking) const
			{
				setFdFlags(blocking ? (getFdFlags() & ~O_NONBLOCK) : (getFdFlags() | O_NONBLOCK));
			}

			bool	isBlocking() const
			{
				return !(static_cast<bool>(getFdFlags() & O_NONBLOCK));
			}

		protected:

			T			m_sockaddr;
			socklen_t	m_len;
	};

#endif
