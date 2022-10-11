/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SimpleSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 18:40:23 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/11 19:42:37 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIMPLESOCKET_HPP
# define SIMPLESOCKET_HPP

# include <arpa/inet.h>
# include <unistd.h>
# include <stdexcept>

template <class T>
	class SimpleSocket
	{
		public:

			SimpleSocket()
				: _M_fd(-1), _M_sockaddr(), _M_len()
			{}

			SimpleSocket(int domain, int type, int protocol)
			{
				if ((_M_fd = ::socket(domain, type, protocol)) == -1)
					throw (std::runtime_error("cannot create socket"));
			}

			virtual ~SimpleSocket()
			{
				if (_M_fd != -1)
					close(_M_fd);
			}

			T*	getSocketAddrPointer()
			{
				return (&_M_sockaddr);
			}

			socklen_t*	getSocketLenPointer()
			{
				return (&_M_len);
			}

			int	getFd() const
			{
				return (_M_fd);
			}

		protected:

			int			_M_fd;
			T			_M_sockaddr;
			socklen_t	_M_len;

		private:

			SimpleSocket(const SimpleSocket& other);
			SimpleSocket&	operator=(const SimpleSocket& rhs);
	};

#endif
