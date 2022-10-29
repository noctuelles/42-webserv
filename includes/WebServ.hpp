/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 18:54:18 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/29 19:21:26 by plouvel          ###   ########.fr       */
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

			void				setStatusCodePage(http::StatusCode, const char*);
			static const char*	getStatusCodePage(http::StatusCode);
			static const char*	getStatusCodePhrase(http::StatusCode);

			bool		isMethodAllowed(http::Method) const;

			const char*	getHTTPVersion() const;
			void		removeListener(int fd);
			void		run();

			class Exception
			{
				public:

					Exception(http::StatusCode code)
						: m_phrase(m_status_table[code].first),
						  m_page(m_status_table[code].second)
					{}

					const char*	getPhrase() const throw()
					{
						return (m_phrase);
					}

					const char*	getPage() const throw()
					{
						return (m_page);
					}

				private:

					const char*	m_phrase;
					const char*	m_page;
			};

		private:

			static std::pair<const char*, const char*>	m_status_table[http::MaxStatusCode];

			WebServ(const WebServ& other);
			WebServ&	operator=(const WebServ& rhs);

			// Poller instance (we're using EPoll).
			EPoll								m_poller;
			// Vector to store all the listening sockets.
			std::vector<ListeningSocket>		m_socks;

			// Vector to store the custom status pages defined by the user.
			std::vector<io::FileContent>		m_custom_status_page;
			const bool							m_forbidden_method[http::NbrAvailableMethod];

			bool	m_listener_init;
	};
}

#endif
