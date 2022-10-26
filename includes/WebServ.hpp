/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 18:54:18 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 15:34:19 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_CLASS_HPP
# define WEBSERV_CLASS_HPP

# include <vector>
# include "ListeningSocket.hpp"
# include "EPoll.hpp"
# include <algorithm>
# include <assert.h>

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

			struct Logger
			{
				static void	reason(const char* cause, const char* reason);
			};

			static const unsigned int	MaxPendingConnection = 5;

			WebServ();
			~WebServ();

			void	addListener(in_port_t port);
			void	initListener();
			void	removeListener(int fd);

			EPoll&	getPoller();

			bool	loop();

		private:

			WebServ(const WebServ& other);
			WebServ&	operator=(const WebServ& rhs);

			EPoll							m_poller;
			std::vector<ListeningSocket>	m_socks;
			bool							m_listener_init;

	};
}

#endif
