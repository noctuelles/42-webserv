/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 18:54:18 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/08 15:45:36 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_CLASS_HPP
# define WEBSERV_CLASS_HPP

# include <functional>
# include <vector>
# include <algorithm>
# include <assert.h>

# include "HTTP.hpp"
# include "EPoll.hpp"
# include "FileUtils.hpp"

namespace ft
{

	class WebServ
	{
		public:

			typedef std::vector<InternetSocket*>	InSockVector;
			typedef std::vector<http::StatusInfo>	StatusInfoVector;

			static const size_t			MaxErrorPageSize		= 2097152; // 2MB
			static const unsigned int	MaxPendingConnection	= 5;
			static const time_t			ConnectionTimeout		= 60; // in seconds.
			static const std::string	Version;

			WebServ();
			~WebServ();

			void	setStatusCodePage(http::StatusCode, const char* filename);
			void	addListener(in_port_t port);
			void	run();

		private:

			static const int	TimeoutCheckOccurence = 1000;

			struct DeleteAndNullifyTimeoutSocket : std::unary_function<InternetSocket*&, void>
			{
				inline void	operator()(InternetSocket*& ptr)
				{
					if (ptr->isTimeout())
					{
						delete ptr;
						ptr = 0;
					}
				}
			};

			struct DeleteObj
			{
				template <class T>
					inline void	operator()(T* ptr)
					{
						delete ptr;
					}
			};

			EPoll				m_poller;
			InSockVector		m_socks;
			StatusInfoVector	m_status_table;

			WebServ(const WebServ& other);
			WebServ&	operator=(const WebServ& rhs);

			inline void	_addClient(int fd);
			inline void	_removeSocket(InternetSocket* ptr);
			inline void	_removeTimeoutSocket();
	};
}

#endif
