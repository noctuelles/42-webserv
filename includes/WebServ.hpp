/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 18:54:18 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 00:46:12 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_CLASS_HPP
# define WEBSERV_CLASS_HPP

# include <vector>
# include "ListeningSocket.hpp"
# include "EPoll.hpp"
# include <algorithm>
# include <assert.h>

class WebServ
{
	private:

		struct ListeningSocketInit
		{
			explicit ListeningSocketInit(EPoll& poller)
				: poll(poller) {}

			inline void operator()(ListeningSocket& sock)
			{
				poll.add(sock.getFd(), EPoll::IN, &sock);
				sock.listen(MaxPendingConnection);
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

		static const unsigned int	MaxPendingConnection = 5;

		WebServ();
		~WebServ();

		void	addListener(in_port_t port)
		{
			m_socks.push_back(ListeningSocket(INADDR_ANY, port));
		}

		void	initListener()
		{
			std::for_each(m_socks.begin(), m_socks.end(), ListeningSocketInit(m_poller));
			m_listener_init = true;
		}

		bool	isAListener(const InternetSocket* sock)
		{
			return (std::find_if(m_socks.begin(), m_socks.end(), SocketComp(sock->getFd())) != m_socks.end());
		}

		EPoll&	getPoller()
		{
			return (m_poller);
		}

		bool	loop()
		{
			assert(m_listener_init);
			m_poller.waitForEvents(EPoll::NOTIMEOUT);
			return (true);
		}

	private:

		WebServ(const WebServ& other);
		WebServ&	operator=(const WebServ& rhs);

		EPoll							m_poller;
		std::vector<ListeningSocket>	m_socks;
		bool							m_listener_init;

};

#endif
