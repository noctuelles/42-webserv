/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 18:54:18 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/02 21:35:53 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_CLASS_HPP
# define WEBSERV_CLASS_HPP

# include <vector>
# include <algorithm>
# include <assert.h>

# include "HTTP.hpp"
# include "ListeningSocket.hpp"
# include "EPoll.hpp"
# include "HTTP.hpp"
# include "FileUtils.hpp"

namespace ft
{
	class WebServ
	{
		public:

			typedef std::vector<InternetSocket*>	InSockVector;
			typedef std::vector<http::StatusInfo>	StatusInfoVector;

			static const size_t			MaxErrorPageSize     = 2097152; // 2MB
			static const unsigned int	MaxPendingConnection = 5;
			static const std::string	Version;

			WebServ();
			~WebServ();

			void		setStatusCodePage(http::StatusCode, const char* filename);
			const char*	getHTTPVersion() const;

			void	addListener(in_port_t port);
			void	addClient(int fd);
			void	removeSocket(int fd);

			void		run();

		private:


			WebServ(const WebServ& other);
			WebServ&	operator=(const WebServ& rhs);

			EPoll								m_poller;

			// We can't use auto_ptr as a container element type.
			InSockVector		m_socks;
			StatusInfoVector	m_status_table;

			bool	m_listener_init;
	};
}

#endif
