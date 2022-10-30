/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 18:54:18 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/30 20:37:28 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_CLASS_HPP
# define WEBSERV_CLASS_HPP

# include <vector>
# include <algorithm>
# include <assert.h>

# include "ListeningSocket.hpp"
# include "EPoll.hpp"
# include "HTTP.hpp"
# include "FileUtils.hpp"

namespace ft
{
	class WebServ
	{
		private:

			struct ListeningSocketInit
			{
				explicit ListeningSocketInit(EPoll& poller)
					: poll(poller) {}

				inline void operator()(ListeningSocket& sock)
				{
					sock.listen(MaxPendingConnection);
					poll.add(sock.getFd(), EPoll::IN, &sock);
				}

				EPoll& poll;
			};

			class SocketComp
			{
				public:
					explicit SocketComp(int fd) : m_fd(fd) {}
					~SocketComp() {}
					inline bool operator()(const InternetSocket& sock) const {return (sock.getFd() == m_fd);}
				private:
					int m_fd;
			};

		public:

			static const size_t			MaxErrorPageSize     = 2097152; // 2MB
			static const unsigned int	MaxPendingConnection = 5;

			WebServ();
			~WebServ();

			void	addListener(in_port_t port);
			void	initListener();

			void				setStatusCodePage(http::StatusCode, const char* filename);
			const http::StatusInfo&	getStatusCodeInfo(http::StatusCode statusCode) const;
			const char*	getHTTPVersion() const;

			void		removeListener(int fd);
			void		run();

		private:


			WebServ(const WebServ& other);
			WebServ&	operator=(const WebServ& rhs);

			// Poller instance (we're using EPoll).
			EPoll								m_poller;
			// Vector to store all the listening sockets.
			std::vector<ListeningSocket>		m_socks;
			std::vector<http::StatusInfo>		m_status_table;


			bool	m_listener_init;
	};
}

#endif
