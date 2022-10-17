/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SimpleSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 18:40:23 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/17 19:40:10 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIMPLESOCKET_HPP
# define SIMPLESOCKET_HPP

# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>
# include <stdexcept>

template <class T>
	class SimpleSocket
	{
		public:

			SimpleSocket()
				: _M_fd(), _M_sockaddr(), _M_len(), _M_should_close(true)
			{}

			SimpleSocket(int domain, int type, int protocol)
				: _M_fd(), _M_sockaddr(), _M_len(), _M_should_close(true)
			{
				if ((_M_fd = ::socket(domain, type, protocol)) == -1)
					throw (std::runtime_error("socket"));
			}

			virtual ~SimpleSocket()
			{
				if (_M_should_close)
					close(_M_fd);
			}

			T&	getSockAddr()
			{
				return (_M_sockaddr);
			}

			socklen_t&	getSockLen()
			{
				return (_M_len);
			}

			void	setFd(int fd)
			{
				_M_fd = fd;
			}

			int	getFd() const
			{
				return (_M_fd);
			}

			void	setSockOpt(int optname, void *optval, socklen_t optlen) const
			{
				if (setsockopt(_M_fd, SOL_SOCKET, optname, optval, optlen) < 0)
					throw (std::runtime_error("setsockopt"));
			}

			void	getSockOpt(int optname, void* optval, socklen_t* optlen) const
			{
				if (getsockopt(_M_fd, SOL_SOCKET, optname, optval, optlen) < 0)
					throw (std::runtime_error("getsockopt"));
			}

			int	getFdFlags() const
			{
				int ret;

				if ((ret = fcntl(_M_fd, F_GETFL)) < 0)
					throw (std::runtime_error("fcntl(F_GETFL)"));
				return (ret);
			}

			void	setFdFlags(int fl) const
			{
				if (fcntl(_M_fd, F_SETFL, fl) < 0)
					throw (std::runtime_error("fcntl(F_SETFL)"));
			}

			/* These functions are shortcut function that avoid you to use setSockOpt or setFdFlags for common socket operations. */

			void	setReusableMode(bool	reusable) const
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

			void	shouldBeClose(bool should_close)
			{
				_M_should_close = should_close;
			}

		protected:

			int			_M_fd;
			T			_M_sockaddr;
			socklen_t	_M_len;

		private:

			SimpleSocket&	operator=(const SimpleSocket& rhs);

			bool	_M_should_close;
	};

#endif
