/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionSocket.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 14:23:54 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/20 20:53:50 by plouvel          ###   ########.fr       */
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
# include "RequestHandler.hpp"


namespace ft
{
	using http::RequestHandler;

	class ConnectionSocket : public InternetSocket
	{
		public:

			typedef std::vector<uint8_t>	DataBuffer;

			static const size_t		MaxRecvBufferSize = 1024 * 8;
			static const size_t		MaxSendBufferSize = 1024 * 16;

			enum	State
			{
				READING = -0xFF,
				WRITING,
				DISCONNECT
			};

			ConnectionSocket(int fd, const VirtServInfo::VirtServMap& virt_serv_map);
			const struct sockaddr_in&	getPeerSock() const;
			virtual ~ConnectionSocket();

			virtual int		recv();
			virtual int		send();
			virtual bool	isTimeout();

		private:

			ConnectionSocket(const ConnectionSocket& other);
			ConnectionSocket&	operator=(const ConnectionSocket& rhs);

			time_t			m_last_activity;
			RequestHandler	m_request_handler; // has-a

			State				m_state;
			struct sockaddr_in	m_peer_sockaddr;
			DataBuffer			m_recv_buff;
			ssize_t				m_recv_bytes;
			size_t				m_sent_bytes;

			inline void	_updateLastActivity() {m_last_activity = time(NULL);};
	};
}

#endif
