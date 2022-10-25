/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 18:54:18 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/25 19:40:52 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_CLASS_HPP
# define WEBSERV_CLASS_HPP

# include <vector>
# include "ListeningSocket.hpp"
# include "EPoll.hpp"
# include <algorithm>

class WebServ
{
	public:

		static const unsigned int	MaxPendingConnection = 5;

		WebServ();
		~WebServ();

		struct SocketComp
		{
			explicit SocketComp(int fd) : m_fd(fd) {}
			inline bool operator()(const InternetSocket& sock) const
			{
				return (sock.getFd() == m_fd);
			}
			private:
				int m_fd;
		};

		void	addSocket(in_port_t port)
		{
			m_socks.push_back(ListeningSocket(INADDR_ANY, port));
			m_poller.add(m_socks.back().getFd(), EPoll::IN, &m_socks.back());
		}

		void	launch()
		{
			for (std::vector<ListeningSocket>::iterator it = m_socks.begin(); it != m_socks.end(); it++)
				it->listen(MaxPendingConnection);
		}

		bool	isListeningSocket(const InternetSocket* sock)
		{
			return (std::find_if(m_socks.begin(), m_socks.end(), SocketComp(sock->getFd())) != m_socks.end());
		}

		EPoll&	getPoller();

	private:

		WebServ(const WebServ& other);
		WebServ&	operator=(const WebServ& rhs);

		EPoll							m_poller;
		std::vector<ListeningSocket>	m_socks;

};

#endif
