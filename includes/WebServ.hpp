/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 18:54:18 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/02 16:43:54 by plouvel          ###   ########.fr       */
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
# include "UniquePtr.hpp"

namespace ft
{
	class WebServ
	{
		public:

			static const size_t			MaxErrorPageSize     = 2097152; // 2MB
			static const unsigned int	MaxPendingConnection = 5;
			static const std::string	Version;

			WebServ();
			~WebServ();

			void	setStatusCodePage(http::StatusCode, const char* filename);
			const http::StatusInfo&	getStatusCodeInfo(http::StatusCode statusCode) const;
			const char*	getHTTPVersion() const;

			void	addListener(in_port_t port);
			void	addClient(int fd);
			void	removeSocket(int fd);

			void		run();

		private:


			WebServ(const WebServ& other);
			WebServ&	operator=(const WebServ& rhs);

			EPoll								m_poller;

			std::vector<unique_ptr<InternetSocket> >	m_socks;
			std::vector<http::StatusInfo>				m_status_table;

			bool	m_listener_init;
	};
}

#endif
