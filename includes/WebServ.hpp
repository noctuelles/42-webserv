/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 18:54:18 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 19:42:11 by plouvel          ###   ########.fr       */
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

			struct Logger
			{
				static void	reason(const char* cause, const char* reason);
			};

			static const size_t			MaxErrorPageSize = 2097152; // 1MB
			static const unsigned int	MaxPendingConnection = 5;

			WebServ();
			~WebServ();

			void	addListener(in_port_t port);
			void	initListener();
			void	loadErrorPage(http::StatusCode errcode, const char *filename);
			const char*	getErrorPage(http::StatusCode errcode);
			void	removeListener(int fd);

			EPoll&	getPoller();

			bool	loop();

		private:

			WebServ(const WebServ& other);
			WebServ&	operator=(const WebServ& rhs);

			EPoll							m_poller;
			std::vector<ListeningSocket>	m_socks;
			std::vector<io::FileContent>	m_custom_err;

			std::map<http::StatusCode, const char *>			m_errtable;

			bool	m_listener_init;

	};
}

#endif
