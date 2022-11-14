/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionSocket.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 14:23:54 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/14 16:35:19 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTIONSOCKET_HPP
# define CONNECTIONSOCKET_HPP 

# include <cstring>
# include <exception>
# include <vector>
# include <list>
# include <fstream>
# include "SocketTypes.hpp"

namespace ft
{
	class ConnectionSocket : public InternetSocket
	{

		public:

			static const size_t		MaxRecvBufferSize = 1024 * 8;
			static const size_t		MaxSendBufferSize = 1024 * 8;

			enum	State
			{
				FETCHING_REQUEST_HEADER = -99,
				FETCHING_REQUEST_BODY,
				SENDING_RESPONSE_HEADER,
				SENDING_RESPONSE_BODY,
				DISCONNECT
			};

			explicit ConnectionSocket(int fd);
			virtual ~ConnectionSocket();

			virtual int		recv();
			virtual int		send();

			bool			isTimeout();

		private:

			ConnectionSocket(const ConnectionSocket& other);
			ConnectionSocket&	operator=(const ConnectionSocket& rhs);

			time_t	m_last_activity;

			inline void	_updateLastActivity() {m_last_activity = time(NULL);};

		protected:

			State					m_state;
			struct sockaddr_in		m_bounded_sockaddr;
			std::vector<uint8_t>	m_recv_buff, m_send_buff;
			ssize_t					m_recv_bytes, m_send_bytes;
	};
}

#endif
